#include "Http.h"

#define MAX_BUF_SIZE (1000+1)
#define MAX_READ_SIZE (1000)

#pragma   comment(lib, "zlib.lib")
#pragma   comment(lib,"Wininet.lib") 

char*  CHttp::TAG="CHttp";
CHttp::CHttp()
{
	m_Session=::InternetOpen("HTTP",PRE_CONFIG_INTERNET_ACCESS,
		NULL,INTERNET_INVALID_PORT_NUMBER,0);
#ifdef _DEBUG
	if(!m_Session)
		LOG(TAG,"Open Internet failed!");
#endif
}

CHttp::~CHttp()
{
	if(m_Session)
		::InternetCloseHandle(m_Session);
}

int CHttp::Get(char *uri,char *page)
{
	return GetOrPost(uri,page,NULL,0);
}

int CHttp::Post(char *uri,char *page,char *postData,int dataLen)
{
	return GetOrPost(uri,page,postData,dataLen);
}

int CHttp::GetOrPost(char *uri,char *page,char *postData,int dataLen)
{
	if(!m_Session)return -1;
	HINTERNET lConnect;
	HINTERNET lHttp;
	CMyString lastUri,lastHtmlPath;
	this->AnalysisUri(uri,lastUri,lastHtmlPath);
	if(!lastHtmlPath.GetStrLen()&&page)
		lastHtmlPath=page;

	m_StatusCode=0;
	m_Charset   ="";
	m_Encode	="";
	do
	{
		lConnect	=HttpConnect(lastUri.GetBuffer());
		if(!lConnect)break;
		if(!dataLen)
			lHttp		=HttpOpenRequest(lConnect,"GET",
			(lastHtmlPath.GetStrLen()?lastHtmlPath.GetBuffer():NULL));
		else
			lHttp		=HttpOpenRequest(lConnect,"POST",
			(lastHtmlPath.GetStrLen()?lastHtmlPath.GetBuffer():NULL));

		if(!lHttp)break;

		HttpAddHeader(lHttp);

		if(!HttpSendRequest(lHttp,NULL,0))break;
	
		HttpGetCharset(lHttp,m_Charset);
		HttpGetEncode(lHttp,m_Encode);
		HttpGetStatusCode(lHttp,m_StatusCode);
		
		if(!HttpReadData(lHttp))break;
		
		m_AcceptType	="";
		m_UserName		="";
		m_Refer			="";
		m_Password		="";
		m_HttpHeader	="";
		return 1;
	}while(0);
	if(lHttp)InternetCloseHandle(lHttp);
	if(lConnect)InternetCloseHandle(lConnect);

	m_AcceptType	="";
	m_UserName		="";
	m_Refer			="";
	m_Password		="";
	m_HttpHeader	="";

	int d=GetLastError();
#ifdef _DEBUG
	if(!lHttp)
		LOG(TAG,"open request failed!");
	else if(!lConnect)
		LOG(TAG,"connect internet failed!");
#endif
	return -1;
}

void CHttp::InitalHttp()
{
	WSADATA wsaData;
	if(NO_ERROR!=::WSAStartup(MAKEWORD(2,2),&wsaData))
	{
		WSACleanup();
		LOG(TAG,"inital http failed!");
	}
}

void CHttp::UInitalHttp()
{
	WSACleanup();
}

HINTERNET CHttp::HttpConnect(char *uri)
{
	return ::InternetConnect(m_Session,uri,
			INTERNET_INVALID_PORT_NUMBER,m_UserName.GetBuffer(),m_Password.GetBuffer(),INTERNET_SERVICE_HTTP,
			0,0);
}

HINTERNET CHttp::HttpOpenRequest(HINTERNET connect,char *verb,char *htmlPath)
{
	char *accept[2]={NULL,NULL};
	accept[0]=m_AcceptType.GetBuffer();
	return ::HttpOpenRequest(connect,verb,htmlPath,
				HTTP_VERSION,
				(m_Refer.GetStrLen()?m_Refer.GetBuffer():NULL),
				(m_AcceptType.GetStrLen()?(const char**)accept:NULL),
				INTERNET_FLAG_DONT_CACHE,
				0);
}

bool CHttp::HttpSendRequest(HINTERNET request,void *extralData,int extralDataLen)
{
	return ::HttpSendRequest(request,NULL,0,extralData,extralDataLen);
}

int CHttp::HttpAddHeader(HINTERNET request)
{
	int r=1;
	if(m_HttpHeader.GetStrLen())
		r=::HttpAddRequestHeaders(request, m_HttpHeader.GetBuffer(), 
				m_HttpHeader.GetStrLen(),
				HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD);
	return r;
}

void CHttp::SetUserInfo(char *userName,char *password)
{
	m_UserName=userName;
	m_Password=password;
}

void CHttp::AddHttpHeader(char *header)
{
	m_HttpHeader+=header;
}

void CHttp::SetAcceptType(char *type)
{
	m_AcceptType=type;
}

void CHttp::SetRefer(char *refer)
{
	m_Refer=refer;
}

int	CHttp::HttpGetContentLen(HINTERNET request)
{
	CMyString buf;
	unsigned long rl=buf.GetBufferSize();
	unsigned long index;
	if(::HttpQueryInfo(request,HTTP_QUERY_CONTENT_LENGTH,
		buf.GetBuffer(),&rl,0))
	{
		return buf.ToInt();
	}
	int d=GetLastError();
	return -1;
}	

