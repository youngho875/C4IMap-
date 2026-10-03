// MapDataMgr.h: interface for the CMapDataMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPDATAMGR_H__8A792912_E83B_4C1E_9BC2_1673AEF72B7C__INCLUDED_)
#define AFX_MAPDATAMGR_H__8A792912_E83B_4C1E_9BC2_1673AEF72B7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once


#include "../inc/define.h"
#include "../inc/CIData.h"
#include "../inc/SafeQueue.h"
#include "shpObject.h"
#include "shpPoint.h"
#include "shpPoly.h"
#include "shpPolygon.h"
#include "DIBSectionLite.h"
#include "GMapInfo.h"
#include "MapConfigMgr.h"


/** 바로가기 등록 갯수 */
#define DIRECTGO_MAXCOUNT 5

/** 소축척지도 종류 */
enum enSmallMapType
{
	SM_UNKNOWN = 0,		/** unknown */
	SM_SMALLMAP,		/** 소축척지도 도시용 벡터데이터 */
	SM_NAMEMAP,			/** 지명리스트 벡터데이터 */
	SM_INDEXMAP_5,		/** 인덱스리스트(5만) 벡터데이터 */
	SM_INDEXMAP_10,		/** 인덱스리스트(10만) 벡터데이터 */
	SM_WORLDMAP			/** 세계지도 */
};

/** 벡터지도 속성 종류 */
enum enMapProperty
{
	MP_POINT = 0,		/** 점 데이터 */
	MP_POLYLINE,		/** 선 데이터 */
	MP_POLYGON			/** 면 데이터 */
};

/** 벡터지도 텍스트 데이터 */
struct stMapText
{
	CGPointGP	m_gpPoint;
	CString		m_strName;
};

/** 지형요소 중분류 코드 */
struct stMapGroupItem
{
	CString		m_strID;			// 
	CString		m_strName;			// 
};

/** 지형요소 대분류 코드 */
struct stMapGroup
{
	CString		m_strID;			// 
	CString		m_strName;			// 
	CTypedPtrArray<CPtrArray, stMapGroupItem*> m_pGroupList;	// 
};

/** 색인맵 리스트 */
struct stIndexData
{
	CString		m_strID;				// 색인ID 
	CString		m_strIndexName;			// 색인명 
	CList<GPoint, GPoint> m_pPoint;		// 색인지점 point list (polygon data) 
	CGPointGP	m_gPoint;				// 색인지점 center point 
};

/** 지명맵 리스트 */
struct stNameData
{
	CString		m_strID;			// 지명ID 
	CString		m_strName;			// 지명 
	GPoint		m_pPoint;			// 지명지점 point (point data) 
};	

/** 지명리스트(취락(PAL105), 행정지역(AFA001) 추출 데이터 */
struct stNameData2
{
	CString		m_strID;			// 지명ID 
	CString		m_strName;			// 지명 
	CGPointGP		m_gpPoint;		// 지명지점 point 
};

/** 소축척지도 shape data */
struct stSmallMap
{
	enSmallMapType	m_enType;		// 소축척지도 종류 
	CString			m_strFileName;	// 소축척지도 파일명(path포함) 
	shpObject*		m_pObject;		// 소축척지도 shape object pointer 
};

/** 점/면 데이터 symbol/pattern */
struct stSymbol
{
	CString			m_strName;		// symbol name 
	HBITMAP			m_hBitmap;		// bitmap handle 
};

/** 도시요소 우선순위 관리 클래스 (default 속성 내용 포함) */
class CMapPriority
{
	// attribute
public:
	UINT			m_uiPriority;	// 우선순위
	EGMapScale		m_enMapScale;	// 축척
	CString			m_strFileName;	// 파일명
	BOOL			m_bTransParent;	// TransParent 여부
	BOOL			m_bIsVisible;	// 도시 여부
	CString			m_strName;		// 속성명
	int				m_iViewRatio;	// 보이기 배율
	CString			m_earth_or_air;	//
	CString			m_type_geo;		//
	
};

/** 점 우선순위 관리 클래스 */
class CPointPriority : public CMapPriority
{
	// attribute
public:
	UINT			m_uiRadius;		// 반지름
	COLORREF		m_crFillColor;	// fill color
	UINT			m_uiType;		// type
	CString			m_strBitmap;	//  bitmap name
	stSymbol*		m_pSymbol;		// symbol pointer
};

/** 선 우선순위 관리 클래스 */
class CPolylinePriority : public CMapPriority
{
	// attribute
public:
	UINT			m_uiLineWidth;	// line width
	COLORREF		m_crLineColor;	// line color
	UINT			m_uiType;		// line type
	UINT			m_uiLineWidth2;	// line width2
	COLORREF		m_crLineColor2;	// line color2
	UINT			m_uiType2;		// line type2
};

/** 면 우선순위 관리 클래스 */
class CPolygonPriority : public CMapPriority
{
	// attribute
public:
	UINT			m_uiLineWidth;	// line width
	COLORREF		m_crLineColor;	// line color
	COLORREF		m_crFillColor;	// fill color 
	UINT			m_uiType;		// line type
	CString			m_strBitmap;	// bitmap name
	stSymbol*		m_pPattern;		// pattern pointer
};

/** 도시요소/Mapping 관리 클래스 */
class CMapProperty
{
	// attribute
public:
	int				m_iMapKey;			// "mapinfo.txt'파일 primary key 
	EGMapScale		m_enMapScale;		// 축척 
	CString			m_strFileName;		// 파일명 
	enMapProperty	m_enMapProperty;	// 속성 종류 
	BOOL			m_bIsVisible;		// 복원도시 여부 
};

/** 도시요소 속성 클래스(점) */
class CPointProperty : public CMapProperty
{
	// attribute
public:
	UINT			m_uiShape;			// 0:circle, 1:square, 2~:symbol 
	UINT			m_uiSize;			// size 
	COLORREF		m_crColor;			// fill color 
	CString			m_strShape;			// symbol path/file name 
	BOOL			m_bTransparent;		// transparent 여부, true, false 
};

/** 도시요소 속성 클래스(선) */
class CPolylineProperty : public CMapProperty
{
	// attribute
public:
	COLORREF		m_crColor;			// line color 
	UINT			m_uiWidth;			// line width 
	UINT			m_uiLineStyle;		// 0:solid, 1:dash, 2:dot, 3:dashdot, 4:dashdotdot, 5~:pattern 
};

