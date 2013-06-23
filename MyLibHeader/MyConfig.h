#ifndef __MY_CONFIG__
#define __MY_CONFIG__

#include "MyString.h"
#include "MyFile.h"
#include "MyHashMap.h"
#include "MyDebug.h"
#include <vector>

using namespace std;

class CMyConfig  
{
private:

	struct _KeyValue
	{
		CMyString m_Key;
		CMyString m_Value;
	};

	typedef  std::vector<_KeyValue>	_SectionInfo;
	typedef  std::vector<_KeyValue>*	_pSectionInfo;

	class HashCode
	{
	public:
		unsigned operator()(const CMyString &obj)
		{
			return ((CMyString&)obj).HashCode();
		}
	};

public:
	CMyConfig();
	virtual ~CMyConfig();

	bool		NewConfig(char* fileName);

	int			GetSectionCount();
	int			GetSectionName(int index,CMyString& sectionName);

	int			PutStringValue(char* sectionName,char* key,char* value);
	int			PutDoubleValue(char* sectionName,char* key,double value);
	int			PutFloatValue(char* sectionName,char* key,float value);
	int			PutIntValue(char* sectionName,char* key,int value);
	int			PutBooleanValue(char* sectionName,char* key,bool value);
	

	int			GetStringValue(char* sectionName,char* key,CMyString& value);
	int			GetDoubleValue(char* sectionName,char* key,double& value);
	int			GetFloatValue(char* sectionName,char* key,float& value);
	int			GetIntValue(char* sectionName,char* key,int& value);
	int			GetBooleanValue(char* sectionName,char* key,bool& value);

	int			GetKeySetCount(char* sectionName);

	int			GetKey(char* sectionName,int index,CMyString& key);
	
	void		ToString(CMyString& str);
	void		Dump(CMyString& str);
	int			Save();

private:
	bool		Parse(CMyString& value);
	void		Reset();
	int			SetKeyValue(char* sectionName,char* key,CMyString& value);
	int			GetKeyValue(char* sectionName,char* key,CMyString& value);
	void		RemoveNotes(CMyString& value);
	void		AddSectionData(CMyString& section,_pSectionInfo sectionInfo);
	void		PreProcess(CMyString& value);
private:
	
	CMyHashMap<CMyString,_pSectionInfo,HashCode>	m_ConfigInfo;
	std::vector<_pSectionInfo>			m_Storage;
	CMyString	m_ConfigContent;
	CMyFile		m_ConfigFile;
	CMyString	m_FileName;
	static char*	TAG;
};

#endif