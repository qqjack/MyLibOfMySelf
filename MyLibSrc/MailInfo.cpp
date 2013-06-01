#include "MailInfo.h"
#include "MyDebug.h"
#include "Encrypt.h"
#include "MyBuffer.h"


#define  SWITCH

#ifndef SWITCH
	#define LogS	LOGNULL
#else
	#define LogS LOG
#endif

#define CONTENT_TYPE_COUNT (7)
#define CONTENT_ENCODE_TYPE_COUNT (7)

#define DATE_REGEX "(Mon|Tues|Wed|Thur|Fri|Sat|Sun) *, +([1-9]|[1-3][0-9]) +(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) +[0-9]{4} +[0-2][0-9]:[0-5][0-9]:[0-5][0-9]( +(\\+|-)[0-9]{4})?"


char* CMailInfo::sContentType[]=
{
	"text",
	"multipart",
	"image",
	"aduio",
	"video",
	"message",
	"application"
};

char* CMailInfo::sTextSubType[]=
{
	"plain",
	"html"
};

char* CMailInfo::sMultipartSubType[]=
{
	"mixed",
	"alternative",
	"parallel",
	"digest"
};

char* CMailInfo::sImageSubType[]=
{
	"jpeg",
	"gif"
};

char* CMailInfo::sAudioSubType[]=
{
	"basic"
};

char* CMailInfo::sVideoSubType[]=
{
	"mpeg"
};

char* CMailInfo::sMessageSubType[]=
{
	"rfc822",
	"partial",
	"external-body"
};

char* CMailInfo::sApplicationSubType[]=
{
	"octet-stream"
};

char* CMailInfo::TAG="CMailInfo";

CMailInfo::ContentType CMailInfo::sContentMap[]=
{
	{sTextSubType		,2},
	{sMultipartSubType	,4},
	{sImageSubType		,2},
	{sAudioSubType		,1},
	{sVideoSubType		,1},
	{sMessageSubType	,3},
	{sApplicationSubType	,1}
};

char* CMailInfo::sTransferEncode[]=
{
	"7bit",
	"8bit",
	"quoted-printable",
	"base64",
	"binary",
	"x-token"
};

#define BASE64 (3)
#define QP	   (4)

CMailInfo::CMailInfo()
{
	m_HeaderParsed	=false;
}

CMailInfo::~CMailInfo()
{
	int size=m_SubMails.size();
	for(int i=0;i<size;i++)
	{
		delete m_SubMails[i];
	}
	m_SubMails.clear();
}

bool CMailInfo::Parse(CMyString& data)
{
	if(!ParseMailHeader(data))return false;
	if(!ParseMailBody(m_ExternData))return false;
	return true;
}

bool CMailInfo::ParseMailBody(CMyString& data)
{
	CMyString value;
	CMyString mainType;
	CMyString subType;
	CMyString tempStr;
	CMyString encode;

	int mainTypeIndex=-1,subTypeIndex=-1,encodeTypeIndex=-1;
	if(-1==m_KeyMap.GetItem(CMyString("Content-Type"),&value))return false;
	
	int count=value.Split(";");
	value.GetSplitString(0,mainType);
	
	mainType.Trim();
	subType=mainType.GetSubStr(mainType.FindChar('/')+1);
	subType.ToLowerCase();
	mainType.EraseFromRight(subType.GetStrLen()+1);
	mainType.ToLowerCase();

	GetTransferEncode(encode);

	if(count==1)
	{
		m_Charset="gbk";
	}
	else
	{
		value.GetSplitString(1,tempStr);
		if(tempStr.Split("=")==1||count==3)
		{
			CMyDebug::Log(TAG,2,0,"content type exception:%s",value.GetBuffer());
		}
		if(tempStr.Match("charset",NULL,true)!=-1)
		{
			tempStr.GetSplitString(1,m_Charset);
			m_Charset.Trim();
		}
		else if(tempStr.Match("boundary",NULL,true)!=-1)
		{
			m_Boundary=tempStr.GetSubStr(tempStr.FindChar('=')+1);
			m_Boundary.Trim();
			if(m_Boundary[0]=='"')
			{
				m_Boundary.Erase(0,1);
				m_Boundary.EraseFromRight(1);
				m_Boundary.Trim();
			}
		}
	}
	m_Charset.ToLowerCase();

	int i=0;
	for(i=0;i<CONTENT_ENCODE_TYPE_COUNT;i++)
	{
		if(encode==sTransferEncode[i])
		{
			encodeTypeIndex=i;
			break;
		}
	}

	if(encodeTypeIndex==-1)
	{
		CMyDebug::Log(TAG,2,0,"unknow encode type:%s",encode.GetBuffer());
		return false;
	}

	for(i=0;i<CONTENT_TYPE_COUNT;i++)
	{
		if(mainType==sContentType[i])
		{
			mainTypeIndex=i;
			break;
		}
	}
	if(mainTypeIndex!=-1)
	{
		char **subTypeList=sContentMap[mainTypeIndex].iSubType;
		int size=sContentMap[mainTypeIndex].iTypeCount;
		for(i=0;i<size;i++)
		{
			if(subType==subTypeList[i])
			{
				subTypeIndex=i;
				break;
			}
		}
		if(subTypeIndex==-1)
		{
			CMyDebug::Log(TAG,2,0,"main type:%s  unknow sub type:%s",mainType.GetBuffer(),subType.GetBuffer());
			return false;
		}
	}


	if(mainTypeIndex==-1)
	{
		CMyDebug::Log(TAG,2,0,"unknow main type:%s",mainType.GetBuffer());
		return false;
	}
	else
	{
		switch(mainTypeIndex)
		{
		case MAIN_TEXT:
			return ParseTextMailBody(data,subTypeIndex,encodeTypeIndex);
		case MAIN_MULTIPART:
			return ParseMulipartMailBody(data,subTypeIndex,encodeTypeIndex);
		case MAIN_IMAGE:
			return ParseImageMailBody(data,subTypeIndex,encodeTypeIndex);
		case MAIN_VIDEO:
			return ParseVideoMailBody(data,subTypeIndex,encodeTypeIndex);
		case MAIN_AUDIO:
			return ParseAudioMailBody(data,subTypeIndex,encodeTypeIndex);
		case MAIN_APPLICATION:
			return ParseApplicationMailBody(data,subTypeIndex,encodeTypeIndex);
		case MAIN_MESSAGE:
			return ParseMessageMailBody(data,subTypeIndex,encodeTypeIndex);
		default:
			CMyDebug::Log(TAG,2,0,"main type:%s  sub type:%s  exception!!!,unkonw type",mainType.GetBuffer(),subType.GetBuffer());
			break;
		}
	}
	return false;
}


