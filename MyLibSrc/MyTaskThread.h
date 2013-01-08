#ifndef __MY_TASK__
#define __MY_TASK__

#include "MyThread.h"
#include "MyDebug.h"
#include <queue>

using namespace std;

class CMyTask
{
public:
	CMyTask():m_ThreadOutWhenFinish(false)
	{}
	virtual ~CMyTask()
	{}

	virtual int		Excute(void *threadArg,void* taskArg){return -1;}

	void			SetThreadOutWhenTaskFinish(bool out=false){m_ThreadOutWhenFinish=out;}
	bool			GetThreadOutWhenTaskFinish(){return m_ThreadOutWhenFinish;}
private:
	bool			m_ThreadOutWhenFinish;  //控制任务是否在执行完后线程退出
};


class CMyTaskThread :public CMyThread
{
public:
	typedef struct _TASK_INFO
	{
		CMyTask*	iTask;
		void*		iTaskArg;
	}TASK_INFO;

	typedef enum TASK_THREAD_STATE
	{
		WAITTING,
		BUSYING
	};

public:
	CMyTaskThread();
	virtual ~CMyTaskThread();
	
	virtual int		Run(void *param);
	virtual HANDLE	Start(void *param);
	//在线程将要被强制终止前，正常结束线程
	virtual	void	OnThreadBeforeTerminate(){ExitThreadDirect();}
	void			AssignTask(CMyTask& task,void* taskArg);
	
	//阻塞模式
	void			ExitThread();   //正常退出线程，如果任务队列中有任务则执行完任务退出
	void			ExitThreadDirect(); //执行完当前任务后退出
	//非阻塞模式
	void			WillExitThread();

	bool			IsBusy(){return m_State==BUSYING;}
	int				GetTaskCount();

private:
	bool		IsSelfThread();

private:
	std::queue<_TASK_INFO>		m_TaskList;
	HANDLE					m_Mutex;   //互斥m_TaskList
	HANDLE					m_Notify;  //在指派到任务的时候会触发
	bool					m_ExitDirect;  //是否直接退出
	bool					m_Exit;			//线程将要退出
	CMyTask					m_ExitTask;     //内部用来退出用的任务

	int						m_State;        //任务进程状态					
};

#endif