#include "Spider.h"

Spider::Spider()
{
}

Spider::~Spider()
{
}

int Spider::SetSpiderMode(SpiderThread::SpiderMode mode)
{
	return m_SpiderThread.SetSpiderMode(mode);
}

int	Spider::AddUrlFilter(ISpiderUrlFilter *urlFilter)
{
	return m_SpiderThread.AddUrlFilter(urlFilter);
}

int	Spider::SetErrorNotify(ISpiderErrorNotify* errorNotify)
{
	return m_SpiderThread.SetErrorNotify(errorNotify);
}

int	Spider::SetPageProcessMethod(ISpiderPageProcess* processMethod)
{
	return m_SpiderThread.SetPageProcessMethod(processMethod);
}

int	Spider::SetFileProcessMethod(ISpiderFileProcess* processMethod)
{
	return m_SpiderThread.SetFileProcessMethod(processMethod);
}

int	Spider::SetUrlModifyRule(ISpiderUrlModify* urlModify)
{
	return m_SpiderThread.SetUrlModifyRule(urlModify);
}

int	Spider::SetPageUrlSortFunc(UrlCmpFunc urlSortFunc)
{
	return m_SpiderThread.SetPageUrlSortFunc(urlSortFunc);
}

int	Spider::SetMaxThread(int count)
{
	return m_SpiderThread.SetMaxThread(count);
}

void Spider::StartSpider(char *url,char* regex)
{
	m_SpiderThread.SetFecthUrlRegex(regex);
	m_SpiderThread.Start((void*)url);
}

void Spider::EndSpider()
{
	m_SpiderThread.End();
}