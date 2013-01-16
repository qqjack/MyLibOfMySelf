
#include "MyAsyncHttp.h"

#define MAX_BUF_SIZE (1000+1)
#define MAX_READ_SIZE (1000)


#pragma   comment(lib, "../MyLibUsedLib/zlib")
#pragma comment(lib,"Wininet.lib") 

char*  CMyAsyncHttp::TAG="CAsyncHttp";
HANDLE CMyAsyncHttp::m_Mutex =NULL;
void __stdcall Callback(HINTERNET hInternet,
              DWORD dwContext,
              DWORD dwInternetStatus,
              LPVOID lpStatusInfo,
              DWORD dwStatusInfoLen)
{
	CMyAsyncHttp::HttpStatusCallback(hInternet,dwContext,dwInternetStatus,lpStatusInfo
		,dwStatusInfoLen);
}

CMyAsyncHttp::CMyAsyncHttp(char *userAgent)
{
	m_UserAgent		=userAgent;
	m_TaskThread	=NULL;
	m_ThreadPool	=NULL;
	m_TimeOut		=MAX_WAIT_TIME;
	m_TaskArg.iAcceptType	=&m_AcceptType;
	m_TaskArg.iCharset		=&m_Charset;
	m_TaskArg.iEncode		=&m_Encode;
	m_TaskArg.iHttpHeader	=&m_HttpHeader;
	m_TaskArg.iHttpState	=&m_HttpState;
	m_TaskArg.iPassword		=&m_Password;
	m_TaskArg.iUserName		=&m_UserName;
	m_TaskArg.iRefer		=&m_Refer;
	m_TaskArg.iPath			=&m_Path;
	m_TaskArg.iUrl			=&m_Url;
	m_TaskArg.iVerb			=&m_Verb;

	m_TaskArg.iTaskRunTarget1	=&m_TaskThread;
	m_TaskArg.iTaskRunTarget2	=&m_ThreadPool;
	m_TaskArg.iConnect		=&m_Connect;
	m_TaskArg.iExtraData	=&m_ExtraData;
	m_TaskArg.iFinishNotify	=&m_FinishNotify;
	m_TaskArg.iHttp			=&m_Http;
	m_TaskArg.iIOStream		=&m_Data;
	m_TaskArg.iNotifyEvent	=&m_Notify;
	m_TaskArg.iSession		=&m_Session;
	m_TaskArg.iStatusCode	=&m_StatusCode;
	m_TaskArg.iStreamLen	=&m_DataLen;
	m_TaskArg.iTerminateEvent	=&m_TerminateEvent;
	m_TaskArg.iTaskList[0]	=&m_ORequestTask;
	m_TaskArg.iTaskList[1]	=&m_SRequestTask;
	m_TaskArg.iTaskList[2]	=&m_ReadResponseTask;
	
	m_TaskArg.iTimeOut		=&m_TimeOut;

	m_TaskArg.iAssignTaskThread	=&m_AssignTaskThread;  
	m_TaskArg.iAsyncResult	=0;

	m_TerminateEvent	=CreateEvent(false,false,NULL);
	m_Notify			=CreateEvent(false,false,NULL);
	m_FinishNotify		=CreateEvent(false,false,NULL);

	m_AssignTaskThread	=false;
	if(m_Mutex)
		m_Mutex				=CreateMutex(false,NULL);

	m_HttpState	=HTTP_FINISH;
	m_Session=::InternetOpen(m_UserAgent.GetBuffer(),PRE_CONFIG_INTERNET_ACCESS,
		NULL,INTERNET_INVALID_PORT_NUMBER,INTERNET_FLAG_ASYNC);
#ifdef _DEBUG
	if(!m_Session)
		LOG(TAG,"Open Internet failed!");
#endif
//	CSetCallBackThread callbackThread;
//	callbackThread.Start((void*)m_Session);
//	WaitForSingleObject(callbackThread.GetThreadHandle(),-1);
	if(INTERNET_INVALID_STATUS_CALLBACK==
		InternetSetStatusCallback(m_Session,(INTERNET_STATUS_CALLBACK)Callback))
//	if(INTERNET_INVALID_STATUS_CALLBACK==(INTERNET_STATUS_CALLBACK)callbackThread.GetRunValue())
	{
#ifdef _DEBUG
		LOG(TAG,"InternetSetStatusCallback failed");
#endif
	}
}