/** 도시요소 속성 클래스(면) */
class CPolygonProperty : public CMapProperty
{
	// attribute
public:
	UINT			m_uiType;			// 0:solid, 1:transparent, 2~:pattern
	UINT			m_uiWidth;			// line width 
	COLORREF		m_crFillColor;		// fill color 
	COLORREF		m_crLineColor;		// line color 
	CString			m_strPattern;		// pattern file path/name 
};

/** 벡터 지도 shape data */
struct stVObject
{
	CString			m_strFileName;								// 벡터지도 파일명
	CTypedPtrArray<CPtrArray, CMapPriority *> m_paMapPriority;	// 도시요소 우선순위 관리 클래스 pointer
	CMapProperty*	m_pMapProperty;								// 도시요소/Mapping 관리 클래스 pointer 
	shpObject*		m_pObject;									// 벡터지도 shape object pointer 
};

/** 벡터 지도 shape data Header */
struct stVectorMap
{
	int				m_iMapKey;							// "mapinfo.txt'파일 primary key 
	EGMapScale		m_enMapScale;						// 축척 
	CString			m_strRootPath;						// main path 
	CString			m_strSubPath1;						// sub path 1 
	CString			m_strSubPath2;						// sub path 2 
	CGRectGP		m_gpRect;							// 지도의 영역 
	BOOL			m_bIsVectorDataOpen;				// data is opened : true 
	CTypedPtrArray<CPtrArray, stVObject*> m_pVObject;	// 벡터 지도 shape data array 
	CTypedPtrArray<CPtrArray, stMapText*> m_pMapText;	// 벡터 지도 text data 
};

/** system font type */
struct stFontType
{
	CString		m_strFontType;		// 굴림 (한글) 
	CString		m_strName;			// 굴림 
	CString		m_strScript;		// 한글 
	BYTE		m_nCharSet;			// CharSet value 
	BYTE		m_nPitchAndFamily;	// PitchAndFamily value 
	DWORD		m_dwFlags;			// flag value 
};

/** text data priority */
struct stTextPriority
{
	EGMapScale		m_enMapScale;			// scale 
	CString			m_strWorldMapPoint;		// 세계지도(점) 구분자 
	BOOL			m_bIsVisible;			// text data visible flag, 1:보이기, 0:숨기기 
	UINT			m_uiVisibleType;		// 0:한글만보이기, 1:영문만보이기, 2:모두보이기 
	UINT			m_uiFontSize;			// 글자크기 
	COLORREF		m_clFontColor;			// 글자색 
	UINT			m_uiViewRatio;			// 보이기 배율 
	CString			m_strFontName;			// 글꼴1 
	CString			m_strFontScript;		// 글꼴2 
	stFontType*		m_pFontType;			// system font type 
};

/** 지형요소정보 data */
struct stFeatureInfo
{
	UINT			m_uiID;			// select ID 
	stVObject		*m_pObject;		// shape data header pointer
};

/** 세계 지도 대륙 영역 데이터 */
struct stWorldMapPriority
{
	UINT			m_uiSN;			// SN 
	CGRectGP		m_rectGP;		// 영역 rect 
	CString			m_strName;		// 대륙명 
};

/** 사용자 저장 지도 정보 */
struct stUserMapInfo
{
	CString			m_strFileName;									// 파일명 
	CString			m_strSaveTime;									// 저장시간 
	EGMapKind		m_eMapKind;										// 맵 종류, MK_VECTOR_AIR=>공도, MK_VECTOR_SEA=>해도, .... 
	EGMapScale		m_eMapScale;									// 맵 scale, MS_1_50000, MS_1_100000, .... 
	EGMapScale		m_eRasterMapScale;								// raster map scale 
	int				m_iMapRatio;									// 맵 배율, 100000=>1:100000, .... 
	UINT			m_uiMapMode;									// 적용 좌표 종류, GMM_GP=>평면좌표계, GMM_UTM=>UTM투영좌표계, .... 
	CGRectGP		m_gpSelRect;									// 도시영역 
	BOOL			m_bIsAutoMap;									// 자동축척 유/무 
	BOOL			m_bIsWorldMap;									// 세계지도 도시 유/무 
																	// 레스터 및 위성영상 지도 도시 
	UINT			m_uiMapType;									// 0:벡터, 1:레스터, 2:벡터+레스터
	CTypedPtrArray<CPtrArray, CMapPriority*> m_mappriority;			// 벡터 지도 우선순위 데이터 
	CTypedPtrArray<CPtrArray, stTextPriority*> m_pTextPriority;		// text data priority list 
	UINT			m_uiID;											// resource ID, 바로가기에서 사용 */
};

/** 레스터 지도 속성정보 */
struct stRasterMap
{
	int				m_iMapKey;				// "rastermapinfo.txt'파일 primary key 
	EGMapScale		m_enMapScale;			// 축척 
	CString			m_strRootPath;			// main path 
	CString			m_strSubPath;			// sub path 
	CString			m_strFileName;			// 파일명 
	CGRectGP		m_gpRect;				// 지도의 영역(검색용) 
	CGRectGP		m_gpRealRect;			// 지도의 영역(실제영역) 
	BOOL			m_bIsRasterDataOpen;	// data is opened : true 
	CDIBSectionLite	*m_pRasterMap;			// 레스터 지도 데이터 
};

/** 위성영상 지도 속성 정보 */
struct stSatelliteMapIndex
{
	int				m_iIndex;			// index key 
	int				m_iMapKey;			// map key 
	EGMapScale		m_enMapScale;		// 축척 
	CGRectGP		m_gpRect;			// 도시영역 
	CGRectGP		m_gpRealRect;		// 실제도시영역
	void			*m_pData;			// 메모리맵(가상메모리) 데이터 
};


struct s_flag_raster_view
{
	BOOL visible;
	BOOL visible_200;
	BOOL visible_100;
	BOOL visible_50;
	BOOL visible_25;
	BOOL visible_10;
	BOOL visible_5;
};


struct s_flag_satellite_view
{
	BOOL visible;
	BOOL visible_1m;
	BOOL visible_5m;
	BOOL visible_10m;
	BOOL visible_20m; 
};


static s_flag_raster_view m_flag_raster_view;			//	래스터 보여줄 축척
static s_flag_satellite_view m_flag_satellite_view;		//	위성영상 보여줄 축척

