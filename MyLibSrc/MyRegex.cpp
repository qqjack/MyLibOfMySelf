#include "MyRegex.h"
#include "MyDebug.h"

char* CMyRegex::TAG="CMyRegex";

#pragma   comment(lib, "pcre.lib")

CMyRegex::CMyRegex()
{
	m_Pcre=NULL;
}


CMyRegex::~CMyRegex()
{
	if(m_Pcre)
		free(m_Pcre);
}

char* CMyRegex::Match(char *reStr,int option)
{
	int			loption=0;
	char *		errorInfo=NULL;
	int			errorOffset=0;
	int			rCode=0;

	if(m_IgnoreCareLess)
		loption|=PCRE_CASELESS;

	loption|=PCRE_DOTALL;

	m_Pcre	= pcre_compile(reStr, loption ,(const char**) &errorInfo, &errorOffset, NULL);

	if(!m_Pcre)
	{
#ifdef _DEBUG
		if(errorInfo)
			LOG(TAG,"Error Message:%s",errorInfo);
#endif
		return NULL;
	}

	rCode = pcre_exec(m_Pcre, NULL, m_MainString, m_MainStrLen , 0, 
						 option, m_Vector, MAX_VECTOR_SIZE);

	if(rCode<0)
	{
#ifdef _DEBUG
		if(rCode==-1)
			LOG(TAG,"Have no match string!");
#endif
		return NULL;
	}

	return m_MainString + m_Vector[0];
}

char* CMyRegex::MatchNext(int option)
{
	int		rCode;

	rCode = pcre_exec(m_Pcre, NULL, m_MainString, m_MainStrLen , m_Vector[1], 
		option, m_Vector, MAX_VECTOR_SIZE);

	if(rCode<0)
	{
#ifdef _DEBUG
		if(rCode==-1)
			LOG(TAG,"Have no match string!");
#endif
		return NULL;
	}

	return m_MainString + m_Vector[0];
}