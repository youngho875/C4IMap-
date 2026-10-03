// MapGeneratorDlg.cpp: 구현 파일
//

#include "pch.h"
#include "C4IMap.h"
#include "MapGeneratorDlg.h"
#include "afxdialogex.h"

#include "MapDataMgr.h"

#define		WM_DATAPROCESS_THREADINFO			WM_USER + 200
#define		TM_DATAPROCESS_UPDATEVIEW			1
#define		TM_DATAPROCESS_UPDATEVIEW1			2
#define		TM_DATAPROCESS_UPDATEVIEW2			3
#define		TM_DATAPROCESS_UPDATEVIEW3			4

/////////////////////////////////////////////////////////////////////////////
// CMapGeneratorDlg dialog

// Thread 가 실제 처리하는 코드
UINT processMapInfo(LPVOID lParam);
UINT processMapProperty(LPVOID lParam);
UINT processRasterMapInfo(LPVOID lParam);
UINT processSatelliteMapInfo(LPVOID lParam);


// CMapGeneratorDlg 대화 상자

IMPLEMENT_DYNAMIC(CMapGeneratorDlg, CDialogEx)

CMapGeneratorDlg::CMapGeneratorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAP_GENERATOR, pParent)
	, m_bChkVector(FALSE)
	, m_bChkRaster(FALSE)
	, m_bChkSatellite(FALSE)
{
	m_pThread[3] = NULL;
	for (int i = 0; i < 4; ++i) m_bCanDataWrite[i] = FALSE;
	m_pThread[0] = NULL;	// mapinfo thread
	m_pThread[1] = NULL;	// mapproperty thread
	m_pThread[2] = NULL;	// raster & satellite mapinfo thread

	m_iMapPropertyCnt = m_iMapInfoTotCnt = m_iRasterCnt = m_iSatelliteCnt = 0;

}

CMapGeneratorDlg::~CMapGeneratorDlg()
{
}

void CMapGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_CREATE, m_btnCreate);
	DDX_Control(pDX, IDC_CHK_VECTOR, m_btnChkVector);
	DDX_Control(pDX, IDC_CHK_RASTER, m_btnChkRaster);
	DDX_Control(pDX, IDC_CHK_SATELLITE, m_btnChkSatellite);
	DDX_Control(pDX, IDC_LIST_TEXT, m_lbText);
	DDX_Check(pDX, IDC_CHK_VECTOR, m_bChkVector);
	DDX_Check(pDX, IDC_CHK_RASTER, m_bChkRaster);
	DDX_Check(pDX, IDC_CHK_SATELLITE, m_bChkSatellite);
}


BEGIN_MESSAGE_MAP(CMapGeneratorDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_VECTOR, OnChkVector)
	ON_BN_CLICKED(IDC_CHK_RASTER, OnChkRaster)
	ON_BN_CLICKED(IDC_BTN_CREATE, OnBtnCreate)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHK_SATELLITE, OnChkSatellite)
END_MESSAGE_MAP()


// CMapGeneratorDlg 메시지 처리기

BOOL CMapGeneratorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: Add extra initialization here
	m_iMapInfoTotCnt = m_iMapPropertyCnt = m_iProcessCnt = m_iRasterCnt = m_iSatelliteCnt = 0;

	for (int i = 0; i < 4; i++)
	{
		startDataProcess(i);
		m_bCanDataWrite[i] = TRUE;
	}

	m_bIsMapInfoOpen = m_bIsMapPropertyOpen =
		m_bIsRasterMapInfoOpen = m_bIsSatelliteMapInfoOpen = FALSE;
	m_strVectorLastPath = m_strRasterLastPath = m_strSatelliteLastPath = "";
	m_strMapInfoPath = CMapConfigMgr::GetMapInfoDataPath();
	m_strShpDataPath = CMapConfigMgr::GetShpDataPath();
	m_strRasterDataPath = CMapConfigMgr::GetRasterDataPath();
	m_strSatelliteDataPath = CMapConfigMgr::GetSatelliteDataPath();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMapGeneratorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CDialogEx::OnPaint() for painting messages
}

void CMapGeneratorDlg::OnChkVector()
{
	// TODO: Add your control notification handler code here
	m_bChkVector = !m_bChkVector;
}

void CMapGeneratorDlg::OnChkRaster()
{
	// TODO: Add your control notification handler code here
	m_bChkRaster = !m_bChkRaster;
}

void CMapGeneratorDlg::OnChkSatellite()
{
	// TODO: Add your control notification handler code here
	m_bChkSatellite = !m_bChkSatellite;
}

//"생성하기" 클릭시.
void CMapGeneratorDlg::OnBtnCreate()
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < 4; i++)  // 쓰레드 4가지 스타트.
	{
		startDataProcess(i);
		m_bCanDataWrite[i] = TRUE;  // 속성파일에 기록가능 플래그 셋
	}

	m_btnCreate.EnableWindow(FALSE);  // 버튼 비활성

	if (m_btnChkVector.GetCheck())  // 백터맵
		CreateVectorMapInfo();

	if (m_btnChkRaster.GetCheck())  // 래스터맵
		CreateRasterMapInfo();

	if (m_btnChkSatellite.GetCheck())  // 위성영상
		CreateSatelliteMapInfo();
}

LRESULT CMapGeneratorDlg::OnThreadMessage(UINT uParam, LONG lParam)
{
	if (uParam == TM_DATAPROCESS_UPDATEVIEW)
		Invalidate();
	if (uParam == TM_DATAPROCESS_UPDATEVIEW1)
		Invalidate();
	if (uParam == TM_DATAPROCESS_UPDATEVIEW2)
		Invalidate();
	if (uParam == TM_DATAPROCESS_UPDATEVIEW3)
		Invalidate();
	return 0;
}

void CMapGeneratorDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_iMapInfoTotCnt = m_iMapPropertyCnt = m_iProcessCnt = m_iRasterCnt = m_iSatelliteCnt = 0;

	for (int i = 0; i < 4; i++)
		m_bCanDataWrite[i] = FALSE;
	Sleep(1000);

	stopDataProcess(0);
	stopDataProcess(1);
	stopDataProcess(2);
	stopDataProcess(3);

	if (m_bIsMapInfoOpen == TRUE)
	{
		m_fileMapInfo.Close();
		m_bIsMapInfoOpen = FALSE;
	}

	if (m_bIsMapPropertyOpen == TRUE)
	{
		m_fileMapProperty.Close();
		m_bIsMapPropertyOpen = FALSE;
	}

	if (m_bIsRasterMapInfoOpen == TRUE)
	{
		m_fileRasterMapInfo.Close();
		m_bIsRasterMapInfoOpen = FALSE;
	}

	if (m_bIsSatelliteMapInfoOpen == TRUE)
	{
		m_fileSatelliteMapInfo.Close();
		m_bIsSatelliteMapInfoOpen = FALSE;
	}

	ArrayInit();

	CDialogEx::OnClose();
}

///////////////////////////////////////////////////////////////////////////
// Thread Process

