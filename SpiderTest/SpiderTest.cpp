
#include "../MyLib.h"
#define HTTP_COUNT	(5)
#define HOST_COUNT  (3)

unsigned GetHashCode(CMyString obj)
{
	return obj.HashCode();
}

class Rule;
CMyString GetHost(CMyString &url);

CMyHashMap<CMyString,int> g_HashMap(GetHashCode);
std::vector<Rule*>		  g_RuleList;
CMyString				  g_Host[HOST_COUNT];
char*					  TAG="Spider";
CMyString				  g_SlefPath;

//用于根据偏移offset，计算相对url的父url路径
class TrackUrl
{
public:
	typedef struct 
	{
		unsigned	iOffset;//偏移位置
		CMyString	iUrl;   //父url
	}TrackRelativeUrl;	
private:
	std::vector<TrackRelativeUrl*>  m_TrackRelativeUrlInfo;  //用于对相对url的补全操作
	unsigned					   m_Sum;
	
public:
	TrackUrl()
	{
	}

	~TrackUrl()
	{
		for(int i=0;i<m_TrackRelativeUrlInfo.size();i++)
			delete m_TrackRelativeUrlInfo[i];
		m_TrackRelativeUrlInfo.clear();
	}

	void RecordTrackInfo(CMyString &url,unsigned dataLen)
	{
		TrackRelativeUrl* info=new TrackRelativeUrl;
		CMyString str=url;
		
		char *p=str.GetBuffer()+str.GetStrLen()-1;
		while(*p!='.'&&*p!='/')p--;
		if(*p=='.')
		{
			while(*p!='/')p--;
			*(p+1)=0;
		}
		info->iUrl	=str;
		info->iOffset=m_Sum;
		m_Sum	+=dataLen;
		m_TrackRelativeUrlInfo.push_back(info);
	}

	void ClearTrackInfo()
	{
		m_Sum		=0;
		for(int i=0;i<m_TrackRelativeUrlInfo.size();i++)
			delete m_TrackRelativeUrlInfo[i];
		m_TrackRelativeUrlInfo.clear();
	}

	CMyString Track(unsigned offset)
	{
		int	start=0,end=m_TrackRelativeUrlInfo.size()-1;
		int mid=0;
		while(1)
		{
			mid=(start+end)/2;
			if(m_TrackRelativeUrlInfo[mid]->iOffset<offset)
			{
				start=mid;
				if(m_TrackRelativeUrlInfo.size()==1)
					return m_TrackRelativeUrlInfo[mid]->iUrl;
				continue;
			}
			if(m_TrackRelativeUrlInfo[mid]->iOffset>offset)
			{
				if(mid==0||m_TrackRelativeUrlInfo[mid-1]->iOffset<offset)
					return m_TrackRelativeUrlInfo[mid]->iUrl;
				end	=mid;
				continue;
			}
		}
	}

	void PrintTrackInfo()
	{
		for(int i=0;i<m_TrackRelativeUrlInfo.size();i++)
			printf("trackUrl:%s\n",m_TrackRelativeUrlInfo[i]->iUrl.GetBuffer());
	}
};

class Rule
{
public:
	virtual bool Check(CMyString &url)=0;
};

//对文件的下载规制
class FileRule:public Rule
{
public:
	virtual bool Check(CMyString &url)
	{
		char *p=url.GetBuffer()+url.GetStrLen()-1;
		int len=0;
		while(p!=url.GetBuffer())
		{
			if(*p=='.'||*p=='/')break;
			p--;
			len++;
		}
		if(len)
		{
			CMyString ext=CMyString::StringFromMem(p+1,0,len);
			if(ext.CompareI("jsp")||ext.CompareI("asp")||ext.CompareI("php")
				||ext.CompareI("html"))
			{
				return true;
			}
			else if(ext.CompareI("jpg"))
			{
				if(url.FindString("img.920mm.com")!=-1||
					url.FindString("pic.920mm.com")!=-1)
					return true;
				return false;
			}
			return false;
		}
		return true;
	}
};

