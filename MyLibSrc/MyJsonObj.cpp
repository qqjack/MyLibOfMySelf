#include "MyJsonObj.h"
#include "MyDebug.h"

//begin  CMyJsonArray
CMyJsonArray::CMyJsonArray()
{
	m_Value.m_Obj=NULL;
	m_Value.m_Next   =NULL;
	m_ValueLen		 =0;
}

CMyJsonArray::~CMyJsonArray()
{
	FreeAll();
}

CMyString CMyJsonArray::ToString()
{
	CMyString  rStr;
	rStr		+="[";
	if(m_ValueLen>0)
	{
		Json_Array* head =&m_Value;
		while(head)
		{
			rStr	+=ArrayItemToString(head->m_Obj);
			head	=head->m_Next;
			if(head)
			{
				rStr	+=",";
			}
		}
	}
	rStr		+="]";
	return rStr;
}

CMyString CMyJsonArray::ArrayItemToString(Array_Item *item)
{
	CMyString rStr;
	switch(item->m_Type)
	{
	case TYPE_DIGTAL:
	case TYPE_BOOL:
		rStr	+=*(item->m_Value.m_String);
		break;
	case TYPE_STRING:
		rStr	+="\"";
		rStr	+=*(item->m_Value.m_String);
		rStr	+="\"";
		break;
	case TYPE_JSONARRAY:
		rStr	+=item->m_Value.m_JsonArray->ToString();
		break;
	case TYPE_JSONOBJ:
		rStr	+=item->m_Value.m_JsonObj->ToString();
		break;
	case TYPE_NULL:
	default:
		rStr	+="null";
	}
	return rStr;
}
void CMyJsonArray::FreeArrayItem(Array_Item *item)
{
	switch(item->m_Type)
	{
	case TYPE_DIGTAL:
	case TYPE_STRING:
	case TYPE_BOOL:
		if(item->m_Value.m_String)
			delete item->m_Value.m_String;
		break;
	case TYPE_JSONARRAY:
		if(item->m_Value.m_JsonArray)
			delete item->m_Value.m_JsonArray;
		break;
	case TYPE_JSONOBJ:
		if(item->m_Value.m_JsonObj)
			delete item->m_Value.m_JsonObj;
		break;
	}
	delete item;
}

void CMyJsonArray::FreeAll()
{
	if(m_ValueLen>0)
	{
		Json_Array* head =&m_Value;
		while(head)
		{
			Json_Array* tP=head;
			head		  =head->m_Next;
			FreeArrayItem(tP->m_Obj);
			if(tP!=&m_Value)delete tP;
		}
		m_ValueLen=0;
	}
	m_Value.m_Obj	=NULL;
	m_Value.m_Next	=NULL;
}

char* CMyJsonArray::SetJsonArrayStr(char *str,int len)
{
	char *pStart,*pEnd;

	FreeAll();
	pStart	=str;
	JUMP_INVALID_C(str,len-(pStart-str),pStart);

	if(*pStart!='[')return NULL;
	pStart++;

	pEnd	=pStart;
	JUMP_INVALID_C(pEnd,len-(pStart-str),pStart);
	if(*pStart==']')
		return ++pStart;

	CMyJsonObj jsonObj;
	Array_Item *item;
	while(1)
	{
		CMyJsonObj::Key_Value keyValue;
		pStart=jsonObj.GetValue(pStart,len-(pStart-str),keyValue);
		if(!pStart)break;

		pEnd	=pStart;
		JUMP_INVALID_C(pEnd,len-(pStart-str),pStart);

		if(*pStart!=']'&&*pStart!=',')
			break;
		item	=new Array_Item;
		item->m_Type=keyValue.m_Type;
		item->m_Value.m_String=keyValue.m_Value.m_String;
		keyValue.m_Value.m_String=NULL;
		AddItem(item);
		if(*pStart==']')
		{
			return pStart+1;
		}
		pStart++;
	}
	FreeAll();
	return NULL;
}