typedef struct s_layer_info
{
	int order;
	
	CString	id_layer; 
	CString	nm_layer;
	CString	nm_layer_kor;
	CString	nm_file; 
	CString	type_geo; 
	
	CString	type_map;
	CString	scale; 
	int	scale_min;
	int	scale_max;
	BOOL    visible;
	
	CString	point_size; 
	CString	point_image; 
	CString	 polyline_width;
	CString	 polyline_clr; 
	CString	 polyline_type;
	
	CString	 polyline_etc; 
	CString	 polygon_width; 
	CString	 polygon_type;
	CString	 polygon_clr_out; 
	CString	 polygon_clr_in;
	
	CString	 polygon_pattern_image; 
	BOOL	 polygon_is_pattern; 
	CString	 polygon_is_in; 
	
	BOOL	is_edit;
} S_LAYER_INFO;


typedef struct s_symbol_info
{ 
	CString	nm_symbol; 
	CString	nm_file;
	
} S_SYMBOL_INFO;


typedef struct s_pattern_info
{ 
	CString	nm_pattern; 
	CString	nm_file;
	
} S_PATTERN_INFO;


typedef struct s_map_info
{ 
	BOOL is_default_map;
	CString nm_usermap;
	
	BOOL bIsAutoMode;
	BOOL bIsWorldMapOpen;
	DOUBLE lMapMode;
	DOUBLE lMapScale;
	DOUBLE iMapRatio;
	DOUBLE iMapType;
	
	DOUBLE dleft;
	DOUBLE dtop;
	DOUBLE dright;
	DOUBLE dbottom;
	CString coord_center;
	DOUBLE dZoomFactor;
	CGRectGP gpSelRect;
	EGMapKind lMapKind;
	//LOGFONT lFont;
} S_MAP_INFO;

static S_MAP_INFO m_current_map_info;
static std::vector< S_MAP_INFO > mvec_user_map_info;


struct s_feature_value
{
	CString field;
	CString value;
};


typedef struct s_selected_feature
{ 
	CString nm_file_kor;
	CString nm_layer_kor;
	
	int id_selected_feature;
	int scale;
	CString type_geo;
	
	int idx_row;
	
	int cnt_vertax;
	int cnt_part;
	std::vector< CCoord_ > vec_vertax;
	std::vector< int > vec_part;
	
	std::vector< s_feature_value > vec_field_value;
	
} S_SELECTED_FEATURE;


static std::vector< S_SELECTED_FEATURE > mvec_selected_feature;

static BOOL m_is_identify;
static CPoint m_pt_selected_coord;

static MBR m_smallmapMBR;								// 배경지도 MBR value */
static BOOL m_bIsSelMarking;							// Marking draw flag */
static CGPointGP m_gpSelMarking;						// Marking draw point */
static stUserMapInfo *m_pCurBaseMapInfo;				// 기본지도 정보 포인터 */
static stUserMapInfo *m_pCurUserMapInfo;				// 사용자저장지도 정보 포인터 */
static BOOL m_bIsTiffRasterMapOpen;						// 레스터 지도파일 오픈여부 */
static BOOL m_bIsSmallMapOpen;							// 소축척지도파일 오픈 여부 */
static BOOL m_bIsSmallWorldMapOpen;						// 소축척세계지도파일 오픈 여부 */
static BOOL m_bIsWorldMapOpen;							// 세계지도파일 오픈여부 */
static POSITION cur_position, s_position, e_position;	// 현재, 시작, 끝 포지션 */

static CTypedPtrList<CPtrList, stSmallMap*> m_smallmaps;			// 소축척 지도 데이터 */
static CTypedPtrList<CPtrList, stSmallMap*> m_smallworldmaps;		// 세계 지도 데이터(소축척) */
static CTypedPtrList<CPtrList, stSmallMap*> m_worldmaps;			// 세계 지도 데이터 */
static CTypedPtrList<CPtrList, stSmallMap*> m_indexmaps[2];			// 인덱스 지도 데이터 */
static CTypedPtrList<CPtrList, stSmallMap*> m_namemaps;				// 지명 지도 데이터 */
static CTypedPtrList<CPtrList, stIndexData*> m_indexdata[2];		// 인덱스 리스트 데이터 */
static CTypedPtrList<CPtrList, stNameData*> m_namedata;				// 지명리스트 데이터 */
static CTypedPtrArray<CPtrArray, stNameData2*> m_namedata2;
static CTypedPtrArray<CPtrArray, stVectorMap*> m_vectormaps;			// 벡터 지도 데이터(full, 지도데이터는 empty) */
static CTypedPtrArray<CPtrArray, stVectorMap*> m_pCurrentMap;			// 100만 벡터 지도 데이터 리스트 */			
static CTypedPtrArray<CPtrArray, stVectorMap*> m_pCurrentMapList;		// 5,10,25,50만 벡터 지도 데이터 리스트 */		
static CTypedPtrArray<CPtrArray, CMapPriority*> m_mappriority;			// 벡터 지도 우선순위 데이터(user)*/			
static CTypedPtrArray<CPtrArray, CMapPriority*> m_defaultmappriority;	// 벡터 지도 우선순위 데이터(default)*/ 	
static CTypedPtrArray<CPtrArray, CMapPriority*> m_tmpmappriority;		// 벡터 지도 우선순위 데이터(temp) */		
static CTypedPtrArray<CPtrArray, stMapGroup*> m_pMapGroup;				// 지형요소관리 그룹리스트 */
static CTypedPtrArray<CPtrArray, stTextPriority*> m_pTextPriority;		// text data priority list(user) */
static CTypedPtrArray<CPtrArray, stTextPriority*> m_pDefaultTextPriority;	// text data priority list(default) */
static CTypedPtrArray<CPtrArray, stTextPriority*> m_tmpTextPriority;		// text data priority list(temp) */
static CTypedPtrArray <CPtrArray, stFontType*> m_pFontType;					// system font type list */
static CTypedPtrArray <CPtrArray, stSymbol*> m_pSymbolList;					// symbol list(point data) */
static CTypedPtrArray <CPtrArray, stSymbol*> m_pPatternList;				// pattern list(polygon data) */
static CTypedPtrArray <CPtrArray, stFeatureInfo*> m_pFeatureInfoList;		// 지형요소 list(임시buffer) */
static CTypedPtrArray<CPtrArray, stMapText*> m_pSmallMapText;				// 소축척지도 text data(인덱스맵에서만 사용) */
static CTypedPtrArray<CPtrArray, stWorldMapPriority*> m_pWorldMapPriority;	// 세계 지도 대륙 영역 리스트 */
static CTypedPtrArray<CPtrArray, stUserMapInfo*> m_pUserMapInfo;			// 사용자 저장 지도 정보 리스트 */
static CTypedPtrArray<CPtrArray, stRasterMap*> m_pRasterMap;				// 레스터 지도 데이터 리스트 */
static CTypedPtrArray<CPtrArray, stUserMapInfo*> m_paDirectGoMapInfo;		// 바로가기 저장 지도 정보 리스트 */
static CTypedPtrArray<CPtrArray, stSatelliteMapIndex*> m_paSatelliteMapIndex;// 위성영상 지돟 데이터(가상메모리이용) */
/** 상태바 다이얼로그 */
//static CProgressDlg* m_pProgressDlg;


