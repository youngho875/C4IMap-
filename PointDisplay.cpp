// PointDisplay.cpp: implementation of the CPointDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "PointDisplay.h"
#include "C4IMapView.h"
#include "GeoDC/CoordMgr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CPointDisplay::m_nSeqNo = 0;

CPointDisplay::CPointDisplay()
{
	m_nHookPoint = -1;
	m_nSeqNo = 0;
}

CPointDisplay::~CPointDisplay()
{
	Reset();
}

BOOL CPointDisplay::AddPoint(DisplayPoint &point)
{
	CString szPoint;
	if(list.GetCount() < MAX_DISPLAY_POINT){
		list.AddTail(point);
	}
	else
	{
		szPoint.Format("%d",point.id); 
		DeletePoint(szPoint);
		list.AddTail(point);
	}
	return TRUE;
}

BOOL CPointDisplay::DeletePoint(CString  name)
{
	DisplayPoint point;
	POSITION pos;
	//point = list.GetAt(pos);
	for(int i=0; i < list.GetCount();i++){
		pos = list.FindIndex(i);
		point = list.GetAt(pos);
		if(point.strPointName == name) {
			list.RemoveAt(pos);
			return TRUE;
		}
		//point = list.GetNext(pos);
	}
	return FALSE;
}

void CPointDisplay::Reset()
{
	list.RemoveAll();
}
int CPointDisplay::GetCount()
{
	return list.GetCount();
}
int CPointDisplay::GetID()
{
	m_nSeqNo = (m_nSeqNo > 60000) ? 0 : m_nSeqNo;

	m_nSeqNo++;

	return m_nSeqNo;
}
void CPointDisplay::Draw(CDC* pDC)
{
	Gdiplus::Point displayPoint;
	CPoint		ptPoint;
	DisplayPoint point;
	Graphics g(pDC->m_hDC);
	//Point points[MAX_TRACE_POINT];

	FontFamily   fontFamily(L"Batang");
	Gdiplus::Font         font(&fontFamily, 10.0, FontStyleRegular, UnitPoint);
	SolidBrush   solidBrush(Color(255, 255, 192,0));
	SolidBrush   redBrush(Color(255,255,0,0));
	SolidBrush   blackBrush(Color(255,0,0,0));
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	// Center the block of text (top to bottom) in the rectangle.
	//stringFormat.SetLineAlignment(StringAlignmentCenter);
	//g.SetTextRenderingHint(TextRenderingHintAntiAlias);	

	// Create a Pen object.
	Pen greenPen(Color(255, 0, 255, 0), 2);
	Pen redPen(Color(255,255,0,0),1);
	Pen yellowPen(Color(255,255,255,0),2);
	CCoordMgr*	CoordMgr = GP_COORDMGR;

	POSITION pos = list.GetHeadPosition();
	//point = list.GetHead();

	for(int i = 0; i < list.GetCount() ; i++){
		point = list.GetNext(pos);
		CoordMgr->WorldXY2ScreenXY(point.fX,point.fY,&ptPoint.x,&ptPoint.y);
		BSTR comment= point.strPointName.AllocSysString();
		displayPoint = Gdiplus::Point(ptPoint.x,ptPoint.y);
		g.FillEllipse(&blackBrush, displayPoint.X-2,displayPoint.Y-1,8,8);
		g.FillEllipse(&redBrush, displayPoint.X-4,displayPoint.Y-3,8,8);
		
		if(point.id == m_nHookPoint)
			g.DrawRectangle(&greenPen,displayPoint.X-8,displayPoint.Y-6,16,16);
		g.DrawString(comment,-1,&font,PointF(displayPoint.X,displayPoint.Y+14),&stringFormat,&solidBrush);
		::SysFreeString(comment);
		

	}
		
}


CString CPointDisplay::FindPointDisplay(CString  name)
{
	DisplayPoint point;
	POSITION pos;
	CString m_strLatLong = "";
	//point = list.GetAt(pos);
	for(int i=0; i < list.GetCount();i++){
		pos = list.FindIndex(i);
		point = list.GetAt(pos);
		if(point.strPointName == name) {
		
			CString FindLat = point.strlat;
			CString FindLong = point.strlong;
			
			
			m_strLatLong.Format("%s  %s", FindLat, FindLong); 
		
		}

	}
	return m_strLatLong;
}

