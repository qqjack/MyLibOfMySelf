#include "common.h"


//事件处理函数
HANDLE CreateEvent(bool bManualReset,bool bInitialState,LPCTSTR pName)
{
	return CreateEvent(NULL,bManualReset,bInitialState,pName);
}

bool   DeleteEvent(HANDLE hEvent)
{
	return CloseHandle(hEvent);
}

DWORD	WaitEvent(HANDLE hEvent,long tWait)
{
	return WaitForSingleObject(hEvent,tWait);
}

//互斥量相关函数
HANDLE CreateMutex(bool bInitialOwner,LPCTSTR pName)
{
	return CreateMutex(NULL,bInitialOwner,pName);
}

bool   DeleteMutex(HANDLE hMutex)
{
	return CloseHandle(hMutex);
}

DWORD  GetMutex(HANDLE hMutex)
{
	return WaitForSingleObject(hMutex,-1);
}

//信号量相关函数
HANDLE CreateSemaphore(LONG lInitialCount,LONG lMaximumCount,LPCTSTR pName)
{
	return CreateSemaphore(NULL,lInitialCount,lMaximumCount,pName);
}

DWORD  GetSemaphore(HANDLE hSemaphore,long tWait)
{
	return WaitForSingleObject(hSemaphore,tWait);
}

int    ReleaseSemaphoreWithPrev(HANDLE hSemaphore)
{
	return ReleaseSemaphoreWithPrev(hSemaphore,1);
}

int    ReleaseSemaphoreWithPrev(HANDLE hSemaphore,int count)
{
	long pre=-1;
	if(ReleaseSemaphore(hSemaphore,count,&pre))
	{
		return pre;
	}
	return -1;
}

bool   ReleaseSemaphore(HANDLE hSemaphore)
{
	return ReleaseSemaphore(hSemaphore,1);
}

bool   ReleaseSemaphore(HANDLE hSemaphore,int count)
{
	return ReleaseSemaphore(hSemaphore,count,NULL);
}

bool   DeleteSemaphore(HANDLE hSemaphore)
{
	return CloseHandle(hSemaphore);
}

int	   GetSlefPath(LPCTSTR fpath,int bufSize)
{
	char *filePath=new char[bufSize];
	int len=::GetModuleFileName(NULL,(char*)filePath,bufSize);
	if(len<bufSize)
	{
		char *p=filePath+len-1;
		while(p>filePath&&*p!='\\')p--;
		if(p<=filePath)
		{
			delete filePath;
			return -1;
		}
		*(p+1)=0;
		strcpy((char*)fpath,filePath);
	}
	else	
	{
		((char*)fpath)[bufSize-1]=0;
	}
	delete filePath;
	return len;
}

struct GroupInfo
{
	HANDLE *HandleSet;
	int		HandleSetSize;
	int		WaitTime;
};

void WaitThread(void *p)
{
	GroupInfo *set=(GroupInfo*)p;
	WaitForMultipleObjectsEx(set->HandleSetSize,set->HandleSet,true,set->WaitTime,true);
}

void APC_Call(void *p)
{
}

