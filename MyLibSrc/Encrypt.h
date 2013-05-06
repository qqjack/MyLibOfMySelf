#ifndef __ENCRYPT__
#define __ENCRYPT__
#include "MyString.h"
#include "MyBuffer.h"
class CEncrypt 
{
public:
	CEncrypt();
	virtual ~CEncrypt();
	
	static int EncodeBase64(const unsigned char* data,int dataLen,CMyBuffer& outBuffer);
    static int DecodeBase64(const char* data,int dataLen,CMyBuffer& outBuffer);
};
#endif