#ifndef __MY_HASH_MAP__
#define __MY_HASH_MAP__

#define BUKET_SIZE		(1024*4)

template<class _T>
unsigned DefaultGetHashCode(_T obj)
{
	int *hashCode	=(int*)(&obj);
	return *hashCode;
}

template<class _T>
int DefaultEqual(_T obj1,_T obj2)
{
	return !(obj1==obj2);
}

template<class _KEYT,class _VALUET>
class CMyHashMap  
{
public:
	typedef unsigned (*GET_HASH_CODE)(_KEYT obj);
	typedef int	(*EQUAL)(_KEYT obj1,_KEYT obj2);

private:
	typedef struct _HashItem
	{
		_KEYT			iKey;
		_VALUET			iData;
		_HashItem*		iNext;
	} HashItem;

public:
	CMyHashMap( GET_HASH_CODE hashCodeFun=DefaultGetHashCode,
				EQUAL		  equalFun	 =DefaultEqual)
	{
		m_HashFun	=hashCodeFun;
		m_EqualFun	=equalFun;
		for(int i=0;i<BUKET_SIZE;i++)
			m_Buket[i]=NULL;
	}

	virtual ~CMyHashMap()
	{
		for(int i=0;i<BUKET_SIZE;i++)
		{
			if(m_Buket[i])
			{
				HashItem*	p=m_Buket[i];
				HashItem*	tP;
				do
				{
					tP=p;
					p	=p->iNext;
					delete tP;
				}while(p->iNext);
			}
		}
	}

	int	GetItem(_KEYT key,_VALUET* value)
	{
		int index		=m_HashFun(key)%BUKET_SIZE;
		if(m_Buket[index])
		{
			HashItem*	p=m_Buket[index];
			do
			{
				if(!m_EqualFun(key,p->iKey))
				{
					if(value)
						*value	=p->iData;
					return 1;
				}
				p=p->iNext;
			}while(p);
		}
		return -1;
	}

	int SetItem(_KEYT key,_VALUET& value)
	{
		int index		=m_HashFun(key)%BUKET_SIZE;
		if(m_Buket[index])
		{
			HashItem*	p=m_Buket[index];
			do
			{
				if(!m_EqualFun(key,p->iKey))
				{
					p->iData	=value;
					return 1;
				}
				p	=p->iNext;
			}while(p);
		}
		return -1;
	}

	int DeleteItem(_KEYT key)
	{
		int index		=m_HashFun(key)%BUKET_SIZE;
		if(m_Buket[index])
		{
			HashItem*	pre;
			HashItem*	p=m_Buket[index];
			do
			{
				pre	=p;
				if(!m_EqualFun(key,p->iKey))
				{
					if(p==m_Buket[index])
					{
						m_Buket[index]=p->iNext;
						delete p;
					}
					else
					{
						pre	->iNext=p->iNext;
						delete p;
					}
					return 1;
				}
				p	=p->iNext;
			}while(p);
		}
		return -1;
	}

	int	AddItem(_KEYT key,_VALUET& value)
	{
		if(GetItem(key,NULL)==-1)
		{
			int index		=m_HashFun(key)%BUKET_SIZE;
			HashItem*	newItem=new HashItem;
			newItem->iNext	=NULL;
			newItem->iData	=value;
			newItem->iKey	=key;
			HashItem*	p=m_Buket[index];
			if(!p)
			{
				m_Buket[index]	=newItem;
				return 1;
			}
			while(p->iNext)p=p->iNext;
			p->iNext	=newItem;
			return 1;
		}
		return -1;
	}

private:
	HashItem*		m_Buket[BUKET_SIZE];
	GET_HASH_CODE	m_HashFun;
	EQUAL			m_EqualFun;
};

#endif