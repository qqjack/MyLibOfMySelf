#ifndef __DATA_RECORD__
#define __DATA_RECORD__

#include "../MyString.h"
#include "../MyDebug.h"
#include "../Charset.h"
#include "../MyHashMap.h"
#include "../MyBuffer.h"
#include "../Encrypt.h"

#include <vector>
using namespace std;

//content type text,sub type
#define MAIN_TEXT	(0)
#define SUB_PLAIN	(0)
#define SUB_HTML	(1)

#define MAX_TEXT_TYPE (2)

//content type mulipart,sub type
#define MAIN_MULTIPART	(1)
#define SUB_MIXED	(0)
#define SUB_ALTERNATIVE  1
#define SUB_PARALLEL     (2)
#define SUB_DIGEST		 (3)

#define MAX_MULTIPART_TYPE (4)

//content type image,sub type
#define MAIN_IMAGE	(2)
#define SUB_JPEG	(0)
#define SUB_GIF		(1)

#define MAX_IMAGE_TYPE (2)

//content type aduio,sub type
#define MAIN_AUDIO  (3)
#define SUB_BASIC  (0)

#define MAX_AUDIO_TYPE (1)

//content type video,sub type
#define MAIN_VIDEO  (4)
#define SUB_MPEG   (0)

#define MAX_VIDEO_TYPE (1)

//content type message,sub type
#define MAIN_MESSAGE (5)
#define SUB_RFC822  (0)
#define SUB_PARTIAL (1)
#define SUB_EXTERNALBODY (2)

#define MAX_MESSAGE_TYPE (3)

//content type application,sub type
#define MAIN_APPLICATION (6)
#define SUB_OCTETSTREAM (0)

#define MAX_APPLICATION_TYPE (1)

//transfer encode
#define BIT_7			(0)
#define BIT_8			(1)
#define Q_P				(2)
#define BASE64			(3)
#define BINARY			(4)
#define X_TOKEN			(5)

#define MAX_ENCODE_TYPE (6)


//charset
#define GB_18030  (0)
#define GB_2312	  (1)
#define GBK		  (2)
#define UTF_8	  (3)
#define US_ASCII  (4)

#define MAX_CHARSET_TYPE (5)


#define MAX_MAIN_TYPE (7)

class CDataRecord  
{
private:

	struct ContentType
	{
		char **	iSubType;
		int	    iTypeCount;
	};

public:

	enum Data_Type
	{
		INNER_RESOURCE_TYPE,  //内嵌资源
		ATTACHMENT_TYPE,		//附件
		HTML_TYPE,				//html内容
		PLAIN_TYPE,				//纯文本内容
		MUTILPART_TYPE,
		OTHER_TYPE				//其他类型
	};

public:
	CDataRecord(CDataRecord* parent);
	virtual ~CDataRecord();

	Data_Type	GetType(){return m_Type;}
	bool		Parse(CMyString& data);

	void		GetEncode(CMyString& encode);
	void		GetCharset(CMyString& charset);
	int			GetLevel(){return m_Level;}
	int 	    GetValue(char* key,CMyString& value,int index=0);
	

	bool		ParseHeader(CMyString& data);
	bool		ParseBody(CMyString& data);

	void		GetMainType(CMyString& mainType);
	void		GetSubType(CMyString&  subType);


	virtual void OnParseKeyValue(CMyString& key,CMyString& value);

	virtual void DumpState(CMyString& state);
	virtual void Reset();
	virtual int	 GetMailTextPlain(CMyString& text);
	virtual int	 GetMailTextHtml(CMyString& html);
	friend  class CExternData;

private:

	
	virtual void AddAttachment(CDataRecord* attachment);
	virtual void AddInnerResource(CDataRecord* innnerResource);

	void		ParseValue(CMyString& value);
	void		ParseContentType(CMyString& value);
	void        DecodeBodyData(CMyString& data);

	
public:

	std::vector<CDataRecord*>	m_Children;
	CDataRecord*				m_Parent;

	static char* sContentType[];
	static char* sTextSubType[];
	static char* sMultipartSubType[];
	static char* sImageSubType[];
	static char* sAudioSubType[];
	static char* sVideoSubType[];
	static char* sMessageSubType[];
	static char* sApplicationSubType[];
	static char* sTransferEncode[];
	static char* sCharset[];
	static ContentType sContentMap[];
	static char* TAG;
private:

	class HashCode
	{
	public:
		unsigned operator()(const CMyString &obj)
		{
			return ((CMyString&)obj).HashCode();
		}
	};

	CMyHashMap<CMyString,std::vector<CMyString>*,HashCode>	m_Field;

	int			m_Level;  //层次
	Data_Type	m_Type;

	int			m_MainType;
	int			m_SubType;

	CMyString	m_Boundary;
	CMyString	m_Charset;
	CMyString	m_Encode;

	CMyBuffer	m_Content;
	int			m_ContentLen;
};

#endif