
#pragma once

#include "pch.h"

#include "shpObject.h"
#include "shpPoint.h"
#include "shpPoly.h"
#include "shpPolygon.h"
#include "mapdatamgr.h"
//#include "ShpMgr.h" 

#include "GMapInfo.h"
#include "GMapLib.h"


//#include "../inc/define.h"
//#include "GViewInfo.h"


/**
* 지도 복원도시 관리 클래스
* @author 유영호
* @version 1.0
* @since 2024.11.
**/
class CGGeoObject;

class CGMapDisplayMgr
{
public:
	CGMapDisplayMgr();
	CGMapDisplayMgr(IGModuleView *pContainer);
	CGMapDisplayMgr(CWnd* pParentWnd);
	virtual ~CGMapDisplayMgr();

// attribute
public:
	CGGeoObject*	m_pGeoObj;
	//CGPointGP		m_WinOrg;

	///** 영역 크기 */
	//CGSizeGP		m_WinExt;
	//CPoint			m_viewportOrg;
	//void OnPrepareDC(CWnd *pWnd, CDC *pDC, int nEllipsoid = EWGE);

	void ShapeMap(CDC* pDC);
	void SatelliteMap(CDC *pDC);
	void Draw(CDC* pDC);
	void DrawPost(CDC *pDC, void *pCurSel, int iSelID);
	void RasterMap(CDC* pDC);
	void RasterMap(CGDCGeo* pDC);
	double GetScreenScale(CDC* pDC, CGRectGP *rect);
	void shpDraw(CDC* pDC, int scale, int ratio, CGRectGP rectGP, int type_dis_map, BOOL draw );
	void worldmapShpDraw(CDC* pDC, double dZoomFactor, int iViewType);
	void backgroundShpDraw(CDC* pDC, double dZoomFactor);
	void shpDrawPost(CDC *pDC, double dZoomFactor, void *pCurSel, int iSelID);
	BOOL shpDrawPoint(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority);
	BOOL shpDrawPoint(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID);
	BOOL shpDrawPolyline(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority);
	BOOL shpDrawPolyline(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID);
	BOOL shpDrawPolygon(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority);
	BOOL shpDrawPolygon(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID);
	void shpDrawText(CDC* pDC, stTextPriority* pPriority, stMapText* pMapText);
	void SelMarkingDraw(CDC *pDC, double dZoomFactor, CGRectGP rectGP, CGPointGP gpoint);
	void DrawMapBoundary(CDC* pDC);

	/** window pointer */
	CWnd*						m_pWnd;

	// 임시로 막음 - youngho
	CComPtr<IGModuleView>		m_ifView;
	//CComPtr<IGModuleMapView>	m_ifMapView;
	CComPtr<IGViewInfo>			m_ifViewInfo;

	//CGViewInfo*	 m_ifViewInfo;
			    
	CGGeoTrans	 m_geoObject;
	CGMapInfo*	 m_pMapInfo;			// 지도 속성정보 관리 클래스 
	BOOL		 m_bBkMode;				// 지도 배경색 도시여부 
	double		 m_dScreenScale;		// 화면 스케일 
	double		 m_dZoomFactor;			// 배율 
	double		 m_dPixelPerMM;			// 1 pixel의 mm 값 
	long		 m_lEllipsoid;
			    
	CGRectGP	 m_ClientRectGp;		// 클라이언트 영역 
	COLORREF	 m_clBackColor[2];		// background color 0:바다, 1:행정지역
	CGDCGeo*	 m_pSelmemDC;			// memory DC

// Operations
public:
	void SetInitInfo(IGViewInfo* pViewInfo, CGMapInfo* pMapInfo);			// 지도 속성 관리 클래스 초기화
	void SetParentWnd(CWnd *pWnd);											// 상위 윈도우 설정 
	void SetBkMode(BOOL flag) {m_bBkMode = flag;};							// 배경색 flag 설정
	BOOL GetBkMode() {return m_bBkMode;};									// 배경색 flag 얻기
	double GetScreenScale(CGDCGeo *pDC, CGRectGP *rect);					// 화면 스케일 얻기 
	double GetPixelPerMM(CGDCGeo *pDC);										// pixel 길이 얻기 
	double GetPixelPerMM(CDC *pDC);
	void backgroundShpDraw(CGDCGeo *pDC, double dZoomFactor);				// 배경지도 도시
	void worldmapShpDraw(CGDCGeo *pDC, double dZoomFactor, int iViewType);	// 세계지도 도시 
	void shpDraw(CGDCGeo *pDC, int mapscale, int ratio, CGRectGP rectGP, int type_dis_map, BOOL draw );		// 벡터 지도 복원 도시
	void shpDrawPost(CGDCGeo *pDC, double dZoomFactor, void *pCurSel, int iSelID);							// 벡터지도에서 선택된 지형요소 도시
	
	// 점,선,면 벡터 데이터 복원도시
	BOOL shpDrawPoint(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, 
							shpObject* pShpObject, CMapPriority* pMapPriority);											// 점 데이터 복원도시(일반) 
	BOOL shpDrawPoint(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, 
							shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID);								// 점 데이터 복원도시(select type) 
	BOOL shpDrawPolyline(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, 
								shpObject* pShpObject, CMapPriority* pMapPriority);										// 선 데이터 복원도시(일반) 
	BOOL shpDrawPolyline(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, 
								shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID);							// 선 데이터 복원도시(select type)
	BOOL shpDrawPolygon(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, 
								shpObject* pShpObject, CMapPriority* pMapPriority);										// 면 데이터 복원도시(일반) 
	BOOL shpDrawPolygon(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, CMapProperty* pMapProperty, 
								shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID);							// 면 데이터 복원도시(select type) 
	void shpDrawText(CGDCGeo* pDC, stTextPriority* pPriority, stMapText* pMapText);							// 벡터 지도 텍스트 데이터 도시
	void SelMarkingDraw(CGDCGeo *pDC, double dZoomFactor, CGRectGP rectGP, CGPointGP gpoint);				// select marking draw 
	void DrawMapBoundary(CGDCGeo* pDC);		// cross draw
	void SetChromView(int iHSLSet, int iType, double H, double S, double L);		// 색도조절

// Overrides
public:
	virtual void Draw(CGDCGeo* pDC);			// 지도 darw 
	virtual void DrawPost(CGDCGeo* pDC, void* pCurSel, int iSelID);		// 지도에서 선택된 아이템 draw 
	void save_current_state_on_mem();
private:
 
	CLoad_path_shps load_mgr; 
	//CShpMgr m_shp_mgr;
	void SaveLogFile(CString strmsg);
 
};
