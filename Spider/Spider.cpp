#include "Spider.h"

Spider::Spider()
{
}

Spider::~Spider()
{
}

SpiderInterfaceConfig*	Spider::GetSpiderInterfaceConfig()
{
	return m_SpiderThread.GetSpiderInterfaceConfig();
}

int Spider::SetSpiderMode(SpiderMode mode)
{
	return m_SpiderThread.SetSpiderMode((SpiderThread::SpiderMode)mode);
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