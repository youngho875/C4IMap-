// GLib.h: interface for the CGLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLIB_H__D51A054B_8DF3_4102_8E56_24AB6E68C384__INCLUDED_)
#define AFX_GLIB_H__D51A054B_8DF3_4102_8E56_24AB6E68C384__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>

#define MAX_ZOOM		64*10
#define	MIN_ZOOM		0.24

static double		m_dMapLeft;
static double		m_dMapTop;
static double		m_dMapRight;
static double		m_dMapBottom;
static int			m_iVectorMapKind;
static int			m_iRasterMapKind;
static CView*		m_pView = NULL;

/**
* 좌표관련 Lib(좌표변환등)
* @author youngho  Peter Croswell
* @version 1.0
* @since 2024.08
**/
class CGLib  
{
//SearchKind : SK
//TableName : TBLNM
//PrimaryKey : PK
private:
	static CString		m_strSearchKind;
	static CString		m_strTableName;
	static CString		m_strPrimaryKey;

	static int			m_iScale;
	static int			m_iRatio;
	
	static BOOL			m_bEmbedOLE;

public:
	static void			SetEmbedOLE(BOOL bEmbed) { m_bEmbedOLE = bEmbed;}
	static BOOL			GetEmbedOLE() {return m_bEmbedOLE;}

	static void			SetSearchKind(CString strSearchKind) { m_strSearchKind = strSearchKind; }
	static CString		GetSearchKind() { return m_strSearchKind; }

	static void			SetTableName(CString strTableName) { m_strTableName = strTableName; }
	static CString		GetTableName() { return m_strTableName;	}

	static void			SetPrimaryKey(CString strPrimaryKey) { m_strPrimaryKey = strPrimaryKey;	}
	static CString		GetPrimaryKey() { return m_strPrimaryKey; 	}

	//static CString		GetHwpExecutePath();

	static double		GetCurrentZoomFactor();
	static void			SetCurrentMapInfo(int scale, int ratio);

public:
	static const double pi;

public:	
	static COleDocument* GetOleDocument();
	static void			SetView(CView* pView) {m_pView = pView;}
	static CView*		GetView() {return m_pView;}

	// jwpark add 2005.11.24
	static double		GetMinZoom(BOOL bIsAutoMode, long scale);
	static double		GetMaxZoom(long scale);
	// jwpark add end
	static double		GetMaxZoom() { return MAX_ZOOM; }
	static double		GetMinZoom() { return MIN_ZOOM; }

	static void			SetMapRect(double dMapLeft, double dMapTop, double dMapRight, double dMapBottom);
	static void			SetMapKind(int iVectorMapKind, int iRasterMapKind);
	static void			GetMapRect(double &dMapLeft, double &dMapTop, double &dMapRight, double &dMapBottom);
	static void			GetMapKind(int &iVectorMapKind, int &iRasterMapKind);

//
// from CommonMath
public:
	static CWnd*	GetWnd() { return m_pWnd; }
	static void		SetWnd(CWnd* pWnd) { m_pWnd = pWnd; }

	static void		SetDockingBarStyle(UINT style) { m_BarStyle = style; }
	static DWORD	GetDockingBarStyle() { return m_BarStyle; }

	static CWinApp* GetAfxGetApp() { return m_pApp; }
	static void	  SetAfxSetApp(CWinApp* pApp) {m_pApp = pApp;}
	/** 현재 Active된 View를 얻는다. */
	static CView* GetCurrentView();
	/** 장치 좌표를 거리로 환산 한다 */
	static void DPtoMM(CDC* pDC, CSize* pSize);
	/** 거리 좌표를 장치 좌표를 환산 한다 */
	static void MMtoDP(CDC* pDC, CSize* pSize);
	static void DPtoPoints(CDC* pDC, CSize* pSize);
	static void PointstoDP(CDC* pDC, CSize* pSize);

