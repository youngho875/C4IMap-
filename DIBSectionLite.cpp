// CDIBSectionLite.cpp : implementation file
//
// General purpose DIBsection wrapper class for Win9x, NT 4.0
//
// Author      : Chris Maunder (cmaunder@dundas.com)
// Date        : 17 May 1999
//
// Copyright ?Dundas Software Ltd. 1999, All Rights Reserved                      
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage, in any form, caused
// by this code. Use it at your own risk and as with all code expect bugs!
// It's been tested but I'm not perfect.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file.
//
// History :  25 May 1999 - First release
//             4 Jun 1999 - Fixed SetBitmap bug

#include "pch.h"
//#include "../LIBSrc/BMGLib/BMGGeoTIFF_DLL/BMGGeoTIFF.h"
#include "DIBSectionLite.h"
#include "gdal_priv.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Standard colours
PALETTEENTRY CDIBSectionLite::ms_StdColours[] = {
    { 0x00, 0x00, 0x00, 0 },    // First 2 will be palette for monochrome bitmaps
    { 0xFF, 0xFF, 0xFF, 0 },

    { 0x00, 0x00, 0xFF, 0 },    // First 16 will be palette for 16 colour bitmaps
    { 0x00, 0xFF, 0x00, 0 },
    { 0x00, 0xFF, 0xFF, 0 },
    { 0xFF, 0x00, 0x00, 0 },
    { 0xFF, 0x00, 0xFF, 0 },
    { 0xFF, 0xFF, 0x00, 0 },

    { 0x00, 0x00, 0x80, 0 },
    { 0x00, 0x80, 0x00, 0 },
    { 0x00, 0x80, 0x80, 0 },
    { 0x80, 0x00, 0x00, 0 },
    { 0x80, 0x00, 0x80, 0 },
    { 0x80, 0x80, 0x00, 0 },
    { 0x80, 0x80, 0x80, 0 },

    { 0xC0, 0xC0, 0xC0, 0 },
};

/////////////////////////////////////////////////////////////////////////////
// CDIBSectionLite static functions

// --- In?: nBitsPerPixel - bits per pixel
// --- Out : 
// --- Returns :The number of colours for this colour depth
// --- Effect : Returns the number of color table entries given the number
//              of bits per pixel of a bitmap
/*static*/ int CDIBSectionLite::NumColorEntries(int nBitsPerPixel) 
{
    int nColors = 0;

    switch (nBitsPerPixel) 
    {
	    case 1:  nColors = 2;   break;
        case 4:  nColors = 16;  break;
        case 8:  nColors = 256; break;
        case 16:
        case 24:
        case 32: nColors = 0;   break; // 16,24 or 32 bpp have no color table

        default:
           ASSERT(FALSE);
    }

    return nColors;
}

// --- In?: nWidth - image width in pixels
//           nBitsPerPixel - bits per pixel
// --- Out :
// --- Returns : Returns the number of storage bytes needed for each scanline 
//               in the bitmap
// --- Effect : 
/*static*/ int CDIBSectionLite::BytesPerLine(int nWidth, int nBitsPerPixel)
{
    int nBytesPerLine = nWidth * nBitsPerPixel;
    nBytesPerLine = ( (nBytesPerLine + 31) & (~31) ) / 8;

    return nBytesPerLine;
}

// --- In?: palette - reference to a palette object which will be filled
//           nNumColours - number of colour entries to fill
// --- Out :
// --- Returns : TRUE on success, false otherwise
// --- Effect : Creates a halftone color palette independant of screen color depth.
//              palette will be filled with the colors, and nNumColours is the No.
//              of colors to file. If nNumColoursis 0 or > 256, then 256 colors are used.
/*static*/ BOOL CDIBSectionLite::CreateHalftonePalette(CPalette& palette, int nNumColours)
{
    palette.DeleteObject();

    int nNumStandardColours = sizeof(ms_StdColours) / sizeof(ms_StdColours[0]);
    int nIndex = 0;
    int nNumEntries = nNumColours;
    if (nNumEntries <= 0 || nNumEntries > 256)
        nNumEntries = 256;

    PALETTEINFO PalInfo;                   
    PalInfo.palNumEntries = (WORD) nNumEntries;

    // The standard colours (16)
    for (int i = 0; i < nNumStandardColours; i++)
    {
        if (nIndex >= nNumEntries) break;
        memcpy( &(PalInfo.palPalEntry[nIndex]), &(ms_StdColours[i]), sizeof(PALETTEENTRY) );
        nIndex++;
    }

    // A colour cube (6 divisions = 216)
    for (int blue = 0; blue <= 5; blue++)
        for (int green = 0; green <= 5; green++)
            for (int red = 0; red <= 5; red++)
            {
                if (nIndex >= nNumEntries)
                    break;

                PalInfo.palPalEntry[nIndex].peRed   = (BYTE) ((red*255)/5);
                PalInfo.palPalEntry[nIndex].peGreen = (BYTE) ((green*255)/5);
                PalInfo.palPalEntry[nIndex].peBlue  = (BYTE) ((blue*255)/5);
                PalInfo.palPalEntry[nIndex].peFlags = 0;
                nIndex++;
            }

    // A grey scale (24 divisions)
    for (int grey = 0; grey <= 23; grey++)
    {
        if (nIndex >= nNumEntries) 
            break;

        PalInfo.palPalEntry[nIndex].peRed   = (BYTE) (grey*255/23);
        PalInfo.palPalEntry[nIndex].peGreen = (BYTE) (grey*255/23);
        PalInfo.palPalEntry[nIndex].peBlue  = (BYTE) (grey*255/23);
        PalInfo.palPalEntry[nIndex].peFlags = 0;
        nIndex++;
    }

    return palette.CreatePalette((LPLOGPALETTE) PalInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CDIBSectionLite

CDIBSectionLite::CDIBSectionLite()
{
    m_hBitmap = NULL;
    m_bDither = FALSE;
    m_hDrawDib = NULL;
	m_dFixelX = m_dFixelY = 0.0;
	m_ppvBits = NULL;
	m_pPalette = NULL;
	m_CoordMgr = NULL;

	//m_pGeoObj = (CGGeoObject *) new CGGeoObjGP;
	//m_pGeoObj->SetEllipsoid(EWGE);			// 타원체를 WGS84로 지정함
	//m_pGeoObj->SetMapMode(GMM_UTM);
	//m_pGeoObj->SetProjBase(129.0, 0.0);		
	//m_pGeoObj->SetProjScaleFactor(0.9996);	// 52 zone UTM투영의 원점
	//m_pGeoObj->SetValidity(1);				// 1m 단위의 유효숫자로 지정함 : 우선은 1m로 함

	DeleteObject(); // This will initialise to a known state - ie. empty

	//GDALAllRegister();
}

CDIBSectionLite::~CDIBSectionLite()
{
    DeleteObject();

	for(int i=0; i<m_pGDALColorTable.GetSize(); i++)
		delete m_pGDALColorTable.GetAt(i);
	m_pGDALColorTable.RemoveAll();

	if(!m_pPalette)
		GlobalFree((HGLOBAL)m_pPalette);
}

// --- In?:
// --- Out :
// --- Returns :
// --- Effect : Resets the object to an empty state, and frees all memory used.
void CDIBSectionLite::DeleteObject()
{
    if (m_hBitmap)
        ::DeleteObject(m_hBitmap);
    m_hBitmap = NULL;

	m_ppvBits = NULL;

    memset(&m_DIBinfo, 0, sizeof(m_DIBinfo));

    if (m_hDrawDib)
        DrawDibClose(m_hDrawDib);
    m_hDrawDib = NULL;

    m_iColorDataType = DIB_RGB_COLORS;
    m_iColorTableSize = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDIBSectionLite diagnostics

#ifdef _DEBUG
void CDIBSectionLite::AssertValid() const
{
    ASSERT(m_hBitmap);

    DIBSECTION ds;
    DWORD dwSize = GetObject( m_hBitmap, sizeof(DIBSECTION), &ds );
    ASSERT(dwSize == sizeof(DIBSECTION));

    ASSERT(0 <= m_iColorTableSize && m_iColorTableSize <= 256);

	CObject::AssertValid();
}

void CDIBSectionLite::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}

#endif //_DEBUG

// --- In?: bDither - whether or not dithering should be enabled
// --- Out :
// --- Returns : TRUE on success
// --- Effect : Turns dithering on by using the DrawDib functions instead of 
//              the GDI functions
BOOL CDIBSectionLite::SetDither(BOOL bDither)
{
    BOOL bResult = TRUE;

    // Check for a no-change op.
    if ( (m_bDither == bDither) &&
         ((m_hDrawDib && bDither) || (!m_hDrawDib && !bDither)) )
        return bResult;

    if (m_hDrawDib != NULL)
    {
        DrawDibClose(m_hDrawDib);
        m_hDrawDib = NULL;
    }

    if (bDither)
    {
        if ( (m_hDrawDib = DrawDibOpen()) == NULL )
            bResult = FALSE;
    }

    m_bDither = (m_hDrawDib != NULL);

    return bResult;
}

// --- In?: 
// --- Out :
// --- Returns : TRUE if dithering is used
// --- Effect : Returns whether or not the DrawDib functions (and hence dithering)
//              is being used.
BOOL CDIBSectionLite::GetDither()
{
    return (m_bDither && (m_hDrawDib != NULL));
}

/////////////////////////////////////////////////////////////////////////////
// CDIBSectionLite operations

// --- In?: pDC - Pointer to a device context
//           ptDest - point at which the topleft corner of the image is drawn
//           bForceBackground - Specifies whether the palette is forced to be 
//                              a background palette
// --- Out :
// --- Returns : TRUE on success
// --- Effect : Draws the image 1:1 on the device context

BOOL CDIBSectionLite::Draw(CDC* pDC, double left, double top, double right, double bottom)
{
	double x, y, dx, dy;
	BOOL bResult = FALSE;
	m_CoordMgr = GP_COORDMGR;

    CPalette* pOldPalette = NULL;
    if(m_Palette.m_hObject && UsesPalette(pDC))
    {
        pOldPalette = pDC->SelectPalette(&m_Palette, FALSE);
        pDC->RealizePalette();
    }

	//CGRectGP rectMapGP(left, top, right, bottom);
	CRect rect;
	m_Coord.degrees_lat_long_to_x_y (top, left, &x, &y); 
	m_Coord.degrees_lat_long_to_x_y (bottom, right, &dx, &dy); 
	m_CoordMgr->WorldXY2ScreenXY(x, y, &rect.left, &rect.top);
	m_CoordMgr->WorldXY2ScreenXY(dx, dy, &rect.right, &rect.bottom);

	//pDC->SetBkColor(RGB(128,128,128));
	pDC->SetStretchBltMode(COLORONCOLOR);

	bResult = ::StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(),
		            0, 0, GetWidth(), GetHeight(),
				    GetDIBits(), GetBitmapInfo(),
				    DIB_RGB_COLORS, SRCCOPY);

	return TRUE;
}

BOOL CDIBSectionLite::DrawCADRG(CDC* pDC, double left, double top, double right, double bottom)
{
	double x, y, dx, dy;
	BOOL bResult = FALSE;
	m_CoordMgr = GP_COORDMGR;

	//	CPalette* pOldPalette = NULL;
	//	if(m_Palette.m_hObject && UsesPalette(pDC))
	//	{
	//		pOldPalette = pDC->SelectPalette(&m_Palette, FALSE);
	//		pDC->RealizePalette();
	//	}
	

	//pGDCgeo->GeoPtoLP(&rect, &rectMapGP);
	CRect rect;
	m_Coord.degrees_lat_long_to_x_y (top, left, &x, &y); 
	m_Coord.degrees_lat_long_to_x_y (bottom, right, &dx, &dy);
	m_CoordMgr->WorldXY2ScreenXY(x, y, &rect.left, &rect.top);
	m_CoordMgr->WorldXY2ScreenXY(dx, dy, &rect.right, &rect.bottom);

	pDC->SetStretchBltMode(COLORONCOLOR);

	// orignal code
	//int iWidth = GetWidth() * m_dFixelX;
	//int iHeight = GetHeight() * m_dFixelY;
	
	// repair code --- young
	int iWidth = rect.Width();// + 12;//m_dFixelX;		// 10
	int iHeight = rect.Height();// - m_dFixelY;

	bResult = ::StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, 
					rect.Width(), rect.Height(), 
					//iWidth, iHeight,
		            0, 0, GetWidth(), GetHeight(),
				    GetDIBits(), GetBitmapInfo(),
 				    DIB_RGB_COLORS, SRCCOPY);

	return TRUE;
}

