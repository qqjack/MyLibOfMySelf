#include "SpiderHttp.h"

SpiderHttp::SpiderHttp():CMyAsyncHttp("Mozilla/5.0 (compatible; iaskspider/1.0; MSIE 6.0)")
{
}

SpiderHttp::~SpiderHttp()
{

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
	if(len)
	{
		CMyString ext=CMyString::StringFromMem(p+1,0,len);
		if(ext.CompareI("jsp")||ext.CompareI("asp")||ext.CompareI("php")
			||ext.CompareI("html"))
		{
			return true;
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