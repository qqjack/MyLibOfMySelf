#include "MyThreadPool.h"

char*	CMyThreadPool::TAG="CMyThreadPool";

CMyThreadPool::CMyThreadPool()
{
	m_MoniterThread	=false;
	Inital();
}

CMyThreadPool::CMyThreadPool(bool moniterThread)
{
	m_MoniterThread	=moniterThread;
	Inital();
}

void CMyThreadPool::Inital()
{
	m_ThreadCount	=DEFAULT_MAX_THREADS;
	m_Mutex		=CreateMutex(false,NULL);
	m_Notify	=CreateEvent(false,false,NULL);

	m_MoniterData.iThreadCount	=&m_ThreadCount;
	m_MoniterData.iThreadList	=m_ThreadList;
	m_MoniterData.iMThread		=&m_MThread;
	m_MoniterData.iTaskList		=&m_TaskList;
	m_MoniterData.iMutex		=m_Mutex;
	m_MoniterData.iThreadPool	=this;
	m_ThreadPoolInfo.iMutex		=m_Mutex;
	m_ThreadPoolInfo.iNotify	=m_Notify;
	m_ThreadPoolInfo.iTaskList	=&m_TaskList;

	for(int i=0;i<m_ThreadCount;i++)
	{
		m_ThreadList[i].Start((void*)&m_ThreadPoolInfo);
	}

	if(m_MoniterThread)
	{
		m_MThread.Start(NULL);
		m_MThread.AssignTask(m_MoniterTask,(void*)&m_MoniterData);
	}
}

CMyThreadPool::~CMyThreadPool()
{
	HANDLE	threadHandle[MAX_THREADS];

	for(int i=0;i<m_ThreadCount;i++)
	{
		threadHandle[i]	=m_ThreadList[i].GetThreadHandle();
		m_ThreadList[i].WillExitThread();
	}

	::WaitForMultipleObjects(m_ThreadCount,threadHandle,true,-1);

	if(m_MoniterThread)
	{
		SetEvent(m_MoniterTask.m_ExitEvent);
		m_MThread.ExitThreadDirect();
	}

	for(i=m_ThreadCount;i<MAX_THREADS;i++)
	{
		if(m_ThreadList[i].isRun())
			m_ThreadList[i].ExitThread();
	}
}

void CMyThreadPool::SetThreadCount(unsigned long threadCount)
{
	GetMutex(m_Mutex);
	if(threadCount>MAX_THREADS)
		threadCount=MAX_THREADS;

	if(threadCount>m_ThreadCount)
	{
		for(int i=m_ThreadCount;i<threadCount;i++)
			m_ThreadList[i].Start((void*)&m_ThreadPoolInfo);
	}
	else
	{
		for(int i=threadCount;i<m_ThreadCount;i++)
			m_ThreadList[i].WillExitThread();
	}

	m_ThreadCount	=threadCount;
	ReleaseMutex(m_Mutex);
}

void CMyThreadPool::AssignTask(CMyTask &task,void*	taskArg)
{
	CMyTaskThread::_TASK_INFO taskInfo;
	taskInfo.iTask		=&task;
	taskInfo.iTaskArg	=taskArg;
	GetMutex(m_Mutex);
	m_TaskList.push(taskInfo);
	ReleaseMutex(m_Mutex);
	SetEvent(m_Notify);
}