	static long   GetDistance(const CPoint& p1, const CPoint& p2);
	static double GetDistancef(const CPoint& p1, const CPoint& p2);
	static double GetDistance(const CPoint& pt1, const CPoint& pt2, const CPoint& pt);
	/** pt1과 pt2를 잇는 직선과 점 pt와의 거리를 구한다. */
	static bool PtInPolygon(CPoint* pts, int nPts, CPoint Point); //PolyGon 내부의 HitTest //jdkim
	static BOOL PtOnPolyline(CPoint* pts, int nPts, CPoint pt, int width, BOOL bLoop);
	// Line Width 가 width인 Polyline상에서
	// 점 pt에 대한 Hit-Test를 한다.

	static double GetRadian(const CPoint& center, const CPoint& point);
	static CPoint Angle2Point(const CPoint& base, double angl, int length, int dir=-1);
	static CPoint Rotate(const CPoint& origin, const CPoint& base, double angl);
	static void RotateArray(CPoint* origin, int n, const CPoint& base, double angle);

	static double Angle2Radian(double X);

	static double Radian2Angle(double X);

	static CPoint GetOffsetPoint(const CPoint& A, const CPoint& B, int W, int dir=-1);

	static CPoint GetIntersection(const CPoint& A, const CPoint& B, const CPoint& C, const CPoint& D);

	static long GetPolyLength(LPPOINT lpPts, int nIndex);
	static BOOL PtWithinPoly(CPoint pt, CPoint* pol, int npol);

	static BOOL DivideSpline(LPPOINT Line, int nSize, int nQty, LPPOINT p);

	static void CopyBitmap(CBitmap * pDestBitmap, CBitmap * pSrcBitmap);
	static HANDLE DDBtoDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal=NULL);
	static HBITMAP DIBtoDDB( HANDLE hDIB );
	static BOOL StretchDibBlt (HDC hdc, INT x0, INT y0, INT dx, INT dy, HANDLE hdib, INT x1, INT y1, INT dx1, INT dy1, LONG rop);
	static BOOL DibInfo(HANDLE hbi, LPBITMAPINFOHEADER lpbi);
	static WORD PaletteSize (VOID FAR * pv);
	static WORD DibNumColors (VOID FAR * pv);
	static BOOL WriteDIB(CArchive& ar, HANDLE hdib);
	static BOOL WriteDIB(LPCTSTR szFile, HANDLE hDIB);
	static BOOL LoadBMPImage( LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal );

	//static CString GetSystemOSVersion();

	static CPoint GetEndPoint(CPoint point, double dAngle, long ldistance);

//
// from CommonMathb
public:
	static COLORREF AlphaBlendRGB(COLORREF color, int alpha);
	static void		RGBtoHSL( COLORREF rgb, double *H, double *S, double *L );
	static COLORREF	HSLtoRGB(double H, double S, double L);
	static double	HuetoRGB(double m1, double m2, double h );

	static void SetInitColor(COLORREF *dwColor);
	static void SetGrayValue(COLORREF *color, BOOL bColorMode, int nLumino);
	static void SetOriginValue(COLORREF *color, BOOL bColorMode, int nLumino);

	static COLORREF GetDisplayColor(COLORREF dwColor);
	static COLORREF GetDisplayColor(COLORREF color, BOOL bColorMode, int nLumino);
	static COLORREF GetDisplayColor(COLORREF color, int nLumino);

//
// Resource utility
	static CString	LoadString(HINSTANCE hInstance, UINT uID);
	static void		SetClientRect(CRect rect) { m_Rect = rect; }
	static CRect	GetClientRect() { return m_Rect; }


	/** 투명도 DB저장 테이블 */
	static CString	GetOverTable() { return CString("SDE.GOMTNOVERTB"); }
	static double	GetLatOffset() ;
	static double   GetLonOffset() ;

	/** 도분초 환산. */
	static CString ConvertTextDMS(double Degree); //return string = "1271243"
	static CString ConvertShortDMS(double Degree); //return string = "124.12.43"


	static void SetHWND(HWND hWnd) { m_hWnd = hWnd;	}
	static HWND GetHWND() { return m_hWnd; }

public:
	static HWND	 m_hWnd;
	static CWnd* m_pWnd;
	static CWinApp* m_pApp;
	static CRect  m_Rect;
	static UINT m_BarStyle;
	static double m_latOffset;
	static double m_lonOffset;

};

#endif // !defined(AFX_GLIB_H__D51A054B_8DF3_4102_8E56_24AB6E68C384__INCLUDED_)
