#ifndef __MYDEBUG__
#define __MYDEBUG__
#include "windows.h"
#include <iostream>
#include <fstream>
#include "common.h"

using namespace std;
#define MAX_LOG_FILE  (10)		  //最多log文件
#define MAX_LOG_LEN   (1000)		  //最长log
#define MAX_TAG_LEN   (100)         //最长标记
#define DEFAULT_TAG   "DEBUG"

class CMyDebug  
{
public:
	enum DEBUG_MODE
	{
		WATCH_LOG,        //观察窗口log
		FILE_LOG,         //写入文件log
		MESSAGE_BOX       //弹出框提示
	};

	CMyDebug();
	virtual ~CMyDebug();
	static void			Log(const char *format,...);
	static void			Log(const char *tag,const char *format,...);
	//fnum为文件号，文件号最大不能大于等于MAX_LOG_FILE
	static void			Log(int mode,int fnum,const char *format,...);
	static void			Log(const char *tag,int mode,int fnum,const char *format,...);
	//在log写入文件模式时，可设置log文件名
	static bool			SetFileLogName(int fnum,char *fileName);

	bool				SetFileLogNameIn(int fnum,char *fileName);
	void				LogOut(const char *tag,int mode,int fnum,char *log);

	
	void				ResetLog();

	char			m_TagMark[MAX_TAG_LEN];      //默认的tag标记
	DEBUG_MODE		m_Mode;

private:
	void				OutputFileLog(int fnum,char *log);
	void				UpdateTime(char* buf);   //更新时间字符串

	ofstream*       m_pStreamOut[MAX_LOG_FILE];  //默认LOG的输出文件流对象
	char*			m_LogFileName[MAX_LOG_FILE];
//	char			m_TimeStr[30];
//	char			m_LogStr[MAX_LOG_LEN+1+MAX_TAG_LEN+40];
	
	HANDLE			m_Mutex;
};

#define LOG CMyDebug::Log

#endif