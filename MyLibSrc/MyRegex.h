#ifndef __MY_REGEX__
#define __MY_REGEX__

#include "pcre.h"
#define MAX_VECTOR_SIZE  (30)


class CMyRegex  
{
public:
	CMyRegex();
	virtual ~CMyRegex();
	void		SetMainString(char* str,int strLen) {m_MainString=str;m_MainStrLen=strLen;}
	void		SetIgnoreCareLess(bool value) {m_IgnoreCareLess=value;}

	int			GetMatchStrLen() {return m_Vector[1]-m_Vector[0];}

	char *		Match(char *reStr,int option=0);
	char *		MatchNext(int option=0);
private:
	pcre*		m_Pcre;
	int			m_Vector[MAX_VECTOR_SIZE];
	char*		m_MainString;
	int			m_MainStrLen;
	bool		m_IgnoreCareLess;
	
	static char*   TAG;
};

#endif