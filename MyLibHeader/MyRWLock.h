#include "windows.h"
#ifndef __MY_RWLOCK__
#define __MY_RWLOCK__

#define MAX_READER (20)
class CMyRWLock  
{
public:
	CMyRWLock();
	virtual ~CMyRWLock();
	void	WaitRead();
	void	WaitWrite();
	void	Done();
	int		GetReaderNum();
	int		GetWriterNum();
private:
	int m_Active;            //当>0，表示当前读者数，<0有写入者，0空闲
	int m_WaitReader;        //当前等待的读者数目
	int m_WaitWriter;        //当前等待的写者数目
	HANDLE m_WriteSemaphore; //写等待信号
	HANDLE m_ReadSemaphore;  //读等待信号
	CRITICAL_SECTION m_CS;   //临界段
};

#endif