#include "MyPOP3.h"

static char END_MARK[]={'\r','\n','.','\r','\n'};
#define ENDMARKLEN	(5)
#define IsEnd(_X) (memcmp((void*)_X,END_MARK,ENDMARKLEN)==0)

#define SUPPROT_MAX_MAIL_SIZE  (4*1024*1024)

CMyPOP3::CMyPOP3()
{
	m_ServerPort	=DEFAULT_POP3_PORT;
}

CMyPOP3::~CMyPOP3()
{

}

void CMyPOP3::SetLoginInfo(char* user,char* password)
{
	m_UserName	=user;
	m_Password	=password;
}

void CMyPOP3::SetPOP3Server(char* url)
{
	m_POP3Server	=url;
}

void CMyPOP3::SetPOP3Port(int port)
{
	m_ServerPort	=port;
}

bool CMyPOP3::ConnectServer()
{
	m_Socket	=socket(AF_INET,SOCK_STREAM,0);
	hostent*	ip=gethostbyname(m_POP3Server.GetBuffer());
	if(ip==NULL)return false;
	sockaddr_in	addr;
	addr.sin_family	=AF_INET;
	addr.sin_port=htons(m_ServerPort);
	addr.sin_addr.S_un.S_addr=*(unsigned*)ip->h_addr_list[0];
	if(connect(m_Socket,(sockaddr*)&addr,sizeof(sockaddr)))return false;
	if(RecvData()==-1)return false;
	return true;
}

bool CMyPOP3::User()
{
	CMyString cmd="user "+m_UserName+"\r\n";
	if(!ExcuteCmd(cmd))return false;
	if(!RecvData())return false;
	return CheckRecvState();
}

bool CMyPOP3::Pass()
{
	CMyString cmd="pass "+m_Password+"\r\n";
	if(!ExcuteCmd(cmd))return false;
	if(!RecvData())return false;
	return CheckRecvState();
}

void CMyPOP3::Quit()
{
	CMyString cmd="quit\r\n";
	ExcuteCmd(cmd);
	closesocket(m_Socket);
	return;
}

bool CMyPOP3::List()
{
	CMyString data="list\r\n";
	if(!ExcuteCmd(data))return false;
	if(!RecvCmdResult(data))return false;
	return FetchListResult(data);
}

bool CMyPOP3::Dele(int index)
{
	CMyString cmd=CMyString::Format("dele %d\r\n",index);
	if(!ExcuteCmd(cmd))return false;
	return CheckRecvState();
}

bool CMyPOP3::Retr(int index,CMailRecord &mail)
{
	CMyString data=CMyString::Format("retr %d\r\n",index);
	if(!ExcuteCmd(data))return false;
	if(!RecvCmdResult(data))return false;
	return FetchRetrResult(data,mail);
}

bool CMyPOP3::Reset()
{
	CMyString cmd="reset\r\n";
	if(!ExcuteCmd(cmd))return false;
	if(!RecvData())return false;
	return CheckRecvState();
}

bool CMyPOP3::Top(int index,int lineCount,CMailRecord &mail)
{
	CMyString data=CMyString::Format("top %d %d\r\n",index,lineCount);
	if(!ExcuteCmd(data))return false;
	if(!RecvCmdResult(data))return false;
	return FetchTopResult(data,mail);
}

int CMyPOP3::Login()
{
	if(!ConnectServer())return 0;
	if(!User())return -1;
	if(!Pass())return -1;
	return 1;
}

bool CMyPOP3::Stat()
{
	CMyString data="stat\r\n";
	if(!ExcuteCmd(data))return false;
	if(!RecvData())return false;
	if(!CheckRecvState())return false;
	return FetchMailData();
}

int CMyPOP3::RecvData()
{
	int r=0;
	while(1)
	{
		int len=recv(m_Socket,m_Buffer,BUFFER_LEN,0);
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

bool CMyPOP3::CheckRecvState()
{
	if((m_Buffer[1]=='O'||m_Buffer[1]=='o')&&
		(m_Buffer[2]=='k'||m_Buffer[2]=='K'))return true;
	return false;
}

bool CMyPOP3::ExcuteCmd(CMyString& cmd)
{
	if(send(m_Socket,cmd.GetBuffer(),cmd.GetStrLen(),0)==-1)return false;
	return true;
}

bool CMyPOP3::RecvCmdResult(CMyString& data)
{
	int recvLen=0;
	int bufLen=BUFFER_LEN;
	char *cmpBuf=NULL;
	int sumLen=0;
	data="";
	do
	{
		recvLen=recv(m_Socket,m_Buffer,bufLen,0);

		m_Buffer[recvLen]=0;
		data+=m_Buffer;
		
		sumLen+=recvLen;
		if(recvLen<0)break;
		if(recvLen>5)
		{
			cmpBuf=&m_Buffer[recvLen-ENDMARKLEN];
		}
		else
		{
			cmpBuf=data.GetBuffer()+data.GetStrLen()-ENDMARKLEN;
		}
		if(IsEnd(cmpBuf))
		{
			return true;
		}
		if(sumLen>SUPPROT_MAX_MAIL_SIZE)
		{
			sprintf(m_Buffer,"recive data is large than 4M,exit!");
			break;
		}
	}while(1);
	return false;
}

bool CMyPOP3::FetchListResult(CMyString& data)
{
	int len=data.GetStrLen();
	if(len<=5)return false;
	char *pStart=data.GetBuffer()+5;
	char *pEnd=data.GetBuffer()+len-5;
	char *pTemp=NULL;
	bool r=false;
	while(pStart<=pEnd)
	{
		r=false;
		if(*pStart<'0'||*pStart>'9')break;

		while(*pStart>='0'&&*pStart<='9'&&pStart<=pEnd)pStart++;
		if(pStart>=pEnd)break;
		while(*pStart==' '&&pStart<=pEnd)pStart++;
		if(pStart>=pEnd)break;

		if(*pStart<'0'||*pStart>'9')break;
		pTemp=pStart;
		while(*pStart>='0'&&*pStart<='9'&&pStart<=pEnd)pStart++;
		unsigned long size=atol(pTemp);
		m_MailSizeList.push_back(size);
		printf("%d %d\n",m_MailSizeList.size(),size);

		if(*pStart!='\r'&&*(pStart+1)!='\n')break;
		pStart+=2;
		r=true;
	}

	if(!r)sprintf(m_Buffer,"the list data is invalid!");
	return r;
}

bool CMyPOP3::FetchTopResult(CMyString& data,CMailRecord &mail)
{
	int len=data.GetStrLen();         
	if(len<=5)return false;
	data.Erase(0,5);
	return mail.ParseHeader(data);	
}

bool CMyPOP3::FetchRetrResult(CMyString& data,CMailRecord &mail)
{
	int len=data.GetStrLen();
	if(len<=5)return false;
	data.Erase(0,5);
	data.EraseFromRight(5);
	return mail.Parse(data);
}

bool CMyPOP3::FetchMailData()
{
	sscanf(&m_Buffer[4],"%d %d",&m_MailCount,&m_MailTotalSize);
	return true;
}

int	CMyPOP3::GetMailCount()
{
	return m_MailCount;
}

int	CMyPOP3::GetMailSize(int index)
{
	if(index>=m_MailSizeList.size())return -1;
	return m_MailSizeList[index];
}