// shpPolygon.cpp: implementation of the shpPolygon class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "shpPolygon.h"
//#include "sgiscomlib/GLib.h"
#include "GeoDC/Coord.h"
//#include "LoadProgress.h"

#include "MainFrm.h"
#include "C4IMapView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#include <crtdbg.h>
#ifdef malloc
#undef malloc
#endif
#define malloc(s) (_malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
#ifdef calloc
#undef calloc
#endif
#define calloc(c, s) (_calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
#ifdef realloc
#undef realloc
#endif
#define realloc(p, s) (_realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
#ifdef _expand
#undef _expand
#endif
#define _expand(p, s) (_expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__  ))
#ifdef free
#undef free
#endif
#define free(p) (_free_dbg(p, _NORMAL_BLOCK))
#ifdef _msize
#undef _msize
#endif
#define _msize(p) (_msize_dbg(p, _NORMAL_BLOCK))
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

shpPolygon::shpPolygon(CWnd *pParentWnd)
{
	m_pMainWnd = pParentWnd;
	m_iTotCnt = m_iReadCnt = 0;
	m_hFMap = m_hFMapShx = NULL;
}

shpPolygon::~shpPolygon()
{
	for(int i=0; i<m_paSHPPoly.GetSize(); i++)
	{
		free(m_paSHPPoly[i]->m_iParts);
		delete []m_paSHPPoly[i]->m_gpPoints;
		delete m_paSHPPoly[i];
	}
	m_paSHPPoly.RemoveAll();
}

bool shpPolygon::Open(CString _filename)
{
	if(!shpObject::Open(_filename))
		return false;

	CC4IMapView* pView = (CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());

	CRect rt;
	pView->GetClientRect(rt);

	// Temporary object..
	GPoint tmpPoint;
	int i, j, tmp;

	// File pointer..
	FILE* fpSHP = fopen(m_strSHPPath, "rb");
	FILE* fpSHX = fopen(m_strSHXPath, "rb");

	CRect dlgRt;
	CString state;

	int nMaxPointNo = 0;
	int nMaxPartsNo = 0;
	//float fPercent = 0.0;
	//m_iTotCnt = m_nRecords;

	CString strText;
	//if(m_pProgressDlg != NULL)
	//	m_pProgressDlg->ProgressStart(_filename, 1000);	// young 삭제
	//int iCnt = m_nRecords / 100;
	//int iTotCnt = iCnt;
	polyObject *SHPPoly;

	for(i=0; i<m_nRecords; ++i)
	{
		SHPPoly = new polyObject;
		fseek(fpSHX, 100 + i * 8, SEEK_SET);
		fread(&tmp, 4, 1, fpSHX);
		SwapWord(4, &tmp);
		tmp=tmp*2;

		fseek(fpSHP, tmp+12, SEEK_SET);
		fread((MBR*)&SHPPoly->m_MBR, 32, 1, fpSHP);			//get MBR
		fread((int*)&SHPPoly->m_nNumParts, 4, 1, fpSHP);	//get parts number
		fread((int*)&SHPPoly->m_nNumPoints, 4, 1, fpSHP);	//get points number

		if(SHPPoly->m_nNumParts <= 0 || SHPPoly->m_nNumPoints <= 0)
		{
			delete SHPPoly;
			continue;
		}

		// part reading
		SHPPoly->m_iParts = (int*)malloc(sizeof(int) * SHPPoly->m_nNumParts);
		for(j=0; j<SHPPoly->m_nNumParts; ++j)
		{
			fread((int*)&tmp, 4, 1, fpSHP);
			SHPPoly->m_iParts[j] = tmp;
		}

		// point reading..
		SHPPoly->m_gpPoints = new CGPointGP[SHPPoly->m_nNumPoints];
		for(j=0; j<SHPPoly->m_nNumPoints; ++j)
		{
			fread((double*)&tmpPoint.x, 8, 1, fpSHP);
			fread((double*)&tmpPoint.y, 8, 1, fpSHP);
			SHPPoly->m_gpPoints[j].SetPoint(tmpPoint.x, tmpPoint.y);
		}
		SHPPoly->m_nID = i;
		m_paSHPPoly.Add(SHPPoly);

		//fPercent = (float)((float)((i+1)*100.0)/(float)m_nRecords);
		m_iReadCnt++;
		//if(m_pProgressDlg != NULL && iTotCnt <= m_iReadCnt)	// young 삭제
		//{
		//	strText = "";
		//	strText.Format("%3.1f", fPercent);
		//	strText += "% " + _filename;
		//	m_pProgressDlg->ProgressSetIt(strText);
		//}
		//if(m_iReadCnt >= iTotCnt)
		//	iTotCnt = iTotCnt + iCnt;
	}

	fclose(fpSHP);
	fclose(fpSHX);

	//if(m_pProgressDlg != NULL)
	//	m_pProgressDlg->ProgressEnd();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		 
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void shpPolygon::Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, 
						const CGRectGP& _rect, double _lfRatio)
{
	CPen	pen;
	CBrush	brush;

	m_lfZoomFactor = _zoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rect.Width() / 2);
	m_scrCenterPoint.y = int(_rect.Height() / 2);
	m_lfRatio = _lfRatio;

	if(m_bSolidPolygon)
		DrawPolygon(pDC, _rect);
	else
		DrawPolyLine(pDC, _rect);

	// 선택되어 있으면 그린다..