//host规则,过滤外部链接
class MainUrlRule:public Rule
{
public:
	virtual bool Check(CMyString &url)
	{
		CMyString host=GetHost(url);
		for(int i=0;i<HOST_COUNT;i++)
		{
			if(GetHost(url)==g_Host[i])
				return true;
		}
		return false;
	}
};


class HttpAccess :public CMyAsyncHttp
{
public:
	HttpAccess():CMyAsyncHttp("Mozilla/5.0 (compatible; iaskspider/1.0; MSIE 6.0)")
	{
	}
	void	Get(char *url)
	{
		SetAcceptType("*/*");
		AddHttpHeader("Accept-Charset: utf-8\r\n");
		AddHttpHeader("Accept-Encoding: gzip, deflate\r\n");
		CMyAsyncHttp::Get(url,"");
		m_Url	=url;
	}

	CMyString GetCurrentUrl()
	{
		return m_Url;
	}

	//判断当前网址是否是文本网页而非文件
	bool IsTxtPage()
	{
		char *p=m_Url.GetBuffer()+m_Url.GetStrLen()-1;
		int len=0;
		while(p!=m_Url.GetBuffer())
		{
			if(*p=='.'||*p=='/')break;
			p--;
			len++;
		}
		if(len)
		{
			CMyString ext=CMyString::StringFromMem(p+1,0,len);
			if(ext.CompareI("jsp")||ext.CompareI("asp")||ext.CompareI("php")
				||ext.CompareI("html"))
			{
				return true;
			}
			return false;
		}
		return true;
	}

	void WriteFile(CMyFile *file)
	{
		file->Write(GetReceiveData(),GetReceiveDataLen());
		file->Flush();
	}


	void SaveFile(CMyString &relativePath)
	{
		if(s_SlefPath=="")
			s_SlefPath	=CMyString::GetSlefPath();
		CMyString path=s_SlefPath;
		path+=relativePath;
		CMyString dir=path;
		for(int i=dir.GetStrLen()-1;i>=0;i--)
		{
			if(dir[i]=='\\')
			{
				dir.SetAt(i,0);
				break;
			}
		}
		CMyFile::CreateDir(dir.GetBuffer());
		SaveData(path.GetBuffer());
	}
private:
	CMyString	m_Url;

	static CMyString	s_SlefPath;
};

CMyString	HttpAccess::s_SlefPath="";

void InitalRuleList()
{
	g_RuleList.push_back(new FileRule());
	g_RuleList.push_back(new MainUrlRule());
}

void InitalHostList()
{
	g_Host[0]="www.920mm.com";
	g_Host[1]="pic.920mm.com";
	g_Host[2]="img.920mm.com";
}

void UnInitalRuleList()
{
	int ruleSize=g_RuleList.size();
	for(int i=0;i<ruleSize;i++)
	{
		delete g_RuleList[i];
	}
	g_RuleList.clear();
}

bool FilterUrl(CMyString &url)
{
	int ruleSize=g_RuleList.size();
	for(int i=0;i<ruleSize;i++)
	{
		if(!g_RuleList[i]->Check(url))return false;
	}
	return true;
}

CMyString GetHost(CMyString &url)
{
	char *p=url.GetBuffer();
	int skip=0;
	if(url.FindString("http://")!=-1)
		skip=7;
	p+=skip;
	char *s=p;
	int len=0;
	while(*p&&*p!='/')
	{
		len++;
		p++;
	}
	return CMyString::StringFromMem(s,0,len);
}

//进行简单过滤，比如对于类似../../url的过滤
bool SampleFilter(CMyString &url)
{
	if(url[0]=='.')
		return false;
	return true;
}

//对url进行修正，主要针对于
void ModifyUrl(CMyString &url,unsigned offset,TrackUrl* trackUrl)
{
	int i=url.GetStrLen()-1;
	while(!((url[i]>='a'&&url[i]<='z')||(url[i]>='A'&&url[i]<='Z')||(url[i]>='0'&&url[i]<='a')))
		i--;
	url.SetAt(i+1,0);;
	if(url.FindString("http://")==-1&&!(url.GetStrLen()>=3&&url[0]=='w'&&url[1]=='w'&&url[2]=='w'))
	{
		CMyString str=trackUrl->Track(offset);
		if(str[str.GetStrLen()-1]!='/'&&url[0]!='/')
			str+="/";
		else if(str[str.GetStrLen()-1]=='/'&&url[0]=='/')
			str.EraseFromRight(1);
		str=str+url;
		LOG(TAG,"source url:%s  change to:%s",url.GetBuffer(),str.GetBuffer());
		url	=str;
	}
}

