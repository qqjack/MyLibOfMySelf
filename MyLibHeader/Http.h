#ifndef __HTTP__
#define __HTTP__
#include "windows.h"
#include "MyString.h"
#include "MyDebug.h"
#include "MyTempFile.h"
#include "wininet.h"

#include "zlib.h"


class CHttp  
{
public:
	CHttp();
	virtual ~CHttp();

	int			Get(char *uri,char *page);
	int			Post(char *uri,char *page,char *postData,int dataLen);

	void		SetUserInfo(char *userName,char *password);
	void		AddHttpHeader(char *header);
	void		SetAcceptType(char *type);
	void		SetRefer(char *refer);
	//返回的缓冲不能进行修改，否则产生无法预期后果
	char*		GetReceiveData(){return m_Data.GetMapping();}
	int			GetReceiveDataLen(){return m_DataLen;}
	int			GetStatusCode(){return m_StatusCode;}    //在调用完Get或者Post后使用
	//返回字符串缓冲只能读取，不可写入
	const char*	GetDataCharset(){return m_Charset.GetBuffer();};


	static void InitalHttp();
	static void UInitalHttp();

public:
	bool		m_StopGetOrPost;     //用于在其它线程中停止Get和Post操作

private:
	HINTERNET HttpConnect(char* uri);
	HINTERNET HttpOpenRequest(HINTERNET connect,char *verb,char *htmlPath);
	bool	  HttpSendRequest(HINTERNET request,void *extralData,int extralDataLen);
	int	      HttpAddHeader(HINTERNET request);
	int		  HttpGetContentLen(HINTERNET request);
	int		  HttpReadData(HINTERNET);
	void	  AnalysisUri(char *uri,CMyString &lastUri,CMyString &lastHtmlPath);
	int		  HttpGetCharset(HINTERNET request,CMyString &charset);
	int	      HttpGetEncode(HINTERNET request,CMyString &encode);
	int		  HttpGetStatusCode(HINTERNET request,int &code);
	int       HttpQueryInfo(HINTERNET request,CMyString &info,int code);

	int       GetOrPost(char *uri,char *page,char *postData,int dataLen);
private:
	CMyString m_Cookie;       //cookie
	CMyString m_HttpHeader;	  //http头
	CMyString m_UserName;	  //连接时候的用户名
	CMyString m_Password;	  //连接时候的用户密码
	CMyString m_AcceptType;   //指定请求时候的接受数据类型，默认文本
	CMyString m_Refer;        //从哪个uri位置引用到请求网址
	HINTERNET m_Session;      //会话句柄

	int				m_DataLen;
	CMyTempFile		m_Data;       //保存数据的文件

	CMyString m_Charset;         //接受到的数据字符集
	CMyString m_Encode;          //接受到的数据压缩格式，为空串则表示没压缩
	int		  m_StatusCode;      //获取Get或者Post后的状态码
	static char*  TAG;
};

#endif
