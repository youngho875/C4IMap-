#include "pch.h"
#include "GColorData.h"

CGColorData::CGColorData()
{
	m_nColors = 0;
	m_pColors = NULL;
}

CGColorData::CGColorData(CGColorData &colorData)
{
	*this = colorData;
}

CGColorData::~CGColorData()
{
	if(m_pColors) {
		delete[] m_pColors;
		m_pColors = NULL;
	}
}

COLORREF CGColorData::GetAt(int nInx)
{
	if(m_pColors) {
		return m_pColors[nInx];
	}
	else {
		return (unsigned long) -1L;
	}
}

COLORREF CGColorData::operator[] (int nInx)
{
//	ASSERT(m_pColors);
//	ASSERT(nInx>=0 && nInx<m_nColors);
	if (m_pColors && (nInx>=0 && nInx<m_nColors)) {
		return m_pColors[nInx];
	}
	else {
		return (unsigned long) -1L;
	}
}

void CGColorData::SetAt(int nInx, COLORREF newColor)
{
//	ASSERT(m_pColors);
//	ASSERT(nInx<m_nColors);
	if (m_pColors && (nInx>=0 && nInx<m_nColors))
		m_pColors[nInx] = newColor;
}

const CGColorData &CGColorData::operator=(const CGColorData &right)
{
	if(m_pColors!=right.m_pColors) {
		if(m_pColors) {
			if(m_nColors!=right.m_nColors) {
				// reallocate memory for colors
				delete[] m_pColors;
				m_pColors = new COLORREF[right.m_nColors];
			}
		}
		else {
			m_pColors = new COLORREF[right.m_nColors];
		}
		m_nColors = right.m_nColors;
		memcpy(m_pColors, right.m_pColors, m_nColors * sizeof(COLORREF));
	}

	return *this;
}

void CGColorData::InitInstance()
{
	m_nColors = 0;
	if(m_pColors) delete[] m_pColors;
	m_pColors = NULL;
}

void CGColorData::Serialize(CArchive &ar)
{
	if(ar.IsStoring()) {
		ar.Write(&m_nColors, sizeof(long));
		if(m_pColors) {
			ar.Write(m_pColors, m_nColors * sizeof(COLORREF));
		}
	}
	else {
		long		nColors;
		COLORREF	*pColors = NULL;

		ar.Read(&nColors, sizeof(long));
		if(nColors>0) {
			pColors = new COLORREF[nColors];
			ar.Read(pColors, nColors * sizeof(COLORREF));
		}

		if(m_pColors) delete[] m_pColors;

		m_nColors = nColors;
		m_pColors = pColors;
	}
}

BOOL CGColorData::Load(CString fileName)
{
	return TRUE;
//	CGRemoteFile	file;
	CFile	file;
	CFileException		e;

	if(!file.Open(fileName, CFile::modeRead | CFile::shareDenyNone
				| CFile::typeBinary, &e)) {
		// 색상정보 파일을 찾을 수 없습니다.
		//CGMessenger::SetMessage(IDS_WGI_0018, CGMessenger::ToDialog, fileName);
		return FALSE;
	}

	COLORREF	*pColors;
	long		nColors;
	UINT		nRead, nSize;

	file.Read(&nColors, sizeof(long));
	if(nColors<0) {
		// ~는 잘못된 색상정보 파일입니다.
		//CGMessenger::SetMessage(IDS_WGI_0019, CGMessenger::ToDialog, fileName);
		file.Close();
		return FALSE;
	}

	if(m_pColors)	delete []m_pColors;

	pColors = new COLORREF[nColors];
	nSize = nColors * sizeof(COLORREF);

	nRead = file.Read(pColors, nSize);

	if(nRead!=nSize) {
		delete[] pColors;
		// ~는 잘못된 색상정보 파일입니다.
		//CGMessenger::SetMessage(IDS_WGI_0019, CGMessenger::ToDialog, fileName);
		file.Close();
		return FALSE;
	}

	m_nColors = nColors;
	m_pColors = pColors;

	file.Close();

	return TRUE;
}
