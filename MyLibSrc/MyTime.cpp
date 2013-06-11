#include "MyTime.h"

//-----------------CMyTimeInterval-----------------------

CMyTimeInterval::CMyTimeInterval()
{
	memset(&m_TimeInterval,0,sizeof(m_TimeInterval));
	m_Singal=true;
}

void CMyTimeInterval::SetDays(int days)
{
	m_TimeInterval.wDay=days;
}

void CMyTimeInterval::SetHours(int hours)
{
	if(hours>=24)
	{
		m_TimeInterval.wDay+=hours/24;
		m_TimeInterval.wHour=hours%24;
	}
	else
		m_TimeInterval.wHour=hours;
}

void CMyTimeInterval::SetMinutes(int mintues)
{
	if(mintues>=60)
	{
		HoursAdd(mintues/60);
		m_TimeInterval.wMinute=mintues%60;
	}
	else
		m_TimeInterval.wMinute=mintues;
}

void CMyTimeInterval::SetSeconds(int seconds)
{
	if(seconds>=60)
	{
		MinuteAdd(seconds/60);
		m_TimeInterval.wSecond=seconds%60;
	}
	else
		m_TimeInterval.wSecond=seconds;
}

void CMyTimeInterval::SetMilliseconds(int milliseconds)
{
	if(milliseconds>1000)
	{
		SecondAdd(milliseconds/1000);
		m_TimeInterval.wMilliseconds=milliseconds%1000;
	}
	else
		m_TimeInterval.wMilliseconds=milliseconds;
}

void CMyTimeInterval::HoursAdd(int inc)
{
	m_TimeInterval.wHour+=inc;
	if(m_TimeInterval.wHour>24)
	{
		m_TimeInterval.wDay+=m_TimeInterval.wHour/24;
		m_TimeInterval.wHour%=24;
	}
}

void CMyTimeInterval::MinuteAdd(int inc)
{
	m_TimeInterval.wMinute+=inc;
	if(m_TimeInterval.wMinute>=60)
	{
		HoursAdd(m_TimeInterval.wMinute/60);
		m_TimeInterval.wMinute%=60;
	}
}

void CMyTimeInterval::SecondAdd(int inc)
{
	m_TimeInterval.wSecond+=inc;
	if(m_TimeInterval.wSecond>=60)
	{
		MinuteAdd(m_TimeInterval.wSecond/60);
		m_TimeInterval.wSecond%=60;
	}
}


//---------------CMyCalendar------------------
char* CMyCalendar::sWeekEn[]=
{
	"Monday",
	"TuesDay",
	"Wednesday",
	"ThursDay",
	"Friday",
	"Saturday",
	"Sunday"
};

char* CMyCalendar::sWeekCn[]=
{
	"星期一",
	"星期二",
	"星期三",
	"星期四",
	"星期五",
	"星期六",
	"星期日"
};

