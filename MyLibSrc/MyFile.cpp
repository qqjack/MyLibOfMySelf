#include "MyFile.h"

char*	CMyFile::TAG="CMyFile";
CMyFile::CMyFile()
{
	m_Locked=false;
	m_Inital=false;
	Reset();
}

CMyFile::~CMyFile()
{
	Reset();
}

void CMyFile::Reset()
{
	if(m_Locked)
	{
		ReleaseMapping();
	}
	else
	{
		m_FileBuf=NULL;
		m_MapHandle=INVALID_HANDLE_VALUE;
		m_FileHandle=INVALID_HANDLE_VALUE;
	}

	m_Error		=0;
	m_FileSize	=0;
	if(m_Inital)
	{
		fclose(m_File);
	}
	m_File		=NULL;
	m_Inital	=false;
}

int	CMyFile::Open(char *filePath,FILE_MODE mode)
{
	CMyString openMode;
	Reset();
	switch(mode)
	{
	case FILE_NEW:
		if(CMyFile::IsFileExist(filePath))
		{
#ifdef _DEBUG
			LOG(TAG,"the file exist!");
#endif
			return -1;
		}
	case FILE_OPEN:
		openMode="r+b";
		break;
	case FILE_NEW_OR_OPEN:
		if(CMyFile::IsFileExist(filePath))
			openMode="r+b";
		else
			openMode="w+b";
		break;
	case FILE_NEW_OR_CLEAR:
		openMode="w+b";
		break;
	}
	m_File=fopen(filePath,openMode.GetBuffer());
#ifdef _DEBUG
	if(!m_File)
		LOG(TAG,"fopen failed!");
#endif
	if(!m_File) return -1;
	m_Inital=true;
	m_FilePath=filePath;
	return 1;
}

int	CMyFile::Read(char *buf,int size)
{
	if(!m_Inital||IsLocked())return -1;
	return fread((void*)buf,1,size,m_File);
}

int	CMyFile::Write(char *buf,int size)
{
	if(!m_Inital||IsLocked())return -1;
	int r= fwrite((const void*)buf,1,size,m_File);
	m_FileSize+=r;
	return r;
}

int	CMyFile::Seek(SEEK_MODE mode,int offset)
{
	if(!m_Inital||IsLocked())return -1;
	return fseek(m_File,offset,mode);
}

int	CMyFile::Flush()
{
	if(!m_Inital||IsLocked())return -1;
	return fflush(m_File);
}

char* CMyFile::GetMapping()
{
	if(!m_Inital)return NULL;
	if(IsLocked())return (char*)m_FileBuf;

	fflush(m_File);

	do
	{
		m_FileHandle=::CreateFile(m_FilePath.GetBuffer(),GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,NULL,0);
		if(m_FileHandle==INVALID_HANDLE_VALUE||!m_FileHandle)break;
		int size=::GetFileSize(m_FileHandle,NULL);
		m_MapHandle=::CreateFileMapping(m_FileHandle,NULL,PAGE_READWRITE,0,size,NULL);
		if(m_MapHandle==INVALID_HANDLE_VALUE||!m_FileHandle)break;

		m_FileBuf=::MapViewOfFile(m_MapHandle,FILE_MAP_ALL_ACCESS,0,0,0);
		if(!m_FileBuf)break;

		LockFile();
		return (char*)m_FileBuf;
	}while(0);
	if(m_FileHandle&&m_FileHandle!=INVALID_HANDLE_VALUE)
		CloseHandle(m_FileHandle);
	if(m_MapHandle&&m_MapHandle!=INVALID_HANDLE_VALUE)
		CloseHandle(m_MapHandle);
#ifdef _DEBUG
	int error=GetLastError();
	if(m_FileHandle==INVALID_HANDLE_VALUE||!m_FileHandle)
		LOG(TAG,"open file failed!  error:%d",error);
	else if(m_MapHandle==INVALID_HANDLE_VALUE||!m_FileHandle)
		LOG(TAG,"create file mapping failed! error:%d",error);
	else 
		LOG(TAG,"map file failed! error:%d",error);
#endif
	return NULL;
}

void CMyFile::ReleaseMapping()
{
	if(!IsLocked())return;
	if(m_FileBuf)
		::UnmapViewOfFile(m_FileBuf);
	if(m_MapHandle!=INVALID_HANDLE_VALUE&&m_MapHandle)
		CloseHandle(m_MapHandle);
	if(m_FileHandle!=INVALID_HANDLE_VALUE&&m_FileHandle)
		CloseHandle(m_FileHandle);

	m_FileBuf=NULL;
	m_MapHandle=INVALID_HANDLE_VALUE;
	m_FileHandle=INVALID_HANDLE_VALUE;
	UnlockFile();
}

int CMyFile::GetUniqueFileName(char* buf,int bufLen)
{
	CMyString str	=GetUniqueFileName();
	if(str.GetStrLen()<bufLen)
	{
		strcpy(buf,str.GetBuffer());
		return 1;
	}
	return -1;
}

CMyString CMyFile::GetUniqueFileName()
{
	char tempBuffer[TEMP_BUFFER_SIZE];

	DWORD part1=::GetTickCount();
	srand(part1+rand());
	DWORD part2=rand();
	sprintf(tempBuffer,"%d+%d",part1,part2);

	return CMyString(tempBuffer);
}

bool CMyFile::IsFileExist(char *filePath)
{
	return -1!=::GetFileAttributes(filePath);
}

void CMyFile::DeleteFile()
{
	Reset();
	if(m_FilePath.GetStrLen())
		::DeleteFile(m_FilePath.GetBuffer());
}

unsigned long CMyFile::GetFileSize()
{
	int size;
	if(!m_Inital)return -1;
	if(!IsLocked())
	{
		m_FileHandle=::CreateFile(m_FilePath.GetBuffer(),GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,NULL,0);
		if(m_FileHandle==INVALID_HANDLE_VALUE||!m_FileHandle)return -1;
	}
	size	=::GetFileSize(m_FileHandle,NULL);
	if(!IsLocked())
		CloseHandle(m_FileHandle);
	return size;
}


void static CreateDirStatic(char *path,int &status)
{
	if(CMyFile::IsFileExist(path))
	{
		status	=1;
		return;
	}

	//如果到达盘符且盘符不存在则返回
	if(((path[0]>='a'&&path[0]<='z')||
		(path[0]>='A'&&path[0]<='Z'))&&
		path[1]==':'&&path[2]==0)
	{
		status=2;
		return ;
	}

	CMyString lPath=path;
	int len=lPath.GetStrLen();
	for(int i=len-1;i>=0;i--)
	{
		if(lPath[i]=='\\'&&i!=len-1)
		{
			lPath.SetAt(i,0);
			break;
		}
	}
	CreateDirStatic(lPath.GetBuffer(),status);
	
	if(status==1)
		::CreateDirectory(path,NULL);
}

void CMyFile::CreateDir(char *path)
{
	int status =0;
	CreateDirStatic(path,status);
}

void CMyFile::CopyToFile(char *filePath)
{
	this->Flush();
	::CopyFile(m_FilePath.GetBuffer(),filePath,false);
}