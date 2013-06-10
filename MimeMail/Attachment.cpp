#include "Attachment.h"

//------------------CExternData--------------
CExternData::CExternData(CDataRecord* externData)
{
	m_Data	=externData;
}

void CExternData::ParseData()
{
	CMyString key;
	CMyString value;
	std::vector<CMyString>*	list;

	int size	=m_Data->m_Field.Size();

	for(int i=0;i<size;i++)
	{
		m_Data->m_Field.GetKey(i,key);
		m_Data->m_Field.GetItem(key,&list);

		key.ToLowerCase();

		int count=list->size();
			
		for(int j=0;j<count;j++)
		{
			value=(*list)[j];
			OnParseKeyValue(key,value);
		}
	}
}

//------------------CAttachment--------------
char* CAttachment::TAG="CAttachment";

CAttachment::CAttachment(CDataRecord* attachment):CExternData(attachment)
{
}

CAttachment::~CAttachment()
{

}

int CAttachment::GetAttachmentFileName(CMyString& fileName)
{
	if(m_AttachmentName.GetStrLen()==0)return -1;
	fileName	=m_AttachmentName;
	return 1;
}

int CAttachment::GetAttachmentExt(CMyString& ext)
{
	if(m_AttachmentName.GetStrLen()==-1)return -1;
	int count=m_AttachmentName.Split(".");
	if(count==1)return 1;
	m_AttachmentName.GetSplitString(count-1,ext);
	return 1;
}

char* CAttachment::GetAttachmentBuffer()
{
	return GetDataBuffer();
}

unsigned CAttachment::GetAttachmentSize()
{
	return GetDataSize();
}

void CAttachment::OnParseKeyValue(CMyString& key,CMyString& value)
{
	if(key=="content-type")
	{
		int index=value.FindString("name");
		if(index!=-1&&value[index+4]==' '||value[index+4]=='=')
		{
			int startIndex=index+4;
			while(startIndex<value.GetStrLen()-1&&value[startIndex]==' ')startIndex++;
			if(startIndex>=value.GetStrLen()||value[startIndex]!='=')return;
			int endIndex=++startIndex;
			while(endIndex<value.GetStrLen()-1&&value[endIndex]!=' ')endIndex++;
			m_AttachmentName=CMyString::StringFromMem(value,startIndex,endIndex-startIndex+1);
			if(m_AttachmentName[0]=='"')
			{
				m_AttachmentName.Erase(0,1);
				m_AttachmentName.EraseFromRight(1);
			}
		}
	}
	else if(key=="content-disposition")
	{
		CMyString attachmentType;
		value.Split(";");
		value.GetSplitString(0,attachmentType);
		attachmentType.Trim();
		attachmentType.ToLowerCase();
		if(attachmentType!="attachment")
		{
			CMyDebug::Log(TAG,2,0,"unkonw attachment type:%s",attachmentType.GetBuffer());
		}
	}
}

//-------------------CInnerResource---------------

CInnerResource::CInnerResource(CDataRecord* innnerResource):CExternData(innnerResource)
{
}

CInnerResource::~CInnerResource()
{

}

int	CInnerResource::GetResourceID(CMyString& id)
{
	if(m_ID.GetStrLen()==0)return -1;
	id	=m_ID;
	return 1;
}

int	CInnerResource::GetResourceLink(CMyString& link)
{
	if(m_Link.GetStrLen()==0)return -1;
	link	=m_Link;
	return 1;
}

int	CInnerResource::GetResourceName(CMyString& name)
{
	if(m_ResourceName.GetStrLen()==0)return -1;
	name	=m_ResourceName;
	return 1;
}

char* CInnerResource::GetResourceBuffer()
{
	return GetDataBuffer();
}

unsigned CInnerResource::GetResourceSize()
{
	return GetDataSize();
}

void CInnerResource::OnParseKeyValue(CMyString& key,CMyString& value)
{
	if(key=="content-type")
	{
		int index=value.FindString("name");
		if(index!=-1&&value[index+4]==' '||value[index+4]==':')
		{
			int startIndex=index+4;
			while(startIndex<value.GetStrLen()-1&&value[startIndex]==' ')startIndex++;
			if(startIndex>=value.GetStrLen()||value[startIndex]!='=')return;
			int endIndex=++startIndex;
			while(endIndex<value.GetStrLen()-1&&value[endIndex]!=' ')startIndex++;
			m_ResourceName=CMyString::StringFromMem(value,startIndex,endIndex-startIndex+1);
			if(m_ResourceName[0]=='"')
			{
				m_ResourceName.Erase(0,1);
				m_ResourceName.EraseFromRight(1);
			}
		}
	}
	else if(key=="content-id")
	{
		if(value[0]=='<')
		{
			value.Erase(0,1);
			value.EraseFromRight(1);
		}
		m_ID=value;
	}
	else if(key=="content-location")
	{
		m_Link	=value;
	}
}