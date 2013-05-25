#ifndef __MY_MSG__
#define __MY_MSG__

class CMyMsg  
{
public:
	CMyMsg();
	virtual ~CMyMsg();
	virtual int ReadFormatData(char*buf,int len);
	virtual int WriteFormatData(char*buf,int len);

private:
	virtual int FormatData(char*buf,int len)=0;
	virtual int DecodeData(char*buf,int len)=0;
	virtual int DecrtyData(char*buf,int len)=0;
	virtual int EncrtyData(char*buf,int len)=0;
};
#endif