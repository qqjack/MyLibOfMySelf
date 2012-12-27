#ifndef __MYTHREAD__
#define __MYTHREAD__
#include <windows.h>
#include "common.h"
#include "process.h"

typedef unsigned int (__stdcall *_beginthreadex_type)( void * );

class CMyThread  
{
public:
	struct ThreadInfo
	{
		CMyThread*  m_Obj;			//线程使用者
		void*		m_Param;       //线程函数参数
		DWORD		m_ThreadID;
		int			m_RunVaule;    //run的返回值
	};

public:
	CMyThread();
	virtual ~CMyThread();

	virtual HANDLE Start(void *param)
	{
		if(!isStop())return m_ThreadHandle;
		m_Info.m_Obj=this;
		m_Info.m_Param=param;
		m_ThreadHandle= (HANDLE)_beginthreadex(NULL,0,(_beginthreadex_type)(ThreadProc),(void*)&m_Info,0,(unsigned int *)&m_Info.m_ThreadID);
		m_State	=RUNNING;
		return m_ThreadHandle;
	}

	virtual int			Run(void *param){return 1;}
	virtual void		OnThreadBeforeTerminate(){}//在线程被强制终止前调用

	bool	isRun(){return m_State==RUNNING;}
	bool	isSuspend(){return m_State==SUSPEND;}
	bool	isStop(){return m_State==STOP;}
	long	SuspendThread();
	long	ResumeThread();
	
	void	Sleep(int time){::Sleep(time);}

	DWORD	GetThreadID(){return m_Info.m_ThreadID;}
	HANDLE	GetThreadHandle(){return m_ThreadHandle;}
	int		GetRunValue(){return m_Info.m_RunVaule;}
	
	static int ThreadProc(void *param);
private:
	//此函数仅内部调用
	void ExitThread();
private:
	//线程状态
	typedef enum 
	{
		SUSPEND,       //挂起
		RUNNING,	   //运行
		STOP
	} THREAD_STATE;

	THREAD_STATE				m_State; 
	ThreadInfo			    	m_Info;
	HANDLE						m_ThreadHandle;
protected:
	
};

#endif