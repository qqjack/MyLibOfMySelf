#include "MyTaskThread.h"


CMyTaskThread::CMyTaskThread()
{
	m_Mutex		=CreateMutex(false,NULL);
	m_Notify	=CreateEvent(false,false,NULL);
	m_ExitDirect=false;
	m_Exit		=false;
	m_ExitTask.SetThreadOutWhenTaskFinish(true);
}

CMyTaskThread::~CMyTaskThread()
{
	DeleteMutex(m_Mutex);
	DeleteEvent(m_Notify);
}

void CMyTaskThread::WillExitThread()
{
	AssignTask(m_ExitTask,NULL);
	m_Exit	=true;
}

void CMyTaskThread::ExitThread()
{
	AssignTask(m_ExitTask,NULL);
	m_Exit	=true;

	if(!IsSelfThread())  //如果是自己线程调用则不用等待，否则引起死锁
		WaitForSingleObject(GetThreadHandle(),-1);
}

void CMyTaskThread::AssignTask(CMyTask &task,void *taskArg)
{
	if(m_Exit)return;  //如果线程将要退出，则无法再指派任务
	TASK_INFO info;
	info.iTask			=&task;
	info.iTaskArg		=taskArg;
	GetMutex(m_Mutex);
	m_TaskList.push(info);
	ReleaseMutex(m_Mutex);
	SetEvent(m_Notify);
}

void CMyTaskThread::ExitThreadDirect()
{
	m_Exit			=true;
	m_ExitDirect	=true;
	SetEvent(m_Notify);

	if(!IsSelfThread())//如果是自己线程调用则不用等待，否则引起死锁
		WaitForSingleObject(GetThreadHandle(),-1);
}

bool CMyTaskThread::IsSelfThread()
{
	return GetCurrentThreadId()==GetThreadID();
}

int	 CMyTaskThread::Run(void *param)
{
	while(1)
	{
		m_State	=WAITTING;

		WaitForSingleObject(m_Notify,-1);
		if(m_ExitDirect)break;

		m_State	=BUSYING;

		std::queue<TASK_INFO> lQueue;

		GetMutex(m_Mutex);
		lQueue	=m_TaskList;
		while(m_TaskList.size())m_TaskList.pop();
		ReleaseMutex(m_Mutex);

		while(lQueue.size())
		{
			if(m_ExitDirect)return 1;
			TASK_INFO	taskInfo=lQueue.front();
			taskInfo.iTask->Excute(param,taskInfo.iTaskArg);
			if(taskInfo.iTask->GetThreadOutWhenTaskFinish())return 1;
			lQueue.pop();
		}
	}
	return 1;
}

HANDLE CMyTaskThread::Start(void *param)
{
	GetMutex(m_Mutex);
	while(m_TaskList.size())m_TaskList.pop();
	ReleaseMutex(m_Mutex);
	return CMyThread::Start(param);
}

int  CMyTaskThread::GetTaskCount()
{
	int size;
	GetMutex(m_Mutex);
	size	=m_TaskList.size();
	ReleaseMutex(m_Mutex);
	return size;
}