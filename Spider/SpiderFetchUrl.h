#ifndef __SPIDER_FETCHURL__
#define __SPIDER_FETCHURL__
#include "../MyLib.h"

class ISpiderFetchUrl  
{
public:
	ISpiderFetchUrl();
	virtual ~ISpiderFetchUrl();
	int			 FetchUrl(char* context,int contextLen,bool regexSame=false);
	int			 GetUrlCount();
	CMyString*	 GetUrl(int index);

private:
	//url为根据正则表达式提取到的有效url
	//context为spider中指派提取url正则表达式提取到达内容
	//返回值决定是否对这一url进行进一步处理(包括url补全(仅对相对url),对此url进行过滤
	//对此url进行网页提取。。)
	virtual bool onFetchUrl(const char* url,const char* context,int contextLen){return true;}
	bool		 GetUrl();
	void		 AddUrl();
	void		 InitalRegex(char* data,int dataLen);

private:
	CMyString		m_TempUrl;
	char*			m_CurrentP;
	char*			m_Context;
	int				m_ContextLen;
	int				m_UrlCount;
	std::vector<CMyString*>	m_TempList;
	CMyRegex		m_Regex;
};
#endif