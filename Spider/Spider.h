#include "../MyLib.h"
#include "SpiderThread.h"
#include "SpiderVirtualClass.h"

class Spider  
{
public:
	Spider();
	virtual ~Spider();

	int 	SetSpiderMode(SpiderThread::SpiderMode mode);          //设置爬取模式（深度或广度）
	int		AddUrlFilter(SpiderUrlFilter *urlFilter);//添加url过滤规则
	int		SetErrorNotify(SpiderErrorNotify* errorNotify); //设置错误提醒处理类
	int		SetPageProcessMethod(SpiderPageProcess* processMethod);//设置数据处理类
	int		SetFileProcessMethod(SpiderFileProcess* processMethod);//设置数据处理类
	int		SetUrlModifyRule(SpiderUrlModify* urlModify);//设置url修改类
	int		SetPageUrlSort(SpiderUrlListSort* urlListSort);//设置对页面url的排序规则类
	int		SetMaxThread(int count);

	void	StartSpider(char *url);
	void    EndSpider();

private:
	SpiderThread		m_SpiderThread;
};