int CMyJsonArray::AddItem(Array_Item *item)
{
	if(!m_ValueLen)
	{
		m_Value.m_Obj	=item;
		m_Value.m_Next	=NULL;
		m_ValueLen++;
		return 1;
	}
	Json_Array *scan =&m_Value;
	Json_Array *pre;
	while(scan)
	{
		pre		=scan;
		scan	=scan->m_Next;
	}
	pre->m_Next	=new Json_Array;
	pre->m_Next->m_Obj=item;
	pre->m_Next->m_Next=NULL;
	m_ValueLen++;
	return 1;
}

int	CMyJsonArray::AddStringItem(char *value)
{
	Array_Item *item	=new Array_Item;
	item->m_Type		=TYPE_STRING;
	item->m_Value.m_String=new CMyString(value);
	AddItem(item);
	return 1;
}

int	CMyJsonArray::AddNumberItem(int value)
{
	Array_Item *item	=new Array_Item;
	item->m_Type		=TYPE_DIGTAL;
	item->m_Value.m_String=new CMyString(value);
	AddItem(item);
	return 1;
}

int	CMyJsonArray::AddNumberItem(double value)
{
	Array_Item *item	=new Array_Item;
	item->m_Type		=TYPE_DIGTAL;
	item->m_Value.m_String=new CMyString(value);
	AddItem(item);
	return 1;
}

int	CMyJsonArray::AddNullItem()
{
	Array_Item *item	=new Array_Item;
	item->m_Type		=TYPE_NULL;
	item->m_Value.m_String=new CMyString("null");
	AddItem(item);
	return 1;
}

int	CMyJsonArray::AddBoolItem(bool value)
{
	Array_Item *item	=new Array_Item;
	item->m_Type		=TYPE_BOOL;
	if(value)
		item->m_Value.m_String=new CMyString("true");
	else
		item->m_Value.m_String=new CMyString("false");
	AddItem(item);
	return 1;
}

int	CMyJsonArray::AddJsonObjItem(CMyJsonObj *jsonObj)
{
	Array_Item *item	=new Array_Item;
	item->m_Type		=TYPE_JSONOBJ;
	item->m_Value.m_JsonObj=jsonObj->CopyData();
	AddItem(item);
	return 1;
}

int	CMyJsonArray::AddJsonArrayItem(CMyJsonArray *jsonArray)
{
	Array_Item *item	=new Array_Item;
	item->m_Type		=TYPE_JSONARRAY;
	item->m_Value.m_JsonArray=jsonArray->CopyData();
	AddItem(item);
	return 1;
}

CMyJsonArray* CMyJsonArray::CopyData()
{
	CMyJsonArray* jsonArray=new CMyJsonArray;
	if(m_ValueLen)
	{
		Json_Array *head=&m_Value;
		while(head)
		{
			switch(head->m_Obj->m_Type)
			{
			case TYPE_STRING:
				jsonArray->AddStringItem(*head->m_Obj->m_Value.m_String);
				break;
			case TYPE_DIGTAL:
				if(head->m_Obj->m_Value.m_String->FindChar('.')>=0)
					jsonArray->AddNumberItem(head->m_Obj->m_Value.m_String->ToDouble());
				else
					jsonArray->AddNumberItem(head->m_Obj->m_Value.m_String->ToInt());
				break;
			case TYPE_BOOL:
				if((*head->m_Obj->m_Value.m_String)=="false")
					jsonArray->AddBoolItem(false);
				else
					jsonArray->AddBoolItem(true);
				break;
			case TYPE_NULL:
				jsonArray->AddNullItem();
				break;
			case TYPE_JSONOBJ:
				jsonArray->AddJsonObjItem(head->m_Obj->m_Value.m_JsonObj);
				break;
			case TYPE_JSONARRAY:
				jsonArray->AddJsonArrayItem(head->m_Obj->m_Value.m_JsonArray);
				break;
			}
			head	=head->m_Next;
		}
	}
	return jsonArray;
}