BOOL CDIBSectionLite::Draw(CDC* pDC, CRect rect)
{
	BOOL bResult = FALSE;

    CPalette* pOldPalette = NULL;
    if (m_Palette.m_hObject && UsesPalette(pDC))
    {
        pOldPalette = pDC->SelectPalette(&m_Palette, FALSE);
        pDC->RealizePalette();
    }

	pDC->SetStretchBltMode(COLORONCOLOR);
	bResult = ::StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(),
		            0, 0, GetWidth(), GetHeight(),
				    GetDIBits(), GetBitmapInfo(),
				    DIB_RGB_COLORS, SRCCOPY);

	return TRUE;
}

BOOL CDIBSectionLite::DrawCIB(CDC* pDC, double left, double top, double right, double bottom)
{
	double x, y, dx, dy;
	BOOL bResult = FALSE;
	m_CoordMgr = GP_COORDMGR;

	//CGRectGP rectMapGP(left, top, right, bottom);
	CRect rect;

	m_Coord.degrees_lat_long_to_x_y (top, left, &x, &y); 
	m_Coord.degrees_lat_long_to_x_y (bottom, right, &dx, &dy);
	m_CoordMgr->WorldXY2ScreenXY(x, y, &rect.left, &rect.top);
	m_CoordMgr->WorldXY2ScreenXY(dx, dy, &rect.right, &rect.bottom);

	//pDC->GeoPtoLP(&rect, &rectMapGP);
	pDC->SetStretchBltMode(COLORONCOLOR);

	int ioffsetX = 1;
	int ioffsetY = 1;

	// orignal code
	//int iWidth = GetWidth() * m_dFixelX + ioffsetX;
	//int iHeight = GetHeight() * m_dFixelY - ioffsetY;

	// repair code --- young
	int iWidth = rect.Width() + m_dFixelX + ioffsetX;
	int iHeight = rect.Height() - m_dFixelY - ioffsetY;

	bResult = ::StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, iWidth, iHeight,
		            0, 0, GetWidth(), GetHeight(),
				    GetDIBits(), GetBitmapInfo(),
				    DIB_RGB_COLORS, SRCCOPY);

	return TRUE;
}

BOOL CDIBSectionLite::Draw(CDC* pDC, CPoint ptDest, BOOL bForceBackground /*=FALSE*/ ) 
{ 
    if (!m_hBitmap)
        return FALSE;

    CSize size = GetSize();
    CPoint SrcOrigin = CPoint(0,0);

    BOOL bResult = FALSE;

    if (GetDither() && GetBitCount() >= 8)
    {
        DrawDibSetPalette( m_hDrawDib, (HPALETTE)m_Palette);
        DrawDibRealize( m_hDrawDib,  pDC->GetSafeHdc(),  bForceBackground);
        
        bResult = DrawDibDraw(m_hDrawDib, pDC->GetSafeHdc(), 
                              ptDest.x, ptDest.y, size.cx, size.cy, 
                              GetBitmapInfoHeader(), GetDIBits(), 
                              SrcOrigin.x, SrcOrigin.y, size.cx, size.cy, 
                              0/*DDF_HALFTONE*/);
    }
    else
    {
        CPalette* pOldPalette = NULL;
        if (m_Palette.m_hObject && UsesPalette(pDC))
        {
            pOldPalette = pDC->SelectPalette(&m_Palette, bForceBackground);
            pDC->RealizePalette();
        }

        bResult = SetDIBitsToDevice(pDC->GetSafeHdc(), 
                                    ptDest.x, ptDest.y, 
                                    size.cx, size.cy,
                                    SrcOrigin.x, SrcOrigin.y,
                                    SrcOrigin.y, size.cy - SrcOrigin.y, 
                                    GetDIBits(), GetBitmapInfo(), 
                                    m_iColorDataType); 

     //   if (pOldPalette)
     //       pDC->SelectPalette(pOldPalette, FALSE);
    }
    
    return bResult;
}

// mbc : draw raster map
BOOL CDIBSectionLite::DrawRaster(CDC *pDC, int nScreenWidth, int nScreenHeight,
								 float fCenterX, float fCenterY, float fScale)	// jykim raster
{
    if (!m_hBitmap)
        return FALSE;

	//m_CoordMgr = GP_COORDMGR;

    CSize size = GetSize();
    CPoint SrcOrigin = CPoint(0,0);
	CPoint ptDest(0, 0);
    BOOL bResult = FALSE;

    if (GetDither() && GetBitCount() >= 8)
    {
        DrawDibSetPalette( m_hDrawDib, (HPALETTE)m_Palette);
        DrawDibRealize( m_hDrawDib,  pDC->GetSafeHdc(),  false);
        
        bResult = DrawDibDraw(m_hDrawDib, pDC->GetSafeHdc(), 
                            ptDest.x, ptDest.y, size.cx, size.cy, 
                              GetBitmapInfoHeader(), GetDIBits(), 
                              SrcOrigin.x, SrcOrigin.y, size.cx, size.cy, 
                              0);
    }
    else
    {
        CPalette* pOldPalette = NULL;
        if (m_Palette.m_hObject && UsesPalette(pDC))
        {
            pOldPalette = pDC->SelectPalette(&m_Palette, false);
            pDC->RealizePalette();
        }

		
		double startx=0., starty=0., endx=0., endy=0.;
		long l_startx=0, l_starty=0, l_endx=0, l_endy=0;


		m_Coord.degrees_lat_long_to_x_y (deg_start_lat, deg_start_long, &startx, &starty);
		m_Coord.degrees_lat_long_to_x_y (deg_end_lat, deg_end_long, &endx, &endy); 

		m_CoordMgr->WorldXY2ScreenXY(startx, starty, &l_startx, &l_starty, 
									(double)fCenterX, (double)fCenterY, (double)fScale);
		m_CoordMgr->WorldXY2ScreenXY(endx, endy, &l_endx, &l_endy,
									(double)fCenterX, (double)fCenterY, (double)fScale);


		size.cx = (l_endx - l_startx);
		size.cy = (l_endy - l_starty);
		
		ptDest = CPoint(l_startx, l_starty); 

        bResult = Stretch(pDC, ptDest, size, m_iColorDataType); 

        if (pOldPalette)
            pDC->SelectPalette(pOldPalette, FALSE);
    }
   
    return bResult;
}

// --- In?: pDC - Pointer to a device context
//           ptDest - point at which the topleft corner of the image is drawn
//           size - size to stretch the image
//           bForceBackground - Specifies whether the palette is forced to be 
//                              a background palette
// --- Out :
// --- Returns : TRUE on success
// --- Effect : Stretch draws the image to the desired size on the device context
BOOL CDIBSectionLite::Stretch(CDC* pDC, CPoint ptDest, CSize size, 
                              BOOL bForceBackground /*=FALSE*/) 
{ 
    if (!m_hBitmap)
        return FALSE;

    CSize imagesize = GetSize();
    CPoint SrcOrigin = CPoint(0,0);

    BOOL bResult = FALSE;

    if (GetDither() && GetBitCount() >= 8)
    {
        DrawDibSetPalette( m_hDrawDib, (HPALETTE)m_Palette);
        DrawDibRealize( m_hDrawDib,  pDC->GetSafeHdc(),  bForceBackground);
        
        bResult = DrawDibDraw(m_hDrawDib, pDC->GetSafeHdc(), 
                              ptDest.x, ptDest.y, size.cx, size.cy, 
                              GetBitmapInfoHeader(), GetDIBits(), 
                              SrcOrigin.x, SrcOrigin.y, imagesize.cx, imagesize.cy, 
                              0/*DDF_HALFTONE*/);
    }
    else
    {
        CPalette* pOldPalette = NULL;
        if (m_Palette.m_hObject && UsesPalette(pDC))
        {
            pOldPalette = pDC->SelectPalette(&m_Palette, bForceBackground);
            pDC->RealizePalette();
        }

        pDC->SetStretchBltMode(COLORONCOLOR);

        bResult = StretchDIBits(pDC->GetSafeHdc(), 
                                ptDest.x, ptDest.y, 
                                size.cx, size.cy,
                                SrcOrigin.x, SrcOrigin.y,
                                imagesize.cx, imagesize.cy, 
                                GetDIBits(), GetBitmapInfo(), 
                                m_iColorDataType, SRCCOPY); 

        if (pOldPalette)
            pDC->SelectPalette(pOldPalette, FALSE);
    }

    return bResult;
}


//////////////////////////////////////////////////////////////////////////////
// Setting the bitmap...


// --- In?: nIDResource - resource ID
// --- Out :
// --- Returns : Returns TRUE on success, FALSE otherwise
// --- Effect : Initialises the bitmap from a resource. If failure, then object is
//              initialised back to an empty bitmap.
BOOL CDIBSectionLite::SetBitmap(UINT nIDResource)
{   
    return SetBitmap(MAKEINTRESOURCE(nIDResource));
}

// --- In?: lpszResourceName - resource name
// --- Out :
// --- Returns : Returns TRUE on success, FALSE otherwise
// --- Effect : Initialises the bitmap from a resource. If failure, then object is
//              initialised back to an empty bitmap.
BOOL CDIBSectionLite::SetBitmap(LPCTSTR lpszResourceName)
{
    HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetResourceHandle(), 
                                        lpszResourceName,
                                        IMAGE_BITMAP, 
                                        0,0, 
                                        LR_CREATEDIBSECTION);

    if (!hBmp) return FALSE;

    BOOL bResult = SetBitmap(hBmp);
    ::DeleteObject(hBmp);
    return bResult;
}

