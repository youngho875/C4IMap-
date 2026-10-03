#if !defined(AFX_CDIBSECTIONLITE_H__35D9F3D4_B960_11D2_A981_2C4476000000__INCLUDED_)
#define AFX_CDIBSECTIONLITE_H__35D9F3D4_B960_11D2_A981_2C4476000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DIBSectionLite.h : header file
//

// Copyright ?Dundas Software Ltd. 1999, All Rights Reserved

// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	NO	Derived from CWnd
//	NO	Is a CWnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO 	Needs a resource (template)
//	YES	Persistent objects (saveable on disk)      
//	YES	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         

// DIBSection is DIBSection wrapper class for Win32 platforms.
// This class provides a simple interface to DIBSections including loading,
// saving and displaying DIBsections.
//

// Using DIBSection :

// This class is very simple to use. The bitmap can be set using either SetBitmap()
// (which accepts either a Device dependant or device independant bitmap, or a 
// resource ID) or by using Load(), which allows an image to be loaded from disk. 
// To display the bitmap simply use Draw or Stretch.
//
// eg. 
//
//      CDIBSectionLite dibsection;
//      dibsection.Load(_T("image.bmp"));
//      dibsection.Draw(pDC, CPoint(0,0));  // pDC is of type CDC*
//
//      CDIBSectionLite dibsection;
//      dibsection.SetBitmap(IDB_BITMAP); 
//      dibsection.Draw(pDC, CPoint(0,0));  // pDC is of type CDC*
//
// The CDIBSectionLite API includes many methods to extract information about the
// image, as well as palette options for getting and setting the current palette.
//
// Author: Chris Maunder (cmaunder@dundas.com)
// Date  : 12 April 1999

#include <vfw.h>
#pragma comment(lib, "vfw32")

#include "GeoDC/Coord.h"
#include "GeoDC/CoordMgr.h"


/////////////////////////////////////////////////////////////////////////////
// defines

/** is always "BM" = 0x4D42 */
#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')

/////////////////////////////////////////////////////////////////////////////
// BITMAPINFO wrapper

/** DIB info struct */
struct DIBINFO : public BITMAPINFO
{
	/** Color table info - adds an extra 255 entries to palette */
	RGBQUAD	 arColors[255];
	operator LPBITMAPINFO()          { return (LPBITMAPINFO) this; }
	operator LPBITMAPINFOHEADER()    { return &bmiHeader;          }
	RGBQUAD* ColorTable()            { return bmiColors;           }
};

/////////////////////////////////////////////////////////////////////////////
// LOGPALETTE wrapper

/** palette info struct */
struct PALETTEINFO : public LOGPALETTE
{
	/** Palette entries */
    PALETTEENTRY arPalEntries[255];
    PALETTEINFO() { palVersion = (WORD) 0x300; }
    operator LPLOGPALETTE()   { return (LPLOGPALETTE) this;            }
    operator LPPALETTEENTRY() { return (LPPALETTEENTRY) (palPalEntry); }
};

/////////////////////////////////////////////////////////////////////////////
// GDALColorTable
// Red, Green, Blue and Alpha in (in c1, c2, c3 and c4), GPI_RGB=1
// Cyan, Magenta, Yellow and Black (in c1, c2, c3 and c4), GPI_CMYK=2,
// Hue, Lightness and Saturation (in c1, c2, and c3), GPI_HLS=3
struct stGDALColorTable
{
	short	m_c1;
	short	m_c2;
	short	m_c3;
	short	m_c4;
};

/////////////////////////////////////////////////////////////////////////////
// CDIBSectionLite object

/**
* 레스터지도데이터(tiff format) 관리 class
* @author 박종원
* @version 1.0
* @since 2005.11.
**/
class CDIBSectionLite : public CObject
{
// Construction
public:
	CDIBSectionLite();
	virtual ~CDIBSectionLite();
    void DeleteObject();

// static helpers
public:
	/** line의 바이트 수 얻기 */
    static int BytesPerLine(int nWidth, int nBitsPerPixel);
	/** get color */
    static int NumColorEntries(int nBitsPerPixel);
	/** color value save array */
    static PALETTEENTRY ms_StdColours[];
    static BOOL UsesPalette(CDC* pDC) { return (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE); }
    static BOOL CreateHalftonePalette(CPalette& palette, int nNumColours);
	/** logpalette struct pointer */
	LOGPALETTE *m_pPalette;

// Attributes
public:
	//CGGeoObject*	m_pGeoObj;

