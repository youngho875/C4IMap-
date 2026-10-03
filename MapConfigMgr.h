// AFC4IConfigMgr.h: interface for the CMapConfigMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFC4ICONFIGMGR_H__FA32BEFD_AA08_489D_8960_A0E338511D0C__INCLUDED_)
#define AFX_AFC4ICONFIGMGR_H__FA32BEFD_AA08_489D_8960_A0E338511D0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../inc/define.h"
#include "Initiator.h"
#include "MapDataMgr.h"
#include "GMapInfo.h"

#define TT_FONT			0x0200
#define DEVICE_FONT		0x0400
#define	INIT_FILE		"D:\\mapdata\\Config.ini"



static	POSITION		cur_pos, s_pos, e_pos;		// 현재, 시작, 끝 포지션 
static 	CStringList		m_slScale;					// 지도 축척 리스트 
static 	CStringList		m_slCoord;					// 좌표 리스트 

static	COLORREF		m_crDlgBKColor;				// 다이얼로그 배경색 
static	COLORREF		m_crDlgFontColor;			// 다이얼로그 font 색 
static	COLORREF		m_crDlgFontBKColor;			// 다이얼로그 font 배경색 

static	CStringList		m_slSmallMapFileName;		// 소축척지도 관련 파일명 
static	CStringList		m_slSmallWorldMapFileName;	// 세계지도 관련 파일명(소축척) 
static	CStringList		m_slWorldMapFileName;		// 세계지도 관련 파일명(면) 
static	CStringList		m_slWorldMapLFileName;		// 세계지도 관련 파일명(선) 
static	CStringList		m_slWorldMapPFileName;		// 세계지도 관련 파일명(점) 

static	CString			m_stSmallMapTextFileName;	// 소축척지도(text data) 관련 파일명 

static	double			m_dMapFirstLeft;			// 지도 초기 좌단 좌표(최초 로드시 사용) 
static	double			m_dMapFirstTop;				// 지도 초기 상단 좌표(최초 로드시 사용) 
static	double			m_dMapFirstZoom;			// 지도 초기 배율(최초 로드시 사용) 
static	int				m_iMapFirstScale;			// 지도 초기 축척(최초 로드시 사용) 

static	DWORD			m_dwMapGeneratorSleep;		// MapGenerator sleep time 

static	int			m_map_min_scale;				// 지도 도시 최소 배율 
static	int			m_map_max_scale;				// 지도 도시 최대 배율 
static	int			m_worldmap_min_scale;			// 세계지도 도시 최소 배율
static	int			m_worldmap_max_scale;			// 세계지도 도시 최대 배율

static	int			m_vector_2000000_minscale;		// 200만 벡터지도 도시 최소 배율
static	int			m_vector_2000000_maxscale;		// 200만 벡터지도 도시 최대 배율
static	int			m_vector_1000000_minscale;		// 100만 벡터지도 도시 최소 배율
static	int			m_vector_1000000_maxscale;		// 100만 벡터지도 도시 최대 배율
static	int			m_vector_500000_minscale;		// 50만 벡터지도 도시 최소 배율
static	int			m_vector_500000_maxscale;		// 50만 벡터지도 도시 최대 배율
static	int			m_vector_250000_minscale;		// 25만 벡터지도 도시 최소 배율
static	int			m_vector_250000_maxscale;		// 25만 벡터지도 도시 최대 배율
static	int			m_vector_100000_minscale;		// 10만 벡터지도 도시 최소 배율
static	int			m_vector_100000_maxscale;		// 10만 벡터지도 도시 최대 배율
static	int			m_vector_50000_minscale;		// 5만 벡터지도 도시 최소 배율 
static	int			m_vector_50000_maxscale;		// 5만 벡터지도 도시 최대 배율 

static	int			m_raster_2000000_minscale;		// 200만 레스터지도 도시 최소 배율
static	int			m_raster_2000000_maxscale;		// 200만 레스터지도 도시 최대 배율
static	int			m_raster_1000000_minscale;		// 100만 레스터지도 도시 최소 배율
static	int			m_raster_1000000_maxscale;		// 100만 레스터지도 도시 최대 배율
static	int			m_raster_500000_minscale;		// 50만 레스터지도 도시 최소 배율
static	int			m_raster_500000_maxscale;		// 50만 레스터지도 도시 최대 배율
static	int			m_raster_250000_minscale;		// 25만 레스터지도 도시 최소 배율
static	int			m_raster_250000_maxscale;		// 25만 레스터지도 도시 최대 배율
static	int			m_raster_100000_minscale;		// 10만 레스터지도 도시 최소 배율
static	int			m_raster_100000_maxscale;		// 10만 레스터지도 도시 최대 배율
static	int			m_raster_50000_minscale;		// 5만 레스터지도 도시 최소 배율 
static	int			m_raster_50000_maxscale;		// 5만 레스터지도 도시 최대 배율 