static CWnd* m_pMainWnd;				// 메인 위도우 포인터 */
static stSymbol* m_pSelPointSymbol;		// 지도 중심점에 사용되는 심볼 포인터 */
static EGMapScale m_enCurMapScale;		// 현재 도시된 지도 축척 */
static double m_dViewRatio;				// 지도 배율 */
static BOOL m_bIsProgressDlgView;		// 상태바 도시 여부 */
static long m_lVectorMapMode;			// 벡터지도 맵모드 */
static HANDLE m_hFMap;					// 메모리맵에서 사용되는 파일 핸들러 */
static int m_iSatelliteMapInfoCnt;		// 위성영상 지도 영역 정보 갯수 */

static LOGFONT m_label_font;/** 폰트정보 */


/**
* 지도 및 속성정보 관리 class
* @author 박종원
* @version 1.0
* @since 2005.11.
**/
class CMapDataMgr
{
public:
	CMapDataMgr(CWnd *pWnd);
	virtual ~CMapDataMgr();
	
	// attribute
protected:
	
	/** vector map file open queue(면,선,점) */
	//	safeQueue m_opendata[3];
	/** vector map draw queue(면,선,점) */
	//	safeQueue m_drawdata[3];
	/** 벡터 지도 draw 데이터 리스트(면,선,점) */
	//	CTypedPtrArray<CPtrArray, stVectorMap*> m_paVectorDrawData[3];
	/** 벡터 지도 데이터 처리 thread */
	//	CWinThread* m_pVectorThread[3];
	
//public:
//    enum {STOPPED=0, RUNNING} m_enumThreadStatus[3];
//	CEvent m_eventAllowParentDie[3];
//	CEvent m_eventKillThread[3];
	
	// operation
public:

	static void worldmapShpDraw(CDC* pDC, double dZoomFactor, int iViewType);
	
	static void set_flag_raster_view( s_flag_raster_view flag_view );
	static s_flag_raster_view get_flag_raster_view();
	
	static void set_flag_satellite_view( s_flag_satellite_view flag_view );
	static s_flag_satellite_view get_flag_satellite_view();
	
	// 소축척지도 관련
	static void ClearSmallMap();															// 소축척관련 지도 데이터 clear */
	static void smallMapOpen();																// 소축척지도 열기 */
	static bool Open(enSmallMapType enType, CString _filename);								// 소축척지도 열기 */
	static stSmallMap* GetSmallMapHead();													// 소축척지도 데이터 얻기 */
	static stSmallMap* GetSmallMapNext();													// 소축척지도 데이터 얻기 */
	static stIndexData* GetIndexDataHead(enSmallMapType enType);							// 인덱스/지명 지도 데이터 얻기 */
	static stIndexData* GetIndexDataNext(enSmallMapType enType);							// 인덱스/지명 지도 데이터 얻기 */
	static stNameData* GetNameDataHead();													// 인덱스/지명 지도 데이터 얻기 */
	static stNameData* GetNameDataNext();													// 인덱스/지명 지도 데이터 얻기 */
	static void InitIndexMapData();															// 인덱스 지도 데이터 초기화 */
	static void AddIndexMap(enSmallMapType enType, stIndexData* pItem, BOOL bIsHead=FALSE);	// 인덱스 지도 데이터 추가 */
	static void InitNameMapData();															// 지명 지도 데이터 초기화 */
	static void AddNameMap(stNameData* pItem, BOOL bIsHead=FALSE);							// 지명 지도 데이터 추가 */
	static int GetSmallMapCount();															// 소축척지도 레코드 갯수 얻기 */
	static int GetIndexDataCount(enSmallMapType enType);									// 인덱스지도 레코드 갯수 얻기 */
	static int GetNameDataCount();															// 지명지도 레코드 갯수 얻기 */
	static void smallworldMapOpen();														// 세계지도 데이터(소축척) 열기*/
	static bool smallworldOpen(CString _filename);											// 세계지도 데이터(소축척) 열기*/
	static stSmallMap* GetSmallWorldMapHead();												// 세계지도 데이터 얻기*/
	static stSmallMap* GetSmallWorldMapNext();												// 세계지도 데이터 얻기*/
	static void ClearSmallWorldMap();														// 세계지도 데이터 clear*/
	static void AddSmallWorldMapPriority(UINT uiSN, long left, long top, long right, long bottom, CString strName);/** 세계지도 대륙정보 추가*/
	static int GetSmallWorldMapPriorityCnt();/** 세계지도 대륙정보 레코드 개수 얻기*/
	static stWorldMapPriority* GetSmallWorldMapPriority(int iIndex);/** 세계지도 대륙정보 얻기*/
	static void worldMapOpen();/** 세계지도 데이터 열기 */
	static bool worldOpen(CString _filename);/** 세계지도 데이터 열기 */
	static stSmallMap* GetWorldMapHead();/** 세계지도 데이터 얻기 */
	static stSmallMap* GetWorldMapNext();/** 세계지도 데이터 얻기 */
	static void ClearWorldMap();/** 세계지도 데이터 clear */
	static BOOL IsWorldMapOpen();/** 세계지도 데이터 open 여부 */
	
