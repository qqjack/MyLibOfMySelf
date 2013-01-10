#ifndef __SPIDER_VIRTUAL_CLASS__
#define __SPIDER_VIRTUAL_CLASS__
class SpiderPageProcess  
{
public:
	SpiderPageProcess();
	virtual ~SpiderPageProcess();

};

class SpiderFileProcess  
{
public:
	SpiderFileProcess();
	virtual ~SpiderFileProcess();
};

class SpiderErrorNotify  
{
public:
	SpiderErrorNotify();
	virtual ~SpiderErrorNotify();

};

class SpiderUrlFilter  
{
public:
	SpiderUrlFilter();
	virtual ~SpiderUrlFilter();

};

class SpiderUrlListSort  
{
public:
	SpiderUrlListSort();
	virtual ~SpiderUrlListSort();

};

class SpiderUrlModify  
{
public:
	SpiderUrlModify();
	virtual ~SpiderUrlModify();

};

#endif