int	CMyJsonArray::DeleteAll()
{
	FreeAll();
	return 1;
}

int	CMyJsonArray::DeleteItem(int index)
{
	Json_Array *head,*pre;
	if(index>=m_ValueLen)return -1;
	if(index==0)
	{
		if(m_Value.m_Next)
		{
			m_Value.m_Obj=m_Value.m_Next->m_Obj;
			pre	=m_Value.m_Next;
			m_Value.m_Next=pre->m_Next;
			delete pre;
		}
		else
		{
			delete m_Value.m_Obj;
			m_Value.m_Obj	=NULL;
		}
	}
	else
	{
		head	=pre	=&m_Value;
		while(head&&index)
		{
			pre		=head;
			head	=head->m_Next;
			index--;
		}
		pre->m_Next=head->m_Next;
		delete head->m_Obj;
		delete head;
	}
	return 1;
}

CMyJsonObj* CMyJsonArray::GetJsonObj(int index)
{
	Array_Item* item;
	item	=GetItem(index);
	if(item)
	{
		if(item->m_Type==TYPE_JSONOBJ)
			return item->m_Value.m_JsonObj;
	}
	return NULL;
}

CMyJsonArray* CMyJsonArray::GetJsonArray(int index)
{
	Array_Item* item;
	item	=GetItem(index);
	if(item)
	{
		if(item->m_Type==TYPE_JSONARRAY)
			return item->m_Value.m_JsonArray;
	}
	return NULL;
}

int	CMyJsonArray::GetDouble(int index,double &value)
{
	Array_Item* item;
	item	=GetItem(index);
	if(item)
	{
		if(item->m_Type==TYPE_DIGTAL)
		{
			value	=item->m_Value.m_String->ToDouble();
			return 1;
		}
	}
	return -1;
}

int	CMyJsonArray::GetInt(int index,int &value)
{
	Array_Item* item;
	item	=GetItem(index);
	if(item)
	{
		if(item->m_Type==TYPE_DIGTAL)
		{
			value	=item->m_Value.m_String->ToInt();
			return 1;
		}
	}
	return -1;
}

int	CMyJsonArray::GetBool(int index,bool &value)
{
	Array_Item* item;
	item	=GetItem(index);
	if(item)
	{
		if(item->m_Type==TYPE_BOOL)
		{
			value	=*item->m_Value.m_String=="true";
			return 1;
		}
	}
	return -1;
}

int	CMyJsonArray::GetString(int index,CMyString &value)
{
	Array_Item* item;
	item	=GetItem(index);
	if(item)
	{
		if(item->m_Type==TYPE_STRING)
		{
			value	=*item->m_Value.m_String;
			return 1;
		}
	}
	return -1;
}

int	CMyJsonArray::GetString(int index,char* buf,int bufLen)
{
	Array_Item* item;
	item	=GetItem(index);
	if(item)
	{
		if(item->m_Type==TYPE_STRING)
		{
			int len=item->m_Value.m_String->GetStrLen();
			if(bufLen>=len)return -1;
			memcpy(buf,item->m_Value.m_String->GetBuffer(),len);
			buf[len]=0;
			return 1;
		}
	}
	return -1;
}

CMyJsonArray::Array_Item* CMyJsonArray::GetItem(int index)
{
	if(index>=m_ValueLen)return NULL;
	Json_Array *head=&m_Value;
	while(index)
	{
		head	=head->m_Next;
		index	--;
	}
	return head->m_Obj;
}

int	CMyJsonArray::GetJsonArrayStr(char* buf,int bufLen)
{
	CMyString str=ToString();
	int	len	=str.GetStrLen();
	if(!buf)
	{
		return len+1;
	}
	if(len>=bufLen)
		return -1;
	memcpy(buf,str.GetBuffer(),len);
	buf[len]=0;
	return 1;
}
//end  CMyJsonArray


