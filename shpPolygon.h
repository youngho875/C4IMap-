// shpPolygon.h: interface for the shpPolygon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHPPOLYGON_H__CCE87B33_CDC4_11D5_BA26_00104B1701D2__INCLUDED_)
#define AFX_SHPPOLYGON_H__CCE87B33_CDC4_11D5_BA26_00104B1701D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoTrans/GDatumTrans.h"
#include "shpObject.h"

/**
* 벡터지도 shape data(polygon type) 관리 class
* @author 박종원
* @version 1.0
* @since 2005.08.
**/
class shpPolygon : public shpObject  
{
public:
	/** real data container.. */
	CTypedPtrArray<CPtrArray, polyObject*> m_paSHPPoly;
	/** tot count */
	int						m_iTotCnt;
	/** read count */
	int						m_iReadCnt;
	/** shp file mapping handler */
	HANDLE	m_hFMap;
	/** shx file mapping handler */
	HANDLE	m_hFMapShx;

protected:

public:
	void Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio);
	void draw_polyline(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio);
	void Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio,
			double dScreenScale, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2);
	void Draw(CDC* pDC, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
				double dScreenScale, int iScale, CString strName, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2);
	void DrawSelected(CDC* pDC, int iID);
	int	 Track(CDC* pDC, void* pObject, GPoint selpoint, CGRectGP* rectGP);
	void DrawMemoryMap(CDC* _pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio);

protected:
	void DrawPolygon(CDC* pDC, CGRectGP _rect);
	void DrawPolyLine(CDC* pDC, CGRectGP _rect);
	void DrawLabel(CDC* pDC);


public:
	CGPointGP *polygonGP;
	CWnd*	m_pMainWnd;

	bool	Open(CString _filename);
	/** select rect get */
	BOOL	GetSelRect(int iSelID, double &left, double &top, double &right, double &bottom);

	/** point type shape file open(use memory map) */
	bool	OpenMemoryMap(CString _filename);
	
	shpPolygon(CWnd *pParentWnd);
	virtual ~shpPolygon();

protected:
	double	Area2(GPoint a, GPoint b, GPoint c);
	GPoint	FindCG(vector<GPoint>& vPoint);
	GPoint	Centroid3(GPoint a, GPoint b, GPoint c);
};


#endif // !defined(AFX_SHPPOLYGON_H__CCE87B33_CDC4_11D5_BA26_00104B1701D2__INCLUDED_)
