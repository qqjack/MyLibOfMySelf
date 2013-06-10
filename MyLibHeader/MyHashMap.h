#ifndef __MY_HASH_MAP__
#define __MY_HASH_MAP__
#include <vector>

using namespace std;

#define BUKET_SIZE		(4117)

template<class _T>
class DefaultGetHashCode
{
public:
	unsigned operator()(const _T &obj)
	{
		unsigned *hashCode	=(unsigned*)(&obj);
		return *hashCode;
	}
};

template<class _T>
class DefaultEqual
{
public:
	bool operator()(const _T &obj1,const _T &obj2)
	{
		return ((_T&)obj1==(_T&)obj2);
	}
};

template<class _KT,class _VT,class _HFT=DefaultGetHashCode<_KT>,class _EFT=DefaultEqual<_KT> >
class CMyHashMap  
{
private:
	typedef struct _HashItem
	{
		_KT				iKey;
		_VT				iData;
		_HashItem*		iNext;
	} HashItem;

public:

	CMyHashMap(_HFT hashCodeF=_HFT(),_EFT equalF=_EFT())
	{
		m_HashFun	=hashCodeF;
		m_EqualFun	=equalF;
		for(int i=0;i<BUKET_SIZE;i++)
			m_Buket[i]=NULL;
	}

	virtual ~CMyHashMap()
	{
		Clear();
	}

	void Clear()
	{
		int size=m_KeyList.size();
		for(int i=0;i<size;i++)
			DeleteItem(m_KeyList[i],false);

		m_KeyList.clear();
	}

	int	GetItem(_KT& key,_VT* value)
	{
		int index		=m_HashFun((const _KT &)key)%BUKET_SIZE;
		if(m_Buket[index])
		{
			HashItem*	p=m_Buket[index];
			do
			{
				if(m_EqualFun(key,p->iKey))
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

	int SetItem(_KT& key,_VT& value)
	{
		int index		=m_HashFun((const _KT &)key)%BUKET_SIZE;
		if(m_Buket[index])
		{
			HashItem*	p=m_Buket[index];
			do
			{
				if(m_EqualFun(key,p->iKey))
				{
					p->iData	=value;
					return 1;
				}
				p	=p->iNext;
			}while(p);
		}
		return -1;
	}

	int DeleteItem(_KT& key)
	{
		return DeleteItem(key,true);
	}

	int	AddItem(_KT& key,_VT& value)
	{
		if(GetItem(key,NULL)==-1)
		{
			int index		=m_HashFun((const _KT &)key)%BUKET_SIZE;
			HashItem*	newItem=new HashItem;
			newItem->iNext	=NULL;
			newItem->iData	=value;
			newItem->iKey	=key;
			HashItem*	p=m_Buket[index];
			if(!p)
			{
				m_Buket[index]	=newItem;
				m_KeyList.push_back(key);
				return 1;
			}
			while(p->iNext)p=p->iNext;
			p->iNext	=newItem;

			m_KeyList.push_back(key);
			return 1;
		}
		return -1;
	}

	int GetKey(int index,_KT& key)
	{
		if(index>=m_KeyList.size())return -1;
		key=m_KeyList[index];
		return 1;
	}

	int Size()
	{
		return m_KeyList.size();
	}

private:

	int DeleteItem(_KT& key,bool updateKey)
	{
		int index		=m_HashFun((const _KT &)key)%BUKET_SIZE;
		if(m_Buket[index])
		{
			HashItem*	pre;
			HashItem*	p=m_Buket[index];
			do
			{
				pre	=p;
				if(m_EqualFun(key,p->iKey))
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

					if(updateKey)EraseKeyList(key);
					return 1;
				}
				p	=p->iNext;
			}while(p);
		}
		return -1;
	}

	void EraseKeyList(_KT& key)
	{
		int size=m_KeyList.size();
		int i=0;
		for(;i<size;i++)
		{
			if(m_EqualFun(key,m_KeyList[i]))break;
		}

		if(i<size)
		{
			std::vector<_KT>::iterator it=m_KeyList.begin()+i;
			m_KeyList.erase(it);
		}
	}

private:
	HashItem*		m_Buket[BUKET_SIZE];
	_HFT			m_HashFun;
	_EFT			m_EqualFun;

	std::vector<_KT>	m_KeyList;
};

#endif