//	if(m_eStatus == stateSelect)
//		DrawSelected(_pDC, m_iID);

	if(m_bLabel)
		DrawLabel(pDC);
}

void shpPolygon::draw_polyline(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio)
{
	CPen	pen;
	CBrush	brush;

	m_lfZoomFactor = _zoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rect.Width() / 2);
	m_scrCenterPoint.y = int(_rect.Height() / 2);
	m_lfRatio = _lfRatio;

 
	DrawPolyLine(pDC, _rect);


	if(m_bLabel)
		DrawLabel(pDC);
}

void shpPolygon::Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio,
					double dScreenScale, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2)
{
}

void shpPolygon::Draw(CDC* pDC, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
						double dScreenScale, int iScale, CString strName, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2)
{
}

void shpPolygon::DrawPolygon(CDC* pDC, CGRectGP _rect)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	double lx, ly;
	CPen pen;
	CBrush brush;
	CRect mbrRect, rectBuffer;

	int* pParts = NULL;

	_rect.m_R.left -= 0.01;
	_rect.m_R.top += 0.01;
	_rect.m_R.right += 0.01;
	_rect.m_R.bottom -= 0.01;

	CPoint *pPoints;
	for(int i=0; i<m_paSHPPoly.GetSize(); ++i)
	{
		if( _rect.m_R.left > m_paSHPPoly[i]->m_MBR.xmax ||
			_rect.m_R.right < m_paSHPPoly[i]->m_MBR.xmin ||
			_rect.m_R.top < m_paSHPPoly[i]->m_MBR.ymin ||
			_rect.m_R.bottom > m_paSHPPoly[i]->m_MBR.ymax )
			continue;


		pParts = new int[m_paSHPPoly[i]->m_nNumParts];
		for(int j=0; j<m_paSHPPoly[i]->m_nNumParts; ++j)
		{
			if(j == m_paSHPPoly[i]->m_nNumParts - 1)
				pParts[j] = m_paSHPPoly[i]->m_nNumPoints - m_paSHPPoly[i]->m_iParts[j];
			else
				pParts[j] = m_paSHPPoly[i]->m_iParts[j+1] - m_paSHPPoly[i]->m_iParts[j];
		}
		
		pPoints = new CPoint[m_paSHPPoly[i]->m_nNumPoints];
		for(int k=0; k<m_paSHPPoly[i]->m_nNumPoints; ++k)
		{
			m_Coord.degrees_lat_long_to_x_y (m_paSHPPoly[i]->m_gpPoints[k].m_P.lat, m_paSHPPoly[i]->m_gpPoints[k].m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints[k].x, &pPoints[k].y);
		}
		if(m_paSHPPoly[i]->m_nNumParts > 0)
		{
			//pDC->PolyPolygon((CPoint*)m_paSHPPoly[i]->m_gpPoints, pParts, m_paSHPPoly[i]->m_nNumParts);
			pDC->PolyPolygon(pPoints, pParts, m_paSHPPoly[i]->m_nNumParts);
		}		
		delete []pParts;
		delete []pPoints;
	}
}

