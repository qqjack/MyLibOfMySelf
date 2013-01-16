#include "MyTempFile.h"

CMyTempFile::CMyTempFile()
{
	CMyString dir=CMyString::GetSlefPath()+"temp";
	::CreateDirectory(dir.GetBuffer(),NULL);
	dir+="\\";
	CMyFile::Open(dir+CMyFile::GetUniqueFileName(),CMyFile::FILE_NEW_OR_CLEAR);
}

CMyTempFile::~CMyTempFile()
{
	DeleteFile();
}

void CMyTempFile::ReCreate()
{
	CMyFile::Open(CMyFile::GetUniqueFileName(),CMyFile::FILE_NEW_OR_CLEAR);
}


