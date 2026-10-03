// GLib.cpp: implementation of the CGLib class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GLib.h"
#include "GMessenger.h"
#include "GMapLib.h"
#include "GEnvVar.h"
#include <math.h>
#include <sysinfoapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const double		CGLib::pi = 3.14159265359;
CWnd*				CGLib::m_pWnd = NULL;
CWinApp*			CGLib::m_pApp = NULL;
CRect				CGLib::m_Rect;
UINT				CGLib::m_BarStyle;
HWND				CGLib::m_hWnd;
double				CGLib::m_latOffset = 0.0;
double				CGLib::m_lonOffset = 0.0;

CString				CGLib::m_strSearchKind = _T("");
CString				CGLib::m_strTableName = _T("");
CString				CGLib::m_strPrimaryKey = _T("");

int					CGLib::m_iScale = 0;
int					CGLib::m_iRatio = 0;

BOOL				CGLib::m_bEmbedOLE = FALSE;

//////////////////////////////////////////////////////////////////////
// CGLib utility functions
// MDI view implementation file
CView *CGLib::GetCurrentView()
{
   CMDIChildWnd * pChild =
       ((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();

   if ( !pChild )
       return NULL;

   CView * pView = pChild->GetActiveView();

   if ( !pView )
      return NULL;

   // Fail if view is of wrong kind
//   if ( ! pView->IsKindOf( RUNTIME_CLASS(CMyView) ) )
//    return NULL;

   return pView;
}


void CGLib::DPtoMM(CDC* pDC, CSize* pSize)
{
	CSize size, res;
	size.cx = pDC->GetDeviceCaps(HORZSIZE);   // millimeter
	size.cy = pDC->GetDeviceCaps(VERTSIZE);
	
	res.cx = pDC->GetDeviceCaps(HORZRES);     // resolution
	res.cy = pDC->GetDeviceCaps(VERTRES);

	if (pSize->cx != 0)
	{
		pSize->cx = pSize->cx*size.cx/res.cx;
		if (pSize->cx == 0)
			pSize->cx = 1;
	}
	
	if (pSize->cy != 0)
	{
		pSize->cy = pSize->cy*size.cy/res.cy;
		if (pSize->cy == 0)
			pSize->cy = 1;
	}
}


void CGLib::DPtoPoints(CDC* pDC, CSize* pSize)
{
	CSize size, res;
	size.cx = pDC->GetDeviceCaps(HORZSIZE);   // millimeter
	size.cy = pDC->GetDeviceCaps(VERTSIZE);
	
	res.cx = pDC->GetDeviceCaps(HORZRES);     // resolution
	res.cy = pDC->GetDeviceCaps(VERTRES);

	if (pSize->cx != 0)
	{
		pSize->cx = (int)(pSize->cx*size.cx*72/24.5)/res.cx;
		if (pSize->cx == 0)
			pSize->cx = 1;
	}
	
	if (pSize->cy != 0)
	{
		pSize->cy = (int)(pSize->cy*size.cy*72/24.5)/res.cy;
		if (pSize->cy == 0)
			pSize->cy = 1;
	}
}


void CGLib::MMtoDP(CDC* pDC, CSize* pSize)
{
	CSize size, res;
	size.cx = pDC->GetDeviceCaps(HORZSIZE);   // millimeter
	size.cy = pDC->GetDeviceCaps(VERTSIZE);
	
	res.cx = pDC->GetDeviceCaps(HORZRES);     // resolution
	res.cy = pDC->GetDeviceCaps(VERTRES);

	if (pSize->cx != 0)
	{
		pSize->cx = pSize->cx*res.cx/size.cx;
		if (pSize->cx == 0)
			pSize->cx = 1;
	}
	
	if (pSize->cy != 0)
	{
		pSize->cy = pSize->cy*res.cy/size.cy;
		if (pSize->cy == 0)
			pSize->cy = 1;
	}
}


void CGLib::PointstoDP(CDC* pDC, CSize* pSize)
{
	CSize size, res;
	size.cx = pDC->GetDeviceCaps(HORZSIZE);   // millimeter
	size.cy = pDC->GetDeviceCaps(VERTSIZE);
	
	res.cx = pDC->GetDeviceCaps(HORZRES);     // resolution
	res.cy = pDC->GetDeviceCaps(VERTRES);

	if (pSize->cx != 0)
	{
		pSize->cx = (int)(pSize->cx*res.cx*24.5/72)/size.cx;
		if (pSize->cx == 0)
			pSize->cx = 1;
	}
	
	if (pSize->cy != 0)
	{
		pSize->cy = (int)(pSize->cy*res.cy*24.5/72)/size.cy;
		if (pSize->cy == 0)
			pSize->cy = 1;
	}
}


long CGLib::GetDistance(const CPoint& p1, const CPoint& p2)
{
	if (p1 == p2)
		return 0L;

	CSize distp(p2 - p1);
	double temp = ((double)distp.cx)*distp.cx + ((double)distp.cy)*distp.cy;
	double dist = sqrt(temp);
	
	return (long)(dist + .5);
}

double CGLib::GetDistancef(const CPoint& p1, const CPoint& p2)
{
	if (p1 == p2)
		return 0.0;

	CPoint distpoint = p2 - p1;
	double dist = sqrt((double)(distpoint.x*distpoint.x + distpoint.y*distpoint.y));
	
	return dist;
}


/*
----------------------------------------------------------------------
Subject 1.02: How do I find the distance from a point to a line?


    Let the point be C (XC,YC) and the line be AB (XA,YA) to (XB,YB).
    The length of the line segment AB is L:

        L=((XB-XA)**2+(YB-YA)**2)**0.5

    and
            (YA-YC)(YA-YB)-(XA-XC)(XB-XA)
        r = -----------------------------
                        L**2

            (YA-YC)(XB-XA)-(XA-XC)(YB-YA)
        s = -----------------------------
                        L**2

    Let I be the point of perpendicular projection of C onto AB, the

        XI=XA+r(XB-XA)
        YI=YA+r(YB-YA)

    Distance from A to I = r*L
    Distance from C to I = s*L

    If r<0      I is on backward extension of AB
    If r>1      I is on ahead extension of AB
    If 0<=r<=1  I is on AB

    If s<0      C is left of AB (you can just check the numerator)
    If s>0      C is right of AB
    If s=0      C is on AB


----------------------------------------------------------------------
*/
double CGLib::GetDistance(const CPoint& pt1, const CPoint& pt2, const CPoint& pt)
// 리턴값이 -1이면 점이 직선의 연장선에 있는 것이다.
{
	double L, R, dist;
	
//            (YA-YC)(YA-YB)-(XA-XC)(XB-XA)
//        r = -----------------------------
//                        L**2
	L = sqrt( (((double)pt2.x - pt1.x) * ((double)pt2.x - pt1.x) +
		      ((double)pt2.y - pt1.y) * ((double)pt2.y - pt1.y)) );
	
	R = ((((double)pt1.y-pt.y))*((double)pt1.y-pt2.y)-
		(((double)pt1.x-pt.x))*((double)pt2.x-pt1.x)) / L / L;
	if (R < 0.0 || R > 1.0)		 // pt is on the extension of the line
		return -1.0;

	dist = ( (((double)pt1.y-pt.y))*((double)pt2.x-pt1.x) - 
			(((double)pt1.x-pt.x))*((double)pt2.y-pt1.y) ) / L;

	return fabs(dist);
}


BOOL CGLib::PtOnPolyline(CPoint* pts, int nPts, CPoint pt, int width, BOOL bLoop)
{
	if (nPts == 0)
		return FALSE;

	if (width < 3) width = 3;
	double Dist;

	for (int i=0; i<nPts-1; i++)
	{
		Dist = GetDistance(pts[i], pts[i+1], pt);
		if ( Dist != -1.0 && Dist <= (double)(width/2.0) )
		return TRUE;
	}
	if (bLoop)
	{
		Dist = GetDistance(pts[0], pts[nPts-1], pt);
		if ( Dist != -1.0 && Dist <= (double)(width/2.0) )
			return TRUE;
	}
	return FALSE;
}

bool CGLib::PtInPolygon(CPoint* pts, int nPts, CPoint Point)
{
	int i, j;
	bool c = false;
	int size = nPts;

    for (i = 0, j = (size - 1); i < size; j = i++) 
	{
		if ((((pts[i].y <= Point.y) && (Point.y < pts[j].y)) || ((pts[j].y <= Point.y) && ( Point.y < pts[i].y))) && (Point.x < (pts[j].x - pts[i].x) * (Point.y - pts[i].y) / (pts[j].y - pts[i].y) + pts[i].x))
			c = !c;
	}
	return c;
}


double CGLib::GetRadian(const CPoint& center, const CPoint& point)
{
	return ::atan2( (double)(point.y-center.y),
		(double)(point.x-center.x));
}


CPoint CGLib::Angle2Point(const CPoint& base, double angl, int length, int dir)  // angle is in radian
{
	CPoint changed;

	
	changed.x = (long)(base.x + length * cos(angl) + .5);
	if (dir < 0)
		changed.y = (long)(base.y + length * sin(angl) + .5);
	else
		changed.y = (long)(base.y - length * sin(angl) + .5);
	

	return changed;
}


CPoint CGLib::Rotate(const CPoint& origin, const CPoint& base, double angl)
{
	if (angl == .0) return origin;

	CPoint tr2origin(origin - base);
	CPoint rotated;
	
	rotated.x = (long)(cos(angl) * tr2origin.x - sin(angl) * tr2origin.y + .5);
	// Y-Axis는 매핑모드에 따라 달라진다.
	rotated.y = (long)(sin(angl) * tr2origin.x + cos(angl) * tr2origin.y + .5);

	return rotated + base;
}


void CGLib::RotateArray(CPoint* origin, int n, const CPoint& base, double angl)
{
	if (angl == .0) return;

	CPoint tr2origin;

	for (int i=0; i<n; i++)
	{
		tr2origin = CPoint(origin[i] - base);
		origin[i].x = (long)(cos(angl) * tr2origin.x - sin(angl) * tr2origin.y + .5);
		origin[i].y = (long)(sin(angl) * tr2origin.x + cos(angl) * tr2origin.y + .5);
	
		origin[i] += base;
	}
}

double CGLib::Angle2Radian(double X)
{
	return (X * (pi / 180.));
}

double CGLib::Radian2Angle(double X)
{
	return (X * (180. / pi));
}

CPoint CGLib::GetOffsetPoint(const CPoint& A, const CPoint& B,	int W, int dir)
{
	double ang = GetRadian(A, B);

	CPoint P = Angle2Point(A, ang + pi/2, W, dir);

	return P;
}

CPoint CGLib::GetIntersection(const CPoint& A, const CPoint& B, const CPoint& C, const CPoint& D)
{
	double nomR = (double)(A.y-C.y)*(D.x-C.x) - (double)(A.x-C.x)*(D.y-C.y);
	double denomR = (double)(B.x-A.x)*(D.y-C.y)-(double)(B.y-A.y)*(D.x-C.x);

	if (denomR == 0.0) return B;	   // 평행..

	CPoint pt;
	double R = nomR/denomR;
	pt.x = (long)(A.x + R*(B.x-A.x) + .5);
	pt.y = (long)(A.y + R*(B.y-A.y) + .5);

	return pt;
}

long CGLib::GetPolyLength(LPPOINT lpPts, int nIndex)
{
	if (nIndex < 2) return 0l;

	CPoint pt1, pt2;
	double lfLen = 0L;
	
	for (int i=1; i<nIndex; i++)
	{
		pt1 = lpPts[i-1];
		pt2 = lpPts[i];
		
		lfLen += GetDistancef(pt1, pt2);
	}

	return (long)lfLen;
}

BOOL CGLib::DivideSpline(LPPOINT Line, int nSize, int nQty, LPPOINT p)
// Line[nSize]를 nQty등분하여 그 포인트들을 p에 넣는다.
// p의 사이즈는 nQty+1이 된다.
{
	if (nSize < 2) return FALSE;

//	int nLen = GetDistance(Line[0], Line[1]);
//	int nUnit = nLen / nQty;         // 단위의 길이
	
	p[0] = Line[0];   // 처음 점.
	p[nQty] = Line[nSize-1];  // 마지막 점.

	if (nQty == 1)     // 나눌필요 없음.
		return TRUE;

	CPoint p1 = Line[0];
	CPoint p2 = Line[nSize-1];
	for (int i=1; i<nQty; i++)
	{
		p[i].x = (p2.x-p1.x)*i/nQty + p1.x;
		p[i].y = (p2.y-p1.y)*i/nQty + p1.y;
	}

	return TRUE;
}

BOOL CGLib::PtWithinPoly(CPoint pt, CPoint* pol, int npol)
{
	int i, j;
	BOOL bRet = FALSE;
	for (i = 0, j = npol-1; i < npol; j = i++)
	{
		if ((((pol[i].y<=pt.y) && (pt.y<pol[j].y)) || 
			((pol[j].y<=pt.y) && (pt.y<pol[i].y))) &&
			(pt.x < (pol[j].x - pol[i].x) * (pt.y - pol[i].y) / 
			(pol[j].y - pol[i].y) + pol[i].x))
			bRet = !bRet;
	}

	return bRet;
}

void CGLib::CopyBitmap(CBitmap * pDestBitmap, CBitmap * pSrcBitmap)
{
	BITMAP bm;
	pSrcBitmap->GetBitmap(&bm);

	DWORD mByteWidth = 0;
	switch(bm.bmBitsPixel)
	{
		case 1:
			mByteWidth =  (bm.bmWidth + 7) >> 3;
			if(mByteWidth & 1) mByteWidth++;
			break;
		case 4:
			mByteWidth =  (bm.bmWidth +1) >>1 ;
			if(mByteWidth & 1) mByteWidth++;
			break;
		case 8:
			mByteWidth =  bm.bmWidth ;
			if(mByteWidth & 1) mByteWidth++;
			break;
		case 16:
			mByteWidth = bm.bmWidth*2;
			if(mByteWidth & 1) mByteWidth++;
			break;
		case 24:
			mByteWidth = bm.bmWidth*3;
			if(mByteWidth & 1) mByteWidth++;
			break;

	}
	
	// 버퍼크기 계산
 	DWORD mBufferSize = (DWORD) mByteWidth * (DWORD) bm.bmPlanes * (DWORD) bm.bmHeight;

	// 비트맵 정보를 저장할 메모리 확보
	HGLOBAL hGlobal = ::GlobalAlloc (GMEM_FIXED, mBufferSize + 1000L);
	if (!hGlobal){
		return ;
		}
	unsigned char *MemoryBuffer =
		(unsigned char *) ::GlobalLock (hGlobal);
//	DWORD byte = pSrcBitmap->GetBitmapBits (mBufferSize, MemoryBuffer);
	
	BOOL res = pDestBitmap->CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes,
											bm.bmBitsPixel, MemoryBuffer);
	//if( !res )
	//{
	//	CGMessenger::SetMessage("비트맵 복사에 실패하였습니다.", CGMessenger::ToDialog);
	//}

	::GlobalUnlock (hGlobal);
	::GlobalFree (hGlobal);
}

// DDBtoDIB             - Creates a DIB from a DDB
// bitmap               - Device dependent bitmap
// dwCompression        - Type of compression - see BITMAPINFOHEADER
// pPal                 - Logical palette
HANDLE CGLib::DDBtoDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal ) 
{
        BITMAP                  bm;
        BITMAPINFOHEADER        bi;
        LPBITMAPINFOHEADER      lpbi;
        DWORD                   dwLen;
        HANDLE                  hDIB;
        HANDLE                  handle;
        HDC                     hDC;
        HPALETTE                hPal;


        ASSERT( bitmap.GetSafeHandle() );

        // The function has no arg for bitfields
        if( dwCompression == BI_BITFIELDS )
                return NULL;

        // If a palette has not been supplied use defaul palette
        if (pPal == NULL)
			hPal = NULL;
		else
			hPal = (HPALETTE) pPal->GetSafeHandle();
        if (hPal==NULL)
                hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

        // Get bitmap information
        bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

        // Initialize the bitmapinfoheader
        bi.biSize               = sizeof(BITMAPINFOHEADER);
        bi.biWidth              = bm.bmWidth;
        bi.biHeight             = bm.bmHeight;
        bi.biPlanes             = 1;
        bi.biBitCount           = (WORD)(bm.bmPlanes * bm.bmBitsPixel);
        bi.biCompression        = dwCompression;
        bi.biSizeImage          = 0;
        bi.biXPelsPerMeter      = 0;
        bi.biYPelsPerMeter      = 0;
        bi.biClrUsed            = 0;
        bi.biClrImportant       = 0;

        // Compute the size of the  infoheader and the color table
        int nColors = (1 << bi.biBitCount);
        if( nColors > 256 ) 
                nColors = 0;
        dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

        // We need a device context to get the DIB from
        hDC = GetDC(NULL);
        hPal = SelectPalette(hDC,hPal,FALSE);
        RealizePalette(hDC);

        // Allocate enough memory to hold bitmapinfoheader and color table
        hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

        if (!hDIB){
                SelectPalette(hDC,hPal,FALSE);
                ReleaseDC(NULL,hDC);
                return NULL;
        }

        lpbi = (LPBITMAPINFOHEADER)hDIB;

        *lpbi = bi;

        // Call GetDIBits with a NULL lpBits param, so the device driver 
        // will calculate the biSizeImage field 
        GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
                        (LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

        bi = *lpbi;

        // If the driver did not fill in the biSizeImage field, then compute it
        // Each scan line of the image is aligned on a DWORD (32bit) boundary
        if (bi.biSizeImage == 0){
                bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
                                                * bi.biHeight;

                // If a compression scheme is used the result may infact be larger
                // Increase the size to account for this.
                if (dwCompression != BI_RGB)
                        bi.biSizeImage = (bi.biSizeImage * 3) / 2;
        }

        // Realloc the buffer so that it can hold all the bits
        dwLen += bi.biSizeImage;
        if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
                hDIB = handle;
        else{
                GlobalFree(hDIB);

                // Reselect the original palette
                SelectPalette(hDC,hPal,FALSE);
                ReleaseDC(NULL,hDC);
                return NULL;
        }

        // Get the bitmap bits
        lpbi = (LPBITMAPINFOHEADER)hDIB;

        // FINALLY get the DIB
        BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
                                0L,                             // Start scan line
                                (DWORD)bi.biHeight,             // # of scan lines
                                (LPBYTE)lpbi                    // address for bitmap bits
                                + (bi.biSize + nColors * sizeof(RGBQUAD)),
                                (LPBITMAPINFO)lpbi,             // address of bitmapinfo
                                (DWORD)DIB_RGB_COLORS);         // Use RGB for color table

        if( !bGotBits )
        {
                GlobalFree(hDIB);
                
                SelectPalette(hDC,hPal,FALSE);
                ReleaseDC(NULL,hDC);
                return NULL;
        }

        SelectPalette(hDC,hPal,FALSE);
        ReleaseDC(NULL,hDC);
        return hDIB;
}

