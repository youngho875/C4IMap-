// GMapInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGMapInfo document
#ifndef _GMapInfo_h
#define _GMapInfo_h

//#include "../../IncludeSgisUtil.h"
#include "GStatusFlag.h"
#include "GColorData.h"

#define	READ_SUCCESS		0
#define	READ_FAIL			-1

#define MAP_DETAIL_VIEW		0		// 지도 상세 보기
#define MAP_NORMAL_VIEW		1		// 지도 표준 보기
#define MAP_ROUGH_VIEW		2		// 지도 간략 보기

#define	MK_IS_RASTER(kind) \
		( (kind)>=MK_RASTER_AIR && (kind)<=MK_RASTER_PORT)
#define	MK_IS_VECTOR(kind) \
		( (kind)>=MK_VECTOR_YODO && (kind)<=MK_VECTOR_WORLD)

// 지도 종류
typedef enum { 
	MK_NOTHING     =	-1,		// 지정되지 않음
	MK_VECTOR_LAND =	0 ,		// 벡터 육도
	MK_RASTER_AIR  =	1,		// 레스터 공도
	MK_VR_LAND     =	2,		// 벡터+래스터
	MK_VECTOR_AIR  =	3 ,		// 벡터 공도
	MK_VECTOR_SEA  =	4,		// 벡터 해도
} EGMapKind;


typedef enum {
	MS_NOTHING = -1,	// 지정되지 않음
	MS_1_25000 = 0,		// 1:2.5만(공도에서 사용)
	MS_1_50000,			// 1:5만
	MS_1_100000,		// 1:10만
	MS_1_250000,		// 1:25만
	MS_1_500000,		// 1:50만
	MS_1_1000000,		// 1:100만
	MS_1_2000000,		// 1:200만
	MS_1_1,				// 1m급 위성영상
	MS_1_10,			// 10m급 위성영상
	MS_1_20,			// 20m급 위성영상
	MS_ERROR,			// error
} EGMapScale;

// 3차원지도 도시형태
typedef	enum {
	G3D_WIRE = 0,		// 그물망
	G3D_SOLID = 1,		// 색상도
	G3D_SHADOW = 2,		// 음영도
	G3D_MAPPING = 3,	// 영상중첩
} EGM3DType;

/**
* 지도정보 item class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class CGMapSetInfo {
public:
	EGMapKind	m_nMapKind;
	EGMapScale	m_msMapScale;

	CString		m_strBasePath;
	CString		m_strLibName;

	CGColorData		m_Color;		// 색상정보
	CGStatusFlag	m_Feature;		// Feature On/Off
	CGStatusFlag	m_Cvg;			// 커버리지 On/Off
	CGStatusFlag	m_Revert;		//
	CGStatusFlag	m_Revertable;	//
public:
	const CGMapSetInfo &operator=(const CGMapSetInfo &right);	
};

//  좌표계
typedef enum{
	CT_NOTHING,		// 지정되지 않음
	CT_GP,			// 경위도
	CT_GEOREF,		// GEOREF
	CT_MGRS,		// MGRS
	CT_UTM			// UTM
} EGCoordType;

typedef CTypedPtrList<CPtrList, CGMapSetInfo *> MapInfoList;

class CGDCGeo;

/**
* 지도 정보 관리 class
* @author 유영호
* @version 1.0
* @since 2024.07.
**/
class CGMapInfo
{
public:
	CGMapInfo();
	virtual ~CGMapInfo();

//MapViewInfo에 대한 부분......................................................................
	//CGMap3D*	m_pMap3d; //3차원 도시관련

public:
	void		SetTerrainCheck(BOOL bTerrain);
	void		SetRecovery(BOOL bIs);
	BOOL		GetTerrainCheck();
	BOOL		GetRecovery();
	void		SetAlignRect(CGRectGP alignRect);
	CGRectGP	GetAlignRect();
//	inline		CGMap3D*	Get3DObj(){ return m_pMap3d; };
	
	CGRectGP	m_AlignRect;	// Align한 rect

protected:
	BOOL		m_bSimple;		// 단순도시(TRUE), 복원도시(FALSE)
	BOOL		m_bTerrainChk;	// 지형중첩 설정
//End of MapViewInfo.............................................................................

public:
	void	InitInstance();	// 초기화
	BOOL	IsValid();		// 지도 자료가 제대로 입력되어 있으면
							// TRUE를 리턴.
	void	Serialize(CArchive& ar);

