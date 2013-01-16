#ifndef __MY_ASYNC_HTTP__
#define __MY_ASYNC_HTTP__

#include "MyTaskThread.h"
#include "MyDebug.h"
#include "MyString.h"
#include "MyTempFile.h"
#include "MyThreadPool.h"
#include "Common.h"
#include "wininet.h"
#include "zlib.h"


#define MAX_BUF_SIZE (1000+1)
#define MAX_READ_SIZE (1000)
#define MAX_WAIT_TIME (20000)

void __stdcall Callback(HINTERNET hInternet,
              DWORD dwContext,
              DWORD dwInternetStatus,
              LPVOID lpStatusInfo,
              DWORD dwStatusInfoLen);
class CMyAsyncHttp  
{
public:
	enum HTTP_STATE
	{
		HTTP_START,			//正在请求中
		HTTP_FINISH,		//请求完毕，并且成功完成请求
		HTTP_STOP,			//请求异常停止，可能为用户停止，或者内部异常
		HTTP_TIMEOUT        //请求超时
	};

	typedef struct _HttpData
	{
		HINTERNET	iSession;      //一个Http的会话句柄
		HANDLE		iNotifyEvent;  //事件通知
	}HttpData;

	typedef struct _HttpContext
	{
		HANDLE*		iNotifyEvent;
		HINTERNET*	iConnect;
		HINTERNET*	iHttp;
		int			iMark;
		DWORD		iAsyncResult;
		CMyString*	iPath;   //for debug
	}HttpContext;

	class CSetCallBackThread :public CMyThread
	{
	public:
		virtual int Run(void *param)
		{
			HINTERNET session=(HINTERNET)param;
			return (int)InternetSetStatusCallback(session,(INTERNET_STATUS_CALLBACK)Callback);
		}
	};

private:

	typedef struct _ExtraData   //用于post数据用
	{
		unsigned long	iExtraDataLen;
		void*			iExtraData;
	}ExtraData;

	typedef struct _TaskArg
	{
		CMyString*			iUrl;
//		CMyString*			iPath;
		CMyString*			iVerb;
		CMyString*			iUserName;
		CMyString*			iPassword;
		CMyString*			iRefer;
		CMyString*			iHttpHeader;
		CMyString*			iAcceptType;
		
		int*				iStatusCode;
		CMyString*			iCharset;
		CMyString*			iEncode;

		ExtraData*			iExtraData;	
		CMyTempFile*		iIOStream;
		int*				iStreamLen;
		
		HINTERNET*			iSession;

		HANDLE*				iNotifyEvent;     //回调通知事件
		HINTERNET*			iConnect;
		HINTERNET*			iHttp;

		int					iMark;            //内部保留使用
		DWORD				iAsyncResult;	  //记录回调错误状态

		CMyString*			iPath;
		
		HTTP_STATE*			iHttpState;

		HANDLE*				iTerminateEvent;  //强制结束任务序列事件 
		HANDLE*				iFinishNotify;

		CMyTaskThread**		iTaskRunTarget1;  //task运行宿主线程
		CMyThreadPool**		iTaskRunTarget2;
		CMyTask*			iTaskList[3];	 //task列表
		//是否是指定的TaskThread，如果是不是指定的话表示内部生成，需要自己结束TaskThread;
		bool*				iAssignTaskThread;  

		unsigned long*		iTimeOut;
	}TaskArg;

	class HttpTask:public CMyTask
	{
	public:
		HttpTask()
		{
			m_NextTask	=NULL;
		}

		virtual	int	Excute(void *threadArg,void* taskArg)
		{
			m_TaskArg=(TaskArg*)taskArg;
			m_TimeOut=false;
			return 1;
		}

		int WaitEvent()
		{
			HANDLE pEvent[2];
			pEvent[0]=*m_TaskArg->iNotifyEvent;
			pEvent[1]=*m_TaskArg->iTerminateEvent;
			int r= WaitForMultipleObjects(2,pEvent,false,*m_TaskArg->iTimeOut);

			if(r==WAIT_TIMEOUT)
				m_TimeOut=true;
			return r;
		}

		void DoNextTask()
		{
			CMyTask*	task=GetNextTask();
			if(task&&(*m_TaskArg->iTaskRunTarget1))
				(*m_TaskArg->iTaskRunTarget1)->AssignTask(*task,m_TaskArg);
			else if(task&&(*m_TaskArg->iTaskRunTarget2))
				(*m_TaskArg->iTaskRunTarget2)->AssignTask(*task,m_TaskArg);
		}