void shpPolygon::DrawPolyLine(CDC* pDC, CGRectGP _rect)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	MBR m_MBR;
 	m_MBR.xmax = m_MBR.ymax = -99999999.;
	m_MBR.xmin = m_MBR.ymin = +99999999.;

	int savedDC = pDC->SaveDC();
	CGPointGP *gp;
	CGPointGP *pGP;
	CGPointGP pGeo;
	CGPointGP sGP;
	int nPolyCount;
	double lx, ly;
	CPoint *pPoints;

	for(int i=0; i<m_paSHPPoly.GetSize(); ++i)
	{
		if(_rect.m_R.left > m_paSHPPoly[i]->m_MBR.xmax) continue;
		if(_rect.m_R.right < m_paSHPPoly[i]->m_MBR.xmin) continue;
		if(_rect.m_R.top < m_paSHPPoly[i]->m_MBR.ymin) continue;
		if(_rect.m_R.bottom > m_paSHPPoly[i]->m_MBR.ymax) continue;

		gp = new CGPointGP[m_paSHPPoly[i]->m_nNumPoints];
		pGP = new CGPointGP[m_paSHPPoly[i]->m_nNumPoints];
		pPoints = new CPoint[m_paSHPPoly[i]->m_nNumPoints];

		nPolyCount = 0;
		for(int j=0; j<m_paSHPPoly[i]->m_nNumPoints; ++j)
		{
			pGeo.SetPoint(&m_paSHPPoly[i]->m_gpPoints[j]);

			// 변환		young
			m_Coord.degrees_lat_long_to_x_y (m_paSHPPoly[i]->m_gpPoints[j].m_P.lat, m_paSHPPoly[i]->m_gpPoints[j].m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints[j].x, &pPoints[j].y);

			if(!nPolyCount)
			{
				sGP.SetPoint(&m_paSHPPoly[i]->m_gpPoints[j]);
				m_MBR.xmax = m_MBR.ymax = -99999999.;
				m_MBR.xmin = m_MBR.ymin = +99999999.;
			}

			m_MBR.xmax = (m_MBR.xmax < pGeo.m_P.lon) ? pGeo.m_P.lon : m_MBR.xmax; 
			m_MBR.xmin = (m_MBR.xmin > pGeo.m_P.lon) ? pGeo.m_P.lon : m_MBR.xmin;
			m_MBR.ymax = (m_MBR.ymax < pGeo.m_P.lat) ? pGeo.m_P.lat : m_MBR.ymax; 
			m_MBR.ymin = (m_MBR.ymin > pGeo.m_P.lat) ? pGeo.m_P.lat : m_MBR.ymin;

			gp[j] = pGeo;
			pGP[nPolyCount] = pGeo;
			
			if(nPolyCount > 0 && sGP == gp[j])
			{
				if(_rect.m_R.left > m_MBR.xmax)	
				{
					delete []gp;
					delete []pGP;
					nPolyCount = 0;
					continue;
				}
				if(_rect.m_R.right < m_MBR.xmin)		
				{
					delete []gp;
					delete []pGP;
					nPolyCount = 0;
					continue;
				}
				if(_rect.m_R.top < m_MBR.ymin)		
				{
					nPolyCount = 0;
					continue;
				}
				if(_rect.m_R.bottom > m_MBR.ymax)		
				{
					delete []gp;
					delete []pGP;
					nPolyCount = 0;
					continue;
				}

				if(nPolyCount > 0)
				{
					//pDC->Polyline((POINT*)pGP, nPolyCount);
					pDC->Polyline(pPoints, nPolyCount);
				}
				nPolyCount = 0;																					
			}
			else													
			{
				nPolyCount++;
			}
		}

		delete []gp;
		delete []pGP;
		delete []pPoints;
	}
	pDC->RestoreDC(savedDC);
}

