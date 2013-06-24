#include "MyConfig.h"


char* CMyConfig::TAG="CMyConfig";

CMyConfig::CMyConfig()
{

}

CMyConfig::~CMyConfig()
{
	Reset();
}

bool CMyConfig::NewConfig(char* fileName)
{
	Reset();
	
	m_FileName	=	fileName;
	if(-1==m_ConfigFile.Open(fileName,CMyFile::FILE_NEW_OR_OPEN))return false;
	int bufSize=m_ConfigFile.GetFileSize()+1;
	m_ConfigContent.Resize(bufSize);
	m_ConfigFile.read(m_ConfigContent,bufSize-1);
	m_ConfigContent.SetAt(bufSize-1,0);
	PreProcess(m_ConfigContent);
	return Parse(m_ConfigContent);
}

int	CMyConfig::GetSectionCount()
{
	return m_ConfigInfo.Size();
}

int	CMyConfig::GetSectionName(int index,CMyString& sectionName)
{
	if(index>=m_ConfigInfo.Size())return -1;
	m_ConfigInfo.GetKey(index,sectionName);
	return 1;
}


int	CMyConfig::PutStringValue(char* sectionName,char* key,char* value)
{
	SetKeyValue(sectionName,key,CMyString(value));
	return 1;
}

int	CMyConfig::PutDoubleValue(char* sectionName,char* key,double value)
{
	CMyString strValue=CMyString::Format("%f",value);
	SetKeyValue(sectionName,key,strValue);
	return 1;
}

int	CMyConfig::PutFloatValue(char* sectionName,char* key,float value)
{
	CMyString strValue=CMyString::Format("%f",value);
	SetKeyValue(sectionName,key,strValue);
	return 1;
}

int	CMyConfig::PutIntValue(char* sectionName,char* key,int value)
{
	CMyString strValue=CMyString::Format("%d",value);
	SetKeyValue(sectionName,key,strValue);
	return 1;
}

int	CMyConfig::PutBooleanValue(char* sectionName,char* key,bool value)
{
	CMyString strValue=value?"true":"fasle";
	SetKeyValue(sectionName,key,strValue);
	return 1;
}

	

int	CMyConfig::GetStringValue(char* sectionName,char* key,CMyString& value)
{
	return GetKeyValue(sectionName,key,value);
}

int	CMyConfig::GetDoubleValue(char* sectionName,char* key,double& value)
{
	CMyString orginValue;
	int r=GetKeyValue(sectionName,key,orginValue);
	if(r!=-1)
	{
		value	=orginValue.ToDouble();
		return 1;
	}
	return r;
}

int	CMyConfig::GetFloatValue(char* sectionName,char* key,float& value)
{
	CMyString orginValue;
	int r=GetKeyValue(sectionName,key,orginValue);
	if(r!=-1)
	{
		value	=orginValue.ToDouble();
		return 1;
	}
	return r;
}

int	CMyConfig::GetIntValue(char* sectionName,char* key,int& value)
{
	CMyString orginValue;
	int r=GetKeyValue(sectionName,key,orginValue);
	if(r!=-1)
	{
		value	=orginValue.ToInt();
		return 1;
	}
	return r;
}

int	CMyConfig::GetBooleanValue(char* sectionName,char* key,bool& value)
{
	CMyString orginValue;
	int r=GetKeyValue(sectionName,key,orginValue);
	if(r!=-1)
	{
		orginValue.ToLowerCase();
		if(orginValue=="true")
			value=true;
		else if(orginValue=="false")
			value=false;
		else 
			return -1;
		return 1;
	}
	return r;
}


int	CMyConfig::GetKeySetCount(char* sectionName)
{
	_pSectionInfo sectionInfo;
	int r=m_ConfigInfo.GetItem(CMyString(sectionName),&sectionInfo);
	if(r!=-1)
	{
		return sectionInfo->size();
	}
	return r;
}


