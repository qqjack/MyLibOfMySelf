#include "SpiderHttp.h"

CMyString SpiderHttp::s_TxtPageExt[]=
{
	CMyString("jsp"),
	CMyString("html"),
	CMyString("php"),
	CMyString("asp"),
	CMyString("mhtm"),
	CMyString("htm")
};

SpiderHttp::SpiderHttp():CMyAsyncHttp("Mozilla/5.0 (compatible; iaskspider/1.0; MSIE 6.0)")
{
	m_Mark	=false;
}

SpiderHttp::~SpiderHttp()
{

}

bool SpiderHttp::IsTxtPage(CMyString& ext)
{
	int size=sizeof(s_TxtPageExt)/sizeof(CMyString);
	for(int i=0;i<size;i++)
	{
		if(ext.CompareI(s_TxtPageExt[i]))
		{
			return true;
		}
	}
	return false;
}

bool SpiderHttp::IsTxtPage()
{
	char *p=m_Url.GetBuffer()+m_Url.GetStrLen()-1;
	int len=0;
	while(p!=m_Url.GetBuffer())
	{
		if(*p=='.'||*p=='/')break;
		p--;
		len++;
	}
	char *pt=p;
	while(len&&pt!=m_Url.GetBuffer()&&*pt!='/')pt--;
	if(len&&*p=='.'&&*pt=='/')
	{
		CMyString ext=CMyString::StringFromMem(p+1,0,len);
		int size=sizeof(s_TxtPageExt)/sizeof(CMyString);
		for(int i=0;i<size;i++)
		{
			if(ext.CompareI(s_TxtPageExt[i]))
			{
				return true;
			}
		}
		return false;
	}
	return true;
	
}

void SpiderHttp::Get(char *url)
{
	SetAcceptType("*/*");
	AddHttpHeader("Accept-Charset: utf-8\r\n");
	AddHttpHeader("Accept-Encoding: gzip, deflate\r\n");
	CMyAsyncHttp::Get(url,"");
	m_Url	=url;
}