	// 벡터 지도 데이터 관련
	/** 벡터 지도 데이터 관리 리스트 추가 (insert sort) */
	static BOOL AddVectorMapInfo(int iMapKey, CString strScale, CString strPath, CString strSub1, CString strSub2,
		double dMinX, double dMinY, double dMaxX, double dMaxY);
	/** 벡터 지도 데이터 관리 리스트 삭제 */
	static void ClearVectorMapList();
	/** 벡터 지도 데이터 헤더 찾기 */
	static stVectorMap* FindVectorMapInfo(int iMapKey);
	/** 지도 속성 정보 추가(점) */
	static BOOL AddMapProperty(int iMapKey, CString strScale, CString strFileName, UINT uiKind, BOOL bIsVisible,
		UINT uiShape, UINT uiSize, COLORREF crColor, CString strShape, BOOL bTransparent);
	/** 지도 속성 정보 추가(선) */
	static BOOL AddMapProperty(int iMapKey, CString strScale, CString strFileName, UINT uiKind, BOOL bIsVisible,
		COLORREF crColor, UINT uiWidth, UINT uiLineStyle);
	/** 지도 속성 정보 추가(면) */
	static BOOL AddMapProperty(int iMapKey, CString strScale, CString strFileName, UINT uiKind, BOOL bIsVisible,
		UINT uiType, UINT uiWidth, COLORREF crFillColor, COLORREF crLineColor, CString strPattern);
	/** 벡터 지도 shape data array에 아이템 추가 (insert sort) */
	static BOOL AddVObject(CString strFileName, stVectorMap* pVectorMap, CMapProperty* pMapProperty);
	/** 지도 속성 정보 리스트 삭제 */
	static void ClearMapProperty();
	/** 벡터 지도 데이터 Default Load(100만) */
	static BOOL OpenDefaultVectorMap();
	/** 벡터 지도 데이터 clear(100만 제외) */
	static BOOL ClearVectorMap(CGPointGP gpoint);
	/** 벡터 지도 데이터 열기 */
	static BOOL OpenVectorMap(stVectorMap* pItem);
	/** 벡터 지도 텍스트 데이터 열기 */
	static void OpenMapText(stVectorMap* pItem);
	/** 복원도시 대상 데이터 추출 */
	static BOOL ExtractVectorMap(EGMapScale enScale, CGRectGP rect, double dViewRatio);
	/** 영역 중복 체크 */
	static BOOL DupCheck(EGMapScale enScale, CGRectGP rect, CGRectGP selrect);
	/** 영역 중복 체크(레스터,위성영상사용) */
	static BOOL DupCheck2(CGRectGP rect, CGRectGP selrect, EGMapScale scale);
	/** 벡터 지도 데이터 레코드 갯수 얻기 */
	static int GetVectorMapCount();
	/** 벡터 지도 데이터 얻기 */
	static stVectorMap * GetVectorMapItem(int iIndex);
	/** 도시요소 우선순위 정보 추가(점) */
	static void AddMapPriority(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
								BOOL bIsVisible, CString strName, 
								CString earth_or_air, CString type_geo,
								UINT uiRadius, COLORREF crFillColor, 
								UINT uiType, CString strBitmap, int iViewRatio);
	
	//static void AddMapPriority(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
	//							BOOL bIsVisible, CString strName, UINT uiRadius, COLORREF crFillColor, 
	//							UINT uiType, CString strBitmap, int iViewRatio);
	/** 도시요소 우선순위 정보 추가(선) */
	static void AddMapPriority2(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
								BOOL bIsVisible, CString strName, 
								CString earth_or_air, CString type_geo,
								UINT uiLineWidth, COLORREF crLineColor, UINT uiType,
								UINT uiLineWidth2, COLORREF crLineColor2, UINT uiType2, int iViewRatio);
	
	//static void AddMapPriority2(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
	//							BOOL bIsVisible, CString strName, UINT uiLineWidth, COLORREF crLineColor, UINT uiType,
	//							UINT uiLineWidth2, COLORREF crLineColor2, UINT uiType2, int iViewRatio);
	/** 도시요소 우선순위 정보 추가(면) */
	static void AddMapPriority3(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
								BOOL bIsVisible, CString strName, 
								CString earth_or_air, CString type_geo,
								UINT uiLineWidth, COLORREF crLineColor, 
								COLORREF crFillColor, UINT uiType, CString strBitmap, int iViewRatio);
	//static void AddMapPriority3(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
	//							BOOL bIsVisible, CString strName, UINT uiLineWidth, COLORREF crLineColor, 
	//							COLORREF crFillColor, UINT uiType, CString strBitmap, int iViewRatio);
	
