#include "MyString.h"
#include "stdio.h"
#include "windows.h"
#include <math.h>

#define EXTRA_SIZE  (100)

CMyString::CMyString()
{
	Inital();
}

CMyString::~CMyString()
{
	UInital();
}

CMyString::CMyString(char *str)
{
	Inital();
	copy(str);
}

CMyString::CMyString(int number)
{
	Inital();
	*this=number;
}

CMyString::CMyString(double number)
{
	Inital();
	*this=number;
}

CMyString::CMyString(const CMyString &str)
{
	Inital();
	CMyString *pStr=(CMyString*)&str;
	copy(pStr->GetBuffer());
}

void CMyString::ToLowerCase()
{
	for(int i=0;i<m_StrLen;i++)
	{
		if(m_Buffer[i]>='A'&&m_Buffer[i]<='Z')
			m_Buffer[i]=m_Buffer[i]+('a'-'A');
	}
}

void CMyString::ToUpcase()
{
	for(int i=0;i<m_StrLen;i++)
	{
		if(m_Buffer[i]>='a'&&m_Buffer[i]<='z')
			m_Buffer[i]=m_Buffer[i]-('a'-'A');
	}
}

CMyString CMyString::GetSubStr(int start)
{
	CMyString str;
	str.copy(&m_Buffer[start]);
	return str;
}

CMyString CMyString::GetSubStr(int start,int end)
{
	CMyString str;
	char *buf=new char[end-start+2];
	memcpy(buf,&m_Buffer[start],end-start+1);
	buf[end-start+1]=0;
	str.copy(buf);
	delete buf;
	return str;
}

CMyString& CMyString::operator =(char* str)
{
	copy(str);
	return *this;
}

CMyString& CMyString::operator =(CMyString& str)
{
	copy(str.GetBuffer());
	return *this;
}

CMyString& CMyString::operator =(int number)
{
	if(MAX_SIZE<50)
	{
		Resize(MAX_SIZE+50);
	}
	sprintf(m_Buffer,"%d",number);
	m_StrLen	=strlen(m_Buffer);
	return *this;
}

CMyString& CMyString::operator =(double	number)
{
	if(MAX_SIZE<50)
	{
		Resize(MAX_SIZE+50);
	}
	sprintf(m_Buffer,"%.20f",number);
	m_StrLen	=strlen(m_Buffer);
	return *this;
}

CMyString& CMyString::operator +=(char* str)
{
	append(str);
	return *this;
}

CMyString& CMyString::operator +=(CMyString& str)
{
	*this+=str.GetBuffer();
	return *this;
}

bool CMyString::operator ==(char* str)
{
	return strcmp(m_Buffer,str)==0;
}

bool CMyString::operator ==(CMyString& str)
{
	return *this==str.GetBuffer();
}

bool CMyString::operator !=(char* str)
{
	return !(*this==str);
}

bool CMyString::operator !=(CMyString& str)
{
	return !(*this==str);
}

char CMyString::operator [](int index)
{
	return m_Buffer[index];
}

CMyString CMyString::operator +(CMyString& str)
{
	return *this+=str;
}

CMyString CMyString::operator +(char* str)
{
	return *this+=str;
}

CMyString::operator char *()
{
	return m_Buffer;
}

void CMyString::Inital()
{
	m_Buffer=(char*)malloc(MAX_SIZE);
	memset(m_Buffer,0,MAX_SIZE);
	m_BufLen=MAX_SIZE;
	m_StrLen=0;
}

void CMyString::UInital()
{
	if(m_Buffer)
	{
		free((void*)m_Buffer);
		m_StrLen=0;
		m_BufLen=0;
	}
}

void CMyString::append(char* str)
{
	int len=strlen(str);
	if(len+m_StrLen>=m_BufLen)
	{
		int size=len+m_StrLen-m_BufLen;
		int n=(size%ADD_STEP)?(size/ADD_STEP+1):(size/ADD_STEP);
		m_Buffer=(char*)realloc(m_Buffer,m_BufLen+ADD_STEP*n+1);
		m_BufLen+=ADD_STEP*n+1;
	}
	m_StrLen+=len;
	strcat(m_Buffer,str);
}

void CMyString::copy(char *str)
{
	int len=strlen(str);
	if(len>=m_BufLen)
	{
		int size=len-m_BufLen;
		int n=size%ADD_STEP?(size/ADD_STEP+1):(size/ADD_STEP);
		m_Buffer=(char*)realloc(m_Buffer,m_BufLen+ADD_STEP*n+1);
		m_BufLen+=ADD_STEP*n+1;
	}
	m_StrLen=len;
	strcpy(m_Buffer,str);
}

void CMyString::Resize(int size)
{
	if(size>m_BufLen)
	{
		m_Buffer=(char*)realloc(m_Buffer,size);
		m_BufLen=size;
	}
}

int CMyString::ToInt()
{
	return atol(m_Buffer);
}

int	CMyString::ToDouble()
{
	return atof(m_Buffer);
}