UINT processMapInfo(LPVOID lParam)
{
	TRACE("info_thread : processMapInfo \n");

	BOOL bUpdateView;
	CTime time = CTime::GetCurrentTime();
	TRACE("INFO: (%02d:%02d:%02d) processMapInfo Thread created\n",
		time.GetHour(), time.GetMinute(), time.GetSecond());

	CMapGeneratorDlg* pParentWnd = (CMapGeneratorDlg*)lParam;

	CSingleLock sl(&pParentWnd->m_eventKillThread);
	pParentWnd->m_enumThreadStatus = CMapGeneratorDlg::RUNNING;
	::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW, NULL);

	DWORD dwSleep = CMapConfigMgr::GetMapGeneratorSleep();
	while (TRUE)
	{
		if (dwSleep == 0)
			break;
		if (sl.Lock(dwSleep))
			break;
		bUpdateView = FALSE;
		if (!pParentWnd->processData(bUpdateView, 0))
			break;
		if (bUpdateView)
			::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW, NULL);
	}

	sl.Unlock();

	::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW, NULL);
	pParentWnd->m_eventAllowParentDie.SetEvent();
	TRACE("INFO: processMapInfo Thread Killed.\n");
	pParentWnd->m_enumThreadStatus = CMapGeneratorDlg::STOPPED;

	return 0;
}

UINT processMapProperty(LPVOID lParam)
{
	BOOL bUpdateView;
	CTime time = CTime::GetCurrentTime();
	TRACE("INFO: (%02d:%02d:%02d) processMapProperty Thread created\n",
		time.GetHour(), time.GetMinute(), time.GetSecond());

	CMapGeneratorDlg* pParentWnd = (CMapGeneratorDlg*)lParam;

	CSingleLock sl(&pParentWnd->m_eventKillThread1);
	pParentWnd->m_enumThreadStatus1 = CMapGeneratorDlg::RUNNING1;
	::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW1, NULL);

	DWORD dwSleep = CMapConfigMgr::GetMapGeneratorSleep();
	while (TRUE)
	{
		if (dwSleep == 0)
			break;
		if (sl.Lock(dwSleep))
			break;
		bUpdateView = FALSE;
		if (!pParentWnd->processData(bUpdateView, 1))
			break;
		if (bUpdateView)
			::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW1, NULL);
	}

	sl.Unlock();

	::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW1, NULL);
	pParentWnd->m_eventAllowParentDie1.SetEvent();
	TRACE("INFO: processMapProperty Thread Killed.\n");
	pParentWnd->m_enumThreadStatus1 = CMapGeneratorDlg::STOPPED1;

	return 0;
}

UINT processRasterMapInfo(LPVOID lParam)
{
	BOOL bUpdateView;
	CTime time = CTime::GetCurrentTime();
	TRACE("INFO: (%02d:%02d:%02d) processRasterMapInfo Thread created\n",
		time.GetHour(), time.GetMinute(), time.GetSecond());

	CMapGeneratorDlg* pParentWnd = (CMapGeneratorDlg*)lParam;

	CSingleLock sl(&pParentWnd->m_eventKillThread2);
	pParentWnd->m_enumThreadStatus2 = CMapGeneratorDlg::RUNNING2;
	::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW2, NULL);

	DWORD dwSleep = CMapConfigMgr::GetMapGeneratorSleep();
	while (TRUE)
	{
		if (dwSleep == 0)
			break;
		if (sl.Lock(dwSleep))
			break;
		bUpdateView = FALSE;
		if (!pParentWnd->processData(bUpdateView, 2))
			break;
		if (bUpdateView)
			::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW2, NULL);
	}

	sl.Unlock();

	::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW2, NULL);
	pParentWnd->m_eventAllowParentDie2.SetEvent();
	TRACE("INFO: processRasterMapInfo Thread Killed.\n");
	pParentWnd->m_enumThreadStatus2 = CMapGeneratorDlg::STOPPED2;

	return 0;
}

UINT processSatelliteMapInfo(LPVOID lParam)
{
	BOOL bUpdateView;
	CTime time = CTime::GetCurrentTime();
	TRACE("INFO: (%02d:%02d:%02d) processSatelliteMapInfo Thread created\n",
		time.GetHour(), time.GetMinute(), time.GetSecond());

	CMapGeneratorDlg* pParentWnd = (CMapGeneratorDlg*)lParam;

	CSingleLock sl(&pParentWnd->m_eventKillThread3);
	pParentWnd->m_enumThreadStatus3 = CMapGeneratorDlg::RUNNING3;
	::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW3, NULL);

	DWORD dwSleep = CMapConfigMgr::GetMapGeneratorSleep();
	while (TRUE)
	{
		if (dwSleep == 0)
			break;
		if (sl.Lock(dwSleep))
			break;
		bUpdateView = FALSE;
		if (!pParentWnd->processData(bUpdateView, 3))
			break;
		if (bUpdateView)
			::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW3, NULL);
	}

	sl.Unlock();

	::PostMessage(pParentWnd->m_hWnd, WM_DATAPROCESS_THREADINFO, TM_DATAPROCESS_UPDATEVIEW3, NULL);
	pParentWnd->m_eventAllowParentDie3.SetEvent();
	TRACE("INFO: processSatelliteMapInfo Thread Killed.\n");
	pParentWnd->m_enumThreadStatus3 = CMapGeneratorDlg::STOPPED3;

	return 0;
}

///////////////////////////////////////////////////////////////////////////
// Operation

void CMapGeneratorDlg::startDataProcess(UINT iIndex)
{
	// 이미 실행 중이면 리턴
	switch (iIndex)
	{
	case 0: if (m_enumThreadStatus == RUNNING)
		return;
		break;
	case 1: if (m_enumThreadStatus1 == RUNNING1)
		return;
		break;
	case 2: if (m_enumThreadStatus2 == RUNNING2)
		return;
		break;
	case 3: if (m_enumThreadStatus3 == RUNNING3)
		return;
		break;
	}

	switch (iIndex)
	{
	case 0:
	{
		if (canDataProcess(iIndex))
		{
			TRACE("Start--processMapInfo\n");
			m_pThread[iIndex] = AfxBeginThread(processMapInfo, this, THREAD_PRIORITY_BELOW_NORMAL);
			if (!m_pThread[iIndex])
				TRACE("error: MapInfo Thread를 생성할 수가 없습니다.\n");
		}
	}
	break;
	case 1:
	{
		if (canDataProcess(iIndex))
		{

			TRACE("Start--processMapProperty\n");
			m_pThread[iIndex] = AfxBeginThread(processMapProperty, this, THREAD_PRIORITY_BELOW_NORMAL);
			if (!m_pThread[iIndex])
				TRACE("error: processMapProperty Thread를 생성할 수가 없습니다.\n");
		}
	}
	break;
	case 2:
	{


		if (canDataProcess(iIndex))
		{
			TRACE("Start--processRasterMapInfo\n");
			m_pThread[iIndex] = AfxBeginThread(processRasterMapInfo, this, THREAD_PRIORITY_BELOW_NORMAL);
			if (!m_pThread[iIndex])
				TRACE("error: processRasterMapInfo Thread를 생성할 수가 없습니다.\n");
		}
	}
	break;
	case 3:
	{
		if (canDataProcess(iIndex))
		{
			TRACE("Start--processSatelliteMapInfo\n");
			m_pThread[iIndex] = AfxBeginThread(processSatelliteMapInfo, this, THREAD_PRIORITY_BELOW_NORMAL);
			if (!m_pThread[iIndex])
				TRACE("error: processSatelliteMapInfo Thread를 생성할 수가 없습니다.\n");
		}
	}
	break;
	}

	Invalidate();
}