	/** 도시요소 우선순위 정보 찾기 */
	static CMapPriority* FindMapPriority(EGMapScale enScale, CString strFileName);
	/** 도시요소 우선순위 정보 레코드 개수 얻기 */
	static int GetMapPriorityCnt();
	/** 도시요소 우선순위 정보 얻기 */
	static CMapPriority* GetMapPriority(int iIndex);
	/** 도시요소 우선순위 정보 clear */
	static void ClearMapPriority();
	static void ClearMapPriority(stUserMapInfo *pUserMapInfo);
	/** 도시요소 우선순위 정보 추가 */
	static void AddMapPriority(CMapPriority* pItem);
	/** 도시요소 우선순위 정보 갱신 */
	static void UpdateMapPriority(CMapPriority* pItem);
	/** 도시요소 text data 정보 갱신 */
	static void UpdateTextPriority(stTextPriority* pItem);
	/** 도시요소 default 우선순위 정보 추가(점) */
	static void AddDefaultMapPriority(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
		BOOL bIsVisible, CString strName, UINT uiRadius, COLORREF crFillColor, 
		UINT uiType, CString strBitmap, int iViewRatio);
	/** 도시요소 default 우선순위 정보 추가(선) */
	static void AddDefaultMapPriority2(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
								BOOL bIsVisible, CString strName, UINT uiLineWidth, COLORREF crLineColor, UINT uiType,
								UINT uiLineWidth2, COLORREF crLineColor2, UINT uiType2, int iViewRatio);
	/** 도시요소 default 우선순위 정보 추가(면) */
	static void AddDefaultMapPriority3(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
								BOOL bIsVisible, CString strName, UINT uiLineWidth, COLORREF crLineColor, 
								COLORREF crFillColor, UINT uiType, CString strBitmap, int iViewRatio);
	/** 도시요소 default 우선순위 정보 찾기 */
	static CMapPriority* FindDefaultMapPriority(EGMapScale enScale, CString strFileName);
	/** 도시요소 default 우선순위 정보 레코드 개수 얻기 */
	static int GetDefaultMapPriorityCnt();
	/** 도시요소 default 우선순위 정보 얻기기 */
	static CMapPriority* GetDefaultMapPriority(int iIndex);
	/** 맵 기본정보 리스트("mapproperty.txt") 자동 구성 */
	static void CreateDefaultMapProperty(CString strScale);
	/** map scale converting */
	static CString ConvertMapScale(EGMapScale enScale);
	/** map scale converting */
	static EGMapScale ConvertMapScale(CString strScale);
	/** 벡터 지도 찾기 */
	static stVectorMap* FindVectorMap(EGMapScale enScale);
	/** 현재 도시된 벡터 지도 찾기 */
	static BOOL FindCurrentMap(EGMapScale enScale);
	/** 벡터 지도 reset */
	static void ResetVectorMap(stVectorMap* pItem);
	/** 현재 도시된 벡터 지도 reset */
	static void ResetCurrentMap(EGMapScale enScale, CGRectGP rect);
	/** 현재 도시된 벡터 지도 reset */
	static void ResetCurrentMap();
	/** 현재 도시될 벡터 지도 추가 */
	static void AddCurrentMap(EGMapScale enScale, CGRectGP rect);
	/** 현재 도시된 벡터 지도 갱신 */
	static void UpdateCurrentMap();
	/** 현재 도시된 벡터 지도(5,10,25,50만) 레코드 개수 얻기 */
	static int GetCurrentMapListCnt();
	/** 현재 도시된 벡터 지도(5,10,25,50만) 얻기 */
	static stVectorMap* GetCurrentMapList(int iIndex);
	/** 현재 도시된 벡터 지도(100만) 레코드 개수 얻기 */
	static int GetCurrentMapCnt();
	/** 현재 도시된 벡터 지도(100만) 얻기 */
	static stVectorMap* GetCurrentMap(int iIndex);
	/** 지도데이터 min, max 영역 추출 함수 */
	static void UpdateMapArea();
	/** 지도데이터 min, max 영역 추출 함수 */
	static BOOL OpenVectorMap2(stVectorMap* pItem);
	/** 지형요소 그룹 데이터 추가 */
	static void AddMapGroup(CString strID, CString strName);
	/** 지형요소 그룹 데이터 레코드 개수 얻기 */
	static int GetMapGroupCnt();
	/** 지형요소 그룹 데이터 얻기 */
	static stMapGroup* GetMapGroup(int iIndex);
	/** 지형요소 그룹 데이터(sub) 추가 */
	static void AddMapGroupItem(CString strGroupID, CString strID, CString strName);
	/** text 속성 데이터 추가 */
	static void AddTextPriority(CString strScale, BOOL bIsVisible, UINT uiVisibleType, UINT uiFontSize, 
								COLORREF clFontColor, UINT uiViewRatio, CString strFontName, CString strFontScript);
	/** text 속성 데이터 레코드 개수 얻기 */
	static int GetTextPriorityCnt();
	/** text 속성 데이터 얻기 */
	static stTextPriority* GetTextPriority(int iIndex);
	/** text 속성 데이터(default) 추가 */
	static void AddDefaultTextPriority(CString strScale, BOOL bIsVisible, UINT uiVisibleType, UINT uiFontSize, 
		COLORREF clFontColor, UINT uiViewRatio, CString strFontName, CString strFontScript);
	/** text 속성 데이터(default) 레코드 개수 얻기 */
	static int GetDefaultTextPriorityCnt();
	/** text 속성 데이터(default) 얻기 */
	static stTextPriority* GetDefaultTextPriority(int iIndex);
	/** system font 정보 추가 */
	static void AddSystemFontType(ENUMLOGFONT* pelf, DWORD dwType, LPCTSTR lpszScript);
	/** system font 정보 레코드 개수 얻기 */
	static int GetFontTypeCnt();
	/** system font 정보 레코드 얻기 */
	static stFontType* GetFontType(int iIndex);
	/** system font 정보 갱신 */
	static void UpdateFontType(stFontType* pFontType);
	/** system font 정보(default) 갱신 */
	static void UpdateDefaultFontType(stFontType* pFontType);
	
	// 사용자 저장 지도 관련
	/** 사용자 저장지도 열기 */
	static BOOL OpenUserMapInfo();
	/** 사용자 저장지도 추가 */
	static BOOL AddUserMapInfo(CString strTime, CString strRootPath, CString strFileName, 
								EGMapKind eMapKind, EGMapScale eMapScale, int iMapRatio, 
								UINT uiMapMode, CGRectGP gpSelRect, BOOL bIsAutoMap, int iMapType);
	/** 사용자 저장지도 레코드 개수 얻기 */
	static int GetUserMapInfoCnt();
	/** 사용자 저장지도 얻기 */
	static stUserMapInfo* GetUserMapInfo(int iIndex);
	/** 사용자 저장지도 clear */
	static void ClearUserMapInfo();
	/** 사용자 저장지도 파일 열기 및 저장 */
	static void Serialize(int iMapType, CArchive &ar);	// iMapType -> 0:base map, 1:user map, 2:directgo map
	/** 사용자 저장지도 설정 */
	static void SetUserMapInfo(stUserMapInfo *pUserMapInfo);
	/** 사용자 저장지도에서 사용되는 지도속성 정보 교체 */
	static BOOL SwapMapPriority(BOOL bIsUserMap);
	/** 기본지도 저장 */
	static BOOL SaveBaseMapInfo(CArchive &ar, BOOL bIsAutoMode, EGMapKind eMapKind, EGMapScale eMapScale, 
		int iMapRatio, long lMapMode, double dleft, double dtop, double dright, double dbottom, 
		long lMapScale, BOOL bIsWorldMapOpen, int iMapType);
	/** 기본지도 열기 */
	static BOOL	OpenBaseMapInfo(CArchive &ar);
	/** 기본지도 얻기 */
	static stUserMapInfo* GetBaseMapInfo();
	/** 사용자 저장지도 삭제 */
	static BOOL DeleteUserMapInfo(CString strFile);
	
