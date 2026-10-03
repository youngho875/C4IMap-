// GLib.cpp: implementation of the CGLib class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GLib.h"
#include <algorithm> 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	FROM_GRgbHsl

//////////////////////////////////////////////////////////////////////
// CGLib color utility functions

COLORREF CGLib::AlphaBlendRGB(COLORREF color, int alpha)
{
	RGBQUAD rgbCol, rgbTemp;
	int		r=0, g=0, b=0;
	
	rgbCol.rgbRed	= static_cast<unsigned char>(color & 0xFF);
	rgbCol.rgbGreen = static_cast<unsigned char>((color & 0xFF00) >> 8);
	rgbCol.rgbBlue	= static_cast<unsigned char>((color & 0xFF0000) >> 16);
	rgbCol.rgbReserved = 0x00;
	
	rgbTemp.rgbRed		= 0xFF - rgbCol.rgbRed;
	rgbTemp.rgbGreen	= 0xFF - rgbCol.rgbGreen;
	rgbTemp.rgbBlue		= 0xFF - rgbCol.rgbBlue;

	rgbTemp.rgbReserved = 0x00;

	if(rgbTemp.rgbRed)		r = (rgbTemp.rgbRed * alpha)   / 100;
	if(rgbTemp.rgbGreen)	g = (rgbTemp.rgbGreen * alpha) / 100;
	if(rgbTemp.rgbBlue)		b = (rgbTemp.rgbBlue * alpha)  / 100;


	if(r)	color += r;
	if(g)	color += (g << 8);
	if(b)	color += (b << 16);

	return color;
}

void CGLib::RGBtoHSL( COLORREF rgb, double *H, double *S, double *L )
{
   double delta;
   double r = (double)GetRValue(rgb) / 255;
   double g = (double)GetGValue(rgb) / 255;
   double b = (double)GetBValue(rgb) / 255;
   double cmax = std::max(r, std::max(g, b));
   double cmin = std::min(r, std::min(g, b));
   *L = (cmax + cmin) / 2.0;
   if (cmax == cmin) {
      *S = 0;
      *H = 0; // it's really undefined
   } else {
      if (*L < 0.5) 
         *S = (cmax - cmin) / (cmax + cmin);
      else
         *S = (cmax - cmin) / (2.0 - cmax - cmin);
      delta = cmax - cmin;
      if (r == cmax)
          *H = (g - b) / delta;
      else if (g == cmax)
          *H = 2.0 + ( b - r) / delta;
      else
          *H = 4.0 + (r - g) / delta;
      *H /= 6.0;
      if(*H < 0.0)
          *H += 1;
   }
}


COLORREF CGLib::HSLtoRGB( double H, double S, double L )
{
#ifdef FROM_GRgbHsl	// from CommonMathb
	double	r, g, b;
	double	m1, m2;

	if (S == 0) {
		r = g = b = L;
	}
	else {
		if (L <= 0.5) {
			m2 = L * (1.0 + S);
		}
		else {
			m2 = L + S - L * S;
		}

		m1 = 2.0 * L - m2;
		r = HuetoRGB(m1, m2, H + 1.0 / 3.0);
		g = HuetoRGB(m1, m2, H);
		b = HuetoRGB(m1, m2, H - 1.0 / 3.0);
	}

	return RGB((BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));

#else	// from CommonMathb.cpp

  double r,g,b;
  double m1, m2;

  if (S == 0) {
     r = g = b = L;
  } else {
     if (L <= 0.5)
        m2 = L * (1.0 + S);
     else
        m2 = L + S - L * S;
     m1 = 2.0 * L - m2;
     r = HuetoRGB(m1, m2, H + 1.0 / 3.0);
     g = HuetoRGB(m1, m2, H);
     b = HuetoRGB(m1, m2, H - 1.0 / 3.0);
  }
  return RGB((BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));

#endif // FROM_GRgbHsl
}


double CGLib::HuetoRGB(double m1, double m2, double h )
{
   if (h < 0) h += 1.0;
   if (h > 1) h -= 1.0;
   if (6.0 * h < 1 )
      return ( m1 + (m2 - m1) * h * 6.0);
   if (2.0 * h < 1)
      return m2;
   if (3.0 * h < 2.0)
      return (m1 + (m2 - m1) * ((2.0 / 3.0) - h) * 6.0);
   return m1;
}


void CGLib::SetInitColor(COLORREF *dwColor)
{
	// 현재의 색상 값이 변경된 값인지 아닌지를 설정한다.
	// 만약 색상의 상위 1바이트의 값이 0일 경우는 기본색상임
	// 0이 아니면 값을 저장하고 있는 경우임
	*dwColor = *dwColor | (((DWORD) 255) << 24);
}


