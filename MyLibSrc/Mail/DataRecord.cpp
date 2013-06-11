#include "../MyLibHeader/Mail/DataRecord.h"

char* CDataRecord::sContentType[]=
{
	"text",
	"multipart",
	"image",
	"aduio",
	"video",
	"message",
	"application"
};

char* CDataRecord::sTextSubType[]=
{
	"plain",
	"html"
};

char* CDataRecord::sMultipartSubType[]=
{
	"mixed",
	"alternative",
	"parallel",
	"digest"
};

char* CDataRecord::sImageSubType[]=
{
	"jpeg",
	"gif"
};

char* CDataRecord::sAudioSubType[]=
{
	"basic"
};

char* CDataRecord::sVideoSubType[]=
{
	"mpeg"
};

char* CDataRecord::sMessageSubType[]=
{
	"rfc822",
	"partial",
	"external-body"
};

char* CDataRecord::sApplicationSubType[]=
{
	"octet-stream"
};

char* CDataRecord::sCharset[]=
{
	"gb18030",
	"gb2312",
	"gbk",
	"utf-8",
	"us-ascii"
};


char* CDataRecord::TAG="CDataHeader";

CDataRecord::ContentType CDataRecord::sContentMap[]=
{
	{sTextSubType		,MAX_TEXT_TYPE},
	{sMultipartSubType	,MAX_MULTIPART_TYPE},
	{sImageSubType		,MAX_IMAGE_TYPE},
	{sAudioSubType		,MAX_AUDIO_TYPE},
	{sVideoSubType		,MAX_VIDEO_TYPE},
	{sMessageSubType	,MAX_MESSAGE_TYPE},
	{sApplicationSubType	,MAX_APPLICATION_TYPE}
};

char* CDataRecord::sTransferEncode[]=
{
	"7bit",
	"8bit",
	"quoted-printable",
	"base64",
	"binary",
	"x-token"
};


CDataRecord::~CDataRecord()
{
	Reset();
}

CDataRecord::CDataRecord(CDataRecord* parent)
{
	m_Parent	=parent;

	if(m_Parent==0)
		m_Level=0;
	else
		m_Level=parent->GetLevel()+1;

	m_ContentLen=0;
}

bool CDataRecord::Parse(CMyString& data)
{
	CMyString header=data.GetSubStr(0,data.FindString("\r\n\r\n")-1);
	data.Erase(0,header.GetStrLen()+4);
	data.Trim();
	do
	{
		if(!ParseHeader(header))break;
		if(!ParseBody(data))break;
		return true;
	}while(0);
	return false;
}

void CDataRecord::GetEncode(CMyString& encode)
{
	if(m_Encode.GetStrLen())
	{
		encode=m_Encode;
		return;
	}
	if(m_Parent)
	{
		m_Parent->GetEncode(encode);
	}
	else
	{
		encode="7bit";
	}
}

void CDataRecord::GetCharset(CMyString& charset)
{
	if(m_Charset.GetStrLen())
	{
		charset=m_Charset;
		return;
	}
	if(m_Parent)
	{
		m_Parent->GetCharset(charset);
	}
	else
	{
		charset="us-ascii";
	}
}

bool CDataRecord::ParseHeader(CMyString& data)
{
	int count=data.Split("\r\n");

	CMyString line;
	CMyString value;
	CMyString key;
	CMyString temp;

	std::vector<CMyString>*	list;

	for(int i=0;i<count;i++)
	{
		data.GetSplitString(i,line);
		if(line.Split(":")>=2&&i!=0)
		{
			OnParseKeyValue(key,value);
		}

		line.GetSplitString(0,temp);
		if(temp[0]==' '||temp[0]=='\t')
		{
			if(i==0)return false;
			m_Field.GetItem(key,&list);

			ParseValue(temp);
			
			temp.Trim();
		/**/if(value[value.GetStrLen()-1]!=' '&&value[value.GetStrLen()-1]!=';')
				value+=" "+temp;
			else
				value+=temp;
			(*list)[list->size()-1]=value;
			continue;
		}
		else if(line.FindChar(':')==-1)
			continue;

		temp.Trim();
		key=temp;
		value=line.GetSubStr(line.FindChar(':')+1);
		value.Trim();

		ParseValue(value);

		if(m_Field.GetItem(key,&list)==-1)
		{
			list=new std::vector<CMyString>();
		}
		list->push_back(value);
		m_Field.AddItem(key,list);
	}

	if(count>=1)
		OnParseKeyValue(key,value);
	return true;
}