	// 레스터 지도 관련
	/** 레스터 지도 속성데이터 추가 */
	static void AddRasterMapInfo(UINT uiSN, int iScale, CString strSubPath, CString strName, double left, double top, double right, double bottom);
	/** 레스터지도 도시 대상 데이터 추출 */
	static BOOL ExtractRasterMap(BOOL bIsAutoMode, UINT uiRasterMapKind, CGRectGP rect, int iMapType, int iZoomRatio);
	/** 레스터맵 스케일 체크 */
	static BOOL IsLoadRasterMap(BOOL bIsAutoMode, int iRatio, stRasterMap *pRasterMap);
	/** 레스터 지도 열기 */
	static BOOL OpenRasterMap(stRasterMap *pRasterMap, CGRectGP rect);
	/** 레스터 지도 열기 NITF("TL2") format */
	static BOOL OpenNitf2Map(stRasterMap *pRasterMap, CGRectGP rect);
	/** 레스터 지도 삭제 */
	static void DeleteRasterMap(stRasterMap *pRasterMap);
	/** 레스터 지도 레코드 개수 얻기 */
	static int GetRasterMapCount();
	/** 레스터 지도 레코드 얻기 */
	static stRasterMap * GetRasterMap(int iIndex);
	/** 레스터 지도 도시 */
	static void DrawRasterMap(CGDCGeo *pDC, stRasterMap *pRasterMap);
	static void DrawRasterMap(CDC *pDC, stRasterMap *pRasterMap);
	static void DrawRaster(CDC *pDC, stRasterMap *pRasterMap, int nScreenWidth, int nScreenHeight,
								 float fCenterX, float fCenterY, float fScale);		/// 추가
	/** 레스터 지도 모두 지우기 */
	static void RemoveAllRasterMap();
	/** UTM좌표를 경위도 좌표로 변환 한반도 52 zone을 기준으로 변환 */
	static CGRectGP ConvertUTMtoGP(double left, double top, double right, double bottom);
	
	// 위성영상 지도 관련
	/** 위성영상 지도 도시 대상 데이터 추출 */
	static BOOL ExtractSatelliteMap(BOOL bIsAutoMode, CGRectGP rect, int iMapType, int iZoomRatio);
	/** 위성영상 스케일 체크 */
	static BOOL IsLoadSatelliteMap(BOOL bIsAutoMode, int iRatio, stSatelliteMapIndex *pIndex);
	/** 위성영상 지도 열기(10m,20m) */
	static BOOL OpenSatelliteMap(stSatelliteMapIndex *pIndex, CGRectGP rect);
	/** 위성영상 지도 열기(1m) */
	static BOOL OpenSatelliteMap1m(stSatelliteMapIndex *pIndex, CGRectGP rect);
	/** 위성영상 지도 도시 */
	static void DrawSatelliteMap(CGDCGeo *pDC, stSatelliteMapIndex *pIndex);
	static void DrawSatelliteMap(CDC *pDC, stSatelliteMapIndex *pIndex);

	/** 위성영상 지도 모두 지우기 */
	static void RemoveAllSatelliteMap();
	/** 위성영상 지도 모두 지우기 */
	static void RemoveAllSatelliteMapData();
	
	// symbol/pattern 관리
	/** 심볼 데이터 열기 */
	static void OpenSymbolData();
	/** 심볼 데이터 레코드 개수 얻기 */
	static int GetSymbolListCnt();
	/** 심볼 데이터 얻기 */
	static stSymbol* GetSymbol(int iIndex);
	/** 패턴 데이터 열기 */
	static void OpenPatternData();
	/** 패턴 데이터 레코드 개수 얻기 */
	static int GetPatternListCnt();
	/** 패턴 데이터 얻기 */
	static stSymbol* GetPattern(int iIndex);
	/** 지형요소 정보 레코드 개수 얻기 */
	static int GetFeatureInfoListCnt();
	/** 지형요소 정보 얻기 */
	static stFeatureInfo* GetFeatureInfo(int iIndex);
	/** 지형요소 정보 보이기 */
	static BOOL InfoShow(CGDCGeo* pDC, long lMapScale, GPoint point, CGRectGP* rectGP);
	/** 지형요소 정보(temp) 찾기 */
	static stFeatureInfo * FindFeatureInfo(UINT uiID);
	/** 지형요소 정보 삭제 */
	static void DeleteAllFeatureInfoList();
	/** 소축척지도 text data 열기 */
	static BOOL OpenSmallMapText();
	/** 소축척지도 text data 레코드 개수 얻기 */
	static int GetSmallMapTextCnt();
	/** 소축척지도 text data 얻기 */
	static stMapText* GetSmallMapText(int iIndex);
	/** 파일명 중복 체크 */
	static BOOL DupChkFileName(CString strRootPath, CString strFileName);
	/** sel point symbol get */
	static stSymbol* GetSelPointSymbol();
	/** Marking flag set */
	static void SetSelMarkingFlag(BOOL bIsSelMarking, CGPointGP gpoint);
	/** Marking flag value get */
	static BOOL GetSelMarkingFlag(CGPointGP &gpoint);
	
	// 기타
	/** parent window pointer set */ 
	static void SetMainWnd(CWnd* pWnd){m_pMainWnd = pWnd;};
	/** ProgressDlg Hide */
	static void ProgressDlgHide();
	/** small map MBR value get*/
	static CGRectGP GetSmallMapRect();
	/** write mappriority file */
	static void WriteMapPriority(CFile &file, CMapPriority *pPriority);
	/** write textpriority file */
	static void WriteTextPriority(CFile &file, stTextPriority *pPriority);
	/** map area calc */
	static CGRectGP CalcMapSelArea(CGDCGeo *pDC, CSize size, CGPointGP gpoint);
	/** LT point clac */
	static CGPointGP CalcMapLTPoint(CGDCGeo *pDC, CSize size, CGRectGP &rectGP);
	/** GeoTiff format map data open, rect value input */
	static BOOL OpenGeoTiffData(CString strFileName, double &left, double &right, double &top, double &bottom);
	/** Nitf format map data open, rect value input */
	static BOOL OpenNitfData(CString strExt, CString strFileName, double &left, double &right, double &top, double &bottom);
	/** raster map 종류 get */
	static UINT GetRasterMapKind(BOOL bIsAutoMode, int iRatio);
	/** 현재 map scale get */
	static EGMapScale GetCurrentMapScale();
	/** 현재 map scale set */
	static void SetCurrentMapScale(stUserMapInfo *pUserMapInfo);	
	/** 벡터 지도 draw 데이터 리스트 clear */
	void InitVectorDrawData();
	/** 레스터 지도 검색 */
	static stRasterMap * FindRasterMap(EGMapScale enScale);
	/** 위성영상 지도 검색 */
	static stSatelliteMapIndex * FindSatelliteMap(EGMapScale enScale);
	/** 벡터지도 영역 탐색 */
	static void GetVectorMapRect(CStringList &slPath, double &left, double &top, double &right, double &bottom);
	/** 레스터지도 영역 탐색 */
	static void GetRasterMapRect(CStringList &slPath, double &left, double &top, double &right, double &bottom);
	/** 위성영상지도 영역 탐색 */
	static void GetSatelliteMapRect(CStringList &slPath, double &left, double &top, double &right, double &bottom);
	/** 축척 변수 값 변환 */
	static CString EGMapScaleToString(EGMapScale scale);
	/** 위성영상지도 영역정보 설정 */
	static void SetSatelliteMapInfo(HANDLE hFMap);
	/** 위성영상지도 영역정보 탐색 */
	static BOOL GetSatelliteMapInfo(stSatelliteMapIndex *pIndexCString, CString &strRootPath, CString &strSubPath, CString &strName);
	/** 위성영상지도 영역정보 갯수 얻기 */
	static int GetSatelliteMapIndexCount();
	/** 위성영상지도 영역정보 탐색 */
	static stSatelliteMapIndex * GetSatelliteMapIndex(int iIndex);
	/** 위성영상지도 영역정보 재생성 */
	static BOOL OnReGeneratorSatelliteMapInfo(int iTotCount);
	