static	int			m_raster_20_minscale;			// 20미터 위성영상 도시 최소 배율 
static	int			m_raster_20_maxscale;			// 20미터 위성영상 도시 최대 배율 
static	int			m_raster_10_minscale;			// 10미터 위성영상 도시 최소 배율 
static	int			m_raster_10_maxscale;			// 10미터 위성영상 도시 최대 배율 
static	int			m_raster_1_minscale;			// 1미터 위성영상 도시 최소 배율 
static	int			m_raster_1_maxscale;			// 1미터 위성영상 도시 최대 배율 

static	int			m_iScale;						// 최초 도시할 지도 축척 
static	int			m_iRatio;						// 최초 도시할 지도 배율 

static	CString		m_strPathOvrXmlFormat;			// 투명도 XML format file path 

///** FTP 서버 IP */
//static	CString		m_strFTPServer_IP;
///** FTP 서버 ID */
//static	CString		m_strFTPServer_ID;
///** FTP 서버 password */
//static	CString		m_strFTPServer_password;
///** FTP 서버 port */
//static	CString		m_strFTPServer_port;
///** FTP 서버 path */
//static	CString		m_strFTPServer_path;
///** FTP 서버 sub path */
//static	CString		m_strFTPServer_subpath;

static	CString		m_strMapVersionDataPath;			// 지도 버젼정보 관리 path
static	CString		m_strServerMapVersionFileName;		// server 지도정보 파일 명
static	CString		m_strLocalMapVersionFileName;		// local 지도정보 파일 명 
static	CString		m_strMapFileMaster;					// 버젼 마스터 파일명 
static	int			m_iMapScale;						// 지도축척(임시변수) 

/** FTP server root path */
//static	CString		m_strFTPServerRootPath;

/**
* 환경설정 관리 클래스
* @author 유영호
* @version 1.0
* @since 2024.09.
**/
class  CMapConfigMgr : public CInitiator
{
public:
	CMapConfigMgr(CWnd* pParentWnd);
	virtual ~CMapConfigMgr();

// Attribute
public:

// Operation
public:
	static	CString GetOvrFormatPath();
	COLORREF GetStringToRGB(LPCTSTR lpValue, CString strDeli);
	static COLORREF GetDlgBKColor();
	static COLORREF GetDlgFontColor();
	static COLORREF GetDlgFontBKColor();

// 공통
	// 축척데이터
	static CString GetScaleHead();						// 축척데이터 얻기
	static int GetScaleCount();							// 축척데이터리스트 개수 얻기
	static CString GetScaleNext();						// 축척데이터리스트 index 증가
	
	// 좌표데이터
	static CString GetCoordHead();						// 좌표데이터 얻기
	static int GetCoordCount();							// 좌표데이터리스트 개수 얻기
	static CString GetCoordNext();						// 좌표데이터리스트 index 증가

	///** FTP server ip address 정보 얻기 */
	//static CString GetFTPServer_IP();
	///** FTP server userid 정보 얻기 */
	//static CString GetFTPServer_ID();
	///** FTP server password 정보 얻기 */
	//static CString GetFTPServer_password();
	///** FTP server portno 정보 얻기 */
	//static CString GetFTPServer_port();
	///** FTP server path 정보 얻기 */
	//static CString GetFTPServer_path();
	//static CString GetFTPServer_subpath();

	static int GetMapScale();							// 현제 도시된 지도 축척 얻기
	static void SetMapScale(EGMapScale enMapScale);		// 현제 도시된 지도 축척 설정

	/** FTP server 파일정보 열기 */
	//static BOOL FTPServerInfoOpen();
	//static BOOL FTPServerInfoOpen(WPARAM wParam);

	static void DirectoryCreate(CString strPath);						// 디렉토리 생성
	static BOOL ExtractDir(CString strSource, CStringList &slData);		// 디렉토리 추출

