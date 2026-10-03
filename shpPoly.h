// shpPoly.h: interface for the shpPoly class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHPPOLY_H__42998652_634E_4D73_B257_2E3CBE5E6B8A__INCLUDED_)
#define AFX_SHPPOLY_H__42998652_634E_4D73_B257_2E3CBE5E6B8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "shpObject.h"

/**
* 벡터지도 shape data(line type) 관리 class
* @author 박종원
* @version 1.0
* @since 2005.08.
**/

/** 선 데이터 도시 종류 */
enum enLineForm
{
	LF_UNKNOWN = 0,			// unknown format
	LF_FORM1,				// 단선
	LF_FORM2,				// 복선(2선)
	LF_FORM3,				// 기둥형태(일반)
	LF_FORM4,				// 기둥형태(내부DOT)
	LF_FORM5,				// 기둥형태(3선)
	LF_FORM6,				// 철도
	LF_FORM7				// 제방
};

/** real data */
struct stPolyObject
{
	CString					m_strCode;
	CTypedPtrArray<CPtrArray, polyObject*> m_paObject;
};

class shpPolyLine : public shpObject
{
public:
	/** real data container.. */
	CTypedPtrArray<CPtrArray, stPolyObject*> m_paSHPPoly;
	/** shp file mapping handler */
	HANDLE	m_hFMap;
	/** shx file mapping handler */
	HANDLE	m_hFMapShx;

public:
	void DrawSelected(CDC* pDC, int iID);
	void shpDrawRoad(CDC* pDC, enLineForm lineform, stPolyObject *pObject, double dScreenScale, CGPointGP scrLT, CGPointGP scrBR, CGRectGP _rctScreen, 
							int iWidth1, COLORREF color1, UINT uiType1, int iWidth2, COLORREF color2, UINT uiType2);
	void Draw(CDC* pDC, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio,
						double dScreenScale, int iScale, CString strName, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2);
	void Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio,
					   double dScreenScale, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2);
	void Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio);
	void DrawMemoryMap(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio);

public:
	/** line type shape file open */
	bool	OpenEx(CString _filename, BOOL bIsCodeCon);
	/** shape polygon data add */
	void	AddSHPPolyArray(polyObject *pItem);
	/** select status view */
	int		Track(CWnd* pWnd, GPoint point);		
	/** select status view */
	int		Track(CGDCGeo* pDC, void* pObject, GPoint selpoint, CGRectGP* rectGP);
	/** RGB to HSL value exchange */
	void	RGBtoHSL(COLORREF rgb, double *H, double *S, double *L);
	/** HSL to RGB value exchange */
	COLORREF	HSLtoRGB(double H, double S, double L);
	/** Hue to RGB value exchange */
	double	HuetoRGB(double m1, double m2, double h);

	/** point type shape file open(use memory map) */
	bool	OpenMemoryMap(CString _filename);

	shpPolyLine();
	virtual ~shpPolyLine();

protected:
	shpObject::hit_status HitTest(GPoint _gPoint, CRect _rctScreen, int& _ID, int& _PointID);
	/** label draw */
	void	DrawLabel(CDC* pDC);
};


#endif // !defined(AFX_SHPPOLY_H__42998652_634E_4D73_B257_2E3CBE5E6B8A__INCLUDED_)
