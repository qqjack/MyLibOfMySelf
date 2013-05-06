#include "MailInfo.h"
#include "MyDebug.h"

#define  SWITCH

#ifndef SWITCH
	#define Log	CMyDebug::LogNULL
#else
	#define Log CMyDebug::Log
#endif

#define CONTENT_TYPE_COUNT (7)
#define CONTENT_ENCODE_TYPE_COUNT (7)

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

CMailInfo::CMailInfo()
{
	m_HeaderParsed	=false;
}

CMailInfo::~CMailInfo()
{

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
	CMyString charset;
	CMyString encode;
	int index1=-1,index2=-1,index3=-1;
	if(-1==m_KeyMap.GetItem(CMyString("Content-Type"),&value))return false;
	GetContentType(mainType,subType,value);
	GetCharset(charset,value);
	GetTransferEncode(encode);
	int i=0;
	for(i=0;i<CONTENT_ENCODE_TYPE_COUNT;i++)
	{
		if(encode==sTransferEncode[i])
		{
			index3=i;
			break;
		}
	}

	if(index3==-1)
	{
		CMyDebug::Log(TAG,2,0,"unknow encode type:%s",encode.GetBuffer());
		return false;
	}

	for(i=0;i<CONTENT_TYPE_COUNT;i++)
	{
		if(mainType==sContentType[i])
		{
			index1=i;
			break;
		}
	}
	if(index1!=-1)
	{
		char **subTypeList=sContentMap[index1].iSubType;
		int size=sContentMap[index1].iTypeCount;
		for(i=0;i<size;i++)
		{
			if(subType==subTypeList[i])
			{
				index2=i;
				break;
			}
		}
		if(index2==-1)
		{
			CMyDebug::Log(TAG,2,0,"main type:%s  unknow sub type:%s",mainType.GetBuffer(),subType.GetBuffer());
			return false;
		}
	}


	if(index1==-1)
	{
		CMyDebug::Log(TAG,2,0,"unknow main type:%s",mainType.GetBuffer());
		return false;
	}
	else
	{
		switch(index1)
		{
		case MAIN_TEXT:
			return ParseTextMailBody(data,index2,index3);
		case MAIN_MULTIPART:
			return ParseMulipartMailBody(data,index2,index3);
		case MAIN_IMAGE:
			return ParseImageMailBody(data,index2,index3);
		case MAIN_VIDEO:
			return ParseVideoMailBody(data,index2,index3);
		case MAIN_AUDIO:
			return ParseAudioMailBody(data,index2,index3);
		case MAIN_APPLICATION:
			return ParseApplicationMailBody(data,index2,index3);
		case MAIN_MESSAGE:
			return ParseMessageMailBody(data,index2,index3);
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
		//此时数据仅包含邮件头
		count=data.Split("\r\n")-1;
	}
	else
	{
		//需要减去5个字节的结束符"\r\n.\r\n"
		m_ExternData	=data.GetSubStr(index+4,data.GetStrLen()-5);
		data.Erase(index,m_ExternData.GetStrLen()+5/*结束符*/+4/*空行分隔符*/);
		count	=data.Split("\r\n");
	}
	CMyString line;
	CMyString key;
	CMyString value;
	CMyString temp;

	for(int i=0;i<count;i++)
	{
		data.GetSplitString(i,line);
		line.Split(":");
		line.GetSplitString(0,temp);
		if(temp[0]==' '||temp[0]=='\t')
		{
			if(i==0)return false;
			value+="\r\n"+line;
			m_KeyMap.SetItem(key,value);
			printf("---fixed\n%s:%s\n",key.GetBuffer(),value.GetBuffer());
			continue;
		}
		else if(line.FindChar(':')==-1)
			continue;

		key	=temp;
		value	=line.GetSubStr(key.GetStrLen()+1,line.GetStrLen());
		if(key=="Subject")
		{
			if(value.Match("=\?.+\?.+\?=",NULL)!=-1)
			{
				int num=value.Split("?");
				if(num!=-1)
				{
					CMyString encode;
					CMyString charset;
					CMyString content;
					value.GetSplitString(1,charset);
					value.GetSplitString(2,encode);
					value.GetSplitString(3,content);
				}
			}
		}
		value.Trim();
		m_KeyMap.AddItem(key,value);

		printf("%s:%s\n",key.GetBuffer(),value.GetBuffer());
	}
	m_HeaderParsed	=true;

	if(m_KeyMap.GetItem(CMyString("Content-Type"),&value)==-1)
	{
		m_KeyMap.AddItem(CMyString("Content-Type"),CMyString("text/plain; charset = us-ascii"));
	}
	return true;
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

void CMailInfo::GetCharset(CMyString& charset,CMyString& value)
{
	int index=value.FindString("charset");
	do
	{
		if(index==-1)break;
		int start=index;
		int len =value.GetStrLen();
		while(start<len&&value[start]==' ')start++;
		if(start>=len)break;
		index	=start;
		while(start<len&&(value[start]!=' '&&value[start]!='\t'&&value[start]!='\n'&&value[start]!='\r'&&value[start]!=';'))start++;
		if(start>=len)break;
		charset	=value.GetSubStr(index,start-1);
		charset.ToLowerCase();
	}while(0);
}

bool CMailInfo::ParseTextMailBody(CMyString& data,int subtype,int encodeType)
{
	switch(subtype)
	{
	case SUB_PLAIN:
		break;
	case SUB_HTML:
		break;
	}
	return false;
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

bool CMailInfo::ParseMulipartMailBody(CMyString& data,int subtype,int encodeType)
{
	switch(subtype)
	{
	case SUB_ALTERNATIVE:
		{
			break;
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
}