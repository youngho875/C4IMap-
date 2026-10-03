// BAMgr.h: interface for the CBAMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAMGR_H__47D085A7_7A7D_4BA7_B975_6D20817278A8__INCLUDED_)
#define AFX_BAMGR_H__47D085A7_7A7D_4BA7_B975_6D20817278A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxctl.h>
#include "CIData.h"

class CGDCGeo;

/**
* 군배부호 데이터 속성 클래스 
* @author 김종득
* @version 1.0
* @since 2005. 7.
**/
class CBA
{
public:
	CString			sTypeCI;	//CI식별문자
	CString			beNumber;	//BE번호
	CString			taNumber;	//접미번호
	double			coordX;		//경도
	double			coordY;		//위도
	CString			symType;	//심볼타입
	CString			visible;	//도시여부
	CString			targetName;	//표적명칭
	CString			dispType;	//명칭도시타입
	CString			order;		//색상(우선순위)
	CString			minArea;	//최소 축척
	CString			maxArea;	//최대 축척
	int				dispMapType;
};

/**
* 군대부호 데이터 API 입력관리 클래스 
* @author 김종득
* @version 1.0
* @since 2005. 7.
**/
class CBAMgr  
{
private:

public:

public:
	CBAMgr();
	virtual ~CBAMgr();

	static CString  DegreeToDmsLon(double lon);
	static CString  DegreeToDmsLat(double lat);
	
	static CString DegreeToDmsLatEx(double deg);
	static CString DegreeToDmsLonEx(double deg);
	
 
	static void		SetMemDC		(CGDCGeo* dc);
	static CGDCGeo* GetMemDC		();
	static int		GetDispMapType	();
	static void		SetDispMapType	(int mapType);

	static BOOL		GetSelect		();
	static void		SelectObject	(BOOL bFlag);
	static COleControl* GetOleControl();
	static void		SetOleControl	(COleControl* pOleControl);
	static BOOL		SetZoomInRect	(CString leftTop, CString rightBottom);
	static CIRect	GetZoomInRect	();
	static CCoord_	SetCoord		(CString coord);
	static CString	DegreeToDms		(double deg);
	static void		ClearAll		();

	static CString		GetCoord	(CPoint pt_device );
 
	static BOOL		SetZoomInRect(double dleft, double dtop, double dright, double dbottom);
	
 
private:
};


#endif // !defined(AFX_BAMGR_H__47D085A7_7A7D_4BA7_B975_6D20817278A8__INCLUDED_)
