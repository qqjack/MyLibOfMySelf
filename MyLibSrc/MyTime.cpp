#include "MyTime.h"

CMyString CMyTime::m_WeekStrCN[]=
{
	CMyString("星期一"),
	CMyString("星期二"),
	CMyString("星期三"),
	CMyString("星期四"),
	CMyString("星期五"),
	CMyString("星期六"),
	CMyString("星期日")
};

CMyString CMyTime::m_WeekStrEN[]=
{
	CMyString("Monday"),
	CMyString("Tuesday"),
	CMyString("Wdnsday"),
	CMyString("Thiday"),
	CMyString("Friday"),
	CMyString("Saturday"),
	CMyString("Sunday")
};

CMyTime::CMyTime()
{
	SYSTEMTIME time;
	::GetLocalTime(&time);
	InitalTime(time);
}

CMyTime::CMyTime(SYSTEMTIME time)
{
	InitalTime(time);
}

CMyString CMyTime::GetDateString()
{
	char tBuf[30];
	sprintf(tBuf,"%04d-%02d-%02d",m_Time.wYear,m_Time.wMonth,m_Time.wDay);
	return CMyString(tBuf);
}

CMyString CMyTime::GetHourString()
{
	char tBuf[30];
	sprintf(tBuf,"%02d:%02d:%02d",m_Time.wHour,m_Time.wMinute,m_Time.wSecond);
	return CMyString(tBuf);
}

CMyString CMyTime::GetWeekString(bool EnOrCn)
{
	if(EnOrCn)
		return m_WeekStrEN[GetWeek()-1];
	return m_WeekStrCN[GetWeek()-1];
}

CMyString CMyTime::GetTimeString(bool EnOrCn)
{
	return GetDateString()+" "+GetWeekString(EnOrCn)+" "+GetHourString();
}

CMyTime::~CMyTime()
{

}

int CMyTime::InitalTime(SYSTEMTIME time)
{
	m_Time=time;
	return 1;
}

CMyTime CMyTime::GetCurrentTime()
{
	return CMyTime();
}