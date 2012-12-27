#ifndef __MY_FILE__
#define __MY_FILE__
#include "MyString.h"
#include "MyDebug.h"
#include "windows.h"

#define TEMP_BUFFER_SIZE (1000)
class CMyFile  
{
public:
	enum FILE_MODE
	{
		FILE_NEW,    //文件不存在则创建,存在则打开失败
		FILE_OPEN,   //文件不存在则打开失败
		FILE_NEW_OR_OPEN,  //文件不存在则创建，文件存在则打开
		FILE_NEW_OR_CLEAR  //文件存在则清空，不存在则创建
	};

	enum SEEK_MODE
	{
		SEEK_START,
		SEEK_CURRENT,
		SEEK_ENDED
	};
public:
	CMyFile();
	virtual ~CMyFile();

	virtual int	Open(char *filePath,FILE_MODE mode);
	int			Read(char *buf,int size);
	int			Write(char *buf,int size);
	int			Seek(SEEK_MODE mode,int offset);
	int			Flush();
	void		DeleteFile();
	
	char*		GetMapping();
	void		ReleaseMapping();

	unsigned long	GetFileSize();

	static int  GetUniqueFileName(char*	buf,int bufLen);
	static CMyString	GetUniqueFileName();
	static bool IsFileExist(char *filePath);
private:
	// 在调用GetMapping函数后调用，锁住文件，此时对文件的读写定位都无效
	void		LockFile(){m_Locked=true;}        
	void		UnlockFile(){m_Locked=false;}
	bool		IsLocked(){return m_Locked;}
	void		Reset();

private:
	FILE*		m_File;      //文件指针
	int			m_FileSize;
	void*		m_FileBuf;   //作为文件映射后的文件映射地址
	HANDLE		m_FileHandle;  //文件句柄
	HANDLE		m_MapHandle;   //文件映射句柄
	int 		m_Error;     //错误标示
	
	bool		m_Locked;

	static char	*TAG;

protected:
	CMyString   m_FilePath;
	bool        m_Inital;    //是否已经创建文件
};
#endif
