// shpPoint.cpp: implementation of the shpPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "shpPoint.h"
#include "mapdatamgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

shpPoint::shpPoint()
{
	m_hFMap = m_hFMapShx = NULL;
	m_iFontSize = 14;
	m_lfCharSet = ANSI_CHARSET;
	m_lfStrikeOut = FALSE;
	m_crFontColor = RGB(0,0,0);
}

shpPoint::~shpPoint()
{
	m_vSHPPoint.clear();
}

bool shpPoint::Open(CString _filename)
{
	if(!shpObject::Open(_filename))
		return false;

	GPoint tmpPoint;
	int tmp;

	// File pointer..
	FILE* fpSHP = fopen(m_strSHPPath, "rb");
	FILE* fpSHX = fopen(m_strSHXPath, "rb");

	// prepare adequate space..
	m_vSHPPoint.reserve(m_nRecords);

	// now real data reading..
	for(int i = 0; i < m_nRecords; ++i)
	{
		fseek(fpSHX, 100 + i * 8, SEEK_SET);// 
		fread(&tmp, 4, 1, fpSHX);			// shx 파일에서 레코드의 시작위치값을 가져온다. 
		SwapWord(4, &tmp); 
		tmp=tmp * 2;

		fseek(fpSHP, tmp + 12, SEEK_SET);

		tmpPoint.x = 0;
		tmpPoint.y = 0;
		fread(&tmpPoint.x, 8, 1, fpSHP);
		fread(&tmpPoint.y, 8, 1, fpSHP);

		m_vSHPPoint.push_back(tmpPoint);
	}
	fclose(fpSHP);
	fclose(fpSHX);

	return true;
}


void shpPoint::Draw(CDC* pDC, double _ZoomFactor, GPoint _ptMapCenter, const CRect& _rect, double _lfRatio)
{
	m_lfZoomFactor = _ZoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rect.Width() / 2);
	m_scrCenterPoint.y = int(_rect.Height() / 2);
	m_lfRatio = _lfRatio;

	CPen pen;
	CBrush	brush;

	// for intersect calculation..
	CRect	mbrRect, rectBuffer;
	CPoint	tmpPoint;

	mbrRect = WorldToDevice(m_MBR);
	if(!rectBuffer.IntersectRect(&mbrRect, &_rect))
		return;

	// DC saving..
	int savedDC = pDC->SaveDC();

	pen.CreatePen(m_nStyle, m_nWidth, m_objColor);
	brush.CreateSolidBrush(m_objColor);

	pDC->SelectObject(&pen);
	pDC->SelectObject(&brush);

	int nSize = m_vSHPPoint.size();
	int nOffset = (2 + m_nWidth);

	// now drawing..
	for(int i=0; i < nSize; ++i)
	{
		tmpPoint = WorldToDevice(m_vSHPPoint[i]);
		pDC->Ellipse(tmpPoint.x - nOffset, tmpPoint.y - nOffset, tmpPoint.x + nOffset, tmpPoint.y + nOffset);
	}

	// if selected, selected point should be drawn
	DrawSelected(pDC);

	// labeling...
	if(m_bLabel)
		DrawLabel(pDC);

	// DC restoring..
	pDC->RestoreDC(savedDC);
}

void shpPoint::DrawSelected(CDC* _pDC)
{
	CBrush	brush;
	CPen	pen;
	CPoint	tmpPoint;

	int savedDC = _pDC->SaveDC();

	pen.CreatePen(PS_SOLID, 0, m_objColor);
	brush.CreateSolidBrush(RGB(255, 0, 0));

	_pDC->SelectObject(&pen);
	_pDC->SelectObject(&brush);

	int nOffset = 3 + m_nWidth;

	for(int i = 0; i < m_vSelectedID.size(); ++i)
	{
		tmpPoint = WorldToDevice(m_vSHPPoint[m_vSelectedID[i]]);
		_pDC->Rectangle(tmpPoint.x - nOffset, tmpPoint.y - nOffset, tmpPoint.x + nOffset, tmpPoint.y + nOffset);
	}

	_pDC->RestoreDC(savedDC);
}