void CGLib::SetGrayValue(COLORREF *color, BOOL bColorMode, int nLumino)
{
#ifdef FROM_GRgbHsl	// from GRgbHsl.cpp

	COLORREF dwColor = *color;
	COLORREF retColor = dwColor;
	double H, S, L;

//	int lumino = (BYTE) (*color >> 24);

	if (bColorMode || nLumino == 50050)	// 기본색상 값을 유지함
		return;
	else	// Gray
	{
		// 현재 색상을 HLS값으로 변경한다.
		RGBtoHSL(dwColor, &H, &S, &L);

		int nPerS, nPerL;
		nPerS = nLumino / 1000;
		nPerL = nLumino % 1000;

		BOOL bHPerS, bHPerL;
		bHPerS = bHPerL = FALSE;

		// 채도
		if (nPerS > 50) 
		{
			bHPerS = TRUE;
			nPerS -= 50;
		}
		nPerS *= 2;

		// 명도
		if (nPerL > 50) 
		{
			bHPerL = TRUE;
			nPerL -= 50;
		}
		nPerL *= 2;

		//	int nH = 239;
		//	int nS = 240;
		//	int nL = 240;
//		float dH = H;
//		float dS = S;
//		float dL = L;
		double dH = H;
		double dS = S;
		double dL = L;

		if (bHPerS == TRUE)
		{
			if (bHPerL == TRUE)
			{	// 채도, 명도 상위
				dS = (1.0 - S) * (nPerS / 100.0) + S;
				dL = (1.0 - L) * (nPerL / 100.0) + L;
			}
			else
			{	// 채도 상위
				dS = (1.0 - S) * (nPerS / 100.0) + S;
				dL = L * (nPerL / 100.0);
			}
		}
		else
		{
			if (bHPerL == TRUE)
			{	// 명도 상위
				dS = S * (nPerS / 100.0);
				dL = (1.0 - L) * (nPerL / 100.0) + L;
			}
			else
			{	// 채도, 명도 하위
				dS = S * (nPerS / 100.0);
				dL = L * (nPerL / 100.0);
			}
		}

		// 파라미터의 명도 값에 맞는 색상으로 변경한다.
		if (0.0 > dS) dS = 0.0;
		if (1.0 < dS) dS = 1.0;

		if (0.0 > dL) dL = 0.0;
		if (1.0 < dL) dL = 1.0;

		retColor = HSLtoRGB(dH, dS, dL);
	
		*color = retColor;
	}
	return;

#else	// from CommonMathb.cpp

	COLORREF dwColor = *color;
	COLORREF retColor = dwColor;
	double H, S, L;

	int lumino = (BYTE) (*color >> 24);

	if (bColorMode)	// Color
	{
		// 현재의 명도값으로 RGB 값을 계산한다.
		if (nLumino == 255 && lumino != 255)
		{
			RGBtoHSL(dwColor, &H, &S, &L);
			if (lumino == 241) lumino = 0;	// 검정색인 경우
			retColor = HSLtoRGB(H, (double) lumino / (double) 240, L);
		}

		*color = retColor | (((DWORD) nLumino) << 24);
	}
	else	// Gray
	{
		// 현재 색상을 HLS값으로 변경한다.
		RGBtoHSL(dwColor, &H, &S, &L);

		int nH, nS, nL;
		nH = (int) (H * (double) 239);
		nS = (int) (S * (double) 240);
		nL = (int) (L * (double) 240);

		// 파라미터의 명도 값에 맞는 색상으로 변경한다.
		retColor = HSLtoRGB(H, (double) nLumino / (double) 240, L);
	
		int preL = (int) (S * (double) 240);
		if (S == 0)	// 검정색인 경우
			S = 241;
		retColor = retColor | (((DWORD) preL) << 24);
		*color = retColor;
	}

#endif // FROM_GRgbHsl

}


void CGLib::SetOriginValue(COLORREF *color, BOOL bColorMode, int nLumino)
{
	// 색상에서 상위 1바이트를 읽는다.
	int lumino = (BYTE) (*color >> 24);

	if (lumino == 0) {
		// 기본 색상이므로 리턴한다.
		return;
	}

	if (!(1 <= lumino && lumino <= 200)) {
		// 값의 범위를 벗어남
		return;
	}

	// 컬러 모드이거나 명도값이 255, nLumino와 같으면 리턴한다.
	if (bColorMode || lumino == 255 || lumino == nLumino) {
		return;
	}

	// *color 값이 원래의 color 값이 아닌 경우
	// 원래의 컬러 값으로 바꾼다.
	SetGrayValue(color, TRUE, 255);
}


COLORREF CGLib::GetDisplayColor(COLORREF dwColor)
{
	COLORREF retColor;

	BYTE R, G, B;

	R = GetRValue(dwColor);
	G = GetGValue(dwColor);
	B = GetBValue(dwColor);

	retColor = RGB(R, G, B);
	return retColor;
}

// 색상 모드와 색도 및 명도의 값으로 변환된 색상을 리턴하는 함수
COLORREF CGLib::GetDisplayColor(COLORREF color, BOOL bColorMode, int nLumino)
{
#ifdef FROM_GRgbHsl	// from GRgbHsl.cpp

	COLORREF retColor = color;

	// 컬러 모드인 경우 리턴한다.
	// 컬러모드가 아닌 경우라도 색도, 명도 값이 모두 100%이면 리턴한다.
	if (!(bColorMode || nLumino == 50050))
		SetGrayValue(&retColor, bColorMode, nLumino);

	return retColor;

#else	// from CommonMathb.cpp

	if (nLumino == 255)
		return color;

	SetInitColor(&color);
	SetOriginValue(&color, bColorMode, nLumino);
	SetGrayValue(&color, bColorMode, nLumino);
	return GetDisplayColor(color);

#endif // FROM_GRgbHsl
}

COLORREF CGLib::GetDisplayColor(COLORREF color, int nLumino)
{
	if (nLumino == 255)
		return color;

	return GetDisplayColor(color, FALSE, nLumino);
}


COleDocument *CGLib::GetOleDocument()
{
 	CFrameWnd* pFrameWnd = (CFrameWnd*)(AfxGetApp()->m_pMainWnd);
   COleDocument* pDocument =  (COleDocument*)pFrameWnd->GetActiveDocument();


   if ( !pDocument )
      return NULL;

   // Fail if view is of wrong kind
//   if ( ! pView->IsKindOf( RUNTIME_CLASS(CMyView) ) )
//    return NULL;

   return pDocument;
}
