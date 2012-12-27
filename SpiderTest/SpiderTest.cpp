#include "../MyLib.h"

#define HTTP_COUNT	(20)

unsigned GetHashCode(CMyString obj)
{
	return obj.HashCode();
}

CMyHashMap<CMyString,int> maps(GetHashCode);

int main(int argc, char* argv[])
{
	CMyAsyncHttp  asyncHttp;

	CMyThreadPool	threadPool(true);

	CMyAsyncHttp::InitalHttp();
	asyncHttp.SetTargetThreadPool(&threadPool);
	asyncHttp.SetAcceptType("*/*");
	asyncHttp.AddHttpHeader("Accept-Charset: utf-8\r\n");
	asyncHttp.AddHttpHeader("Accept-Encoding: gzip, deflate\r\n");
	asyncHttp.Get("http://www.youku.com/","");

	WaitForSingleObject(asyncHttp.m_FinishNotify,-1);

//	CMyFile		file;
//	if(file.Open("5564020+16448.html",CMyFile::FILE_OPEN)==1)//asyncHttp.m_HttpState==CMyAsyncHttp::HTTP_FINISH)
//	{
//		CMyRegex regex;
//		char *p;
//		regex.SetIgnoreCareLess(true);
//		regex.SetMainString(file.GetMapping(),file.GetFileSize());//asyncHttp.GetReceiveData(),asyncHttp.GetReceiveDataLen());
//		p=regex.Match("http://.*?[\",;]");
//
//		CMyAsyncHttp http[HTTP_COUNT];
//		HANDLE		 notify[HTTP_COUNT];
//		CMyString	 httpPath;
//		for(int i=0;i<HTTP_COUNT;i++)
//		{
//			http[i].SetTargetThreadPool(&threadPool);
//			notify[i]=http[i].m_FinishNotify;
//		}
//		int index=0;
//
//		while(p)
//		{
//			httpPath=CMyString::StringFromMem(p,0,regex.GetMatchStrLen());
//			httpPath.EraseFromRight(1);
//			printf("%s\n",httpPath.GetBuffer());
//			if(map.GetItem(httpPath,&index)!=-1)
//			{
//				printf("----------------skip the same url:%s!\n",httpPath.GetBuffer());
//				p=regex.MatchNext();
//				continue;
//			}
//			map.AddItem(httpPath,index);
//			for(int i=0;i<HTTP_COUNT;i++)
//			{
//				if(http[i].IsIdle())
//				{
//					http[i].AddHttpHeader("Accept-Charset: utf-8\r\n");
//					http[i].AddHttpHeader("Accept-Encoding: gzip, deflate\r\n");
//					http[i].Get(httpPath.GetBuffer(),NULL);
//					break;
//				}
//			}
//			if(i>=HTTP_COUNT)
//			{
//				printf("Waiting for http finish!\n");
//				WaitForMultipleObjects(HTTP_COUNT,notify,false,-1);
//				printf("continue!\n");
//			}
//			p=regex.MatchNext();
//		}
//	}
//	getchar();
	return 0;
}
