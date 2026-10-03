#include "pch.h"
#include "FilePath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char CFilePath::m_szRoot[FILEPATH_MAX_LENGTH] = {0, };

void CFilePath::ResetRoot(void)
{
	DWORD dwLen = GetCurrentDirectory(FILEPATH_MAX_LENGTH, m_szRoot);
}

CFilePath::CFilePath(void)
{
	ZeroMemory(m_szWork,			sizeof(char) * FILEPATH_MAX_LENGTH);	
	ZeroMemory(m_szFileName,	sizeof(char) * FILEPATH_MAX_LENGTH);	
	ZeroMemory(m_szBuffer,		sizeof(char) * FILEPATH_MAX_LENGTH);	
}

CFilePath::~CFilePath(void)
{

}

char* CFilePath::SetWork(LPCSTR lpszPath)
{
	size_t nLen = strlen(lpszPath);
	ASSERT(nLen < 256);

	ZeroMemory(m_szWork, sizeof(char) * FILEPATH_MAX_LENGTH);	
	strncpy(m_szWork, lpszPath, nLen);

	return GetWork();
}

char* CFilePath::GetWork(void)
{
	sprintf(m_szBuffer, "%s%s", m_szRoot, m_szWork);
	return m_szBuffer;
}

char* CFilePath::AttachWork(LPCTSTR lpszPath)
{
	sprintf(m_szBuffer, "%s%s", m_szRoot, lpszPath);
	return m_szBuffer;
}

char*	CFilePath::SetFileName(LPCTSTR lpszFileName)
{
	size_t nLen = strlen(lpszFileName);
	ASSERT(nLen < 256);

	ZeroMemory(m_szFileName, sizeof(char) * FILEPATH_MAX_LENGTH);	
	strncpy(m_szFileName, lpszFileName, nLen);

	return GetFileName();
}

char* CFilePath::SetFileName(LPCTSTR lpszName, LPCTSTR lpszExt)
{
	size_t nLen = strlen(lpszName) + strlen(lpszExt);
	ASSERT(nLen < 256);

	ZeroMemory(m_szFileName, sizeof(char) * FILEPATH_MAX_LENGTH);	
	sprintf(m_szFileName, "%s.%s", lpszName, lpszExt);	

	return GetFileName();
}

char* CFilePath::GetFileName(void)
{
	sprintf(m_szBuffer, "%s%s%s", m_szRoot, m_szWork, m_szFileName);
	return m_szBuffer;
}

char* CFilePath::AttachFileName(LPCTSTR lpszFileName) 
{
	ASSERT(lpszFileName);
	sprintf(m_szBuffer, "%s%s%s", m_szRoot, m_szWork, lpszFileName);
	return m_szBuffer;
}

char* CFilePath::AttachFileName(LPCTSTR lpszName, LPCTSTR lpszExt)
{
	size_t nLen = strlen(lpszName) + strlen(lpszExt);
	ASSERT(nLen < 256);

	ZeroMemory(m_szFileName, sizeof(char) * FILEPATH_MAX_LENGTH);	
	sprintf(m_szBuffer, "%s%s%s.%s", m_szRoot, m_szWork, lpszName, lpszExt);	

	return m_szBuffer;
}

BOOL CFilePath::operator==(CFilePath& rhs)
{	
	return (strcmp(rhs.GetFileName(), GetFileName()) == 0)? TRUE:FALSE;
}

BOOL CFilePath::operator!=(CFilePath& rhs)
{
	return (strcmp(rhs.GetFileName(), GetFileName()) != 0)? TRUE:FALSE;
}

void CFilePath::operator=(CFilePath& rhs)
{
	strcpy(m_szWork, rhs.m_szWork);
	strcpy(m_szFileName, rhs.m_szFileName);
}
