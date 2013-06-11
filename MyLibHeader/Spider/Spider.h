#include "SpiderThread.h"
#include "SpiderVirtualClass.h"
#define  HTTP_REGEX "(href=|src=)\".*?[\",;]"

class Spider  
{
public:
	typedef enum
	{
		SPIDER_DEPTH,
		SPIDER_BREAD
	}SpiderMode;

	Spider();
	virtual ~Spider();

	SpiderInterfaceConfig*	GetSpiderInterfaceConfig();
	int 	SetSpiderMode(SpiderMode mode);          //设置爬取模式（深度或广度）
	int		SetPageUrlSortFunc(UrlCmpFunc urlSortFunc);//设置对页面url的排序规则类
	int		SetMaxThread(int count);

	//url为开始抓取网页Url,regex为提取网页url的正则表达式
	//(引擎会根据此正则表达式提取的内容，从中提取有效合法url)
	void	StartSpider(char *url,char* regex=HTTP_REGEX);
	void    EndSpider();

private:
	SpiderThread		m_SpiderThread;
};
