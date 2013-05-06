#ifndef __MY_SMTP__
#define __MY_SMTP__
#include "MyString.h"
#include "Encrypt.h"

#define DEFAULT_PORT	(25)
#define DEFAULT_TITLE	"Hello email!^v^"
class CMySMTP  
{
public:
	CMySMTP();
	virtual ~CMySMTP();
	
	void	 SetLoginInfo(char *user,char* password);
	void	 SetSMTPServer(char *url);
	void	 SetMailData(char *data,int dataLen);
	void	 SetMailFrom(char *mailFrom);
	void	 SetMailTo(char *mailTo);
	void	 SetMailTitle(char *title);
	void	 setMailPort(int port){m_ServerPort	=port;}
	bool	 Mail();

	bool	 Login();
	void	 Quit();

private:
	bool	 ConnectServer();
	bool	 AuthLogin();
	bool	 Helo();
	bool	 MailFrom();
	bool	 RcptTo();
	bool     Data();
	
	
	int	     RecvData();
	bool	 CheckRecvState();
	bool	 ExcuteCmd(CMyString& cmd);
	char*	 GetErrorMessage(){return m_Buffer;}
private:
	CMyString	m_SMTPServer;
	unsigned	m_ServerPort;

	CMyString	m_UserName;
	CMyString   m_Password;
	CMyString	m_From;
	CMyString	m_To;
	CMyString	m_Data;
	CMyString	m_Title;

	int			m_Socket;

	char		m_Buffer[513];
};
#endif