		//出现异常情况,包括用户强制停止任务
		void DoException(HTTP_STATE httpState=HTTP_STOP)
		{
			//如果线程是http自己创建的则通知线程执行完task后退出
			CMyString url=*m_TaskArg->iUrl;
			url+=*m_TaskArg->iPath;

			if(!(*m_TaskArg->iAssignTaskThread))
				SetThreadOutWhenTaskFinish(true);
			//释放相关资源
			*m_TaskArg->iUserName	="";
			*m_TaskArg->iPassword	="";
			*m_TaskArg->iRefer		="";
			*m_TaskArg->iHttpHeader	="";
			*m_TaskArg->iAcceptType	="";

			if(*m_TaskArg->iConnect)
				::InternetCloseHandle(*m_TaskArg->iConnect);
			if(*m_TaskArg->iHttp)
				::InternetCloseHandle(*m_TaskArg->iHttp);
			//修改状态，通知处理完毕

			if(m_TimeOut)
			{
				LOG("HttpTask","Url:%s  TimeOut",url.GetBuffer());
				httpState =HTTP_TIMEOUT;
			}

			GetMutex(CMyAsyncHttp::m_Mutex);
			*(m_TaskArg->iHttpState)	=httpState;
			ReleaseMutex(CMyAsyncHttp::m_Mutex);
			
			SetEvent(*m_TaskArg->iFinishNotify);
			
			if(httpState==HTTP_STOP)
				LOG("HttpTask","Url:%s  have a exception",url.GetBuffer());
		}
	private:
		virtual CMyTask* GetNextTask(){return NULL;}

	protected:
		CMyTask*	m_NextTask;
		TaskArg*	m_TaskArg;
		bool		m_TimeOut;
	};

	class ConnectHttpTask:public HttpTask
	{
	public:
		virtual	int	Excute(void *threadArg,void* taskArg)
		{
			HttpTask::Excute(threadArg,taskArg);
			CMyString url=*m_TaskArg->iUrl;
			url+=*m_TaskArg->iPath;
			LOG("ConnectHttpTask","Connect to Url:%s",url.GetBuffer());
			do
			{
				m_TaskArg->iMark	=1;
				*m_TaskArg->iConnect=
					::InternetConnect(*m_TaskArg->iSession,
									m_TaskArg->iUrl->GetBuffer(),
									INTERNET_INVALID_PORT_NUMBER,
									m_TaskArg->iUserName->GetBuffer(),
									m_TaskArg->iPassword->GetBuffer(),
									INTERNET_SERVICE_HTTP,
									0,(DWORD)(&m_TaskArg->iNotifyEvent));


				if(!(*m_TaskArg->iConnect))
				{
					if(GetLastError() != ERROR_IO_PENDING)
					{
						LOG("ConnectHttpTask","Url:%s connect have a execption!",
							url.GetBuffer());
						break;
					}
				}
				if(WaitEvent()!=0)break;
				if(!*m_TaskArg->iConnect)
				{
					LOG("ConnectHttpTask","Url:%s connect failed!",
						url.GetBuffer());
					break;
				}
				DoNextTask();
				return 1;
			}while(0);
			DoException();
			return -1;
		}

	private:
		virtual CMyTask* GetNextTask()
		{
			return m_TaskArg->iTaskList[0];
		}
	};

	class OpenRequestTask:public HttpTask
	{
	public:
		virtual	int	Excute(void *threadArg,void* taskArg)
		{
			HttpTask::Excute(threadArg,taskArg);
			
			CMyString url=*m_TaskArg->iUrl;
			url+=*m_TaskArg->iPath;

			LOG("OpenRequestTask","Rquest Url:%s",url.GetBuffer());
			char *accept[2]={NULL,NULL};
			accept[0]=m_TaskArg->iAcceptType->GetBuffer();
			m_TaskArg->iMark	=2;
			*m_TaskArg->iHttp	=
				::HttpOpenRequest(*m_TaskArg->iConnect,
								  m_TaskArg->iVerb->GetBuffer(),
								  (m_TaskArg->iPath->GetStrLen()?m_TaskArg->iPath->GetBuffer():NULL),
							      NULL,
								  (m_TaskArg->iRefer->GetStrLen()?m_TaskArg->iRefer->GetBuffer():NULL),
						          (m_TaskArg->iAcceptType->GetStrLen()?(const char**)accept:NULL),
								  INTERNET_FLAG_DONT_CACHE,
								  (DWORD)(&m_TaskArg->iNotifyEvent));
			do
			{
				if(*m_TaskArg->iHttp==NULL)
				{
					if(GetLastError() != ERROR_IO_PENDING)
					{
						LOG("OpenRequestTask","Url:%s connect have a execption!",
							url.GetBuffer());
						break;
					}
				}
				if(0!=WaitEvent())break;
				if(!*m_TaskArg->iHttp)
				{
					LOG("OpenRequestTask","Url:%s request failed!",
							url.GetBuffer());
					break;
				}

				if(m_TaskArg->iHttpHeader->GetStrLen())
				{
					int r;
					r=::HttpAddRequestHeaders(*m_TaskArg->iHttp, 
											  m_TaskArg->iHttpHeader->GetBuffer(), 
											  m_TaskArg->iHttpHeader->GetStrLen(),
											  HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD);
				}
				DoNextTask();
				return 1;
			}while(0);

			DoException();
			return -1;
		}