void CDataRecord::OnParseKeyValue(CMyString& key,CMyString& value)
{
	printf("%s : %s\n",key.GetBuffer(),value.GetBuffer());

	key.ToLowerCase();
	if(key=="content-type")
	{
		ParseContentType(value);
	}
	else if(key=="content-transfer-encoding")
	{
		m_Encode=value;
		m_Encode.ToLowerCase();
		m_Encode.Trim();
	}
	else if(key=="content-location"||((key=="content-id")&&m_MainType==MAIN_MULTIPART))
	{
		AddInnerResource(this);
	}
	else if(key=="content-disposition")
	{
		AddAttachment(this);
	}
	else if(key=="content-base")
	{
		CMyDebug::Log(TAG,2,0,"the content-base:%s  ,need update lib",value.GetBuffer());
	}
}

bool CDataRecord::ParseBody(CMyString& data)
{
	if(m_MainType==MAIN_MULTIPART)
	{
		CMyString	tempBoundary="--"+m_Boundary;
		CMyString	subData;
		CDataRecord* record;
		int index=data.FindString(tempBoundary);
		data.Erase(0,index);
		int count=data.Split(tempBoundary);
		for(int i=0;i<count;i++)
		{
			data.GetSplitString(i,subData);
			if(subData[0]=='-'&&subData[1]=='-')continue;
			record=new CDataRecord(this);
			record->Parse(subData);
			m_Children.push_back(record);
		}
	}
	else
	{
		if(data.GetStrLen())
			DecodeBodyData(data);
	}
	return true;
}

void CDataRecord::DecodeBodyData(CMyString&	data)
{
	CMyString encode;
	CMyString charset;

	GetEncode(encode);
	GetCharset(charset);

	int deocodeLen=0;
	bool hadDecode=false;
		
	int index=0;
	int i;
	for(i=0;i<MAX_ENCODE_TYPE;i++)
	{
		if(encode==sTransferEncode[i])
		{
			index=i;
			break;
		}
	}
	if(i>=MAX_ENCODE_TYPE)
	{
		CMyDebug::Log(TAG,2,0,"unkown encode:%s",encode.GetBuffer());
		_asm int 3;
	}

	if(index==BASE64)
	{
		m_Content.Alloc(data.GetStrLen()+2);
		m_ContentLen=CEncrypt::DecodeBase64((const unsigned char *)data.GetBuffer(),data.GetStrLen(),m_Content);
		hadDecode=true;
	}
	else if(index==Q_P)
	{
		m_Content.Alloc(data.GetStrLen()+2);
		m_ContentLen=CEncrypt::DecodeQuotedPrintable((const unsigned char *)data.GetBuffer(),data.GetStrLen(),m_Content);
		hadDecode=true;
	}
	
	if(m_MainType==MAIN_TEXT)
	{
		int j=0;
		for(j=0;j<MAX_CHARSET_TYPE;j++)
		{
			if(charset==sCharset[j])
			{
				index=j;
				break;
			}
		}
		if(j>=MAX_CHARSET_TYPE)
		{
			CMyDebug::Log(TAG,2,0,"unkown charset:%s",charset.GetBuffer());
			_asm int 3;
		}

		if(index==UTF_8)
		{
			if(hadDecode)
				data=CMyString::StringFromMem(m_Content.GetBuffer(),0,m_ContentLen);
			else
				m_Content.Alloc(data.GetStrLen()+2);

			m_ContentLen=CCharset::UTF_8ToGB2312(m_Content.GetBuffer(),data.GetBuffer(),data.GetStrLen());

			
		}
		else if(!hadDecode)
		{
			m_Content.Alloc(data.GetStrLen()+2);
			m_ContentLen=data.GetStrLen();
			memcpy(m_Content.GetBuffer(),data.GetBuffer(),data.GetStrLen());
		}
/*
		printf("\n\n");
		printf(m_Content.GetBuffer());
		printf("\n\n");
		*/
	}
	else if(!m_Parent)
	{
		CMyDebug::Log(TAG,2,0,"the type:%s can't save the data and it have no child,subtype:%s"
									,sContentType[m_MainType],sContentMap[m_MainType].iSubType[m_SubType]);
		_asm int 3;
	}
}

void CDataRecord::ParseContentType(CMyString& value)
{
	CMyString temp1;
	CMyString temp2;
	int count=value.Split(";");
	for(int i=0;i<count;i++)
	{
		value.GetSplitString(i,temp1);
		if(i==0)
		{
			temp2=temp1.GetSubStr(0,temp1.FindChar('/')-1);
			temp1.Erase(0,temp2.GetStrLen()+1);
			temp1.ToLowerCase();
			temp2.ToLowerCase();
			int j=0;
			for(;j<MAX_MAIN_TYPE;j++)
			{
				if(temp2==sContentType[j])
				{
					m_MainType=j;
					break;
				}
			}
			if(j>=MAX_MAIN_TYPE)
			{
				CMyDebug::Log(TAG,2,0,"unkonw main type:%s",temp2.GetBuffer());
			}
			else
			{
				j=0;
				for(;j<sContentMap[m_MainType].iTypeCount;j++)
				{
					if(temp1==sContentMap[m_MainType].iSubType[j])
					{
						m_SubType=j;
						break;
					}
				}
				if(j>=sContentMap[m_MainType].iTypeCount)
				{
					CMyDebug::Log(TAG,2,0,"main type:%s  unkonw subtype:%s",temp2.GetBuffer(),temp1.GetBuffer());
				}
			}
		}
		else
		{
			temp2=temp1.GetSubStr(0,temp1.FindChar('=')-1);
			temp1.Erase(0,temp2.GetStrLen()+1);

			temp1.Trim();
			temp2.Trim();

			temp2.ToLowerCase();
			if(temp2=="boundary")
			{
				if(temp1[0]=='"')
				{
					temp1.Erase(0,1);
					temp1.EraseFromRight(1);
				}
				m_Boundary=temp1;
			}
			else if(temp2=="charset")
			{
				if(temp1[0]=='"')
				{
					temp1.Erase(0,1);
					temp1.EraseFromRight(1);
				}
				m_Charset=temp1;
			}
		}
	}
}