void CMapGeneratorDlg::stopDataProcess(UINT iIndex)
{
	DWORD dwExitCode;
	switch (iIndex)
	{
	case 0:
	{
		if (m_pThread[iIndex] != NULL &&
			GetExitCodeThread(m_pThread[iIndex]->m_hThread, &dwExitCode) &&
			dwExitCode == STILL_ACTIVE)
		{
			DWORD dwThreadID = m_pThread[iIndex]->m_nThreadID;
			m_eventKillThread.SetEvent();
			CSingleLock sl(&m_eventAllowParentDie);
			sl.Lock(500);
		}
		m_pThread[iIndex] = NULL;
	}
	break;
	case 1:
	{
		if (m_pThread[iIndex] != NULL &&
			GetExitCodeThread(m_pThread[iIndex]->m_hThread, &dwExitCode) &&
			dwExitCode == STILL_ACTIVE)
		{
			DWORD dwThreadID = m_pThread[iIndex]->m_nThreadID;
			m_eventKillThread1.SetEvent();
			CSingleLock sl(&m_eventAllowParentDie1);
			sl.Lock(500);
		}
		m_pThread[iIndex] = NULL;
	}
	break;
	case 2:
	{
		if (m_pThread[iIndex] != NULL &&
			GetExitCodeThread(m_pThread[iIndex]->m_hThread, &dwExitCode) &&
			dwExitCode == STILL_ACTIVE)
		{
			DWORD dwThreadID = m_pThread[iIndex]->m_nThreadID;
			m_eventKillThread2.SetEvent();
			CSingleLock sl(&m_eventAllowParentDie2);
			sl.Lock(500);
		}
		m_pThread[iIndex] = NULL;
	}
	break;
	case 3:
	{
		if (m_pThread[iIndex] != NULL &&
			GetExitCodeThread(m_pThread[iIndex]->m_hThread, &dwExitCode) &&
			dwExitCode == STILL_ACTIVE)
		{
			DWORD dwThreadID = m_pThread[iIndex]->m_nThreadID;
			m_eventKillThread3.SetEvent();
			CSingleLock sl(&m_eventAllowParentDie3);
			sl.Lock(500);
		}
		m_pThread[iIndex] = NULL;
	}
	break;
	}

	Invalidate();
}

BOOL CMapGeneratorDlg::canDataProcess(UINT iIndex)
{
	switch (iIndex)
	{
	case 0:
	{
		if (m_enumThreadStatus == RUNNING)
			return FALSE;
	}
	break;
	case 1:
	{
		if (m_enumThreadStatus1 == RUNNING1)
			return FALSE;
	}
	break;
	case 2:
	{
		if (m_enumThreadStatus2 == RUNNING2)
			return FALSE;
	}
	break;
	case 3:
	{
		if (m_enumThreadStatus3 == RUNNING3)
			return FALSE;
	}
	break;
	}

	return TRUE;
}

BOOL CMapGeneratorDlg::processData(BOOL& bUpdateView, UINT iIndex)
{
	stDataHeader* pItem;
	CPtrList list;
	BOOL bRes;
	int iSuccess = 0;

	switch (iIndex)
	{
	case 0: m_mapinfodata.popAll(list);
		break;
	case 1: m_mappropertydata.popAll(list);
		break;
	case 2: m_rasterdata.popAll(list);
		break;
	case 3: m_satellitedata.popAll(list);
		break;
	}

	if (list.IsEmpty())
		return TRUE;

	while (!list.IsEmpty())
	{
		pItem = (stDataHeader*)list.RemoveHead();
		bRes = OnDataWrite(pItem);
		if (bRes) iSuccess++;
		delete pItem;
	}

	if (iSuccess > 0)
		bUpdateView = TRUE;

	return TRUE;
}

void CMapGeneratorDlg::CreateVectorMapInfo()
{
	CString strText, strPath, strPath1, strPath2;
	stDirList* pDirList;

	ArrayInit();
	m_strVectorLastPath = "";

	if (m_bIsMapInfoOpen == TRUE)
	{
		m_fileMapInfo.Close();
		m_bIsMapInfoOpen = FALSE;
	}
	if (m_bIsMapPropertyOpen == TRUE)
	{
		m_fileMapProperty.Close();
		m_bIsMapPropertyOpen = FALSE;
	}

	CTime time = CTime::GetCurrentTime();
	strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
	strText += " MapInfo Data Create Start!";
	m_lbText.AddString(strText);


	DirectoryFind(VECTOR);
	time = CTime::GetCurrentTime();
	strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
	strText += " Directory 탐색 End!";
	m_lbText.AddString(strText);

	m_iMapInfoTotCnt = 0;
	CString strTmp;
	int iPos;
	for (int i = 0; i < m_paDirList.GetSize(); i++)
	{
		strPath1 = "";
		pDirList = m_paDirList[i];
		strPath = pDirList->m_strRootPath;
		strTmp = pDirList->m_strSubPath;

		iPos = strTmp.Find("\\");
		if (iPos != -1)
		{
			strPath1 = strTmp.Mid(0, iPos);
			strPath2 = strTmp.Mid(iPos + 1, strTmp.GetLength());
		}
		else
		{
			if (!strTmp.IsEmpty())
				strPath1 = strTmp;
			else
				strPath1 = "-1";
			strPath2 = "-1";
		}

		if (i == m_paDirList.GetSize() - 1)
			m_strVectorLastPath = strPath + strPath1;

		AddMapInfo(i, strPath, strPath1, strPath2);
	}
}

void CMapGeneratorDlg::CreateRasterMapInfo()
{
	CString strText, strPath, strPath1, strPath2, strPath3;
	stDirList* pDirList;

	if (m_bIsRasterMapInfoOpen == TRUE)
	{
		m_fileRasterMapInfo.Close();
		m_bIsRasterMapInfoOpen = FALSE;
	}

	m_iRasterCnt = 0;
	m_strRasterLastPath = "";

	// rastermapinfo.txt create
	ArrayInit();
	CTime time = CTime::GetCurrentTime();
	strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
	strText += " RasterMapInfo Data Create Start!";
	m_lbText.AddString(strText);

	// 공도 & 육도 format find
	DirectoryFind(RASTER);
	time = CTime::GetCurrentTime();
	strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
	strText += " Directory 탐색 End!";
	m_lbText.AddString(strText);
	for (int i = 0; i < m_paDirList.GetSize(); i++)
	{
		pDirList = m_paDirList[i];
		strPath = pDirList->m_strRootPath;
		strPath1 = pDirList->m_strSubPath;
		if (i == m_paDirList.GetSize() - 1)
			m_strRasterLastPath = strPath + strPath1;
		AddRasterMapInfo(strPath, strPath1);
	}
}

void CMapGeneratorDlg::CreateSatelliteMapInfo()
{
	CString strText, strPath, strPath1, strPath2, strPath3;
	stDirList* pDirList;

	if (m_bIsSatelliteMapInfoOpen == TRUE)
	{
		m_fileSatelliteMapInfo.Close();
		m_bIsSatelliteMapInfoOpen = FALSE;
	}

	m_iSatelliteCnt = 0;
	m_strSatelliteLastPath = "";

	// satellitemapinfo.txt create
	ArrayInit();
	CTime time = CTime::GetCurrentTime();
	strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
	strText += " SatelliteMapInfo Data Create Start!";
	m_lbText.AddString(strText);

	DirectoryFind(CIB);
	time = CTime::GetCurrentTime();
	strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
	strText += " Directory 탐색 End!";
	m_lbText.AddString(strText);

	for (int i = 0; i < m_paDirList.GetSize(); i++)
	{
		pDirList = m_paDirList[i];
		strPath = pDirList->m_strRootPath;
		strPath1 = pDirList->m_strSubPath;
		if (i == m_paDirList.GetSize() - 1)
			m_strSatelliteLastPath = strPath + strPath1;
		AddSatelliteMapInfo(strPath, strPath1);
	}
}