void shpPolygon::DrawSelected(CDC* pDC, int iID)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	double lx, ly;
	CPoint *pPoints;
	int* pParts = NULL;


	for(int i=0; i<m_paSHPPoly.GetSize(); ++i)
	{
		if(m_paSHPPoly[i]->m_nID != iID) continue;

		pParts = new int[m_paSHPPoly[i]->m_nNumParts];
		for(int j=0; j<m_paSHPPoly[i]->m_nNumParts; ++j)
		{
			if(j == m_paSHPPoly[i]->m_nNumParts - 1)
				pParts[j] = m_paSHPPoly[i]->m_nNumPoints - m_paSHPPoly[i]->m_iParts[j];
			else
				pParts[j] = m_paSHPPoly[i]->m_iParts[j+1] - m_paSHPPoly[i]->m_iParts[j];
		}
		
		pPoints = new CPoint[m_paSHPPoly[i]->m_nNumPoints];
		for(int k=0; k<m_paSHPPoly[i]->m_nNumPoints; ++k)
		{
			m_Coord.degrees_lat_long_to_x_y (m_paSHPPoly[i]->m_gpPoints[k].m_P.lat, m_paSHPPoly[i]->m_gpPoints[k].m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints[k].x, &pPoints[k].y);
		}
		//pDC->PolyPolygon((POINT*)m_paSHPPoly[i]->m_gpPoints, pParts, m_paSHPPoly[i]->m_nNumParts);
		pDC->PolyPolygon(pPoints, pParts, m_paSHPPoly[i]->m_nNumParts);

		delete []pParts;
		delete []pPoints;
	}
}

void shpPolygon::DrawLabel(CDC* pDC)
{
	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));

	int nFontSize;
	nFontSize = 14;
	CSize cSize(nFontSize, nFontSize);
	pDC->DPtoLP(&cSize);

	lf.lfHeight = (int)(cSize.cx);
	//lf.lfWidth = 0;
	lf.lfEscapement =  0;
	lf.lfOrientation =  0;
	lf.lfWeight = FW_REGULAR;
	lf.lfCharSet = ANSI_CHARSET;  //한글 지원
	CFont font;
	font.CreateFontIndirect(&lf);

	CFont *def_font	= pDC->SelectObject(&font);
	int iOldRop2 = pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = pDC->SetTextColor(RGB(0,0,0));
	pDC->SetTextAlign(TA_LEFT);

	CGPointGP gpoint;
	CString tmpString;
	int nSize = m_paSHPPoly.GetSize();
	for(int i=0; i<nSize; ++i)
	{
		tmpString = m_DBF.ReadStringAttribute(i, m_nLabelField);
		pDC->TextOut(m_paSHPPoly[i]->m_gpPoints[0].m_P.lon, m_paSHPPoly[i]->m_gpPoints[0].m_P.lat, tmpString);
	}

	pDC->SetTextColor(oldColor2);
	pDC->SelectObject(def_font);
	font.DeleteObject();
}