void shpPoint::DrawLabel(CDC* _pDC)
{
	CFont	m_font;
	CPoint	tmpPoint;
	int		nHeight;

	if(_pDC->IsPrinting())
		nHeight = 12 * _pDC->GetDeviceCaps(HORZRES) / 1024;
	else
		nHeight = 12;
	
	int savedDC = _pDC->SaveDC();
	
	m_font.CreateFont(nHeight, nHeight * 0.666,
		0, 0, 0,
		FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "돋움체");
		
	_pDC->SelectObject(&m_font);
	_pDC->SetBkMode(TRANSPARENT);
	_pDC->SetTextColor(RGB(0, 0, 0));
	_pDC->SetTextAlign(0|0);

	int nSize = m_vSHPPoint.size();
	for(int i=0; i < nSize; ++i)
	{
		CString tmpString = m_DBF.ReadStringAttribute(i, m_nLabelField);
		tmpPoint = WorldToDevice(m_vSHPPoint[i]);
		_pDC->TextOut(tmpPoint.x, tmpPoint.y, tmpString);
	}
	_pDC->RestoreDC(savedDC);
}


int	shpPoint::Track(CWnd* pWnd, GPoint point)
{
	// 일단 기존 selected point를 제거한다..
	m_vSelectedID.clear();

	// 점을 찍었는지 선을 찍었는지 hit test 한다.
	vector<GPoint>::iterator iter;
	int ID;

	CRect rect;
	CPoint node;

	hit_status hit_test = HitTest(point, ID);
	*iter = m_vSHPPoint[ID];

	if(hit_test != hitPoint)
	// 아무데도 아니다.
	{
		if (m_eStatus == stateSelect)
		{
			m_eStatus = stateNone;

			// 다시 그리라고 한다.
			pWnd->InvalidateRect(NULL);
		}
		return -1;
	}
	m_vSelectedID.push_back(ID);

	// 원래 선택이 되어있지 않았다면, 선택된 것을 표시하기 위해 한번 그려준다.
	if (m_eStatus == stateNone)
	{
		m_eStatus = stateSelect;
		pWnd->InvalidateRect(NULL);				

		// 선택되어 있지 않았을 때는 점을 옮기는 것은 지원 안한다.
		return ID;
	}
	m_eStatus = stateSelect;	

	pWnd->InvalidateRect(NULL);

	return ID;
}

shpObject::hit_status shpPoint::HitTest(GPoint _GPoint, int& _nID)
{
	vector<GPoint>::iterator itr;
	// 점 위를 일단 먼저 본다.
	itr = m_vSHPPoint.begin();
	for (int i = 0; i < m_vSHPPoint.size(); ++i)
	{
		// mouse pointer가 점 근처에 있나 본다.
		if (PtInRect(*itr, _GPoint))
		{
			_nID = i;
			return hitPoint;
		}
		++itr;
	}
	return hitNone;
}

BOOL shpPoint::GetSelRect(int iSelID, double &left, double &top, double &right, double &bottom)
{
	BOOL bRes = FALSE;

	return bRes;
}

bool shpPoint::OpenMemoryMap(CString _filename)
{
	if(!shpObject::Open(_filename))
		return false;

	HANDLE hFile;
	hFile = CreateFile(_filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile)
		return FALSE;
	m_hFMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	hFile = CreateFile(m_strSHXPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile)
		return FALSE;
	m_hFMapShx = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	if(m_hFMap == NULL || m_hFMapShx == NULL)
		return false;

	return true;
}


void shpPoint::SetFontSize(int iFontSize)
{
	m_iFontSize = iFontSize;
}

void shpPoint::SetFontType(BYTE lfCharSet)
{
	m_lfCharSet = lfCharSet;
}

void shpPoint::SetFontStrikeOut(BYTE lfStrikeOut)
{
	m_lfStrikeOut = lfStrikeOut;
}

void shpPoint::SetFontColor(COLORREF crFontColor)
{
	m_crFontColor = crFontColor;
}

void shpPoint::SetFaceName(CString strFaceName)
{
	m_strFaceName = strFaceName;
}

