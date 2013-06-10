// test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#define START_DEBUG


#include "stdio.h"
#include "../MyLib.h"
#include "../Mail.h"

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
			if(MessageBox(NULL,"Ñ¡ï¿½ï¿½È·ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È¡ï¿½ï¿½Ô´ï¿½ï¿½È¡ï¿½ï¿½Ð´ï¿½ï¿½ï¿½ï¿½Ô´","tst",1)==IDOK)
			{
				lock.WaitRead();
				MessageBox(NULL,"ï¿½ï¿½ï¿½Ú¶ï¿½È¡ï¿½ï¿½Ô´","tst",0);
				lock.Done();
			}
			else
			{
				lock.WaitWrite();
				MessageBox(NULL,"ï¿½ï¿½ï¿½ï¿½Ð´ï¿½ï¿½ï¿½ï¿½Ô´","tst",0);
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
#define DATE_REGEX "(Mon|Tues|Wed|Thur|Fri|Sat|Sun) *, +([1-9]|[1-3][0-9]) +(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) +[0-9]{4} +[0-2][0-9]:[0-5][0-9]:[0-5][0-9]( +(\\+|-)[0-9]{4})?"

int main(int argc, char* argv[])
{

	int ts='\r';
	DWORD d=::GetCurrentProcessId();
	CHttp::InitalHttp();
	
	CMyString str1="wed,    3   may 2009 07:57:02     +0800";
	CMyString str2;
	str1.Match(DATE_REGEX,&str2,true);


	str1="00004";
	int r=str1.ToInt();

	str1.Replace("004","");
	
	str1="ÒÑ´´½¨ Gmail µØÖ· qqjackcn@gmail.com";
/*
	CMyTime time;
	CMyTimeInterval interval;
	interval.SetSeconds(1);
	interval.SetMilliseconds(0); 
	for(int a=0;a<183;a++)
	{
		time+=interval;
		printf("%s\n",time.ToStringCn().GetBuffer());
		printf("%s\n",time.ToStringEn().GetBuffer());
	}

	CMyTime time2(2012,1,1,23,59,59,999);
	CMyTimeInterval interval2;
	interval2.SetMilliseconds(500);
	time2+=interval2;

	printf("%s\n",time2.ToStringCn().GetBuffer());
	CMyCalendar calendar(2063,3,16);
	
	interval.SetDays(600);
	interval.SetSingal(false);
	for(int j=0;j<30;j++)
	{
		calendar-=interval;
		printf("%s\n",calendar.GetDateInCN().GetBuffer());
		printf("%s\n",calendar.GetDateInEN().GetBuffer());
		printf("%s\n",calendar.GetDateInEnSimple().GetBuffer());
	}

	CMyCalendar calendar2(2063,3,5);
	interval=calendar-calendar2;
	printf("%d Ìì",interval.GetDays());


	CMyTime timeFirst(2012,1,3,20,34,40);
	CMyTime timeSecond(2012,1,3,20,34,48);

	CMyTimeInterval diff =timeFirst-timeSecond;

	printf("\n%d Ìì,%d Ð¡Ê±,%d ·ÖÖÓ ,%d ÃëÖÓ ,%d ºÁÃë\n",diff.GetDays()
			,diff.GetHours(),diff.GetMinutes(),diff.GetSeconds(),diff.GetMilliseconds());


*/
	
/*
	CMailInfo	mailInfo;
	CMyPOP3 pop3;
	pop3.SetLoginInfo("","");
	pop3.SetPOP3Server("pop.qq.com");
	pop3.Login();
	pop3.Stat();
	pop3.Retr(pop3.GetMailCount(),mailInfo);
*/
/*
	CMyString str="aaa\r\ndddddddddd\r\nadsfadsf\r\nfffff\r\n\r\nbbbb";

	int num=str.Split("\r\n");
	for(int i=0;i<num;i++)
	{
		CMyString d;
		str.GetSplitString(i,d);
		printf("%s\n",d.GetBuffer());
	}
*/

/*

	CMySMTP smtp;
	smtp.SetLoginInfo("","");
	smtp.SetSMTPServer("smtp.qq.com");
	smtp.Login();

	smtp.SetMailFrom(".com");
	smtp.SetMailTo(".com");
	
	smtp.SetMailData("just test!!!",strlen("just test!!!"));
	smtp.SetMailTitle("cmd");
	smtp.Mail();
	smtp.Quit();
*/

/*
	CHttp http;
	http.AddHttpHeader("Accept-Charset: utf-8\r\n");
	http.AddHttpHeader("Accept-Encoding: gzip, deflate\r\n");

	http.Get("bbs.pediy.com","");
*/

/*
	CMyJsonObj obj;
	CMyJsonArray objArray;

	char *p="{\"is\":[{\"UserID\":11, \"Name\":{\"FirstName\":\"Truly\",\"LastName\":\"Zhu\"}, \"Email\":\"zhuleipro¡òhotmail.com\"},{\"UserID\":12, \"Name\":{\"FirstName\":\"Jeffrey\",\"LastName\":\"Richter\"}, \"Email\":\"xxx¡òxxx.com\"},{\"UserID\":13, \"Name\":{\"FirstName\":\"Scott\",\"LastName\":\"Gu\"}, \"Email\":\"xxx2¡òxxx2.com\"}]}";
	char *pp=obj.SetJsonStr(p,strlen(p));

	LOG("tst",(char*)obj.ToString());

	char* pa="[{\"UserID\":{}, \"Name\":{\"FirstName\":\"Truly\",\"LastName\":\"Zhu\"}, \"Email\":\"zhuleiproï¿½ï¿½hotmail.com\"},{\"UserID\":12, \"Name\":{\"FirstName\":\"Jeffrey\",\"LastName\":\"Richter\"}, \"Email\":\"xxxï¿½ï¿½xxx.com\"},{\"UserID\":13, \"Name\":{\"FirstName\":\"Scott\",\"LastName\":\"Gu\"}, \"Email\":\"xxx2ï¿½ï¿½xxx2.com\"}]";
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
*/

/*
	CMyTaskThread taskThread;

	task t1,t2,t3;

	taskThread.AssignTask(t1,NULL);
	taskThread.AssignTask(t2,NULL);
	taskThread.AssignTask(t3,NULL);

	Sleep(0);
	taskThread.ExitThreadDirect();

	CMyThreadPool threadPool(true);
*/

//mail parse test
/**/	
	CMyFile file;
	CMyBuffer buffer;
	CMyString mailString;

	file.Open("D:\\Download\\tst3.eml",CMyFile::FILE_OPEN);
	buffer.Alloc(file.GetFileSize()+1);
	file.read(buffer.GetBuffer(),file.GetFileSize());
	mailString	=CMyString::StringFromMem(buffer.GetBuffer(),0,file.GetFileSize());

	CMailRecord mailRecord;
	
	mailRecord.Parse(mailString);
	
	mailRecord.DumpState(mailString);

	mailRecord.Reset();
	printf("\n\n");
	printf(mailString.GetBuffer());
	getchar();


/*

	char buf[4];
	unsigned short value=0x4E25;
	CCharset::UnicodeToUTF_8(buf,value);
	CCharset::UTF_8ToUnicode(value,buf);
*/
	return 0;
}