//begin CMyJsonObj

CMyJsonObj::CMyJsonObj()
{
	m_KeyValueList.m_KeyValue	=NULL;
	m_KeyValueList.m_Next		=NULL;
	m_KeyValueListSize			=0;
}

CMyJsonObj::~CMyJsonObj()
{
	FreeAll();
}


CMyString CMyJsonObj::ToString()
{
	CMyString rStr;
	rStr		+="{";
	if(m_KeyValueListSize>0)
	{
		Json_KeyValue_List *head =&m_KeyValueList;
		while(head)
		{
			rStr	+=head->m_KeyValue->ToString();


			head	=head->m_Next;
			if(head)
			{
				rStr	+=",";
			}
		}
	}
	rStr		+="}";
	return  rStr;
}

char*	CMyJsonObj::SetJsonStr(char *str,int len)
{
	FreeAll();
	char *pStart=str,*pEnd;
	JUMP_INVALID_C(str,len,pStart);

	if(*pStart!='{')return NULL;
	pStart++;

	pEnd	=pStart;
	JUMP_INVALID_C(pEnd,len-(pStart-str),pStart);
	if(*pStart=='}')
		return ++pStart;

	Key_Value  *keyValue;
	while(1)
	{
		keyValue=new Key_Value;
		pStart	=GetKey(pStart,len-(pStart-str),keyValue->m_Key);
		if(!pStart)break;;
		
		pEnd	=pStart;
		JUMP_INVALID_C(pEnd,len-(pStart-str),pStart);
		if(*pStart!=':')break;;
		pStart++;
		pEnd	=pStart;
		JUMP_INVALID_C(pEnd,len-(pStart-str),pStart);

		pStart	=GetValue(pStart,len-(pStart-str),*keyValue);
		if(!pStart)break;
		pEnd	=pStart;
		JUMP_INVALID_C(pEnd,len-(pStart-str),pStart);
		
		if(*pStart!=','&&*pStart!='}')
		{
			//没有间隔符号，说明非法的json数据格式
			break;
		}

		if(AddKeyValue(keyValue,true)<=0)break;
		if(*pStart=='}')
		{
			//到达json数据的结尾
			return pStart+1;
		}
		//跳过间隔符号
		pStart++;
	}
	delete keyValue;
	FreeAll();
	return NULL;
}

char* CMyJsonObj::GetKey(char *str,int len,CMyString &key)
{
	char *pStart=str;
	int l=strlen(str);
	JUMP_INVALID_C(str,len,pStart);
	//如果第一个不是",则非法
	if(*pStart!='\"')return NULL;

	char *pEnd  =GetBound(pStart+1,len-(pStart-str),'\"');
	if(!pEnd)return NULL;
	key =CMyString::StringFromMem(pStart+1,0,pEnd-pStart-1);
	return pEnd+1;
}