void shpPoint::SetVisibleType(int iVisibleType)
{
	m_iVisibleType = iVisibleType;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//		
//
/////////////////////////////////////////////////////////////////////////////////////////


void shpPoint::Draw(CDC* pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	m_lfZoomFactor = _ZoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rect.Width() / 2);
	m_scrCenterPoint.y = int(_rect.Height() / 2);
	m_lfRatio = _lfRatio;

	// DC saving..
	int savedDC = pDC->SaveDC();
	int nSize = m_vSHPPoint.size();

	// now drawing..
	double lx, ly, dx, dy, ex, ey;
	CGPointGP gpLT, gpRB;
	CGRectGP gpRect;
	CPoint point, screenpt;
	CGPointGP gpoint;
	CPoint point1, point2;
	CRect rc;

	for(int i=0; i < nSize; ++i)
	{
		gpoint.SetPoint(m_vSHPPoint[i].x, m_vSHPPoint[i].y);
		if(!_rect.PtInRect(gpoint))
			continue;

		//_pDC->GeoPtoDP(&point, &gpoint);

		m_Coord.degrees_lat_long_to_x_y (gpoint.m_P.lat, gpoint.m_P.lon, &lx, &ly); 
		m_CoordMgr->WorldXY2ScreenXY(lx, ly, &point.x, &point.y);

		point1.x = point.x - m_dPointLen;
		point1.y = point.y - m_dPointLen;
		point2.x = point.x + m_dPointLen;
		point2.y = point.y + m_dPointLen;

		//_pDC->DPtoGeoP(&gpLT, &point1);
		m_CoordMgr->ScreenXY2WorldXY(point1.x, point1.y, &dx, &dy);
		m_Coord.x_y_to_degrees_lat_long(dx, dy, &gpLT.m_P.lat, &gpLT.m_P.lon);

		//_pDC->DPtoGeoP(&gpRB, &point2);
		m_CoordMgr->ScreenXY2WorldXY(point.x, point.y, &ex, &ey);
		m_Coord.x_y_to_degrees_lat_long(ex, ey, &gpRB.m_P.lat, &gpRB.m_P.lon);

		gpRect.SetRect(gpLT, gpRB);

		switch(m_nPointType)
		{
			rc.left = gpRect.m_R.left;
			rc.top = gpRect.m_R.top;
			rc.right = gpRect.m_R.right;
			rc.bottom = gpRect.m_R.bottom;

			case 0 :
				//_pDC->Ellipse(&gpRect);
				pDC->Ellipse(&rc);
				break;
			case 1 : 
				//_pDC->Rectangle(&gpRect);
				pDC->Rectangle(&rc);
				break;
			default : 
				//_pDC->Ellipse(&gpRect);  
				pDC->Ellipse(&rc);  
		}
	}
// if selected, selected point should be drawn
//	DrawSelected(_pDC);
// labeling...

	if(m_bLabel)	
		DrawLabel(pDC);

	pDC->RestoreDC(savedDC);
}

