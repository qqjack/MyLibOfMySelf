#ifndef __FIX_MEM__
#define __FIX_MEM__
#include "BaseMemory.h"

#define DEFAULT_ITME_COUNT	(720)

template<class _T,class _Alloc=CLinKMemory>
class CFixedTMemory  
{
private:

	struct _Array_Item
	{
		_T	iDataItem;
		int	iResver[2];
	};
public:

	CFixedTMemory(int maxItemCount=DEFAULT_ITME_COUNT)
	{
		m_ArraySize	=maxItemCount;
		m_Array	=new _Array_Item[m_ArraySize];
		m_Mem.InitalMemory(m_Array,sizeof(_Array_Item)*m_ArraySize,sizeof(_T));
	}

	virtual ~CFixedTMemory()
	{
		if(m_Array)
			delete[] m_Array;
	}

	_T*	AllocMem()
	{
		if(!m_Array)return NULL;
		return (_T*)m_Mem.AllocMem();
	}

	void FreeMem(_T* data)
	{
		if(!m_Array)return;
		m_Mem.FreeMem(data);
	}

	bool IsFull()
	{
		if(!m_Array)return true;
		return m_Mem.IsFull();
	}

	void ClearLockMark()
	{
		m_Mem.ClearLockMark();
	}

	void* GetBaseAddr()
	{
		return (void*)m_Array;
	}
private:
	_Alloc			m_Mem;

	_Array_Item*	m_Array;
	unsigned long	m_ArraySize;
};

#endif