#include "../MyLib.h"
class SpiderHttp:public CMyAsyncHttp
{
public:
	SpiderHttp();
	virtual ~SpiderHttp();

	CMyString	GetCurrentUrl(){return m_Url;}
	void		WriteFile(CMyFile *file);
	bool		IsTxtPage();
	void		Get(char *url);
	void		SetParentUrl(CMyString &parentUrl){m_ParentUrl	=parentUrl;}
	CMyFile*	GetFile(){return GetRecevieFile();}
	void		SetMark(bool value){m_Mark=value;}
	bool		GetMark(){return m_Mark;}
	CMyString m_Url;
	CMyString m_ParentUrl;
private:
	bool	  m_Mark;

	static CMyString	s_TxtPageExt[];
};
