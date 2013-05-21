#include "MyBuffer.h"

CMyBuffer::CMyBuffer()
{
	m_Mode	=BUFFER_ALLOC;
	m_Buf	=NULL;
	m_BufLen=0;
	m_Cursor=0;
}

CMyBuffer::~CMyBuffer()
{
	Free();
}

int CMyBuffer::read(char* buf,int len)
{
	int less=m_BufLen-m_Cursor;
	if(m_Buf&&m_BufLen&&less>0)
	{
		len=(less>len?len:less);
		memcpy(buf,&m_Buf[m_Cursor],len);
		m_Cursor+=len;
		return len;
	}
	return -1;
}

int CMyBuffer::write(char* buf,int len)
{
	int less=m_BufLen-m_Cursor;
	if(m_Buf&&m_BufLen&&less>0)
	{
		len=(less>len?len:less);
		memcpy(&m_Buf[m_Cursor],buf,len);
		m_Cursor+=len;
		return len;
	}
	return -1;
}

int CMyBuffer::seek(SEEK_MODE seekType,int offset)
{
	if(!m_Buf&&!m_BufLen)return -1;
	int tempCursor=m_Cursor;
	switch(seekType)
	{
	case SEEK_START:
		tempCursor	=offset;
		break;
	case SEEK_END:
		tempCursor	=m_BufLen+offset-1;
		break;
	case SEEK_CURRENT:
		tempCursor	+=offset;
		break;
	default:
		return -1;
	}
	if(tempCursor<0)return -1;
	if(tempCursor>=m_BufLen)
	{
		if(m_Mode==BUFFER_ALLOC)
		{
			m_Buf	=(char*)realloc(m_Buf,tempCursor+1);
			m_BufLen=tempCursor+1;
		}
		else
		{
			tempCursor=m_BufLen-1;
		}
	}
	m_Cursor	=tempCursor;
	return 1;
}

int	CMyBuffer::Alloc(int len)
{
	if(len==m_BufLen)return 1;
	Free();
	m_Buf	=(char*)malloc(len);
	if(m_Buf)
	{
		memset(m_Buf,0,len);
		m_Mode	=BUFFER_ALLOC;
		m_BufLen=len;
		return 1;
	}
	return -1;
}

int	CMyBuffer::Realloc(int len)
{
	m_Buf	=(char*)realloc(m_Buf,len);
	m_BufLen	=len;
	if(m_Buf==NULL)return -1;
	return 1;
}

void CMyBuffer::Free()
{
	if(m_Buf&&m_Mode==BUFFER_ALLOC)
	{
		free((void*)m_Buf);
		m_BufLen	=0;
	}
}

int	CMyBuffer::Attach(char* buf,int len)
{
	if(!buf||!len)return -1;
	Free();
	m_Buf	=buf;
	m_BufLen=len;
	m_Mode	=BUFFER_ATTACH;
	return 1;
}

int	CMyBuffer::Dettach(char* buf)
{
	if(m_Mode==BUFFER_ATTACH)
	{
		m_Buf	=NULL;
		m_BufLen=0;
		m_Mode	=BUFFER_ALLOC;
		return 1;
	}
	return -1;
}

char CMyBuffer::operator [](int index)
{
	return m_Buf[index];
}

void CMyBuffer::SetAt(int index,char value)
{
	m_Buf[index]=value;
}

char CMyBuffer::GetAt(int index)
{
	return m_Buf[index];
}