CMyAsyncHttp::~CMyAsyncHttp()
{
	if(!IsIdle())
	{
		//在析构前停止未完成的请求
		TerminateHttp();
	}
	if(m_Session)
	{
		InternetSetStatusCallback(m_Session,(INTERNET_STATUS_CALLBACK)NULL);
		::InternetCloseHandle(m_Session);
	}
	if(m_SelfThread.isRun())
		m_SelfThread.ExitThreadDirect();
}


int CMyAsyncHttp::Get(char *uri,char *page)
{
	return request(true,uri,page,NULL,0);
}

int CMyAsyncHttp::Post(char *uri,char *page,char *postData,int dataLen)
{
	return request(false,uri,page,postData,dataLen);
}

int CMyAsyncHttp::request(bool get,char *uri,char *page,char *postData,int dataLen)
{
	if(!m_Session)return -1;

	CMyString lastUri,lastHtmlPath;
	this->AnalysisUri(uri,m_Url,m_Path);
	if(!m_Path.GetStrLen()&&page)
		m_Path=page;
	m_ExtraData.iExtraData=NULL;
	m_ExtraData.iExtraDataLen=0;
	if(get)
		m_Verb="GET";
	else 
	{
		m_Verb="POST";
		m_ExtraData.iExtraData=postData;
		m_ExtraData.iExtraDataLen=dataLen;
	}
	m_StatusCode=0;
	m_Charset   ="";
	m_Encode	="";
	
	m_HttpState		=HTTP_START;
	m_Data.ReleaseMapping();
	m_Data.Seek(CMyFile::SEEK_START,0);
	m_DataLen	=0;
	if(!m_AssignTaskThread)
	{
		m_TaskThread	=&m_SelfThread;
		WaitForSingleObject(m_TaskThread->GetThreadHandle(),-1);
		if(!m_SelfThread.isRun())
			m_TaskThread->Start(NULL);
		m_TaskThread->AssignTask(m_ConnectTask,&m_TaskArg);
	}
	else
	{
		if(m_TaskThread)
			m_TaskThread->AssignTask(m_ConnectTask,&m_TaskArg);
		else if(m_ThreadPool)
			m_ThreadPool->AssignTask(m_ConnectTask,&m_TaskArg);
	}
	return 1;
}

void CMyAsyncHttp::InitalHttp()
{
	WSADATA wsaData;
	if(NO_ERROR!=::WSAStartup(MAKEWORD(2,2),&wsaData))
	{
		WSACleanup();
		LOG(TAG,"inital http failed!");
	}
}

void CMyAsyncHttp::UInitalHttp()
{
	WSACleanup();
}


int CMyAsyncHttp::HttpAddHeader(HINTERNET request)
{
	int r=1;
	if(m_HttpHeader.GetStrLen())
		r=::HttpAddRequestHeaders(request, m_HttpHeader.GetBuffer(), 
				m_HttpHeader.GetStrLen(),
				HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD);
	return r;
}

void CMyAsyncHttp::SetUserInfo(char *userName,char *password)
{
	if(IsIdle())
		m_Password=password;
}

void CMyAsyncHttp::AddHttpHeader(char *header)
{
	if(IsIdle())
		m_HttpHeader+=header;
}

void CMyAsyncHttp::SetAcceptType(char *type)
{
	if(IsIdle())
		m_AcceptType=type;
}

void CMyAsyncHttp::SetRefer(char *refer)
{
	if(IsIdle())
		m_Refer=refer;
}

void CMyAsyncHttp::SetInnerTaskWaitTime(unsigned timeOut)
{
	if(IsIdle())
		m_TimeOut	=timeOut;
}

void CMyAsyncHttp::SetTargetThread(CMyTaskThread* thread)
{
	m_TaskThread=thread;
	m_AssignTaskThread	=true;
	m_ThreadPool		=NULL;
}

void CMyAsyncHttp::SetTargetThreadPool(CMyThreadPool* threadPool)
{
	m_ThreadPool		=threadPool;
	m_AssignTaskThread	=true;
	m_TaskThread		=NULL;
}