static CTypedPtrArray<CPtrArray, stDirList*> paDirList;

BOOL CMapGeneratorDlg::DirectoryFind(enDataType eDataType)
{
	BOOL ret;
	CString strFileName, strPath, strPath1, strSubPath, strTmp, strText;
	CStringList slSubPath;
	CFileFind find;
	stDirList* pDirList;

	strPath1 = "";

	if (m_iProcessCnt == 0)
	{
		switch (eDataType)
		{
		case VECTOR: strFileName = m_strShpDataPath + "\\*.*";
			break;
		case RASTER: strFileName = m_strRasterDataPath + "\\*.*";
			break;
		case CIB: strFileName = m_strSatelliteDataPath + "\\*.*";
			break;
			// UnKnown format
		default:
			break;
		}

		ret = find.FindFile(strFileName);
		if (ret)
		{
			while (find.FindNextFile())
			{
				strPath = find.GetFileName();
				if (find.IsDots()) continue;
				if (!find.IsDirectory()) continue;
				AddTmpDirList(strPath, "");
			}
			strPath = find.GetFileName();
			if (find.IsDirectory())
				AddTmpDirList(strPath, "");
		}
		find.Close();
	}
	else if (paDirList.GetSize() > 0)
	{
		int iSize = paDirList.GetSize();
		for (int i = 0; i < iSize; )
		{
			pDirList = paDirList[i];
			strPath = pDirList->m_strRootPath;
			strPath1 = pDirList->m_strSubPath;

			switch (eDataType)
			{
			case VECTOR: strFileName = m_strShpDataPath + "\\" + strPath + "\\" + strPath1 + "\\*.*";
				break;
			case RASTER: strFileName = m_strRasterDataPath + "\\" + strPath + "\\" + strPath1 + "\\*.*";
				break;
			case CIB: strFileName = m_strSatelliteDataPath + "\\" + strPath + "\\" + strPath1 + "\\*.*";
				break;
				// UnKnown format
			default:
				break;
			}

			ret = find.FindFile(strFileName);
			if (ret)
			{
				while (find.FindNextFile())
				{
					strTmp = find.GetFileName();
					if (find.IsDots()) continue;
					if (!find.IsDirectory()) continue;
					if (!strPath1.IsEmpty())
						strTmp = strPath1 + "\\" + strTmp;
					AddTmpDirList(strPath, strTmp);
				}
				strTmp = find.GetFileName();
				if (find.IsDirectory() && !find.IsDots())
				{
					if (!strPath1.IsEmpty())
						strTmp = strPath1 + "\\" + strTmp;
					AddTmpDirList(strPath, strTmp);
				}
			}
			find.Close();
			i++;
		}
	}

	/* //by Xman94 2009-07-24 주석처리
	for(int i=0; i<paDirList.GetSize(); )
	{
		pDirList = paDirList[i];
		strPath = pDirList->m_strRootPath;
		strPath1 = pDirList->m_strSubPath;

		if(pDirList->m_bIsAccess)
		{
			delete paDirList.GetAt(i);
			paDirList.RemoveAt(i);
			continue;
		}

		switch(eDataType)
		{
		case VECTOR : strFileName = m_strShpDataPath + "\\" + strPath + "\\" + strPath1 + "\\*.shp";
			break;
		case RASTER : strFileName = m_strRasterDataPath + "\\" + strPath + "\\" + strPath1;
			if(strPath == "5만")
				strFileName += "\\*.TL2";
			else if(strPath == "10만")
				strFileName += "\\*.TC2";
			else // tiff format
				strFileName += "\\*.TIF";
			break;
		case CIB : strFileName = m_strSatelliteDataPath + "\\" + strPath + "\\" + strPath1;
			if(strPath == "1m")
				strFileName += "\\*.I42";
			else
				strFileName += "\\*.TIF";
			break;
		// UnKnown format
		default :
			break;
		}

		ret = find.FindFile(strFileName);
		//zzzzz test 2008-10-08
		TRACE("파일리스트(AddTmpDirList) : strFileName = %s\n", strFileName);
		if(ret)
		{
			//zzzzz test 2008-10-08
			TRACE("파일리스트(FindFile) : strPath = %s, strPath1 = %s\n", strPath, strPath1);
			AddDirList(strPath, strPath1);
			delete paDirList.GetAt(i);
			paDirList.RemoveAt(i);
			continue;
		}
		else
			pDirList->m_bIsAccess = TRUE;

		i++;
	}
	*/
	//by Xman94 2009-07-24
	int nCnt = paDirList.GetSize() - 1;
	for (int i = nCnt; i >= 0; i--)
	{
		pDirList = paDirList[i];
		strPath = pDirList->m_strRootPath;
		strPath1 = pDirList->m_strSubPath;

		if (pDirList->m_bIsAccess)
		{
			delete paDirList.GetAt(i);
			paDirList.RemoveAt(i);
			continue;
		}

		switch (eDataType)
		{
		case VECTOR:
			strFileName = m_strShpDataPath + "\\" + strPath + "\\" + strPath1 + "\\*.shp";
			break;
		case RASTER:
			strFileName = m_strRasterDataPath + "\\" + strPath + "\\" + strPath1;
			if (strPath == "5만")
				strFileName += "\\*.TL2";
			else if (strPath == "10만")
				strFileName += "\\*.TC2";
			else // tiff format
				strFileName += "\\*.TIF";
			break;
		case CIB:
			strFileName = m_strSatelliteDataPath + "\\" + strPath + "\\" + strPath1;

			// png file과 
			if (strPath == "1m")
				strFileName += "\\*.I42";
			////////////////////////////////////////////
			// 이곳을 추가했음  young
			else if (strPath == "9")
				strFileName += "\\*.PNG";
			////////////////////////////////////////////
			else
				strFileName += "\\*.TIF";
			break;
			// UnKnown format
		default:
			break;
		}

		ret = find.FindFile(strFileName);
		//zzzzz test 2008-10-08
		TRACE("파일리스트(AddTmpDirList) : strFileName = %s\n", strFileName);
		if (ret)
		{
			//zzzzz test 2008-10-08
			TRACE("파일리스트(FindFile) : strPath = %s, strPath1 = %s\n", strPath, strPath1);
			AddDirList(strPath, strPath1);
			delete paDirList.GetAt(i);
			paDirList.RemoveAt(i);
			continue;
		}
		else
			pDirList->m_bIsAccess = TRUE;
	}
	m_iProcessCnt++;

	CTime time = CTime::GetCurrentTime();
	strText.Format("(%02d:%02d:%02d) %d번째 Directory 탐색...", time.GetHour(), time.GetMinute(), time.GetSecond(), m_iProcessCnt);
	m_lbText.AddString(strText);

	// 재귀호출
	if (paDirList.GetSize() > 0)
		DirectoryFind(eDataType);

	m_iProcessCnt = 0;
	return TRUE;
}

BOOL CMapGeneratorDlg::AddTmpDirList(CString strPath, CString strSubPath)
{
	stDirList* pDirList = new stDirList;
	pDirList->m_bIsAccess = FALSE;
	pDirList->m_strRootPath = strPath;
	pDirList->m_strSubPath = strSubPath;
	paDirList.Add(pDirList);
	return TRUE;
}