	private:
		virtual CMyTask* GetNextTask()
		{
			return m_TaskArg->iTaskList[1];
		}
	};

	class SendRequestTask:public HttpTask
	{
	public:
		virtual	int	Excute(void *threadArg,void* taskArg)
		{
			HttpTask::Excute(threadArg,taskArg);
			CMyString url=*m_TaskArg->iUrl;
			url+=*m_TaskArg->iPath;
			LOG("SendRequestTask","Send data to Url:%s",url.GetBuffer());	
			do
			{
				if(!::HttpSendRequest(*m_TaskArg->iHttp,NULL,0,m_TaskArg->iExtraData->iExtraData
					,m_TaskArg->iExtraData->iExtraDataLen))
				{
					if (GetLastError() != ERROR_IO_PENDING)
					{
						LOG("SendRequestTask","Url:%s send direct failed!",
							url.GetBuffer());
						break;
					 }
				}
			
				if(0!=WaitEvent())break;
				if(ERROR_SUCCESS!= m_TaskArg->iAsyncResult)
				{
					break;
				}
				DoNextTask();
				return 1;
			}while(0);
			DoException();
			return -1;
		}

	private:
		virtual CMyTask* GetNextTask()
		{
			return m_TaskArg->iTaskList[2];
		}
	};

	class ReadResponseTask:public HttpTask
	{
	private:
		//检测是否外部有命令停止任务
		bool CheckOutCmd()
		{
			if(0==WaitForSingleObject(*m_TaskArg->iTerminateEvent,0))return true;
			return false;
		}
		//判断一个字节中高位连续1的个数
		int GetByteFirstOneCount(char c)
		{
			for(int i=0;i<8;i++)
			{
				if(((c>>(8-i-1))&1)==0)break;
			}
			return i;
		}
		//根据最后一个UTF-8字节判断需要保存截断的UTF-8字节数
		//U-00000000 - U-0000007F: 0xxxxxxx  
		//U-00000080 - U-000007FF: 110xxxxx 10xxxxxx  
		//U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx  
		//U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
		//U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
		//U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
		int	GetRestByte(char *p)
		{
			int i=GetByteFirstOneCount(*p);
			if(i>=2&&i<=6)
				return 1;
			else if(i==1)
			{
				int count=2;
				int a;
				bool error=false;
				while(1)
				{
					a=GetByteFirstOneCount(*(--p));
					if(a>1)
					{
						break;
					}
					else if(a==0)
					{
						error=true;
						break;
					}
					count++;
					if(count>5)
					{
						error=true;
						break;
					}
				}
				if(error)
				{
					CMyDebug::Log("UTF-8",CMyDebug::MESSAGE_BOX,0,"the charset is not UTF-8");
				}
				else
				{
					if(count==a)
						count= 0;
					return count;
				}
			}
			else if(i==0)
			{
				return 0;
			}
			else
			{
				CMyDebug::Log("UTF-8",CMyDebug::MESSAGE_BOX,0,"the charset is not UTF-8");
			}
			return -1;
		}