// --- In?: lpBitmapInfo - pointer to a BITMAPINFO structure
//           lpBits - pointer to image bits
// --- Out :
// --- Returns : Returns TRUE on success, FALSE otherwise
// --- Effect : Initialises the bitmap using the information in lpBitmapInfo to determine
//              the dimensions and colours, and the then sets the bits from the bits in
//              lpBits. If failure, then object is initialised back to an empty bitmap.
BOOL CDIBSectionLite::SetBitmap(LPBITMAPINFO lpBitmapInfo, LPVOID lpBits)
{
    DeleteObject();

    if (!lpBitmapInfo || !lpBits)
        return FALSE;

    HDC hDC = NULL;
    try {
        BITMAPINFOHEADER& bmih = lpBitmapInfo->bmiHeader;

        // Compute the number of colours in the colour table
        m_iColorTableSize = NumColorEntries(bmih.biBitCount);

        DWORD dwBitmapInfoSize = sizeof(BITMAPINFO) + m_iColorTableSize*sizeof(RGBQUAD);

        // Copy over BITMAPINFO contents
        memcpy(&m_DIBinfo, lpBitmapInfo, dwBitmapInfoSize);

        // Should now have all the info we need to create the sucker.
        //TRACE(_T("Width %d, Height %d, Bits/pixel %d, Image Size %d\n"),
        //      bmih.biWidth, bmih.biHeight, bmih.biBitCount, bmih.biSizeImage);

        // Create a DC which will be used to get DIB, then create DIBsection
        hDC = ::GetDC(NULL);
        if (!hDC) 
        {
            TRACE0("Unable to get DC\n");
            AfxThrowResourceException();
        }

        m_hBitmap = CreateDIBSection(hDC, (const BITMAPINFO*) m_DIBinfo,
                                     m_iColorDataType, &m_ppvBits, NULL, 0);
        ::ReleaseDC(NULL, hDC);
        if (!m_hBitmap)
        {
            TRACE0("CreateDIBSection failed\n");
            AfxThrowResourceException();
        }

        DWORD dwImageSize = m_DIBinfo.bmiHeader.biSizeImage;
        if (dwImageSize == 0)
        {
            int nBytesPerLine = BytesPerLine(lpBitmapInfo->bmiHeader.biWidth, 
                                             lpBitmapInfo->bmiHeader.biBitCount);
            dwImageSize = nBytesPerLine * lpBitmapInfo->bmiHeader.biHeight;
        }

        // Flush the GDI batch queue 
        GdiFlush();

        memcpy(m_ppvBits, lpBits, dwImageSize);

//		if(!CreatePalette2())
//		{
//			TRACE0("Unable to create palette\n");
//			AfxThrowResourceException();
//		}
    }
    catch (CException *e)
    {
        e->Delete();
        _ShowLastError();
        if (hDC)
            ::ReleaseDC(NULL, hDC);
        DeleteObject();
        return FALSE;
    }

    return TRUE;
}

/*
// --- In?: lpBitmapInfo - pointer to a BITMAPINFO structure
//           poColorTable - pointer to a GDalColorTable class
//           lpBits - pointer to image bits
// --- Out :
// --- Returns : Returns TRUE on success, FALSE otherwise
// --- Effect : Initialises the bitmap using the information in lpBitmapInfo to determine
//              the dimensions and colours, and the then sets the bits from the bits in
//              lpBits. If failure, then object is initialised back to an empty bitmap.
BOOL CDIBSectionLite::SetBitmap(LPBITMAPINFO lpBitmapInfo, GDALColorTable *poColorTable, LPVOID lpBits)
{
    DeleteObject();

    if (!lpBitmapInfo || !lpBits)
        return FALSE;

    HDC hDC = NULL;
    try {
        BITMAPINFOHEADER& bmih = lpBitmapInfo->bmiHeader;

        // Compute the number of colours in the colour table
        m_iColorTableSize = NumColorEntries(bmih.biBitCount);

        DWORD dwBitmapInfoSize = sizeof(BITMAPINFO) + m_iColorTableSize*sizeof(RGBQUAD);

        // Copy over BITMAPINFO contents
        memcpy(&m_DIBinfo, lpBitmapInfo, dwBitmapInfoSize);

        // Should now have all the info we need to create the sucker.
        //TRACE(_T("Width %d, Height %d, Bits/pixel %d, Image Size %d\n"),
        //      bmih.biWidth, bmih.biHeight, bmih.biBitCount, bmih.biSizeImage);

        // Create a DC which will be used to get DIB, then create DIBsection
        hDC = ::GetDC(NULL);
        if (!hDC) 
        {
            TRACE0("Unable to get DC\n");
            AfxThrowResourceException();
        }

        m_hBitmap = CreateDIBSection(hDC, (const BITMAPINFO*) m_DIBinfo,
                                     m_iColorDataType, &m_ppvBits, NULL, 0);
        ::ReleaseDC(NULL, hDC);
        if (!m_hBitmap)
        {
            TRACE0("CreateDIBSection failed\n");
            AfxThrowResourceException();
        }

        DWORD dwImageSize = m_DIBinfo.bmiHeader.biSizeImage;
        if (dwImageSize == 0)
        {
            int nBytesPerLine = BytesPerLine(lpBitmapInfo->bmiHeader.biWidth, 
                                             lpBitmapInfo->bmiHeader.biBitCount);
            dwImageSize = nBytesPerLine * lpBitmapInfo->bmiHeader.biHeight;
        }

        // Flush the GDI batch queue 
        GdiFlush();

        memcpy(m_ppvBits, lpBits, dwImageSize);

        if (!CreatePalette())
        {
            TRACE0("Unable to create palette\n");
            AfxThrowResourceException();
        }
    }
    catch (CException *e)
    {
        e->Delete();
        _ShowLastError();
        if (hDC) 
            ::ReleaseDC(NULL, hDC);
        DeleteObject();
        return FALSE;
    }

    return TRUE;
}
*/