//从提取保存图片的相对路径
void	GetRelativePath(char* info,CMyString &relativePath,CMyString &url)
{
	bool getDir=false;
	while(*info==' ')info++;
	if(strstr(info,"alt=\"")==info)
	{
		info+=5;
		int		len=0;
		char*	s=info;
		while(*info!='\"'&&*info!=' '){len++;info++;}
		if(len)
		{
			relativePath	=CMyString::StringFromMem(s,0,len);
			getDir			=true;
		}
	}

	char *p=url.GetBuffer()+url.GetStrLen()-1;
	int	 len=0;
	
	CMyString fileName;
	CMyString selfDir;
	CMyString ext;
	getDir	=false;
	if(url.Match("[0-9,a-z]*/[0-9,a-z]*\.jpg",NULL,true)==-1)
	{
		LOG(TAG,2,0,"the image url:%s is not right!",url.GetBuffer());
	}
	else
	{
		while(*p!='/')
		{
			p--;
			len++;
		}
		fileName	=CMyString::StringFromMem(p+1,0,len);
		if(!getDir)
		{
			p--;
			len=0;
			while(*p!='/')
			{
				p--;
				len++;
			}
			selfDir	=CMyString::StringFromMem(p+1,0,len);
		}
		if(!getDir)
		{
			relativePath	=selfDir;
		}
		relativePath	 ="Image\\"+relativePath+"\\";
		relativePath	+=fileName;
		LOG(TAG,"get dir:%s  url:%s",relativePath.GetBuffer(),url.GetBuffer());
	}
	return;
}

bool IsJPGUrl(CMyString &url)
{
	for(int i=url.GetStrLen()-1;i>=0;i--)
	{
		if(url[i]=='.'&&(url[i+1]=='j'||url[i+1]=='J'))return true;
		else if(url[i]=='/')break;
	}
	return false;
}
#define _TST



#ifdef _TST

int main(int argc,char *argv[])
{
	CMyThreadPool	threadPool(true);
	LOG("22","aGDADF");
	HttpAccess  asyncHttp;
	HttpAccess::InitalHttp();
	asyncHttp.SetTargetThreadPool(&threadPool);
	asyncHttp.Get("http://www.920mm.com/OnlyTease/201205/6750_19.html");

	int index=0;
	g_HashMap.AddItem(asyncHttp.GetCurrentUrl(),index);
	g_HashMap.AddItem(CMyString("aidjga4twetfewga"),index);
}