char* CMyCalendar::sWeekEnS[]=
{
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

char* CMyCalendar::sMonth[]=
{
	"",
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

char* CMyCalendar::sMonthS[]=
{
	"",
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

char CMyCalendar::sMonthDays[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

CMyString CMyCalendar::GetDateInArab()
{
	return CMyString::Format("%d-%d-%d",m_Time.wYear,m_Time.wMonth,m_Time.wDay);
}

CMyString CMyCalendar::GetWeekEN()
{
	CMyString week=sWeekEn[m_Time.wDayOfWeek];
	return week;
}

CMyString CMyCalendar::GetWeekCN()
{
	CMyString week=sWeekCn[m_Time.wDayOfWeek];
	return week;
}

CMyString CMyCalendar::GetDateInEN()
{
	CMyString date=CMyString::Format("%s, %d %s %d",sWeekEn[m_Time.wDayOfWeek],m_Time.wDay,
								sMonth[m_Time.wMonth],m_Time.wYear);
	return date;
}

CMyString CMyCalendar::GetDateInEnSimple()
{
	CMyString date=CMyString::Format("%s, %d %s %d",sWeekEnS[m_Time.wDayOfWeek],m_Time.wDay,
								sMonthS[m_Time.wMonth],m_Time.wYear);
	return date;
}

CMyString CMyCalendar::GetDateInCN()
{
	CMyString date=CMyString::Format("%d年 %d月 %d日 %s",m_Time.wYear,m_Time.wMonth,
									m_Time.wDay,sWeekCn[m_Time.wDayOfWeek]);
	return date;
}

CMyCalendar::CMyCalendar()
{
	SYSTEMTIME time;
	::GetLocalTime(&time);
	InitalTime(time);
}

CMyCalendar::CMyCalendar(int year,int month,int day)
{
	memset(&m_Time,0,sizeof(m_Time));
	m_Time.wDay=day;
	m_Time.wMonth=month;
	m_Time.wYear=year;
	m_Time.wDayOfWeek=GetWeek(year,month,day);
}

bool CMyCalendar::IsLeapYear(int year)
{
	if(!(year%100))
	{
		if(year%400)return false;
		return true;
	}
	else
	{
		if(year%4)return false;
		return true;
	}
	return false;
}

CMyCalendar::Week CMyCalendar::GetWeek(int year,int month,int day)
{
	int y=year%100;
	int c=year/100;
	int m=month;
	int d=day;
	if(m==1||m==2)
	{
		m+=12;
		y--;
		if(y<0)
		{
			y+=100;
			c--;
		}
	}

	int w=(y+y/4+c/4-2*c+26*(m+1)/10+d-1)%7;
	w--;
	if(w<0)w=6;
	return (CMyCalendar::Week)w;
}

int	CMyCalendar::InitalTime(SYSTEMTIME time)
{
	m_Time=time;
	m_Time.wDayOfWeek=GetWeek(m_Time.wYear,m_Time.wMonth,m_Time.wDay);
	return 1;
}

int CMyCalendar::MonthDays(int year,int month)
{
	if(month==2)
	{
		if(IsLeapYear(year))
			return 29;
		return 28;
	}
	return sMonthDays[month];
}

int	CMyCalendar::YearDaysLess(int year,int month,int day)
{
	int days=0;
	for(int i=1;i<month;i++)
	{
		days+=MonthDays(year,i);
	}
	days+=day;
	return YearDays(year)-days;
}

int	CMyCalendar::YearDays(int year)
{
	if(IsLeapYear(year))
		return 366;
	return 365;
}

CMyCalendar& CMyCalendar::operator-=(CMyTimeInterval& timeInterval)
{
	if(!timeInterval.GetSingal())
	{
		CMyTimeInterval t=timeInterval;
		t.SetSingal(true);
		return *this+=t;
	}
	int days=timeInterval.GetDays();
	if(days<=0)return *this;
	int y=GetYear();
	int m=GetMonth();
	int d=GetDay();
	int less=YearDaysLess(y,m,d);
	int uless=YearDays(y)-less;
	if(days<uless)
	{
		//没有跨年
		if(days<d)
		{
			//没有跨月
			m_Time.wDay-=days;
		}
		else
		{
			//跨月
			days-=d;
			int md=0;
			while(1)
			{
				md=MonthDays(y,--m);
				if(md>=days)break;
				days-=md;
			}
			m_Time.wDay=md-days;
			m_Time.wMonth=m;
		}
	}
	else
	{
		//跨年
		days-=uless;
		int yd=0;
		while(1)
		{
			yd=YearDays(--y);
			if(yd>=days)break;
			days-=yd;
		}
		m_Time.wMonth=MonthDayInYear(y,YearDays(y)-days,(int &)m_Time.wDay);
		m_Time.wYear=y;
	}

	m_Time.wDayOfWeek=GetWeek(m_Time.wYear,m_Time.wMonth,m_Time.wDay);
	return *this;
}

CMyCalendar& CMyCalendar::operator+=(CMyTimeInterval& timeInterval)
{
	if(!timeInterval.GetSingal())
	{
		CMyTimeInterval t=timeInterval;
		t.SetSingal(true);
		return *this-=t;
	}
	int days=timeInterval.GetDays();
	if(days<=0)return *this;
	int y=GetYear();
	int m=GetMonth();
	int d=GetDay();
	int less=YearDaysLess(y,m,d);
	if(days<=less)
	{
		//没有跨年
		int md=MonthDays(y,m);
		if(days<=md-d)
		{
			//没有跨月
			m_Time.wDay+=days;
		}
		else
		{
			//跨月
			days-=md-d;
			md=0;
			while(1)
			{
				md=MonthDays(y,++m);
				if(md>=days)break;
				days-=md;
			}
			m_Time.wDay=days;
			m_Time.wMonth=m;
		}
	}
	else
	{
		//跨年
		days-=less;
		int yd=0;
		while(1)
		{
			yd=YearDays(++y);
			if(yd>=days)break;
			days-=yd;
		}
		m_Time.wMonth=MonthDayInYear(y,days,(int &)m_Time.wDay);
		m_Time.wYear=y;
	}
	m_Time.wDayOfWeek=GetWeek(m_Time.wYear,m_Time.wMonth,m_Time.wDay);
	return *this;
}

int CMyCalendar::MonthDayInYear(int year,int days,int& day)
{
	int d=YearDays(year);
	if(d<days)return -1;
	int m=0;
	int md=0;
	while(1)
	{
		md=MonthDays(year,++m);
		if(md>=days)break;
		days-=md;
	}
	day=days;
	return m;
}

CMyTimeInterval CMyCalendar::operator-(CMyCalendar time)
{
	CMyTimeInterval	interval;
	bool singal=*this>=time;
	if(*this==time)
	{
		interval.SetSingal(true);
		return interval;
	}
	int  mark=(singal?1:-1);
	int y1=GetYear();
	int y2=time.GetYear();
	int m1=GetMonth();
	int m2=time.GetMonth();
	int d=0;
	
	if(y1==y2)
	{
		if(m1==m2)
		{
			//同年同月情况
			interval.SetDays(mark*(GetDay()-time.GetDay()));
		}
		else
		{
			int max=(m1>m2?m1:m2);
			int min=(m1<m2?m1:m2);
			int count=max-min;
			for(int i=1;i<count;i++)
			{
				d+=MonthDays(y1,min+i);
			}
			d+=MonthDays(y1,min)-(singal?time.GetDay():GetDay());
			d+=(singal?GetDay():time.GetDay());
			interval.SetDays(d);
		}
	}
	else
	{
		int max=(y1>y2?y1:y2);
		int min=(y1<y2?y1:y2);
		int count=max-min;
		for(int i=1;i<count;i++)
		{
			d+=YearDays(min+i);
		}
		if(singal)
		{
			d+=YearDaysLess(min,time.GetMonth(),time.GetDay());
			d+=YearDays(max)-YearDaysLess(max,GetMonth(),GetDay());
		}
		else
		{
			d+=YearDaysLess(min,GetMonth(),GetDay());
			d+=YearDays(max)-YearDaysLess(max,time.GetMonth(),time.GetDay());
		}
		interval.SetDays(d);
	}
	interval.SetSingal(singal);
	return interval;
}

bool CMyCalendar::operator<(CMyCalendar& time)
{
	if(!(*this>time)&&!(*this==time))
		return true;
	return false;
}

bool CMyCalendar::operator>(CMyCalendar& time)
{
	if(GetYear()>time.GetYear())return true;
	if(GetYear()==time.GetYear())
	{
		if(GetMonth()>time.GetMonth())return true;
		if(GetMonth()==time.GetMonth())
		{
			if(GetDay()>time.GetDay())return true;
		}
	}
	return false;
}

bool CMyCalendar::operator==(CMyCalendar& time)
{
	if(GetYear()==time.GetYear()&&
	   GetMonth()==time.GetMonth()&&
	   GetDay()==time.GetDay())
	   return true;
	return false;
}

bool CMyCalendar::operator!=(CMyCalendar& time)
{
	return !(*this==time);
}

bool CMyCalendar::operator>=(CMyCalendar& time)
{
	if(*this>time||*this==time)
		return true;
	return false;
}


//---------------------CMyTime------------------

CMyTime::CMyTime()
{
}

CMyTime::CMyTime(int year,int month,int day,int hour,int minute,int second,int milliseconds)
{
	SYSTEMTIME time;
	time.wDay=day;
	time.wYear=year;
	time.wMonth=month;
	time.wMilliseconds=milliseconds;
	time.wSecond=second;
	time.wHour=hour;
	time.wMinute=minute;
	m_Calendar.InitalTime(time);
}

CMyTime::CMyTime(SYSTEMTIME time)
{
	m_Calendar.InitalTime(time);
}

CMyString CMyTime::GetHourString()
{
	return CMyString::Format("%02d:%02d:%02d",m_Calendar.m_Time.wHour,m_Calendar.m_Time.wMinute,m_Calendar.m_Time.wSecond);
}

CMyString CMyTime::ToStringEn()
{
	return m_Calendar.GetDateInEnSimple()+" "+GetHourString();
}

CMyString CMyTime::ToStringCn()
{
	return m_Calendar.GetDateInCN()+" "+GetHourString();
}

CMyTime::~CMyTime()
{
}

CMyTime CMyTime::GetCurrentTime()
{
	return CMyTime();
}

bool CMyTime::operator>(CMyTime& time)
{
	if(GetYear()>time.GetYear())return true;
	if(GetYear()==time.GetYear())
	{
		if(GetMonth()>time.GetMonth())return true;
		if(GetMonth()==time.GetMonth())
		{
			if(GetDay()>time.GetDay())return true;
			if(GetDay()==time.GetDay())
			{
				if(GetHour()>time.GetHour())return true;
				if(GetHour()==time.GetHour())
				{
					if(GetMinute()>time.GetMinute())return true;
					if(GetMinute()==time.GetMinute())
					{
						if(GetSecond()>time.GetSecond())return true;
					}
				}
			}
		}
	}
	return false;
}

bool CMyTime::operator<(CMyTime& time)
{
	if(!(*this>time)&&!(*this==time))
		return true;
	return false;
}

bool CMyTime::operator==(CMyTime& time)
{
	if(m_Calendar.m_Time.wDay==time.GetDay()&&
	   m_Calendar.m_Time.wYear==time.GetYear()&&
	   m_Calendar.m_Time.wMonth==time.GetMonth()&&
	   m_Calendar.m_Time.wHour	==time.GetHour()&&
	   m_Calendar.m_Time.wMinute==time.GetMinute()&&
	   m_Calendar.m_Time.wSecond==time.GetSecond())
	   return true;
	return false;
}

bool CMyTime::operator>=(CMyTime& time)
{
	if(*this>time||*this==time)
		return true;
	return false;
}

CMyTime& CMyTime::operator-=(CMyTimeInterval& timeInterval)
{
	CMyTimeInterval interval=timeInterval;
	m_Calendar-=interval;
	unsigned totalSecond1=GetTotalSecond(m_Calendar.m_Time.wHour,m_Calendar.m_Time.wMinute,m_Calendar.m_Time.wSecond);
	unsigned totalSecond2=GetTotalSecond(timeInterval.m_TimeInterval.wHour,timeInterval.m_TimeInterval.wMinute,
											timeInterval.m_TimeInterval.wSecond);
	int less=totalSecond1-totalSecond2;
	if(less<0)
	{
		//一天内的时间不够减，减去一天
		interval.SetDays(1);
		m_Calendar-=interval;

		less=24*60*60+less;
	}
	else if(less==0&&m_Calendar.m_Time.wMilliseconds<interval.m_TimeInterval.wMilliseconds)
	{
		//秒不够，借一天
		interval.SetDays(1);
		m_Calendar-=interval;

		less=24*60*60;
	}


	m_Calendar.m_Time.wMilliseconds-=interval.m_TimeInterval.wMilliseconds;
	if((short)m_Calendar.m_Time.wMilliseconds<0)
	{
		//毫秒不够，借一秒
		m_Calendar.m_Time.wMilliseconds+=1000;
		less--;
	}

	m_Calendar.m_Time.wHour=less/3600;
	less%=3600;
	m_Calendar.m_Time.wMinute=less/60;
	less%=60;
	m_Calendar.m_Time.wSecond=less;

	return *this;
}

CMyTime& CMyTime::operator+=(CMyTimeInterval& timeInterval)
{
	CMyTimeInterval interval=timeInterval;
	unsigned totalSecond1=GetTotalSecond(m_Calendar.m_Time.wHour,m_Calendar.m_Time.wMinute,m_Calendar.m_Time.wSecond);
	unsigned totalSecond2=GetTotalSecond(timeInterval.m_TimeInterval.wHour,timeInterval.m_TimeInterval.wMinute,
											timeInterval.m_TimeInterval.wSecond);
	totalSecond1+=totalSecond2;
	m_Calendar.m_Time.wMilliseconds+=timeInterval.m_TimeInterval.wMilliseconds;
	totalSecond1+=m_Calendar.m_Time.wMilliseconds/1000;

	interval.SetDays(interval.GetDays()+totalSecond1/(24*3600));

	totalSecond1%=(24*3600);

	m_Calendar.m_Time.wMilliseconds%=1000;

	m_Calendar.m_Time.wHour=totalSecond1/3600;
	totalSecond1%=3600;
	m_Calendar.m_Time.wMinute=totalSecond1/60;
	totalSecond1%=60;
	m_Calendar.m_Time.wSecond=totalSecond1;

	m_Calendar+=interval;
	return *this;
}

CMyTimeInterval CMyTime::operator-(CMyTime time)
{
	CMyCalendar		otherCalendar=time.GetCalendar();
	CMyTimeInterval timeInterval=m_Calendar-otherCalendar;

	unsigned totalSecond1=GetTotalSecond(m_Calendar.m_Time.wHour,m_Calendar.m_Time.wMinute,m_Calendar.m_Time.wSecond);
	unsigned totalSecond2=GetTotalSecond(otherCalendar.m_Time.wHour,otherCalendar.m_Time.wMinute,
											otherCalendar.m_Time.wSecond);

	int days=timeInterval.GetDays();
	int diff=totalSecond1-totalSecond2;
	int diffM=m_Calendar.m_Time.wMilliseconds-time.GetMilliseconds();
	if(*this==time&&diffM==0)return timeInterval;

	bool big=*this>time||(*this==time&&diffM>0);
	if(days==0)
	{
		if(!big)
		{
			diff=-diff;
		}
	}
	else
	{
		if(big)
		{
			diff+=3600*24;
			days--;
		}
		else
		{
			diff=-diff+3600*24;
			days--;
		}
	}

	if(big)
		diffM+=1000;
	else
		diffM=-diffM+1000;
	diff--;

	diff+=diffM/1000;
	diffM%=1000;

	days+=diff/(3600*24);
	diff%=(3600*24);

	timeInterval.SetDays(days);
	timeInterval.SetHours(diff/3600);
	diff%=3600;
	timeInterval.SetMinutes(diff/60);
	diff%=60;
	timeInterval.SetSeconds(diff);
	timeInterval.SetMilliseconds(diffM);
	timeInterval.SetSingal(big);
	return timeInterval;
}

unsigned CMyTime::GetTotalSecond(unsigned hour,unsigned minute,unsigned second)
{
	return hour*60*60+minute*60+second;
}

CMyCalendar	CMyTime::GetCalendar()
{
	return m_Calendar;
}