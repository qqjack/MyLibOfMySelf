#include "Url.h"


CUrl::CUrl()
{

}

CUrl::CUrl(char* url)
{
	SetUrl(CMyString(url));
}

CUrl::CUrl(CMyString &url)
{
	SetUrl(url);
}

CUrl::~CUrl()
{

}

void CUrl::SetUrl(CMyString &url)
{
	m_Host	=CUrl::GetUrlHost(url);
	m_Path	=CUrl::GetUrlPath(url);
	m_FileName=CUrl::GetFileName(url);
	m_Url	=url;
}

CMyString CUrl::GetUrlHost(CMyString &url)
{
	int start=0;
	int end=0;
	char *p;
	if(url.GetStrLen()>4&&url[0]=='h'&&url[1]=='t'&&url[2]=='t'&&
		url[3]=='p')
	{
		start+=7;
	}
	p=url.GetBuffer()+start;
	while(*p&&*p!='/')p++;
	end=p-url.GetBuffer()-1;
	return url.GetSubStr(start,end);
}

CMyString CUrl::GetUrlPath(CMyString &url)
{
	int start=0;
	int end=0;
	char *p;
	if(url.GetStrLen()>4&&url[0]=='h'&&url[1]=='t'&&url[2]=='t'&&
		url[3]=='p')
	{
		start+=7;
	}
	p=url.GetBuffer()+start;
	while(*p&&*p!='/')p++;
	if(*p)
	{
		if(*(p+1))
		{
			start	=p-url.GetBuffer()+1;
			CMyString str=url.GetSubStr(start);
			p=str.GetBuffer()+str.GetStrLen()-1;
			int i=0;
			while(*p!='.'&&*p!='/'&&p!=str.GetBuffer()){p--;i++;}
			if(*p=='.')
			{
				p--;
				i++;
				while(*p!='.'&&*p!='/'&&p!=str.GetBuffer()){p--;i++;}
				str.EraseFromRight(i);
			}
			return str;
		}
	}
	return CMyString();
}

CMyString CUrl::GetFileName(CMyString &url)
{
	char *p=url.GetBuffer()+url.GetStrLen()-1;
	char *e=p;
	while(*p!='/'&&*p!='.'&&p!=url.GetBuffer())p--;
	if(*p=='.')
	{
		while(*p!='/'&&p!=url.GetBuffer())p--;
		if(*p=='/'&&*(p-1)!='/')
		{
			return  url.GetSubStr(p-url.GetBuffer()+1);
		}
	}
	return CMyString();
}
