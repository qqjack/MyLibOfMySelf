#include "Encrypt.h"

CEncrypt::CEncrypt()
{

}

CEncrypt::~CEncrypt()
{

}


int CEncrypt::EncodeBase64(const unsigned char* Data,int DataByte,CMyBuffer& outBuffer)
{
    //返回值
	int encodeLen=0;
    CMyString strEncode;
    //编码表
    const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char Tmp[4]={0};
    int LineLength=0;
    for(int i=0;i<(int)(DataByte / 3);i++)
    {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        Tmp[3] = *Data++;
		outBuffer.SetAt(encodeLen++,EncodeTable[Tmp[1] >> 2]);
        outBuffer.SetAt(encodeLen++,EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F]);
        outBuffer.SetAt(encodeLen++,EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F]);
        outBuffer.SetAt(encodeLen++,EncodeTable[Tmp[3] & 0x3F]);
        if(LineLength+=4,LineLength==76)
		{
			outBuffer.SetAt(encodeLen++,'\r');
			outBuffer.SetAt(encodeLen++,'\n');
			LineLength=0;
		}
    }
    //对剩余数据进行编码
    int Mod=DataByte % 3;
    if(Mod==1)
    {
        Tmp[1] = *Data++;
        outBuffer.SetAt(encodeLen++,EncodeTable[(Tmp[1] & 0xFC) >> 2]);
        outBuffer.SetAt(encodeLen++,EncodeTable[((Tmp[1] & 0x03) << 4)]);
		outBuffer.SetAt(encodeLen++,'=');
		outBuffer.SetAt(encodeLen++,'=');
    }
    else if(Mod==2)
    {
        Tmp[1] = *Data++;
        Tmp[2] = *Data++;
        outBuffer.SetAt(encodeLen++,EncodeTable[(Tmp[1] & 0xFC) >> 2]);
        outBuffer.SetAt(encodeLen++,EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)]);
        outBuffer.SetAt(encodeLen++,EncodeTable[((Tmp[2] & 0x0F) << 2)]);
		outBuffer.SetAt(encodeLen++,'=');
    }
    return encodeLen; 
}

int CEncrypt::DecodeBase64(const unsigned char* Data,int DataByte,CMyBuffer& outBuffer)
{
	//返回值
	int decodeLen=0;
	//解码表
    const char DecodeTable[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };

    int nValue;
    int i= 0;
	char c;
    while (i < DataByte)
    {
        if (*Data != '\r' && *Data!='\n')
        {
            nValue = DecodeTable[*Data++] << 18;
            nValue += DecodeTable[*Data++] << 12;
            outBuffer.SetAt(decodeLen++,(nValue & 0x00FF0000) >> 16);
            if (*Data != '=')
            {
                nValue += DecodeTable[*Data++] << 6;   
				outBuffer.SetAt(decodeLen++,(nValue & 0x0000FF00) >> 8);
                if (*Data != '=')
                {
                    nValue += DecodeTable[*Data++];
					outBuffer.SetAt(decodeLen++,nValue & 0x000000FF);
                }
            }
            i += 4;
        }
        else// 回车换行,跳过
        {
            Data++;
            i++;
        }
     }
    return decodeLen;
}


int CEncrypt::EncodeQuotedPrintable(const unsigned char* data,int dataLen,CMyBuffer& outBuffer)
{
#define PART1(_X) ((((_X)&0x0f)>=10)?(((_X)&0x0f)-10+'A'):(((_X)&0x0f)+'0'))
#define PART2(_X) (((((_X)>>4)&0x0f)>=10)?((10-((_X)>>4)&0x0f)+'A'):((((_X)>>4)&0x0f)+'0'))
#define IS_3_BYTE(_X) (!((_X)<33||(_X)>126))

	int encodeLen=0;
	for(int i=0;i<dataLen;i++)
	{
		if(IS_3_BYTE(data[i])||(data[i]=='='))
		{
			outBuffer.SetAt(encodeLen++,'=');
			outBuffer.SetAt(encodeLen++,PART2(data[i]));
			outBuffer.SetAt(encodeLen++,PART1(data[i]));
		}
		else
		{
			outBuffer.SetAt(encodeLen++,data[i]);
		}
	}
	return encodeLen;
}

int CEncrypt::DecodeQuotedPrintable(const unsigned char* data,int dataLen,CMyBuffer& outBuffer)
{
#define X_TO_PART(_X) (((_X)>'9')?(10+(_X)-'A'):((_X)-'0'))
	int decodeLen=0;
	int count=0;
	bool flag=false;
	int value=0;
	bool is3Byte=false;

	while(count<dataLen)
	{
		if(data[count]=='=')
		{
			if(count+2<dataLen&&(data[count+1]=='\r'&&data[count+2]=='\n'))
			{
				//跳过软换行符
				count+=3;
				continue;
			}
			else if(count+1<dataLen&&data[count+1]=='=')
			{
				outBuffer.SetAt(decodeLen++,'=');
				count+=2;
				continue;
			}

			count++;
			is3Byte=true;
			flag=false;
			continue;
		}
		if(is3Byte)
		{
			if(!flag)
			{
				value=X_TO_PART(data[count])<<4;
				flag=!flag;
			}
			else
			{
				value|=X_TO_PART(data[count]);
				outBuffer.SetAt(decodeLen++,value);
				value=0;
				is3Byte=false;
			}
		}
		else
		{
			outBuffer.SetAt(decodeLen++,data[count]);
		}
		count++;
		
	}
	return decodeLen;
}