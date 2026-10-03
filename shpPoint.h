// shpPoint.h: interface for the shpPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHPPOINT_H__36940BE1_615C_4C4A_8258_97B397438EB8__INCLUDED_)
#define AFX_SHPPOINT_H__36940BE1_615C_4C4A_8258_97B397438EB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "shpObject.h"


/**
* 벡터지도 shape data(point type) 관리 class
* @author 박종원
* @version 1.0
* @since 2005.08.
**/
class shpPoint : public shpObject
{
public:
	/** real data container.. */
	vector<GPoint>		m_vSHPPoint;
	/** shp file mapping handler */
	HANDLE	m_hFMap;
	/** shx file mapping handler */
	HANDLE	m_hFMapShx;
	/** font size */
	int		m_iFontSize;
	/** font type */
	BYTE	m_lfCharSet;
	/** font strikeout */
	BYTE	m_lfStrikeOut;
	/** font color */
	COLORREF	m_crFontColor;
	/** font face name */
	CString	m_strFaceName;
	/** visible type */
	int		m_iVisibleType;

public:
	/** point type shape file open */
	bool	Open(CString	_filename);
	/** point type shape data draw(general type2) */
	void	Draw(CDC* _pDC, double _ZoomFactor, GPoint _ptMapCenter, const CRect& _rect, double _lfRatio);
	/** select status view */
	int		Track(CWnd* pWnd, GPoint point);
	/** select rect get */
	BOOL	GetSelRect(int iSelID, double &left, double &top, double &right, double &bottom);

	/** point type shape file open(use memory map) */
	bool	OpenMemoryMap(CString _filename);

	/** font info set */
	void	SetFontSize(int iFontSize);
	void	SetFontType(BYTE lfCharSet);
	void	SetFontStrikeOut(BYTE lfStrikeOut);
	void	SetFontColor(COLORREF crFontColor);
	void	SetFaceName(CString strFaceName);
	void	SetVisibleType(int iVisibleType);

	shpPoint();
	virtual ~shpPoint();

protected:
	/** select status draw */
	void	DrawSelected(CDC* _pDC);
	/** label draw */
	void	DrawLabel(CDC* _pDC);
	/** label draw */
	void	DrawLabel(CGDCGeo* _pDC);
	shpObject::hit_status HitTest(GPoint _GPoint, int& _nID);

public:
	void Draw(CDC* pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio);
	void Draw(CDC* pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
			CBitmap *pBitmap, COLORREF colorTransparent);
	void Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio,
			double dScreenScale, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2);
	void Draw(CDC* pDC, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
			double dScreenScale, int iScale, CString strName, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2);
	void DrawSelected(CDC* pDC, int iID);
	int	Track(CDC* pDC, void* pObject, GPoint selpoint, CGRectGP* rectGP);
	void DrawMemoryMap(CDC* pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio);
	
protected:
	void DrawSelected1(CDC* pDC);
	void DrawLabel1(CDC* pDC);
	void DrawLabelMemoryMap(CDC* pDC, CGPointGeo *pPoint, int i);



};

#endif // !defined(AFX_SHPPOINT_H__36940BE1_615C_4C4A_8258_97B397438EB8__INCLUDED_)