int CDataRecord::GetValue(char* key,CMyString& value,int index)
{
	std::vector<CMyString>*	list;
	if(m_Field.GetItem(CMyString(key),&list)==-1)return -1;
	value=(*list)[index];
	return 1;
}

void CDataRecord::ParseValue(CMyString&	value)
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

			buffer.Alloc(content.GetStrLen()+2);
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
					decodeLen=CCharset::UTF_8ToGB2312(buffer.GetBuffer(),decodeContent.GetBuffer(),decodeContent.GetStrLen());
					decodeContent=CMyString::StringFromMem(buffer.GetBuffer(),0,decodeLen);
//					CMyDebug::Log(TAG,2,0,"unsupport the charset:%s",charset.GetBuffer());
				}

				bak.Replace(subStr.GetBuffer(),decodeContent.GetBuffer(),0);

				value=bak;
				return;
			}
		}
	}while(value.MatchNext(&subStr)!=-1);
}

void CDataRecord::GetMainType(CMyString& mainType)
{
	mainType=sContentType[m_MainType];
}

void CDataRecord::GetSubType(CMyString&  subType)
{
	subType=sContentMap[m_MainType].iSubType[m_SubType];
}

void CDataRecord::AddAttachment(CDataRecord* attachment)
{
	if(m_Parent)
		m_Parent->AddAttachment(attachment);
}

void CDataRecord::AddInnerResource(CDataRecord* innerResource)
{
	if(m_Parent)
		m_Parent->AddInnerResource(innerResource);
}

void CDataRecord::DumpState(CMyString& state)
{
	CMyString pre;
	int i=0;
	int size=0;
	std::vector<CMyString>* list;
	CMyString key;

	
	for(;i<m_Level;i++)
		pre+="------";
	

	state+=pre;
	state+="<StartRecord>\n";

	size=m_Field.Size();
	for(i=0;i<size;i++)
	{
		m_Field.GetKey(i,key);
		m_Field.GetItem(key,&list);

		int len=list->size();
		for(int j=0;j<len;j++)
		{
			state+=pre;
			state+=key;
			state+=" : ";
			state+=(*list)[j];
			state+="\n";
		}
	}

	state+="\n";

	if(m_Children.size()==0)
	{
		state+=pre;
		state+="+++++++++data+++++++++\n";
	}
	else
	{
		for(int j=0;j<m_Children.size();j++)
			m_Children[j]->DumpState(state);
	}

	state+=pre;
	state+="<EndRecord>\n\n";
}

void CDataRecord::Reset()
{
	int size=m_Children.size();
	int i=0;

	for(;i<size;i++)
		delete m_Children[i];

	size=m_Field.Size();

	std::vector<CMyString>*	list;
	CMyString key;
	for(i=0;i<size;i++)
	{
		m_Field.GetKey(i,key);
		m_Field.GetItem(key,&list);
		delete list;
	}
	m_Field.Clear();
	m_Children.clear();
}

int	CDataRecord::GetMailTextPlain(CMyString& text)
{
	if((m_MainType == MAIN_TEXT)&&(m_SubType == SUB_PLAIN))
	{
		text=CMyString::StringFromMem(m_Content.GetBuffer(),0,m_ContentLen);
		return 1;
	}
	else
	{
		int size	=m_Children.size();
		for(int i=0;i<size;i++)
		{
			if(-1==m_Children[i]->GetMailTextPlain(text))continue;
			return 1;
		}
	}
	return -1;
}

int	CDataRecord::GetMailTextHtml(CMyString& html)
{
	if(m_MainType=MAIN_TEXT&&m_SubType==SUB_HTML)
	{
		html=CMyString::StringFromMem(m_Content.GetBuffer(),0,m_ContentLen);
		return 1;
	}
	else
	{
		int size	=m_Children.size();
		for(int i=0;i<size;i++)
		{
			if(-1==m_Children[i]->GetMailTextHtml(html))continue;
			return 1;
		}
	}
	return -1;
}