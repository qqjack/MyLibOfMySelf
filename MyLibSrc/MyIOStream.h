#ifndef __MY_IOSTREAM__
#define __MY_IOSTREAM__

class CMyIOStream  
{
public:
	enum SEEK_MODE
	{
		SEEK_START,
		SEEK_CURRENT,
		SEEK_ENDED
	};
public:
	CMyIOStream();
	virtual ~CMyIOStream();

	virtual int read(char* buf,int len)=0;
	virtual int write(char* buf,int len)=0;
	virtual int seek(SEEK_MODE seekType,int offset)=0;
};

#endif