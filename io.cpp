#include "pch.h"
#include "io.hpp"

UINT IOThreadFunc( LPVOID lParam );


////////////////////////////////////////////////////////////////////////////////////
// CIO Class Implementations

CIO::CIO()
{
	m_eIOType = IO_NONE;
	m_eStatus = STAT_BEFORE_INIT;

	CallBack = NULL;
	m_outer = NULL;

	m_bInit = FALSE;
	m_bOwnThread = TRUE;
	m_pThread = NULL;
}

CIO::~CIO()
{
}

BOOL CIO::AssignCallBack( LPVOID outer, CallBackFunc cbFunc )
{
	m_outer = outer;
	CallBack = cbFunc;
	return TRUE;
}

// q manipulation
BOOL CIO::push( LPVOID p, BOOL bInputQ )
{
	/*
	BOOL bRet;
	if( bInputQ )
		bRet = m_inputQ.push(p);
	else
		bRet = m_outputQ.push(p);

	return bRet;
	*/
	return bInputQ? m_inputQ.push(p): m_outputQ.push(p);
}


LPVOID CIO::pop( BOOL bInputQ )
{
	return bInputQ? m_inputQ.pop(): m_outputQ.pop();
}


int CIO::popAll( CPtrList& l, BOOL bInputQ )
{
	return bInputQ? m_inputQ.popAll(l): m_outputQ.popAll(l);
}


LPVOID CIO::get( BOOL bInputQ )
{
	return bInputQ? m_inputQ.get(): m_outputQ.get();
}

int CIO::getAll( CPtrList& l, BOOL bInputQ )
{
	return bInputQ? m_inputQ.getAll(l): m_outputQ.getAll(l);
}


int CIO::Remove( int iCnt, BOOL bInputQ )
{
	return bInputQ? m_inputQ.Remove(iCnt): m_outputQ.Remove(iCnt);
}

void CIO::RemoveHead( BOOL bInputQ )
{
	bInputQ? m_inputQ.RemoveHead(): m_outputQ.RemoveHead();
}

int CIO::getCount( BOOL bInputQ )
{
	return bInputQ? m_inputQ.getCount(): m_outputQ.getCount();	
}


////////////////////////////////////////////////////////////////////////////////////
// CFileIO Class Implementations

CFileIO::CFileIO()
{
}

CFileIO::~CFileIO()
{

}


BOOL CFileIO::Init( BOOL bOwnThread )
{
	if( bOwnThread )
	{
		AfxMessageBox("독립쓰레드를 사용하실 때는 상속 클래스를 이용하세요\n");
		return FALSE;
	}

	m_bOwnThread = bOwnThread;

	m_bInit = TRUE;
	return TRUE;
// OwnThread인 경우 Thread를 초기화한다.
/*
	if( bOwnThread && 
		m_pThread == NULL )
		m_pThread = AfxBeginThread( IOThreadFunc, this, THREAD_PRIORITY_BELOW_NORMAL );
*/
}


BOOL CFileIO::Open(   LPCSTR lpOpenInfo,	//file(pathname)
							DWORD  dwMode,		//file(sharemode)
							DWORD  dwOption,	//file(openflag)
							LPCSTR lpszSQL,		//db only
							UINT   nSocketPort  //socket only
							)
{
// Init 체크
	if( !m_bInit )
	{
		TRACE("초기화 전입니다. Init 함수를 실행해야 합니다.\n");
		return FALSE;
	}
// 파일을 오픈한다.
	return m_file.Open( lpOpenInfo, (UINT)dwMode );
}


int CFileIO::Read( LPVOID lpBuf, int nBufLen )
{
	return m_file.Read( lpBuf, nBufLen );
}


int CFileIO::Write( LPVOID lpBuf, int nBufLen )
{
	m_file.Write( lpBuf, nBufLen );
	return 0;
}

BOOL CFileIO::ReadString(CString& rString)
{
	return m_file.ReadString( rString );
}

