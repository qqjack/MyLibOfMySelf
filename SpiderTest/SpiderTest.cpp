
#include "../MyLib.h"
#define HTTP_COUNT	(5)

char*					  TAG="Spider";
CMyString				  g_SlefPath;

//host规则,过滤外部链接
class MainUrlRule:public ISpiderUrlFilter
{
public:
	static CMyString		m_HostList[];
	static int				m_HostCount;
public:
	virtual bool FilterCheck(const char *parentUrl,const char *url);
};
int			MainUrlRule::m_HostCount=0;
CMyString	MainUrlRule::m_HostList[]={
	CMyString("www.920mm.com"),
	CMyString("pic.920mm.com"),
	CMyString("img.920mm.com")
};

bool MainUrlRule::FilterCheck(const char *parentUrl,const char *url)
{
	if(!m_HostCount)
		m_HostCount	=sizeof(m_HostList)/sizeof(CMyString);
	CMyString host=CUrl::GetUrlHost(CMyString((char*)url));
	for(int i=0;i<m_HostCount;i++)
	{
		if(host==m_HostList[i])
		{
			return true;
		}
	}
//	printf("skip url:%s\n",url);
	return false;
}

class ImageUrlRule:public ISpiderUrlFilter
{
public:
	virtual bool FilterCheck(const char *parentUrl,const char *url)
	{
		CMyString urlS=(char*)url;
		CMyString ext=CUrl::GetFileName(urlS);
		if(ext!="")
		{
			for(int i=ext.GetStrLen();i>=0;i--)
			{
				if(ext[i]=='.')break;
			}
			if(i!=0)
			{
				ext=ext.GetSubStr(i+1);
				if(ext=="jpg")
				{
//printf("give up link:%s\n",url);
					return false;
				}
			}
		}
		return true;
	}
};

class OnlyPageRule:public ISpiderUrlFilter
{
public:
	virtual bool FilterCheck(const char *parentUrl,const char *url)
	{
		CMyString urlS=(char*)url;
		CMyString ext=CUrl::GetFileName(urlS);
		if(ext!="")
		{
			for(int i=ext.GetStrLen();i>=0;i--)
			{
				if(ext[i]=='.')break;
			}
			if(i!=0)
			{
				ext=ext.GetSubStr(i+1);
				bool r= SpiderHttp::IsTxtPage(ext);
//				if(!r)
//					printf("skip url:%s\n",url);
				return r;
			}
		}
		return true;
	}
};

class ErrorNotify:public ISpiderErrorNotify
{
public:
	virtual	bool ErrorProcess(const char *parentUrl,const char* url,int error,char *data,int dataLen)
	{
		printf("---------error:%s\n",url);
		return false;
	}
};

class ImageSave:public ISpiderFileProcess
{
public:
	CMyString m_FilePath;
	CMyString m_TempStr;
	ImageSave()
	{
		m_FilePath=CMyString::GetSlefPath();
		m_FilePath+="\\Download\\";
	}

	void GetDirName(CMyString &path)
	{
		int len=path.GetStrLen();
		int start=0;
		for(int i=len-1;i>=0;i++)
		{
			if(path[i]=='/')
			{
				start=i+1;
				break;
			}
		}
		m_TempStr=path.GetSubStr(start);
		if(path[len-1]=='/')
			m_TempStr.EraseFromRight(1);
	}

public:
	virtual	void FileProcess(const char *parentUrl,const char* url,const char* fileData,int dataLen)
	{
		CMyString lUrl=(char*)url;
		CMyString path=CUrl::GetUrlPath(lUrl);
		CMyString name=CUrl::GetFileName(lUrl);
		if(path!=""&&name!="")
		{
			GetDirName(path);
			m_TempStr=m_FilePath+m_TempStr;
			CMyFile::CreateDir(m_TempStr.GetBuffer());
			m_TempStr+="\\"+name;
		}
	}
};


class DoPageProcess:public ISpiderPageProcess
{
public:
	virtual	void PageProcess(const char *parentUrl,const char* url,const char* data,int dataLen)
	{
		printf("Process:%s\n",url);
	}
};


//#define _TST

#ifdef _TST

int main(int argc,char *argv[])
{
//	CMyThreadPool	threadPool(true);
//	LOG("22","aGDADF");
//	HttpAccess  asyncHttp;
//	HttpAccess::InitalHttp();
//	asyncHttp.SetTargetThreadPool(&threadPool);
//	asyncHttp.Get("http://www.920mm.com/OnlyTease/201205/6750_19.html");
//
//	int index=0;
//	g_HashMap.AddItem(asyncHttp.GetCurrentUrl(),index);
//	g_HashMap.AddItem(CMyString("aidjga4twetfewga"),index);

//	CMyString parent("http://www.920mm.com/OnlyTease/201205/6750_19.html");
//	Spider spider;
//	spider.StartSpider(parent.GetBuffer());
//	Sleep(3000);
//	spider.EndSpider();
//	Sleep(3000000);

	CMyString str="\t\t\n\n\t\n343242  \n \t adf\r  ";
	str.Trim();
}

#else
int main(int argc, char* argv[])
{
	Spider			spider;
	SpiderInterfaceConfig*	interfaceConfig;

	interfaceConfig	=spider.GetSpiderInterfaceConfig();

	MainUrlRule		mainRule;
	ImageUrlRule	imageRule;
	OnlyPageRule	onlyPageRule;
	ImageSave		imageSave;
	DoPageProcess	pageProcess;
	ErrorNotify		errorNotify;

	interfaceConfig->AddUrlFilter(&mainRule);
	interfaceConfig->AddUrlFilter(&onlyPageRule);
	interfaceConfig->SetErrorNotify(&errorNotify);
//	interfaceConfig.SetFileProcessMethod(&imageSave);
//	interfaceConfig.SetPageProcessMethod(&pageProcess);

	spider.StartSpider("www.920mm.com/DGC");

	printf("输入任意字符退出爬虫。。。\n");
	getchar();
	spider.EndSpider();
}
#endif