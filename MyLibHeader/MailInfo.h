#ifndef __MAIL_INFO__
#define __MAIL_INFO__
#include "MyString.h"
#include "MyHashMap.h"
#include "MyTime.h"
#include <vector>

using namespace std;

//content type text,sub type
#define MAIN_TEXT	(0)
#define SUB_PLAIN	(0)
#define SUB_HTML	(1)

//content type mulipart,sub type
#define MAIN_MULTIPART	(1)
#define SUB_MIXED	(0)
#define SUB_ALTERNATIVE  1
#define SUB_PARALLEL     (2)
#define SUB_DIGEST		 (3)

//content type image,sub type
#define MAIN_IMAGE	(2)
#define SUB_JPEG	(0)
#define SUB_GIF		(1)

//content type aduio,sub type
#define MAIN_AUDIO  (3)
#define SUB_BASIC  (0)

//content type video,sub type
#define MAIN_VIDEO  (4)
#define SUB_MPEG   (0)

//content type message,sub type
#define MAIN_MESSAGE (5)
#define SUB_RFC822  (0)
#define SUB_PARTIAL (1)
#define SUB_EXTERNALBODY (2)

//content type application,sub type
#define MAIN_APPLICATION (6)
#define SUB_OCTETSTREAM (0)

class CMailInfo  
{
private:
	struct ContentType
	{
		char **	iSubType;
		int	    iTypeCount;
	};

public:
	CMailInfo();
	virtual ~CMailInfo();

	bool		Parse(CMyString& data);
	bool		ParseMailBody(CMyString& data);
	bool		ParseMailHeader(CMyString& data);
	bool		IsMailHeaderParsed(){return m_HeaderParsed;}
	bool		ResetParse(){m_HeaderParsed=false;}
	
	CMyString	GetTextMailContent();
	CMyString	GetHtmlMailContent();
	CMyTime		GetMailTime(){return m_Time;}

private:
	void		GetContentType(CMyString& mainType,CMyString& subType,CMyString& value);
	void		GetCharset(CMyString& charset,CMyString& value);
	void		GetTransferEncode(CMyString& encode);
	void		NotifyUnsupport(CMyString content);
	
	bool		ParseTextMailBody(CMyString& data,int subtype,int encodeType,CMyString& charset);
	bool		ParseImageMailBody(CMyString& data,int subtype,int encodeType,CMyString& charset);
	bool		ParseVideoMailBody(CMyString& data,int subtype,int encodeType,CMyString& charset);
	bool		ParseAudioMailBody(CMyString& data,int subtype,int encodeType,CMyString& charset);
	bool		ParseApplicationMailBody(CMyString& data,int subtype,int encodeType,CMyString& charset);
	bool		ParseMessageMailBody(CMyString& data,int subtype,int encodeType,CMyString& charset);
	bool		ParseMulipartMailBody(CMyString& data,int subtype,int encodeType,CMyString& charset);

	void		GetSubject(CMyString& subject,CMyString& vaule);
	void		ParseValue(CMyString& value);
	bool		ParseDate(CMyString& value);
private:
	
	CMyHashMap<CMyString,CMyString>	m_KeyMap;
	std::vector<CMyString>			m_Received;

	CMyTime		m_Time;

	bool		m_HeaderParsed;

	CMyString	m_ExternData;

	static char* sContentType[];
	static char* sTextSubType[];
	static char* sMultipartSubType[];
	static char* sImageSubType[];
	static char* sAudioSubType[];
	static char* sVideoSubType[];
	static char* sMessageSubType[];
	static char* sApplicationSubType[];
	static char* sTransferEncode[];
	static ContentType sContentMap[];
	static char* TAG;
};
#endif