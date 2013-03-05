#ifndef __MASS_FIXED_MEM__
#define __MASS_FIXED_MEM__

#include "FixedTMemory.h"
#define DEFAULT_BUCKETS (10)
#define MAX_DATA_ITEM	(500)
#define ADD_EXTERN (2)

template<class _T,class _Alloc=CLinKMemory>
class CMassFixedMem  
{
public:
	CMassFixedMem()
	{
		m_Bucket	=(CFixedTMemory<_T,_Alloc>**)malloc(DEFAULT_BUCKETS*4);
		if(!m_Bucket)return;
		m_BucketCount	=DEFAULT_BUCKETS;
		for(int i=0;i<m_BucketCount-1;i++)
			m_Bucket[i]=NULL;
		m_Bucket[0]=new CFixedTMemory<_T,_Alloc>(MAX_DATA_ITEM);
		m_Bucket[0]->ClearLockMark();
		m_UseBucketCount	=1;
		m_AllocCount		=0;
	}

	virtual ~CMassFixedMem()
	{
		if(m_Bucket)
		{
			for(int i=0;i<m_UseBucketCount;i++)
				delete m_Bucket[i];
			free(m_Bucket);
		}
	}
	
	_T*		AllocMem()
	{
		_T* data;
		Lock();
		data	=Alloc();
		m_AllocCount++;
		UnLock();
		return data;
	}

	void	FreeMem(_T* data)
	{
		Lock();
		Free(data);
		m_AllocCount--;
		UnLock();
	}

	unsigned long GetAllocCount()
	{
		return m_AllocCount;
	}
private:

	int GetBucketIndex(_T* data)
	{
//		if((unsigned)data>(unsigned)m_Bucket[m_UseBucketCount-1]->GetBaseAddr()||
//			(unsigned)data<(unsigned)m_Bucket[0]->GetBaseAddr())return -1;

//		int index=SearchTwo((void*)data,0,m_UseBucketCount-1);
		for(int i=0;i<m_UseBucketCount;i++)
		{
			if((unsigned)data>=(unsigned)m_Bucket[i]->GetBaseAddr()&&
				(unsigned)data<(unsigned)m_Bucket[i]->GetBaseAddr()+MAX_DATA_ITEM*(sizeof(_T)+8))
				break;
		}
		if(i>=m_UseBucketCount)return -1;
		return i;
	}

	int SearchTwo(void*	data,int start,int end)
	{
		int mid=start+end/2;
		if(data>m_Bucket[mid].GetBaseAddr())start=mid;
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
			if(!m_Bucket)return NULL;
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

	void Free(_T* data)
	{
		int whichBucket=GetBucketIndex(data);
		if(whichBucket==-1||whichBucket>=m_UseBucketCount)return;
		m_Bucket[whichBucket]->FreeMem(data);
	}

	
private:
	CFixedTMemory<_T,_Alloc>**	m_Bucket;
	unsigned long	m_BucketCount;
	unsigned long	m_UseBucketCount;
	unsigned long	m_AllocCount;

	CMemLock		m_Lock;
};

#endif