	EGMapScale	GetMapScale();
	void		SetMapScale(EGMapScale mapScale);
	long		GetMapScaleValue();
	void		SetMapScaleValue(long nScale);
	inline EGCoordType	GetCoordType() { return m_ctCoordType; };
	inline void		SetCoordType(EGCoordType coordType) { m_ctCoordType = coordType; };
	inline EGMapKind GetMapKind() { return m_nMapKind; };
	BOOL			SetMapKind(EGMapKind mapKind);
	CString			GetBasePath();
	void			SetBasePath(CString basePath);
	inline CString	GetLibName() { return m_strLibName; };
	inline void		SetLibName(CString libName) { m_strLibName = libName; };

	inline CGColorData	*GetColorData() { return &m_Color; };
	inline CGStatusFlag	*GetFeatureStatus() { return &m_Feature; };
	inline CGStatusFlag	*GetCvgStatus() { return &m_Cvg; };
	inline CGStatusFlag	*GetRevertStatus() { return &m_Revert; };
	inline CGStatusFlag	*GetRevertableStatus() { return &m_Revertable; };

	inline void		SetColorData(CGColorData color) { m_Color = color; };
	inline void		SetCvgStatus(CGStatusFlag cvg) { m_Cvg= cvg; };
	inline void		SetFeatureStatus(CGStatusFlag feature) { m_Feature = feature; };
	inline void		SetRevertStatus(CGStatusFlag revert) { m_Revert = revert; };
	inline void		SetRevertableStatus(CGStatusFlag revertable) { m_Revertable = revertable; };

	const CGMapInfo &operator=(const CGMapInfo &right);

	inline BOOL		IsDrawed() { return m_bDrawed; };
	inline void		SetDrawed(BOOL flag) { m_bDrawed = flag ; };
	inline BOOL		IsReplaceMap() { return m_bReplaceMap; };
	inline void		SetReplaceMap(BOOL flag) { m_bReplaceMap = flag; };
	//도로망 분석 용
	inline	void	SetSharedMapInfo( LPVOID lpView) { m_MapPoint = lpView;};
	inline	LPVOID	GetSharedMapInfo() {return m_MapPoint;};  
	inline  HANDLE	GetMapHandle() { return m_MapHandle ;};
	inline  void	SetMapHandle(HANDLE MapHandle) {m_MapHandle =  MapHandle;};
	BOOL			IsMapExist( EGMapKind mapKind, EGMapScale mapScale);
	void			InsertMapInfoList(EGMapKind mapKind, EGMapScale mapScale);
	void			ReplaceMapInfo( EGMapKind mapKind, EGMapScale mapScale); 

	// 지도종류별 축척에 자료가 있는지 없는지의 여부를 판단
	int				IsHaveMapData();
	int				IsHaveMapData( EGMapKind nMapKind, EGMapScale nMapScale );
	int				IsHaveMapData( CString path );
	CString			GetScaleName(int nMapScale);
	CString			GetMapKindName(int nMapKind);
	inline SHORT	GetMapViewType() { return m_nMapViewType; };
	inline void		SetMapViewType(SHORT mapViewType) { m_nMapViewType = mapViewType; };
	inline BOOL		GetIfDispMode() { return m_bIfDisp; };
	inline void		SetIfDispMode(BOOL bDisp) { m_bIfDisp = bDisp; };

protected:
	BOOL			LoadDefaultInformations();	// m_nMapKind에 따라
			// 해당 지도에서 사용하는
			// 기본 색상값, feature 상태, coverage 상태,
			// 복원도시 상태, 복원도시 가능여부 등을 읽는다.
			// 성공하며 TRUE, 실패하면 FALSE를 리턴한다.
	
	//	지도축척에 해당하는 디렉토리명을 설정한다
	CString			GetScaleName( EGMapKind nMapKind, EGMapScale nMapScale );
//public:
//	EGM3DType	m_n3DModel;	// 3차원 지도의 형태

// Attributes
protected:
	EGMapKind	m_nMapKind;
	EGMapScale	m_msMapScale;
	EGCoordType	m_ctCoordType;
	BOOL		m_bDrawed;		// 도시 설정
	BOOL		m_bReplaceMap;	// 지도전환 여부

	CString		m_strBasePath;
	CString		m_strLibName;

