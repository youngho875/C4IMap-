// safeQueue.cpp 
//

#include "PCH.h"
#include "SafeQueue.h"
#include <afxmt.h>

safeQueue::safeQueue()
{
	//CCriticalSection csection;
	//CPtrList list;
	//by Xman94 2009-08-19
	list.RemoveAll();

}

safeQueue::~safeQueue()
{
	while( !list.IsEmpty() )
		list.RemoveHead();
}

BOOL safeQueue::push( LPVOID pData )
{

	CSingleLock lock(&csection, TRUE);
	list.AddTail( pData );
	return TRUE;
}

LPVOID safeQueue::pop()
{
	CSingleLock lock(&csection, TRUE);
	LPVOID pItem = list.IsEmpty() ? NULL : list.RemoveHead();
	return pItem;
}

int	safeQueue::popAll( CPtrList& list2 )
{
	CSingleLock lock(&csection, TRUE);
	while( !list.IsEmpty() )
		list2.AddTail( list.RemoveHead() );
	return list2.GetCount();
}

int	safeQueue::Remove( int iCnt )
{
	int iRet=0;
	if( iCnt <= 0 )
		return 0;

	CSingleLock lock(&csection, TRUE);
	while( !list.IsEmpty() )
	{
		list.RemoveHead();
		if( ++iRet == iCnt )
			break;
	}
	return iRet;
}

void safeQueue::RemoveHead()
{
	CSingleLock lock(&csection, TRUE);
	if( !list.IsEmpty() )
		list.RemoveHead();
}


LPVOID safeQueue::get()
{
	CSingleLock lock(&csection, TRUE);
	LPVOID pItem = list.IsEmpty() ? NULL : list.GetHead();
	return pItem;
}

int	safeQueue::getAll( CPtrList& list2 )
{
	CSingleLock lock(&csection, TRUE);
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{	
		list2.AddTail(list.GetNext(pos));
	}
	return list2.GetCount();
}

int safeQueue::getCount()
{
	CSingleLock lock(&csection, TRUE);
	return list.GetCount();
}