HBITMAP CGLib::DIBtoDDB( HANDLE hDIB )
{
    LPBITMAPINFOHEADER      lpbi;
    HBITMAP                 hbm;
    CPalette                pal;
    CPalette*               pOldPal = NULL;
    CClientDC               dc(NULL);

    if (hDIB == NULL)
            return NULL;

//	BITMAPINFOHEADER bmHeader;
	lpbi = (LPBITMAPINFOHEADER)::GlobalLock(hDIB);
//	memcpy(&bmHeader, lpbi, sizeof(BITMAPINFOHEADER));

	int nColors = lpbi->biClrUsed ? lpbi->biClrUsed : 
                        1 << lpbi->biBitCount;

	::GlobalUnlock(lpbi);
//	int nColors = bmHeader.biClrUsed ? bmHeader.biClrUsed : 
//                        1 << bmHeader.biBitCount;

    BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
    LPVOID lpDIBBits;
    if( bmInfo.bmiHeader.biBitCount > 8 )
            lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
                    bmInfo.bmiHeader.biClrUsed) + 
                    ((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
    else
            lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

    // Create and select a logical palette if needed
    if( nColors <= 256 && dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
    {
            UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
            LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

            pLP->palVersion = 0x300;
            pLP->palNumEntries = (WORD)nColors;

            for( int i=0; i < nColors; i++)
            {
                    pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
                    pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
                    pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
                    pLP->palPalEntry[i].peFlags = 0;
            }

            pal.CreatePalette( pLP );

            delete[] pLP;

            // Select and realize the palette
            pOldPal = dc.SelectPalette( &pal, FALSE );
            dc.RealizePalette();
    }

    
    hbm = CreateDIBitmap(dc.GetSafeHdc(),           // handle to device context
                    (LPBITMAPINFOHEADER)lpbi,       // pointer to bitmap info header 
                    (LONG)CBM_INIT,                 // initialization flag
                    lpDIBBits,                      // pointer to initialization data 
                    (LPBITMAPINFO)lpbi,             // pointer to bitmap info
                    DIB_RGB_COLORS );               // color-data usage 

    if (pal.GetSafeHandle())
            dc.SelectPalette(pOldPal,FALSE);

    return hbm;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : StretchDibBlt( HDC hdc,                                    *
 *                              int x, int y,                               *
 *                              int dx, int dy,                             *
 *                              HANDLE hdib,                                *
 *                              int x0, int y0,                             *
 *                              int dx0, int dy0,                           *
 *                              LONG rop)                                   *
 *                                                                          *
 *  PURPOSE    : Draws a bitmap in CF_DIB format, using StretchDIBits()     *
 *               taking the same parameters as StretchBlt().                *
 *                                                                          *
 *  RETURNS    : TRUE  - if function succeeds.                              *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
BOOL CGLib::StretchDibBlt (
    HDC hdc,
    INT x,
    INT y,
    INT dx,
    INT dy,
    HANDLE hdib,
    INT x0,
    INT y0,
    INT dx0,
    INT dy0,
    LONG rop)

{
    LPBITMAPINFOHEADER lpbi;
    LPSTR        pBuf;
    BOOL         f;

    if (!hdib)
        return PatBlt(hdc,x,y,dx,dy,rop);

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    if (!lpbi)
        return FALSE;

    pBuf = (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi);

    f = StretchDIBits ( hdc,
                        x, y,
                        dx, dy,
                        x0, y0,
                        dx0, dy0,
                        pBuf, (LPBITMAPINFO)lpbi,
                        DIB_RGB_COLORS,
                        rop);

    GlobalUnlock(hdib);
    return f;
}

/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)   ((i+31)/32*4)

BOOL CGLib::DibInfo (
    HANDLE hbi,
    LPBITMAPINFOHEADER lpbi)
{
    if (hbi){
        *lpbi = *(LPBITMAPINFOHEADER)GlobalLock (hbi);

        /* fill in the default fields */
        if (lpbi->biSize != sizeof (BITMAPCOREHEADER)){
            if (lpbi->biSizeImage == 0L)
                                lpbi->biSizeImage = WIDTHBYTES(lpbi->biWidth*lpbi->biBitCount) * lpbi->biHeight;

            if (lpbi->biClrUsed == 0L)
                                lpbi->biClrUsed = DibNumColors (lpbi);
    }
        GlobalUnlock (hbi);
        return TRUE;
    }
    return FALSE;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  PaletteSize(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  *
 *                is of the BITMAPCOREHEADER type, the number of colors is  *
 *                multiplied by 3 to give the palette size, otherwise the   *
 *                number of colors is multiplied by 4.                                                          *
 *                                                                          *
 *  RETURNS    :  Palette size in number of bytes.                          *
 *                                                                          *
 ****************************************************************************/
WORD CGLib::PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibNumColors(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *               the BitCount filed in the info block.                      *
 *                                                                          *
 *  RETURNS    : The number of colors in the DIB.                           *
 *                                                                          *
 ****************************************************************************/
WORD CGLib::DibNumColors (VOID FAR * pv)
{
    INT                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    }
    else
        bits = lpbc->bcBitCount;

    switch (bits){
        case 1:
                return 2;
        case 4:
                return 16;
        case 8:
                return 256;
        default:
                /* A 24 bitcount DIB has no color table */
                return 0;
    }
}

#define BFT_BITMAP 0x4d42   /* 'BM' */
#define SIZEOF_BITMAPFILEHEADER_PACKED  (   \
    sizeof(WORD) +      /* bfType      */   \
    sizeof(DWORD) +     /* bfSize      */   \
    sizeof(WORD) +      /* bfReserved1 */   \
    sizeof(WORD) +      /* bfReserved2 */   \
    sizeof(DWORD))      /* bfOffBits   */

// WriteDIB : 투명도 자료용의 비트맵 저장.
BOOL CGLib::WriteDIB(CArchive& ar, HANDLE hdib)
{
    BITMAPFILEHEADER    hdr;
    LPBITMAPINFOHEADER  lpbi;

    if (!hdib)
        return FALSE;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock (hdib);

    /* Fill in the fields of the file header */
    hdr.bfType          = BFT_BITMAP;
    hdr.bfSize          = GlobalSize (hdib) + SIZEOF_BITMAPFILEHEADER_PACKED;
    hdr.bfReserved1     = 0;
    hdr.bfReserved2     = 0;
    hdr.bfOffBits       = (DWORD) (SIZEOF_BITMAPFILEHEADER_PACKED + lpbi->biSize +
                          PaletteSize(lpbi));

    /* Write the file header */
#ifdef  FIXDWORDALIGNMENT
    ar.Write(&hdr, (UINT)(SIZEOF_BITMAPFILEHEADER_PACKED));
#else
	ar.Write(&hdr.bfType, (UINT)sizeof (WORD));
        /* now pass over extra word, and only write next 3 DWORDS!*/
	ar.Write(&hdr.bfSize, sizeof(DWORD) * 3);
#endif
	DWORD nBits = GlobalSize(hdib);      // 추가,,
	ar .Write(&nBits, sizeof(DWORD));    // 추가,,

        /* this struct already DWORD aligned!*/
    /* Write the DIB header and the bits */
    ar.Write(lpbi, nBits);

    GlobalUnlock (hdib);
    return TRUE;
}

// WriteDIB             - Writes a DIB to file
// Returns              - TRUE on success
// szFile               - Name of file to write to
// hDIB                 - Handle of the DIB
BOOL CGLib::WriteDIB(LPCTSTR szFile, HANDLE hDIB)
{
        BITMAPFILEHEADER        hdr;
        LPBITMAPINFOHEADER      lpbi;

        if (!hDIB)
                return FALSE;

        CFile file;
        if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
                return FALSE;

        lpbi = (LPBITMAPINFOHEADER)hDIB;

        int nColors = 1 << lpbi->biBitCount;

        // Fill in the fields of the file header 
        hdr.bfType              = ((WORD) ('M' << 8) | 'B');    // is always "BM"
        hdr.bfSize              = GlobalSize (hDIB) + sizeof( hdr );
        hdr.bfReserved1         = 0;
        hdr.bfReserved2         = 0;
        hdr.bfOffBits           = (DWORD) (sizeof( hdr ) + lpbi->biSize +
                                                nColors * sizeof(RGBQUAD));

        // Write the file header 
        file.Write( &hdr, sizeof(hdr) );

        // Write the DIB header and the bits 
        file.Write( lpbi, GlobalSize(hDIB) );

        return TRUE;
}

BOOL CGLib::LoadBMPImage( LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal )
{
	CFile file;
	if( !file.Open( sBMPFile, CFile::modeRead) )
		return FALSE;

	BITMAPFILEHEADER bmfHeader;

	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;

	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;

	// Get length of the remainder of the file and allocate memory
	DWORD nPackedDIBLen = file.GetLength() - sizeof(BITMAPFILEHEADER);
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if (hDIB == 0)
		return FALSE;

	// Read the remainder of the bitmap file.
	if (file.Read((LPSTR)hDIB, nPackedDIBLen) != nPackedDIBLen )
	{
		::GlobalFree(hDIB);
		return FALSE;
	}


	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;

	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
						1 << bmiHeader.biBitCount;

	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
			((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	// Create the logical palette
	if( pPal != NULL )
	{
		// Create the palette
		if( nColors <= 256 )
		{
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

			pLP->palVersion = 0x300;
			pLP->palNumEntries = (WORD)nColors;

			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
				pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}

			pPal->CreatePalette( pLP );

			delete[] pLP;
		}
	}

	CClientDC dc(NULL);
	CPalette* pOldPalette = NULL;
	if( pPal )
	{
		pOldPalette = dc.SelectPalette( pPal, FALSE );
		dc.RealizePalette();
	}

	HBITMAP hBmp = CreateDIBitmap( dc.m_hDC,		// handle to device context 
				&bmiHeader,	// pointer to bitmap size and format data 
				CBM_INIT,	// initialization flag 
				lpDIBBits,	// pointer to initialization data 
				&bmInfo,	// pointer to bitmap color-format data 
				DIB_RGB_COLORS);		// color-data usage 
	bitmap.Attach( hBmp );

	if( pOldPalette )
		dc.SelectPalette( pOldPalette, FALSE );

	::GlobalFree(hDIB);
	return TRUE;
}

#define DEGREE_VALUE 1.6666666667
CString CGLib::ConvertTextDMS(double Degree)
{
	double Do, Bun, Cho, tmp, Min;

	Do  = (int)Degree;
	tmp = (Degree - Do)/DEGREE_VALUE + 0.000001;
	Bun = (int)(tmp * 100);  //분을 추출한다.
	Min =  Bun / 100;
	Cho = (tmp - Min)/DEGREE_VALUE * 10000; //초단위를 추출한다.

	CString Result;
	Result.Format("%3.0f.%02.0f%02.0f", Do, Bun, Cho);
	return Result;
}

CString CGLib::ConvertShortDMS(double Degree)
{
	double Do, Bun, Cho, tmp, Min;

	Do  = (int)Degree;
	tmp = (Degree - Do)/DEGREE_VALUE + 0.000001;
	Bun = (int)(tmp * 100);  //분을 추출한다.
	Min =  Bun / 100;
	Cho = (tmp - Min)/DEGREE_VALUE * 10000; //초단위를 추출한다.

	CString Result;
	Result.Format("%2.0f%02.0f%02.0f", Do, Bun, Cho );

	return Result;

}

#define OFFSET 0.001
double CGLib::GetLatOffset()
{
	double lat = 0.0;
	int nMapScale = CGMapLib::GetMapScale();
	switch(nMapScale) {
		case 25000:
			lat = OFFSET;
			break;
		case 50000:
			lat = OFFSET*2;
			break;
		case 100000:
			lat = OFFSET*4;
			break;
		case 250000:
			lat = OFFSET*8;
			break;
		case 500000:
			lat = OFFSET*16;
			break;
		case 1000000:
			lat = OFFSET*32;
			break;
			break;
	}

	return lat;
}

double CGLib::GetLonOffset()
{
	double lon = 0.0;
	int nMapScale = CGMapLib::GetMapScale();
	switch(nMapScale) {
		case 25000:
			lon = OFFSET;
			break;
		case 50000:
			lon = OFFSET*2;
			break;
		case 100000:
			lon = OFFSET*4;
			break;
		case 250000:
			lon = OFFSET*8;
			break;
		case 500000:
			lon = OFFSET*16;
			break;
		case 1000000:
			lon = OFFSET*32;
			break;
	}

	return lon;
}

/*
CString CGLib::GetSystemOSVersion()
{
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure,
   // which is supported on Windows 2000.
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) &osvi)) )
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (!GetVersionEx( (OSVERSIONINFO *) &osvi) )
         return "none";
   }

   switch (osvi.dwPlatformId)
   {
      case VER_PLATFORM_WIN32_NT:

	      // Test for the product.
         if ( osvi.dwMajorVersion <= 4 )
			 return "Windows NT";

         if ( osvi.dwMajorVersion == 5 )
			 return "Windows 2000";

	     // Test for workstation versus server.
         if( bOsVersionInfoEx )
         {
//            if ( osvi.wProductType == VER_NT_WORKSTATION )
//               return "Professional ";
//
//            if ( osvi.wProductType == VER_NT_SERVER )
//               return "Server ";
			 ;
         }
         else
         {
            HKEY hKey;
            char szProductType[80];
            DWORD dwBufLen;

            RegOpenKeyEx( HKEY_LOCAL_MACHINE,
               "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
               0, KEY_QUERY_VALUE, &hKey );
            RegQueryValueEx( hKey, "ProductType", NULL, NULL,
               (LPBYTE) szProductType, &dwBufLen);
            RegCloseKey( hKey );
            if ( lstrcmpi( "WINNT", szProductType) == 0 )
               return "Workstation ";
            if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
               return "Server ";
         }

      // Display version, service pack (if any), and build number.
       //printf ("version %d.%d %s (Build %d)\n",
       //           osvi.dwMajorVersion,
       //           osvi.dwMinorVersion,
       //           osvi.szCSDVersion,
       //           osvi.dwBuildNumber & 0xFFFF);
         break;

      case VER_PLATFORM_WIN32_WINDOWS:

         if ((osvi.dwMajorVersion > 4) || 
            ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0)))
         {
             return "Windows 98";
         } 
         else 
			 return "Windows 95";

         break;

      case VER_PLATFORM_WIN32s:
			return "Microsoft Win32s";
         break;
   }

   return "none"; 
}
*/

//hwp가 링크된 실행 경로를 찾는다.
/*
CString CGLib::GetHwpExecutePath()
{
	CString strExecPath;

	CGEnvVar env("AFC4I_SGIS_RELATED_INFO");
	CString PathName = env.GetValue();
	CString strHwpPath;
	strHwpPath = PathName + "\\HwpPath.log";
	FILE* stream;
	if( (stream = fopen(strHwpPath, "r")) == NULL) {
		CGMessenger::SetMessage("HwpPath.log 파일을 찾을수가 없습니다.", 
		CGMessenger::ToDialog, MB_OK|MB_ICONINFORMATION);
		return "";
	}

	char strExecHwp[128];
	fscanf(stream, "%s", strExecHwp);
	fclose(stream);

	//SGIS_TRACE(__FILE__, __LINE__, "Execute Hwp File: %s", strExecHwp);
	return (CString)strExecHwp;
}
*/

// jwpark add 2005.11.24
double CGLib::GetMinZoom(BOOL bIsAutoMode, long scale)
{
	if(bIsAutoMode == TRUE)
	{
		switch(scale)
		{
			case 50000 : return 0.01;
			case 100000 : return 0.02;
			case 250000 : return 0.05;
			case 500000 : return 0.1;
			case 1000000 : return 0.2;
		}
	}
	else
	{
		switch(scale)
		{
			case 50000 : return 2.0;
			case 100000 : return 2.0;
			case 250000 : return 1.0;
			case 500000 : return 1.0;
			case 1000000 : return 0.2;
		}
/*
		switch(scale)
		{
			case 50000 : return 2.1;
			case 100000 : return 2.1;
			case 250000 : return 1.1;
			case 500000 : return 1.1;
			case 1000000 : return 0.2;
		}
*/
	}

	return MIN_ZOOM;
}

double CGLib::GetMaxZoom(long scale)
{
	// 1:500 이하만 처리
	switch(scale)
	{
		case 50000 : return 100.0;
		case 100000 : return 200.0;
		case 250000 : return 500.0;
		case 500000 : return 1000.0;
		case 1000000 : return 2000.0;
	}

	return MAX_ZOOM;
}
// jwpark update end


void CGLib::SetCurrentMapInfo(int iScale, int iRatio)
{
	m_iScale = iScale;
	m_iRatio = iRatio;
}

double CGLib::GetCurrentZoomFactor()
{
	double zoomfactor;
	return zoomfactor = (double)1000000 / (double)m_iRatio;
}

void CGLib::SetMapRect(double dMapLeft, double dMapTop, double dMapRight, double dMapBottom)
{
	m_dMapLeft = dMapLeft;
	m_dMapTop = dMapTop;
	m_dMapRight = dMapRight;
	m_dMapBottom = dMapBottom;
}

void CGLib::SetMapKind(int iVectorMapKind, int iRasterMapKind)
{
	m_iVectorMapKind = iVectorMapKind;
	m_iRasterMapKind = iRasterMapKind;
}

void CGLib::GetMapRect(double &dMapLeft, double &dMapTop, double &dMapRight, double &dMapBottom)
{
	dMapLeft = m_dMapLeft;
	dMapTop = m_dMapTop;
	dMapRight = m_dMapRight;
	dMapBottom = m_dMapBottom;
}

void CGLib::GetMapKind(int &iVectorMapKind, int &iRasterMapKind)
{
	iVectorMapKind = m_iVectorMapKind;
	iRasterMapKind = m_iRasterMapKind;
}

CPoint CGLib::GetEndPoint(CPoint point, double dAngle, long ldistance)
{
	CPoint endpoint;
	double dradian = Angle2Radian(dAngle);
	endpoint.x = (long)(ldistance * cos(dradian) + point.x);
	endpoint.y = (long)(ldistance * sin(dradian) + point.y);

	return endpoint;
}