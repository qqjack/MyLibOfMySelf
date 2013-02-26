#include "SpiderConfig.h"


SpiderInterfaceConfig::SpiderInterfaceConfig()
{
	m_IsLock	=false;
	m_FetchUrl	=&m_InnerFetchUrl;
	m_SpiderFinish		=&m_InnerSpiderFinish;
	m_ErrorNotify		=NULL;
	m_PageProcess		=NULL;
	m_FileProcess		=NULL;
	m_UrlModify			=NULL;
}

SpiderInterfaceConfig::~SpiderInterfaceConfig()
{
}


int SpiderInterfaceConfig::AddUrlFilter(ISpiderUrlFilter *urlFilter)
{
	if(m_IsLock)return -1;
	m_UrlFilterList.push_back(urlFilter);
	return 1;
}

int SpiderInterfaceConfig::SetErrorNotify(ISpiderErrorNotify* errorNotify)
{
	if(m_IsLock)return -1;
	m_ErrorNotify	=errorNotify;
	return 1;
}

int SpiderInterfaceConfig::SetPageProcessMethod(ISpiderPageProcess* processMethod)
{
	if(m_IsLock)return -1;
	m_PageProcess	=processMethod;
	return 1;
}

int SpiderInterfaceConfig::SetFileProcessMethod(ISpiderFileProcess* processMethod)
{
	if(m_IsLock)return -1;
	m_FileProcess	=processMethod;
	return 1;
}

int SpiderInterfaceConfig::SetUrlModifyRule(ISpiderUrlModify* urlModify)
{
	if(m_IsLock)return -1;
	m_UrlModify		=urlModify;
	return 1;
}

int	SpiderInterfaceConfig::SetSpiderFinishNotify(ISpiderFinish* finishNotify)
{
	if(m_IsLock)return -1;
	m_SpiderFinish		=finishNotify;
	return 1;
}