char* CMyJsonObj::GetValue(char* str,int len,Key_Value &keyValue)
{
	char *pStart	=str;
	char *pEnd		=NULL;
	JUMP_INVALID_C(str,len,pStart);
	if(*pStart=='\"')
	{
		//字符串类型
		keyValue.m_Type	=TYPE_STRING;
		keyValue.m_Value.m_String	=new CMyString();
		char *p=IsString(pStart,len-(pStart-str),*(keyValue.m_Value.m_String));
		if(p)
		{
			return p;
		}
		delete keyValue.m_Value.m_String;
		return NULL;
	}
	else if(*pStart<='9'&&*pStart>='0'||*pStart=='+'||*pStart=='-')
	{
		//开始时数字，说明值为数字，检验数值有效性，包括小数，指数形式表示数值
		keyValue.m_Type	=TYPE_DIGTAL;
		keyValue.m_Value.m_String	=new CMyString();
		char *p=IsDigtal(pStart,len-(pStart-str),*keyValue.m_Value.m_String);
		if(p)
		{
			return p;
		}
		delete keyValue.m_Value.m_String;
		keyValue.m_Value.m_String=NULL;
		return NULL;
	}
	else if(*pStart=='[')
	{
		//数组对象类型
		keyValue.m_Type	=TYPE_JSONARRAY;
		keyValue.m_Value.m_JsonArray=new CMyJsonArray();
		char *p =keyValue.m_Value.m_JsonArray->SetJsonArrayStr(pStart,len-(pStart-str));
		if(p)
		{
			return p;
		}
		delete keyValue.m_Value.m_JsonArray;
		keyValue.m_Value.m_JsonArray=NULL;
		return NULL;
	}
	else if(*pStart=='{')
	{
		//json对象类型
		keyValue.m_Type	=TYPE_JSONOBJ;
		keyValue.m_Value.m_JsonObj=new CMyJsonObj();
		char *p=keyValue.m_Value.m_JsonObj->SetJsonStr(pStart,len-(pStart-str));
		if(p)
		{
			return p;
		}
		delete keyValue.m_Value.m_JsonObj;
		keyValue.m_Value.m_JsonObj=NULL;
		return NULL;
	}
	else
	{
		int l=len-(pStart-str);
		//剩余长度不足，直接返回
		if(l<5)
			return NULL;
		CMyString value=CMyString::StringFromMem(pStart,0,4);
		keyValue.m_Value.m_String	=new CMyString();
		if(value.CompareI("null"))
		{
			//值为数值
			keyValue.m_Type	=TYPE_NULL;
		}
		else if((value.CompareI("fals")&&(*(pStart+4)=='e'||*(pStart+4)=='E'))||value.CompareI("true"))
		{
			//值为布尔值
			keyValue.m_Type	=TYPE_BOOL;
			//如果第一个字母不是t则值为false,重新对value赋值
			if(*pStart!='t')
			{
				pStart++;
				value="false";
			}
		}
		else
		{
			//无效值
			keyValue.m_Type	=TYPE_INVALID;
			delete keyValue.m_Value.m_String;
			keyValue.m_Value.m_String=NULL;
			return NULL;
		}
		*keyValue.m_Value.m_String =value;
		return pStart+4;
		//检验最终数据有效性,结尾是否正确
	}
	return NULL;
}

char* CMyJsonObj::GetBound(char *str,int len,char startC)
{
	char endC;
	bool flag	=false;   //决定配对符号是否支持嵌套
	int	 count	=0;		  //嵌套层次计数器

	switch(startC)
	{
	case '{':
		endC='}';
		flag=true;
		break;
	case '[':
		endC=']';
		flag=true;
		break;
	case '\"':
		endC='\"';
		break;
	default:
		return NULL;
	}
	char *p	=str;
	JUMP_INVALID_C(str,len,p);
	//寻找配对符号，如果是配对符号但前面有转义符号，则跳过
	while(*p&&(p-str<len)&&(*p!=endC||*(p-1)=='\\')||(flag&&!count))
	{
		if(flag&&*p==startC)
			count++;
		if(flag&&*p==endC)
			count--;
		p++;
	}

	if(*p==endC)
		return p;
	return NULL;
}

char* CMyJsonObj::IsString(char *str,int len,CMyString &outStr)
{
	char *pStart,*pEnd;
	pStart =pEnd =str;
	pEnd	=GetBound(pStart+1,len,'\"');
	if(pEnd)
	{
		outStr=CMyString::StringFromMem(pStart+1,0,pEnd-pStart-1);
		return pEnd+1;
	}
	return NULL;
}

