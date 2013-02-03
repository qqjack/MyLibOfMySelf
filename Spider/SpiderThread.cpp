#include "SpiderThread.h"
#include <algorithm>

#define LOG_SWITCH

#ifdef LOG_SWITCH
	#define Log LOG
#else
	#define Log LOGNULL
#endif

char *SpiderThread::TAG="SpiderThread";

SpiderThread::SpiderThread()
{
	m_EndEvent	=CreateEvent(false,false,NULL);
//	m_HttpCount	=DEFAULT_SPIDER_THREAD;
	m_HttpCount	=6;
	m_ThreadPool.SetThreadCount(DEFAULT_SPIDER_THREAD);
	m_UrlCmp	=DefaultUrlSortCmpFun;
	m_SpiderMode=SPIDER_DEPTH;

	m_FetchUrl	=&m_InnerFetchUrl;
	m_ErrorNotify	=NULL;
	m_PageProcess	=NULL;
	m_FileProcess	=NULL;
	m_UrlModify		=NULL;
	for(int i=0;i<m_HttpCount;i++)
	{
		m_Http[i].SetTargetThreadPool(&m_ThreadPool);
	}
}

SpiderThread::~SpiderThread()
{
	if(isRun())
		End();
	DeleteEvent(m_EndEvent);
}

int SpiderThread::Run(void *param)
{
	HANDLE		httpNotifyEvent[MAX_SPIDER_THREAD+1];
	int waitR=0;
	CMyAsyncHttp::HTTP_STATE state;
	int idleHttpCount=0;
	bool	exit=false;
	for(int i=0;i<m_HttpCount;i++)
	{
		httpNotifyEvent[i]=m_Http[i].m_FinishNotify;
		m_Http[i].SetMark(false);
	}
	httpNotifyEvent[i]=m_EndEvent;

	UrlInfo*	url=new UrlInfo;
	url->iUrl	=(char*)param;
			
	m_UrlList.push_back(url);

	AddHashMap(CUrl::GetUrlHost(url->iUrl),url->iUrl);

	while(1)
	{
		idleHttpCount	=0;
		for(int i=0;i<m_HttpCount;i++)
		{
			//检查退出指令
			if(WaitEvent(m_EndEvent,0)==0)
			{
				exit	=true;
				break;
			}
			//判断当前连接是否空闲
			if(m_Http[i].IsIdle())
			{
				if(!m_Http[i].GetMark())
				{
					state	=m_Http[i].GetHttpState();
					if(state==CMyAsyncHttp::HTTP_FINISH&&m_Http[i].GetCurrentUrl()!="")
					{
						//http请求正确
						int state=m_Http[i].GetStatusCode();
						if(state<400)
						{
							//服务器返回正常
							if(state<300)
								AnalysisData(&m_Http[i]);
							else
								RelocateUrl(&m_Http[i]);
						}
						else
						{
							ErrorProcess(&m_Http[i]);
						}
					}
					else if(state==CMyAsyncHttp::HTTP_TIMEOUT||state==CMyAsyncHttp::HTTP_STOP)
					{
						//http请求超时
						ErrorProcess(&m_Http[i]);
					}
				}
				else
				{
					//如果http连接已经标记则递增计算器，
					idleHttpCount++;
				}

				if(GetNextUrl())
				{
					m_Http[i].SetParentUrl(m_CurrentUrl->iParentUrl);
					m_Http[i].Get(m_CurrentUrl->iUrl.GetBuffer());
					//获取到url说明客运进行get操作，即可能获取更多的Url
					//对计算器进行清零操作
					idleHttpCount	=0;
					delete m_CurrentUrl;
				}
				else
				{
					//获取url失败表示Url已经提前完，此对此http连接进行标记
					//此后不再有get操作
					m_Http[i].SetMark(true);
				}
			}
			
		}
		if(exit)
			break;

		//如果计数器到达最大数说明
		//所以url已经处理完成可以退出
		if(idleHttpCount==m_HttpCount)
			break;

		Log("tst","wait!!!");
		waitR=::WaitForMultipleObjects(m_HttpCount+1,httpNotifyEvent,false,-1);
		Log("tst","contiue!!");

		if(waitR==m_HttpCount)
			break;
	}

	ClearUrlList();
	return 1;
}

void SpiderThread::ClearUrlList()
{
	for(int i=0;i<m_UrlList.size();i++)
		delete m_UrlList[i];
	m_UrlList.clear();
}

int SpiderThread::SetSpiderMode(SpiderMode mode)
{
	if(isRun())return -1;
	m_SpiderMode	=mode;
	return 1;
}

int SpiderThread::AddUrlFilter(ISpiderUrlFilter *urlFilter)
{
	if(isRun())return -1;
	m_UrlFilterList.push_back(urlFilter);
	return 1;
}

int SpiderThread::SetErrorNotify(ISpiderErrorNotify* errorNotify)
{
	if(isRun())return -1;
	m_ErrorNotify	=errorNotify;
	return 1;
}

