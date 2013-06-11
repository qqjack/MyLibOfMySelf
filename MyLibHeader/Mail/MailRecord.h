#ifndef __MAIL_RECORD__
#define __MAIL_RECORD__

#include "DataRecord.h"
#include "Attachment.h"
#include "../MyLibHeader/MyTime.h"

class CMailRecord:public CDataRecord
{
public:
	CMailRecord();
	virtual ~CMailRecord();

	CMyTime		GetMailTime(){return m_Time;}
	void		GetMailSubject(CMyString& subject){subject= m_Subject;}
	void		GetMailFrom(CMyString& from){from= m_From;}
	void		GetMailFromAddress(CMyString& addr);

	CAttachment*	GetAttachment(int index);
	CInnerResource*	GetInnerResourceByID(CMyString& innerResourceID);
	CInnerResource* GetInnerResourceByLink(CMyString& innerResourceLink);
	int				GetAttachmentCount(){return m_Attachment.size();}


	virtual void OnParseKeyValue(CMyString& key,CMyString& value);

	virtual void DumpState(CMyString& state);
	virtual void Reset();

	int			GetChildrenCount(){return m_Children.size();}
	CDataRecord*	GetChildAt(int index);

private:

	virtual void AddAttachment(CDataRecord* attachement);
	virtual void AddInnerResource(CDataRecord* innerResource);
	
	bool		 ParseDate(CMyString& value);
	void		 Recyle();
private:
	
	std::vector<CAttachment*>		m_Attachment;
	std::vector<CInnerResource*>		m_InnerResource;

	CMyTime		m_Time;
	CMyString	m_Subject;
	CMyString   m_From;
};

#endif