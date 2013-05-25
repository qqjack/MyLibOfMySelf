#include "MySMTP.h"
#include "Windows.h"

CMySMTP::CMySMTP()
{
	m_ServerPort	=DEFAULT_PORT;
	m_Title	=DEFAULT_TITLE;
}

CMySMTP::~CMySMTP()
{
}

void CMySMTP::SetLoginInfo(char *user,char* password)
{
	int userLen=strlen(user);
	int passwordLen=strlen(password);
	int len=0;
	CMyBuffer buffer;

	buffer.Alloc(2*(userLen>passwordLen?userLen:passwordLen));

	len	=CEncrypt::EncodeBase64((const unsigned char *)user,strlen(user),buffer);
	m_UserName	=CMyString::StringFromMem(buffer.GetBuffer(),0,len);

	buffer.seek(CMyIOStream::SEEK_START,0);
	len	=CEncrypt::EncodeBase64((const unsigned char *)password,strlen(password),buffer);
	m_Password	=CMyString::StringFromMem(buffer.GetBuffer(),0,len);
}

void CMySMTP::SetSMTPServer(char *url)
{
	m_SMTPServer	=url;
}

void CMySMTP::SetMailTitle(char *title)
{
	m_Title	=title;
}

void CMySMTP::SetMailData(char *data,int dataLen)
{
	m_Data	=CMyString::StringFromMem(data,0,dataLen);
}

void CMySMTP::SetMailFrom(char *mailFrom)
{
	m_From	="<";
	m_From	+=mailFrom;
	m_From	+=">";
}

void CMySMTP::SetMailTo(char *mailTo)
{
	m_To	="<";
	m_To	+=mailTo;
	m_To    +=">";
}

bool CMySMTP::Login()
{
	do
	{
		if(!ConnectServer())break;
		if(!Helo())break;
		if(!AuthLogin())break;
		return true;
	}while(0);
	return false;
}

bool CMySMTP::Mail()
{
	do
	{
		if(!MailFrom())break;
		if(!RcptTo())break;
		if(!Data())break;
		return true;
	}while(0);
	return false;
}

bool CMySMTP::ConnectServer()
{
	m_Socket	=socket(AF_INET,SOCK_STREAM,0);
	hostent*	ip=gethostbyname(m_SMTPServer.GetBuffer());
	if(ip==NULL)return false;
	sockaddr_in	addr;
	addr.sin_family	=AF_INET;
	addr.sin_port=htons(m_ServerPort);
	addr.sin_addr.S_un.S_addr=*(unsigned*)ip->h_addr_list[0];
	if(connect(m_Socket,(sockaddr*)&addr,sizeof(sockaddr)))return false;
	if(RecvData()==-1)return false;
	return true;
}

bool CMySMTP::AuthLogin()
{
	CMyString cmd="auth login\r\n";
	do
	{
		if(!ExcuteCmd(cmd))break;
		cmd	=m_UserName+"\r\n";
		if(send(m_Socket,cmd.GetBuffer(),cmd.GetStrLen(),0)==-1)return false;
		if(RecvData()==-1)break;
		if(!CheckRecvState())break;

		cmd =m_Password+"\r\n";
		if(send(m_Socket,cmd.GetBuffer(),cmd.GetStrLen(),0)==-1)return false;
		if(RecvData()==-1)break;
		if(!CheckRecvState())break;
		return true;
	}while(0);
	return false;
}

bool CMySMTP::Helo()
{
	CMyString cmd="helo "+m_SMTPServer+"\r\n";
	return ExcuteCmd(cmd);
}

bool CMySMTP::MailFrom()
{
	CMyString cmd ="mail from:"+m_From+"\r\n";
	return ExcuteCmd(cmd);
}

bool CMySMTP::RcptTo()
{
	CMyString cmd ="rcpt to:"+m_To+"\r\n";
	return ExcuteCmd(cmd);
}

bool CMySMTP::Data()
{
	CMyString cmd ="data\r\n";
	do
	{
		if(!ExcuteCmd(cmd))break;
		cmd="Subject:"+m_Title+"\n";
		cmd+="From:"+m_From+"\n\n";
		m_Data=cmd+m_Data;
		m_Data+="\r\n.\r\n";
		if(send(m_Socket,m_Data,m_Data.GetStrLen(),0)==-1)break;
		m_Data="";
		if(RecvData()==-1)break;
		if(!CheckRecvState())break;
		return true;
	}while(0);
	m_Data="";
	return false;
}

void CMySMTP::Quit()
{
	CMyString cmd ="quit\r\n";
	send(m_Socket,cmd.GetBuffer(),cmd.GetStrLen(),0);
	closesocket(m_Socket);
}

int CMySMTP::RecvData()
{
	int r=0;
	while(1)
	{
		int len=recv(m_Socket,m_Buffer,100,0);
		if(len==-1)
		{
			r=len;
			break;
		}
		r+=len;
		if(strstr(m_Buffer,"\r\n"))break;
	}
	return r;
}

bool CMySMTP::CheckRecvState()
{
	int state =atoi(m_Buffer);
	if(state==354||state==250||state==235||state==334)return true;
	return false;
}

bool CMySMTP::ExcuteCmd(CMyString& cmd)
{
	do
	{
		if(send(m_Socket,cmd.GetBuffer(),cmd.GetStrLen(),0)==-1)break;
		if(RecvData()==-1)break;
		if(!CheckRecvState())break;
		return true;
	}while(0);
	return false;
}