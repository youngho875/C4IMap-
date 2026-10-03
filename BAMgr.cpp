// BAMgr.cpp: implementation of the CBAMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <afxctl.h>
//#include "CIDataMgr.h"
#include "BAMgr.h"
//#include "../ModuleSitu/GHlpModuleSituView.h"
//#include "../../libsrc/smapsrc/sgisres/SituCmds.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CList<CBA, CBA> listBA;
static BOOL bObjectSelect = FALSE;
static int dispMapType = 0;
//-----------------------------
// No1... 
// 상황도에서 사용합니다...
static COleControl* pOleCtrl; 
//-----------------------------
static CGDCGeo *curMemDC;
static CIRect zoomRect;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction	ID_S_EDIT_SITU_NEW
//////////////////////////////////////////////////////////////////////

CBAMgr::CBAMgr()
{
	pOleCtrl = NULL;
	curMemDC = NULL;
	dispMapType = 0;
	bObjectSelect = FALSE;
}

CBAMgr::~CBAMgr()
{
	listBA.RemoveAll();
}

CGDCGeo* CBAMgr::GetMemDC()
{
	return curMemDC;
}

void CBAMgr::SetMemDC(CGDCGeo *dc)
{
	curMemDC = dc;
}

void CBAMgr::SetOleControl(COleControl *pOleControl)
{
	pOleCtrl = pOleControl;
}

COleControl* CBAMgr::GetOleControl()
{
	return pOleCtrl;
}

void CBAMgr::SelectObject(BOOL bFlag)
{
	bObjectSelect = bFlag;

}

BOOL CBAMgr::GetSelect()
{
	return bObjectSelect;
}

void CBAMgr::SetDispMapType(int mapType)
{
	dispMapType = mapType;
}

int CBAMgr::GetDispMapType()
{
	return dispMapType;
}

BOOL CBAMgr::SetZoomInRect(CString leftTop, CString rightBottom)
{
	if(leftTop == "" || rightBottom == "")
	{
		//CGMessenger::SetMessage("유효하지 않은 좌표가 입력되었습니다.[영역확대]", CGMessenger::ToDialog);
		return FALSE;
	}

	CCoord_ LT = SetCoord(leftTop);
	CCoord_ RB = SetCoord(rightBottom);
	zoomRect.left = LT.X;
	zoomRect.top = LT.Y;
	zoomRect.right = RB.X;
	zoomRect.bottom = RB.Y;

	if(zoomRect.left < 0 || zoomRect.left > 180)
		return FALSE;

	if(zoomRect.right < 0 || zoomRect.right > 180)
		return FALSE;

	if(zoomRect.top < 0 || zoomRect.top > 90)
		return FALSE;

	if(zoomRect.bottom < 0 || zoomRect.bottom > 90)
		return FALSE;

	return TRUE;
}

CIRect CBAMgr::GetZoomInRect()
{
	return zoomRect;
}

// 좌표 형식 변환 dms -> degree
CCoord_ CBAMgr::SetCoord(CString coord)
{

	double m_dCoordX;
	double m_dCoordY;

	CCoord_ coordGeo;
	coordGeo.X = 0.0;
	coordGeo.Y = 0.0;

	if( coord == "" ) return coordGeo;


	//CString coord = "385959.999N1275959.999E";
	CString x, y, tempX, tempY, degreeY, degreeX;
	double d, m, s;
	
	int i = coord.Find('N');
	//위도 
	tempY = coord.Left(i);
	y = tempY.Left(2);
	y += ".";
	y += tempY.Right(tempY.GetLength()-2);

	degreeY = tempY.Right(tempY.GetLength()-2);
	d = atof((LPCTSTR)tempY.Left(2));
	m = atof((LPCTSTR)degreeY.Left(2));
	s = atof((LPCTSTR)degreeY.Mid(2,degreeY.GetLength()));

	m_dCoordY = (((s/60)+m)/60)+d;
	
	if(m_dCoordY < 30 || m_dCoordY > 45)
	{
		TRACE("CBAMgr::SetCoord()--CoordY(%f) Out of Range!! \n", m_dCoordY);
	//	return;
	}
	
	//경도
	tempX = coord.Mid(i+1);
	tempX = tempX.Left(tempX.GetLength()-1);
	x = tempX.Left(3);
	x += ".";
	x += tempX.Right(tempX.GetLength()-3);

	degreeX = tempX.Right(tempX.GetLength()-3);
	d = atof((LPCTSTR)tempX.Left(3));
	m = atof((LPCTSTR)degreeX.Left(2));
	s = atof(degreeX.Mid(2, degreeX.GetLength()));
	m_dCoordX = (((s/60)+m)/60)+d;

	if(m_dCoordX < 100 || m_dCoordX > 150)
	{
		TRACE("CBAMgr::SetCoord()--CoordX(%f) Out of Range!! \n", m_dCoordX);
	//	return FALSE;
	}

	
	coordGeo.X = m_dCoordX;
	coordGeo.Y = m_dCoordY;
	
	return coordGeo;
}

