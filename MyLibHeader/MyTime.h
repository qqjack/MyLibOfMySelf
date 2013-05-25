#ifndef __MYTIME__
#define __MYTIME__

#include "windows.h"
#include "stdio.h"
#include "time.h"
#include "MyString.h"

#define MAX_TIMESTR_LEN  40

class CMyTimeInterval
{
public:
	CMyTimeInterval();
	//设置时间间隔必须按大单位到小单位进行设置，否则结果可能错误(例如俺顺序： 天 小时 分钟 秒钟 毫秒)

	void	SetDays(int days);
	void	SetHours(int hours);
	void	SetMinutes(int minutes);
	void	SetSeconds(int seconds);
	void	SetMilliseconds(int milliseconds);

	void    SetSingal(bool plus){m_Singal=plus;}

	int		GetDays(){return m_TimeInterval.wDay;}
	int		GetHours(){return m_TimeInterval.wHour;}
	int		GetMinutes(){return m_TimeInterval.wMinute;}
	int		GetSeconds(){return m_TimeInterval.wSecond;}
	int		GetMilliseconds(){return m_TimeInterval.wMilliseconds;}
	int		GetSingal(){return m_Singal;}
	
	int		Reset(){memset(&m_TimeInterval,0,sizeof(m_TimeInterval));}

	friend class CMyTime;

	void	HoursAdd(int inc);
	void	MinuteAdd(int inc);
	void	SecondAdd(int inc);
private:
	SYSTEMTIME	m_TimeInterval;
	bool		m_Singal;
};


class CMyCalendar
{
public:
	enum Week
	{
		MONDAY=0,
		TUESDAY,
		WEDNESDAY,
		THURSDAY,
		FRIDAY,
		SATURDAY,
		SUNDAY
	};

public:
	CMyCalendar();
	CMyCalendar(int year,int month,int day);

	static bool		IsLeapYear(int year);
	static Week		GetWeek(int year,int month,int day);
	static int		MonthDays(int year,int month);
	static int		YearDays(int year);
	static int		YearDaysLess(int year,int month,int day);
	//days为新的一年开始到现在的天数,返回这一年的几月几日
	static int		MonthDayInYear(int year,int days,int& day);

	

	unsigned	GetDay(){return m_Time.wDay;}
	unsigned	GetYear(){return m_Time.wYear;}
	unsigned	GetMonth(){return m_Time.wMonth;}
	Week		GetWeek(){return (Week)m_Time.wDayOfWeek;}

	CMyString	GetDateInArab();//阿拉伯写法  yy-mm-dd

	CMyString	GetWeekEN();		//获取星期
	CMyString	GetWeekCN();

	CMyString   GetDateInEN();
	CMyString	GetDateInEnSimple();//英文简写
	CMyString	GetDateInCN();

	CMyCalendar&	operator-=(CMyTimeInterval& timeInterval);
	CMyCalendar&	operator+=(CMyTimeInterval& timeInterval);
	CMyTimeInterval operator-(CMyCalendar time);
	bool			operator<(CMyCalendar& time);
	bool			operator>(CMyCalendar& time);
	bool			operator==(CMyCalendar& time);
	bool			operator>=(CMyCalendar& time);
	bool			operator!=(CMyCalendar& time);

	friend class CMyTime;

	static char*	sWeekEn[];
	static char*	sWeekCn[];
	static char*	sWeekEnS[];
	static char  	sMonthDays[13];
	static char*	sMonth[];
	static char*	sMonthS[];
private:
	
	int			InitalTime(SYSTEMTIME time);
	
private:
	SYSTEMTIME	m_Time;	
};

class CMyTime  
{
public:
	CMyTime();
	CMyTime(int year,int month,int day,int hour=0,int minute=0,int second=0,int milliseconds=0);
	CMyTime(SYSTEMTIME time);
	virtual ~CMyTime();

	 //获取小时时间
	CMyString	GetHourString(); 

	//格式化为字符串
	CMyString	ToStringEn();
	CMyString	ToStringCn();

	unsigned	GetDay(){return m_Calendar.m_Time.wDay;}
	unsigned	GetYear(){return m_Calendar.m_Time.wYear;}
	unsigned	GetMonth(){return m_Calendar.m_Time.wMonth;}
	unsigned	GetHour(){return m_Calendar.m_Time.wHour;}
	unsigned	GetMinute(){return m_Calendar.m_Time.wMinute;}
	unsigned	GetSecond(){return m_Calendar.m_Time.wSecond;}
	unsigned	GetWeek(){return m_Calendar.m_Time.wDayOfWeek;}
	unsigned	GetMilliseconds(){return m_Calendar.m_Time.wMilliseconds;}

	//从1700年元旦到现在经过的秒数
	unsigned	GetTotalSecond(){time_t t;return time(&t);}
	
	//阿拉伯写法  yy-mm-dd
	CMyString	GetDateInArab(){return m_Calendar.GetDateInArab();}

	//获取星期
	CMyString	GetWeekEN(){return m_Calendar.GetWeekEN();}		
	CMyString	GetWeekCN(){return m_Calendar.GetWeekCN();}

	CMyString   GetDateInEN(){return m_Calendar.GetDateInEN();}
	//英文简写
	CMyString	GetDateInEnSimple(){return m_Calendar.GetDateInEnSimple();}
	CMyString	GetDateInCN(){return m_Calendar.GetDateInCN();}

	//返回当前时间
	static CMyTime	GetCurrentTime(); 

	//精确度是秒级别
	bool		operator<(CMyTime& time);
	bool		operator>(CMyTime& time);
	bool		operator==(CMyTime& time);
	bool		operator>=(CMyTime& time);

	CMyTime& 	operator-=(CMyTimeInterval& timeInterval);
	CMyTime&	operator+=(CMyTimeInterval& timeInterval);
	CMyTimeInterval operator-(CMyTime time);
	CMyCalendar	GetCalendar();

private:
	static unsigned	GetTotalSecond(unsigned hour,unsigned minute,unsigned second);

private:
	CMyCalendar	m_Calendar;
};

#endif