	// 지도 도시 관련
	static CStringList* GetSmallMapFileName();				// 소축척지도 관련 파일명 얻기
	static CStringList* GetSmallWorldMapFileName();			// 세계지도(소축척) 관련 파일명 얻기
	static CStringList* GetWorldMapFileName();				// 세계지도 관련 파일명 얻기
	static CStringList* GetWorldMapLFileName();				// 세계지도 관련 파일명 얻기
	static CStringList* GetWorldMapPFileName();				// 세계지도 관련 파일명 얻기
	static CString GetSmallMapTextFileName();				// 소축척지도 text data 파일명 얻기
	/** system font list get */
	void AddFont(ENUMLOGFONT* pelf, DWORD dwType, LPCTSTR lpszScript);
	static BOOL CALLBACK AFX_EXPORT EnumFamScreenCallBackEx( ENUMLOGFONTEX* pelf, 
						NEWTEXTMETRICEX* /*lpntm*/, int FontType, LPVOID pThis);

	static double GetMapFirstLeft();						// 지도 좌단 좌표 얻기
	static double GetMapFirstTop();							// 지도 상단 좌표 얻기
	static double GetMapFirstZoom();						// 지도 배율 얻기
	static int GetMapFirstScale();							// 지도 축척 얻기

	
	static CString GetMapVersionDataPath();					// mapversion data path get 
	static CString GetServerMapVersionFileName();			// server mapversion file name get 
	static CString GetLocalMapVersionFileName();			// local mapversion file name get 
	static CString GetMapFileMasterFileName();				// mapfilemaster file name get 
	static CString GetMapInfoDataPath();					// mapinfo data path get 
	static CString GetShpDataPath();						// shp data path get 
	static CString GetRasterDataPath();						// raster data path get 
	static CString GetSatelliteDataPath();					// satellite data path get 
	static CString GetSmallmapDataPath();					// smallmap data path get 
	static CString GetWorldmapDataPath();					// worldmap data path get 
	static CString GetSymbolDataPath();						// symbol data path get 
	static CString GetPatternDataPath();					// pattern data path get
	static CString GetUserMapDataPath();					// usermap data path get

	
	static BOOL GetMapMinMaxScale(int &iminscale, int &imaxscale);							// map min/max scale get 
	static BOOL GetVectorMapScale(EGMapScale enScale, int &iminscale, int &imaxscale);		// vector map scale get 
	static BOOL GetVectorMapScale(int iscale, int &iminscale, int &imaxscale);
	static BOOL GetRasterMapScale(EGMapScale enScale, int &iminscale, int &imaxscale);		// raster map scale get 
	static BOOL GetRasterMapScale(int iscale, int &iminscale, int &imaxscale);
	static int GetVectorMapKind(int iScale);												// 벡터지도 종류 얻기 
	static int GetRasterMapKind(int iScale);												// 레스터 지도 종류 얻기 

	/** 지도 축척 검사 */
	static BOOL IsScaleOver(int iMapType, BOOL bIsAutoMode, BOOL bIsMaxScale, long lScale, double dZoomFactor, double &newfactor, CString &strMsg);
	static BOOL IsScaleOver2(int iMapType, BOOL bIsAutoMode, BOOL bIsMaxScale, long lScale, double dZoomFactor, double &newfactor, CString &strMsg);
	static void SetCurrentMapInfo(int iScale, int iRatio);			// 현재 도시된 지도정보(축척,배율) 설정
	static double GetCurrentZoomFactor();							// 현재 도시된 지도 배율 얻기 
	static void GetCurrentMapInfo(int &iScale, int &iRatio);
	static int GetWorldMapMaxRatio();

	//static CString GetFTPServerRootPath();


// 투명도 관련
// THREAD
	
	static DWORD GetMapGeneratorSleep();		// 지도버젼관리 스레드 sleep time 얻기 

// implementations
protected:
	
	virtual void	OnReadItem( LPCTSTR lpszSection, LPCTSTR lpszItem, LPCTSTR lpValue, BOOL bSection );	// 환경 설정 파일 읽기 
	virtual void	OnReadEnd();	// 환경 설정 파일 닫기 
};

#endif // !defined(AFX_AFC4ICONFIGMGR_H__FA32BEFD_AA08_489D_8960_A0E338511D0C__INCLUDED_)
