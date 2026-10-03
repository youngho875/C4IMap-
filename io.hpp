// io.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIO Declaration   -- note CIO class is a pure abstract class


/*
BOOL Create( UINT nSocketPort = 0, int nSocketType = SOCK_STREAM, long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE, LPCTSTR lpszSocketAddress = NULL );
virtual BOOL Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL );
virtual BOOL Open( UINT nOpenType = AFX_DB_USE_DEFAULT_TYPE, LPCTSTR lpszSQL = NULL, DWORD dwOptions = none );

virtual UINT Read( void* lpBuf, UINT nCount );
virtual int Receive( void* lpBuf, int nBufLen, int nFlags = 0 );
virtual int Send( const void* lpBuf, int nBufLen, int nFlags = 0 );
*/
#ifndef __IO____H__
#define __IO____H__

#include <afxtempl.h>
#include "../inc/SafeQueue.h"

/** caller, sender, data, size */
typedef void (*CallBackFunc)(LPVOID, LPVOID, LPVOID, int);

/**
* 표준 파일 io 관리 class
* @author 박종원
* @version 1.0
* @since 2005.11.
**/
class CIO
{
public:
	CIO();
	~CIO();

// attributes
protected:
	enum		IOType{
				IO_NONE	= 0,
				IO_FILE,
				IO_DB,
				IO_SOCKET,
				IO_PIPE,
				IO_RPC} m_eIOType;
	enum		Status{
				STAT_BEFORE_INIT = 0,
				STAT_INIT,
				STAT_OPEN,
				STAT_CLOSE} m_eStatus;

	CallBackFunc CallBack;
	LPVOID		m_outer;

	BOOL		m_bOwnThread;
	BOOL		m_bInit;
	safeQueue	m_inputQ;
	safeQueue	m_outputQ;
public:
	CWinThread* m_pThread;
	/** 부모가 종료되어도 좋다는 시그널을 주는 이벤트 */
	CEvent		m_eventAllowParentDie;
	/** 쓰레드를 죽이는 이벤트 */
	CEvent		m_eventKillThread;

// operations
	BOOL			IsOwnThread(){ return m_bOwnThread; }
	UINT			GetType(){ return (UINT)m_eIOType; }
	UINT			GetStatus(){ return (UINT)m_eStatus; }
	virtual BOOL	Init( BOOL bOwnThread=FALSE ) = 0;
	virtual BOOL	Open(   /** file(pathname),db(dsn),socket(ip).. */
							LPCSTR lpOpenInfo,
							/** file(sharemode),db(settype),socket(event) */
							DWORD  dwMode,
							/** db(option),socket(type) */
							DWORD  dwOption=0,
							/** db only */
							LPCSTR lpszSQL=NULL,
							/** socket only */
							UINT   nSocketPort=0
							) = 0;
	virtual BOOL	Close() = 0;
	/** called by thread */
	virtual BOOL	DoSomething() = 0;
	/** call back */
	virtual BOOL	DoCallBack( LPVOID, int ) = 0;
	/** assign callback */
	BOOL			AssignCallBack( LPVOID outer, CallBackFunc cbFunc );

	// q manipulation
	BOOL			push( LPVOID, BOOL bInputQ );
	LPVOID			pop( BOOL bInputQ );
	int				popAll( CPtrList&, BOOL bInputQ );
	LPVOID			get( BOOL bInputQ );
	int				getAll( CPtrList&, BOOL bInputQ );
	int				Remove( int iCnt, BOOL bInputQ );
	void			RemoveHead( BOOL bInputQ );
	int				getCount( BOOL bInputQ );	

// implementations
protected:

};


/**
* file io 관리 class
* @author 박종원
* @version 1.0
* @since 2005.11.
**/
class CFileIO : public CIO
{
public:
	CFileIO();
	~CFileIO();

// attributes

public:
	CStdioFile		m_file;

// operations
	virtual BOOL	Init( BOOL bOwnThread=FALSE );
	virtual BOOL	Open(   /** file(pathname) */
							LPCSTR lpOpenInfo,
							/** file(sharemode) */
							DWORD  dwMode,
							/** none */
							DWORD  dwOption=0,
							/** db only */
							LPCSTR lpszSQL=NULL,
							/** socket only */
							UINT   nSocketPort=0
							);
	virtual BOOL	Close();
	/** called by thread */
	virtual BOOL	DoSomething();
	/** call back */
	virtual BOOL	DoCallBack( LPVOID buf, int iSize );

	int				Read( LPVOID lpBuf, int nBufLen );
	int				Write( LPVOID lpBuf, int nBufLen );
	BOOL			ReadString(CString& rString);
	void			WriteString( LPCTSTR lpsz );
	DWORD			SeekToEnd();
// implementations

};

/**
* CStdioFile 클래스 사용시 파일카피 문제 해결을 위해 CStdioFile 대신 ANSI C 라이브러리를 사용함.
* @author 박종원
* @version 1.0
* @since 2005.11.
**/
class CAnsiFileIO : public CIO
{
public:
	CAnsiFileIO();
	~CAnsiFileIO();

// attributes

public:
	FILE		*m_pFile;

// operations
	virtual BOOL	Init( BOOL bOwnThread=FALSE );
	virtual BOOL	Open( LPCSTR lpFileName, LPCTSTR lpMode);
	virtual BOOL	Close();
	/** called by thread */
	virtual BOOL	DoSomething();
	/** call back */
	virtual BOOL	DoCallBack( LPVOID buf, int iSize );
	void			WriteString( LPCTSTR lpsz );
	void			WriteLog(  LPCSTR lpFileName, LPCTSTR lpsz );
};

// prior declaration
class CSQLBindP;
class CStdRecord;




#endif