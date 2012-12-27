//#include "MyArrayList.h"
//
//template<class _T>
//CMyArrayList<_T>::CMyArrayList()
//{
//
//}
//
//template<class _T>
//CMyArrayList<_T>::~CMyArrayList()
//{
//
//}
//
//template<class _T>
//void CMyArrayList<_T>::SetMem(void *mem,int memSize)
//{
//	m_MemData	=(_Item*)mem;
//	m_MemSize	=memSize;
//	m_MaxItem	=memSize/sizeof(_T);
//	m_ItemCount	=0;
//	m_DataHeader=-1;
//	m_FreeHeader=0;
//	m_DataEnd	=-1;
//	for(int i=0;i<m_MaxItem-1;i++)
//		m_MemData[i].iNext=i+1;
//	m_MemData[m_MaxItem-1].iNext=-1;
//}
//
//template<class _T>
//int	CMyArrayList<_T>::AddItem(_T &item)
//{
//	if(IsFull())return -1;
//	if(m_DataHeader==-1)
//	{
//		m_DataHeader	=m_FreeHeader;
//		m_DataEnd		=m_DataHeader;
//	}
//	else
//	{
//		m_MemData[m_DataEnd].iNext	=m_FreeHeader;
//	}
//	m_MemData[m_FreeHeader].iNext	=-1;
//	m_MemData[m_FreeHeader].iData	=item;
//	m_FreeHeader	=m_MemData[m_FreeHeader].iNext;
//	m_ItemCount++;
//	return m_ItemCount-1;
//}
//
//template<class _T>
//int	CMyArrayList<_T>::DelItem(int index)
//{
//	if(index>=m_ItemCount)return -1;
//	int pos=m_DataHeader,pre;
//	while(index)
//	{
//		pre		=pos;
//		pos		=m_MemData[pos].iNext;
//		index--;
//	}
//	if(pos==m_DataHeader)
//		m_DataHeader=m_MemData[pos].iNext;
//	else
//		m_MemData[pre].iNext	=m_MemData[pos].iNext;
//	m_MemData[pos].iNext	=m_FreeHeader;
//	m_FreeHeader			=pos;
//	m_ItemCount--;
//	return 1;
//}
//
//template<class _T>
//int	CMyArrayList<_T>::InsertItem(_T &item,int index)
//{
//	if(IsFull())return -1;
//	if(index>=m_ItemCount)return -1;
//	int pos=m_DataHeader,pre;
//	while(index)
//	{
//		pre		=pos;
//		pos		=m_MemData[pos].iNext;
//		index--;
//	}
//	
//	m_MemData[m_FreeHeader].iNext	=pos;
//	if(pos==m_DataHeader)
//		m_DataHeader	=m_FreeHeader;
//	else
//		m_MemData[pre].iNext	=m_FreeHeader;
//
//	m_MemData[m_FreeHeader].iData	=item;
//	m_FreeHeader	=m_MemData[m_FreeHeader].iNext;
//	m_ItemCount++;
//	return m_ItemCount-1;
//}
//
//template<class _T>
//int	CMyArrayList<_T>::GetItem(int index,_T &item)
//{
//	if(index>=m_ItemCount)return -1;
//	int pos=m_DataHeader;
//	while(index)
//	{
//		pos		=m_MemData[pos].iNext;
//		index--;
//	}
//	item=m_MemData[pos];
//	return 1;
//}
//
//template<class _T>
//int	CMyArrayList<_T>::GetItemCount()
//{
//	return m_ItemCount;
//}
//
//template<class _T>
//int	CMyArrayList<_T>::GetListSpace()
//{
//	return m_MaxItem;
//}
//
//template<class _T>
//bool CMyArrayList<_T>::IsEmpty()
//{
//	return (m_ItemCount==0);
//}
//
//template<class _T>
//bool CMyArrayList<_T>::IsFull()
//{
//	return (m_MaxItem==m_ItemCount);
//}
//
//template<class _T>
//int	CMyArrayList<_T>::Clear()
//{
//	m_ItemCount	=0;
//	m_DataHeader=-1;
//	m_FreeHeader=0;
//	m_DataEnd	=-1;
//	_Item*	array=(_Item*)m_MemData;
//	for(int i=0;i<m_MaxItem-1;i++)
//		array[i].iNext=i+1;
//	array[m_MaxItem-1].iNext=-1;
//	return 1;
//}