CString CBAMgr::DegreeToDms(double deg)
{
	int d,m;
	double s;
	d = (int)deg;					//도
	m = (int)((deg-d)*60);			//분
	s = (((deg-d)*60)-m)*60;		//초

	CString dms;

	dms.Format("%d%02d%6.3f",d,m,s);						//	channy	수정
	if(dms.GetLength() == 11)  //위도이면
		dms+="E";
	else
		dms+="N";
	return dms;
}

//-------------------------------------------------------------------------------------------------
//jdkim	2005-11-17	0 ~ 90도
CString CBAMgr::DegreeToDmsLat(double deg)
{
//	int d,m,sd;
//	double s,sf;
	int d,m;
	double s;
	d = (int)deg;					//도
	m = (int)((deg-d)*60);			//분
	s = (((deg-d)*60)-m)*60;		//초

	CString dms;
	
	if(s > 10)
		dms.Format("%d%02d%.3f",d,m,s);					
	else
		dms.Format("%d%02d0%.3f",d,m,s);					
	dms+="N";

	return dms;
}

//jdkim	2005-11-17	0 ~ 180도
CString CBAMgr::DegreeToDmsLon(double deg)
{
	int d,m;
	double s;
	d = (int)deg;					//도
	m = (int)((deg-d)*60);			//분
	s = (((deg-d)*60)-m)*60;		//초

	CString dms;

	if(s > 10)
		dms.Format("%d%02d%.3f",d,m,s);					
	else
		dms.Format("%d%02d0%.3f",d,m,s);					

	
	dms+="E";
	return dms;
}

//jdkim	2005-11-17	0 ~ 90도
CString CBAMgr::DegreeToDmsLatEx(double deg)
{
//	int d,m,sd;
//	double s,sf;
	int d,m;
	double s;
	d = (int)deg;					//도
	m = (int)((deg-d)*60);			//분
	s = (((deg-d)*60)-m)*60;		//초

	CString dms;
	
	if(s > 10)
		dms.Format("%d%02d%.0f",d,m,s);					
	else
		dms.Format("%d%02d0%.0f",d,m,s);					
	dms+="N";

	return dms;
}

//jdkim	2005-11-17	0 ~ 180도
CString CBAMgr::DegreeToDmsLonEx(double deg)
{
	int d,m;
	double s;
	d = (int)deg;					//도
	m = (int)((deg-d)*60);			//분
	s = (((deg-d)*60)-m)*60;		//초

	CString dms;

	if(s > 10)
		dms.Format("%d%02d%.0f",d,m,s);					
	else
		dms.Format("%d%02d0%.0f",d,m,s);							

	
	dms+="E";
	return dms;
}


void CBAMgr::ClearAll()
{
	listBA.RemoveAll();
}

// jwpark, 2007-01-23
BOOL CBAMgr::SetZoomInRect(double dleft, double dtop, double dright, double dbottom)
{
	zoomRect.left = dleft;
	zoomRect.top = dtop;
	zoomRect.right = dright;
	zoomRect.bottom = dbottom;

	if(zoomRect.left < 0 || zoomRect.left > 180)
		return FALSE;
	if(zoomRect.right < 0 || zoomRect.right > 180)
		return FALSE;
	if(zoomRect.top < 0 || zoomRect.top > 90)
		return FALSE;
	if(zoomRect.bottom < 0 || zoomRect.bottom > 90)
		return FALSE;

	return TRUE;
}
