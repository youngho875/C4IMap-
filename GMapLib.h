// GMapLib.h: interface for the CGMapLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMAPLIB_H__439C20B2_67F2_4336_831F_6758B8B993F7__INCLUDED_)
#define AFX_GMAPLIB_H__439C20B2_67F2_4336_831F_6758B8B993F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include "../IncludeSDEConn.h"


typedef struct {
	double left;
	double top;
	double right;
	double bottom;
} SRECT;


/**
* 지도 정보 관리 및 편집.
* @author oneriver
* @version 1.0
* @since 2005.11
**/
class CGMapLib  
{
public:
// MapKind
	/** 지도종류를 얻는다. */
	static void		SetMapKind(int nMapKind) {m_nMapKind = nMapKind;} 
	static BOOL		GetMapKind() { return m_nMapKind; }
	/** 지도축척을 얻는다. */
	static void		SetMapScale(int nMapScale) {m_nMapScale = nMapScale; } 
	static int		GetMapScale() {return m_nMapScale;	}
	static void		SetZoomFactor(double zoomFactor) {m_dZoomFactor = zoomFactor;}
	static double	GetZoomFactor() { return m_dZoomFactor;	}
	/** 지도선택 UI를 통해 지도를 선택도시한다.*/
	static void		SetMapDraw(BOOL bMapDraw) { m_bMapDraw = bMapDraw; 	}
	static BOOL		GetMapDraw() { return m_bMapDraw; }
	/** 자동축척변경을 위한 함수 */
	static int		GetAutoScaleChange(double zoomFactor);

// Layer Activate
	/** 지도레이어가 활성화 되어있는지를 검사한다. */
	static BOOL		GetMapActivate() { return m_bMapActivate; } 
	static void	    SetMapActivate(BOOL bActive) { m_bMapActivate = bActive; }

	static void		SetMapGeoType(long lGeoType) { m_lGeoType = lGeoType; };
	static long		GetMapGeoType() { return m_lGeoType; };

	static CString  GetMapSelectedName() { return m_strSelectedMapName; };
	static void     SetMapSelectedName(CString strMapName) { m_strSelectedMapName = strMapName; }

	static void		SetMapIndexInfo(BOOL bIndex) { m_bIndexMap = bIndex; }
	static BOOL		GetMapIndexInfo() { return m_bIndexMap;	}

// Map Edit
	//지형정보를 새롭게 생성하는지 검사한다.
//	static void		SetMapEditStart(BOOL bStart) { m_bEditStart = bStart; }
//	static BOOL		GetMapEditStart() { return m_bEditStart; }

	static void		SetMapFeatureObjectID(long lObjectID) {m_lObjectID = lObjectID; }
	static long		GetMapFeatureObjectID() { return m_lObjectID; }

	static void		SetMapFeatureSaved(BOOL bSaved) { m_bMapSaved = bSaved;	}
	static BOOL		GetMapFeatureSaved() { return m_bMapSaved; }

	static void		SetNewFeatureDraw(BOOL bStart) { m_bEditStart = bStart; }
	static BOOL		GetNewFeatureDraw() { return m_bEditStart; }

	static void		SetNewFeatureComplete(BOOL bNewFeatureComplete) { m_bNewFeatureComplete = bNewFeatureComplete; }
	static BOOL		GetNewFeatureComplete() { return m_bNewFeatureComplete;	}
	
	/** 지도편집이 시작되었는지 검사한다. */
	static void		SetMapEditActivate(BOOL bMapEditActivate) { m_bMapEditActivate = bMapEditActivate; }
	static BOOL		GetMapEditActivate() { return m_bMapEditActivate; }

	/** 형정보가 편집중인지 검사한다. */
	static void		SetMapModified(BOOL bModified) { m_bModified = bModified; }
	static BOOL		GetMapModified() { return m_bModified; }

	/** 잠정관련 인덱스정보관리-공식정보, 잠정정보, 공식+잠정정보 */
	static void		SetMapHistory(int nMapHistory) { m_nMapHistory = nMapHistory;}
	static int		GetMapHistory() { return m_nMapHistory;	}
	static void		SetMapHistoryIndexPattern(int nMapHistoryIndexPattern) { m_nMapHistoryIndexPattern = nMapHistoryIndexPattern;}
	static int		GetMapHistoryIndexPattern() { return m_nMapHistoryIndexPattern;	}


// Query Attribute 
	static void		SetMapFeatureQuery(BOOL bQuery) { m_bMapQuery = bQuery; }
	static BOOL		GetMapFeatureQuery() { return m_bMapQuery; }
	static void		SetMapFeatureQueryMarking(BOOL bMark) {m_bMark = bMark;	}
	static BOOL		GetMapFeatureQueryMarking() {return m_bMark; }
	static void		SetMapHasItemText(BOOL bText) { m_bItemText = bText; }
	static BOOL		GetMapHasItemText() { return m_bItemText; }
	static void		SetMapItemGridCell(int row) { m_Row = row; }
	static int		GetMapItemGridCell() { return m_Row; }
	static void		SetGridCellUpdate(BOOL bInvalidate) {m_bGridInvalidate = bInvalidate;}
	static BOOL		GetGridCellUpdate() { return m_bGridInvalidate; }

	static void		SetMapFeatureQueryRect(double left, double top, double right, double bottom) {
					m_rect.left = left;
					m_rect.top = top;
					m_rect.right = right;
					m_rect.bottom = bottom;
	}

	static SRECT	GetMapFeatureQueryRect() { return m_rect; }


	static CString	GetVariant(VARIANT vValue);

	//시간조회
	static void		StartClock();
	static void		EndClock(CString strMsg="");



public:
	static int		m_nMapKind;
	static int		m_nMapScale;
	static double   m_dZoomFactor;
	static BOOL		m_bMapDraw;
	static BOOL		m_bIndexMap;

	static long		m_lObjectID;
	static BOOL		m_bMapSaved;
	static BOOL		m_bNewFeatureComplete;
	static BOOL		m_bMapEditActivate;
	static BOOL		m_bMapActivate;
	static long		m_lGeoType;
	static CString  m_strSelectedMapName;
	static BOOL		m_bEditStart;
	static BOOL		m_bMapQuery;
	static BOOL		m_bMark;
	static BOOL		m_bModified;
	static int		m_nMapHistory;
	static int		m_nMapHistoryIndexPattern;

	static BOOL		m_bItemText;
	static int		m_Row;
	static BOOL		m_bGridInvalidate;
	static SRECT	m_rect;

	static VARIANT  m_vValue;

	static clock_t	m_startClock;
	static clock_t  m_endClock;

};

#endif // !defined(AFX_GMAPLIB_H__439C20B2_67F2_4336_831F_6758B8B993F7__INCLUDED_)
