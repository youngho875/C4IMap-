// ¿€º∫: subifox,06.06.19

#ifndef _FILE_PATH_H_
#define _FILE_PATH_H_

#pragma once

class CFilePath  
{
public:
	enum { FILEPATH_MAX_LENGTH =	256 };

private:
	static char m_szRoot[FILEPATH_MAX_LENGTH];

public:
	static void		ResetRoot(void);
	static char*	GetRoot(void)			{ return m_szRoot; }

private:
	char m_szWork[FILEPATH_MAX_LENGTH];
	char m_szFileName[FILEPATH_MAX_LENGTH];
	char m_szBuffer[FILEPATH_MAX_LENGTH];	

public:
	CFilePath(void);
	virtual ~CFilePath(void);

	char* SetWork(LPCSTR lpszPath);
	char* GetWork(void);
	char* AttachWork(LPCTSTR lpszPath);		

	char*	SetFileName(LPCTSTR lpszFileName);
	char* SetFileName(LPCTSTR lpszName, LPCSTR lpszExt);
	char* GetFileName(void);
	char* AttachFileName(LPCTSTR lpszFileName);			
	char* AttachFileName(LPCTSTR lpszName, LPCSTR lpszExt);

	BOOL operator==(CFilePath& path);	
	BOOL operator!=(CFilePath& path);
	void operator=(CFilePath& path);
};

#endif 
