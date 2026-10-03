#include "pch.h"
//???#include "GRemoteFile.h"
#include "GStatusFlag.h"
//???#include "SgisUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGStatusFlag::CGStatusFlag()
{
	m_nStatus = 0;
	m_pStatus = NULL;
}

CGStatusFlag::CGStatusFlag(CGStatusFlag &StatusFlag)
{
	*this = StatusFlag;
}

CGStatusFlag::~CGStatusFlag()
{
	if(m_pStatus) {
		delete[] m_pStatus;
		m_pStatus = NULL;
	}
}

BOOL CGStatusFlag::GetAt(int nInx) 
{
	if(m_pStatus)	return m_pStatus[nInx]; 
	else			return FALSE;
}

BOOL CGStatusFlag::operator[] (int nInx)
{
//	ASSERT(m_pStatus);
//	ASSERT(nInx>=0 && nInx<m_nStatus);
	if (m_pStatus && (nInx>=0 && nInx<m_nStatus))
		return m_pStatus[nInx];
	else
		return FALSE;
}

void CGStatusFlag::SetAt(int nInx, BOOL flag)
{
//	ASSERT(m_pStatus);
//	ASSERT(nInx>=0 && nInx<m_nStatus);
	if (m_pStatus && (nInx>=0 && nInx<m_nStatus))
		m_pStatus[nInx] = flag;
}

const CGStatusFlag &CGStatusFlag::operator=(const CGStatusFlag &right)
{
	if(m_pStatus!=right.m_pStatus) {
		if(m_pStatus) {
			if(m_nStatus!=right.m_nStatus) {
				// reallocate memory for colors
				delete[] m_pStatus;
				m_pStatus = new BOOL[right.m_nStatus];
			}
		}
		else {
			m_pStatus = new BOOL[right.m_nStatus];
		}
		m_nStatus = right.m_nStatus;
		memcpy(m_pStatus, right.m_pStatus, m_nStatus * sizeof(BOOL));
	}

	return *this;
}

void CGStatusFlag::InitInstance()
{
	m_nStatus = 0;
	if(m_pStatus) delete[] m_pStatus;
	m_pStatus = NULL;
}

void CGStatusFlag::Serialize(CArchive &ar)
{
	if(ar.IsStoring()) {
		ar.Write(&m_nStatus, sizeof(long));
		if(m_pStatus) {
			ar.Write(m_pStatus, m_nStatus * sizeof(BOOL));
		}
	}
	else {
		long	nStatus;
		BOOL	*pStatus = NULL;

		ar.Read(&nStatus, sizeof(long));
		if(nStatus>0) {
			pStatus = new BOOL[nStatus];
			ar.Read(pStatus, nStatus * sizeof(BOOL));
		}

		if(m_pStatus) delete[] m_pStatus;

		m_nStatus = nStatus;
		m_pStatus = pStatus;
	}
}

BOOL CGStatusFlag::Load(CString fileName)
{
//???	CGRemoteFile	file;
	return TRUE;
	CFile file;
	CFileException	e;

	if(!file.Open(fileName, CFile::modeRead | CFile::shareDenyNone
				| CFile::typeBinary, &e)) {
		// ~ 상태정보 파일을 찾을 수 없습니다.
			//CGMessenger::SetMessage(IDS_WGI_0021, CGMessenger::ToDialog, fileName     );
		return FALSE;
	}

	BOOL	*pStatus;
	UINT	nRead, nSize;

	file.Read(&m_nStatus, sizeof(long));
	if(m_nStatus<0) {
		// ~는 잘못된 상태정보 파일입니다.
			//CGMessenger::SetMessage(IDS_WGI_0022, CGMessenger::ToDialog, fileName     );
		file.Close();
		return FALSE;
	}

	if(m_pStatus) delete [] m_pStatus;

	pStatus = new BOOL[m_nStatus];
	nSize = m_nStatus * sizeof(BOOL);
	nRead = file.Read(pStatus, nSize);

	if(nRead!=nSize) {
		delete[] pStatus;
		// ~는 잘못된 상태정보 파일입니다.
			//CGMessenger::SetMessage(IDS_WGI_0022, CGMessenger::ToDialog, fileName     );
		file.Close();
		return FALSE;
	}

	m_pStatus = pStatus;

	return TRUE;
}