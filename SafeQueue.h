// safeQueue.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// safeQueue

#ifndef __safeQueue_H__
#define __safeQueue_H__

/**
* data list manager(queue type)
* @author 박종원
* @version 1.0
* @since 2005.10.
**/
struct safeQueue
{
	safeQueue();
	~safeQueue();
protected:
	CCriticalSection csection;
	CPtrList list;

public:
	/** 데이터 추가 */
	BOOL	push( LPVOID );
	/** 데이터 얻기 및 삭제 */
	LPVOID	pop();
	/** 데이터 모두 얻기 및 삭제 */
	int		popAll( CPtrList& );
	/** 데이터 얻기 */
	LPVOID	get();
	/** 모든 데이터 얻기 */
	int		getAll( CPtrList& );
	/** 하나의 데이터 삭제 */
	int		Remove( int iCnt );
	/** 하나의 데이터(앞부분) 삭제 */
	void	RemoveHead();
	/** 데이터 레코드 개수 얻기 */
	int		getCount();

};

#endif