// --- In?: hBitmap - handle to image
//           pPalette - optional palette to use when setting image
// --- Out :
// --- Returns : Returns TRUE on success, FALSE otherwise
// --- Effect : Initialises the bitmap from the HBITMAP supplied. If failure, then
//              object is initialised back to an empty bitmap.
BOOL CDIBSectionLite::SetBitmap(HBITMAP hBitmap, CPalette* pPalette /*= NULL*/)
{
    DeleteObject();

    if (!hBitmap)
        return FALSE;

    // Get dimensions of bitmap
    BITMAP bm;
    if (!::GetObject(hBitmap, sizeof(bm),(LPVOID)&bm))
        return FALSE;
    bm.bmHeight = abs(bm.bmHeight);

    CWindowDC dc(NULL);
    CPalette* pOldPalette = NULL;

    try {
        m_iColorTableSize = NumColorEntries(bm.bmBitsPixel);

        // Initialize the BITMAPINFOHEADER in m_DIBinfo
        BITMAPINFOHEADER& bih = m_DIBinfo.bmiHeader;
        bih.biSize          = sizeof(BITMAPINFOHEADER);
        bih.biWidth         = bm.bmWidth;
        bih.biHeight        = bm.bmHeight;
        bih.biPlanes        = 1;                // Must always be 1 according to docs
        bih.biBitCount      = bm.bmBitsPixel;
        bih.biCompression   = BI_RGB;
        bih.biSizeImage     = BytesPerLine(bm.bmWidth, bm.bmBitsPixel) * bm.bmHeight;
        bih.biXPelsPerMeter = 0;
        bih.biYPelsPerMeter = 0;
        bih.biClrUsed       = 0;
        bih.biClrImportant  = 0;

        // calls GetDIBits with NULL bits pointer to fill in the BITMAPINFOHEADER data
        if (!::GetDIBits(dc.m_hDC, hBitmap, 0, bm.bmHeight, NULL, m_DIBinfo, m_iColorDataType))
        {
            TRACE0("Unable to GetDIBits\n");
            AfxThrowResourceException();
        }

        // If the driver did not fill in the biSizeImage field, then compute it
        // Each scan line of the image is aligned on a DWORD (32bit) boundary
        if (bih.biSizeImage == 0)
            bih.biSizeImage = BytesPerLine(bih.biWidth, bih.biBitCount) * bih.biHeight;

        if (pPalette)
            SetPalette(pPalette);
        else
            CreateHalftonePalette(m_Palette, m_iColorTableSize);

        if (m_Palette.GetSafeHandle())
        {
            pOldPalette = dc.SelectPalette(&m_Palette, FALSE);
            dc.RealizePalette();
        }

        // Create it!
        m_hBitmap = CreateDIBSection(dc.m_hDC, 
                                     (const BITMAPINFO*) m_DIBinfo,
                                     m_iColorDataType,
                                     &m_ppvBits, 
                                     NULL, 0);
        if (pOldPalette)
            dc.SelectPalette(pOldPalette, FALSE);
        pOldPalette = NULL; 

        if (!m_hBitmap)
        {
            TRACE0("Unable to CreateDIBSection\n");
            AfxThrowResourceException();
        }
    
        // Need to copy the supplied bitmap onto the newly created DIBsection
        CDC memDC, CopyDC;
        if (!CopyDC.CreateCompatibleDC(&dc) || !memDC.CreateCompatibleDC(&dc)) 
        {
            TRACE0("Unable to create compatible DC's\n");
            AfxThrowResourceException();
        }

        if (m_Palette.GetSafeHandle())
        {
            memDC.SelectPalette(&m_Palette, FALSE);  memDC.RealizePalette();
            CopyDC.SelectPalette(&m_Palette, FALSE); CopyDC.RealizePalette();
        }

        GdiFlush();

        HBITMAP hOldMemBitmap  = (HBITMAP) SelectObject(memDC.m_hDC,  hBitmap);
        HBITMAP hOldCopyBitmap = (HBITMAP) SelectObject(CopyDC.m_hDC, m_hBitmap);

        CopyDC.BitBlt(0,0, bm.bmWidth, bm.bmHeight, &memDC, 0,0, SRCCOPY);

        SelectObject(memDC.m_hDC, hOldMemBitmap);
        SelectObject(CopyDC.m_hDC, hOldCopyBitmap);

        if (m_Palette.GetSafeHandle())
        {
            memDC.SelectStockObject(DEFAULT_PALETTE);
            CopyDC.SelectStockObject(DEFAULT_PALETTE);
        }
    }
    catch (CException *e)
    {
        e->Delete();
        _ShowLastError();
        if (pOldPalette)
            dc.SelectPalette(pOldPalette, FALSE);
        DeleteObject();
        return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Persistance...


// --- In?: lpszFileName - image filename
// --- Out :
// --- Returns : Returns TRUE on success, FALSE otherwise
// --- Effect : Loads the bitmap from a bitmap file with the name lpszFileName. 
//              If failure, then object is initialised back to an empty bitmap.
BOOL CDIBSectionLite::Load(LPCTSTR lpszFileName)
{
    CFile file;
    if (!file.Open(lpszFileName, CFile::modeRead))
        return FALSE;

    // Get the current file position.  
    DWORD dwFileStart = file.GetPosition();

    // The first part of the file contains the file header.
	// This will tell us if it is a bitmap, how big the header is, and how big 
    // the file is. The header size in the file header includes the color table.
	BITMAPFILEHEADER BmpFileHdr;
    int nBytes;
    nBytes = file.Read(&BmpFileHdr, sizeof(BmpFileHdr));
    if (nBytes != sizeof(BmpFileHdr)) 
    {
        TRACE0("Failed to read file header\n");
        return FALSE;
    }

    // Check that we have the magic 'BM' at the start.
    if (BmpFileHdr.bfType != DS_BITMAP_FILEMARKER)
    {
        TRACE0("Not a bitmap file\n");
        return FALSE;
    }

    // Read the header (assuming it's a DIB). 
	DIBINFO	BmpInfo;
    nBytes = file.Read(&BmpInfo, sizeof(BITMAPINFOHEADER)); 
    if (nBytes != sizeof(BITMAPINFOHEADER)) 
    {
        TRACE0("Failed to read BITMAPINFOHEADER\n");
        return FALSE;
    }

    // Check that we have a real Windows DIB file.
    if (BmpInfo.bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
    {
        TRACE0(" File is not a Windows DIB\n");
        return FALSE;
    }

    // See how big the color table is in the file (if there is one).  
    int nColors = NumColorEntries(BmpInfo.bmiHeader.biBitCount);
	if (nColors > 0) 
    {
        // Read the color table from the file.
        int nColorTableSize = nColors * sizeof(RGBQUAD);
		nBytes = file.Read(BmpInfo.ColorTable(), nColorTableSize);
		if (nBytes != nColorTableSize) 
        {
			TRACE0("Failed to read color table\n");
			return FALSE;
        }
	}

	// So how big the bitmap surface is.
    int nBitsSize = BmpFileHdr.bfSize - BmpFileHdr.bfOffBits;

    // Allocate the memory for the bits and read the bits from the file.
    BYTE* pBits = (BYTE*) malloc(nBitsSize);
    if (!pBits) 
    {
        TRACE0("Out of memory for DIB bits\n");
        return FALSE;
    }

    // Seek to the bits in the file.
    file.Seek(dwFileStart + BmpFileHdr.bfOffBits, CFile::begin);

    // read the bits
    nBytes = file.Read(pBits, nBitsSize);
    if (nBytes != nBitsSize) 
    {
        TRACE0("Failed to read bits\n");
        free(pBits);
		return FALSE;
    }

	// Everything went OK.
	BmpInfo.bmiHeader.biSizeImage = nBitsSize;

    if (!SetBitmap((LPBITMAPINFO) BmpInfo, pBits))
    {
        TRACE0("Failed to set bitmap info\n");
        free(pBits);
		return FALSE;
    }

    free(pBits);

    return TRUE;
}

// --- In?: lpszFileName - image filename
// --- Out :
// --- Returns : Returns TRUE on success, FALSE otherwise
// --- Effect : Saves the image to file.
BOOL CDIBSectionLite::Save(LPCTSTR lpszFileName)
{
    BITMAPFILEHEADER   hdr;
    LPBITMAPINFOHEADER lpbi = GetBitmapInfoHeader();

    if (!lpbi || !lpszFileName)
        return FALSE;

    CFile file;
    if (!file.Open(lpszFileName, CFile::modeWrite|CFile::modeCreate))
        return FALSE;

    DWORD dwBitmapInfoSize = sizeof(BITMAPINFO) + m_iColorTableSize*sizeof(RGBQUAD);
    DWORD dwFileHeaderSize = dwBitmapInfoSize + sizeof(hdr);

    // Fill in the fields of the file header 
    hdr.bfType       = DS_BITMAP_FILEMARKER;
    hdr.bfSize       = dwFileHeaderSize + lpbi->biSizeImage;
    hdr.bfReserved1  = 0;
    hdr.bfReserved2  = 0;
    hdr.bfOffBits    = dwFileHeaderSize;

    // Write the file header 
    file.Write(&hdr, sizeof(hdr));

    // Write the DIB header
    file.Write(lpbi, dwBitmapInfoSize);
    
    // Write DIB bits
    file.Write(GetDIBits(), lpbi->biSizeImage);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDIBSectionLite palette stuff


// --- In?:
// --- Out :
// --- Returns : TRUE on success
// --- Effect : Creates the palette from the DIBSection's color table. Assumes 
//              m_iColorTableSize has been set and the DIBsection m_hBitmap created
BOOL CDIBSectionLite::CreatePalette()
{
	int i;
    m_Palette.DeleteObject();

    if (!m_hBitmap)
        return FALSE;

    // Create a 256 color halftone palette if there is no color table in the DIBSection
    if (m_iColorTableSize == 0)
        return CreateHalftonePalette(m_Palette, 256);


    HDC hDC = ::GetDC(NULL);
    if (!hDC)
        return FALSE;

    // Get space for the colour entries
    RGBQUAD *pRGB = new RGBQUAD[m_iColorTableSize];
    if (!pRGB)
    {
        ReleaseDC(NULL, hDC);
        return CreateHalftonePalette(m_Palette, m_iColorTableSize);
    }

    // Create a memory DC compatible with the current DC
    CDC MemDC;
    MemDC.CreateCompatibleDC(CDC::FromHandle(hDC));
    if (!MemDC.GetSafeHdc())
    {
        delete [] pRGB;
        ::ReleaseDC(NULL, hDC);
        return CreateHalftonePalette(m_Palette, m_iColorTableSize);
    }
    ::ReleaseDC(NULL, hDC);
    
    HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(MemDC.GetSafeHdc(), m_hBitmap);
    if (!hOldBitmap)
    {
        delete [] pRGB;
        return CreateHalftonePalette(m_Palette, m_iColorTableSize);
    }

    // Get the colours used. 
    int nColours = ::GetDIBColorTable(MemDC.GetSafeHdc(), 0, m_iColorTableSize, pRGB);

    // Clean up
    ::SelectObject(MemDC.GetSafeHdc(), hOldBitmap);

    if (!nColours)   // No colours retrieved => the bitmap in the DC is not a DIB section
    {
        delete [] pRGB;
        return CreateHalftonePalette(m_Palette, m_iColorTableSize);
    }   
    
    // Create and fill a LOGPALETTE structure with the colours used.
    PALETTEINFO PaletteInfo;
    PaletteInfo.palNumEntries = (WORD) m_iColorTableSize;
                        
    for ( i = 0; i < nColours; i++)
    {
        PaletteInfo.palPalEntry[i].peRed   = pRGB[i].rgbRed;
        PaletteInfo.palPalEntry[i].peGreen = pRGB[i].rgbGreen;
        PaletteInfo.palPalEntry[i].peBlue  = pRGB[i].rgbBlue;
        PaletteInfo.palPalEntry[i].peFlags = 0;
    }
    for (; i < (int) m_iColorTableSize; i++)
    {
        PaletteInfo.palPalEntry[i].peRed   = 0;
        PaletteInfo.palPalEntry[i].peGreen = 0;
        PaletteInfo.palPalEntry[i].peBlue  = 0;
        PaletteInfo.palPalEntry[i].peFlags = 0;
    }

    delete [] pRGB;

    // Create Palette!
    return m_Palette.CreatePalette(&PaletteInfo);
}

BOOL CDIBSectionLite::CreatePalette2()
{
	int i;
	m_Palette.DeleteObject();

	if(!m_hBitmap)
		return FALSE;

    // Create a 256 color halftone palette if there is no color table in the DIBSection
	if (m_iColorTableSize == 0)
		return CreateHalftonePalette(m_Palette, 256);

	HDC hDC = ::GetDC(NULL);
	if(!hDC)
		return FALSE;

    // Create a memory DC compatible with the current DC
	CDC MemDC;
	MemDC.CreateCompatibleDC(CDC::FromHandle(hDC));
    if(!MemDC.GetSafeHdc())
	{
		::ReleaseDC(NULL, hDC);
		return CreateHalftonePalette(m_Palette, m_iColorTableSize);
	}
	::ReleaseDC(NULL, hDC);
    
	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(MemDC.GetSafeHdc(), m_hBitmap);
	if(!hOldBitmap)
		return CreateHalftonePalette(m_Palette, m_iColorTableSize);

    // Clean up
	::SelectObject(MemDC.GetSafeHdc(), hOldBitmap);

	int nColours = m_pGDALColorTable.GetSize();
	if(nColours <= 0)
		return CreateHalftonePalette(m_Palette, m_iColorTableSize);

    // Create and fill a LOGPALETTE structure with the colours used.
	int nFree;

	nFree = GetDeviceCaps(hDC, SIZEPALETTE) - GetDeviceCaps(hDC, NUMRESERVED);
	m_pPalette = (LOGPALETTE *)GlobalAlloc(GPTR, sizeof(LOGPALETTE)+nFree*sizeof(PALETTEENTRY));
	m_pPalette->palVersion = 0x300;
	m_pPalette->palNumEntries = (WORD)m_iColorTableSize;
	for( i=0; i<nColours; i++)
	{
		m_pPalette->palPalEntry[i].peRed   = (unsigned char)(m_pGDALColorTable[i]->m_c1);
		m_pPalette->palPalEntry[i].peGreen = (unsigned char)(m_pGDALColorTable[i]->m_c2);
		m_pPalette->palPalEntry[i].peBlue  = (unsigned char)(m_pGDALColorTable[i]->m_c3);
		m_pPalette->palPalEntry[i].peFlags = 0;
	}
	for (; i < (int) m_iColorTableSize; i++)
	{
		m_pPalette->palPalEntry[i].peRed = 0;
		m_pPalette->palPalEntry[i].peGreen = 0;
		m_pPalette->palPalEntry[i].peBlue  = 0;
		m_pPalette->palPalEntry[i].peFlags = 0;
	}

	if(!m_Palette.CreatePalette(m_pPalette))
		return FALSE;

	if(!m_Palette.GetSafeHandle())
	{
		TRACE("error: m_Palette handle not found!\n");
		return FALSE;
	}

    RGBQUAD RGBquads[256];
    for( i=0; i<(int)m_iColorTableSize; i++)
	{
		RGBquads[i].rgbRed   = m_pPalette->palPalEntry[i].peRed;
		RGBquads[i].rgbGreen = m_pPalette->palPalEntry[i].peGreen;
		RGBquads[i].rgbBlue  = m_pPalette->palPalEntry[i].peBlue;
		RGBquads[i].rgbReserved = 0;
	}
    
	return FillDIBColorTable(m_iColorTableSize, RGBquads);

/*
    // Create and fill a LOGPALETTE structure with the colours used.
	PALETTEINFO PaletteInfo;
	PaletteInfo.palNumEntries = (WORD) m_iColorTableSize;

	for(int i=0; i<nColours; i++)
	{
		PaletteInfo.palPalEntry[i].peRed   = (unsigned char)(m_pGDALColorTable[i]->m_c1);
		PaletteInfo.palPalEntry[i].peGreen = (unsigned char)(m_pGDALColorTable[i]->m_c2);
		PaletteInfo.palPalEntry[i].peBlue  = (unsigned char)(m_pGDALColorTable[i]->m_c3);
		PaletteInfo.palPalEntry[i].peFlags = 0;
	}

	for (; i < (int) m_iColorTableSize; i++)
	{
		PaletteInfo.palPalEntry[i].peRed   = 0;
		PaletteInfo.palPalEntry[i].peGreen = 0;
		PaletteInfo.palPalEntry[i].peBlue  = 0;
		PaletteInfo.palPalEntry[i].peFlags = 0;
	}

	// Create Palette!
//	return m_Palette.CreatePalette(&PaletteInfo);
	if(!m_Palette.CreatePalette(&PaletteInfo))
		return FALSE;

	if (m_Palette.GetSafeHandle())
	{
//            pOldPalette = dc.SelectPalette(&m_Palette, FALSE);
  //          dc.RealizePalette();
	}

	return SetPalette(&m_Palette);
*/

}

// --- In?: pPalette - new palette to use
// --- Out :
// --- Returns : TRUE on success
// --- Effect : Sets the current palette used by the image from the supplied CPalette,
//              and sets the color table in the DIBSection
BOOL CDIBSectionLite::SetPalette(CPalette* pPalette)
{
    m_Palette.DeleteObject();

    if (!pPalette)
        return FALSE;

    UINT nColours = pPalette->GetEntryCount();
    if (nColours <= 0 || nColours > 256)
        return FALSE;

    // Get palette entries
    PALETTEINFO pi;
    pi.palNumEntries = (WORD) pPalette->GetPaletteEntries(0, nColours, (LPPALETTEENTRY) pi);
                          
    // TODO: If pi.palNumEntries < m_iColorTableSize, then fill in blanks with 0's

    return SetLogPalette(&pi);
}

// --- In?: pLogPalette - new palette to use
// --- Out :
// --- Returns : TRUE on success
// --- Effect : Sets the current palette used by the image from the supplied LOGPALETTE
BOOL CDIBSectionLite::SetLogPalette(LOGPALETTE* pLogPalette)
{
    ASSERT(pLogPalette->palVersion == (WORD) 0x300);
    UINT nColours = pLogPalette->palNumEntries;
    if (nColours <= 0 || nColours > 256)
    {
        CreatePalette();
        return FALSE;
    }

    // Create new palette
    m_Palette.DeleteObject();
    if (!m_Palette.CreatePalette(pLogPalette))
    {
        CreatePalette();
        return FALSE;
    }

    if (m_iColorTableSize == 0)
        return TRUE;

    // Set the DIB colours
    RGBQUAD RGBquads[256]; 
    for (UINT i = 0; i < nColours; i++)
    {
        RGBquads[i].rgbRed   = pLogPalette->palPalEntry[i].peRed;
        RGBquads[i].rgbGreen = pLogPalette->palPalEntry[i].peGreen;
        RGBquads[i].rgbBlue  = pLogPalette->palPalEntry[i].peBlue;
        RGBquads[i].rgbReserved = 0;
    }
    
    return FillDIBColorTable(nColours, RGBquads);
}

// --- In?: nNumColours - number of colours to set
//           pRGB - colours to fill
// --- Out :
// --- Returns : Returns TRUE on success
// --- Effect : Sets the colours used by the image. Only works if # colours <= 256
BOOL CDIBSectionLite::FillDIBColorTable(UINT nNumColours, RGBQUAD *pRGB)
{
    if (!m_hBitmap || !pRGB || !nNumColours || !m_iColorTableSize
        || nNumColours > 256)
        return FALSE;

    // Create a memory DC compatible with the screen
    HDC hDC = ::GetDC(NULL);
    if (!hDC) return FALSE;

    CDC MemDC;
    MemDC.CreateCompatibleDC(CDC::FromHandle(hDC));
    ::ReleaseDC(NULL, hDC);
    if (!MemDC.GetSafeHdc())
        return FALSE;

    HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(MemDC.GetSafeHdc(), m_hBitmap);

    // Set the bitmap colours.
    int nColours = ::SetDIBColorTable(MemDC.GetSafeHdc(), 0, nNumColours, pRGB);

    // Clean up
    if (hOldBitmap)
        ::SelectObject(MemDC.GetSafeHdc(), hOldBitmap);
    MemDC.DeleteDC();

    return (nColours > 0);
}

#ifdef _DEBUG
// Makes trace windows a little bit more informative...
void CDIBSectionLite::_ShowLastError()
{
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,    
                  NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpMsgBuf, 0, NULL);
    TRACE1("Last error: %s\n", lpMsgBuf);
    LocalFree(lpMsgBuf);
}
#else
void CDIBSectionLite::_ShowLastError() {}
#endif

BYTE *CDIBSectionLite::ReadCIB(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect)
{
	double adfGeoTransform[6];
	int nXSize, nYSize;
	byte *temp, *data;
	GDALDataset *poDataset;
	GDALRasterBand *poBand;
	double left, top, right, bottom;
	int Pic_X, Pic_Y;
	data = 0;

	try
	{
		GDALAllRegister();

		poDataset = (GDALDataset*)GDALOpen(filename, GA_ReadOnly);

		if(poDataset == NULL)
		{
			*isok = false;
			return data;
		}

	// 그림의 가로 size
		Pic_X = poDataset->GetRasterXSize();
	// 그림의 세로 size
		Pic_Y = poDataset->GetRasterYSize();
	// band get(R band:1, G band:2, B band:3)
		poBand = poDataset->GetRasterBand(1);
	// band x,y size get
		nXSize = poBand->GetXSize();
		nYSize = poBand->GetYSize();
	// memory alloc
		temp = new byte[nXSize * nYSize];
		data = new byte[nXSize * nYSize*3];
	// reading raster data
		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, 
						 temp, nXSize, nYSize, GDT_Byte, 
						 0, 0);

		rect->SetRect(0,0,0,0);
	//	left, top, pixel & raster size 이용 right, bottom 구하는 계산식.
	//	right = left + (pixel x size * raster x size)
	//	bottom = top + (pixel y size * raster y size)
		if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
		{
			left = adfGeoTransform[0];
			top = adfGeoTransform[3];
			right = left + (adfGeoTransform[1] * (double)Pic_X);
			bottom = top + (adfGeoTransform[5] * (double)Pic_Y);
			rect->SetRect(left, top, right, bottom);
		}

	// image x,y rate get
		m_dFixelX = adfGeoTransform[1] * 100000.0;
		m_dFixelY = adfGeoTransform[5] * 100000.0;

		int j = nXSize*3;
		int k = 1;

	// raster data copy
		for(int i=nXSize*nYSize-1; i>=0; i--)
		{
			data[j-3] = (int)(temp[i]);
			data[j-2] = (int)(temp[i]);
			data[j-1] = (int)(temp[i]);
			j = j-3;
			if(j == (k-1)*nXSize*3)
			{
				k++;
				j = nXSize*3*k;
			}
		}

	// BITMAPINFOHEADER struct composition
		pBmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		pBmi->bmiHeader.biWidth         = nXSize;
		pBmi->bmiHeader.biHeight        = nYSize;
		pBmi->bmiHeader.biCompression   = BI_RGB;
		pBmi->bmiHeader.biBitCount      = 24;
		pBmi->bmiHeader.biPlanes        = 1;
		pBmi->bmiHeader.biClrUsed       = 0;
		pBmi->bmiHeader.biClrImportant  = 0;
		pBmi->bmiHeader.biSizeImage     = 0;
		pBmi->bmiHeader.biXPelsPerMeter = 0;
		pBmi->bmiHeader.biYPelsPerMeter = 0;
	}
	catch(...)
	{
		TRACE("error: CIB format data open&read failed!\n");
		*isok = false;
		return data;
	}

	GDALClose(poDataset);
	delete temp;
	*isok = true;
	return data;
}

BYTE *CDIBSectionLite::ReadCADRG(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect)
{
	double adfGeoTransform[6];
	int nXSize, nYSize;
	byte *temp, *data;
	GDALDataset *poDataset;
	GDALDriver *poDriver;
	GDALRasterBand *poBand;
	GDALColorTable *poColor;
	stGDALColorTable *pColorTable;
	double left, top, right, bottom;
	int Pic_X, Pic_Y;
	temp = 0;
	data = 0;

	int bGotMin, bGotMax;
	double adfMinMax[2];

	for(int i=0; i<m_pGDALColorTable.GetSize(); i++)
		delete m_pGDALColorTable.GetAt(i);
	m_pGDALColorTable.RemoveAll();

	try
	{
		GDALAllRegister();

		poDataset = (GDALDataset*)GDALOpen(filename, GA_ReadOnly);
		if(poDataset == NULL)
		{
			*isok = false;
			return data;
		}

	// 그림의 가로 size
		Pic_X = poDataset->GetRasterXSize();
	// 그림의 세로 size
		Pic_Y = poDataset->GetRasterYSize();
	// band get(Red band:1, Green band:2, Blue band:3)
		poBand = poDataset->GetRasterBand(1);
	// band x,y size get
		nXSize = poBand->GetXSize();
		nYSize = poBand->GetYSize();

		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);
		if(!(bGotMin && bGotMax))
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

		poColor = poBand->GetColorTable();
		if(poColor == NULL)
		{
			*isok = false;
			return data;
		}

// Grayscale (in GDALColorEntry.c1), GPI_Gray=0
// Red, Green, Blue and Alpha in (in c1, c2, c3 and c4), GPI_RGB=1
// Cyan, Magenta, Yellow and Black (in c1, c2, c3 and c4), GPI_CMYK=2,
// Hue, Lightness and Saturation (in c1, c2, and c3), GPI_HLS=3
//		for(int i=0; i<poColor->GetColorEntryCount()-1; i++)
//		{
//			GDALPaletteInterp oPalleteInterp = poColor->GetPaletteInterpretation();
//			const GDALColorEntry *poColorEntry = poColor->GetColorEntry(i);
//			TRACE("info2: [%d] %d %d %d %d\n", i, poColorEntry->c1, poColorEntry->c2,
//					poColorEntry->c3, poColorEntry->c4);
//		}

	// color table set
		for(int i=0; i<poColor->GetColorEntryCount()-1; i++)
		{
			GDALPaletteInterp oPalleteInterp = poColor->GetPaletteInterpretation();
			const GDALColorEntry *poColorEntry = poColor->GetColorEntry(i);
			pColorTable = new stGDALColorTable;
			pColorTable->m_c1 = poColorEntry->c1;
			pColorTable->m_c2 = poColorEntry->c2;
			pColorTable->m_c3 = poColorEntry->c3;
			pColorTable->m_c4 = poColorEntry->c4;
			m_pGDALColorTable.Add(pColorTable);
		}

	// memory alloc
		data = new byte[nXSize * nYSize*3];
		temp = new byte[nXSize * nYSize];
	// reading raster data
		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, temp, 
			nXSize, nYSize, GDT_Byte, 0, 0);

		rect->SetRect(0,0,0,0);
	//	left, top, pixel & raster size 이용 right, bottom 구하는 계산식.
	//	right = left + (pixel x size * raster x size)
	//	bottom = top + (pixel y size * raster y size)
		if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
		{
			left = adfGeoTransform[0];
			top = adfGeoTransform[3];
			//right = left + (adfGeoTransform[1] * (double)Pic_X);
			//bottom = top + (adfGeoTransform[5] * (double)Pic_Y);

			right = left + (adfGeoTransform[1] * (double)Pic_X);
			bottom = top + (adfGeoTransform[5] * (double)Pic_Y);

			rect->SetRect(left, top, right, bottom);

			////////////////////////////////////////////////////////////////////////////////////////
			//double dscale = poBand->GetScale();
			mdoffset = poBand->GetOffset();
			//TRACE(" info=> %s %.9f %.9f / %.9f,%.9f,%.9f,%.9f  %.9f %.9f %.9f %.9f %.9f %.9f\n", 
			//	filename, dscale, doffset, left, right, top, bottom,
			//	adfGeoTransform[0],
			//	adfGeoTransform[1],
			//	adfGeoTransform[2],
			//	adfGeoTransform[3],
			//	adfGeoTransform[4],
			//	adfGeoTransform[5]);
			///////////////////////////////////////////////////////////////////////////////////////
		}

	// image x,y rate get
		m_dFixelX = adfGeoTransform[1] * 100000.0;
		m_dFixelY = adfGeoTransform[5] * 100000.0;

		int j = nXSize*3;
		int k = 1;
		int iIndex;
		for(int i=nXSize*nYSize-1; i>=0; i--)
		{
			iIndex = (int)(temp[i]);
			if(iIndex < 0 || iIndex > m_pGDALColorTable.GetSize()-1)
			{
				data[j-3] = (int)(0);	// Blue color
				data[j-2] = (int)(0);	// Green color
				data[j-1] = (int)(0);	// Red color
			}
			else
			{
				pColorTable = m_pGDALColorTable.GetAt(iIndex);
				data[j-3] = (int)(pColorTable->m_c3);	// Blue color
				data[j-2] = (int)(pColorTable->m_c2);	// Green color
				data[j-1] = (int)(pColorTable->m_c1);	// Red color
			}
			j = j-3;
			if(j == (k-1)*nXSize*3)
			{
				k++;
				j = nXSize*3*k;
			}
		}

	// BITMAPINFOHEADER struct composition
		pBmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		pBmi->bmiHeader.biWidth         = nXSize;
		pBmi->bmiHeader.biHeight        = nYSize;
		pBmi->bmiHeader.biCompression   = BI_RGB;
		pBmi->bmiHeader.biBitCount      = 24;
		pBmi->bmiHeader.biPlanes        = 1;
		pBmi->bmiHeader.biClrUsed       = 0;
		pBmi->bmiHeader.biClrImportant  = 0;
		pBmi->bmiHeader.biSizeImage     = 0;
		pBmi->bmiHeader.biXPelsPerMeter = 0;
		pBmi->bmiHeader.biYPelsPerMeter = 0;
	}
	catch(...)
	{
		TRACE("error: CADRG format data open&read failed!\n");
		delete temp;
		*isok = false;
		return data;
	}

	GDALClose(poDataset);
	delete temp;
	*isok = true;
	return data;
}

