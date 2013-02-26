#include "SpiderFetchUrl.h"

#define  HTTP_REGEX "(href=|src=)\".*?[\",;]"

ISpiderFetchUrl::ISpiderFetchUrl()
{
	m_Context	=NULL;
	m_ContextLen=0;
	m_CurrentP	=NULL;
	m_UrlCount	=0;
}

ISpiderFetchUrl::~ISpiderFetchUrl()
{
	int size=m_TempList.size();
	for(int i=0;i<size;i++)
		delete m_TempList[i];
	m_TempList.clear();
}

void ISpiderFetchUrl::InitalRegex(char* data,int dataLen)
{
	m_Regex.SetMainString(data,dataLen);
	m_CurrentP	=m_Regex.Match(HTTP_REGEX);
}

//返回值提取到的有效url个数
int	ISpiderFetchUrl::FetchUrl(char* context,int contextLen,bool regexSame)
{
	//提取context中的有效url
	m_Context	=context;
	m_ContextLen=contextLen;
	m_CurrentP	=m_Context;
	m_UrlCount	=0;
	if(!regexSame)
	{
		InitalRegex(context,contextLen);
		while(GetUrl())
		{
			if(this->onFetchUrl(m_TempUrl.GetBuffer(),context,contextLen))
			{
				AddUrl();
			}
		}
	}
	else
	{
		m_TempUrl	=CMyString::StringFromMem(m_CurrentP,0,contextLen);
		//去除结尾"
		m_TempUrl.EraseFromRight(1);
		//去除href="和src="
		if(m_TempUrl[0]=='h')
			m_TempUrl.Erase(0,6);
		else
			m_TempUrl.Erase(0,5);
		//去除http://
		if(m_TempUrl.FindString("http://")!=-1)
			m_TempUrl.Erase(0,7);
		int len=m_TempUrl.GetStrLen();
		
		m_TempUrl.Trim();
		AddUrl();
	}
	return GetUrlCount();
}

bool ISpiderFetchUrl::GetUrl()
{
	if(!m_CurrentP)return false;
	m_TempUrl	=CMyString::StringFromMem(m_CurrentP,0,m_Regex.GetMatchStrLen());
	//去除结尾"
	m_TempUrl.EraseFromRight(1);
	//去除href="和src="
	if(m_TempUrl[0]=='h')
		m_TempUrl.Erase(0,6);
	else
		m_TempUrl.Erase(0,5);
	//去除http://
	if(m_TempUrl.FindString("http://")!=-1)
		m_TempUrl.Erase(0,7);
	int len=m_TempUrl.GetStrLen();
	
	m_TempUrl.Trim();
	m_CurrentP	=m_Regex.MatchNext();
	return true;
}

CMyString* ISpiderFetchUrl::GetUrl(int index)
{
	if(index>=m_TempList.size()||index<0)return NULL;
	return m_TempList[index];
}

void ISpiderFetchUrl::AddUrl()
{
	if(m_UrlCount<m_TempList.size())
	{
		*m_TempList[m_UrlCount++]=m_TempUrl;
	}
	else
	{
		m_TempList.push_back(new CMyString(m_TempUrl));
		m_UrlCount++;
	}
}

int ISpiderFetchUrl::GetUrlCount()
{
	return m_UrlCount;
}