	/** bitmap handle get */
    HBITMAP      GetSafeHandle() const       { return (this)? m_hBitmap : NULL;        }
	/** image size get */
    CSize        GetSize() const             { return CSize(GetWidth(), GetHeight());  }
	/** image height get */
    int          GetHeight() const           { return m_DIBinfo.bmiHeader.biHeight;    } 
	/** image width get */
    int          GetWidth() const            { return m_DIBinfo.bmiHeader.biWidth;     }
	/** image planes get */
    int          GetPlanes() const           { return m_DIBinfo.bmiHeader.biPlanes;    }
	/** image header count get */
    int          GetBitCount() const         { return m_DIBinfo.bmiHeader.biBitCount;  }
	/** DIBit get */
    LPVOID       GetDIBits()                 { return m_ppvBits;                       }
	/** bitmap info get */
    LPBITMAPINFO GetBitmapInfo()             { return  (BITMAPINFO*) m_DIBinfo;        }
	/** image size get */
    DWORD        GetImageSize() const        { return m_DIBinfo.bmiHeader.biSizeImage; }
	/** bitmap info header get */
    LPBITMAPINFOHEADER GetBitmapInfoHeader() { return (BITMAPINFOHEADER*) m_DIBinfo;   }
	/** bitmap set */
    BOOL SetBitmap(UINT nIDResource);
	/** bitmap set */
    BOOL SetBitmap(LPCTSTR lpszResourceName);
	/** bitmap set */
    BOOL SetBitmap(HBITMAP hBitmap, CPalette* pPalette = NULL);
	/** bitmap set */
    BOOL SetBitmap(LPBITMAPINFO lpBitmapInfo, LPVOID lpBits);
	/** bitmap set */
//	BOOL SetBitmap(LPBITMAPINFO lpBitmapInfo, GDALColorTable *poColorTable, LPVOID lpBits);
	/** palette get */
    CPalette *GetPalette()  { return &m_Palette; }
	/** palette set */
    BOOL SetPalette(CPalette* pPalette);
	/** log palette set */
    BOOL SetLogPalette(LOGPALETTE* pLogPalette);
    BOOL SetDither(BOOL bDither);
    BOOL GetDither();

// Operations
public:
	/** raster file open */
    BOOL Load(LPCTSTR lpszFileName);
	/** raster file save */
    BOOL Save(LPCTSTR lpszFileName);
	/** raster data draw */
	BOOL DrawCIB(CDC* pDC, double left, double top, double right, double bottom);
	BOOL Draw(CDC* pDC, CRect rect);
	BOOL Draw(CDC* pDC, double left, double top, double right, double bottom);
	BOOL DrawCADRG(CDC* pDC, double left, double top, double right, double bottom);
	BOOL Draw(CDC* pDC, CPoint ptDest, BOOL bForceBackground = FALSE);
    BOOL Stretch(CDC* pDC, CPoint ptDest, CSize size, BOOL bForceBackground = FALSE);
	/** CIB(*.I42, Controlled Image Base) format raster data Open & Read */
	// 2006-09-25 byte -> BYTE 수정
	BYTE * ReadCIB(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect);
	/** CADRG(*.TL2, Compressed ARC Digitized Raster Graphics) format raster data Open & Read */
	BYTE * ReadCADRG(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect);
	/** DTED1(*.dt1, Controlled Image Base) format raster data Open & Read */
	void ReadDTED(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect);

// Overrideables
// Implementation
// Implementation
public:
	void dibSetTrueColor(HBITMAP* phDIB);
	void dibSetBrightness(HBITMAP hDIB, int nLevel);
	BOOL DrawRaster(CDC *pDC, int nScreenWidth, int nScreenHeight, float fCenterX, float fCenterY, float fScale);
	void SetStartEnd(double x1, double y1, double x2, double y2); // jykim raster

	CCoord				m_Coord; 
	CCoordMgr*			m_CoordMgr; 

	// jykim raster
	double deg_start_lat;
	double deg_start_long;
	double deg_end_lat;
	double deg_end_long;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
protected:
    void _ShowLastError();
	/** palette create */
    BOOL CreatePalette();
	/** palette create */
	BOOL CreatePalette2();
	/** color table set */
    BOOL FillDIBColorTable(UINT nNumColours, RGBQUAD *pRGB);

protected:
	/** Handle to DIBSECTION */
    HBITMAP  m_hBitmap;
	/** Bitmap header & color table info */
    DIBINFO  m_DIBinfo;
	/** Pointer to bitmap bits */
    VOID    *m_ppvBits;
	/** color data type (palette or RGB values) */
    UINT     m_iColorDataType;
	/** Size of color table */
    UINT     m_iColorTableSize;
	/** Color palette */
    CPalette m_Palette;
	/** Use DrawDib routines for dithering? */
    BOOL     m_bDither;
	/** handle to a DrawDib DC */
    HDRAWDIB m_hDrawDib;
	/** fixel x size (use NITFF format raster data) */
	double		m_dFixelX;
	/** fixel y size (use NITFF format raster data) */
	double		m_dFixelY;
	/** GDALColorTable list */
	CTypedPtrArray<CPtrArray, stGDALColorTable*> m_pGDALColorTable;

public:
	BOOL DrawCibEX(CDC* pDC, double left, double top, double right, double bottom);
	BYTE * ReadTiff(CString filename, BITMAPINFO *pBmi, bool *isok, CGRectGP *rect);
	BOOL DrawTiff(CDC* pDC, double left, double top, double right, double bottom,int nbrightness = 0,bool type = 0);
	void ExtractCadrgInfo(CString FullPath, CString & Left, CString & Top, CString & Right, CString  & Bottom );	
	BOOL DrawTCTL(CDC* pDC, double left, double top, double right, double bottom);
	//BOOL DrawTiff_B(CDC* pDC, double left, double top, double right, double bottom,int nbrightness);
	//BOOL DrawTCTL(CDC* pDC, double left, double top, double right, double bottom, float fCenterX, float fCenterY, float fScale);

	double mdoffset;		 
	double          GetFixelX() const  { return m_dFixelX;    } 
    double          GetFixelY() const  { return m_dFixelY;    }

private:
	/** Storage for previous bitmap in Memory DC */
    HBITMAP  m_hOldBitmap;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDIBSECTIONLITE_H__35D9F3D4_B960_11D2_A981_2C4476000000__INCLUDED_)
