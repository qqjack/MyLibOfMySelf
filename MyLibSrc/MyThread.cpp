#include "MyThread.h"
#include "MyDebug.h"


CMyThread::CMyThread()
{
	m_State				=STOP;
}

CMyThread::~CMyThread()
{
	if(m_State==STOP)return;

	OnThreadBeforeTerminate();

	if(m_State==SUSPEND)
		ResumeThread();
	::TerminateThread(m_ThreadHandle,0);
}

int CMyThread::ThreadProc(void *param)
{
	CMyDebug::Log("ThreadInfo",0,0,"----start thread---");
	ThreadInfo* p_Info=(ThreadInfo*)param;
	int r=p_Info->m_Obj->Run(p_Info->m_Param);
	p_Info->m_RunVaule	=r;
	p_Info->m_Obj->ExitThread();
	CMyDebug::Log("ThreadInfo",0,0,"----end thread---");
	return r;
}

void CMyThread::ExitThread()
{
	m_State  =STOP;
}

long CMyThread::SuspendThread()
{
	m_State=SUSPEND;
	return ::SuspendThread(m_ThreadHandle);
}

long CMyThread::ResumeThread()
{
	m_State=RUNNING;
	return ::ResumeThread(m_ThreadHandle);
}