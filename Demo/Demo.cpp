// test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#define START_DEBUG


#include "stdio.h"
#include "../MyLib.h"

//#pragma   comment(lib, "../Debug/MyLib")
char *tsss=
"asdfadsfasf"
"asdgasgadsgasg"
"asdgasdgadsgasdsdgsatwet"
;

CMyRWLock lock;

class tst :public CMyThread
{
public:
	virtual int Run(void* param)
	{
		void *p=param;
		while(1)
		{
			if(MessageBox(NULL,"选择确定，读取资源，取消写入资源","tst",1)==IDOK)
			{
				lock.WaitRead();
				MessageBox(NULL,"正在读取资源","tst",0);
				lock.Done();
			}
			else
			{
				lock.WaitWrite();
				MessageBox(NULL,"正在写入资源","tst",0);
				lock.Done();
			}
		}
		return 1;
	}
};

struct TST
{
	int a,b;
};

struct TST_E
{
	struct TST m1;
	int m2;
};

TST_E	s[20];

unsigned HashCode(CMyString str)
{
	return str.HashCode();
}

class task:public CMyTask
{
	static int count;
public:
	virtual int Excute(void *arg)
	{
		count++;
		Sleep(0);
		LOG("tst","test task!!!%d",count);
		return 1;
	}
};