bool CMailInfo::ParseMailHeader(CMyString& data)
{
	int index=data.FindString("\r\n\r\n");

	int count;
	if(index==-1)
	{
		count=data.Split("\r\n")-1;
	}
	else
	{
		//需要减去5个字节的结束符"\r\n.\r\n"
		m_ExternData	=data.GetSubStr(index+4,data.GetStrLen()-1);

		data.Erase(index,m_ExternData.GetStrLen()+4/*空行分隔符*/);
		count	=data.Split("\r\n");
	}

	CMyString line;
	CMyString key;
	CMyString value;
	CMyString temp;
	bool	  haveDateFeild=false;

	for(int i=0;i<count;i++)
	{
		data.GetSplitString(i,line);
		line.Split(":");
		line.GetSplitString(0,temp);
		if(temp[0]==' '||temp[0]=='\t')
		{
			if(i==0)return false;
			ParseValue(line);
			line.Trim();
			value+=line;
			if(key.CompareI("received"))
				m_Received[m_Received.size()-1]=value;
			else
				m_KeyMap.SetItem(key,value);
			printf("---fixed\n%s:%s\n",key.GetBuffer(),value.GetBuffer());
			continue;
		}
		else if(line.FindChar(':')==-1)
			continue;

		key	=temp;
		value	=line.GetSubStr(key.GetStrLen()+1,line.GetStrLen());
		ParseValue(value);

		value.Trim();

		if(key.CompareI("received"))
		{
			m_Received.push_back(value);
		}
		else
		{
			m_KeyMap.AddItem(key,value);
		}

		if(key.CompareI("Date"))
		{
			if(!ParseDate(value))
			{
				CMyDebug::Log(TAG,2,0,"the date formal is not right,date:%s",value.GetBuffer());
			}
			haveDateFeild=true;
		}

		printf("%s:%s\n",key.GetBuffer(),value.GetBuffer());
	}
	
	if(!haveDateFeild)
	{
		//没有时间域，到其他字段中提取
		if(m_Received.size()>0)
		{
			ParseDate(m_Received[0]);
		}
	}

	m_HeaderParsed	=true;

	if(m_KeyMap.GetItem(CMyString("Content-Type"),&value)==-1)
	{
		m_KeyMap.AddItem(CMyString("Content-Type"),CMyString("text/plain; charset = us-ascii"));
	}
	return true;
}

bool CMailInfo::ParseDate(CMyString& value)
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
			if(splitTxt.CompareI(CMyCalendar::sMonth[i]))
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

void CMailInfo::ParseValue(CMyString& value)
{
	CMyString subStr;
	CMyString encode;
	CMyString charset;
	CMyString content;
	CMyString decodeContent;
	CMyString bak=value;

	CMyBuffer buffer;

	int r=value.Match("=\\\?.+\\\?.+\\\?=",&subStr);
	if(r==-1)return;
	do
	{
		int num=subStr.Split("?");
		if(num!=-1)
		{
			subStr.GetSplitString(1,charset);
			subStr.GetSplitString(2,encode);
			subStr.GetSplitString(3,content);

			buffer.Alloc(content.GetStrLen()*3);
			int decodeLen=0;
			if(encode[0]=='B')
			{
				decodeLen=CEncrypt::DecodeBase64((const unsigned char *)content.GetBuffer(),content.GetStrLen(),buffer);
				decodeContent=CMyString::StringFromMem(buffer.GetBuffer(),0,decodeLen);
			}
			else if(encode[0]=='Q')
			{
				decodeLen=CEncrypt::DecodeQuotedPrintable((const unsigned char *)content.GetBuffer(),content.GetStrLen(),buffer);
				decodeContent=CMyString::StringFromMem(buffer.GetBuffer(),0,decodeLen);
			}
			if(encode[0]=='Q'||encode[0]=='B')
			{
				if(charset.CompareI("utf-8"))
				{
					
				}

				bak.Replace(subStr.GetBuffer(),decodeContent.GetBuffer(),0);

				value=bak;
				return;
			}
		}
	}while(value.MatchNext(&subStr)!=-1);
}

