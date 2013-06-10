#include "Charset.h"
#include "windows.h"
#include "MyDebug.h"

char* CCharset::TAG="CCharset";

//返回解析pText中的字节数
int CCharset::UTF_8ToUnicode(unsigned short& outC,char *pText)
{
	char* pOut=(char*)&outC;
	static char mark[3]={0x80,0xC0,0xE0};

	if(((*pText)&mark[0])==0)
	{
		outC=*pText;
		return 1;
	}
	else
	{
		if(((*pText)&mark[2])==mark[1])
		{
			pOut[0]=(pText[1]&(~mark[1]))+((pText[0]&0x03)<<6);
			pOut[1]=(pText[0]&0x1c)>>2;
			return 2;
		}
		else if(((*pText)&mark[2])==mark[2])
		{
			pOut[0]=(pText[2]&(~mark[1]))+((pText[1]&0x03)<<6);
			pOut[1]=((pText[1]&0x3c)>>2)+((pText[0]&0x0F)<<4);
			return 3;
		}
		else
		{
			CMyDebug::Log(TAG,2,0,"the UTF-8 format is invalid!");
		}
	}
	return -1;
}

//返回解析完写入pOut中的字节数
int CCharset::UnicodeToUTF_8(char* pOut,unsigned short c)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	if(c<=0x7f)
	{
		*pOut=c&0x00ff;
		return 1;
	}
	else if(c<=0x07ff)
	{
		pOut[1]=0x80+(c&0x003f);
		pOut[0]=((c&0xff00)>>6)+((c&0xc0)>>6)+0xc0;
		return 2;
	}
	else
	{
		pOut[2]=(c&0x003f)+0x80;
		pOut[1]=((c&0xc0)>>6)+(((((c&0xff00)>>8)&(0x0F))<<2)|0x80);
		pOut[0]=0xE0+((c&0xff00)>>12);
		return 3;
	}
	return -1;
}

int CCharset::UnicodeToGB2312(char* pOut,unsigned short uData)
{
	return	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);
}    

int CCharset::Gb2312ToUnicode(char* pOut,char *gbBuffer)
{
	return MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,(wchar_t*)pOut,1);
}

int CCharset::GB2312ToUTF_8(char* pOut,char *pText, int pTextLen)
{
	char buf[4];
	
	memset(buf,0,4);

	int i = 0;
	int j = 0;      
	while(i < pTextLen)
	{
		//如果是英文直接复制就能
		if( *(pText + i) >= 0)
		{
			pOut[j++] = pText[i++];
		}
		else
		{
			wchar_t pbuffer;

			Gb2312ToUnicode((char*)&pbuffer,pText+i);
			
			int count=UnicodeToUTF_8(buf,pbuffer);
			
			for(int a=0;a<count;a++)
			{
				pOut[j++]=buf[a];
			}
			
			j += 3;
			i += 2;
		}
	}
	*((unsigned short*)&pOut[j])=0;
	return j;
}

int CCharset::UTF_8ToGB2312(char* pOut, char *pText, int pLen)
{
	char Ctemp[4];
	memset(Ctemp,0,4);
	
	int i =0;
	int j = 0;
	
	while(i < pLen)
	{
		if(*(pText+i) >=0)
		{
			pOut[j++] = pText[i++];                       
		}
		else                 
		{
			WCHAR Wtemp;
			int count=UTF_8ToUnicode(Wtemp,pText + i);

			i+=count;

			UnicodeToGB2312(&pOut[j],Wtemp);
			j += 2;   
		}
	}
	pOut[j]=0;
	return j; 
} 

int CCharset::UnicodeToGB2312(char* pOut,char* pText,int pTextLen)
{
	int j=0;
	int i=0;
	while(i<pTextLen)
	{
		j+=UnicodeToGB2312(&pOut[j],*((unsigned short*)pText[i]));
		i+=2;
	}
	return j;
}

int CCharset::Gb2312ToUnicode(char* pOut,char* pText,int pTextLen)
{
	int j=0;
	int i=0;
	while(i<pTextLen)
	{
		j+=Gb2312ToUnicode(&pOut[j],&pText[i]);
		i+=2;
	}
	return j;
}