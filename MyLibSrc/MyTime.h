#ifndef __MYTIME__
#define __MYTIME__

#include "windows.h"
#include "stdio.h"
#include "time.h"
#include "MyString.h"

#define MAX_TIMESTR_LEN  40

class CMyTime  
{
public:
	CMyTime();
	CMyTime(SYSTEMTIME time);
	virtual ~CMyTime();
	
	CMyString	GetDateString();  //获取日期时间	
	CMyString	GetWeekString(bool EnOrCn=true);  //获取星期时间
	CMyString	GetHourString();  //获取小时时间
	CMyString	GetTimeString(bool EnOrCn=true);  //获取日期小时时间

	unsigned	GetDay(){return m_Time.wDay;}
	unsigned	GetYear(){return m_Time.wYear;}
	unsigned	GetMonth(){return m_Time.wMonth;}
	unsigned	GetHour(){return m_Time.wHour;}
	unsigned	GetMinute(){return m_Time.wMinute;}
	unsigned	GetSecond(){return m_Time.wSecond;}
	unsigned	GetWeek(){return m_Time.wDayOfWeek;}
	unsigned	GetMilliseconds(){return m_Time.wMilliseconds;}
	unsigned	GetTotalSecond(){time_t t;return time(&t);}//从1700年元旦到现在经过的秒数

	static CMyTime	GetCurrentTime(); //返回当前时间
private:
	int				InitalTime(SYSTEMTIME time);

	static CMyString	m_WeekStrCN[];
	static CMyString	m_WeekStrEN[];
private:
	SYSTEMTIME	m_Time;
};

#endif