	public:
		virtual	int	Excute(void *threadArg,void* taskArg)
		{
			HttpTask::Excute(threadArg,taskArg);
			unsigned long rl;
			INTERNET_BUFFERS internetBuf;
			char buf[MAX_BUF_SIZE+6];  //前六个字节用于UTF-8转换用
			int  less=0;  
			char assicBuf[MAX_BUF_SIZE+12];
			bool isUnicode=false;
			CMyString url=*m_TaskArg->iUrl;
			url+=*m_TaskArg->iPath;
			

			*m_TaskArg->iStreamLen	=0;

			LOG("ReadResponseTask","Read data from Url:%s",url.GetBuffer());	
			CMyAsyncHttp::HttpGetCharset(*m_TaskArg->iHttp,*m_TaskArg->iCharset);
			CMyAsyncHttp::HttpGetEncode(*m_TaskArg->iHttp,*m_TaskArg->iEncode);
			CMyAsyncHttp::HttpGetStatusCode(*m_TaskArg->iHttp,*m_TaskArg->iStatusCode);
			if(m_TaskArg->iCharset->GetStrLen())
			{
				if(m_TaskArg->iCharset->CompareI("utf-8"))
					isUnicode=true;
//				else
//					CMyDebug::Log("ReadResponseTask",CMyDebug::MESSAGE_BOX,0,"unknow charset:%s",m_TaskArg->iCharset->GetBuffer());
			}
			while(!m_TaskArg->iIOStream->IsUsable())m_TaskArg->iIOStream->ReCreate();
			m_TaskArg->iIOStream->ReleaseMapping();
			m_TaskArg->iIOStream->Seek(CMyFile::SEEK_START,0);

			if(!m_TaskArg->iEncode->GetStrLen())
			{
			//数据没有压缩
				while(1)
				{
					memset(&internetBuf,0,sizeof(INTERNET_BUFFERS));
					internetBuf.lpvBuffer=&buf[6];
					internetBuf.dwStructSize=sizeof(internetBuf);
					internetBuf.dwBufferLength=MAX_BUF_SIZE;

					if(!::InternetReadFileEx(*m_TaskArg->iHttp,&internetBuf,IRF_ASYNC,(DWORD)(&m_TaskArg->iNotifyEvent)))
					{
						if (GetLastError() != ERROR_IO_PENDING)
						{
							LOG("ReadResponseTask","Url:%s read data failed!",
							url.GetBuffer());
							break;
						}
						if(0!=WaitEvent())break;
					}

					if(!internetBuf.dwBufferLength)
					{
						m_TaskArg->iIOStream->Flush();
						DoException(HTTP_FINISH);
						LOG("ReadResponseTask","Url:%s success finish!",url.GetBuffer());
						return 1;
					}
					if(isUnicode)
					{
						int writeLen;
						writeLen=CMyAsyncHttp::FromUnicodeToAssic(&buf[6-less],internetBuf.dwBufferLength+less,assicBuf,sizeof(assicBuf));
						assicBuf[writeLen]=0;
						m_TaskArg->iIOStream->Write(assicBuf,writeLen);
						*m_TaskArg->iStreamLen+=writeLen;
						less=GetRestByte(&buf[6+internetBuf.dwBufferLength-1]);
						if(less>0)
						{
							memcpy(&buf[6-less],&buf[6+internetBuf.dwBufferLength-1-less+1],less);
						}
					}
					else
					{
						*m_TaskArg->iStreamLen+=internetBuf.dwBufferLength;
						m_TaskArg->iIOStream->Write(&buf[6],internetBuf.dwBufferLength);
					}
				}
				
			}
			else if(m_TaskArg->iEncode->CompareI("gzip")||m_TaskArg->iEncode->CompareI("deflate"))
			{
				//数据用gzip进行压缩，需解压缩
				//deflate使用inflateInit()，而gzip使用inflateInit2()进行初始化，
				//比 inflateInit()多一个参数: -MAX_WBITS，表示处理raw deflate数据
				//。因为gzip数据中的zlib压缩数据块没有zlib header的两个字节。
				//使用inflateInit2时要求zlib库忽略zlib header。
				char lBuf[MAX_BUF_SIZE+6];
				int decodeLen=0;
				int err = 0;
				bool  raw=true;				//是否是defalte格式
				bool  flag=false;           //用于在压缩格式为deflate时候


				if(m_TaskArg->iEncode->CompareI("gzip"))
					raw	=false;
	
				z_stream d_stream = {0}; /* decompression stream */
#ifdef _DEBUG
				LOG("ReadResponseTask","Url:%s  the data have encoded, need decode....",url.GetBuffer());
#endif
				static char dummy_head[2] = 
				{
					0x8 + 0x7 * 0x10,
					(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
				};
				d_stream.zalloc = (alloc_func)0;
				d_stream.zfree = (free_func)0;
				d_stream.opaque = (voidpf)0;
				if(!raw)
				{
					if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
				}
				else
				{
					if(inflateInit(&d_stream)!=Z_OK)return -1;
				}

				while(1)
				{
					if(CheckOutCmd())break;

					memset(&internetBuf,0,sizeof(INTERNET_BUFFERS));
					internetBuf.lpvBuffer=&buf[6];
					internetBuf.dwStructSize=sizeof(internetBuf);
					internetBuf.dwBufferLength=MAX_BUF_SIZE;

					if(!::InternetReadFileEx(*m_TaskArg->iHttp,&internetBuf,IRF_ASYNC,(DWORD)(&m_TaskArg->iNotifyEvent)))
					{
						if (GetLastError() != ERROR_IO_PENDING)
						{
							LOG("ReadResponseTask","Url:%s read data failed!",
							url.GetBuffer());
							break;
						}
						if(0!=WaitEvent())break;
					}

					if(!internetBuf.dwBufferLength)
					{
						m_TaskArg->iIOStream->Flush();
						DoException(HTTP_FINISH);
						LOG("ReadResponseTask","Url:%s success finish!",url.GetBuffer());
						return 1;
					}
					//判断是否存在zlib header头
					//没有补上，Apache的deflate变种可能也没有zlib header，
					//需要添加假头后处理。
					//即MS的错误deflate (raw deflate).zlib头第1字节一般是0x78, 
					//第2字节与第一字节合起来的双字节应能被31整除
					if(raw&&!flag&&(buf[6]!=(char)0x78||!((buf[6]<<8+buf[7])%31)))
					{
						d_stream.next_in  = (unsigned char*)&buf[4];
						memcpy(&buf[4],dummy_head,2);
						internetBuf.dwBufferLength+=2;
					}
					else
					{
						d_stream.next_in  = (unsigned char*)&buf[6];
					}
					d_stream.avail_in = 0;
					d_stream.next_out = (unsigned char*)&lBuf[6];

					d_stream.total_in	=0;
					d_stream.total_out	=0;
					do
					{
						while (d_stream.total_out < MAX_BUF_SIZE && d_stream.total_in < internetBuf.dwBufferLength) 
						{
							d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
							if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) 
							break;
							if(err != Z_OK )
							{
								if(err == Z_DATA_ERROR)
								{
									CMyDebug::Log("ReadResponseTask",CMyDebug::MESSAGE_BOX,0,"Url:%s  decode have a exception,need debug!!",url.GetBuffer());
									d_stream.next_in = (Bytef*) dummy_head;
									d_stream.avail_in = sizeof(dummy_head);
									if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
									{
										break;
									}
								}
								else 
								{
									break;
								}
							}
						}
						if(err!=Z_OK&err!=Z_STREAM_END)
							break;

						if(isUnicode)
						{
							int writeLen;
							writeLen=CMyAsyncHttp::FromUnicodeToAssic(&lBuf[6-less],d_stream.total_out+less,assicBuf,sizeof(assicBuf));
							assicBuf[writeLen]=0;
							m_TaskArg->iIOStream->Write(assicBuf,writeLen);
							m_TaskArg->iIOStream->Flush();
							*m_TaskArg->iStreamLen+=writeLen;
							less=GetRestByte(&lBuf[6+d_stream.total_out-1]);
							if(less>0)
							{
								memcpy(&lBuf[6-less],&lBuf[6+d_stream.total_out-1-less+1],less);
							}
						}
						else
						{
							*m_TaskArg->iStreamLen+=d_stream.total_out;
							m_TaskArg->iIOStream->Write(&lBuf[6],d_stream.total_out);
							m_TaskArg->iIOStream->Flush();
						}

						//输入缓冲区满,则需提供新的输入
						if(d_stream.total_out<MAX_BUF_SIZE||err==Z_STREAM_END)
						{
							memset(&lBuf[6],0,MAX_BUF_SIZE);
							break;
						}
						//其它情况说明，输出缓冲不足，清空缓冲
						d_stream.total_out=0;
						d_stream.next_out = (unsigned char*)&lBuf[6];
						memset(&lBuf[6],0,MAX_BUF_SIZE);
						flag	=true;
					}while(1);
			
					if(err!=Z_OK&&err!=Z_STREAM_END)
					{
#ifdef _DEBUG
						LOG("ReadResponseTask","Url:%s there have a err happened when decode the stream!",url.GetBuffer());
#endif
						break;
					}
				}
			}
			else
			{
				//未知压缩格式
				CMyDebug::Log("ReadResponseTask",CMyDebug::MESSAGE_BOX,0,"Url:%s the unknow encode format:%s",url.GetBuffer(),m_TaskArg->iEncode->GetBuffer());
			}
			DoException();
			return -1;
		}
	};

public:
	CMyAsyncHttp(char *userAgent="Async_Http");
	virtual ~CMyAsyncHttp();