/** DTED1(*.dt1, Controlled Image Base) format raster data Open & Read */
void CDIBSectionLite::ReadDTED(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect)
{
	double adfGeoTransform[6];
	int nXSize, nYSize;
	short *temp, *data;
	GDALDataset *poDataset;
	GDALRasterBand *poBand;
	double left, top, right, bottom;
	int Pic_X, Pic_Y;
	data = 0;
	temp = 0;

	int bGotMin, bGotMax;
	double adfMinMax[2];

	try
	{
		GDALAllRegister();

		poDataset = (GDALDataset*)GDALOpen(filename, GA_ReadOnly);
		if(poDataset == NULL)
		{
			*isok = false;
			return;
		}

	// 그림의 가로 size
		Pic_X = poDataset->GetRasterXSize();
	// 그림의 세로 size
		Pic_Y = poDataset->GetRasterYSize();
	// band get(Red band:1, Green band:2, Blue band:3)
		poBand = poDataset->GetRasterBand(1);
	// band x,y size get
		nXSize = poBand->GetXSize();
		nYSize = poBand->GetYSize();

		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);
		if(!(bGotMin && bGotMax))
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

//		poColor = poBand->GetColorTable();
//		if(poColor == NULL)
//		{
//			*isok = false;
//			return;
//		}

// Grayscale (in GDALColorEntry.c1), GPI_Gray=0
// Red, Green, Blue and Alpha in (in c1, c2, c3 and c4), GPI_RGB=1
// Cyan, Magenta, Yellow and Black (in c1, c2, c3 and c4), GPI_CMYK=2,
// Hue, Lightness and Saturation (in c1, c2, and c3), GPI_HLS=3
//		for(int i=0; i<poColor->GetColorEntryCount()-1; i++)
//		{
//			GDALPaletteInterp oPalleteInterp = poColor->GetPaletteInterpretation();
//			const GDALColorEntry *poColorEntry = poColor->GetColorEntry(i);
//			TRACE("info2: [%d] %d %d %d %d\n", i, poColorEntry->c1, poColorEntry->c2,
//					poColorEntry->c3, poColorEntry->c4);
//		}

	// color table set
//		for(int i=0; i<poColor->GetColorEntryCount()-1; i++)
//		{
///			GDALPaletteInterp oPalleteInterp = poColor->GetPaletteInterpretation();
//			const GDALColorEntry *poColorEntry = poColor->GetColorEntry(i);
//			pColorTable = new stGDALColorTable;
//			pColorTable->m_c1 = poColorEntry->c1;
//			pColorTable->m_c2 = poColorEntry->c2;
//			pColorTable->m_c3 = poColorEntry->c3;
//			pColorTable->m_c4 = poColorEntry->c4;
//			m_pGDALColorTable.Add(pColorTable);
//		}

	// memory alloc
		data = new short[nXSize * nYSize*3];
		temp = new short[nXSize * nYSize];

	// reading raster data
//		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, temp, 
//			nXSize, nYSize, GDT_Int16, 0, 0);
		rect->SetRect(0,0,0,0);
	//	left, top, pixel & raster size 이용 right, bottom 구하는 계산식.
	//	right = left + (pixel x size * raster x size)
	//	bottom = top + (pixel y size * raster y size)
		if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
		{
			left = adfGeoTransform[0];
			top = adfGeoTransform[3];
			right = left + (adfGeoTransform[1] * (double)Pic_X);
			bottom = top + (adfGeoTransform[5] * (double)Pic_Y);
			rect->SetRect(left, top, right, bottom);
//			TRACE(" info=> %.9f %.9f %.9f %.9f %.9f %.9f\n", 
//				adfGeoTransform[0],
//				adfGeoTransform[1],
//				adfGeoTransform[2],
//				adfGeoTransform[3],
//				adfGeoTransform[4],
//				adfGeoTransform[5]);
		}

	// image x,y rate get
		m_dFixelX = adfGeoTransform[1] * 100000.0;
		m_dFixelY = adfGeoTransform[5] * 100000.0;

		CString strTmp;
		CTime tTime = CTime::GetCurrentTime();
		int ix, iy;
		poBand->GetBlockSize(&ix, &iy);

		for(int i=0; i<ix; i++)
		{
			for(int j=0; j<iy; j++)
			{
				poBand->ReadBlock(i, j, temp);
			}
		}
		CTime tTime2 = CTime::GetCurrentTime();

		strTmp.Format("%d:%d:%d ~ %d:%d:%d", tTime.GetHour(), tTime.GetMinute(), tTime.GetSecond(),
						tTime2.GetHour(), tTime2.GetMinute(), tTime2.GetSecond());

	AfxMessageBox(strTmp);

//		int j = nXSize*3;
//		int k = 1;
//		int iIndex;
//		for(int i=nXSize*nYSize-1; i>=0; i--)
//		{
//			TRACE(" info2=> %d\n", temp[i]);
//		}
		for(int i=0; i<nXSize*nYSize-1; i++)
		{
			TRACE(" info2=> %d\n", temp[i]);
		}
	}
	catch(...)
	{
		TRACE("error: DTED1 format data open&read failed!\n");
		delete temp;
		delete data;
		*isok = false;
		return;
	}

	GDALClose(poDataset);
	delete temp;
	delete data;
	*isok = true;
	return;
}