void CMyAsyncHttp::AnalysisUri(char *uri,CMyString &lastUri,CMyString &lastHtmlPath)
{
	char *pStart,*pEnd;
	CMyString strUri=uri;
	bool hasHtmlPath=true;
	pStart=uri;

	pStart		=strstr(uri,"http://");
	if(!pStart)
		pStart=uri;
	else
		pStart+=7;
	pEnd=pStart;
	while(*pEnd&&*pEnd!='/')pEnd++;

	if(!*pEnd)hasHtmlPath=false;
	pEnd--;

	lastUri=strUri.GetSubStr(pStart-uri,pEnd-uri);
	if(hasHtmlPath)
		lastHtmlPath=strUri.GetSubStr(pEnd+1-uri);
}

int CMyAsyncHttp::HttpGetCharset(HINTERNET request,CMyString &charset)
{
	CMyString str;
	if(HttpQueryInfo(request,str,HTTP_QUERY_CONTENT_TYPE))
	{
		char *p=str.GetBuffer();
		p=strstr(p,"charset=");
		if(p)charset=p+8;
		return 1;
	}
#ifdef _DEBUG
	int d=::GetLastError();
	if(d==ERROR_HTTP_HEADER_NOT_FOUND)
	{
		LOG(TAG,"HttpGetCharset,can't find the header!");
	}
#endif
	return 1;
	
}

int CMyAsyncHttp::HttpGetEncode(HINTERNET request,CMyString &encode)
{
	if(HttpQueryInfo(request,encode,HTTP_QUERY_CONTENT_ENCODING))
	{
	}
#ifdef _DEBUG
	int d=::GetLastError();
	if(d==ERROR_HTTP_HEADER_NOT_FOUND)
	{
		LOG(TAG,"HttpGetEncode,can't find the header!");
	}
#endif
	return 1;
}

int CMyAsyncHttp::HttpQueryInfo(HINTERNET request,CMyString &info,int code)
{
	unsigned long rl=0;
	::HttpQueryInfo(request,code,NULL,&rl,0);

	if(rl>0)
	{
		info.Resize(rl+1);
		return ::HttpQueryInfo(request,code,info.GetBuffer(),&rl,0);
	}
	return false;
}

int	CMyAsyncHttp::HttpGetStatusCode(HINTERNET request,int &code)
{
	CMyString str;
	code=0;

	if(HttpQueryInfo(request,str,HTTP_QUERY_STATUS_CODE))
	{
		code=str.ToInt();
	};
#ifdef _DEBUG
	int d=::GetLastError();
	if(d==ERROR_HTTP_HEADER_NOT_FOUND)
	{
		LOG(TAG,"HttpGetStatusCode,can't find the header!");
	}
#endif
	return 1;
}

void CMyAsyncHttp::TerminateHttp()
{
	SetEvent(m_TerminateEvent);
	::WaitForSingleObject(m_FinishNotify,-1);
}

CMyAsyncHttp::HTTP_STATE CMyAsyncHttp::GetHttpState()
{
	HTTP_STATE	state;
	GetMutex(m_Mutex);
	state	=m_HttpState;
	ReleaseMutex(m_Mutex);
	return state;
}

bool CMyAsyncHttp::IsIdle()
{
	HTTP_STATE state	=GetHttpState();
	return state!=HTTP_START;
}

void CMyAsyncHttp::SaveData(char*	filePath)
{
	if(IsIdle()&&m_DataLen)
		m_Data.CopyToFile(filePath);
}

int CMyAsyncHttp::FromUnicodeToAssic(char *utf8,int utf8Len,char *assic,int assicLen)
{
	char  unicodeBuf[MAX_BUF_SIZE*2];
	int count;
	count	=MultiByteToWideChar(CP_UTF8,0,utf8,utf8Len,(unsigned short*)unicodeBuf,MAX_BUF_SIZE*2);
    unicodeBuf[count*2]=0;
	unicodeBuf[count*2+1]=0;
	count   =WideCharToMultiByte(CP_ACP, 0, (const unsigned short*)unicodeBuf, count, assic, assicLen, NULL, NULL);
	return count;
}