void shpPolygon::DrawMemoryMap(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio)
{
	if(m_hFMap == NULL || m_hFMapShx == NULL)
		return;

	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	m_lfZoomFactor = _zoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rect.Width() / 2);
	m_scrCenterPoint.y = int(_rect.Height() / 2);
	m_lfRatio = _lfRatio;

	double lx, ly;
	CPoint *pPoints;

	CPen pen;
	CBrush brush;
	CRect mbrRect, rectBuffer;
	int iTmp, iSeek, iSeek2;
	iSeek = iSeek2 = 0;
	GPoint tmpPoint;
	int* pParts = NULL;

	CGRectGP rect = _rect;

	rect.m_R.left -= 0.01;
	rect.m_R.top += 0.01;
	rect.m_R.right += 0.01;
	rect.m_R.bottom -= 0.01;

	unsigned char *pData = (unsigned char *)MapViewOfFile(m_hFMap, FILE_MAP_READ, 0, 0, 0);
	unsigned char *pDataShx = (unsigned char *)MapViewOfFile(m_hFMapShx, FILE_MAP_READ, 0, 0, 0);

	polyObject SHPPoly;

	for( int i=0; i < m_nRecords; ++i )
	{
		iSeek = 100 + (i * 8);
		memcpy(&iTmp, pDataShx+iSeek, 4);
		SwapWord(4, &iTmp);
		iTmp = iTmp * 2;

		iSeek2 = iTmp + 12;
		memcpy(&SHPPoly.m_MBR, pData+iSeek2, 32);

		if( rect.m_R.left > SHPPoly.m_MBR.xmax ||
			rect.m_R.right < SHPPoly.m_MBR.xmin ||
			rect.m_R.top < SHPPoly.m_MBR.ymin ||
			rect.m_R.bottom > SHPPoly.m_MBR.ymax )
			continue;

		memcpy(&SHPPoly.m_nNumParts, pData+iSeek2+32, 4);
		memcpy(&SHPPoly.m_nNumPoints, pData+iSeek2+32+4, 4);

		if(SHPPoly.m_nNumParts <= 0 || SHPPoly.m_nNumPoints <= 0)
			continue;

		iSeek = iSeek2+32+4+4;

		SHPPoly.m_iParts = new int[SHPPoly.m_nNumParts];
		// part reading
		for(int j=0; j<SHPPoly.m_nNumParts; ++j)
		{
			memcpy((int*)&iTmp, pData+iSeek, 4);
			SHPPoly.m_iParts[j] = iTmp;
			iSeek += 4;
		}

		// point reading..
		SHPPoly.m_gpPoints = new CGPointGP[SHPPoly.m_nNumPoints];
		for(int j=0; j<SHPPoly.m_nNumPoints; ++j)
		{
			memcpy(&tmpPoint.x, pData+iSeek, 8);
			iSeek += 8;
			memcpy(&tmpPoint.y, pData+iSeek, 8);
			iSeek += 8;
			SHPPoly.m_gpPoints[j].SetPoint(tmpPoint.x, tmpPoint.y);
		}
		SHPPoly.m_nID = i;

		//CPoint *pPoint = new CPoint[SHPPoly.m_nNumParts];
		pParts = new int[SHPPoly.m_nNumParts];
		for(int j=0; j<SHPPoly.m_nNumParts; ++j)
		{
			if(j == SHPPoly.m_nNumParts - 1)
				pParts[j] = SHPPoly.m_nNumPoints - SHPPoly.m_iParts[j];
			else
				pParts[j] = SHPPoly.m_iParts[j+1] - SHPPoly.m_iParts[j];
		}

		pPoints = new CPoint[SHPPoly.m_nNumPoints];
		for(int k=0; k<SHPPoly.m_nNumPoints; ++k)
		{
			m_Coord.degrees_lat_long_to_x_y (SHPPoly.m_gpPoints[k].m_P.lat, SHPPoly.m_gpPoints[k].m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints[k].x, &pPoints[k].y);
		}

		//pDC->PolyPolygon((POINT*)SHPPoly.m_gpPoints, pParts, SHPPoly.m_nNumParts);
		pDC->PolyPolygon(pPoints, pParts, SHPPoly.m_nNumParts);
		

		delete []pParts;
		delete []SHPPoly.m_iParts;
		delete []SHPPoly.m_gpPoints;
		delete []pPoints;
	}

//	if(m_bLabel)
//		DrawLabel(_pDC);
	if(pData)
		UnmapViewOfFile(pData);
	if(pDataShx)
		UnmapViewOfFile(pDataShx);
}

