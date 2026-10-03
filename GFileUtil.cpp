// GFileUtil.cpp: implementation of the CGFileUtil class.
//	파일 관련 utility class
//////////////////////////////////////////////////////////////////////

#include "pch.h"
//#include "SgisUtil.h"
#include "GFileUtil.h"
#include "GEnvVar.h"
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CGFileUtil operations

BOOL CGFileUtil::RecursiveMkDir(CString pathName)
{
	CString		strPath, strTail;
	int		nInx, nLength, i;
	DWORD		fileAttr;

	nInx = 2;
	nLength = pathName.GetLength();

	strPath = pathName.Left(nInx);
	strTail = pathName.Right(nLength - nInx);

	if(strPath!="\\\\" && strPath[1]!=':') {
		// full path가 아니므로 디렉토리를 만들 수 없음
		return FALSE;
	}
	if(strPath=="\\\\") {
		nInx = pathName.Right(nLength-nInx).Find('\\') + nInx;
	}
	nInx++;

	for(i=nInx; i<=nLength; i++) {
		if(i==nLength || pathName[i]=='\\') {
			strPath = pathName.Left(i);
			fileAttr = GetFileAttributes(strPath);
			if(fileAttr==-1) {
				// 새로운 디렉토리 생성
				if(_mkdir(strPath)!=0) {
					return FALSE;
				}
			}
			else if(!(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
				// 파일이 이미 존재하면서 디렉토리가 아닌 경우
				return FALSE;
			}
		}
	}

	return TRUE;
}


BOOL CGFileUtil::RecursiveRmDir(CString pathName)
{
	TCHAR		orgPath [MAX_PATH], path [MAX_PATH], imsi[MAX_PATH];
	CString		strFileName;
	CStringArray	pathArray, DelArray;
	WIN32_FIND_DATA findFileData;
	HANDLE		findHandle;

	if (pathName.IsEmpty ())
		return FALSE;

	//목표 디렉토리가 없을 경우 
	if(::GetFileAttributes(pathName) == 0xFFFFFFFF )	
		return FALSE;
	
	pathArray.Add (pathName);
	int ndir = 0 ;
	while (pathArray.GetSize ()) {
		::strcpy (path, pathArray[pathArray.GetSize () - 1]);
		::strcpy (orgPath, path);
		::strcat (path, "\\*.*");

		while (DelArray.GetSize ()) {
			::strcpy (imsi, DelArray[DelArray.GetSize () - 1]);
			if(_rmdir(imsi) != 0)
			{
				TRACE("%s디렉토리 삭제 실패 !!\n", imsi);
				return FALSE;
			}
			DelArray.RemoveAt (DelArray.GetSize () - 1);
		}

		if ((findHandle = ::FindFirstFile (path, &findFileData)) == INVALID_HANDLE_VALUE)
			continue;
		
		do {
			ndir = 0;
			if (findFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
				if (::strcmp (findFileData.cFileName, ".") == 0 ||
					::strcmp (findFileData.cFileName, "..") == 0)
					continue;
				else {
					strFileName.Format ("%s\\%s", orgPath, findFileData.cFileName);
					pathArray.Add (strFileName);
					ndir ++;
				}
			}
			else {
				//파일 삭제 
				strFileName.Format ("%s\\%s", orgPath, findFileData.cFileName);
				::DeleteFile(strFileName);
			}
		} while (::FindNextFile (findHandle, &findFileData));

		if(ndir == 0)	//추가한 디렉토리가 없을때 
		{
			//서브 디렉토리가 없고 파일은 삭제되었으므로 디렉토리를 삭제할수 있음
			pathArray.RemoveAt (pathArray.GetSize () - 1);
			//여기서는 _rmdir()이 제대로 되지 않으므로 삭제할 디렉토리를
			//관리할 변수(DelArray)를 사용하였음
			DelArray.Add(orgPath);
		}
		::FindClose (findHandle);
	}
	pathArray.RemoveAll ();

	while (DelArray.GetSize ()) {
		::strcpy (imsi, DelArray[DelArray.GetSize () - 1]);
		if(_rmdir(imsi) != 0)
		{
			TRACE("%s디렉토리 삭제 실패 !!\n", imsi);
			return FALSE;
		}
		DelArray.RemoveAt (DelArray.GetSize () - 1);
	}
	return TRUE;
}


//---------------------------------------------------------------------------
//	함수명	:	RecursiveFileCopy
//
//	설명	:	filePath 설정된 디렉토리에 있는 서브디렉토리와 파일들을 
//				targetRoot Copy한다.
//				서버의 파일을 로컬로 copy하려면 이 함수를 사용하기 전에 
//				서버 접속을 해야 함.
//
//	반환값	:	성공시 TRUE, 실패시 FALSE
//---------------------------------------------------------------------------
BOOL CGFileUtil::RecursiveFileCopy(const CString filePath, const CString targetRoot)
{
	TCHAR			orgPath [MAX_PATH], pathName [MAX_PATH];
	CString			strFileName, targetPath;
	CStringArray	path;
	WIN32_FIND_DATA findFileData, findFileData2;
	HANDLE			findHandle;

	if (filePath.IsEmpty() || targetRoot.IsEmpty())
		return FALSE;

	//목표 디렉토리가 없을 경우 디렉토리 생성 
	if(::GetFileAttributes(targetRoot) == 0xFFFFFFFF )	{
		RecursiveMkDir(targetRoot) ; 
	}

	//소스경로에 있는 파일들의 리스트와 속성리스트를 만든다.
	CArray <WIN32_FIND_DATA, WIN32_FIND_DATA> findDataList;
	CStringArray	pathDataList;

	path.Add (filePath);
	while (path.GetSize ()) {
		::strcpy (pathName, path [path.GetSize () - 1]);
		::strcpy (orgPath, pathName);
		::strcat (pathName, "\\*.*");
		path.RemoveAt (path.GetSize () - 1);
		if ((findHandle = ::FindFirstFile (pathName, &findFileData)) == INVALID_HANDLE_VALUE)
			continue;
		do {
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (::strcmp (findFileData.cFileName, ".") == 0 ||
					::strcmp (findFileData.cFileName, "..") == 0)
					continue;
				else {
					strFileName.Format ("%s\\%s", orgPath, findFileData.cFileName);
					path.Add (strFileName);
					findDataList.Add (findFileData);
					pathDataList.Add (strFileName);
				}
			}
			else {
				strFileName.Format ("%s\\%s", orgPath, findFileData.cFileName);
				findDataList.Add (findFileData);
				pathDataList.Add (strFileName);
			}
		} while (::FindNextFile (findHandle, &findFileData));

		::FindClose (findHandle);
	}

	path.RemoveAll ();

	for (int i = 0; i < findDataList.GetSize (); ++i) {

		findFileData = findDataList [i];
		strFileName = pathDataList [i];
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			targetPath = targetRoot + "\\" + 
				strFileName.Mid (filePath.GetLength ());
			if (!::CreateDirectory (targetPath, NULL))
				if (::GetLastError () != ERROR_ALREADY_EXISTS)
					continue;
		}
		else {
			targetPath = targetRoot + "\\" + 
				strFileName.Mid (filePath.GetLength ());

			// 목표경로에 파일이 없으면 무조건 복사
			if ((findHandle = ::FindFirstFile (targetPath, &findFileData2)) == INVALID_HANDLE_VALUE) {
				OwnerFileCopy (strFileName, targetPath);
				continue;
			}

			CTime		time1 (findFileData.ftLastWriteTime);
			CTime		time2 (findFileData2.ftLastWriteTime);
			CTimeSpan	time3 = time2 - time1;

			// File Time의 오차가 2 sec.
			if (::abs((long)time3.GetTotalSeconds ()) > 2)
				OwnerFileCopy (strFileName, targetPath);

			::FindClose (findHandle);
		}
	}

	return TRUE;
}