void CMailInfo::GetContentType(CMyString& mainType,CMyString& subType,CMyString& value)
{
	do
	{
		int index=value.FindChar('/');
		if(index==-1)break;
		mainType	=value.GetSubStr(0,index-1);
		int len=value.GetStrLen();
		int start=index+1;
		while(start<len&&(value[start]!=' '&&value[start]!='\t'&&value[start]!='\n'&&value[start]!='\r'&&value[start]!=';'))start++;
		if(start>=len)break;
		subType	=value.GetSubStr(index+1,start-1);


		mainType.Trim();
		mainType.ToLowerCase();

		subType.Trim();
		subType.ToLowerCase();
	}while(0);
	return;
}

bool CMailInfo::ParseTextMailBody(CMyString& data,int subtype,int encodeType)
{
	if(encodeType==BASE64||encodeType==QP)
	{
		CMyBuffer buffer;
		int len=0;
		if(encodeType==BASE64)
		{
			buffer.Alloc(data.GetStrLen()*2);
			len=CEncrypt::DecodeBase64((const unsigned char *)data.GetBuffer(),data.GetStrLen(),buffer);
		}
		else
		{
			buffer.Alloc(data.GetStrLen()*3);
			len=CEncrypt::DecodeQuotedPrintable((const unsigned char *)data.GetBuffer(),data.GetStrLen(),buffer);
		}
		data=CMyString::StringFromMem(buffer.GetBuffer(),0,len);
	}
	switch(subtype)
	{
	case SUB_PLAIN:
		m_Text=data;
		break;
	case SUB_HTML:
		m_Html=data;
		break;
	}
	return true;
}

bool CMailInfo::ParseImageMailBody(CMyString& data,int subtype,int encodeType)
{
	CMyDebug::Log(TAG,2,0,"main type: image subtype: %s  unsupport! you can fix it",sImageSubType[subtype]);
	return false;
}

bool CMailInfo::ParseVideoMailBody(CMyString& data,int subtype,int encodeType)
{
	CMyDebug::Log(TAG,2,0,"main type: video subtype: %s  unsupport! you can fix it",sVideoSubType[subtype]);
	return false;
}

bool CMailInfo::ParseAudioMailBody(CMyString& data,int subtype,int encodeType)
{
	CMyDebug::Log(TAG,2,0,"main type: audio subtype: %s  unsupport! you can fix it",sAudioSubType[subtype]);
	return false;
}

bool CMailInfo::ParseApplicationMailBody(CMyString& data,int subtype,int encodeType)
{
	CMyDebug::Log(TAG,2,0,"main type: application subtype: %s  unsupport! you can fix it",sApplicationSubType[subtype]);
	return false;
}

bool CMailInfo::ParseMessageMailBody(CMyString& data,int subtype,int encodeType)
{
	CMyDebug::Log(TAG,2,0,"main type: message subtype: %s  unsupport! you can fix it",sMessageSubType[subtype]);
	return false;
}

void sa()
{
	CMyDebug::Log("asdga",2,0,"main type: multipart subtype  unsupport! you can fix it");
}

bool CMailInfo::ParseMulipartMailBody(CMyString& data,int subtype,int encodeType)
{
	switch(subtype)
	{
	case SUB_ALTERNATIVE:
		{
			int count=data.Split(("--"+m_Boundary));
			CMyString temp;
			if(count!=3)
			{
				CMyDebug::Log(TAG,2,0,"exception   data: %s",data.GetBuffer());
			}
			for(int i=0;i<count;i++)
			{
				data.GetSplitString(i,temp);
				temp.Trim();
				if(temp=="--")continue;

				CMailInfo* mailInfo=new CMailInfo();
				mailInfo->Parse(temp);
				m_SubMails.push_back(mailInfo);
			}
			return true;
		}
	case SUB_MIXED:
	case SUB_PARALLEL:
	case SUB_DIGEST:
		CMyDebug::Log(TAG,2,0,"main type: multipart subtype: %s  unsupport! you can fix it",sMultipartSubType[subtype]);
		break;
	}
	return false;
}

void CMailInfo::GetTransferEncode(CMyString& encode)
{
	if(-1!=m_KeyMap.GetItem(CMyString("Content-Transfer-Encoding"),&encode))
	{
		encode.ToLowerCase();
	}
	else
	{
		encode="7bit";
	}
}

CMyString CMailInfo::GetMailSubject()
{
	CMyString subject;
	m_KeyMap.GetItem(CMyString("Subject"),&subject);
	return subject;
}