#else
int main(int argc, char* argv[])
{
	HttpAccess  asyncHttp;
	int index=0;
	CMyThreadPool	threadPool(true);

	HttpAccess::InitalHttp();
	asyncHttp.SetTargetThreadPool(&threadPool);
	asyncHttp.Get("http://www.920mm.com/OnlyTease/201205/6750_19.html");

	InitalHostList();

	//把host填入哈希表，防止形成递归访问
	g_HashMap.AddItem(asyncHttp.GetCurrentUrl(),index);
	WaitForSingleObject(asyncHttp.m_FinishNotify,-1);

	//初始化过滤规制
	InitalRuleList();

	g_SlefPath	=CMyString::GetSlefPath();

	if(asyncHttp.GetHttpState()==CMyAsyncHttp::HTTP_FINISH)
	{
		//用于保存抓取的文本网页的内容
		CMyTempFile *lTempData1=new CMyTempFile(),*lTempData2=new CMyTempFile();
		CMyTempFile *lTempData=NULL;
		TrackUrl	*trackUrl1=new TrackUrl(),*trackUrl2=new TrackUrl();
		TrackUrl	*trackUrl=NULL;
		CMyString   relativePath;
		char*		base=asyncHttp.GetReceiveData();;

		while(!lTempData1->IsUsable())lTempData1->ReCreate();
		while(!lTempData2->IsUsable())lTempData2->ReCreate();

		CMyRegex regex;
		char *p;
		HttpAccess   http[HTTP_COUNT];
		HANDLE		 notify[HTTP_COUNT];
		CMyString	 httpPath;
		for(int i=0;i<HTTP_COUNT;i++)
		{
			http[i].SetTargetThreadPool(&threadPool);
			notify[i]=http[i].m_FinishNotify;
		}
		trackUrl1->ClearTrackInfo();
		trackUrl1->RecordTrackInfo(asyncHttp.GetCurrentUrl(),asyncHttp.GetReceiveDataLen());

		do
		{
			regex.SetIgnoreCareLess(true);

			if(!index)
			{
				if(asyncHttp.GetReceiveDataLen()==0)break;
				regex.SetMainString(asyncHttp.GetReceiveData(),asyncHttp.GetReceiveDataLen());
			}
			else
			{
				if(lTempData1->GetFileSize()==0)break;
				regex.SetMainString(lTempData1->GetMapping(),lTempData1->GetFileSize());
			}

			p=regex.Match("(href=|src=)\".*?[\",;]");
			
			
			while(p)
			{
				httpPath=CMyString::StringFromMem(p,0,regex.GetMatchStrLen());
				if(httpPath[0]=='h')
					httpPath.Erase(0,6);
				else
					httpPath.Erase(0,5);

				httpPath.EraseFromRight(1);
				
				//进行第一次简单url过滤
				if(!SampleFilter(httpPath))
				{
					LOG(TAG,"skip url:%s",httpPath.GetBuffer());
					p=regex.MatchNext();
					continue;
				}
				
				//修正url，包括补全相对url
				ModifyUrl(httpPath,(unsigned)(p-base),trackUrl1);

				//如果新网址访问过或者不符合过滤规则则跳过
				if(g_HashMap.GetItem(httpPath,&index)!=-1||!FilterUrl(httpPath))
				{
					LOG(TAG,"skip url:%s",httpPath.GetBuffer());
					p=regex.MatchNext();
					continue;
				}
				//访问过的网址添加入hash表中防止重复访问
				g_HashMap.AddItem(httpPath,index);
				if(IsJPGUrl(httpPath))
				{
					printf("------%s\n",httpPath.GetBuffer());
				}
				while(1)
				{
					for(int i=0;i<HTTP_COUNT;i++)
					{
						if(http[i].IsIdle())
						{
							if(http[i].GetHttpState()==CMyAsyncHttp::HTTP_FINISH&&http[i].GetCurrentUrl()!="")
							{
								if(http[i].IsTxtPage())
								{
									//为文本网页则保存数据进行并进行第二次抓取
									http[i].WriteFile(lTempData2);
									trackUrl2->RecordTrackInfo(http[i].GetCurrentUrl(),http[i].GetReceiveDataLen());
								}
								else
								{
									LOG(TAG,"-----------find image url:%s",http[i].GetCurrentUrl().GetBuffer());
									//为大图数据，则保存图片
									GetRelativePath(p+regex.GetMatchStrLen(),relativePath,http[i].GetCurrentUrl());
									http[i].SaveFile(relativePath);
								}
							}
							http[i].Get(httpPath.GetBuffer());
							printf("request:%s\n",httpPath.GetBuffer());
							break;
						}
					}
					if(i>=HTTP_COUNT)
					{
						WaitForMultipleObjects(HTTP_COUNT,notify,false,-1);
					}
					else
					{
						break;
					}
				}
				p=regex.MatchNext();
			}

		lTempData1->ReleaseMapping();
		lTempData1->Seek(CMyFile::SEEK_START,0);
		lTempData=lTempData1;
		lTempData1	=lTempData2;
		lTempData2	=lTempData;
		base		=lTempData1->GetMapping();

		trackUrl	=trackUrl1;
		trackUrl1	=trackUrl2;
		trackUrl2	=trackUrl;
		trackUrl2->ClearTrackInfo();
		index=1;

		trackUrl1->PrintTrackInfo();
		LOG(TAG,"finish a recyle!");
		}while(1);

		delete lTempData1;
		delete lTempData2;
	}
	UnInitalRuleList();
	getchar();
	return 0;
}
#endif