void CPointDisplay::ModifyPoint(CString key, DisplayPoint& point)
{
	POSITION pos;
	DisplayPoint p;
	for(int i = 0; i < list.GetCount(); i++) {
		pos = list.FindIndex(i);
		p = list.GetAt(pos);
		if(p.strPointName == key)
		{
			p.fX = point.fX;
			p.fY = point.fY;
			p.strComment = point.strComment;
			p.strlat = point.strlat;
			p.strlong = point.strlong;

			list.SetAt(pos,p);
		}
	} 
}

DisplayPoint CPointDisplay::GetPoint(int id)
{
	POSITION pos;
	pos = list.FindIndex(id);
	return list.GetAt(pos);
}
DisplayPoint CPointDisplay::GetPointByID(int id)
{
	DisplayPoint point;
	DisplayPoint emptyPoint;
	POSITION pos;
	for(int i=0; i < list.GetCount();i++){
		pos = list.FindIndex(i);
		point = list.GetAt(pos);
		if(point.id == id)
			return point;

	}
	
	return emptyPoint;
}
BOOL CPointDisplay::FindPointName(const CString &name)
{
	DisplayPoint point;
	POSITION pos;
	for(int i=0; i < list.GetCount();i++){
		pos = list.FindIndex(i);
		point = list.GetAt(pos);
		if(point.strPointName == name)
			return TRUE;

	}
	return FALSE;
}

// 후킹한 SpecialPoint의 TrackNumber를 반환한다.
int CPointDisplay::ResearchHookPoint(double fXCoord, double fYCoord, double fCenterX, double fCenterY, double fScale)
{
 	CPoint ptPoint;				// Point의 스크린좌표
	CPoint ptHookedPoint;		// 후킹한 스크린좌표
	DisplayPoint point;
	CCoordMgr* pCoordMgr = GP_COORDMGR; 

	pCoordMgr->WorldXY2ScreenXY(fXCoord, fYCoord, &ptHookedPoint.x, &ptHookedPoint.y, fCenterX, fCenterY, fScale);
	
	m_nHookPoint				= -1;
	POSITION pos = list.GetHeadPosition();
	for(int i = 0; i < list.GetCount() ; i++){
		point = list.GetNext(pos);
		pCoordMgr->WorldXY2ScreenXY(point.fX,point.fY,&ptPoint.x,&ptPoint.y);

		// 후킹한 지점과의 거리차를 구한다
		double fDiff=sqrt((double)(ptPoint.x-ptHookedPoint.x)*(double)(ptPoint.x-ptHookedPoint.x)+(double)(ptPoint.y-ptHookedPoint.y)*(double)(ptPoint.y-ptHookedPoint.y)); 

		if(fDiff < 5.0f && fDiff >= 0) {
			m_nHookPoint = point.id; 
			return m_nHookPoint;
		}

	}

	return -1; 
}

int CPointDisplay::ResearchHookPoint(double fXCoord, double fYCoord, double fCenterX, double fCenterY, double fScale,CString &strComment)
{
 	CPoint ptPoint;				// Point의 스크린좌표
	CPoint ptHookedPoint;		// 후킹한 스크린좌표
	DisplayPoint point;
	CCoordMgr* pCoordMgr = GP_COORDMGR; 
    
	pCoordMgr->WorldXY2ScreenXY(fXCoord, fYCoord, &ptHookedPoint.x, &ptHookedPoint.y, fCenterX, fCenterY, fScale);
	
	m_nHookPoint				= -1;
	POSITION pos = list.GetHeadPosition();
	for(int i = 0; i < list.GetCount() ; i++){
		point = list.GetNext(pos);
		pCoordMgr->WorldXY2ScreenXY(point.fX,point.fY,&ptPoint.x,&ptPoint.y);

		// 후킹한 지점과의 거리차를 구한다
		double fDiff=sqrt((double)(ptPoint.x-ptHookedPoint.x)*(double)(ptPoint.x-ptHookedPoint.x)+(double)(ptPoint.y-ptHookedPoint.y)*(double)(ptPoint.y-ptHookedPoint.y)); 

		if(fDiff < 5.0f && fDiff >= 0) {
			m_nHookPoint = point.id;
			strComment = point.strComment;
			return m_nHookPoint;
		}

	}

	return -1; 
}