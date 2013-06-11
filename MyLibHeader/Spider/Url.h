#ifndef __URL__
#define __URL__

#include "../MyString.h"

class CUrl  
{
public:
	CUrl();
	CUrl(char *url);
	CUrl(CMyString &url);
	virtual ~CUrl();
	
	CMyString GetUrlHost(){return m_Host;}
	CMyString GetUrlPathFile(){return m_Path+m_FileName;}
	CMyString GetUrlPath(){return m_Path;}
	CMyString GetFileName(){return m_FileName;}
	void	  SetUrl(CMyString &url);

	static CMyString GetUrlHost(CMyString &url);
	static CMyString GetUrlPath(CMyString &url);
	static CMyString GetFileName(CMyString &url);


private:
	CMyString m_Url;
	CMyString m_Path;
	CMyString m_Host;
	CMyString m_FileName;
};
#endif