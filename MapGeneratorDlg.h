#pragma once

#include "SafeQueue.h"
#include "MapConfigMgr.h"
#include "resource.h"

enum enDataHeader { MAPINFO, MAPPROPERTY, RASTERMAPINFO, SATELLITEMAPINFO };
enum enDataType { VECTOR, RASTER, CIB };	// VECTOR=*.SHP, RASTER=*.TIF/*.TL2/*.TC2, CIB=*.I42

/** 디렉토리 관리 구조체 */
struct stDirList
{
	BOOL		m_bIsAccess;
	CString		m_strRootPath;
	CString		m_strSubPath;
};

/** 지도영역정보 구조체 헤더 */
struct stDataHeader
{
	enDataHeader	m_enDataType;
};

/** 벡터지도 영역정보 구조체 */
struct stMapInfo : public stDataHeader
{
	CString		m_strSN;
	CString		m_strScale;
	CString		m_strPath;
	CString		m_strSubPath;
	CString		m_strLeft;
	CString		m_strRight;
	CString		m_strTop;
	CString		m_strBottom;
};

/** 지도파일리스트 구조체 */
struct stMapProperty : public stDataHeader
{
	CString		m_strSN;
	CString		m_strScale;
	CString		m_strFileName;
	int			m_nShapeType;
	CString		m_strIsVisible;
};

/** 레스터지도 영역정보 구조체 */
struct stRasterMapInfo : public stDataHeader
{
	CString		m_strScale;
	CString		m_strPath;
};

/** 위성영상지도 영역정보 구조체 */
struct stSatelliteMapInfo : public stDataHeader
{
	CString		m_strScale;
	CString		m_strPath;
};

// CMapGeneratorDlg 대화 상자

class CMapGeneratorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMapGeneratorDlg)

public:
	CMapGeneratorDlg(CWnd* pParent);   // 표준 생성자입니다.
	virtual ~CMapGeneratorDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAP_GENERATOR };
#endif

public:



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// Generated message map functions
	//{{AFX_MSG(CMapGeneratorDlg)
	afx_msg void OnChkVector();
	afx_msg void OnChkRaster();
	afx_msg void OnBtnCreate();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg LRESULT OnThreadMessage(UINT uParam, LONG lParam);
	afx_msg void OnChkSatellite();
	afx_msg void OnBtnNameExtract();

	/** 스레드 기동 */
	void		startDataProcess(UINT iIndex);
	/** 스레드 멈춤 */
	void		stopDataProcess(UINT iIndex);
	/** 영역정보 데이터 처리 가능 여부 체크 */
	BOOL		canDataProcess(UINT iIndex);

	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnCreate;
	CButton m_btnChkVector;
	CButton m_btnChkRaster;
	CButton m_btnChkSatellite;
	CListBox m_lbText;
	BOOL m_bChkVector;
	BOOL m_bChkRaster;
	BOOL m_bChkSatellite;

protected:
	/** 벡터지도 영역정보 큐 */
	safeQueue	m_mapinfodata;
	/** 벡터지도 파일리스트 큐 */
	safeQueue	m_mappropertydata;
	/** 레스터지도 영역정보 큐 */
	safeQueue	m_rasterdata;
	/** 위성영상지도 영역정보 큐 */
	safeQueue	m_satellitedata;
	/** 지도 영역정보 관리 스레드 */
	CWinThread* m_pThread[4];