	int			Get(char *uri,char *page);
	int			Post(char *uri,char *page,char *postData,int dataLen);

	void		SetUserInfo(char *userName,char *password);
	void		AddHttpHeader(char *header);
	void		SetAcceptType(char *type);
	void		SetRefer(char *refer);
	//返回的缓冲不能进行修改，否则产生无法预期后果
	char*		GetReceiveData(){return m_Data.GetMapping();}
	CMyFile*	GetRecevieFile(){return &m_Data;}
	int			GetReceiveDataLen(){return m_DataLen;}
	int			GetStatusCode(){return m_StatusCode;}  

	CMyString	GetDataCharset(){return m_Charset;};
	 //获取当前此http请求的状态,可以根据状态判断当前http是否结束
	HTTP_STATE	GetHttpState();          

	void		SaveData(char*	filePath);

	void		TerminateHttp();

	bool		IsIdle();

	static void InitalHttp();
	static void UInitalHttp();
	static void	HttpStatusCallback(	HINTERNET	session,
									DWORD		context,
									DWORD		sessionStatus,
									LPVOID		statusInformation,
									DWORD		statusInformationLength
									);

	static int	HttpGetCharset(HINTERNET request,CMyString &charset);
	static int	HttpGetEncode(HINTERNET request,CMyString &encode);
	static int	HttpGetStatusCode(HINTERNET request,int &code);
	static int	HttpQueryInfo(HINTERNET request,CMyString &info,int code);
	static int	FromUnicodeToAssic(char *utf8,int utf8Len,char *assic,int assicLen);
	static void	SetUserAgent(char *userAgent);
	//设置异步处理线程
	void		SetTargetThread(CMyTaskThread* thread);
	void		SetTargetThreadPool(CMyThreadPool* threadPool);