int	CHttp::HttpReadData(HINTERNET request)
{
	unsigned long rl;
	char tBuf[MAX_BUF_SIZE];

	m_DataLen	=0;

	while(!m_Data.IsUsable())m_Data.ReCreate();
	m_Data.ReleaseMapping();
	m_Data.seek(CMyFile::SEEK_START,0);

	if(!m_Encode.GetStrLen())
	{
		//数据没有压缩
		while(::InternetReadFile(request,tBuf,MAX_READ_SIZE,&rl))
		{
			unsigned long l;
			if(!rl)break;
			m_DataLen+=rl;
			m_Data.write(tBuf,rl);
		}
		m_Data.Flush();
	}
	else if(m_Encode.CompareI("gzip"))
	{
		//数据用gzip进行压缩，需解压缩
		char lBuf[MAX_READ_SIZE];
		int decodeLen=0;
		int err = 0;
		z_stream d_stream = {0}; /* decompression stream */
#ifdef _DEBUG
		LOG(TAG,"the data have encoded, need decode....");
#endif
		static char dummy_head[2] = 
		{
			0x8 + 0x7 * 0x10,
			(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
		};
		d_stream.zalloc = (alloc_func)0;
		d_stream.zfree = (free_func)0;
		d_stream.opaque = (voidpf)0;
		if(inflateInit2(&d_stream, 47) != Z_OK) return -1;

		while(::InternetReadFile(request,tBuf,MAX_READ_SIZE,&rl))
		{
			if(!rl)
				break;
			d_stream.next_in  = (unsigned char*)tBuf;
			d_stream.avail_in = 0;
			d_stream.next_out = (unsigned char*)lBuf;

			d_stream.total_in	=0;
			d_stream.total_out	=0;
			do
			{
				while (d_stream.total_out < MAX_READ_SIZE && d_stream.total_in < MAX_READ_SIZE) 
				{
					d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
					if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) 
						break;
					if(err != Z_OK )
					{
						if(err == Z_DATA_ERROR)
						{
							CMyDebug::Log(TAG,CMyDebug::MESSAGE_BOX,0,"decode have a exception,need debug!!");
							d_stream.next_in = (Bytef*) dummy_head;
							d_stream.avail_in = sizeof(dummy_head);
							if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
							{
								break;
							}
						}
						else 
							break;
					}
				}
				if(err!=Z_OK&err!=Z_STREAM_END)
					break;

				m_DataLen+=d_stream.total_out;
				m_Data.write(lBuf,d_stream.total_out);

				//输入缓冲区满,则需提供新的输入
				if(d_stream.total_out<MAX_READ_SIZE||err==Z_STREAM_END)
					break;
				//其它情况说明，输出缓冲不足，清空缓冲
				d_stream.total_out=0;
				d_stream.next_out = (unsigned char*)lBuf;
				memset(lBuf,0,MAX_READ_SIZE);
			}while(1);
			
			m_Data.Flush();
			if(err!=Z_OK&&err!=Z_STREAM_END)
			{
#ifdef _DEBUG
				LOG(TAG,"there have a err happened when decode the stream!");
#endif
				break;
			}
		}
	}
	else
	{
		//未知压缩格式
		CMyDebug::Log(TAG,CMyDebug::MESSAGE_BOX,0,"the unknow encode format:%s",m_Encode.GetBuffer());
	}
	return 1;
}

void CHttp::AnalysisUri(char *uri,CMyString &lastUri,CMyString &lastHtmlPath)
{
	char *pStart,*pEnd;
	CMyString strUri=uri;
	bool hasHtmlPath=true;
	pStart=uri;

	pStart		=strstr(uri,"http://");
	if(!pStart)
		pStart=uri;
	else
		pStart+=7;
	pEnd=pStart;
	while(*pEnd&&*pEnd!='/')pEnd++;

	if(!*pEnd)hasHtmlPath=false;
	pEnd--;

	lastUri=strUri.GetSubStr(pStart-uri,pEnd-uri);
	if(hasHtmlPath)
		lastHtmlPath=strUri.GetSubStr(pEnd+1-uri);
}

int CHttp::HttpGetCharset(HINTERNET request,CMyString &charset)
{
	CMyString str;
	if(HttpQueryInfo(request,str,HTTP_QUERY_CONTENT_TYPE))
	{
		char *p=str.GetBuffer();
		p=strstr(p,"charset=");
		if(p)charset=p+8;
		return 1;
	}
#ifdef _DEBUG
	int d=::GetLastError();
	if(d==ERROR_HTTP_HEADER_NOT_FOUND)
	{
		LOG(TAG,"HttpGetCharset,can't find the header!");
	}
#endif
	return 1;
	
}

int CHttp::HttpGetEncode(HINTERNET request,CMyString &encode)
{
	if(HttpQueryInfo(request,encode,HTTP_QUERY_CONTENT_ENCODING))
	{
	}
#ifdef _DEBUG
	int d=::GetLastError();
	if(d==ERROR_HTTP_HEADER_NOT_FOUND)
	{
		LOG(TAG,"HttpGetEncode,can't find the header!");
	}
#endif
	return 1;
}

int CHttp::HttpQueryInfo(HINTERNET request,CMyString &info,int code)
{
	unsigned long rl=0;
	::HttpQueryInfo(request,code,NULL,&rl,0);
	if(rl>0)
	{
		info.Resize(rl+1);
		return ::HttpQueryInfo(request,code,info.GetBuffer(),&rl,0);
	}
	return false;
}

int	CHttp::HttpGetStatusCode(HINTERNET request,int &code)
{
	CMyString str;
	code=0;

	if(HttpQueryInfo(request,str,HTTP_QUERY_STATUS_CODE))
	{
		code=str.ToInt();
	};
#ifdef _DEBUG
	int d=::GetLastError();
	if(d==ERROR_HTTP_HEADER_NOT_FOUND)
	{
		LOG(TAG,"HttpGetStatusCode,can't find the header!");
	}
#endif
	return 1;
}