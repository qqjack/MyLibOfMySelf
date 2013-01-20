#include "MyMsg.h"

CMyMsg::CMyMsg()
{

}

CMyMsg::~CMyMsg()
{

}

int CMyMsg::ReadFormatData(char* buf,int len)
{
	this->FormatData(buf,len);
	this->EncrtyData(buf,len);
	return 1;
}

int CMyMsg::WriteFormatData(char* buf,int len)
{
	this->DecodeData(buf,len);
	this->DecrtyData(buf,len);
	return 1;
}