	void		SetInnerTaskWaitTime(unsigned timeOut);
	
public:
	HANDLE				m_FinishNotify;	//当一次异步请求处理完毕时候次事件有信号
	
private:
	int			HttpAddHeader(HINTERNET request);
	int			HttpReadData(HINTERNET);
	void		AnalysisUri(char *uri,CMyString &lastUri,CMyString &lastHtmlPath);
	
	

	int			request(bool getOrPost,char *uri,char *page,char *postData,int dataLen);
private:
	ConnectHttpTask		m_ConnectTask;
	OpenRequestTask		m_ORequestTask;
	SendRequestTask		m_SRequestTask;
	ReadResponseTask	m_ReadResponseTask;

	CMyTaskThread		m_SelfThread;
	CMyTaskThread*		m_TaskThread;   
	CMyThreadPool*		m_ThreadPool;
	bool				m_AssignTaskThread;  

	HANDLE				m_TerminateEvent;	//用于通知task终止任务事件
	HANDLE				m_Notify;			//task通知事件

	HTTP_STATE			m_HttpState;   //http状态

	HINTERNET			m_Connect;
	HINTERNET			m_Http;
	HINTERNET			m_Session;

	CMyString			m_Url;        //访问网址
	CMyString			m_Path;		  //访问路径
	CMyString			m_Verb;       //动作，post或者get
	ExtraData			m_ExtraData;  //用于保存post的数据

	TaskArg				m_TaskArg;
private:
	CMyString m_Cookie;       //cookie
	CMyString m_HttpHeader;	  //http头
	CMyString m_UserName;	  //连接时候的用户名
	CMyString m_Password;	  //连接时候的用户密码
	CMyString m_AcceptType;   //指定请求时候的接受数据类型，默认文本
	CMyString m_Refer;        //从哪个uri位置引用到请求网址
	CMyString m_UserAgent;

	int				m_DataLen;
	CMyTempFile		m_Data;       //保存数据的文件

	CMyString m_Charset;         //接受到的数据字符集
	CMyString m_Encode;          //接受到的数据压缩格式，为空串则表示没压缩
	int		  m_StatusCode;      //获取Get或者Post后的状态码

	unsigned	long	m_TimeOut;//每次任务的等待超时时间
public:
	static	HANDLE m_Mutex;			
	static char*  TAG;
};

#endif