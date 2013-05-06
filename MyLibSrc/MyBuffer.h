#ifndef __MY_BUFFER__
#define __MY_BUFFER__

#include "MyIOStream.h"
#include "stdio.h"
#include "windows.h"

class CMyBuffer:public CMyIOStream
{
private:
	enum BUFFER_MODE
	{
		BUFFER_ALLOC,   //此buffer为调用alloc函数取得空间的，需要释放
		BUFFER_ATTACH   //此buffer是attach的空间，无需释放
	};
public:
	CMyBuffer();
	virtual ~CMyBuffer();
	
	virtual int read(char* buf,int len);
	virtual int write(char* buf,int len);
	virtual int seek(SEEK_MODE seekType,int offset);

	int			Alloc(int len);
	int			Realloc(int len);
	void		Free();
	int			Attach(char* buf,int len);
	int			Dettach(char* buf);

	char*		GetBuffer(){return m_Buf;}
	int 		GetBufferLen(){return m_BufLen;}

	char		operator [](int index);
private:

	BUFFER_MODE		m_Mode;
	char*			m_Buf;
	int				m_BufLen;
	int				m_Cursor;
};
#endif