void shpPoint::Draw(CDC* pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
					CBitmap *pBitmap, COLORREF colorTransparent)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	m_lfZoomFactor = _ZoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rect.Width() / 2);
	m_scrCenterPoint.y = int(_rect.Height() / 2);
	m_lfRatio = _lfRatio;

	CDC memDC, maskDC, tempDC;
	CBitmap maskBitmap;
	CBitmap bmpImage ;
	BITMAP bm;
	CBitmap *pOldBitmap, *pOldmaskBmp, *pold;
	CSize widthLP, widthDP;
	CPoint Src;
	CGPointGP GpSrc;
	POINT DestLP;
	CGPointGP DestGP;

	if(pBitmap == NULL) return;
	if(pBitmap->m_hObject == NULL) return;
	if(pBitmap->GetBitmap(&bm) == 0) return;

	// DC saving..
	int savedDC = pDC->SaveDC();
	int nSize = m_vSHPPoint.size();

	maskDC.CreateCompatibleDC(pDC);
	memDC.CreateCompatibleDC(pDC);
	tempDC.CreateCompatibleDC(pDC);

	pold = tempDC.SelectObject(pBitmap);

	bmpImage.CreateCompatibleBitmap(pDC,bm.bmWidth,bm.bmHeight);
	pOldBitmap = memDC.SelectObject(&bmpImage);

	memDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	memDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight,&tempDC, 0,0,bm.bmWidth,bm.bmHeight,SRCCOPY); 

	maskBitmap.CreateBitmap(bm.bmWidth,bm.bmHeight,1,1,NULL);
	pOldmaskBmp=maskDC.SelectObject(&maskBitmap); 
	memDC.CDC::SetBkColor(colorTransparent);

	maskDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	maskDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight,&memDC, 0,0,bm.bmWidth,bm.bmHeight,SRCCOPY); 

	memDC.CDC::SetBkColor(RGB(0,0,0)); 
	memDC.CDC::SetTextColor(RGB(255,255,255));
	memDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	memDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight, &maskDC, 0,0,bm.bmWidth,bm.bmHeight,SRCAND);

	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(RGB(0,0,0));

	::SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);

	if (_ZoomFactor > 0.4f)
		_ZoomFactor = 0.4f;
	double dblScale = 2.5f * _ZoomFactor;

	// now drawing..
	double lx, ly;
	CGPointGP gpLT, gpRB;
	CGRectGP gpRect;
	CPoint point;
	CGPointGP gpoint, gpoint2;
	double len = m_dPointLen;
	CPoint point1, point2;
	CPoint tpt;
	double dlimit = bm.bmWidth;

	for(int i=0; i < nSize; ++i)
	{
		gpoint.SetPoint(m_vSHPPoint[i].x, m_vSHPPoint[i].y);
		if(!_rect.PtInRect(gpoint))
			continue;

		//_pDC->GeoPtoLP(&point, &gpoint);
		m_Coord.degrees_lat_long_to_x_y (gpoint.m_P.lat, gpoint.m_P.lon, &lx, &ly); 
		m_CoordMgr->WorldXY2ScreenXY(lx, ly, &point.x, &point.y);

		DestLP.x = point.x;
		DestLP.y = point.y;

		CPoint dpt = DestLP;
		CPoint symPos;
		pDC->LPtoDP(&dpt);
		symPos = dpt;

		dpt.x -= dlimit;
		dpt.y += dlimit;
		symPos.x -= dlimit/2;
		symPos.y -= dlimit/2;

		pDC->DPtoLP(&dpt);
		pDC->DPtoLP(&symPos);

		tpt.x = DestLP.x - dpt.x;
		tpt.y = DestLP.y - dpt.y;
		widthLP = tpt;

		switch(m_nPointType)
		{
			case 0 :
			case 1 :
				break;
			case 2 : pDC->StretchBlt(symPos.x, symPos.y, (INT)(widthLP.cx * dblScale), 
						 (INT)(-widthLP.cy * dblScale), &maskDC, 0,0, bm.bmWidth,bm.bmHeight,SRCAND);
					pDC->StretchBlt(symPos.x, symPos.y, (INT)(widthLP.cx * dblScale), 
						(INT)(-widthLP.cy * dblScale), &memDC, 0,0, bm.bmWidth,bm.bmHeight,SRCPAINT);
				break;
			default : continue;
		}
	}

// if selected, selected point should be drawn
//	DrawSelected(_pDC);

	// labeling...
	if(m_bLabel)
		DrawLabel(pDC);

	tempDC.SelectObject(pold);
	maskDC.SelectObject(pOldmaskBmp);
	memDC.SelectObject(pOldBitmap);
	
	//maskDC.MemReset();
	//memDC.MemReset();
	//tempDC.MemReset();
	//ReleaseDC(maskDC);

	maskBitmap.DeleteObject();
	bmpImage.DeleteObject();

	// DC restoring..
	pDC->RestoreDC(savedDC);
}

void shpPoint::Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio,
					double dScreenScale, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2)
{
}

void shpPoint::Draw(CDC* pDC, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
					double dScreenScale, int iScale, CString strName, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2)
{
}


