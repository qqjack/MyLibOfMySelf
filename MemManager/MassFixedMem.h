#ifndef __MASS_FIXED_MEM__
#define __MASS_FIXED_MEM__

#include "FixedTMemory.h"
#define DEFAULT_BUCKETS (10)
#define MAX_DATA_ITEM	(2000)
#define ADD_EXTERN (2)

template<class _T,class _Alloc=CLinKMemory>
class CMassFixedMem  
{
public:
	CMassFixedMem()
	{
		m_Bucket	=(CFixedTMemory<_T,_Alloc>**)malloc(DEFAULT_BUCKETS);
		if(m_Bucket)return;
		m_BucketCount	=DEFAULT_BUCKETS;
		for(int i=0;i<m_BucketCount;i++)
			m_Bucket[i]=NULL;
		m_Bucket[i]=new CFixedTMemory<_T,_Alloc>(MAX_DATA_ITEM);
		m_Bucket[i]->ClearLockMark();
		m_UseBucketCount	=1;
	}

	virtual ~CMassFixedMem()
	{
		for(int i=0;i<m_UseBucketCount;i++)
			delete m_Bucket[i];
		free(m_Bucket);
	}
	
	_T*		AllocMem()
	{
		_T* data;
		Lock();
		data	=Alloc();
		UnLock();
		return data;
	}

	void	FreeMem(_T* data)
	{
		Lock();
		Free(data);
		UnLock();
	}

private:

	int GetBucketIndex(_T* data)
	{
		if((DWORD)data>(DWROD)m_Bucket[m_UseBucketCount-1]||
			(DWORD)data<(DWROD)m_Bucket[0])return -1;
		
		int index=SearchTwo(0,m_UseBucketCount-1);
		return 0;
	}

	int SearchTwo(int start,int end)
	{
	}

	void SortBucket()
	{
	}

	void Lock()
	{
		m_Lock.Lock();
	}

	void UnLock()
	{
		m_Lock.UnLock();
	}

	_T*	Alloc()
	{
		for(int i=0;i<m_UseBucketCount;i++)
		{
			if(!m_Bucket[i]->IsFull())break;
		}
		if(i<m_UseBucketCount)
		{
			return m_Bucket[i]->AllocMem();
		}
		if(m_UseBucketCount>=m_BucketCount)
		{
			m_Bucket	=(CFixedTMemory<_T,_Alloc>**)realloc(m_Bucket,m_BucketCount+ADD_EXTERN);
			m_BucketCount	+=ADD_EXTERN;
			SortBucket();
		}
		m_Bucket[m_UseBucketCount++]=new CFixedTMemory<_T,_Alloc>(MAX_DATA_ITEM);
		if(m_Bucket[m_UseBucketCount-1])
		{
			m_Bucket[m_UseBucketCount-1]->ClearLockMark();
			return m_Bucket[m_UseBucketCount-1]->AllocMem();
		}
		return NULL;
	}

	void Free()
	{
		int whichBucket=GetBucketIndex(data);
		if(whichBucket==-1||whichBucket>=m_UseBucketCount)return;
		m_Bucket[whichBucket]->FreeMem(data);
	}

private:
	CFixedTMemory<_T,_Alloc>**	m_Bucket;
	unsigned long	m_BucketCount;
	unsigned long	m_UseBucketCount;

	CMemLock		m_Lock;
};

#endif