int SpiderThread::SetPageProcessMethod(ISpiderPageProcess* processMethod)
{
	if(isRun())return -1;
	m_PageProcess	=processMethod;
	return 1;
}

int SpiderThread::SetFileProcessMethod(ISpiderFileProcess* processMethod)
{
	if(isRun())return -1;
	m_FileProcess	=processMethod;
	return 1;
}

int SpiderThread::SetUrlModifyRule(ISpiderUrlModify* urlModify)
{
	if(isRun())return -1;
	m_UrlModify		=urlModify;
	return 1;
}

int SpiderThread::SetPageUrlSortFunc(UrlCmpFunc urlSortFunc)
{
	if(isRun())return -1;
	m_UrlCmp	=urlSortFunc;
	return 1;
}

int SpiderThread::SetMaxThread(int count)
{
	if(isRun())return -1;
	m_ThreadPool.SetThreadCount(count);
	return 1;
}

void SpiderThread::End()
{
	SetEvent(m_EndEvent);
	if(::GetCurrentThreadId()!=this->GetThreadID())
		WaitEvent(this->GetThreadHandle(),-1);
}

void SpiderThread::AnalysisData(SpiderHttp* spiderHttp)
{
	CMyString url;
	CMyString host;

	bool	  haveUrl=true;
	if(spiderHttp->IsTxtPage())
	{
		if(!InitalFetchEngine(spiderHttp->GetReceiveData(),spiderHttp->GetReceiveDataLen()))
			haveUrl=false;
		while(haveUrl)
		{
			int urlCount=m_FetchUrl->FetchUrl(m_CurrentP,m_Regex.GetMatchStrLen());
			for(int i=0;i<urlCount;i++)
			{
				url	=*(m_FetchUrl->GetUrl(i));
				if(url=="./")continue;
				ReBuildUrlIfNeed(spiderHttp->m_Url,url,host);
				if(HaveAcess(host,url))continue;
				for(int i=0;i<m_UrlFilterList.size();i++)
				{
					if(!m_UrlFilterList[i]->FilterCheck(spiderHttp->m_Url.GetBuffer(),url.GetBuffer()))break;
				}
				if(i<m_UrlFilterList.size())continue;
				if(m_UrlModify)
				{
					m_UrlModify->ModifyUrl(spiderHttp->m_Url.GetBuffer(),url);
				}
				AddHashMap(host,url);
				AddTempUrlList(url);
			}
			if(!FetchUrl(url))break;
		}
		
		if(m_PageProcess)
		{
			m_PageProcess->PageProcess(spiderHttp->m_ParentUrl.GetBuffer(),spiderHttp->m_Url.GetBuffer(),spiderHttp->GetReceiveData(),spiderHttp->GetReceiveDataLen());
		}
		if(m_UrlCmp)
		{
			SortTempUrlList();
		}
		
		//根据深度优先还是广度优先进行抓取
		AddAllUrlToUrlList(spiderHttp->m_Url);
	}
	else if(m_FileProcess)
	{
		m_FileProcess->FileProcess(spiderHttp->m_ParentUrl.GetBuffer(),spiderHttp->m_Url.GetBuffer(),spiderHttp->GetReceiveData(),spiderHttp->GetReceiveDataLen());
	}	
}

bool SpiderThread::ErrorProcess(SpiderHttp* spiderHttp)
{
	
	int errorCode=0;
	bool processed=false;
	//错误的分类处理
	errorCode	=spiderHttp->GetStatusCode();
	if(errorCode<400)
	{
		if(spiderHttp->GetHttpState()==CMyAsyncHttp::HTTP_STOP)
		{
			errorCode	=-1;  //内部请求过程出错
		}
		else 
		{
			errorCode	=-2;  //内部请求超时出错
		}
	}
	Log(TAG,"ErrorProcess:%s errorCode:%d",spiderHttp->m_Url.GetBuffer(),errorCode);
	if(m_ErrorNotify!=NULL)
	{
		if(errorCode>=0)
			processed= m_ErrorNotify->ErrorProcess(spiderHttp->m_ParentUrl.GetBuffer(),
					spiderHttp->m_Url.GetBuffer(),errorCode,
					spiderHttp->GetReceiveData(),spiderHttp->GetReceiveDataLen());
		else 
			processed= m_ErrorNotify->ErrorProcess(spiderHttp->m_ParentUrl.GetBuffer(),
					spiderHttp->m_Url.GetBuffer(),errorCode,
					NULL,0);

		if(processed)
		{
			UrlInfo *url	=new UrlInfo;
			url->iUrl		=spiderHttp->m_Url;
			url->iParentUrl	=spiderHttp->m_ParentUrl;
			m_UrlList.push_back(url);
		}
	}
	return processed;
}

void SpiderThread::AddHashMap(CMyString &host,CMyString &url)
{
	SecondHashMap* hashMap;
	void *value	=NULL;
	if(-1==m_MainHashMap.GetItem(host,(void**)&hashMap))
	{
		hashMap	=new SecondHashMap();
		m_MainHashMap.AddItem(host,(void* &)hashMap);
	}
	hashMap->AddItem(url,value);
}