void shpPoint::DrawSelected(CDC* pDC, int iID)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	int nSize = m_vSHPPoint.size();

	// now drawing..
	double lx, ly, dx, dy, ex, ey;
	CGPointGP gpLT, gpRB;
	CGRectGP gpRect;
	CPoint point;
	CGPointGP gpoint;
	CPoint point1, point2;

	m_dPointLen += 2;

	for(int i=0; i<nSize; ++i)
	{
		if(iID != i) continue;

		gpoint.SetPoint(m_vSHPPoint[i].x, m_vSHPPoint[i].y);
		//pDC->GeoPtoDP(&point, &gpoint);
		m_Coord.degrees_lat_long_to_x_y (gpoint.m_P.lat, gpoint.m_P.lon, &lx, &ly); 
		m_CoordMgr->WorldXY2ScreenXY(lx, ly, &point.x, &point.y);

		point1.x = point.x - m_dPointLen;
		point1.y = point.y - m_dPointLen;
		point2.x = point.x + m_dPointLen;
		point2.y = point.y + m_dPointLen;

		//pDC->DPtoGeoP(&gpLT, &point1);
		m_CoordMgr->ScreenXY2WorldXY(point1.x, point1.y, &dx, &dy);
		m_Coord.x_y_to_degrees_lat_long(dx, dy, &gpLT.m_P.lat, &gpLT.m_P.lon);

		//pDC->DPtoGeoP(&gpRB, &point2);
		m_CoordMgr->ScreenXY2WorldXY(point2.x, point2.y, &ex, &ey);
		m_Coord.x_y_to_degrees_lat_long(ex, ey, &gpRB.m_P.lat, &gpRB.m_P.lon);

		gpRect.SetRect(gpLT, gpRB);

		CRect rc;
		rc.left = gpRect.m_R.left;
		rc.top = gpRect.m_R.top;
		rc.right = gpRect.m_R.right;
		rc.bottom = gpRect.m_R.bottom;

		pDC->Ellipse(&rc);
		break;
	}
}

void shpPoint::DrawSelected1(CDC* pDC)
{
	CBrush	brush;
	CPen	pen;
	CPoint	tmpPoint;

	int savedDC = pDC->SaveDC();

	pen.CreatePen(PS_SOLID, 0, m_objColor);
	brush.CreateSolidBrush(RGB(255, 0, 0));

	pDC->SelectObject(&pen);
	pDC->SelectObject(&brush);

	int nOffset = 3 + m_nWidth;

	for(int i = 0; i < m_vSelectedID.size(); ++i)
	{
		tmpPoint = WorldToDevice(m_vSHPPoint[m_vSelectedID[i]]);
		pDC->Rectangle(tmpPoint.x - nOffset, tmpPoint.y - nOffset, tmpPoint.x + nOffset, tmpPoint.y + nOffset);
	}

	pDC->RestoreDC(savedDC);
}

void shpPoint::DrawLabel1(CDC* pDC)
{
	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));

	int nFontSize;
	nFontSize = m_iFontSize;
	CSize cSize(nFontSize, nFontSize);
	pDC->DPtoLP(&cSize);

	lf.lfHeight = (int)(cSize.cx);
	//lf.lfWidth = 0;
	lf.lfEscapement =  0;
	lf.lfOrientation =  0;
	lf.lfWeight = FW_REGULAR;
	lf.lfCharSet = m_lfCharSet;  //한글 지원
	lf.lfStrikeOut = m_lfStrikeOut;
	CFont font;
	font.CreateFontIndirect(&lf);

	CFont *def_font	= pDC->SelectObject(&font);
	int iOldRop2 = pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = pDC->SetTextColor(m_crFontColor);
	pDC->SetTextAlign(TA_LEFT);

	CGPointGP gpoint;
	CString tmpString;
	int nSize = m_vSHPPoint.size();
	for(int i=0; i < nSize; ++i)
	{
		tmpString = m_DBF.ReadStringAttribute(i, m_nLabelField);
		gpoint.SetPoint(m_vSHPPoint[i].x, m_vSHPPoint[i].y);
		//pDC->TextOut(&gpoint, tmpString);
		pDC->TextOut(gpoint.m_P.lat, gpoint.m_P.lon, tmpString);
	}

	pDC->SetTextColor(oldColor2);
	pDC->SelectObject(def_font);
	font.DeleteObject();
}


