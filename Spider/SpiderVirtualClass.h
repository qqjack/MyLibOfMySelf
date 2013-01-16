#ifndef __SPIDER_VIRTUAL_CLASS__
#define __SPIDER_VIRTUAL_CLASS__
#include "../MyLib.h"
class SpiderPageProcess  
{
public:
	SpiderPageProcess();
	virtual ~SpiderPageProcess();
	virtual	void PageProcess(const char *parentUrl,const char* url,const char* data,int dataLen);
};

class SpiderFileProcess  
{
public:
	SpiderFileProcess();
	virtual ~SpiderFileProcess();
	virtual	void FileProcess(const char *parentUrl,const char* url,CMyFile *file);
};

class SpiderErrorNotify  
{
public:
	SpiderErrorNotify();
	virtual ~SpiderErrorNotify();
	//返回值决定异常连接是否进行重试error 大于0属服务端反馈错误，小于0代表内部错误
	virtual	bool ErrorProcess(const char *parentUrl,const char* url,int error);
};

class SpiderUrlFilter  
{
public:
	SpiderUrlFilter();
	virtual ~SpiderUrlFilter();
	virtual bool FilterCheck(const char *parentUrl,const char *url)=0;
};

class SpiderUrlListSort
{
public:
	SpiderUrlListSort();
	virtual ~SpiderUrlListSort();
	virtual bool CompareUrl(const char *parentUrl,const char *url1,const char *url2);
};

class SpiderUrlModify  
{
public:
	SpiderUrlModify();
	virtual ~SpiderUrlModify();
	virtual bool ModifyUrl(const char *parent,CMyString &url)=0;
};

#endif