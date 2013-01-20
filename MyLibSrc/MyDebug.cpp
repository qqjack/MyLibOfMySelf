#include "MyDebug.h"
#include <stdio.h> 

CMyDebug g_Debug;

CMyDebug::CMyDebug()
{
	m_Mutex	=CreateMutex(false,NULL);
	ResetLog();
}

CMyDebug::~CMyDebug()
{
	ResetLog();
	CloseHandle(m_Mutex);
}

void CMyDebug::Log(const char *format,...)
{
#ifdef START_DEBUG
	char buf[MAX_LOG_LEN+1];
	va_list arglist;
	va_start(arglist,format);
	_vsnprintf(buf,MAX_LOG_LEN,format,arglist);
	va_end(arglist);
	buf[MAX_LOG_LEN]=0;
	g_Debug.LogOut(g_Debug.m_TagMark,g_Debug.m_Mode,0,buf);
#endif
}

void CMyDebug::Log(const char *tag,const char *format,...)
{
#ifdef START_DEBUG
	char buf[MAX_LOG_LEN+1];
	va_list arglist;
	va_start(arglist,format);
	_vsnprintf(buf,MAX_LOG_LEN,format,arglist);
	va_end(arglist);
	buf[MAX_LOG_LEN]=0;
	g_Debug.LogOut(tag,g_Debug.m_Mode,0,buf);
#endif
}

void CMyDebug::Log(int mode,int fnum,const char *format,...)
{
#ifdef START_DEBUG
	char buf[MAX_LOG_LEN+1];
	va_list arglist;
	va_start(arglist,format);
	_vsnprintf(buf,MAX_LOG_LEN,format,arglist);
	va_end(arglist);
	buf[MAX_LOG_LEN]=0;
	g_Debug.LogOut(g_Debug.m_TagMark,mode,fnum,buf);
#endif
}

void CMyDebug::Log(const char *tag,int mode,int fnum,const char *format,...)
{
#ifdef START_DEBUG
	char buf[MAX_LOG_LEN+1];
	va_list arglist;
	va_start(arglist,format);
	_vsnprintf(buf,MAX_LOG_LEN,format,arglist);
	va_end(arglist);
	buf[MAX_LOG_LEN]=0;
	g_Debug.LogOut(tag,mode,fnum,buf);
#endif
}

void CMyDebug::LogOut(const char *tag,int mode,int fnum,char *log)
{
	char			lTimeStr[30];
	char			lLogStr[MAX_LOG_LEN+1+MAX_TAG_LEN+40];

	if(!tag||!log)
	{
		return;
	}
	if(mode>MESSAGE_BOX||mode<FILE_LOG)  //LOG模式不在模式范围内，直接以默认模式打LOG
	{
		mode=WATCH_LOG;
	}
	if((mode==FILE_LOG)&&(fnum>=MAX_LOG_FILE||fnum<0))//LOG模式为写入文件，fnum不在合法范围，则算为合法范围
	{
		if(fnum<0)
		{
			fnum=-fnum;
		}
		fnum=fnum%MAX_LOG_FILE;
	}
	this->UpdateTime(lTimeStr);            //更新时间
	if(mode!=FILE_LOG)
	{
		sprintf(lLogStr,"%s[%s]:  %s\n",lTimeStr,tag,log);
	}
	else
	{
		sprintf(lLogStr,"%s  FileNo'%d[%s]:  %s\n",lTimeStr,fnum,tag,log);
	}
	lLogStr[MAX_LOG_LEN+1+MAX_TAG_LEN+39]=0;   //防止字符串长度过长，溢出
	switch(mode)
	{
	case FILE_LOG:
		{
			OutputFileLog(fnum,lLogStr);
			break;
		}
	case WATCH_LOG:
		{
			::OutputDebugString(lLogStr);
			break;
		}
	case MESSAGE_BOX:
		{
			::MessageBox(NULL,lLogStr,"Log_Message",0);
			break;
		}
	}
}

void CMyDebug::ResetLog()
{
	m_Mode	=WATCH_LOG;
	strcpy(m_TagMark,DEFAULT_TAG);
	for(int i=0;i<MAX_LOG_FILE;i++)
	{
		if(m_LogFileName[i])
		{
			delete m_LogFileName[i];
			m_LogFileName[i]=NULL;
		}
		if(m_pStreamOut[i])
		{
			m_pStreamOut[i]->close();
			delete m_pStreamOut[i];
			m_pStreamOut[i]=NULL;
		}
	}
}

void CMyDebug::OutputFileLog(int fnum,char *log)
{
	if(fnum>MAX_LOG_FILE||fnum<0)
	{
		CMyDebug::Log(MESSAGE_BOX,0,"OutputFileLog-------the fnum is invalid!!");
		return;
	}
	GetMutex(m_Mutex);

	char logFile[MAX_FILE_PATH];
	char filePath[MAX_FILE_PATH];
	if(!m_pStreamOut[fnum])
	{
		GetSlefPath(filePath,MAX_FILE_PATH);
		if(m_LogFileName[fnum])
		{
			sprintf(logFile,"%s%s.log",filePath,m_LogFileName[fnum]);
		}
		else
		{
			sprintf(logFile,"%s%s_%d.log",filePath,DEFAULT_TAG,fnum);
		}
		m_pStreamOut[fnum]=new ofstream();
		m_pStreamOut[fnum]->open(logFile,ios::app);
	}
	(*(m_pStreamOut[fnum]))<<log;
	(*(m_pStreamOut[fnum])).flush();

	ReleaseMutex(m_Mutex);
}

void CMyDebug::UpdateTime(char *buf)
{
	SYSTEMTIME systime;
	::GetLocalTime(&systime);
	sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);
}

bool CMyDebug::SetFileLogName(int fnum,char *fileName)
{
	return g_Debug.SetFileLogNameIn(fnum,fileName);
}

bool CMyDebug::SetFileLogNameIn(int fnum,char *fileName)
{
	if(fnum>MAX_LOG_FILE||fnum<0)
	{
		return false;
	}
	int len;
	len		=strlen(fileName);

	GetMutex(m_Mutex);

	if(m_LogFileName[fnum])
	{
		//如果原有文件号对应已经设置文件名，释放空间
		delete m_LogFileName[fnum];
		m_LogFileName[fnum]=NULL;
	}
	if(m_pStreamOut[fnum])
	{
		//如果原有文件号，已经使用，则关闭原有文件句柄，释放空间
		m_pStreamOut[fnum]->close();
		delete m_pStreamOut[fnum];
		m_pStreamOut[fnum]=NULL;
	}
	m_LogFileName[fnum]=new char[len+1];
	strcpy(m_LogFileName[fnum],fileName);

	ReleaseMutex(m_Mutex);
	return true;
}