int	CMyConfig::GetKey(char* sectionName,int index,CMyString& key)
{
	_pSectionInfo sectionInfo;
	int r=m_ConfigInfo.GetItem(CMyString(sectionName),&sectionInfo);
	if(r!=-1)
	{
		int size=sectionInfo->size();
		if(index>=size)return -1;
		key=((*sectionInfo)[index]).m_Key;
	}
	return r;
}


bool CMyConfig::Parse(CMyString& value)
{
	CMyString content=value;
	CMyString section="Defalut";

	CMyString line;
	_KeyValue		keyValue;
	_SectionInfo	sectionInfo;

	RemoveNotes(content);

	int count=content.Split("\n");

	for(int i=0;i<count;i++)
	{
		content.GetSplitString(i,line);
		line.Trim();
		if(line[0]=='['&&line[line.GetStrLen()-1]==']')
		{
			if(section!="Defalut")AddSectionData(section,&sectionInfo);
			section=line;
			section.Erase(0,1);
			section.EraseFromRight(1);
			section.Trim();
			sectionInfo.clear();
		}
		else
		{
			int index=line.FindChar('=');
			if(index==-1)continue;

			keyValue.m_Key=line.GetSubStr(0,index-1);
			keyValue.m_Key.Trim();
			keyValue.m_Value=line.GetSubStr(index+1,line.GetStrLen()-1);
			keyValue.m_Value.Trim();
			sectionInfo.push_back(keyValue);
		}
	}
	if(count>0)
	{
		AddSectionData(section,&sectionInfo);
	}
	return true;
}

void CMyConfig::AddSectionData(CMyString& section,_pSectionInfo sectionInfo)
{
	_pSectionInfo orginSection;
	int r=m_ConfigInfo.GetItem(section,&orginSection);
	if(r!=-1)
	{
		orginSection->insert(orginSection->end(),sectionInfo->begin(),sectionInfo->end());
	}
	else
	{
		orginSection=new _SectionInfo(*sectionInfo);
		m_Storage.push_back(orginSection);
		m_ConfigInfo.AddItem(section,orginSection);
	}
}

void CMyConfig::RemoveNotes(CMyString& value)
{
	int len=value.GetStrLen();
	int start=0,end=0;
	while(start<len)
	{
		while((value[start]!='#'&&value[start]!=';')&&start<len)start++;
		if(start>=len)break;
		if(value[start]==';'&&(start!=0&&value[start-1]!='\n'))
		{
			start++;
			continue;
		}
		end=start;
		while(value[end]!='\n'&&end<len)end++;
		value.Erase(start,end-start);
		len-=end-start;
	}
}

void CMyConfig::Reset()
{
	m_ConfigInfo.Clear();
	for(int i=0;i<m_Storage.size();i++)
	{
		delete m_Storage[i];
	}
	m_Storage.clear();
}

