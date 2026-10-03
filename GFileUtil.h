// GFileUtil.h: interface for the CGFileUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GFILEUTIL_H__61120C8A_34B3_45C2_B28C_63D00064C096__INCLUDED_)
#define AFX_GFILEUTIL_H__61120C8A_34B3_45C2_B28C_63D00064C096__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
* 파일 입출력관리 class
* @author 유영호
* @version 1.0
* @since 204.09.
**/
class CGFileUtil  
{
public:
	//
	/** 디렉토리 관리 */
	static BOOL	RecursiveMkDir(CString pathName);
	static BOOL	RecursiveRmDir(CString pathName);
	static BOOL	RecursiveFileCopy(const CString filePath, const CString targetRoot);
	static CString	GetTemporaryDirName(CString pathName);
	static CString	GetTransDstDir();
	static BOOL		RemoveDir(CString dir);

	//
	/** 파일 입출력 */
	static void	ReadString(CArchive &ar, CString &str);
	static void	ReadString(CFile &file, CString &str);
	static void	WriteString(CArchive &ar, CString &str);
	static void	WriteString(CFile &file, CString &str);
	static void	ReadNString(CArchive &ar, CString &str, int nLength);
	static void	WriteNString(CArchive &ar, CString &str, int nLength);

protected:
	static BOOL	OwnerFileCopy(const CString sourcePath, const CString targetPath);
};

#endif // !defined(AFX_GFILEUTIL_H__61120C8A_34B3_45C2_B28C_63D00064C096__INCLUDED_)
