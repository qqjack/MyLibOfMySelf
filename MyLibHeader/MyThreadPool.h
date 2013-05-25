#ifndef __MY_THREAD_POOL__
#define __MY_THREAD_POOL__
#include "MyTaskThread.h"
#include "MyThread.h"
#include "MyDebug.h"
#include <queue>

using namespace std;

#define DEFAULT_MAX_THREADS (8)
#define MAX_THREADS			(30)
#define TIME_INTERVAL		(1000)
#define MAX_COUNT			(5)

class CMyThreadPool  
{
	typedef struct _ThreadPoolInfo
	{
		std::queue<CMyTaskThread::_TASK_INFO>*		iTaskList;
		HANDLE										iMutex;
		HANDLE										iNotify;
	}ThreadPoolInfo;

	class CPoolExcuteTaskThread :public CMyThread
	{
	public:
		CPoolExcuteTaskThread()
		{
			m_IsBusy	=false;
			m_ExitEvent	=CreateEvent(false,false,NULL);
		}
		
		~CPoolExcuteTaskThread()
		{
			DeleteEvent(m_ExitEvent);
		}

		void	ExitThread()
		{
			SetEvent(m_ExitEvent);
			::WaitForSingleObject(GetThreadHandle(),-1);
		}

		void	WillExitThread()
		{
			SetEvent(m_ExitEvent);
		}

		bool	IsBusy()
		{
			return m_IsBusy;
		}

		virtual int Run(void *arg)
		{
			ThreadPoolInfo*  info=(ThreadPoolInfo*)arg;
			int size;
			CMyTaskThread::_TASK_INFO	taskInfo;
			int r;
			HANDLE	pEvent[2];
			pEvent[0]	=m_ExitEvent;
			pEvent[1]	=info->iNotify;
			
			while(1)
			{
				m_IsBusy	=false;

				GetMutex(info->iMutex);
				size	=info->iTaskList->size();
				if(size>0)
				{
					taskInfo	=info->iTaskList->front();
					info->iTaskList->pop();
				}
				ReleaseMutex(info->iMutex);
				if(!size)
				{
					r	=::WaitForMultipleObjects(2,pEvent,false,-1);
				}
				else
				{
					r	=::WaitForSingleObject(m_ExitEvent,0);
				}
				if(r==0)break;

				if(size)
				{
					m_IsBusy	=true;
					taskInfo.iTask->Excute(NULL,taskInfo.iTaskArg);
				}
			}
			return 1;
		}

	private:
		HANDLE			m_ExitEvent;
		bool			m_IsBusy;
	};

	typedef struct _MoniterData
	{
		CPoolExcuteTaskThread*	iThreadList;
		unsigned long*	iThreadCount;
		CMyTaskThread*	iMThread;
		std::queue<CMyTaskThread::_TASK_INFO>*	iTaskList;
		HANDLE			iMutex;
		CMyThreadPool*	iThreadPool;
	}MoniterData;

	class CMoniterTask :public CMyTask
	{
	public:
		CMoniterTask()
		{
			m_TimeInterval	=TIME_INTERVAL;
			m_ExitEvent		=CreateEvent(false,false,NULL);
			m_CountTick		=0;
		}
		
		void SetMoniterInterval(unsigned long interval)
		{
			m_TimeInterval	=interval;
		}

		virtual int Excute(void *threadArg,void *taskArg)
		{
			m_CountTick=(++m_CountTick)%MAX_COUNT;
			MoniterData*	data=(MoniterData*)taskArg;
			if(0==WaitForSingleObject(m_ExitEvent,m_TimeInterval))
			{
				SetThreadOutWhenTaskFinish(true);
				return -1;
			}

			char *TAG	="ThreadPool_Moniter";
			int	 activeThread=0;
			int  count=0;
			int  threadNum=0;
			GetMutex(data->iMutex);
			count		=data->iTaskList->size();
			threadNum	=*data->iThreadCount;
			ReleaseMutex(data->iMutex);

			for(int i=0;i<threadNum;i++)
			{
				if(data->iThreadList[i].IsBusy())
					activeThread++;
			}
			LOG(TAG,"ThreadPool  --activeThread:%d   --WaitThread:%d  --TaskCout:%d  --ThreadCount:%d",activeThread,*data->iThreadCount-activeThread,count,threadNum);
			
			if((m_CountTick==(MAX_COUNT-1))&&count>0&&threadNum<MAX_THREADS)
			{
				LOG(TAG,"Need Add Thread!!");
				data->iThreadPool->SetThreadCount(threadNum+1);	
			}
			
			data->iMThread->AssignTask(*this,taskArg);
			return 1;
		}

	public:
		HANDLE				m_ExitEvent;
	private:
		unsigned long		m_TimeInterval;
		unsigned long		m_CountTick;
	};

public:
	CMyThreadPool();
	CMyThreadPool(bool moniterThread);
	virtual ~CMyThreadPool();


	void		SetThreadCount(unsigned long threadCount);
	void		AssignTask(CMyTask &task,void* taskArg);

private:
	void		Inital();
private:

	HANDLE				m_Mutex;
	HANDLE				m_Notify;
	std::queue<CMyTaskThread::_TASK_INFO>   m_TaskList;

	CPoolExcuteTaskThread		m_ThreadList[MAX_THREADS];
	unsigned long		m_ThreadCount;
	ThreadPoolInfo		m_ThreadPoolInfo;

	bool				m_MoniterThread;   //是否监控线程池线程状态
	MoniterData			m_MoniterData;
	CMoniterTask		m_MoniterTask;
	CMyTaskThread		m_MThread;
	static char*		TAG;
};

#endif