int	CMyString::CompareI(char *str)
{
	CMyString str1,str2;
	str1=*this;
	str2=str;
	str1.ToUpcase();
	str2.ToUpcase();
	return str1==str2;
}

int	CMyString::CompareI(CMyString &str)
{
	return CompareI(str.GetBuffer());
}

int CMyString::GetStrLen()
{
	return 	m_StrLen=strlen(m_Buffer);
}

int CMyString::Replace(char *str,char *newStr, int startOffset)
{
	int strLen1  =strlen(str);
	int strLen2	 =strlen(newStr);
	int newSize  =0;
	int offset	 =strLen1-strLen2;
	int moveSize =0;
	char *p=NULL;

	p	= strstr((const char*)(m_Buffer+startOffset),str);
	if(!p)  return -1;
	
	moveSize=GetStrLen()-(p-m_Buffer+strLen1);

	if(strLen1<strLen2&&(m_BufLen<GetStrLen()+strLen2-strLen1+1))
	{
		newSize=m_StrLen+strLen2-strLen1+EXTRA_SIZE;
		Resize(newSize);
	}

	if(offset!=0)
	{
		memmove(&p[strLen1-offset],&p[strLen1],moveSize+1);
	}
	memcpy(p,newStr,strLen2);
	return p-m_Buffer;
}

int CMyString::ReplaceAll(char *str,char *newStr)
{
	int strLen  =strlen(newStr);
	int r		 =Replace(str,newStr,0);
	if(r<0)return r;
	while(r>=0)
	{
		r=Replace(str,newStr,r+strLen);
	}
	return 1;
}


int	CMyString::Match(char *regex,CMyString *outStr,bool ignoreCareLess,int option)
{
	char *p  =NULL;

	m_Regex.SetMainString(m_Buffer,GetStrLen());
	m_Regex.SetIgnoreCareLess(ignoreCareLess);
	p  = m_Regex.Match(regex,option);

	if(p&&outStr)
	{
		*outStr  =CMyString::StringFromMem(p,0,m_Regex.GetMatchStrLen());
	}
	
	return (p==NULL?-1:1);
}

int	CMyString::MatchNext(CMyString *outStr,int option)
{
	char *p  =NULL;

	p = m_Regex.MatchNext(option);
	if(p&&outStr)
	{
		*outStr  =CMyString::StringFromMem(p,0,m_Regex.GetMatchStrLen());
	}
	
	return (p==NULL?-1:1);
}

CMyString CMyString::StringFromMem(char *str,int start,int len)
{
	CMyString lStr;
	if(lStr.GetBufferSize()<len+1)
		lStr.Resize(len+1);
	memcpy(lStr.GetBuffer(),str,len);
	lStr.GetBuffer()[len]=0;
	return lStr;
}

CMyString CMyString::Format(const char *format,...)
{
	char buf[MAX_FORMATE_LEN+1];
	CMyString lStr;
	va_list arglist;
	va_start(arglist,format);
	_vsnprintf(buf,MAX_FORMATE_LEN,format,arglist);
	va_end(arglist);
	buf[MAX_FORMATE_LEN]=0;
	lStr	=buf;
	return lStr;
}

CMyString CMyString::GetSlefPath()
{
	CMyString str;
	char buf[1000];
	::GetSlefPath(buf,1000);
	return CMyString(buf);
}

CMyString CMyString::FromUnicode(char *unicodeBuf)
{
	CMyString str;
	int bufLen	=::wcslen((const unsigned short*)unicodeBuf);
	char *buf	=new char[bufLen+1];
	buf[bufLen]=0;
	WideCharToMultiByte(CP_ACP, 0, (const unsigned short*)unicodeBuf, -1, buf, bufLen, NULL, NULL);
	str	=buf;
	delete buf;
	return str;
}

int	CMyString::FindChar(char c)
{
	char str[2];
	str[0]=c;
	str[1]=0;
	return FindString(str);
}

int	CMyString::FindString(char *str)
{
	char *p;
	p	=strstr(m_Buffer,str);
	if(p)
	{
		return p-m_Buffer;
	}
	return -1;
}

CMyString operator +(char * str1,CMyString& str2)
{
	CMyString r=str1;
	return r+str2;
}

unsigned CMyString::HashCode()
{
	int len	=GetStrLen();
	unsigned r=0;
	for(int i=0;i<len;i++)
	{
		r+=m_Buffer[i]|i;
	}
	return r;
}

int	CMyString::Erase(int start,int count)
{
	int strL=GetStrLen();
	if(start+count>strL)return -1;
	if(start+count==strL)
	{
		m_Buffer[start]=0;
		return 1;
	}
	memmove( m_Buffer+start, m_Buffer+start+count,strL-start-count);
	m_Buffer[strL-count]=0;
	return 1;
}

int CMyString::EraseFromRight(int count)
{
	int strL=GetStrLen();
	if(count>strL)return -1;
	m_Buffer[strL-count]=0;
	return 1;
}

void CMyString::SetAt(int index,char c)
{
	m_Buffer[index]=c;
}

char CMyString::GetAt(int index)
{
	return m_Buffer[index];
}