#ifndef __COMMON__
#define __COMMON__
#include "windows.h"

#include "winbase.h"
#include "process.h"


#pragma   comment   (lib,"ws2_32.lib")
#define MAX_FILE_PATH (300)
#define START_DEBUG
//事件处理函数
HANDLE CreateEvent(bool bManualReset,bool bInitialState,LPCTSTR pName);
bool   DeleteEvent(HANDLE hEvent);
DWORD	WaitEvent(HANDLE hEvent,long tWait);

//互斥量相关函数
HANDLE CreateMutex(bool bInitialOwner,LPCTSTR pName);
bool   DeleteMutex(HANDLE hMutex);
DWORD  GetMutex(HANDLE hMutex);

//信号量相关函数
HANDLE CreateSemaphore(LONG lInitialCount,LONG lMaximumCount,LPCTSTR pName);
DWORD  GetSemaphore(HANDLE hSemaphore,long tWait);
bool   ReleaseSemaphore(HANDLE hSemaphore);
bool   ReleaseSemaphore(HANDLE hSemaphore,int count);
int    ReleaseSemaphoreWithPrev(HANDLE hSemaphore,int count); //返回先前的count值
int    ReleaseSemaphoreWithPrev(HANDLE hSemaphore);			  //返回先前的count值
bool   DeleteSemaphore(HANDLE hSemaphore);

//获取自身所在文件路径
int	   GetSlefPath(LPCTSTR fpath,int bufSize);

int	   WaitForMultiGroupHander(int groups,HANDLE *groupHanders,int tWait);

//提升进程拥有调试权限
bool   DebugPrivilegeEnable(bool Enable=true);

//添加内存地址保护
bool   AddProtect(void *addr,int memSize);
bool   AddProtect(HANDLE processHandle,void *addr,unsigned long &proctect);
//去掉内存地址保护，使内存区域可读，可写，可执行
bool   RemoveProtect(void *addr,int memSize);
bool   RemoveProtect(HANDLE processHandle,void *addr,unsigned long &oldProctect);
//远程注入函数,proHandle远程进程句柄
void* InjectProcess(char *dllName,HANDLE proHandle);
#endif