#include "../MyLib.h"
#include "SpiderThread.h"
#include "SpiderVirtualClass.h"
#define  HTTP_REGEX "(href=|src=)\".*?[\",;]"

class Spider  
{
public:
	Spider();
	virtual ~Spider();

	int 	SetSpiderMode(SpiderThread::SpiderMode mode);          //设置爬取模式（深度或广度）
	int		AddUrlFilter(ISpiderUrlFilter *urlFilter);//添加url过滤规则
	int		SetErrorNotify(ISpiderErrorNotify* errorNotify); //设置错误提醒处理类
	int		SetPageProcessMethod(ISpiderPageProcess* processMethod);//设置数据处理类
	int		SetFileProcessMethod(ISpiderFileProcess* processMethod);//设置数据处理类
	int		SetUrlModifyRule(ISpiderUrlModify* urlModify);//设置url修改类
	int		SetPageUrlSortFunc(UrlCmpFunc urlSortFunc);//设置对页面url的排序规则类
	int		SetMaxThread(int count);

	//url为开始抓取网页Url,regex为提取网页url的正则表达式
	//(引擎会根据此正则表达式提取的内容，从中提取有效合法url)
	void	StartSpider(char *url,char* regex=HTTP_REGEX);
	void    EndSpider();

private:
	SpiderThread		m_SpiderThread;
};