//  (red * 256 + green + blue/256) – 32678
//  elevation (m) = -10000 + ((R * 256 * 256 + G * 256 + B) * 0.1)







/* old code, 지우지 마세요!
byte *CDIBSectionLite::ReadCADRG(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect)
{
	double adfGeoTransform[6];
	int nXSize, nYSize;
	byte *temp, *data;
	GDALDataset *poDataset;
	GDALRasterBand *poBand;
	GDALColorTable *poColor;
	stGDALColorTable *pColorTable;
	int nBlockXSize, nBlockYSize;
	double left, top, right, bottom;
	int Pic_X, Pic_Y, colors;
	data = 0;

	int bGotMin, bGotMax;
	double adfMinMax[2];

	try
	{
		GDALAllRegister();

		poDataset = (GDALDataset*)GDALOpen(filename, GA_ReadOnly);
		if(poDataset == NULL)
		{
			*isok = false;
			return data;
		}

	// 그림의 가로 size
		Pic_X = poDataset->GetRasterXSize();
	// 그림의 세로 size
		Pic_Y = poDataset->GetRasterYSize();
	// band get(Red band:1, Green band:2, Blue band:3)
		poBand = poDataset->GetRasterBand(1);
	// band x,y size get
		nXSize = poBand->GetXSize();
		nYSize = poBand->GetYSize();

		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);
		if(!(bGotMin && bGotMax))
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

		poColor = poBand->GetColorTable();
		if(poColor == NULL)
		{
			*isok = false;
			return data;
		}

// Grayscale (in GDALColorEntry.c1), GPI_Gray=0
// Red, Green, Blue and Alpha in (in c1, c2, c3 and c4), GPI_RGB=1
// Cyan, Magenta, Yellow and Black (in c1, c2, c3 and c4), GPI_CMYK=2,
// Hue, Lightness and Saturation (in c1, c2, and c3), GPI_HLS=3
//		for(int i=0; i<poColor->GetColorEntryCount()-1; i++)
//		{
//			GDALPaletteInterp oPalleteInterp = poColor->GetPaletteInterpretation();
//			const GDALColorEntry *poColorEntry = poColor->GetColorEntry(i);
//			TRACE("info2: [%d] %d %d %d %d\n", i, poColorEntry->c1, poColorEntry->c2,
//					poColorEntry->c3, poColorEntry->c4);
//		}

	// color table set
		for(int i=0; i<poColor->GetColorEntryCount()-1; i++)
		{
			GDALPaletteInterp oPalleteInterp = poColor->GetPaletteInterpretation();
			const GDALColorEntry *poColorEntry = poColor->GetColorEntry(i);
			pColorTable = new stGDALColorTable;
			pColorTable->m_c1 = poColorEntry->c1;
			pColorTable->m_c2 = poColorEntry->c2;
			pColorTable->m_c3 = poColorEntry->c3;
			pColorTable->m_c4 = poColorEntry->c4;
			m_pGDALColorTable.Add(pColorTable);
		}

	// memory alloc
//		data = new byte[nXSize * nYSize*3];
		data = new byte[nXSize * nYSize];
	// reading raster data
		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, data, 
			nXSize, nYSize, GDT_Byte, 0, 0);

		rect->SetRect(0,0,0,0);
	//	left, top, pixel & raster size 이용 right, bottom 구하는 계산식.
	//	right = left + (pixel x size * raster x size)
	//	bottom = top + (pixel y size * raster y size)
		if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
		{
			left = adfGeoTransform[0];
			top = adfGeoTransform[3];
			right = left + (adfGeoTransform[1] * (double)Pic_X);
			bottom = top + (adfGeoTransform[5] * (double)Pic_Y);
			rect->SetRect(left, top, right, bottom);
		}

	// image x,y rate get
		m_dFixelX = adfGeoTransform[1] * 100000.0;
		m_dFixelY = adfGeoTransform[5] * 100000.0;

	// BITMAPINFOHEADER struct composition
		pBmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		pBmi->bmiHeader.biWidth         = nXSize;
		pBmi->bmiHeader.biHeight        = nYSize;
		pBmi->bmiHeader.biCompression   = BI_RGB;
		pBmi->bmiHeader.biBitCount      = 8;
		pBmi->bmiHeader.biPlanes        = 1;
		pBmi->bmiHeader.biClrUsed       = 0;
		pBmi->bmiHeader.biClrImportant  = 0;
		pBmi->bmiHeader.biSizeImage     = 0;
		pBmi->bmiHeader.biXPelsPerMeter = 0;
		pBmi->bmiHeader.biYPelsPerMeter = 0;
	}
	catch(...)
	{
		TRACE("error: CADRG format data open&read failed!\n");
		*isok = false;
		return data;
	}

	GDALClose(poDataset);
//	delete temp;
	*isok = true;
	return data;
}

byte *CDIBSectionLite::ReadNITF(CString strType, CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect)
{
	double adfGeoTransform[6];
	int nXSize, nYSize;
	byte *temp, *data;
	data = 0;
	GDALDataset *poDataset;

	GDALAllRegister();

	poDataset = (GDALDataset*)GDALOpen(filename, GA_ReadOnly);

	if(poDataset == NULL)
	{
		*isok = false;
//		nXSize = nYSize = 1536;
//		data = new byte[nXSize * nYSize*3];
//		int j=nXSize * 3;
//		int k=1;
//		for(int i=nXSize*nYSize-1; i>=0; i--)
//		{
//			data[j-3] = (int)(0);
//			data[j-2] = (int)(255);
//			data[j-1] = (int)(0);
//			j = j-3;
//			if(j == (k-1)*nXSize*3)
//			{
//				k++;
//				j = nXSize*3*k;
//			}
//		}
		return data;
	}

	int Pic_X, Pic_Y, colors;
	Pic_X = poDataset->GetRasterXSize(); // 그림의 가로 size
	Pic_Y = poDataset->GetRasterYSize(); // 그림의 세로 size
	colors = poDataset->GetRasterCount();// color 사이즈
	GDALRasterBand *poBand;
	int nBlockXSize, nBlockYSize;
	poBand = poDataset->GetRasterBand(1); //band는 rgb 경우 1,2,3
	poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
	nXSize = poBand->GetXSize();
	nYSize = poBand->GetYSize();
	temp = new byte[nXSize * nYSize];
	data = new byte[nXSize * nYSize*3];
	poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, 
					 temp, nXSize, nYSize, GDT_Byte, 
					 0, 0);

	rect->SetRect(0,0,0,0);
	double left, top, right, bottom;
//	left, top, pixel & raster size 이용 right, bottom 구하는 계산식.
//	right = left + (pixel x size * raster x size)
//	bottom = top + (pixel y size * raster y size)
	if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		left = adfGeoTransform[0];
		top = adfGeoTransform[3];
		right = left + (adfGeoTransform[1] * (double)Pic_X);
		bottom = top + (adfGeoTransform[5] * (double)Pic_Y);
		rect->SetRect(left, top, right, bottom);
    }

	m_dFixelX = adfGeoTransform[1] * 100000.0;
	m_dFixelY = adfGeoTransform[5] * 100000.0;

	int j=nXSize * 3;
	int k=1;

	for(int i=nXSize*nYSize-1; i>=0; i--)
	{
//		if (temp[i] < 127)
//			temp[i] *= 1.0;
//		else
//		{
//			temp[i] *= 1.2;
//			if (temp[i] >= 255)
//				temp[i] = 200;
//		}
		data[j-3] = (int)(temp[i]);
		data[j-2] = (int)(temp[i]);
		data[j-1] = (int)(temp[i]);
		j = j-3;
		if(j == (k-1)*nXSize*3)
		{
			k++;
			j = nXSize*3*k;
		}
	}

	strType.MakeUpper();
	if(strType == "I42")
	{
		pBmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		pBmi->bmiHeader.biWidth         = nXSize;
		pBmi->bmiHeader.biHeight        = nYSize;
		pBmi->bmiHeader.biCompression   = BI_RGB;
		pBmi->bmiHeader.biBitCount      = 24;
		pBmi->bmiHeader.biPlanes        = 1;
		pBmi->bmiHeader.biClrUsed       = 0;
		pBmi->bmiHeader.biClrImportant  = 0;
//		pBmi->bmiHeader.biSizeImage     = BytesPerLine(nXSize, 24) * nYSize;
		pBmi->bmiHeader.biSizeImage     = 0;
		pBmi->bmiHeader.biXPelsPerMeter = 0;
		pBmi->bmiHeader.biYPelsPerMeter = 0;
	}
	else if(strType == "TL2")
	{
		pBmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		pBmi->bmiHeader.biWidth         = nXSize;
		pBmi->bmiHeader.biHeight        = nYSize;
		pBmi->bmiHeader.biCompression   = BI_RGB;
		pBmi->bmiHeader.biBitCount      = 24;
		pBmi->bmiHeader.biPlanes        = 1;
		pBmi->bmiHeader.biClrUsed       = 0;
		pBmi->bmiHeader.biClrImportant  = 0;
//		pBmi->bmiHeader.biSizeImage     = BytesPerLine(nXSize, 24) * nYSize;
		pBmi->bmiHeader.biSizeImage     = 0;
		pBmi->bmiHeader.biXPelsPerMeter = 0;
		pBmi->bmiHeader.biYPelsPerMeter = 0;
	}

	GDALClose(poDataset);
	delete temp;
	*isok = true;
	return data;
}
*/