bool SpiderThread::InitalFetchEngine(char *buf,int bufLen)
{
	m_Regex.SetMainString(buf,bufLen);
	m_CurrentP	=m_Regex.Match(m_UrlRegex.GetBuffer());
	if(m_CurrentP)return true;
	return false;
}

void SpiderThread::ReBuildUrlIfNeed(CMyString &parentUrl,CMyString &url,CMyString &host)
{
	bool needRebuild=false;
	int len=url.GetStrLen();
	host	=CUrl::GetUrlHost(parentUrl);
	if(len>=4)
	{
		if(url[0]=='w'&&url[1]=='w'&&url[2]=='w'&&url[3]=='.')
		{
		}
		else if(url[0]=='b'&&url[1]=='b'&&url[2]=='s'&&url[3]=='.')
		{
		}
		else if(url[0]=='h'&&url[1]=='t'&&url[2]=='t'&&url[3]=='p')
		{
			if(len>=5&&url[4]==':')
			{
			}
			else
			{
				needRebuild=true;
			}
		}
		else
		{
			if(len>0&&url[0]=='.')
			{
				needRebuild=true;
			}
			else if(len>=3&&url[0]=='/'&&url[1]=='.'&&url[2]=='.')
			{
				needRebuild=true;
				len--;
				url.Erase(0,1);
			}
			else
			{
				int status=0;
				int i=0;
				for(;i<len;i++)
				{
					if(!status&&url[i]=='.')
					{
						status=1;
					}
					else if(status==1&&url[i]=='.')
					{
						break;
					}
					else if(status==1&&url[i]=='/')
					{
						break;
					}
				}
				if(i>=len)
					needRebuild=true;
			}
		}
	}
	else
	{
		needRebuild=true;
	}

	if(needRebuild)
	{
		CUrl lUrl(parentUrl);
		CMyString path=lUrl.GetUrlHost()+"/"+lUrl.GetUrlPath();
		int count=0;
		while(1)
		{
			if(url[0]=='.'&&url[1]=='.'&&url[2]=='/')
			{
				url.Erase(0,3);
				count++;
			}
			else
			{
				break;
			}
		}
		
		while(count)
		{
			count--;
			char *p=path.GetBuffer()+path.GetStrLen()-1;
			if(*p=='/')
			{
				path.EraseFromRight(1);
				p--;
			}
			char *e=p;
			while(p!=path.GetBuffer()&&*p!='/')p--;
			if(*p!='/')
			{
				_asm int 3;
			}
			else
			{
				if(count)
					path.EraseFromRight(e-p+1);
				else 
					path.EraseFromRight(e-p);
			}
		}
		len =path.GetStrLen();
		if(path[len-1]=='/'&&url[0]=='/')
			path.EraseFromRight(1);
		else if(path[len-1]!='/'&&url[0]!='/')
			path+="/";
		url=path+url;
	}
}

bool SpiderThread::HaveAcess(CMyString &host,CMyString &url)
{
	SecondHashMap* hashMap;
	void *value	=NULL;
	if(-1==m_MainHashMap.GetItem(host,(void**)&hashMap))
	{
		return false;
	}
	else
	{
		if(hashMap->GetItem(url,(void**)&value)==-1)
			return false;
	}
	return true;
}

void SpiderThread::AddTempUrlList(CMyString &url)
{
	m_TempList.push_back(new CMyString(url));
}

void SpiderThread::SortTempUrlList()
{
	sort(m_TempList.begin(),m_TempList.end(),m_UrlCmp);
}

void SpiderThread::AddAllUrlToUrlList(CMyString &parent)
{
	int size=m_TempList.size();
	for(int i=0;i<size;i++)
	{
		UrlInfo *url	=new UrlInfo;
		url->iUrl		=*m_TempList[i];
		url->iParentUrl	=parent;
		if(m_SpiderMode==SPIDER_DEPTH)	
			m_UrlList.push_back(url);
		else
			m_UrlList.insert(0,url);
		delete m_TempList[i];

		Log(TAG,"the url:%s",url->iUrl.GetBuffer());
	}
	m_TempList.clear();
	if(m_UrlList.size()==0)
		_asm int 3;
	Log(TAG,"the url num:%d",m_UrlList.size());
}

bool SpiderThread::FetchUrl(CMyString &url)
{

	m_CurrentP=m_Regex.MatchNext();
	if(m_CurrentP)return true;
	return false;
}

bool SpiderThread::GetNextUrl()
{
	if(m_UrlList.size())
	{
		m_CurrentUrl=m_UrlList[0];
		m_UrlList.erase(m_UrlList.begin());
		return true;
	}
	return false;
}

void SpiderThread::RelocateUrl(SpiderHttp* spiderHttp)
{
	Log(TAG,2,0,"need relocate url!!");
}

int SpiderThread::SetFecthUrlRegex(char* regex)
{
	if(isRun())return -1;
	m_UrlRegex	=regex;
}