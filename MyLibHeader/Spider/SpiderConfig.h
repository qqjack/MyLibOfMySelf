#ifndef __SPIDER_CONFIG__
#define __SPIDER_CONFIG__
#include "../MyDebug.h"
#include "SpiderVirtualClass.h"
#include "SpiderFetchUrl.h"

class SpiderInterfaceConfig  
{
	class DefaultSpiderFinish:public ISpiderFinish
	{
	public:
		virtual void OnFinish()
		{
			CMyDebug::Log("Notify",2,0,"Spider have finish!!");
		}
	};

public:
	SpiderInterfaceConfig();
	virtual ~SpiderInterfaceConfig();
	
	int		AddUrlFilter(ISpiderUrlFilter *urlFilter);//添加url过滤规则
	int		SetErrorNotify(ISpiderErrorNotify* errorNotify); //设置错误提醒处理类
	int		SetPageProcessMethod(ISpiderPageProcess* processMethod);//设置数据处理类
	int		SetFileProcessMethod(ISpiderFileProcess* processMethod);//设置数据处理类
	int		SetUrlModifyRule(ISpiderUrlModify* urlModify);//设置url修改类

	int		SetSpiderFinishNotify(ISpiderFinish* finishNotify);
	friend class SpiderThread;

private:
	void	Lock(){m_IsLock	=true;}
	void	UnLock(){m_IsLock=false;}

private:
	ISpiderErrorNotify*	m_ErrorNotify;
	ISpiderPageProcess*	m_PageProcess;
	ISpiderFileProcess*	m_FileProcess;
	ISpiderUrlModify*	m_UrlModify;
	ISpiderFetchUrl*	m_FetchUrl;
	std::vector<ISpiderUrlFilter*>	m_UrlFilterList;
	ISpiderFinish*		m_SpiderFinish;

	ISpiderFetchUrl		m_InnerFetchUrl;
	DefaultSpiderFinish	m_InnerSpiderFinish;
	bool				m_IsLock;
};

#endif