int	shpPolygon::Track(CDC* pDC, void* pObject, GPoint selpoint, CGRectGP* rectGP)
{
	int ID = -1;
	GPoint gpoint;
	int* pParts;

	// 면 위를 본다.
	for(int i=0; i<m_paSHPPoly.GetSize(); ++i)
	{
		// 화면 밖이면 처리하지 않는다.
		if(rectGP->m_R.left > m_paSHPPoly[i]->m_MBR.xmax ||
			rectGP->m_R.right < m_paSHPPoly[i]->m_MBR.xmin ||
			rectGP->m_R.top < m_paSHPPoly[i]->m_MBR.ymin ||
			rectGP->m_R.bottom > m_paSHPPoly[i]->m_MBR.ymax)
			continue;

		// 파트별 포인트의 개수를 계산..
		pParts = (int*)malloc(sizeof(int) * m_paSHPPoly[i]->m_nNumParts);
		for(int j=0; j<m_paSHPPoly[i]->m_nNumParts; ++j)
		{
			if(j == m_paSHPPoly[i]->m_nNumParts - 1)
				pParts[j] = m_paSHPPoly[i]->m_nNumPoints - m_paSHPPoly[i]->m_iParts[j];
			else
				pParts[j] = m_paSHPPoly[i]->m_iParts[j+1] - m_paSHPPoly[i]->m_iParts[j];
		}

		// 파트별로 회전하며 면위를 뒤진다..
		for(int j=0; j<m_paSHPPoly[i]->m_nNumParts; ++j)
		{
			if (PtInPolygon(pParts[j], m_paSHPPoly[i]->m_gpPoints, selpoint))
			{
				free(pParts);
				return m_paSHPPoly[i]->m_nID;
			}
		} // for j
		free(pParts);
	} // for i

	return ID;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double shpPolygon::Area2(GPoint a, GPoint b, GPoint c)
{
	return	(b.x - a.x) * (c.y - a.y) -	(c.x - a.x) * (b.y - a.y);
}

GPoint shpPolygon::FindCG(vector<GPoint>& vPoint)
{
	GPoint	gPoint;
	GPoint	point;
	double  A2, Areasum2 = 0;        /* Partial area sum */    

	for(int i = 1; i < (vPoint.size() - 1); ++i)
	{
		point = Centroid3(vPoint[0], vPoint[i], vPoint[i+1]);
		A2 = Area2(vPoint[0], vPoint[i], vPoint[i+1]);

		gPoint.x += A2 * point.x;
		gPoint.y += A2 * point.y;

		Areasum2 += A2;
	}

	gPoint.x /= 3 * Areasum2;
	gPoint.y /= 3 * Areasum2;

	return gPoint;
}

GPoint shpPolygon::Centroid3(GPoint a, GPoint b, GPoint c)
{
	GPoint result;
	result.x = a.x + b.x + c.x;
	result.y = a.y + b.y + c.y;

	return result;
}


BOOL shpPolygon::GetSelRect(int iSelID, double &left, double &top, double &right, double &bottom)
{
	BOOL bRes = FALSE;
	left = 999;
	bottom = 999;
	right = -1;
	top = -1;

	for(int i=0; i<m_paSHPPoly.GetSize(); ++i)
	{
		if(m_paSHPPoly[i]->m_nID != iSelID) continue;

		for(int j=0; j<m_paSHPPoly[i]->m_nNumPoints; ++j)
		{
			if(m_paSHPPoly[i]->m_gpPoints[j].m_P.lon < left)
				left = m_paSHPPoly[i]->m_gpPoints[j].m_P.lon;
			if(m_paSHPPoly[i]->m_gpPoints[j].m_P.lon > right)
				right = m_paSHPPoly[i]->m_gpPoints[j].m_P.lon;
			if(m_paSHPPoly[i]->m_gpPoints[j].m_P.lat > top)
				top = m_paSHPPoly[i]->m_gpPoints[j].m_P.lat;
			if(m_paSHPPoly[i]->m_gpPoints[j].m_P.lat < bottom)
				bottom = m_paSHPPoly[i]->m_gpPoints[j].m_P.lat;
		}

		bRes = TRUE;
		break;
	}

	return bRes;
}

bool shpPolygon::OpenMemoryMap(CString _filename)
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