int task::count=0;
int main(int argc, char* argv[])
{
//	CMyThread thread;
	tst mytst;
	CMyArrayList<TST> list;
	list.SetMem((void*)&s,sizeof(s));

	TST a;
	a.a=32;
	a.b=53;
	list.AddItem(a);
	list.AddItem(a);
	list.AddItem(a);
	list.AddItem(a);
	list.DelItem(0);
	list.AddItem(a);

	int ts='\r';
	DWORD d=::GetCurrentProcessId();
	
//	mytst.Start(NULL);
//	for(int i=0;i<5;i++)
//	{
//		thread.Run(&mytst,reinterpret_cast<THREADFUNC>(tst::run));
//	}
//	Sleep(1000000);
//	char filePath[200];
//	CMyDebug::SetFileLogName(1,"justtest");
//	CMyDebug::Log("tst",0,21,"asdfadsfa%d",10);
//	CMyDebug::Log(CMyDebug::FILE_LOG,0,"asdfadsfa%d",10);
//	GetSlefPath(filePath,200);
//	printf("finsh thread!!\n");
//	WaitEvent(thread.ExitThread(),-1);
//	printf("finsih main\n");
//	CMyTime::GetCurTimeStr(filePath,200,12);
//	printf(filePath);
//	CMyTime t;
//	t.InitalTime("2034-11-23 23:04:34");
//	t.GetTimeStr(filePath,200,12);
//	printf(filePath);
//	printf(tsss);
//	int rl;
//	char buf[4000];
//	CHttp::InitalHttp();
//	CHttp http;
//	http.Get("http://www.baidu.com",buf,4000,rl);
//	CHttp::UnitalHttp();
	CMyString str1="";
	CMyString str2="3543634634";
	str1+="hwrehwersa";
	str1+=str2;
	str1=str2;
	CMyString str3="adsga";
	str1=str1+str3;
	str1.ToUpcase();
	str1.ToLowerCase();
	char c=str1[3];
	bool l=(str1==str2);

	l=CMyFile::IsFileExist("adgasd");

	str1=str2;

	CHttp http;
	CHttp::InitalHttp();
	http.SetAcceptType("*/*");
	http.AddHttpHeader("Accept-Charset: utf-8\r\n");
	http.AddHttpHeader("Accept-Encoding: gzip, deflate\r\n");
	//http://www.it.com.cn/games/image/dm/2011/02/21/11/dmzb2011022106.jpg
	//http://mat1.qq.com/www/js/swfobjectopt.js
	http.Get("bbs.pediy.com","");
//	WaitForSingleObject(http.m_FinishNotify,-1);

//	CMyRegex regex;
//	regex.SetIgnoreCareLess(true);
//
//	regex.SetMainString(http.GetReceiveData(),http.GetReceiveDataLen());
//	char *p=regex.Match("href=\".*?\"");
//	while(p)
//	{
//		CMyString str=CMyString::StringFromMem(p,0,regex.GetMatchStrLen());
//		printf("%s \n",(char*)str);
//		p=regex.MatchNext();
//	}
//	CHttp::UInitalHttp();

	CMyJsonObj obj;
	CMyJsonArray objArray;
//	char *p="{ \"firstName\":  {  \"id\"  : 5345}, \"lastName\":false, \"email\": null } ";
//	char *p="{ \"people\": [     { \"firstName\": \"Brett\", \"lastName\":\"McLaughlin\", \"email\": \"aaaa\" },    { \"firstName\": \"Jason\", \"lastName\":\"Hunter\", \"email\": \"bbbb\"},   { \"firstName\": \"Elliotte\", \"lastName\":\"Harold\", \"email\": \"cccc\" }          ]   }";
	
//	char *p="{ \"programmers\":   [   { \"firstName\": \"Brett\", \"lastName\":\"McLaugh\", \"email\": \"aaaa\" },     { \"firstName\": \"Jason\", \"lastName\":\"Hunter\", \"email\": \"bbbb\" },   { \"firstName\": \"Elliotte\", \"lastName\":\"Harold\", \"email\": \"cccc\" }   ],  \"authors\":   [   { \"firstName\": \"Isaac\", \"lastName\": \"Asimov\", \"genre\": \"science fiction\" },    { \"firstName\": \"Tad\", \"lastName\": \"Williams\", \"genre\": \"fantasy\" },   { \"firstName\": \"Frank\", \"lastName\": \"Peretti\", \"genre\": \"christian fiction\" }     ],   \"musicians\":   [   { \"firstName\": \"Eric\", \"lastName\": \"Clapton\", \"instrument\": \"guitar\" },    { \"firstName\": \"Sergei\", \"lastName\": \"Rachmaninoff\", \"instrument\": \"piano\" }     ] } ";

//	char *p="{ \"identifier\" : \"id\", \"item\" : [ { \"MemSize\" : \"40\", \"RsNum\" : \"20\", \"TaNum\" : \"30\", \"dataClass\" : [ { \"content\" : \"TERRE\", \"title\" : \"遥感文件名\", \"url\" : \"http://www.baidu.com\", \"urlflag\" : \"true\" }, { \"content\" : \"TERRAFEDCDJF\", \"title\" : \"遥感影像名\", \"url\" : \"\", \"urlflag\" : \"false\" } ], \"delPoint\" : \"北京西城区白广路2条2号\", \"id\" : \"1\", \"rpIndName\" : \"水利部水利信息中心\", \"rpOrgName\" : \"陈德清\", \"voiceNum\" : \"63202038\" } ], \"label\" : \"id\", \"queryType\" : \"4\", \"time\" : \"56\", \"totalNum\" : \"5324\", \"turnPageName\" : [ \"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", \"9\", \"10\", \"[下一页]\" ], \"turnPageNum\" : [ \"-1\", \"10\", \"20\", \"30\", \"40\", \"50\", \"60\", \"70\", \"80\", \"90\", \"10\" ] }";
	char *p="{\"is\":[{\"UserID\":11, \"Name\":{\"FirstName\":\"Truly\",\"LastName\":\"Zhu\"}, \"Email\":\"zhuleipro◎hotmail.com\"},{\"UserID\":12, \"Name\":{\"FirstName\":\"Jeffrey\",\"LastName\":\"Richter\"}, \"Email\":\"xxx◎xxx.com\"},{\"UserID\":13, \"Name\":{\"FirstName\":\"Scott\",\"LastName\":\"Gu\"}, \"Email\":\"xxx2◎xxx2.com\"}]}";

	char *pp=obj.SetJsonStr(p,strlen(p));
	LOG("tst",(char*)obj.ToString());

	char* pa="[{\"UserID\":{}, \"Name\":{\"FirstName\":\"Truly\",\"LastName\":\"Zhu\"}, \"Email\":\"zhuleipro◎hotmail.com\"},{\"UserID\":12, \"Name\":{\"FirstName\":\"Jeffrey\",\"LastName\":\"Richter\"}, \"Email\":\"xxx◎xxx.com\"},{\"UserID\":13, \"Name\":{\"FirstName\":\"Scott\",\"LastName\":\"Gu\"}, \"Email\":\"xxx2◎xxx2.com\"}]";
	pa=objArray.SetJsonArrayStr(pa,strlen(pa));
	LOG("tst",(char*)objArray.ToString());

	objArray.GetJsonObj(0)->PutNumberKeyValue("addnumber",325.23);
	LOG("tst",(char*)objArray.ToString());
	objArray.DeleteItem(0);
	LOG("tst",(char*)objArray.ToString());
	objArray.DeleteItem(0);
	LOG("tst",(char*)objArray.ToString());
	objArray.AddStringItem("agafdsafsd");
	LOG("tst",(char*)objArray.ToString());
	objArray.AddJsonArrayItem(&objArray);
	LOG("tst",(char*)objArray.ToString());
	objArray.AddJsonObjItem(&obj);
	LOG("tst",(char*)objArray.ToString());
	objArray.DeleteItem(1);
	LOG("tst",(char*)objArray.ToString());

	obj.PutJsonArrayKeyValue("tst",&objArray);
	LOG("tst",(char*)obj.ToString());
	obj.DeleteKeyValue("is");
	LOG("tst",(char*)obj.ToString());
	obj.PutBoolKeyValue("string",124.124);
	LOG("tst",(char*)obj.ToString());
	obj.PutBoolKeyValue("string",0);
	LOG("tst",(char*)obj.ToString());
	obj.DeleteKeyValue("tst");
	LOG("tst",(char*)obj.ToString());

	CMyTaskThread taskThread;



	task t1,t2,t3;

	taskThread.AssignTask(t1,NULL);
	taskThread.AssignTask(t2,NULL);
	taskThread.AssignTask(t3,NULL);

	Sleep(0);
	taskThread.ExitThreadDirect();

	CMyThreadPool threadPool(true);
	getchar();
	return 0;
}