	CGColorData		m_Color;	// 색상정보
	CGStatusFlag	m_Feature;	// Feature On/Off
	CGStatusFlag	m_Cvg;		// 커버리지 On/Off
	CGStatusFlag	m_Revert;	//
	CGStatusFlag	m_Revertable;	//

// 아래의 attribute들은 파일에 저장하지 않는다.

	//도로망 분석 용
	LPVOID      m_MapPoint;
	HANDLE		m_MapHandle;
	MapInfoList	m_MapInfoList;	// 축척별 MapInfo 리스트를 관리
	SHORT		m_nMapViewType;	// 지도자료 보기 욥션
	BOOL		m_bIfDisp;		// 지형요소 도시 제한 유무
								// TRUE:도시제한값 적용
								// FALSE:도시제한값 미적용

public:
	// 이 정보는 Serialize할 필요가 없는 요소임
	// Cvrrg에서 팔래트 정보를 얻기위해서 임시로 사용
	CPalette	*m_pPalette;

	// VPF에서 지도정보를 Serialze 유무를 보기위해서 임시로 사용
	EGMapKind		m_nOldMapKind;
	EGMapScale		m_nOldMapScale;

	BOOL			m_bBlankMsgOK;	// 래스터 도시시 자료 없는 영역을 읽고자 할 때 메세지의 표현 여부를 판단
									// FALSE : 메세지를 보낸다. TRUE : 메세지를 표시하지 않는다.

//protected:
	BOOL	m_bColorModified;	// 색상정보를 변경 했는가.
	BOOL	m_bFeatureModified;	// Feature 상태를 변경 했는가.
	BOOL	m_bCvgModified;		// Coverage 상태를 변경 했는가.
	BOOL	m_bRevModified;		//
	BOOL	m_bRevTableModified;	//
	//CGDCGeo*	m_pClientDC;	// 3차원 객체 생성시에 꼭 필요하고 또한 지도뿐만이 아니고 다른 곳에서도 
	CDC*	m_pClientDC;		// 사용해야 되기에 이곳으로 이전 함(전에는 CGMap3DispMgr이 가지고 있었슴)
								// 단, 생성과 소멸은 CGMap3DispMgr이 담당함.

	// 호성추가(축척변경 & 화면 스케일 관련)
protected:
	int		m_nDispScale;	// 화면 스케일
public:
	inline void	SetDispScale(int nDispScale) { m_nDispScale = nDispScale; };
	inline int  GetDispScale() { return m_nDispScale; };

		// 해도,항만도용 축척 저장변수
protected:
	int			m_nEncScale;	// 1:3000 ~ 1:2000000까지 integer값으로 유지
								// 선택된 해도,항만도의 원시 축척
	BOOL		m_bEncBackDisp;
	BOOL	    m_bNeighbDisp;
	UINT		m_ChartSort;
	BOOL		m_bObjSelChange;	
	int			m_HighlightShallowRadio;
	int			m_TwoShadeRadio;	
	int			m_PlainBoundRadio;	
	int			m_SimpleSymbolRadio;	
	BOOL		m_ShowMagvarCheck;	
	BOOL		m_ShowTextCheck;	
	BOOL		m_UseScaminCheck;	
	BOOL		m_ShowTopmark;
	BOOL		m_ShowIsoDanger;
	float		m_DeepContourEdit;	 
	float		m_SafeDepthEdit;	 
	float		m_SafetyContourEdit;
	float		m_ShallowContourEdit;
	WORD	    mp_ItemCode[168];
	int		    m_nSelCodeNum;	 	
	char		m_CheckedItemList[167];
	UINT		m_SelectedRadio;
public:
	//해도 직접선택
	CGRectGP    m_gpNewRect, m_gpOldRect;
	CString		m_OldEncStr, m_NowencStr;	
	CGRectGP	m_KrRect;	
public:
	CList< CGRectGP, CGRectGP> m_SencRectList;
	CList< CString, CString> m_SencRectInfoList;
	CList< int , int > m_SencScaleList; 

	void        SetSencRectInfo(CString);
	void		SetSencRect(CGRectGP);
	void		SetSencScaleInfo(int);
//////////////////////////
	BOOL		SetSelObjCodeList(WORD *, int);
	WORD *      GetSelObjCodeList();						//Draw시 사용
	inline int	 GetSelObjNum() {return m_nSelCodeNum; };	//Progress를 위해 사용 	
	BOOL         SetCheckedItemList(char*);
	char *       GetCheckedItemList();					//Dlg생성시 이용