int	CMyConfig::SetKeyValue(char* sectionName,char* key,CMyString& value)
{
	_pSectionInfo sectionInfo;
	_KeyValue keyValue;
	bool	  newAdd=false;

	int r=m_ConfigInfo.GetItem(CMyString(sectionName),&sectionInfo);
	if(r!=-1)
	{
		
		int size=sectionInfo->size();
		int i=0;
		for(;i<size;i++)
		{
			if((*sectionInfo)[i].m_Key==key)break;
		}
		if(i>=size)
		{
			keyValue.m_Key=key;
			keyValue.m_Value=value;
			sectionInfo->push_back(keyValue);
			newAdd=true;
		}

		int index=0;
		//搜索段头
		do
		{
			index=m_ConfigContent.FindString(sectionName);
			index+=strlen(sectionName);
			while(index<m_ConfigContent.GetStrLen()&&index==' ')index++;

			if(index>=m_ConfigContent.GetStrLen())
			{
				CMyDebug::Log(TAG,2,0,"exception of program! fix it!");
				return -1;
			}
			if(m_ConfigContent[index]!=']')
				continue;
			index++;
			while(index<m_ConfigContent.GetStrLen()&&m_ConfigContent[index]==' ')index++;
			if(m_ConfigContent[index]!='\n')
				continue;
			else
				break;
		}while(1);
		
		if(!newAdd)
		{
			int innerIndex=index;
			int keyLen=strlen(key);
			do
			{
				innerIndex=m_ConfigContent.FindString(key,innerIndex);
				if(innerIndex==-1)
				{
					CMyDebug::Log(TAG,2,0,"can't find the key,the parse part have a problem!");
					return -1;
				}
				innerIndex+=keyLen;
				//跳过空格
				while(innerIndex<m_ConfigContent.GetStrLen()&&m_ConfigContent[innerIndex]==' ')innerIndex++;
				
				if(m_ConfigContent[innerIndex]=='=')
				{
					m_ConfigContent.Replace((*sectionInfo)[i].m_Value,value);
					(*sectionInfo)[i].m_Value=value;
					break;
				}
			}while(1);
			return 1;
		}
		//定位段尾，并在段尾添加新数据

		int start=index;
		bool isNextLine=false;
		while(start<m_ConfigContent.GetStrLen())
		{
			if(m_ConfigContent[start]=='\n')isNextLine=true;
			if(m_ConfigContent[start]=='['&&isNextLine)break;
			start++;
		}

		if(start>=m_ConfigContent.GetStrLen())
		{
			m_ConfigContent+="\n";
			m_ConfigContent+=key;
			m_ConfigContent+=" = ";
			m_ConfigContent+=value;
		}
		else
		{
			CMyString newContent;
			newContent+="\n";
			newContent+=key;
			newContent+=" = ";
			newContent+=value;

			m_ConfigContent=m_ConfigContent.GetSubStr(0,start-2)+newContent+m_ConfigContent.GetSubStr(start-1);
		}
		return 1;
	}
	else
	{
		sectionInfo =new _SectionInfo;
		keyValue.m_Key=key;
		keyValue.m_Value=value;
		sectionInfo->push_back(keyValue);
		m_Storage.push_back(sectionInfo);
		m_ConfigInfo.AddItem(CMyString(sectionName),sectionInfo);

		m_ConfigContent+="\n[";
		m_ConfigContent+=sectionName;
		m_ConfigContent+="]\n";
		m_ConfigContent+=key;
		m_ConfigContent+=" = ";
		m_ConfigContent+=value;
	}
	return r;
}

int	CMyConfig::GetKeyValue(char* sectionName,char* key,CMyString& value)
{
	_pSectionInfo sectionInfo;
	int r=m_ConfigInfo.GetItem(CMyString(sectionName),&sectionInfo);
	if(r!=-1)
	{
		int size=sectionInfo->size();
		for(int i=0;i<size;i++)
		{
			if((*sectionInfo)[i].m_Key==key)
			{
				value	=(*sectionInfo)[i].m_Value;
				return 1;
			}
		}
		r=-1;
	}
	return r;
}

void CMyConfig::PreProcess(CMyString& value)
{
	value.ReplaceAll("\r\n","\n");
}

void CMyConfig::ToString(CMyString& str)
{
	CMyString key;
	_pSectionInfo sectionInfo;
	int sectionCount=m_ConfigInfo.Size();

	str="";
	for(int i=0;i<sectionCount;i++)
	{
		m_ConfigInfo.GetKey(i,key);
		m_ConfigInfo.GetItem(key,&sectionInfo);
		str+="[";
		str+=key;
		str+="]\r\n";
		int keyValueSize=sectionInfo->size();
		for(int j=0;j<keyValueSize;j++)
		{
			str+=(*sectionInfo)[j].m_Key;
			str+=" = ";
			str+=(*sectionInfo)[j].m_Value;
			str+="\r\n";
		}

		if(keyValueSize==0)
		{
			str+="\r\n";
		}
	}
}

int CMyConfig::Save()
{
	m_ConfigContent.ReplaceAll("\n","\r\n");
	if(m_ConfigFile.Open(m_FileName,CMyFile::FILE_NEW_OR_CLEAR)==-1)return -1;
	return m_ConfigFile.write(m_ConfigContent,m_ConfigContent.GetStrLen());
}

void CMyConfig::Dump(CMyString& str)
{
	str=m_ConfigContent;
}