char* CMyJsonObj::IsDigtal(char *str,int len,CMyString &outStr)
{
	char *pStart,pEnd;
	int	  dataType=1;  //1为整数，2为小数，3为指数
	pStart	=str;

	if(*pStart=='+'||*pStart=='-')
		pStart++;

	if(*pStart<'0'||*pStart>'9')return NULL;
	pStart++;

	while(*pStart&&(pStart-str<len))
	{
		if(dataType==1&&*pStart=='.')
		{
			dataType=2;
		}
		else if(*pStart>'9'||*pStart<'0')
		{
			//浮点数的点在最后，则不合法
			if(dataType==2&&*(pStart-1)=='.')return NULL;
			outStr=CMyString::StringFromMem(str,0,pStart-str);
			return pStart;
		}
		pStart++;
	}
	return NULL;
}

int CMyJsonObj::AddKeyValue(Key_Value *keyValue,bool withCheck)
{
	Json_KeyValue_List *scan=&m_KeyValueList;
	Json_KeyValue_List *pre =scan;
	if(!m_KeyValueListSize)
	{
		scan->m_KeyValue=keyValue;
		scan->m_Next	=NULL;
		m_KeyValueListSize++;
		return 1;
	}
	while(scan)
	{
		if(withCheck&&(scan->m_KeyValue->m_Key==keyValue->m_Key))return -1;
		pre =scan;
		scan=scan->m_Next;
	}
	pre->m_Next=new Json_KeyValue_List;
	pre->m_Next->m_KeyValue=keyValue;
	pre->m_Next->m_Next	   =NULL;
	m_KeyValueListSize++;
	return 1;
}


int	CMyJsonObj::PutStringKeyValue(char* key,char* value)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_STRING);
	if(CAN_PUT_KEY_VALUE(keyValue,TYPE_STRING))
	{
		if(keyValue)
		{
			*keyValue->m_Value.m_String=value;
		}
		else
		{
			keyValue		=new Key_Value;
			keyValue->m_Key	=key;
			keyValue->m_Type=TYPE_STRING;
			keyValue->m_Value.m_String=new CMyString(value);
			AddKeyValue(keyValue,false);
		}
		return 1;
	}
	return -1;
}

int	CMyJsonObj::PutNumberKeyValue(char* key,double value)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_DIGTAL);
	if(CAN_PUT_KEY_VALUE(keyValue,TYPE_DIGTAL))
	{
		if(keyValue)
		{
			*keyValue->m_Value.m_String=value;
		}
		else
		{
			keyValue		=new Key_Value;
			keyValue->m_Key	=key;
			keyValue->m_Type=TYPE_DIGTAL;
			keyValue->m_Value.m_String=new CMyString(value);
			AddKeyValue(keyValue,false);
		}
		return 1;
	}
	return -1;
}

int	CMyJsonObj::PutNumberKeyValue(char* key,int value)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_DIGTAL);
	if(CAN_PUT_KEY_VALUE(keyValue,TYPE_DIGTAL))
	{
		if(keyValue)
		{
			*keyValue->m_Value.m_String=value;
		}
		else
		{
			keyValue		=new Key_Value;
			keyValue->m_Key	=key;
			keyValue->m_Type=TYPE_DIGTAL;
			keyValue->m_Value.m_String=new CMyString(value);
			AddKeyValue(keyValue,false);
		}
		return 1;
	}
	return -1;
}

int	CMyJsonObj::PutBoolKeyValue(char *key,bool value)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_BOOL);
	if(CAN_PUT_KEY_VALUE(keyValue,TYPE_BOOL))
	{
		if(keyValue)
		{
			*keyValue->m_Value.m_String=(value?"true":"false");
		}
		else
		{
			keyValue		=new Key_Value;
			keyValue->m_Key	=key;
			keyValue->m_Type=TYPE_BOOL;
			keyValue->m_Value.m_String=new CMyString((value?"true":"false"));
			AddKeyValue(keyValue,false);
		}
		return 1;
	}
	return -1;
}