	/** 바로가기 지도정보 열기 */
	static BOOL OpenDirectGoMapInfo();
	/** 바로가기 지도정보 추가 */
	static BOOL AddDirectGoMapInfo(UINT uiID, int iIndex, CString strTime, CString strRootPath, EGMapKind eMapKind, 
		EGMapScale eMapScale, int iMapRatio, UINT uiMapMode, 
		CGRectGP gpSelRect, BOOL bIsAutoMap, int iMapType);
	/** 바로가기 저장지도 리스트 삭제 */
	static void ClearDirectGoMapInfo();
	/** 바로가기 저장지도 아이템 갯수 얻기 */
	static int GetDirectGoMapInfoCnt();
	/** 바로가기 저장지도 아이템 얻기 */
	static stUserMapInfo * GetDirectGoMapInfo(int iIndex);
	/** 바로가기 저장지도 아이템 찾기 */
	static stUserMapInfo * FindDirectGoMapInfo(UINT uiID);
	/** 벡터지도 맵모드 변경 */
	static void SetVectorMapMode(long lVectorMapMode);
	/** 벡터지도 맵모드 얻기 */
	static long GetVectorMapMode();
	
	/** 색도조절 */
	static double HuetoRGB(double m1, double m2, double h);
	static COLORREF HSLtoRGB(double H, double S, double L);
	static void RGBtoHSL(COLORREF rgb, double *H, double *S, double *L);
	static void SetChromView(int iHSLSet, int iType, double H, double S, double L);
	static COLORREF SetHSLColor(COLORREF color, double H, double S, double L, int type);
	static COLORREF SetHSLColor(int mode, COLORREF color, double H, double S, double L);
	static void SetChromLineColor(int iHSLSet, double H, double S, double L);
	static void SetChromFillColor(int iHSLSet, double H, double S, double L);
	static void SetChromFontColor(int iHSLSet, double H, double S, double L);
	static void AddNameMap2(int iSN, CString strName, double dX, double dY);
	static int GetNameDataCount2();
	static stNameData2 * GetNameData2(int iIndex);
	
	
	
	
	
	static void add_order_info( int order, CString nm_file, short scale );	 
	static short get_order( short scale, CString nm_file );
	
	typedef std::vector< CString >::iterator itr_order2;
	static BOOL get_order_info( short scale,  itr_order2 &itr_begin, itr_order2 &itr_end );
	static int get_cnt_order( short scale );
	static void add_vector_layer_info( S_LAYER_INFO & layer_info, short scale );
	static BOOL get_vector_lyaer_info( CString nm_layer, short scale, S_LAYER_INFO & layer_info );
	static void set_visibie_vector_layer( int order, int scale, BOOL visible );
	static BOOL get_visibie_vector_layer( int scale, CString nm_file );
	
	
	typedef std::vector< S_SYMBOL_INFO >::iterator itr_symbol_info;
	static void get_symbol_info( itr_symbol_info &itr_begin, itr_symbol_info &itr_end );
	static void add_symbol_info( CString & nm_symbol, CString & nm_file );
	static CString get_nm_file( int order, int scale );
	static void update_lyaer_info( CString nm_layer, short scale, S_LAYER_INFO & layer_info );
	static BOOL is_edit_layer_info( int scale, CString nm_file );
	static void done_edit_layer_info( int scale, CString nm_file );
	
	typedef std::vector< S_PATTERN_INFO >::iterator itr_pattern_info;
	static void add_pattern_info( CString & nm_pattern, CString & nm_file );
	static void get_pattern_info( itr_pattern_info &itr_begin, itr_pattern_info &itr_end );
	static S_MAP_INFO get_current_map_info();	
	static void add_user_map_info( S_MAP_INFO & map_info );
	
	typedef std::vector< S_MAP_INFO >::iterator itr_user_map_info;
	static void get_user_map_info( itr_user_map_info &itr_begin, itr_user_map_info &itr_end );
	static void save_current_map_info_on_mdb( CString nm_user_map );
	static void save_current_map_info_on_mem( S_MAP_INFO & map_info );
	static void delete_user_map_info_on_mdb( CString nm_user_map );
	static void save_user_map_attri_on_mdb( S_LAYER_INFO & layer_info );
	static void save_user_map_attri_font_on_mdb(CString nm_user_map);
	static void delete_vector_layer_info();
	static void load_user_map( CString nm_user_map );
	
	
	typedef std::vector< S_SELECTED_FEATURE >::iterator itr_feature;
	static void get_feature( itr_feature &itr_begin, itr_feature &itr_end );
	static void add_feature( S_SELECTED_FEATURE & feature );
	static void delete_feature();
	static int get_feature_cnt();
	static void set_identify( BOOL identify );
	static BOOL is_identify();
	
	static void set_identify_coord( CPoint pt_coord );
	static CPoint get_identify_coord();	
	
	static void set_dbf_font(LOGFONT font);
	static LOGFONT get_dbf_font();	
	static void set_current_map_kind(EGMapKind eMapKind);

	
};


#endif // !defined(AFX_MAPDATAMGR_H__8A792912_E83B_4C1E_9BC2_1673AEF72B7C__INCLUDED_)
