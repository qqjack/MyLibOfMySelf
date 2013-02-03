#ifndef __SPIDER_VIRTUAL_CLASS__
#define __SPIDER_VIRTUAL_CLASS__
#include "../MyLib.h"
class ISpiderPageProcess  
{
public:
	ISpiderPageProcess(){}
	virtual ~ISpiderPageProcess(){}
	//对于提取的url的页面数据处理
	virtual	void PageProcess(const char *parentUrl,const char* url,const char* data,int dataLen)=0;
};

class ISpiderFileProcess  
{
public:
	ISpiderFileProcess(){}
	virtual ~ISpiderFileProcess(){}
	//对于提取的url文件进行文件处理
	virtual	void FileProcess(const char *parentUrl,const char* url,const char* fileData,int dataLen)=0;
};

class ISpiderErrorNotify  
{
public:
	ISpiderErrorNotify(){}
	virtual ~ISpiderErrorNotify(){}
	//返回值决定异常连接是否进行重试error 大于0属服务端反馈错误，小于0代表内部错误
	virtual	bool ErrorProcess(const char *parentUrl,const char* url,int error,char *data,int dataLen)=0;
};

class ISpiderUrlFilter  
{
public:
	ISpiderUrlFilter(){}
	virtual ~ISpiderUrlFilter(){}
	virtual bool FilterCheck(const char *parentUrl,const char *url)=0;
};

class ISpiderUrlModify  
{
public:
	ISpiderUrlModify(){}
	virtual ~ISpiderUrlModify(){}
	virtual bool ModifyUrl(const char *parent,CMyString &url)=0;
};

class ISpiderFinish
{
public:
	ISpiderFinish(){}
	virtual ~ISpiderFinish(){}
	virtual void OnFinish()=0;
};
#endif