int	CMyJsonObj::PutNullKeyValue(char *key)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_NULL);
	if(CAN_PUT_KEY_VALUE(keyValue,TYPE_NULL))
	{
		if(keyValue)
		{
			*keyValue->m_Value.m_String="null";
		}
		else
		{
			keyValue		=new Key_Value;
			keyValue->m_Key	=key;
			keyValue->m_Type=TYPE_NULL;
			keyValue->m_Value.m_String=new CMyString("null");
			AddKeyValue(keyValue,false);
		}
		return 1;
	}
	return -1;
}

int	CMyJsonObj::PutJsonObjKeyValue(char *key,CMyJsonObj *jsonObj)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_JSONOBJ);
	if(CAN_PUT_KEY_VALUE(keyValue,TYPE_JSONOBJ))
	{
		if(keyValue)
		{
			delete keyValue->m_Value.m_JsonObj;
			keyValue->m_Value.m_JsonObj=jsonObj->CopyData();
		}
		else
		{
			keyValue		=new Key_Value;
			keyValue->m_Key	=key;
			keyValue->m_Type=TYPE_JSONOBJ;
			keyValue->m_Value.m_JsonObj=jsonObj->CopyData();
			AddKeyValue(keyValue,false);
		}
		return 1;
	}
	return -1;
}

int	CMyJsonObj::PutJsonArrayKeyValue(char*key,CMyJsonArray *jsonArray)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_JSONOBJ);
	if(CAN_PUT_KEY_VALUE(keyValue,TYPE_JSONOBJ))
	{
		if(keyValue)
		{
			delete keyValue->m_Value.m_JsonObj;
			keyValue->m_Value.m_JsonArray=jsonArray->CopyData();
		}
		else
		{
			keyValue		=new Key_Value;
			keyValue->m_Key	=key;
			keyValue->m_Type=TYPE_JSONARRAY;
			keyValue->m_Value.m_JsonArray=jsonArray->CopyData();
			AddKeyValue(keyValue,false);
		}
		return 1;
	}
	return -1;
}

int	CMyJsonObj::DeleteKeyValue(char* key)
{
	Json_KeyValue_List *head,*pre;
	pre=  head	=&m_KeyValueList;
	if(m_KeyValueListSize)
	{
		while(head&&!(head->m_KeyValue->m_Key==key))
		{
			pre	=head;
			head=head->m_Next;
		}
		if(!head)return -1;
		if(head==&m_KeyValueList)
		{
			delete head->m_KeyValue;
			if(head->m_Next)
			{
				head->m_KeyValue=head->m_Next->m_KeyValue;
				pre	=head->m_Next;
				pre->m_KeyValue	=NULL;
				head->m_Next	=head->m_Next->m_Next;
				delete pre;
			}
			else 
			{
				head->m_KeyValue=NULL;
			}
		}
		else
		{
			pre->m_Next=head->m_Next;
			delete head;
		}
		return 1;
	}
	return -1;
}

CMyJsonObj::Key_Value*	CMyJsonObj::CanAddOrModify(char *key,Value_Type type)
{
	Json_KeyValue_List *head;
	head	=&m_KeyValueList;
	if(m_KeyValueListSize)
	{
		while(head&&!(head->m_KeyValue->m_Key==key))
			head=head->m_Next;
		if(head)
			return head->m_KeyValue;
	}
	return NULL;
}

void CMyJsonObj::FreeAll()
{
	Json_KeyValue_List *head;
	head	=&m_KeyValueList;
	if(m_KeyValueListSize)
	{
		while(head)
		{
			Json_KeyValue_List *tP=head;
			head		=head->m_Next;
			if(tP!=&m_KeyValueList)
				delete tP;
			else
				delete tP->m_KeyValue;
		}
		m_KeyValueListSize=0;
	}
	m_KeyValueList.m_KeyValue=NULL;
	m_KeyValueList.m_Next	 =NULL;
}

