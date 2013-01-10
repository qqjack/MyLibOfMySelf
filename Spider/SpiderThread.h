#include "../MyLib.h"
#include "SpiderVirtualClass.h"

#define MAX_SPIDER_THREAD (30)
#define DEFAULT_SPIDER_THREAD (20)

class SpiderThread : public CMyThread  
{
public:
	typedef enum
	{
		SPIDER_DEPTH,
		SPIDER_BREAD
	}SpiderMode;

private:
	typedef struct
	{
		CMyString	iParentUrl;  //从哪个网页中提取的url
		CMyString	iUrl;        
	}FailUrlInfo;

public:
	SpiderThread();
	virtual ~SpiderThread();
	virtual int Run(void *param);

	int			SetSpiderMode(SpiderMode mode);          //设置爬取模式（深度或广度）
	int			AddUrlFilter(SpiderUrlFilter *urlFilter);//添加url过滤规则
	int			SetErrorNotify(SpiderErrorNotify* errorNotify); //设置错误提醒处理类
	int			SetPageProcessMethod(SpiderPageProcess* processMethod);//设置数据处理类
	int			SetFileProcessMethod(SpiderFileProcess* processMethod);//设置数据处理类
	int			SetUrlModifyRule(SpiderUrlModify* urlModify);//设置url修改类
	int			SetPageUrlSort(SpiderUrlListSort* urlListSort);//设置对页面url的排序规则类
	int			SetMaxThread(int count);
	void		End();

private:
	typedef CMyHashMap<CMyString,void*> FirstHashMap;
	typedef CMyHashMap<CMyString,CMyString> SecondHashMap;
private:

	SpiderMode			m_SpiderMode;
	SpiderErrorNotify*	m_ErrorNotify;
	SpiderPageProcess*	m_PageProcess;
	SpiderFileProcess*	m_FileProcess;
	SpiderUrlModify*	m_UrlModify;
	SpiderUrlListSort*	m_UrlListSort;
	std::vector<SpiderUrlFilter*>	m_UrlFilterList;

	CMyAsyncHttp		m_Http[MAX_SPIDER_THREAD];
	FirstHashMap		m_MainHashMap;
	CMyThreadPool		m_ThreadPool;

	std::vector<FailUrlInfo>	m_FailList;  //请求失败url信息表,用于进行再次尝试
};
