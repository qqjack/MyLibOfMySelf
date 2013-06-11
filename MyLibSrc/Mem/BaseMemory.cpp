#include "../MyLibHeader/Mem/BaseMemory.h"

CLinKMemory::CLinKMemory()
{
	m_MemArea	=0;
	m_Lock		=NULL;
}

CLinKMemory::~CLinKMemory()
{
}

void CLinKMemory::InitalMemory(void* mem,unsigned long memLen,unsigned long itemLen)
{
	m_MemArea	=mem;
	m_MemSize	=memLen;
	m_MaxItem	=memLen/(itemLen+8);
	m_ItemSize	=itemLen;
	m_ItemCount	=0;
	m_DataHeader=-1;
	m_FreeHeader=0;
	m_DataEnd	=-1;
	for(int i=0;i<m_MaxItem;i++)
	{
		Mem_Item_Next(i)=i+1;
		Mem_Item_Prev(i)=i-1;
	}
	Mem_Item_Next(m_MaxItem-1)=-1;
}

void* CLinKMemory::Alloc()
{
	if(!m_MemArea)return 0;
	if(IsFull())return 0;
	if(m_DataHeader==-1)
	{
		m_DataHeader	=m_FreeHeader;
	}
	else
	{
		Mem_Item_Next(m_DataEnd)	=m_FreeHeader;
	}
	int pos	=m_FreeHeader;
	m_FreeHeader	=Mem_Item_Next(m_FreeHeader);

	if(m_FreeHeader!=-1)
		Mem_Item_Prev(m_FreeHeader)=-1;

	Mem_Item_Next(pos)	=-1;
	Mem_Item_Prev(pos)	=m_DataEnd;
	m_DataEnd		=pos;
	m_ItemCount++;
	return GET_MEM_ITEM_DATA(pos);
}

void CLinKMemory::Free(void *memItem)
{
	if(!m_MemArea)return;
	long offset=(long)memItem-(long)m_MemArea;
	//地址不合法，地址范围小于基址
	if(offset<0)return;

	//地址不合法，地址范围大于总地址范围
	unsigned count=offset/(m_ItemSize+8);
	if(count>m_MaxItem)return;
	//地址不合法，没有停在数据边界
	if(offset%(m_ItemSize+8))return;

	if(count==m_DataHeader)
	{
		m_DataHeader=Mem_Item_Next(count);
		if(m_DataHeader!=-1)
			Mem_Item_Prev(m_DataHeader)=-1;
		else
			m_DataEnd	=-1;
	}
	else
	{
		unsigned next=Mem_Item_Next(count);
		unsigned prev=Mem_Item_Prev(count);
		Mem_Item_Next(prev)	=next;
		if(next!=-1)
			Mem_Item_Prev(next)=prev;
	}
	
	Mem_Item_Next(count)	=m_FreeHeader;
	Mem_Item_Prev(count)	=-1;
	if(m_FreeHeader!=-1)
		Mem_Item_Prev(m_FreeHeader)=count;

	m_FreeHeader			=count;
	m_ItemCount--;
	if(!m_ItemCount)m_DataEnd	=-1;
}

void CLinKMemory::Lock()
{
	if(!m_Lock)
		m_Lock=new CMemLock();

	if(m_Lock)m_Lock->Lock();
}

void CLinKMemory::UnLock()
{
	if(m_Lock)m_Lock->UnLock();
}