int	   WaitForMultiGroupHander(int groups,HANDLE *groupHanders,int tWait)
{
	int i=0,len=0;
	HANDLE semaphore=NULL;
	HANDLE *threadHandle=NULL;
	if(groups>1&&groups<64)
	{
		GroupInfo		*group=new GroupInfo[groups];
		semaphore		=CreateSemaphore(1,1,NULL);
		threadHandle	=new HANDLE[groups];
		for(int j=0;j<groups;j++)
		{
			len=0;
			while(groupHanders[i])
			{
				i++;
				len++;
			}
			group[j].HandleSet=new HANDLE[len+1];
			group[j].HandleSet[0]=semaphore;
			group[j].HandleSetSize=len;
			group[j].WaitTime=tWait;
			memcpy(&group[j].HandleSet[1],&groupHanders[i-len],len);
		}
		for(i=0;i<groups;i++)
		{
			threadHandle[i]=(HANDLE)_beginthread(WaitThread,0,&group[i]);
		}
		int r=WaitForMultipleObjects(groups,threadHandle,false,tWait);
		if(r==WAIT_TIMEOUT)
		{
			//如果等待超时，尝试获取信号量，以确保线程在有句柄为有信号的时候依然等待（确保内核对象状态不变）
			if(WaitForSingleObject(semaphore,0)==WAIT_TIMEOUT)
			{
				//成功维持内核对象状态
			}
			else
			{
				//已经有内核对象状态改变了,再次调用获取哪组句柄
				r=WaitForMultipleObjects(groups,threadHandle,false,tWait);
			}
		}
		//给等待线程加入异步回调处理，然等待的线程重新唤醒(这也是在线程函数中用WaitForMultipleObjectsEx的原因)
		for(i=0;i<groups;i++)
		{
			if(r==WAIT_TIMEOUT||(r-WAIT_OBJECT_0!=i))
			{
//				QueueUserAPC(APC_Call,threadHandle[i],0);
			}
			CloseHandle(threadHandle[i]);
		}
		return r;
	}
	else if(groups==1)
	{
		while(groupHanders[i])
		{
			i++;
			len++;
		}
		return WaitForMultipleObjects(len,groupHanders,true,tWait);
	}
	else
	{
	}

	return -1;
}

bool DebugPrivilegeEnable(bool Enable)
{
	HANDLE m_Token;
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&m_Token))
	{
		return false;
	}
	TOKEN_PRIVILEGES m_TokenP;
	m_TokenP.PrivilegeCount=1;
	if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&m_TokenP.Privileges[0].Luid))
	{
		return false;
	}
	m_TokenP.Privileges[0].Attributes=Enable?SE_PRIVILEGE_ENABLED :0;
	if(!AdjustTokenPrivileges(m_Token,false,&m_TokenP,sizeof(m_TokenP),0,0))
	{
		return false;
	} 
	CloseHandle(m_Token);
	return true;
}

static unsigned long g_OldProtect;
bool AddProtect(void *addr,int memSize)
{
	return VirtualProtectEx(::GetCurrentProcess(),addr,memSize,g_OldProtect,NULL);
}

bool RemoveProtect(void *addr,int memSize)
{
	return VirtualProtectEx(::GetCurrentProcess(),addr,memSize,PAGE_READWRITE,&g_OldProtect);
}

bool AddProtect(HANDLE processHandle,void *addr,unsigned long &proctect)
{
	return VirtualProtectEx(processHandle,addr,10,proctect,NULL);
}

bool RemoveProtect(HANDLE processHandle,void *addr,unsigned long &oldProctect)
{
	return VirtualProtectEx(processHandle,addr,10,PAGE_READWRITE,&oldProctect);
}

#define MAX_REMOTE_MEM (100)
void* InjectProcess(char *dllName,HANDLE proHandle)
{
	int		len	=strlen(dllName);
	void*	pMem=NULL;
	pMem=::VirtualAllocEx(proHandle,NULL,MAX_REMOTE_MEM,MEM_COMMIT,PAGE_READWRITE);
	if(!pMem)return NULL;
	unsigned long rl;
	::WriteProcessMemory(proHandle,pMem,dllName,len+1,&rl);
	HANDLE lThreadHandle=::CreateRemoteThread(proHandle,NULL,0,
					(unsigned long (__stdcall *)(void *))GetProcAddress(GetModuleHandle("Kernel32.dll"),"LoadLibraryA"),
					pMem,0,0);
	if(!lThreadHandle)
	{
		::VirtualFreeEx(proHandle,pMem,len+1,MEM_COMMIT);
		return NULL;
	}
	CloseHandle(lThreadHandle);
	return pMem;
}