#ifndef __EXTERN_MAIL_CLASS__
#define __EXTERN_MAIL_CLASS__

#include "DataRecord.h"

class CExternData
{
public:
	CExternData(CDataRecord* externData);
	virtual void	ParseData();

protected:

	char* GetDataBuffer(){return m_Data->m_Content.GetBuffer();}
	unsigned GetDataSize(){return m_Data->m_ContentLen;}

private:

	virtual void	OnParseKeyValue(CMyString& key,CMyString& value)=0;

protected:

	CDataRecord*		m_Data;
};

class CAttachment:public CExternData
{
public:
	CAttachment(CDataRecord* attachment);
	virtual ~CAttachment();

	int		GetAttachmentFileName(CMyString& fileName);
	int		GetAttachmentExt(CMyString& ext);
	char*	GetAttachmentBuffer();
	unsigned GetAttachmentSize();

private:

	virtual void	OnParseKeyValue(CMyString& key,CMyString& value);	

private:
	static	char*	TAG;
	CMyString		m_AttachmentName;
};

class CInnerResource:public CExternData
{
public:
	CInnerResource(CDataRecord* innerResource);
	virtual ~CInnerResource();

	int		GetResourceID(CMyString& id);
	int		GetResourceLink(CMyString& link);
	char*	GetResourceBuffer();
	unsigned GetResourceSize();
	int		GetResourceName(CMyString& name);
private:

	virtual void	OnParseKeyValue(CMyString& key,CMyString& value);

private:

	CMyString	m_Link;
	CMyString	m_ID;
	CMyString	m_ResourceName;
};

#endif