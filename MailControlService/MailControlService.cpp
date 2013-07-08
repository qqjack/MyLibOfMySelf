// MailControlService.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <MyTaskThread.h>
#include <MyPOP3.h>
#include <MyDebug.h>
#include <MyTime.h>
#include <MyAsyncHttp.h>
#include <MyConfig.h>

static char*	Tag="MailControlService";
static char*	MARK="#cmd#";

#define	LOG_FILE_NUM (0)
#define MAX_CMDSET_SIZE (4*1024)

class CCmdTask:public CMyTask
{
public:

	virtual int	Excute(void *threadArg,void* taskArg)
	{
	}
};

class CCustomTask:public CMyTask
{
	public:

	virtual int	Excute(void *threadArg,void* taskArg)
	{
		delete this;
	}
};

class AppConfig
{
public:
	bool LoadAppConfig()
	{
		bool r=true;

		CMyString path=CMyString::GetSlefPath();
		path+="/Config";

		CreateDirectory(path,NULL);

		path+="/config.ini";
		
		if(!m_Config.NewConfig(path))return false;
		
		if(m_Config.GetStringValue("AppConfig","user",m_UserName)==-1)
		{
			m_Config.PutStringValue("AppConfig","user","user name");
			r=false;
		}

		if(m_Config.GetStringValue("AppConfig","password",m_Password)==-1)
		{
			m_Config.PutStringValue("AppConfig","password","password");
			r=false;
		}

		if(m_Config.GetStringValue("AppConfig","service",m_POP3ServiceAddr)==-1)
		{
			m_Config.PutStringValue("AppConfig","service","pop3 service");
			r=false;
		}
		
		if(m_Config.GetIntValue("AppConfig","port",m_Port)==-1)
		{
			m_Port=110;
		}

		if(m_Config.GetIntValue("AppConfig","check_interval",m_CheckInterval)==-1)
		{
			m_CheckInterval=5000;
		}
		
		if(!r)
			m_Config.Save();
		return r;
	}

public:
	CMyString m_UserName;
	CMyString m_Password;
	CMyString m_POP3ServiceAddr;
	int		  m_Port;
	int		  m_CheckInterval;

private:
	CMyConfig m_Config;
};

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	AppConfig		config;
	CMyPOP3			pop3Mail;
	CMyTaskThread	processTask;
	CMailRecord		mail;
	CMyString		mailContent;
	CMyString		mailTitle;

	int			preMailCount=0;
	int			preMailTotalSize=0;

	CMyAsyncHttp::InitalHttp();

	CMyTime		startTime;
	CMyTime		tempTime;

	CMyDebug::SetFileLogName(0,Tag);
	
	if(!config.LoadAppConfig())
	{
		LOG(Tag,1,LOG_FILE_NUM,"read app config failed!");
		return 0;
	}
	
	pop3Mail.SetLoginInfo(config.m_UserName,config.m_Password);
	pop3Mail.SetPOP3Server(config.m_POP3ServiceAddr);
	
	processTask.Start(NULL);

	while(1)
	{
		do
		{
			int r=pop3Mail.Login();
			if(r==0)
			{
				LOG(Tag,"connect server failed!");
				break;
			}
			else if(r==-1)
			{
				LOG(Tag,"user name or password is invalid!");
				break;
			}

			if(-1==pop3Mail.Stat())
			{
				LOG(Tag,"stat command failed!");
				break;
			}

			bool first=true;
			for(int i=pop3Mail.GetMailCount();i>=1;i--)
			{
				mail.Reset();

				if(-1==pop3Mail.Top(i,0,mail))
				{
					LOG(Tag,"top %d command failed!",i);
					continue;
				}
				
				if(mail.GetMailTime()>startTime)
				{
					mail.GetMailSubject(mailTitle);
					if(mailTitle==MARK)
					{
						mail.Reset();
						pop3Mail.Retr(i,mail);
						mail.GetMailTextPlain(mailContent);
						if(mailContent.GetStrLen()>MAX_CMDSET_SIZE)
						{
						}
					}
					else if(mailTitle=="#shutdown")
					{
						system("shutdown -s");
					}
					else if(mailTitle=="#killself")
					{
						exit(0);
					}
					if(first)
					{
						tempTime=mail.GetMailTime();
						first=false;
					}
				}
				else
				{
					break;
				}
			}

			if(!first)
			{
				startTime=tempTime;
			}
			pop3Mail.Quit();
		}while(0);
		Sleep(config.m_CheckInterval);
	}
	return 0;
}