//---------------------------------------------------------------------------
//	함수명	:	OwnerFileCopy
//
//	설명	:	파일을 copy하는데 실패하였을 때 응답이 없도록 하는 것을 방지 하기 
//				위해서 작성하였다고 함.
//
//	반환값	:	성공시 TRUE, 실패시 FALSE
//---------------------------------------------------------------------------
BOOL CGFileUtil::OwnerFileCopy(const CString sourcePath, const CString targetPath)
{
	CFile			file1, file2;
	CFileStatus		rStatus;
	CFileException	e1;
	int				length;
	char			*data;

	TRY
	{
		// 소스 파일 오픈
		if (!file1.Open (sourcePath, CFile::modeRead|CFile::shareDenyNone, &e1)) {
//			SGIS_TRACE2 (__FILE__, __LINE__);
			return FALSE;
		}

		// 목적 파일 오픈
		if (!file2.Open (targetPath, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive, &e1)) {
//			SGIS_TRACE2 (__FILE__, __LINE__);
			file1.Close ();
			return FALSE;
		}

		length = file1.GetLength();
		data = (char *) new char [length+1];
		if (data) {
			file1.Read(data, length);
			file2.Write(data, length);
			file2.Flush ();
			delete [] data;
		}
		file1.GetStatus (rStatus);
		file1.Close ();
		file2.Close ();
		CFile::SetStatus (targetPath, rStatus);
	}
	CATCH_ALL (e)
	{
		file1.Close ();
		file2.Close ();
//SGIS_TRACE2 (__FILE__, __LINE__);
		e -> Delete ();
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;
}

CString CGFileUtil::GetTemporaryDirName(CString pathName)
{
	CString	tempDir;
	char	*pTempDir;
	int	inx;
	UINT	nResult;

	inx = pathName.ReverseFind('\\');
	if(inx<=0) {
		ASSERT(FALSE);
		// 파일 전송을 위한 임시 디렉토리(%s)를
		// 생성할 수 없습니다.
		//CGMessenger::SetMessage(IDS_EGI_0017, CGMessenger::ToDialog);
//		tempDir.Empty();
		tempDir = _T("");
		return tempDir;
	}

	if(!RecursiveMkDir(pathName)) {
		// 파일 전송을 위한 임시 디렉토리(%s)를
		// 생성할 수 없습니다.
		//CGMessenger::SetMessage(IDS_EGI_0017, CGMessenger::ToDialog);
//		tempDir.Empty();
		tempDir = _T("");
		return tempDir;
	}

	pTempDir = tempDir.GetBuffer(pathName.GetLength() * 2 + 40);
	nResult = GetTempFileName(pathName, "gtr", 0, pTempDir);
	tempDir.ReleaseBuffer();

	if(nResult==0) {
		// 파일 전송을 위한 임시 디렉토리(%s)를
		// 생성할 수 없습니다.
		//CGMessenger::SetMessage(IDS_EGI_0017, CGMessenger::ToDialog);
//		tempDir.Empty();
		tempDir = _T("");
		return tempDir;
	}

	// 임시 파일이 생성되어 있으므로 파일을 삭제하고
	// 디렉토리를 만든다.
	CFile::Remove(tempDir);	// 로컬 디렉토리

	// 디렉토리명에서 '.'을 '_'로 바꾼다.
	inx = tempDir.ReverseFind('.');
	if(inx>=0) {
		tempDir.SetAt(inx, '_');
	}
	tempDir.MakeLower();

	// 임시 디렉토리 생성
	if(_mkdir(tempDir)!=0) {
		// 파일 전송을 위한 임시 디렉토리(%s)를
		// 생성할 수 없습니다.
		//CGMessenger::SetMessage(IDS_EGI_0017, CGMessenger::ToDialog);
//		tempDir.Empty();
		tempDir = _T("");
	}

	return tempDir;
}


////////////////////////////////////////////////////////////////////////////////
// 파일 입출력

void CGFileUtil::ReadString(CArchive &ar, CString &str)
{
	unsigned long	length;
	char		*lpStr;

	ar.Read(&length, sizeof(unsigned long));
	ASSERT(length >= 0);

	if(length == 0) {
		//str.Empty();
		str = _T("");
		return;
	}

	lpStr = str.GetBuffer(length * 2 + 2);
	ar.Read(lpStr, length);
	lpStr[length] = '\0';
	str.ReleaseBuffer();
	return;
}


void CGFileUtil::ReadString(CFile &file, CString &str)
{
	unsigned long	length;
	char			*lpStr;

	file.Read(&length, sizeof(unsigned long));
	ASSERT(length >= 0);

	if(length == 0) {
		str.ReleaseBuffer();
//		str.Empty();
		str = _T("");
		return;
	}

	lpStr = str.GetBuffer(length * 2 + 2);
	file.Read(lpStr, length);
	lpStr[length] = '\0';
	str.ReleaseBuffer();
	return;
}


void CGFileUtil::WriteString(CFile &file, CString &str)
{
	unsigned long	length;
	char		*lpStr;

	length = str.GetLength();
	file.Write(&length, sizeof(unsigned long));

	if(length > 0) {
		lpStr = str.GetBuffer(length * 2 + 2);
		file.Write(lpStr, length);
		str.ReleaseBuffer();
	}
	return;
}


void CGFileUtil::WriteString(CArchive &ar, CString &str)
{
	unsigned long	length;
	char		*lpStr;

	length = str.GetLength();
	ar.Write(&length, sizeof(unsigned long));
	if(length > 0) {
		lpStr = str.GetBuffer(length * 2 + 2);
		ar.Write(lpStr, length);
		str.ReleaseBuffer();
	}
	return;
}


void CGFileUtil::ReadNString(CArchive &ar, CString &str, int nLength)
{
	ASSERT(nLength > 0);

	char	*lpStr;
	DWORD	dwLength;

	dwLength = nLength * 2 + 2;
	lpStr = str.GetBuffer(dwLength);
	ZeroMemory(lpStr, dwLength);
	ar.Read(lpStr, nLength);
	lpStr[nLength] = '\0';
	str.ReleaseBuffer();
}


void CGFileUtil::WriteNString(CArchive &ar, CString &str, int nLength)
{
	ASSERT(nLength>0);

	char	*lpStr, *tmpStr;
	DWORD	dwLength;

	dwLength = nLength * 2 + 2;
	lpStr = str.GetBuffer(dwLength);

	tmpStr = &(lpStr[nLength]);
	ZeroMemory(tmpStr, dwLength - nLength);
	ar.Write(lpStr, nLength);
	str.ReleaseBuffer();
}

CString CGFileUtil::GetTransDstDir()
{
	CString		strRet;

#if !defined(_TMPAPI)

	CGEnvVar	envVar;

	envVar.SetEnvVar("TMSTEMP");
	strRet = envVar.GetValue();

#else

	DWORD	bufLen = 255;
	bufLen = ::GetEnvironmentVariable("TMSTEMP", strRet.GetBuffer(bufLen + 1), bufLen);
	strRet.ReleaseBuffer();

#endif // !defined(_TMPAPI)

	if(strRet.IsEmpty()) {
#if !defined(_TMPAPI)
		// 환경변수 TMSTEMP'가 지정되지 않았으므로 문서 송수신을 할 수 없습니다.
		//CGMessenger::SetMessage(IDS_EGP_0035, CGMessenger::ToDialog);
#else
		TRACE0("환경변수 'TMSTEMP'가 지정되지 않았으므로 문서 송수신을 할 수 없습니다.");
#endif // !defined(_TMPAPI)
//		strRet.Empty();
		strRet = _T("");
		return strRet;
	}

	if(strRet[strRet.GetLength()-1]!='\\') {
		strRet += "\\sgis";
	}
	else {
		strRet += "sgis";
	}


	// 임시 디렉토리 생성

#if !defined(_TMPAPI)

	strRet = CGFileUtil::GetTemporaryDirName(strRet);

#else

	if(_mkdir(strRet)!=0 && errno!=EEXIST) {
		CString	strMsg;
		strMsg.Format("파일 전송을 위한 임시 디렉토리(%s)를 생성할 수 없습니다.", strRet);
		TRACE0(strMsg);
//		strRet.Empty();
		strRet = _T("");
		return strRet;
	}

	static int	sn = 1;
	CString		tmpDir;

	tmpDir.Format("%s\\%d", strRet, sn);
	sn++;

	if(_mkdir(tmpDir)!=0 && errno!=EEXIST) {
		CString	strMsg;
		strMsg.Format("파일 전송을 위한 임시 디렉토리(%s)를 생성할 수 없습니다.", tmpDir);
		TRACE0(strMsg);
//		tmpDir.Empty();
		tmpDir = _T("");
		return strRet;
	}

	strRet = tmpDir;

#endif // !defined(_TMPAPI)

//	SGIS_TRACE(__FILE__, __LINE__, "임시 디렉토리 : '%s'", strRet);

	return strRet;
}

BOOL CGFileUtil::RemoveDir(CString dir)
{
	CFileFind	ff;
	CString		wild, strPath;
	BOOL		bStat;
	int			nGoOn;
	CString		pathCur, pathParent;

	if(dir.Right(1)==CString("\\")) {
		dir = dir.Left(dir.GetLength()-1);
	}

	wild.Format("%s\\*", dir);
	pathCur.Format("%s\\.", dir);
	pathParent.Format("%s\\..", dir);

	// 디렉토리 내의 파일 및 디렉토리 삭제
	if(ff.FindFile(wild)) {
		do {
			nGoOn = ff.FindNextFile();
			strPath = ff.GetFilePath();

			if(strPath==pathCur || strPath==pathParent) {
				// ., ..은 디렉토리이므로 skip
				continue;
			}

			if(GetFileAttributes(strPath)==FILE_ATTRIBUTE_DIRECTORY) {
				bStat = RemoveDir(strPath);
				if(bStat==FALSE) {
					return FALSE;
				}
			}
			else {
				if(!DeleteFile(strPath)) {
					// ~ 파일을 삭제할 수 없습니다.
#if !defined(_TMPAPI)
					//CGMessenger::SetMessage(IDS_WGI_0061, CGMessenger::ToDialog);
#else
					TRACE0("파일을 삭제할 수 없습니다.");
#endif // !defined(_TMPAPI)
					
				}
			}
		} while(nGoOn);
	}
	ff.Close();

	dir.MakeLower();

	CString		parentDir;
	BOOL		bDirStat;

	parentDir.Format("%s\\..", dir);
//	_chdir(parentDir);
	bDirStat = SetCurrentDirectory(parentDir);

	if(RemoveDirectory(dir)!=0)
	{
//		SGIS_TRACE(__FILE__, __LINE__, "임시 디렉토리 삭제 성공 : '%s'", dir);
		return TRUE;
	}
	else {
		// ~ 디렉토리를 삭제할 수 없습니다.
//		SGIS_TRACE(__FILE__, __LINE__, "임시 디렉토리 삭제 실패 : '%s'", dir);

		DWORD dwErr = ::GetLastError();
		LPVOID lpMsgBuf;

		FormatMessage( 
		    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		    NULL,
		    dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);

		// Display the string.
		::MessageBox( NULL, (char*)lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

		// Free the buffer.
		LocalFree( lpMsgBuf );
		
		return FALSE;
	}

	//
	// DOS 명령을 사용하여 확실히 지우도록 한다.
//	CString		cmdStr;

//	cmdStr.Format("rmdir /s /q \"%s\"", dir);
//	SGIS_TRACE(__FILE__, __LINE__, "임시 디렉토리 삭제 : '%s'", cmdStr);
//	WinExec(cmdStr, SW_HIDE);

	return TRUE;
}