public:
	/** 파일 생성 여부 */
	BOOL		m_bCanDataWrite[4];
	/** 스레드 상태 변수 */
	enum { STOPPED = 0, RUNNING } m_enumThreadStatus;
	/** 스레드 상태 변수 */
	enum { STOPPED1 = 0, RUNNING1 } m_enumThreadStatus1;
	/** 스레드 상태 변수 */
	enum { STOPPED2 = 0, RUNNING2 } m_enumThreadStatus2;
	/** 스레드 상태 변수 */
	enum { STOPPED3 = 0, RUNNING3 } m_enumThreadStatus3;
	/** 스레드 관리 이벤트 변수 */
	CEvent		m_eventAllowParentDie;
	/** 스레드 관리 이벤트 변수 */
	CEvent		m_eventAllowParentDie1;
	/** 스레드 관리 이벤트 변수 */
	CEvent		m_eventAllowParentDie2;
	/** 스레드 관리 이벤트 변수 */
	CEvent		m_eventAllowParentDie3;
	/** 스레드 관리 이벤트 변수 */
	CEvent		m_eventKillThread;
	/** 스레드 관리 이벤트 변수 */
	CEvent		m_eventKillThread1;
	/** 스레드 관리 이벤트 변수 */
	CEvent		m_eventKillThread2;
	/** 스레드 관리 이벤트 변수 */
	CEvent		m_eventKillThread3;
	/** 벡터지도 영역정보 저장 디렉토리 */
	CString		m_strMapInfoPath;
	/** 벡터지도 데이터 루트 디렉토리 */
	CString		m_strShpDataPath;
	/** 레스터지도 데이터 루트 디렉토리 */
	CString		m_strRasterDataPath;
	/** 위성영상지도 데이터 루트 디렉토리 */
	CString		m_strSatelliteDataPath;
	/** 벡터지도 영역정보파일 open 여부 */
	BOOL		m_bIsMapInfoOpen;
	/** 벡터지도 파일리스트 open 여부 */
	BOOL		m_bIsMapPropertyOpen;
	/** 레스터지도 영역정보파일 open 여부 */
	BOOL		m_bIsRasterMapInfoOpen;
	/** 위성영상지도 영역정보파일 open 여부 */
	BOOL		m_bIsSatelliteMapInfoOpen;
	/** 벡터지도 영역정보 파일관리자 */
	CFile		m_fileMapInfo;
	/** 벡터지도 파일리스트 파일관리자 */
	CFile		m_fileMapProperty;
	/** 레스터지도 영역정보 파일관리자 */
	CFile		m_fileRasterMapInfo;
	/** 위성영상지도 영역정보 파일관리자 */
	CFile		m_fileSatelliteMapInfo;
	/** 벡터지도 영역정보 건수 */
	int			m_iMapInfoTotCnt;
	/** 벡터지도 파일리스트 건수 */
	int			m_iMapPropertyCnt;
	/** 레스터지도 영역정보 건수 */
	int			m_iRasterCnt;
	/** 위성영상지도 영역정보 건수 */
	int			m_iSatelliteCnt;
	/** 파일 write 건수 */
	int			m_iProcessCnt;
	CString		m_strVectorLastPath;
	CString		m_strRasterLastPath;
	CString		m_strSatelliteLastPath;
	/** 디렉토리 탐색 임시 리스트 */
	CTypedPtrArray<CPtrArray, stDirList*> m_paDirList;

public:
	/** 디렉토리 탐색 임시 리스트 초기화 */
	void		ArrayInit();
	/** 영역정보 생성 시작 */
	BOOL		processData(BOOL& bUpdateView, UINT iIndex);
	/** 벡터지도 영역정보 생성 */
	void		CreateVectorMapInfo();
	/** 레스터지도 영역정보 생성 */
	void		CreateRasterMapInfo();
	/** 위성영상지도 영역정보 생성 */
	void		CreateSatelliteMapInfo();
	/** 디렉토리 탐색 */
	BOOL		DirectoryFind(enDataType eDataType);
	/** 탐색된 디렉토리 임시 리스트에 추가 */
	BOOL		AddTmpDirList(CString strPath, CString strSubPath);
	/** 디렉토리 명 분리 */
	BOOL		SeparatePath(CString strPath1, CStringList& slData);
	/** 탐색된 디렉토리 리스트에 추가 */
	BOOL		AddDirList(CString strPath, CString strPath1);
	/** 벡터지도 영역정보 추가 */
	BOOL		AddMapInfo(int iIndex, CString strPath, CString strPath1, CString strPath2);
	/** 벡터지도 파일리스트정보 추가 */
	BOOL		AddMapProperty(CString strSN, int nShapeType, CString strScale, CString strFileName);
	/** 레스터지도 영역정보 추가 */
	BOOL		AddRasterMapInfo(CString strScale, CString strPath);
	/** 위성영상지도 영역정보 추가 */
	BOOL		AddSatelliteMapInfo(CString strScale, CString strPath);
	/** 영역정보 파일 write */
	BOOL		OnDataWrite(stDataHeader* pItem);

	
};
