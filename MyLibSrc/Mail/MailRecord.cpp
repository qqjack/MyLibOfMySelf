#include "../MyLibHeader/Mail/MailRecord.h"

#define DATE_REGEX "(Mon|Tue|Wed|Thu|Fri|Sat|Sun) *, +([1-9]|[1-3][0-9]) +(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) +[0-9]{4} +[0-2][0-9]:[0-5][0-9]:[0-5][0-9]( +(\\+|-)[0-9]{4})?"
CMailRecord::CMailRecord():CDataRecord(NULL)
{
	
}

CMailRecord::~CMailRecord()
{
	Recyle();
}


void CMailRecord::OnParseKeyValue(CMyString& key,CMyString& value)
{
	if(key=="date"||key=="Date")
	{
		ParseDate(value);
	}
	else if(key=="Subject")
	{
		m_Subject	=value;
	}
	else if(key == "From")
	{
		m_From	=value;
	}

	CDataRecord::OnParseKeyValue(key,value);
}

void CMailRecord::AddAttachment(CDataRecord* attachment)
{
	CAttachment* a=new CAttachment(attachment);
	a->ParseData();
	m_Attachment.push_back(a);
}

void CMailRecord::AddInnerResource(CDataRecord* innerResource)
{
	CInnerResource* i=new CInnerResource(innerResource);
	i->ParseData();
	m_InnerResource.push_back(i);
}

bool CMailRecord::ParseDate(CMyString& value)
{
	CMyString date;
	CMyString splitTxt;
	CMyString temp;
	CMyTimeInterval interval;

	int year=0,month=0,day=0,hour=0,minute=0,second=0;

	if(value.Match(DATE_REGEX,&date,true)!=-1)
	{
		int count=date.Split(" ");
		date.GetSplitString(1,splitTxt);
		day=splitTxt.ToInt();

		date.GetSplitString(2,splitTxt);
		for(int i=1;i<=12;i++)
		{
			if(splitTxt.CompareI(CMyCalendar::sMonthS[i]))
			{
				month=i;
				break;
			}
		}
		
		date.GetSplitString(3,splitTxt);
		year=splitTxt.ToInt();
		
		date.GetSplitString(4,splitTxt);
		splitTxt.Split(":");
		splitTxt.GetSplitString(0,temp);
		hour=temp.ToInt();
		splitTxt.GetSplitString(1,temp);
		minute=temp.ToInt();
		splitTxt.GetSplitString(2,temp);
		second=temp.ToInt();

		m_Time=CMyTime(year,month,day,hour,minute,second);

		if(count==6)
		{
			date.GetSplitString(5,splitTxt);
			char tempBuf[3];
			tempBuf[2]=0;

			tempBuf[0]=splitTxt[1];
			tempBuf[1]=splitTxt[2];
	
			hour=atol(tempBuf);

			tempBuf[0]=splitTxt[3];
			tempBuf[1]=splitTxt[4];
			
			minute=atol(tempBuf);

			int totalSecond2=(hour*3600+minute*60)*(splitTxt[0]=='+'?1:-1);
			int totalSecond1=8*3600;

			int diff =totalSecond1-totalSecond2;

			if(diff<0)
			{
				interval.SetSingal(false);
			}
			else
			{
				diff=-diff;
			}

			interval.SetDays(diff/(3600*24));
			diff%=3600*24;
			interval.SetHours(diff/3600);
			diff%=60;
			interval.SetMinutes(diff/60);
			m_Time+=interval;
		}
		return true;
	}
	return false;
}

void CMailRecord::DumpState(CMyString& state)
{
	char buf[100];
	sprintf(buf,"Attachment : %d   InnerResource : %d \n\n",m_Attachment.size(),m_InnerResource.size());
	state=buf;
	CDataRecord::DumpState(state);
}

CAttachment* CMailRecord::GetAttachment(int index)
{
	if(index>=m_Attachment.size())return NULL;
	return m_Attachment[index];
}

CInnerResource*	CMailRecord::GetInnerResourceByID(CMyString& innerResourceID)
{
	int size=m_InnerResource.size();
	CMyString id;
	for(int i=0;i<size;i++)
	{
		if(m_InnerResource[i]->GetResourceID(id)==-1)continue;
		if(id==innerResourceID)return m_InnerResource[i];
	}
	return NULL;
}

CInnerResource* CMailRecord::GetInnerResourceByLink(CMyString& innerResourceLink)
{
	int size=m_InnerResource.size();
	CMyString link;
	for(int i=0;i<size;i++)
	{
		if(m_InnerResource[i]->GetResourceLink(innerResourceLink)==-1)continue;
		if(link==innerResourceLink)return m_InnerResource[i];
	}
	return NULL;
}

CDataRecord* CMailRecord::GetChildAt(int index)
{
	if(index>=m_Children.size())return NULL;
	return m_Children[index];
}

void CMailRecord::Recyle()
{
	int i=0;
	int size=m_Attachment.size();
	for(i=0;i<size;i++)
		delete m_Attachment[i];

	size=m_InnerResource.size();
	for(i=0;i<size;i++)
		delete m_InnerResource[i];

	m_Attachment.clear();
	m_InnerResource.clear();
}

void CMailRecord::Reset()
{
	Recyle();
	CDataRecord::Reset();
}