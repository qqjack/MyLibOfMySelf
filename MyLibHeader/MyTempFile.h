#ifndef __MY_TEMPFILE__
#define __MY_TEMPFILE__
#include "MyFile.h"
#include "MyString.h"
#include "windows.h"

class CMyTempFile : public CMyFile
{
public:
	CMyTempFile();
	virtual ~CMyTempFile();
	virtual int		Open(char *filePath,FILE_MODE mode){return -1;}

	bool		IsUsable(){return m_Inital;}  //判断临时文件是否成功创建
	void		ReCreate();                   //在临时文件创建失败后，可以调用次函数重建
};

#endif