void CFileIO::WriteString( LPCTSTR lpsz )
{
	m_file.WriteString( lpsz );
}

DWORD CFileIO::SeekToEnd()
{
	return m_file.SeekToEnd();
}


BOOL CFileIO::Close()
{
	if( m_file.m_hFile == CFile::hFileNull )
		return TRUE;

	m_file.Close();
	return TRUE;
}


BOOL CFileIO::DoSomething()
{
	return TRUE;
}


BOOL CFileIO::DoCallBack( LPVOID buf, int iSize )
{
	CallBack( m_outer, this, buf, iSize );
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
// CAnsiFileIO Class Implementations
// CStdioFile 클래스 사용시 파일카피 문제 해결을 위해
// CStdioFile 대신 ANSI C 라이브러리를 사용하는 클래스 추가.
// 2002/11/15 By 박정준

CAnsiFileIO::CAnsiFileIO()
{
	m_pFile = NULL;
}

CAnsiFileIO::~CAnsiFileIO()
{
	m_pFile = NULL;
}


BOOL CAnsiFileIO::Init( BOOL bOwnThread )
{
	if( bOwnThread )
	{
		AfxMessageBox("독립쓰레드를 사용하실 때는 상속 클래스를 이용하세요\n");
		return FALSE;
	}

	m_bOwnThread = bOwnThread;

	m_bInit = TRUE;
	return TRUE;
// OwnThread인 경우 Thread를 초기화한다.
/*
	if( bOwnThread && 
		m_pThread == NULL )
		m_pThread = AfxBeginThread( IOThreadFunc, this, THREAD_PRIORITY_BELOW_NORMAL );
*/
}


BOOL CAnsiFileIO::Open( LPCSTR lpFileName, LPCTSTR lpMode)
{
// Init 체크
	if( !m_bInit )
	{
		TRACE("초기화 전입니다. Init 함수를 실행해야 합니다.\n");
		return FALSE;
	}
	
	Close();

	m_pFile = fopen((LPCTSTR)lpFileName, lpMode);
	if(!m_pFile) return FALSE;

	return TRUE;
}

void CAnsiFileIO::WriteString( LPCTSTR lpsz )
{
	if( m_pFile ) fprintf(m_pFile,"%s", lpsz);
}

// 로그남기는 함수 추가.
void CAnsiFileIO::WriteLog( LPCSTR lpFileName, LPCTSTR lpsz )
{
	if(Open(lpFileName, "a"))
	{
		WriteString(lpsz);
		Close();
	}
}

BOOL CAnsiFileIO::Close()
{
	if( m_pFile ) 
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	return TRUE;
}


BOOL CAnsiFileIO::DoSomething()
{
	return TRUE;
}


BOOL CAnsiFileIO::DoCallBack( LPVOID buf, int iSize )
{
	CallBack( m_outer, this, buf, iSize );
	return TRUE;
}




//////////////////////////////////////////////////////////////////////////
// Thread Handler 

#define		THREAD_WAIT_TIME	10

UINT IOThreadFunc( LPVOID lParam )
{
	TRACE("info_thread : IOThreadFunc \n");
	CIO* pParent = (CIO*)lParam;

	CTime time = CTime::GetCurrentTime();
	//TRACE( "INFO: (%02d:%02d:%02d) Client( %s ) thread created\n",
	//		pParent->GetPeer(), time.GetHour(), time.GetMinute(), time.GetSecond() );
	
	CSingleLock sl( &pParent->m_eventKillThread );
	while ( TRUE )
	{
		if ( sl.Lock( THREAD_WAIT_TIME ) )    // Locking이 성공하면 엄마가 죽으라고 시그널을 준 것임.
			break;

		if( !pParent->DoSomething() )
			break;
	}
	sl.Unlock();

	pParent->m_eventAllowParentDie.SetEvent();      // 엄마도 죽을 수 있도록 신호를 줌.
	//TRACE( "INFO: Client( %s ) thread Killed.\n", pParent->GetPeer() );
	return UINT(0);
}