void CMyAsyncHttp::HttpStatusCallback(	HINTERNET	session,
									DWORD		context,
									DWORD		sessionStatus,
									LPVOID		statusInformation,
									DWORD		statusInformationLength
									)
{
	HttpContext*	cxt=(HttpContext*)context;
	switch(cxt->iMark)
	{
	case 1:
		{
			if(INTERNET_STATUS_HANDLE_CREATED==sessionStatus)
			{
				INTERNET_ASYNC_RESULT*		result=(INTERNET_ASYNC_RESULT*)statusInformation;
				*(cxt->iConnect)=(HINTERNET)result->dwResult;
//				LOG(TAG,"Connect handle created");
				LOG(TAG,"Path:%s  Connect handle created",cxt->iPath->GetBuffer());
				SetEvent(*cxt->iNotifyEvent);
			}
			break;
		}
	case 2:
		{
			switch(sessionStatus)
			{
			case INTERNET_STATUS_HANDLE_CREATED:
				{
					INTERNET_ASYNC_RESULT*		result=(INTERNET_ASYNC_RESULT*)statusInformation;
					*cxt->iHttp=(HINTERNET)result->dwResult;
					cxt->iAsyncResult	=((INTERNET_ASYNC_RESULT*)statusInformation)->dwError;
//					LOG(TAG,"Request handle created");
					LOG(TAG,"Path:%s  Request handle created",cxt->iPath->GetBuffer());
					SetEvent(*cxt->iNotifyEvent);
					break;
				}
			case INTERNET_STATUS_REQUEST_SENT:
				{
//					LOG(TAG,"INTERNET_STATUS_REQUEST_SENT");
					LOG(TAG,"Path:%s  INTERNET_STATUS_REQUEST_SENT",cxt->iPath->GetBuffer());
					break;
				}
			case INTERNET_STATUS_REQUEST_COMPLETE:
				{
//					LOG(TAG,"INTERNET_STATUS_REQUEST_COMPLETE");
					LOG(TAG,"Path:%s  INTERNET_STATUS_REQUEST_COMPLETE",cxt->iPath->GetBuffer());
					cxt->iAsyncResult	=((INTERNET_ASYNC_RESULT*)statusInformation)->dwError;
					SetEvent(*cxt->iNotifyEvent);
					break;
				}
			case INTERNET_STATUS_RECEIVING_RESPONSE:
				{
//					LOG(TAG,"INTERNET_STATUS_RECEIVING_RESPONSE");
					LOG(TAG,"Path:%s  INTERNET_STATUS_RECEIVING_RESPONSE",cxt->iPath->GetBuffer());
					break;
				}
			case INTERNET_STATUS_RESPONSE_RECEIVED:
				{
//					LOG(TAG,"INTERNET_STATUS_RESPONSE_RECEIVED");
					LOG(TAG,"Path:%s  INTERNET_STATUS_RESPONSE_RECEIVED",cxt->iPath->GetBuffer());
					break;
				}
			case INTERNET_STATUS_CONNECTING_TO_SERVER:
				{
//					LOG(TAG,"INTERNET_STATUS_CONNECTING_TO_SERVER");
					LOG(TAG,"Path:%s  INTERNET_STATUS_CONNECTING_TO_SERVER",cxt->iPath->GetBuffer());
					break;
				}
			case INTERNET_STATUS_CONNECTED_TO_SERVER:
				{
//					LOG(TAG,"INTERNET_STATUS_CONNECTED_TO_SERVER");
					LOG(TAG,"Path:%s  INTERNET_STATUS_CONNECTED_TO_SERVER",cxt->iPath->GetBuffer());
					break;
				}
			case INTERNET_STATUS_SENDING_REQUEST:
				{
//					LOG(TAG,"INTERNET_STATUS_SENDING_REQUEST");
					LOG(TAG,"Path:%s  INTERNET_STATUS_SENDING_REQUEST",cxt->iPath->GetBuffer());
					break;
				}
			default:
				{
//					LOG(TAG,"default  %d",sessionStatus);
//					LOG(TAG,"Path:%s  default  %d",cxt->iPath->GetBuffer(),sessionStatus);
				}
			}
			break;
		}
	default:
		{
//			LOG(TAG,"Path:%s  HttpStatusCallback default!",cxt->iPath->GetBuffer());
		}
	}
}