int	shpPoint::Track(CDC* pDC, void* pObject, GPoint selpoint, CGRectGP* rectGP)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	int ID = -1;
	double lx, ly, dx, dy, ex, ey;
	CGPointGP gpLT, gpRB;
	CGRectGP gpRect;
	CPoint point;
	CGPointGP gpoint, selgpoint;
	CPoint point1, point2;
	double len;

	CPointPriority* pPriority = (CPointPriority*)pObject;
	len = pPriority->m_uiRadius;
	selgpoint.SetPoint(selpoint.x, selpoint.y);
	int nSize = m_vSHPPoint.size();

	for(int i=0; i<nSize; ++i)
	{
		gpoint.SetPoint(m_vSHPPoint[i].x, m_vSHPPoint[i].y);
		//pDC->GeoPtoDP(&point, &gpoint);
		m_Coord.degrees_lat_long_to_x_y (gpoint.m_P.lat, gpoint.m_P.lon, &lx, &ly); 
		m_CoordMgr->WorldXY2ScreenXY(lx, ly, &point.x, &point.y);

		point1.x = point.x - len;
		point1.y = point.y - len;
		point2.x = point.x + len;
		point2.y = point.y + len;

		//pDC->DPtoGeoP(&gpLT, &point1);
		m_CoordMgr->ScreenXY2WorldXY(point1.x, point1.y, &dx, &dy);
		m_Coord.x_y_to_degrees_lat_long(dx, dy, &gpLT.m_P.lat, &gpLT.m_P.lon);

		//pDC->DPtoGeoP(&gpRB, &point2);
		m_CoordMgr->ScreenXY2WorldXY(point2.x, point2.y, &ex, &ey);
		m_Coord.x_y_to_degrees_lat_long(ex, ey, &gpRB.m_P.lat, &gpRB.m_P.lon);

		gpRect.SetRect(gpLT, gpRB);

		if(gpRect.PtInRect(selgpoint) == TRUE)
		{
			return i;
		}
	}

	return ID;
}


void shpPoint::DrawMemoryMap(CDC* pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio)
{
	if(m_hFMap == NULL || m_hFMapShx == NULL)
		return;

	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	m_lfZoomFactor = _ZoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rect.Width() / 2);
	m_scrCenterPoint.y = int(_rect.Height() / 2);
	m_lfRatio = _lfRatio;

	// DC saving..
	int savedDC = pDC->SaveDC();

	double lx, ly, dx, dy, ex, ey;
	CGPointGP gpLT, gpRB;
	CGRectGP gpRect;
	CPoint point;
	CGPointGP gpoint;
	CPoint point1, point2;
	int iTmp, iSeek, iSeek2;
	iSeek = iSeek2 = 0;
	GPoint tmpPoint;
	CRect rc;

	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));

	int nFontSize;
	nFontSize = m_iFontSize;
	CSize cSize(nFontSize, nFontSize);
	pDC->DPtoLP(&cSize);

	lf.lfHeight = (int)(cSize.cx);
	lf.lfEscapement =  0;
	lf.lfOrientation =  0;
	lf.lfStrikeOut = m_lfStrikeOut;
	lf.lfWeight = FW_REGULAR;
	lf.lfCharSet = m_lfCharSet;  //한글 지원
	lstrcpynA(lf.lfFaceName, T2A((LPTSTR)(LPCTSTR)m_strFaceName), LF_FACESIZE);

	CFont font;
	font.CreateFontIndirect(&lf);
	CFont *def_font	= pDC->SelectObject(&font);
	int iOldRop2 = pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = pDC->SetTextColor(m_crFontColor);
	pDC->SetTextAlign(TA_LEFT);

	CString tmpString, strName, strByte;
	BYTE byte[2];
	int iByte;
	BOOL bRes = FALSE;

	unsigned char *pData = (unsigned char *)MapViewOfFile(m_hFMap, FILE_MAP_READ, 0, 0, 0);
	unsigned char *pDataShx = (unsigned char *)MapViewOfFile(m_hFMapShx, FILE_MAP_READ, 0, 0, 0);

	for(int i=0; i<m_nRecords; i++)
	{
		iSeek = 100 + (i * 8);
		memcpy(&iTmp, pDataShx+iSeek, 4);
		SwapWord(4, &iTmp);
		iTmp = iTmp * 2;

		iSeek2 = iTmp + 12;
		memcpy(&tmpPoint.x, pData+iSeek2, 8);
		iSeek2 = iSeek2 + 8;
		memcpy(&tmpPoint.y, pData+iSeek2, 8);

		gpoint.SetPoint(tmpPoint.x, tmpPoint.y);
		if(!_rect.PtInRect(gpoint))
			continue;

		//pDC->GeoPtoDP(&point, &gpoint);
		m_Coord.degrees_lat_long_to_x_y (gpoint.m_P.lat, gpoint.m_P.lon, &lx, &ly); 
		m_CoordMgr->WorldXY2ScreenXY(lx, ly, &point.x, &point.y);

		point1.x = point.x - m_dPointLen;
		point1.y = point.y - m_dPointLen;
		point2.x = point.x + m_dPointLen;
		point2.y = point.y + m_dPointLen;

		//pDC->DPtoGeoP(&gpLT, &point1);
		m_CoordMgr->ScreenXY2WorldXY(point1.x, point1.y, &dx, &dy);
		m_Coord.x_y_to_degrees_lat_long(dx, dy, &gpLT.m_P.lat, &gpLT.m_P.lon);

		//_pDC->DPtoGeoP(&gpRB, &point2);
		m_CoordMgr->ScreenXY2WorldXY(point2.x, point2.y, &ex, &ey);
		m_Coord.x_y_to_degrees_lat_long(ex, ey, &gpRB.m_P.lat, &gpRB.m_P.lon);

		gpRect.SetRect(gpLT, gpRB);

		switch(m_nPointType)
		{
			rc.left = gpRect.m_R.left;
			rc.top = gpRect.m_R.top;
			rc.right = gpRect.m_R.right;
			rc.bottom = gpRect.m_R.bottom;

			case 0 : 
				pDC->Ellipse(&rc);
				break;
			case 1 : 
				pDC->Rectangle(&rc);
				break;
			default : continue;
		}

		// label draw
		if(m_bLabel)
		{
			bRes = TRUE;
			tmpString = m_DBF.ReadStringAttribute(i, m_nLabelField);
			strName = tmpString.Mid(0,2);
			memset(byte, ' ', 2);
			memcpy(byte, (LPCTSTR)strName, strName.GetLength());
			strByte.Format("%d", byte[0]);
			iByte = atoi(strByte);

			switch(m_iVisibleType)
			{
				// 한글만 출력
				case 0 : 
					if(iByte >=0 && iByte <= 127) bRes = FALSE;
					break;
				// 영문만 출력
				case 1 : 
					if(iByte > 127) bRes = FALSE;
					break;
				// 한글+영문 출력
				case 2 :
					break;
				default :
					break;
			}

			if(bRes)
				pDC->TextOut(gpoint.m_P.lat, gpoint.m_P.lon, tmpString);
				//pDC->TextOut(&gpoint, tmpString);
//			DrawLabelMemoryMap(_pDC, &gpoint, i);
		}
	}

	// DC restoring..
	pDC->RestoreDC(savedDC);

	pDC->SetTextColor(oldColor2);
	pDC->SelectObject(def_font);
	font.DeleteObject();

	if(pData)
		UnmapViewOfFile(pData);
	if(pDataShx)
		UnmapViewOfFile(pDataShx);
}