CMyJsonObj* CMyJsonObj::CopyData()
{
	CMyJsonObj* jsonObj=new CMyJsonObj();
	if(m_KeyValueListSize)
	{
		Json_KeyValue_List *head;
		head	=&m_KeyValueList;
		while(head)
		{
			switch(head->m_KeyValue->m_Type)
			{
			case TYPE_STRING:
				jsonObj->PutStringKeyValue(head->m_KeyValue->m_Key,*head->m_KeyValue->m_Value.m_String);
				break;
			case TYPE_DIGTAL:
				if(head->m_KeyValue->m_Value.m_String->FindChar('.')>=0)
					jsonObj->PutNumberKeyValue(head->m_KeyValue->m_Key,head->m_KeyValue->m_Value.m_String->ToDouble());
				else
					jsonObj->PutNumberKeyValue(head->m_KeyValue->m_Key,head->m_KeyValue->m_Value.m_String->ToInt());
				break;
			case TYPE_BOOL:
				if((*head->m_KeyValue->m_Value.m_String)=="false")
					jsonObj->PutBoolKeyValue(head->m_KeyValue->m_Key,false);
				else
					jsonObj->PutBoolKeyValue(head->m_KeyValue->m_Key,true);
				break;
			case TYPE_NULL:
				jsonObj->PutNullKeyValue(head->m_KeyValue->m_Key);
				break;
			case TYPE_JSONOBJ:
				jsonObj->PutJsonObjKeyValue(head->m_KeyValue->m_Key,head->m_KeyValue->m_Value.m_JsonObj);
				break;
			case TYPE_JSONARRAY:
				jsonObj->PutJsonArrayKeyValue(head->m_KeyValue->m_Key,head->m_KeyValue->m_Value.m_JsonArray);
				break;
			}
			head	=head->m_Next;
		}
	}
	return jsonObj;
}

int CMyJsonObj::GetDoubleValue(char *key,double &value)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_DIGTAL);
	if(keyValue)
	{
		value	=keyValue->m_Value.m_String->ToDouble();
		return 1;
	}
	return -1;
}

int	CMyJsonObj::GetIntValue(char *key,int &value)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_DIGTAL);
	if(keyValue)
	{
		value	=keyValue->m_Value.m_String->ToInt();
		return 1;
	}
	return -1;
}

int CMyJsonObj::GetBoolValue(char *key,bool &value)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_BOOL);
	if(keyValue)
	{
		value	=(*keyValue->m_Value.m_String=="true");
		return 1;
	}
	return -1;
}

CMyJsonObj*	CMyJsonObj::GetJsonValue(char *key)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_JSONOBJ);
	if(keyValue)
	{
		return keyValue->m_Value.m_JsonObj;
	}
	return NULL;
}

CMyJsonArray* CMyJsonObj::GetJsonArrayValue(char* key)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_JSONARRAY);
	if(keyValue)
	{
		return keyValue->m_Value.m_JsonArray;
	}
	return NULL;
}

int	CMyJsonObj::GetStringValue(char *key,char *buf,int &bufLen)
{
	CMyString str;
	if(GetStringValue(key,str)>0)
	{
		int len	=str.GetStrLen();
		if(!buf)
		{
			bufLen	=len+1;
			return 1;
		}
		if(bufLen<=len)return -1;
		strcpy(buf,str.GetBuffer());
		return 1;
	}
	return -1;
}

int	CMyJsonObj::GetStringValue(char *key,CMyString &str)
{
	Key_Value *keyValue;
	keyValue	=CanAddOrModify(key,TYPE_STRING);
	if(keyValue)
	{
		str	=*keyValue->m_Value.m_String;
		return 1;
	}
	return -1;
}

int	CMyJsonObj::GetJsonStr(char* buf,int bufLen)
{
	CMyString str=ToString();
	int	len	=str.GetStrLen();
	if(!buf)
	{
		return len+1;
	}
	if(len>=bufLen)
		return -1;
	memcpy(buf,str.GetBuffer(),len);
	buf[len]=0;
	return 1;
}
//end CMyJsonObj