BOOL CMapGeneratorDlg::SeparatePath(CString strPath1, CStringList& slData)
{
	CString strDir;
	CString strData = strPath1;
	int iIndex, iIndex2;
	iIndex2 = 0;

	while (TRUE)
	{
		iIndex = strData.Find("\\");
		if (iIndex == -1)
			break;
		strDir = strData.Mid(iIndex2, iIndex);
		strData = strData.Mid(iIndex + 1, strData.GetLength() - iIndex + 1);
		slData.AddTail(strDir);
	}

	slData.AddTail(strData);

	if (slData.GetCount() > 0)
		return TRUE;
	else
		return FALSE;
}

BOOL CMapGeneratorDlg::AddDirList(CString strPath, CString strPath1)
{
	CString strKey1, strKey2;
	CString strSubPath;
	stDirList* pDirList = NULL;
	CString strScale = strPath.Mid(0, strPath.GetLength() - 2);

	CStringList slData;
	POSITION pos;
	if (SeparatePath(strPath1, slData))
	{
		strKey1.Format("%010s", strPath);
		pos = slData.GetHeadPosition();
		while (pos)
		{
			strSubPath.Format("%020s", slData.GetNext(pos));
			strKey1 += strSubPath;
		}
		slData.RemoveAll();
	}
	else
		strKey1.Format("%010s%020s", strPath, strPath1);

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_paDirList.GetUpperBound();
	int iMid;
	int iCmp;

	// binary search ==> add
	while (iLow <= iHigh)
	{
		iMid = (iLow + iHigh) / 2;
		strScale = m_paDirList[iMid]->m_strRootPath;
		strSubPath = m_paDirList[iMid]->m_strSubPath;

		if (SeparatePath(strSubPath, slData))
		{
			strKey2.Format("%010s", strScale);
			pos = slData.GetHeadPosition();
			while (pos)
			{
				strSubPath.Format("%020s", slData.GetNext(pos));
				strKey2 += strSubPath;
			}
			slData.RemoveAll();
		}
		else
			strKey2.Format("%010s%020s", strScale, strSubPath);

		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

		// 같은게 있으면 
		if (iCmp == 0)
		{
			pDirList = m_paDirList[iMid];
			bFound = TRUE;
			break;
		}
		else if (iCmp > 0)
		{
			iHigh = iMid - 1;
		}
		else
		{
			iLow = iMid + 1;
		}
	}	// wnd while

	if (pDirList == NULL)
		pDirList = new stDirList;
	pDirList->m_strRootPath = strPath;
	pDirList->m_strSubPath = strPath1;
	//zzzzz test 2008-10-08
	TRACE("파일리스트(AddDirList) : bFound = %d, m_strRootPath = %s, m_strSubPath = %s\n"
		, bFound
		, pDirList->m_strRootPath
		, pDirList->m_strSubPath);

	if (!bFound)
	{
		if (iLow > m_paDirList.GetUpperBound())
			m_paDirList.Add(pDirList);
		else
			m_paDirList.InsertAt(iLow, pDirList);
	}

	return TRUE;
}

BOOL CMapGeneratorDlg::AddMapInfo(int iIndex, CString strPath, CString strPath1, CString strPath2)
{
	stMapInfo* mapinfo = new stMapInfo;
	mapinfo->m_enDataType = MAPINFO;
	mapinfo->m_strSN.Format("%d", iIndex + 1);
	mapinfo->m_strScale = strPath;
	mapinfo->m_strPath = strPath1;
	mapinfo->m_strSubPath = strPath2;
	m_mapinfodata.push((LPVOID)mapinfo);

	return TRUE;
}

BOOL CMapGeneratorDlg::AddMapProperty(CString strSN, int nShapeType, CString strScale, CString strFileName)
{
	stMapProperty* mapproperty = new stMapProperty;

	mapproperty->m_enDataType = MAPPROPERTY;
	mapproperty->m_strSN = strSN;
	mapproperty->m_strScale = strScale;
	mapproperty->m_strFileName = strFileName;
	mapproperty->m_nShapeType = nShapeType;
	mapproperty->m_strIsVisible = "1";
	m_mappropertydata.push((LPVOID)mapproperty);

	return TRUE;
}

BOOL CMapGeneratorDlg::AddRasterMapInfo(CString strScale, CString strPath)
{
	stRasterMapInfo* rastermapinfo = new stRasterMapInfo;
	rastermapinfo->m_enDataType = RASTERMAPINFO;
	rastermapinfo->m_strScale = strScale;
	if (strPath.IsEmpty())
		strPath = "NULL";
	rastermapinfo->m_strPath = strPath;
	m_rasterdata.push((LPVOID)rastermapinfo);

	return TRUE;
}

BOOL CMapGeneratorDlg::AddSatelliteMapInfo(CString strScale, CString strPath)
{
	stSatelliteMapInfo* satellitemapinfo = new stSatelliteMapInfo;
	satellitemapinfo->m_enDataType = SATELLITEMAPINFO;
	satellitemapinfo->m_strScale = strScale;
	if (strPath.IsEmpty())
		strPath = "NULL";
	satellitemapinfo->m_strPath = strPath;
	m_satellitedata.push((LPVOID)satellitemapinfo);

	return TRUE;
}