	inline void		SetSelObjRadio(UINT nSelRadio) { m_SelectedRadio = nSelRadio; };
	inline UINT		GetSelObjRadio()  { return m_SelectedRadio; };

	inline void		SetEncScale(int nEncScale) { m_nEncScale = nEncScale; };
	inline int		GetEncScale() { return m_nEncScale; };
	inline  void	SetEncBDSP(BOOL bEncBDSP) { m_bEncBackDisp = bEncBDSP;};
	inline  BOOL	GetEncBDSP() { return m_bEncBackDisp; };
	inline  void	SetEncNDSP(BOOL bEncNDSP) { m_bNeighbDisp = bEncNDSP; };
	inline  BOOL	GetEncNDSP() { return m_bNeighbDisp ;};
	inline  void	SetEncCSort(UINT sort) { m_ChartSort = sort; };
	inline  UINT	GetEncCSort() { return m_ChartSort ;};

	inline  void	SetObjListChangeFlag(BOOL ched ){ m_bObjSelChange = ched; };
	inline  BOOL	GetObjListChangeFlag() { return  m_bObjSelChange ;};

	inline void		SetSencHShallowRd(int nRadio) { m_HighlightShallowRadio = nRadio; };
	inline int		GetSencHShallowRd() { return m_HighlightShallowRadio; };
	inline void		SetSencTShadeRd(int nRadio) { m_TwoShadeRadio = nRadio; };
	inline int		GetSencTShadeRd() { return m_TwoShadeRadio ;};
	inline void		SetSencPBoundRd(int nRadio){ m_PlainBoundRadio =  nRadio; };
	inline int		GetSencPBoundRd() { return m_PlainBoundRadio; };
	inline void		SetSencSSymbolRd(int nRadio) { m_SimpleSymbolRadio =  nRadio; };
	inline int		GetSencSSymbolRd() { return m_SimpleSymbolRadio; };
	inline void		SetSencSMagvar(BOOL bCheck) { m_ShowMagvarCheck = bCheck; };
	inline BOOL		GetSencSMagvar() { return  m_ShowMagvarCheck; };
	inline void		SetSencSText(BOOL bCheck) { m_ShowTextCheck = bCheck; };
	inline BOOL		GetSencSText() { return  m_ShowTextCheck; };
	inline void		SetSencUScamin(BOOL bCheck) { m_UseScaminCheck = bCheck; };
	inline BOOL		GetSencUScamin() { return  m_UseScaminCheck; };
	inline void		SetSencTopmark(BOOL bCheck) { m_ShowTopmark = bCheck; };
	inline BOOL		GetSencTopmark() { return  m_ShowTopmark; };
	inline void		SetSencIsoDanger(BOOL bCheck) { m_ShowIsoDanger = bCheck; };
	inline BOOL		GetSencIsoDanger() { return  m_ShowIsoDanger;};
	inline void		SetSencDeepContour(float fDeep) { m_DeepContourEdit  = fDeep; };
	inline float	GetSencDeepContour() { return m_DeepContourEdit ;};
	inline void		SetSencSafeDepth(float fDeep) { m_SafeDepthEdit  = fDeep; };
	inline float	GetSencSafeDepth() { return m_SafeDepthEdit ;};
	inline void		SetSencSaftyContour(float fDeep) { m_SafetyContourEdit  = fDeep; };
	inline float	GetSencSaftyContour() { return m_SafetyContourEdit; };
	inline void		SetSencShallowContour(float fDeep) { m_ShallowContourEdit  = fDeep; };
	inline float	GetSencShallowContour() { return m_ShallowContourEdit ; };


		// 항만도 초기 좌상좌표 저장용
public:
	CGPointGP	m_HcrfPointLT;
	CGSizeGP	m_HcrfExt;
		// 요도 래스터 저장 관련
protected:
	BOOL		m_bIsYodoRaster;	// true:요도를 래스터로 저장, false: 요도를 기존 지도정보로 저장
public:
	inline BOOL	IsYodoRaster() { return m_bIsYodoRaster; };
	inline void SetYodoRaster(BOOL bFlag) { m_bIsYodoRaster = bFlag; };
};


#endif _GMapInfo_h