void shpPoint::DrawLabelMemoryMap(CDC* pDC, CGPointGeo *pPoint, int i)
{
	CString tmpString = m_DBF.ReadStringAttribute(i, m_nLabelField);
	CString strName = tmpString.Mid(0,2);
	BYTE byte[2];
	memset(byte, ' ', 2);
	memcpy(byte, (LPCTSTR)strName, strName.GetLength());

	CString strByte;
	strByte.Format("%d", byte[0]);
	int iByte = atoi(strByte);

	switch(m_iVisibleType)
	{
		// 한글만 출력
		case 0 : 
			if(iByte >=0 && iByte <= 127) return;
			break;
		// 영문만 출력
		case 1 : 
			if(iByte > 127) return;
			break;
		// 한글+영문 출력
		case 2 :
			break;
		default :
			break;
	}

	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));

	int nFontSize;
	nFontSize = m_iFontSize;
	CSize cSize(nFontSize, nFontSize);
	pDC->DPtoLP(&cSize);

	lf.lfHeight = (int)(cSize.cx);
	//lf.lfWidth = 0;
	lf.lfEscapement =  0;
	lf.lfOrientation =  0;
	lf.lfStrikeOut = m_lfStrikeOut;
	lf.lfWeight = FW_REGULAR;
	lf.lfCharSet = m_lfCharSet;
	lstrcpynA(lf.lfFaceName, T2A((LPTSTR)(LPCTSTR)m_strFaceName), LF_FACESIZE);

	CFont font;
	font.CreateFontIndirect(&lf);

	CFont *def_font	= pDC->SelectObject(&font);
	int iOldRop2 = pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = pDC->SetTextColor(m_crFontColor);
	pDC->SetTextAlign(TA_LEFT);

	//  수정필요.  YOUNG
	//pDC->TextOut(pPoint, tmpString);

	pDC->SetTextColor(oldColor2);
	pDC->SelectObject(def_font);
	font.DeleteObject();
}