BOOL CMapGeneratorDlg::OnDataWrite(stDataHeader* pItem)
{
	int nShapeType;
	FILE* fpSHP;
	CFile file;
	CFileFind find;
	BOOL ret = FALSE;
	BOOL bRes = FALSE;
	CString strData, strPath, strPath1, strPath2, strFileName, strScale, strFullName, strFile, strName,
		strSN, strLeft, strRight, strTop, strBottom, strText, strSaveFileName, strDeli, strExt, strTmp;
	shpPoint* point;
	shpPolyLine* polyline;
	shpPolygon* polygon;
	stMapInfo* pMapInfo;
	stMapProperty* mapproperty;
	stRasterMapInfo* rastermapinfo;
	stSatelliteMapInfo* satellitemapinfo;
	CWnd* pWnd = AfxGetMainWnd();
	double left, top, right, bottom;
	MBR mbr;
	int iCnt, iCnt2;
	CTime time;
	strDeli = ",";

	switch (pItem->m_enDataType)
	{
		// "MapInfo.txt" file write
	case MAPINFO:
	{
		if (!m_bCanDataWrite[0])
			break;

		left = bottom = 999.0;
		right = top = 0.0;
		pMapInfo = (stMapInfo*)pItem;
		strPath = pMapInfo->m_strScale;
		strPath1 = pMapInfo->m_strPath;
		strPath2 = pMapInfo->m_strSubPath;
		strExt = "*.shp";

		strFileName = m_strShpDataPath + strPath + "\\";
		if (strPath1 != "-1")
			strFileName += strPath1 + "\\";
		if (strPath2 != "-1")
			strFileName += strPath2 + "\\";
		strFileName += strExt;
		//strScale = strPath.Mid(0, strPath.GetLength()-2);
		strScale = strPath.Mid(4, strPath.GetLength() - 2);
		strScale.Replace("만", "");

		time = CTime::GetCurrentTime();
		strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
		strText += " updating mapinfo.txt, " + strPath + "/" + strPath1 + "/" + strPath2;
		iCnt = m_lbText.AddString(strText);
		m_lbText.SetCurSel(iCnt);
		ret = find.FindFile(strFileName);
		if (!ret) break;

		// all *.shp file open
		while (find.FindNextFile())
		{
			strFullName = find.GetFilePath();
			strFile = find.GetFileName();
			strName = find.GetFileTitle();

			fpSHP = fopen(strFullName, "rb");
			if (fpSHP == NULL) continue;

			// get shp type
			fseek(fpSHP, 32L, SEEK_SET);
			fread(&nShapeType, 4, 1, fpSHP);

			switch (nShapeType)
			{
			case SHPT_POINT:
			case SHPT_POINTZ:
			case SHPT_POINTM:
			case SHPT_MULTIPOINT:
			case SHPT_MULTIPOINTZ:
			case SHPT_MULTIPOINTM:
				point = new shpPoint;

				if (!(point->Open(strFullName)))
				{
					delete point;
					point = NULL;
					fclose(fpSHP);
					continue;
				}
				mbr = point->GetMBR();
				delete point;
				point = NULL;
				break;


			case SHPT_ARC:
			case SHPT_ARCZ:
			case SHPT_ARCM:

				polyline = new shpPolyLine;
				if (!(polyline->Open(strFullName)))
				{
					delete polyline;
					polyline = NULL;
					fclose(fpSHP);
					continue;
				}
				mbr = polyline->GetMBR();
				delete polyline;
				polyline = NULL;
				break;


			case SHPT_POLYGON:
			case SHPT_POLYGONZ:
			case SHPT_POLYGONM:
				polygon = new shpPolygon(pWnd);
				if (!(polygon->Open(strFullName)))
				{
					delete polygon;
					polygon = NULL;
					fclose(fpSHP);
					continue;
				}
				mbr = polygon->GetMBR();
				delete polygon;
				polygon = NULL;
				break;
			default:
			{
				fclose(fpSHP);
				continue;
			}
			}	// end case
			fclose(fpSHP);

			// rect value check
			if (left > mbr.xmin)
				left = mbr.xmin;
			if (right < mbr.xmax)
				right = mbr.xmax;
			if (top < mbr.ymax)
				top = mbr.ymax;
			if (bottom > mbr.ymin)
				bottom = mbr.ymin;

			// Add mapproerty basic infomation 
			AddMapProperty(pMapInfo->m_strSN, nShapeType, strScale, strName);
		}	// end while

		strFullName = find.GetFilePath();
		strFile = find.GetFileName();
		strName = find.GetFileTitle();
		fpSHP = fopen(strFullName, "rb");
		if (fpSHP == NULL) break;
		// get shp type
		fseek(fpSHP, 32L, SEEK_SET);
		fread(&nShapeType, 4, 1, fpSHP);
		switch (nShapeType)
		{
		case SHPT_POINT:
		case SHPT_POINTZ:
		case SHPT_POINTM:
		case SHPT_MULTIPOINT:
		case SHPT_MULTIPOINTZ:
		case SHPT_MULTIPOINTM:
			point = new shpPoint;
			if (!(point->Open(strFullName)))
			{
				delete point;
				point = NULL;
				fclose(fpSHP);
				break;
			}
			mbr = point->GetMBR();
			delete point;
			point = NULL;
			break;
		case SHPT_ARC:
		case SHPT_ARCZ:
		case SHPT_ARCM:
			polyline = new shpPolyLine;
			if (!(polyline->Open(strFullName)))
			{
				delete polyline;
				polyline = NULL;
				fclose(fpSHP);
				break;
			}
			mbr = polyline->GetMBR();
			delete polyline;
			polyline = NULL;
			break;
		case SHPT_POLYGON:
		case SHPT_POLYGONZ:
		case SHPT_POLYGONM:
			polygon = new shpPolygon(pWnd);
			if (!(polygon->Open(strFullName)))
			{
				delete polygon;
				polygon = NULL;
				fclose(fpSHP);
				break;
			}
			mbr = polygon->GetMBR();
			delete polygon;
			polygon = NULL;
			break;
		default:
		{
			fclose(fpSHP);
			break;
		}
		}	// end case
		fclose(fpSHP);

		// rect value check
		if (left > mbr.xmin)
			left = mbr.xmin;
		if (right < mbr.xmax)
			right = mbr.xmax;
		if (top < mbr.ymax)
			top = mbr.ymax;
		if (bottom > mbr.ymin)
			bottom = mbr.ymin;

		// Add mapproerty basic infomation 
		AddMapProperty(pMapInfo->m_strSN, nShapeType, strScale, strName);

		if (left >= right)
		{
			find.Close();
			break;
		}

		strLeft.Format("%.2f", left);
		strRight.Format("%.2f", right);
		strTop.Format("%.2f", top);
		strBottom.Format("%.2f", bottom);

		strData = pMapInfo->m_strSN + strDeli +
			strScale + strDeli +
			strPath1 + strDeli +
			strPath2 + strDeli +
			strLeft + strDeli +
			strRight + strDeli +
			strTop + strDeli +
			strBottom + "\r\n";

		// mapinfo.txt file create
		if (m_bIsMapInfoOpen == FALSE)
		{
			strSaveFileName = m_strMapInfoPath + "\\MapInfo(new).txt";
			if (!m_fileMapInfo.Open(strSaveFileName, CFile::modeCreate | CFile::modeWrite))
			{
				TRACE("error: OnDataWrite => file open failed!\n");
				return ret;
			}
			m_bIsMapInfoOpen = TRUE;
		}

		m_fileMapInfo.Write(strData, strData.GetLength());

		find.Close();

		m_iMapInfoTotCnt++;
		//			if(m_iMapInfoTotCnt == m_paDirList.GetSize())
		strTmp = strPath + strPath1;
		if (m_strVectorLastPath == strTmp)
		{
			time = CTime::GetCurrentTime();
			strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
			strText += " MapInfo Data Create End!";
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);

			// Add mapproerty basic infomation (예외처리:한반도국가경계 추가)
			AddMapProperty(pMapInfo->m_strSN, SHPT_ARC, "100", "LFA999");
		}
	}
	break;

	// "MapProperty.txt" file write
	case MAPPROPERTY:
	{
		if (!m_bCanDataWrite[1])
			break;
		mapproperty = (stMapProperty*)pItem;
		mapproperty->m_strFileName.MakeUpper();
		strData = mapproperty->m_strSN + strDeli +
			mapproperty->m_strScale + strDeli +
			mapproperty->m_strFileName + strDeli;

		switch (mapproperty->m_nShapeType)
		{
		case SHPT_POINT:
		case SHPT_POINTZ:
		case SHPT_POINTM:
		case SHPT_MULTIPOINT:
		case SHPT_MULTIPOINTZ:
		case SHPT_MULTIPOINTM:
		{
			strData += "0" + strDeli +		// kind
				"1" + strDeli +			// is visible
				"0" + strDeli +			// shape type
				"2" + strDeli +			// size
				"000000000" + strDeli +	// color
				"NULL" + strDeli +		// symbol name
				"1" + "\r\n";			// transparent
		}
		break;
		case SHPT_ARC:
		case SHPT_ARCZ:
		case SHPT_ARCM:
		{
			strData += "1" + strDeli +		// kind
				"1" + strDeli +			// is visible
				"000000000" + strDeli +	// color
				"1" + strDeli +			// width
				"0" + "\r\n";			// line style
		}
		break;
		case SHPT_POLYGON:
		case SHPT_POLYGONZ:
		case SHPT_POLYGONM:
		{
			strData += "2" + strDeli +		// kind
				"1" + strDeli +			// is visible
				"0" + strDeli +			// type
				"1" + strDeli +			// width
				"255255255" + strDeli +	// fill color
				"000000000" + strDeli +	// line color
				"NULL" + "\r\n";		// pattern name
		}
		break;
		default:
			return ret;
		}

		// mapproperty.txt file create
		if (m_bIsMapPropertyOpen == FALSE)
		{
			time = CTime::GetCurrentTime();
			strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
			strText += " MapProperty Data Create Start!";
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);
			strSaveFileName = m_strMapInfoPath + "\\MapProperty(new).txt";
			if (!m_fileMapProperty.Open(strSaveFileName, CFile::modeCreate | CFile::modeWrite))
			{
				TRACE("error: OnDataWrite => file open failed!\n");
				return ret;
			}
			m_bIsMapPropertyOpen = TRUE;
		}

		m_fileMapProperty.Write(strData, strData.GetLength());

		if (m_iMapPropertyCnt < 100)
			m_iMapPropertyCnt++;
		else
		{
			m_iMapPropertyCnt = 0;
			time = CTime::GetCurrentTime();
			strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
			strText += " updating mapproperty.txt, " +
				mapproperty->m_strSN + "/" +
				mapproperty->m_strScale + "/" +
				mapproperty->m_strFileName;
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);
		}
	}
	break;
	// "RasterMapinfo.txt" file write
	case RASTERMAPINFO:
	{
		if (!m_bCanDataWrite[2])
			break;

		rastermapinfo = (stRasterMapInfo*)pItem;
		strPath = rastermapinfo->m_strScale;
		strPath1 = rastermapinfo->m_strPath;
		if (strPath == "5만")
			strExt = "*.tl2";
		else
			if (strPath == "10만")
				strExt = "*.tc2";
			else
				strExt = "*.tif";
		strFileName = m_strRasterDataPath + strPath + "\\";
		if (strPath1 != "NULL")
			strFileName += strPath1 + "\\";

		strFileName += strExt;

		ret = find.FindFile(strFileName);
		if (!ret) break;

		// all *.tif file open
		while (find.FindNextFile())
		{
			strFullName = find.GetFilePath();
			strFile = find.GetFileName();
			strName = find.GetFileTitle();
			if (strPath == "5만")
				bRes = CMapDataMgr::OpenNitfData("tl2", strFullName, left, right, top, bottom);
			else
				if (strPath == "10만")
					bRes = CMapDataMgr::OpenNitfData("tc2", strFullName, left, right, top, bottom);
				else
					bRes = CMapDataMgr::OpenGeoTiffData(strFullName, left, right, top, bottom);
			if (!bRes) continue;

			m_iRasterCnt++;
			strSN.Format("%d", m_iRasterCnt);
			strScale = strPath.Mid(0, strPath.GetLength() - 2);
			strLeft.Format("%10.6f", left);
			strRight.Format("%10.6f", right);
			strTop.Format("%10.6f", top);
			strBottom.Format("%10.6f", bottom);

			strData = strSN + strDeli +
				strScale + strDeli +
				strPath1 + strDeli +
				strName + strDeli +
				strLeft + strDeli +
				strRight + strDeli +
				strTop + strDeli +
				strBottom + "\r\n";

			// rastermapinfo.txt file create
			if (m_bIsRasterMapInfoOpen == FALSE)
			{
				time = CTime::GetCurrentTime();
				strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
				strText += " RasterMapInfo Data Write Start!";
				iCnt = m_lbText.AddString(strText);
				m_lbText.SetCurSel(iCnt);
				strSaveFileName = m_strMapInfoPath + "\\RasterMapInfo(new).txt";
				if (!m_fileRasterMapInfo.Open(strSaveFileName, CFile::modeCreate | CFile::modeWrite))
				{
					TRACE("error: OnDataWrite => file open failed!\n");
					return ret;
				}
				m_bIsRasterMapInfoOpen = TRUE;
			}
			m_fileRasterMapInfo.Write(strData, strData.GetLength());

			iCnt2 = m_iRasterCnt % 50;
			if (iCnt2 == 0)
			{
				time = CTime::GetCurrentTime();
				strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
				strText += " updating rastermapinfo.txt, " + strPath + "/" + strPath1 + "/" + strName;
				iCnt = m_lbText.AddString(strText);
				m_lbText.SetCurSel(iCnt);
			}
		}	// end while

		strFullName = find.GetFilePath();
		strFile = find.GetFileName();
		strName = find.GetFileTitle();
		if (strPath == "5만")
			bRes = CMapDataMgr::OpenNitfData("tl2", strFullName, left, right, top, bottom);
		else
			if (strPath == "10만")
				bRes = CMapDataMgr::OpenNitfData("tc2", strFullName, left, right, top, bottom);
			else
				bRes = CMapDataMgr::OpenGeoTiffData(strFullName, left, right, top, bottom);
		if (!bRes)
		{
			find.Close();
			break;
		}

		m_iRasterCnt++;
		strSN.Format("%d", m_iRasterCnt);
		strScale = strPath.Mid(0, strPath.GetLength() - 2);
		strLeft.Format("%10.6f", left);
		strRight.Format("%10.6f", right);
		strTop.Format("%10.6f", top);
		strBottom.Format("%10.6f", bottom);

		strData = strSN + strDeli +
			strScale + strDeli +
			strPath1 + strDeli +
			strName + strDeli +
			strLeft + strDeli +
			strRight + strDeli +
			strTop + strDeli +
			strBottom + "\r\n";

		m_fileRasterMapInfo.Write(strData, strData.GetLength());

		find.Close();

		//strTmp = strPath + strPath1;
		//by Xman94 2008-10-07 strPath1이 "NULL"를 가질때 문제가 되어 수정함.
		if (strPath1 != "NULL")
			strTmp = strPath + strPath1;
		else
			strTmp = strPath;

		if (m_strRasterLastPath == strTmp)
		{
			m_fileRasterMapInfo.Close();
			m_bIsRasterMapInfoOpen = FALSE;
			time = CTime::GetCurrentTime();
			strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
			strText += " RasterMapInfo Data Create End!";
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);
		}
	}
	break;
	// "SatelliteMapinfo.txt" file write
	case SATELLITEMAPINFO:
	{
		if (!m_bCanDataWrite[3])// 4번째 배열 값을 SATELLITEMAPINFO 파일 오픈 여부 플래그.
			break;

		satellitemapinfo = (stSatelliteMapInfo*)pItem;
		strPath = satellitemapinfo->m_strScale; // 축척 1m, 10m, 20m
		strPath1 = satellitemapinfo->m_strPath; // local map 파일 경로.
		if (strPath == "1m")    // 축척 여부에 따라 분기.
			strExt = "*.i42";  // 1m 영상 파일.
		/////////////////////////////////////////////////////////
		// 이곳 추가		 young
		else if (strPath == "9")
			strExt = "*.png";
		///////////////////////////////////////////////////////
		else
			strExt = "*.tif";  // 10m & 20m 영상 파일.

		strFileName = m_strSatelliteDataPath + strPath + "\\";  // 영상파일 풀패스.
		if (strPath1 != "NULL") // 폴더 존재여부.
			strFileName += strPath1 + "\\";
		strFileName += strExt;

		ret = find.FindFile(strFileName); // 파일 존재 여부.
		if (!ret) break;

		// all *.tif file open
		while (find.FindNextFile())
		{
			strFullName = find.GetFilePath();  // 파일 이름과 확장자를 포함한 풀패스.
			strFile = find.GetFileName();   // 파일 확장자를 포함한 파일 이름.
			strName = find.GetFileTitle();  // 파일 확장자를 제외한 파일 이름
			if (strPath == "1m")                 // type,    in,       out,  out,  out,  out
				bRes = CMapDataMgr::OpenNitfData("i42", strFullName, left, right, top, bottom);
			/////////////////////////////////////////////////////////
			// 이곳 추가		 young
			//else if (strPath == "9")
			//	bRes = CMapDataMgr::OpenGeoPngData(strFullName, left, right, top, bottom);
			else
				bRes = CMapDataMgr::OpenGeoTiffData(strFullName, left, right, top, bottom);

			if (!bRes) continue;     // 영상파일을 비트맵생성 실패시 continue.

			m_iSatelliteCnt++;
			strSN.Format("%d", m_iSatelliteCnt);
			strScale = strPath.Mid(0, strPath.GetLength() - 1);
			strLeft.Format("%10.6f", left);
			strRight.Format("%10.6f", right);
			strTop.Format("%10.6f", top);
			strBottom.Format("%10.6f", bottom);

			strData = strSN + strDeli +   // no
				strScale + strDeli +  // 축척
				strPath1 + strDeli +  // 폴더경로
				strName + strDeli +   // 파일이름
				strLeft + strDeli +   // 영상 좌측 좌표
				strRight + strDeli +  // 영상 우측 좌표
				strTop + strDeli +    // 영상 상단 좌표
				strBottom + "\r\n";   // 영상 하단 좌표

		// satellitemapinfo.txt file create
			if (m_bIsSatelliteMapInfoOpen == FALSE)
			{
				time = CTime::GetCurrentTime();
				strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
				strText += " SatelliteMapInfo Data Write Start!";
				iCnt = m_lbText.AddString(strText);
				m_lbText.SetCurSel(iCnt);
				strSaveFileName = m_strMapInfoPath + "\\SatelliteMapInfo(new).txt";
				if (!m_fileSatelliteMapInfo.Open(strSaveFileName, CFile::modeCreate | CFile::modeWrite))
				{
					TRACE("error: OnDataWrite => file open failed!\n");
					return ret;
				}
				m_bIsSatelliteMapInfoOpen = TRUE;
			}
			m_fileSatelliteMapInfo.Write(strData, strData.GetLength());

			iCnt2 = m_iSatelliteCnt % 50;
			if (iCnt2 == 0)
			{
				time = CTime::GetCurrentTime();
				strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
				strText += " updating SMapInfo.idx, " + strPath + "/" + strPath1 + "/" + strName;
				iCnt = m_lbText.AddString(strText);
				m_lbText.SetCurSel(iCnt);
			}
		}	// end while

		strFullName = find.GetFilePath();
		strFile = find.GetFileName();
		strName = find.GetFileTitle();

		if (strPath == "1m")
			bRes = CMapDataMgr::OpenNitfData("i42", strFullName, left, right, top, bottom);
		/////////////////////////////////////////////////////////////////////////////////////
		// 이곳 추가    young
		//else if (strPath == "9")
		//	bRes = CMapDataMgr::OpenGeoPngData(strFullName, left, right, top, bottom);
		////////////////////////////////////////////////////////////////////////////////////////
		else
			bRes = CMapDataMgr::OpenGeoTiffData(strFullName, left, right, top, bottom);

		if (!bRes)
		{
			find.Close();
			break;
		}

		m_iSatelliteCnt++;
		strSN.Format("%d", m_iSatelliteCnt);
		strScale = strPath.Mid(0, strPath.GetLength() - 1);
		strLeft.Format("%10.6f", left);
		strRight.Format("%10.6f", right);
		strTop.Format("%10.6f", top);
		strBottom.Format("%10.6f", bottom);

		strData = strSN + strDeli +
			strScale + strDeli +
			strPath1 + strDeli +
			strName + strDeli +
			strLeft + strDeli +
			strRight + strDeli +
			strTop + strDeli +
			strBottom + "\r\n";

		// satellitemapinfo.txt file create
		if (m_bIsSatelliteMapInfoOpen == FALSE)
		{
			time = CTime::GetCurrentTime();
			strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
			strText += " SatelliteMapInfo Data Create Start!";
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);
			strSaveFileName = m_strMapInfoPath + "\\SatelliteMapInfo(new).txt";
			if (!m_fileSatelliteMapInfo.Open(strSaveFileName, CFile::modeCreate | CFile::modeWrite))
			{
				TRACE("error: OnDataWrite => file open failed!\n");
				return ret;
			}
			m_bIsSatelliteMapInfoOpen = TRUE;
		}
		m_fileSatelliteMapInfo.Write(strData, strData.GetLength());

		find.Close();

		//strTmp = strPath + strPath1;
		//by Xman94 2008-10-07 strPath1이 "NULL"를 가질때 문제가 되어 수정함.
		if (strPath1 != "NULL")
			strTmp = strPath + strPath1;
		else
			strTmp = strPath;

		if (m_strSatelliteLastPath == strTmp)
		{
			m_fileSatelliteMapInfo.Close();
			m_bIsSatelliteMapInfoOpen = FALSE;
			time = CTime::GetCurrentTime();
			strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
			strText += " SatelliteMapInfo Data Create End!";
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);

			strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
			strText += " SatelliteMapInfo Data ReGenerator Start!";
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);

			CMapDataMgr::OnReGeneratorSatelliteMapInfo(m_iSatelliteCnt);

			strText.Format("(%02d:%02d:%02d)", time.GetHour(), time.GetMinute(), time.GetSecond());
			strText += " SatelliteMapInfo Data ReGenerator End!";
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);
		}
		/*
		else //zzzzz test 2008-10-06
		{
			strText.Format("m_strSatelliteLastPath != strTmp");
			iCnt = m_lbText.AddString(strText);
			strText.Format("m_strSatelliteLastPath = %s", m_strSatelliteLastPath);
			iCnt = m_lbText.AddString(strText);
			strText.Format("strTmp = %s", strTmp);
			iCnt = m_lbText.AddString(strText);
			m_lbText.SetCurSel(iCnt);
		}
		*/
	}
	break;

	}	// case end

	return ret;
}

void CMapGeneratorDlg::ArrayInit()
{
	/*
	for(int i=0; i<m_paDirList.GetSize(); i++)
		delete m_paDirList.GetAt(i);
		*/
		//by Xman94 2008-10-08 수정.
	stDirList* pstDirList = NULL;
	for (int i = 0; i < m_paDirList.GetSize(); i++)
	{
		pstDirList = m_paDirList.GetAt(i);
		try {
			if (pstDirList != NULL)
			{
				delete pstDirList;
				pstDirList = NULL;
			}
		}
		catch (...)
		{
			TRACE("Exception(delete) : element of m_paDirList[%d] can't delete.\n", i);
		}
	}
	m_paDirList.RemoveAll();

	m_iProcessCnt = 0;
}

// 벡터데이터 지명리스트 추출 윈도우 출력
void CMapGeneratorDlg::OnBtnNameExtract()
{
	// TODO: Add your control notification handler code here
	//CPointTypeExtractFromVMAP dlg;
	//dlg.DoModal(theApp_m_hInstance);
}