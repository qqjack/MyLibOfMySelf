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

	int ts='\r';
	DWORD d=::GetCurrentProcessId();
	CHttp::InitalHttp();

	CMySMTP smtp;
	CMailInfo	mailInfo;
	CMyPOP3 pop3;
	pop3.SetLoginInfo("415472580","zj8847523");
	pop3.SetPOP3Server("pop.qq.com");
	pop3.Login();
	pop3.List();
	pop3.Retr(2,mailInfo);

	CMyString str="aaa\r\ndddddddddd\r\nadsfadsf\r\nfffff\r\n\r\nbbbb";
	int num=str.Split("\r\n");
	for(int i=0;i<num;i++)
	{
		CMyString d;
		str.GetSplitString(i,d);
		printf("%s\n",d.GetBuffer());
	}
//	smtp.SetLoginInfo("415472580","zj8847523");
//	smtp.SetSMTPServer("smtp.qq.com");
//	smtp.Login();

//	smtp.SetMailFrom("415472580@qq.com");
//	smtp.SetMailTo("415472580@qq.com");
	
//	smtp.SetMailData("just test!!!",strlen("just test!!!"));
//	smtp.SetMailTitle("cmd");
//	for(int i=0;i<101;i++)
//		smtp.Mail();

//	smtp.Quit();
//	smtp.Mail();
	CHttp http;
	
	http.SetAcceptType("*/*");
	http.AddHttpHeader("Accept-Charset: utf-8\r\n");
	http.AddHttpHeader("Accept-Encoding: gzip, deflate\r\n");

	http.Get("bbs.pediy.com","");


	CMyJsonObj obj;
	CMyJsonArray objArray;

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