// mbc 8.17
void CDIBSectionLite::dibSetBrightness(HBITMAP hDIB, int nLevel)
{
	DIBSECTION ds;
	
	GetObject(hDIB, sizeof(DIBSECTION), &ds);
	if(ds.dsBmih.biBitCount != 24) return;

	for(int y=0; y<ds.dsBm.bmHeight; y++)
	{
		RGBTRIPLE* lpLine = (RGBTRIPLE*)ds.dsBm.bmBits + (y * ds.dsBm.bmWidth);
		for(int x=0; x<ds.dsBm.bmWidth; x++)
		{
			RGBTRIPLE rgbT = lpLine[x];
			int r = (int)rgbT.rgbtRed + nLevel;
			int g = (int)rgbT.rgbtGreen + nLevel;
			int b = (int)rgbT.rgbtBlue + nLevel;
			rgbT.rgbtRed = (r > 255) ? 255 : ((r<0)? 0 : r);
			rgbT.rgbtGreen = (g > 255) ? 255 : ((g<0)? 0 : g);
			rgbT.rgbtBlue = (b > 255) ? 255 :((b<0)? 0 : b);
			lpLine[x] = rgbT;
		}
	}

}
// mbc 8.17
void CDIBSectionLite::dibSetTrueColor(HBITMAP *phDIB)
{
	DIBSECTION ds;
	GetObject(*phDIB, sizeof(DIBSECTION), &ds);

	if(ds.dsBmih.biBitCount == 24) return;

	HDC hDC = GetDC(GetDesktopWindow());

	HBITMAP hDDB = CreateCompatibleBitmap(hDC, ds.dsBm.bmWidth, ds.dsBm.bmHeight);
	SetDIBits(hDC, hDDB, 0, ds.dsBm.bmHeight, ds.dsBm.bmBits, (LPBITMAPINFO)&ds.dsBmih, 
				DIB_RGB_COLORS);

	BITMAPINFOHEADER bmih;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = ds.dsBm.bmWidth;
	bmih.biHeight = ds.dsBm.bmHeight;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;

	LPVOID lpDIBits;
	HBITMAP hDIB = CreateDIBSection(hDC, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS, &lpDIBits, NULL, 0);
	::GetDIBits(hDC, hDDB, 0, ds.dsBm.bmHeight, lpDIBits, (LPBITMAPINFO)&bmih, 
				DIB_RGB_COLORS);
	ReleaseDC(GetDesktopWindow(), hDC);

	::DeleteObject(hDDB);
	::DeleteObject(*phDIB);

	*phDIB = hDIB;

}

// jykim raster
void CDIBSectionLite::SetStartEnd(double lat1, double long1, double lat2, double long2)
{
	deg_start_lat = lat1;
	deg_start_long = long1;
	deg_end_lat = lat2;
	deg_end_long = long2;
}

