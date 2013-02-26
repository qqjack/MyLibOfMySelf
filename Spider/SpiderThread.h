#ifndef __SPIDER_THREAD__
#define	__SPIDER_THREAD__

#include "../MyLib.h"
#include "SpiderVirtualClass.h"
#include "Url.h"
#include "SpiderHttp.h"
#include "SpiderFetchUrl.h"
#include "SpiderConfig.h"

#define MAX_SPIDER_THREAD (30)
#define DEFAULT_SPIDER_THREAD (20)

typedef bool (*UrlCmpFunc)(CMyString *str1,CMyString *str2);

static bool DefaultUrlSortCmpFun(CMyString *str1,CMyString *str2)
{
	return strcmp(str1->GetBuffer(),str2->GetBuffer())>0;
}

class SpiderThread : public CMyThread  
{
public:
	typedef enum
	{
		SPIDER_DEPTH,
		SPIDER_BREAD
	}SpiderMode;

	class HashCode
	{
	public:
		unsigned operator()(const CMyString &obj)
		{
			return ((CMyString&)obj).HashCode();
		}
	};

private:
	typedef struct
	{
		CMyString	iParentUrl;  //从哪个网页中提取的url
		CMyString	iUrl;        
	}UrlInfo;

public:
	SpiderThread();
	virtual ~SpiderThread();
	virtual int Run(void *param);

	SpiderInterfaceConfig*	GetSpiderInterfaceConfig(){return &m_InterfaceConfig;}

	int			SetSpiderMode(SpiderMode mode);          //设置爬取模式（深度或广度）
	int			SetPageUrlSortFunc(UrlCmpFunc urlSortFunc);//设置对页面url的排序规则类
	int			SetFecthUrlRegex(char* regex);//设置提取url的正则表达式
	int			SetMaxThread(int count);
	void		End();

public:
	bool		GetNextUrl();
	void		AnalysisData(SpiderHttp* spiderHttp);
	bool		ErrorProcess(SpiderHttp* spiderHttp);
	void		AddHashMap(CMyString &host,CMyString &url);
	//返回值表示buf中是否存在url
	bool		InitalFetchEngine(char *buf,int bufLen);
	void		ReBuildUrlIfNeed(CMyString &parentUrl,CMyString &url,CMyString &host);
	bool		HaveAcess(CMyString &host,CMyString &url);
	void		AddTempUrlList(CMyString &url);
	void		SortTempUrlList();
	void		AddAllUrlToUrlList(CMyString &host);
	bool		FetchUrl(CMyString &url);
	void		ClearUrlList();
	void		RelocateUrl(SpiderHttp* spiderHttp);

	typedef CMyHashMap<CMyString,void*,HashCode> FirstHashMap;
	typedef FirstHashMap SecondHashMap;
private:

	SpiderMode			m_SpiderMode;
	CMyString			m_UrlRegex;
	CMyThreadPool		m_ThreadPool;
	SpiderHttp			m_Http[MAX_SPIDER_THREAD];
	unsigned			m_HttpCount;
	FirstHashMap		m_MainHashMap;

	std::vector<UrlInfo*>	m_FailList;  //请求失败url信息表,用于进行再次尝试
	std::vector<UrlInfo*>	m_UrlList;
	UrlInfo*				m_CurrentUrl;

	HANDLE				m_EndEvent;
	CMyRegex			m_Regex;
	char*				m_CurrentP;
	std::vector<CMyString*> m_TempList;
	UrlCmpFunc			m_UrlCmp;

	static char*		TAG;

	SpiderInterfaceConfig	m_InterfaceConfig;
	bool				m_SameRegex;
};
#endif