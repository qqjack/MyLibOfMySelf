#ifndef __MY_CHARSET__
#define __MY_CHARSET__

class CCharset
{
private:
	static int UTF_8ToUnicode(unsigned short& outC,char *pText); // 把UTF-8转换成Unicode
	static int UnicodeToUTF_8(char* pOut,unsigned short c); //Unicode 转换成UTF-8	

	static int UnicodeToGB2312(char* pOut,unsigned short uData); // 把Unicode 转换成 GB2312 
	static int Gb2312ToUnicode(char* pOut,char *gbBuffer);// GB2312 转换成　Unicode

	static char* TAG;
public:
	
	//返回写入输出缓冲的字节数

	static int UnicodeToGB2312(char* pOut,char* pText,int pTextLen); 
	static int Gb2312ToUnicode(char* pOut,char* pText,int pTextLen);
	static int GB2312ToUTF_8(char* pOut,char *pText, int pLen);//GB2312 转为 UTF-8
	static int UTF_8ToGB2312(char* pOut, char *pText, int pLen);//UTF-8 转为 GB2312
};

#endif