BYTE *CDIBSectionLite::ReadTiff(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect)
{
	double adfGeoTransform[6];
	int nXSize, nYSize;
	byte *temp, *data;
	GDALDataset *poDataset;
	GDALDriver *poDriver;
	GDALRasterBand *poBand;
	GDALColorTable *poColor;
	stGDALColorTable *pColorTable;
	double left, top, right, bottom;
	int Pic_X, Pic_Y;
	data = 0;
	temp = 0;

	int bGotMin, bGotMax;
	double adfMinMax[2];

	for(int i=0; i<m_pGDALColorTable.GetSize(); i++)
		delete m_pGDALColorTable.GetAt(i);
	m_pGDALColorTable.RemoveAll();

	try
	{
		GDALAllRegister();

		poDataset = (GDALDataset*)GDALOpen(filename, GA_ReadOnly);
		if(poDataset == NULL)
		{
			*isok = false;
			return data;
		}

	// 그림의 가로 size
		Pic_X = poDataset->GetRasterXSize();
	// 그림의 세로 size
		Pic_Y = poDataset->GetRasterYSize();
	// band get(Red band:1, Green band:2, Blue band:3)
		poBand = poDataset->GetRasterBand(1);
	// band x,y size get
		nXSize = poBand->GetXSize();
		nYSize = poBand->GetYSize();

		adfMinMax[0] = poBand->GetMinimum(&bGotMin);
		adfMinMax[1] = poBand->GetMaximum(&bGotMax);
		if(!(bGotMin && bGotMax))
			GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

		poColor = poBand->GetColorTable();
		if(poColor == NULL)
		{
			*isok = false;
			return data;
		} 

	// color table set
		for(int i=0; i<poColor->GetColorEntryCount()-1; i++)
		{
			GDALPaletteInterp oPalleteInterp = poColor->GetPaletteInterpretation();
			const GDALColorEntry *poColorEntry = poColor->GetColorEntry(i);
			pColorTable = new stGDALColorTable;
			pColorTable->m_c1 = poColorEntry->c1;
			pColorTable->m_c2 = poColorEntry->c2;
			pColorTable->m_c3 = poColorEntry->c3;
			pColorTable->m_c4 = poColorEntry->c4;
			m_pGDALColorTable.Add(pColorTable);
		}

	// memory alloc
		data = new byte[nXSize * nYSize*3];
		temp = new byte[nXSize * nYSize];
	// reading raster data
		poBand->RasterIO(GF_Read, 0, 0, nXSize, nYSize, temp, 
			nXSize, nYSize, GDT_Byte, 0, 0);

		rect->SetRect(0,0,0,0); 
		if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
		{
			left = adfGeoTransform[0];
			top = adfGeoTransform[3];
			right = left + (adfGeoTransform[1] * (double)Pic_X);
			bottom = top + (adfGeoTransform[5] * (double)Pic_Y);
			rect->SetRect(left, top, right, bottom); 
		}

	// image x,y rate get
		m_dFixelX = adfGeoTransform[1] * 100000.0;
		m_dFixelY = adfGeoTransform[5] * 100000.0;

		int j = nXSize*3;
		int k = 1;
		int iIndex;
		for(int i=nXSize*nYSize-1; i>=0; i--)
		{
			iIndex = (int)(temp[i]);
			if(iIndex < 0 || iIndex > m_pGDALColorTable.GetSize()-1)
			{
				data[j-3] = (int)(0);	// Blue color
				data[j-2] = (int)(0);	// Green color
				data[j-1] = (int)(0);	// Red color
			}
			else
			{
				pColorTable = m_pGDALColorTable.GetAt(iIndex);
				data[j-3] = (int)(pColorTable->m_c3);	// Blue color
				data[j-2] = (int)(pColorTable->m_c2);	// Green color
				data[j-1] = (int)(pColorTable->m_c1);	// Red color
			}
			j = j-3;
			if(j == (k-1)*nXSize*3)
			{
				k++;
				j = nXSize*3*k;
			}
		}
	// BITMAPINFOHEADER struct composition
		pBmi->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		pBmi->bmiHeader.biWidth         = nXSize;
		pBmi->bmiHeader.biHeight        = nYSize;
		pBmi->bmiHeader.biCompression   = BI_RGB;
		pBmi->bmiHeader.biBitCount      = 24;
		pBmi->bmiHeader.biPlanes        = 1;
		pBmi->bmiHeader.biClrUsed       = 0;
		pBmi->bmiHeader.biClrImportant  = 0;
		pBmi->bmiHeader.biSizeImage     = 0;
		pBmi->bmiHeader.biXPelsPerMeter = 0;
		pBmi->bmiHeader.biYPelsPerMeter = 0;
	}
	catch(...)
	{
		TRACE("error: CADRG format data open&read failed!\n");
		delete temp;
		*isok = false;
		return data;
	}
	GDALClose(poDataset);
	delete temp;
	*isok = true;
	return data;
}

BOOL CDIBSectionLite::DrawTiff(CDC* pDC, double left, double top, double right, double bottom,int nbrightness,bool type)
{
	BOOL bResult = FALSE;

	pDC->SetStretchBltMode(COLORONCOLOR);
	
	double iWidth = ((int)right - (int)left);//1픽셀 줄생김수정 (int)    이유: 소수점계산후 버림해서 다른계산과 1차이가 남
	double iHeight = ((int)bottom - (int)top);


	//if(!nbrightness) bResult = ::StretchDIBits(pDC->GetSafeHdc(), left, top, 
	//				iWidth, iHeight,
	//	            0, 0, GetWidth(), GetHeight(),
	//			    GetDIBits(), GetBitmapInfo(),
 //				    DIB_RGB_COLORS, SRCCOPY);
	//else
	{
		COLORREF colorB;
		if(nbrightness<-100) nbrightness = -100;
		if(nbrightness>100)nbrightness = 100;
		if(nbrightness<0) colorB =RGB(0,0,0);
		else colorB = RGB(255,255,255);


		CDC bitCDC;
		CBitmap m_pBitmapTemp;
		bitCDC.CreateCompatibleDC(pDC); 
		m_pBitmapTemp.CreateCompatibleBitmap(pDC,iWidth,iHeight);
		bitCDC.SelectObject(&m_pBitmapTemp);
		bitCDC.FillSolidRect(0,0,iWidth,iHeight,colorB);
		BLENDFUNCTION opt;
		opt.AlphaFormat = 0;
		opt.BlendFlags = 0;//AC_SRC_ALPHA
		opt.BlendOp = AC_SRC_OVER;
		opt.SourceConstantAlpha = nbrightness<0?-nbrightness:nbrightness;
		


		::StretchDIBits(pDC->GetSafeHdc(),left,top,iWidth,iHeight,0, 0, GetWidth(), GetHeight(),GetDIBits(), GetBitmapInfo(),DIB_RGB_COLORS, SRCCOPY);


		if(nbrightness!=0)
		{
			if(type) pDC->AlphaBlend(left,top,iWidth,iHeight,&bitCDC,0,0,iWidth,iHeight, opt);
			else
			{
				bitCDC.BitBlt(0,0,iWidth,iHeight,pDC,left,top,SRCCOPY);
				for(int i = 0;i <= iHeight;i++)
				{
					for(int k = 0;k <= iWidth;k++)
					{
						COLORREF colorP = bitCDC.GetPixel(k,i);
						int iR = colorP/256/256,iG = (colorP/256)%256 ,iB= colorP%256;
						iR += nbrightness; iG += nbrightness;iB += nbrightness;
						if(iR>255) iR =255;
						if(iR<0) iR =0;
						if(iG>255) iG =255;
						if(iG<0) iG =0;
						if(iB>255) iB =255;
						if(iB<0) iB =0;
						bitCDC.SetPixel(k,i,iR*256*256+iG*256+iB);
						
					}
				}
				pDC->BitBlt(left,top,iWidth,iHeight,&bitCDC,0,0,SRCCOPY);
			}
		}

		

	}
	return TRUE;
}

BOOL CDIBSectionLite::DrawCibEX(CDC* pDC, double left, double top, double right, double bottom)
{ 
	pDC->SetStretchBltMode(COLORONCOLOR);

	int ioffsetX = 1;
	int ioffsetY = 1;
	 
	int iWidth = (right - left) + 1 + ioffsetX;
	int iHeight = (bottom - top) + 1 - ioffsetY;

	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->Rectangle(left, top, right+200, bottom+200);


	 LPVOID ppvBits = GetDIBits();
	 LPBITMAPINFO DIBinfo = GetBitmapInfo(); 

	::StretchDIBits(pDC->GetSafeHdc(), left, top, iWidth, iHeight,
		            0, 0, iWidth,iHeight,
				    ppvBits, DIBinfo,
				    DIB_RGB_COLORS, SRCCOPY);
	return TRUE;
}

void CDIBSectionLite::ExtractCadrgInfo(CString FullPath, CString & Left, CString & Top, CString & Right, CString  & Bottom )
{	
	double adfGeoTransform[6]; 
	GDALDataset *poDataset; 
	GDALRasterBand *poBand; 
	double left, top, right, bottom;
	int Pic_X, Pic_Y;
 
	int bGotMin, bGotMax;
	double adfMinMax[2];
		
	GDALAllRegister();
	poDataset = (GDALDataset*)GDALOpen(FullPath, GA_ReadOnly);
	if(poDataset == NULL) return;
	 
	// 그림의 가로 size
	Pic_X = poDataset->GetRasterXSize();
	// 그림의 세로 size	
	Pic_Y = poDataset->GetRasterYSize();

	if(poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		left = adfGeoTransform[0];
		top = adfGeoTransform[3];
		right = left + (adfGeoTransform[1] * (double)Pic_X);
		bottom = top + (adfGeoTransform[5] * (double)Pic_Y); 
	}		
	Left.Format("%f", left);
	Top.Format("%f", top);
	Right.Format("%f", right);
	Bottom.Format("%f", bottom);
	
	GDALClose(poDataset);
}

BOOL CDIBSectionLite::DrawTCTL(CDC* pDC, double left, double top, double right, double bottom)
{		
	BOOL bResult = FALSE;
 
		
    if (!m_hBitmap)
        return FALSE;
	 
    CSize size = GetSize();
    CPoint SrcOrigin = CPoint(0,0);
	CPoint ptDest(0, 0); 
	
	      CPalette* pOldPalette = NULL;
      if (m_Palette.m_hObject && UsesPalette(pDC))
        {
            pOldPalette = pDC->SelectPalette(&m_Palette, false);
            pDC->RealizePalette();
        }
			 
		size.cx = (right - left) ;//* 104/100;// m_dFixelX;;
		size.cy = (bottom - top );// * 101/100;// m_dFixelY;
		ptDest = CPoint(left, top);  

      bResult = Stretch(pDC, ptDest, size, m_iColorDataType); 
		
		
	  if (pOldPalette)
            pDC->SelectPalette(pOldPalette, FALSE); 


	

    
    return bResult;
}
 
//
//BOOL CDIBSectionLite::DrawTiff_B(CDC* pDC, double left, double top, double right, double bottom,int nbrightness)
//{
//	if(nbrightness<-100) nbrightness =-100;
//	if(nbrightness>100) nbrightness =100;
//	BOOL bResult = FALSE;
//
//	pDC->SetStretchBltMode(COLORONCOLOR);
//	
//	double iWidth = (right - left);
//	double iHeight = (bottom - top);
//	CDC TempDC,FillterDC;
//	TempDC.CreateCompatibleDC(pDC); 
//	FillterDC.CreateCompatibleDC(pDC); 
//	CBitmap m_pBitmapTemp,m_pBitmapTemp2;
//	m_pBitmapTemp.CreateCompatibleBitmap(pDC,iWidth,iHeight);
//	m_pBitmapTemp2.CreateCompatibleBitmap(pDC,iWidth,iHeight);
//	TempDC.SelectObject(&m_pBitmapTemp);
//	FillterDC.SelectObject(&m_pBitmapTemp);
//
//	bResult = ::StretchDIBits(TempDC.GetSafeHdc(), left, top, 
//					iWidth, iHeight,
//		            0, 0, GetWidth(), GetHeight(),
//				    GetDIBits(), GetBitmapInfo(),
// 				    DIB_RGB_COLORS, SRCCOPY);
//
//	if(nbrightness >= 0)FillterDC.FillSolidRect(left, top, right, bottom,RGB(255,255,255));
//	else {
//		nbrightness = -nbrightness;
//		FillterDC.FillSolidRect(left, top, right, bottom,RGB(0,0,0));
//	}
//	BLENDFUNCTION opt;
//	opt.AlphaFormat = 0;
//	opt.BlendFlags = 0;//AC_SRC_ALPHA
//	opt.BlendOp = AC_SRC_OVER;
//	opt.SourceConstantAlpha = nbrightness;
//
//	TempDC.AlphaBlend(left,top,iWidth, iHeight,&FillterDC,left,top,iWidth, iHeight, opt);
//
//	pDC->BitBlt(left,top,iWidth, iHeight,&TempDC,left,top,SRCCOPY);
//
//	return TRUE;
//}