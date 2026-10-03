#include "pch.h"
#include "GViewInfo.h"



/////////////////////////////////////////////////////////////////////////////
// CGViewInfo Constructor/Destructor

CGViewInfo::CGViewInfo()
{
	//	m_WinOrg = ???;
	//	m_WinExt = ???;
	//	m_selRect = ???;
	//	m_ClientDPRect = ???;
	//	m_nEllipsoid = 1; //???
	//	m_nMapMode = 1;	//???
	//	m_b3D = FALSE;
	//	m_ZoomFactor = 1.0;

		// 3차원 도시 정보
	//	m_fX_Angle = 0.0;
	//	m_fY_Angle = 0.0;
	//	m_fZ_Angle = 0.0;

		// 3차원 도시 형태
	//???	m_bTerrainChk = FALSE;
	//	m_ptGridNum = CPoint(200, 200);
	m_nZoomMsg = 0;
	m_nPrepareDCMsg = 0;
	m_bShift = TRUE;

	m_bAutoScaleMode = TRUE; //자동축척변경
	m_bWrapAround = TRUE; //세계지도 도시관련 Wrap효과..세계지도일 경우에만 TRUE
}


CGViewInfo::~CGViewInfo()
{
}


/////////////////////////////////////////////////////////////////////////////
// CGViewInfo Properties

void CGViewInfo::GetWorldOrigin(double* lon, double* lat)
{
	*lon = m_WinOrg.m_P.lon;
	*lat = m_WinOrg.m_P.lat;
}


void CGViewInfo::SetWorldOrigin(double lon, double lat)
{
	m_WinOrg.SetPoint(lon, lat);
}


void CGViewInfo::GetWorldExt(double* clon, double* clat)
{
	*clon = m_WinExt.m_S.clon;
	*clat = m_WinExt.m_S.clat;
}


void CGViewInfo::SetWorldExt(double clon, double clat)
{
	m_WinExt.SetSize(clon, clat);
}


//CGRectGP CGViewInfo::GetSelectedRange()
//{
//	double left, top, right, bottom;
//	CGRectGP rectGP;
//	CRect rect;
//	GetClientRect(&rect);
//
//	m_pCoordMgr->ScreenXY2WorldXY(rect.left, rect.top, &left, &top);
//	m_pCoordMgr->ScreenXY2WorldXY(rect.right, rect.bottom, &right, &bottom);
//	m_Coord.x_y_to_degrees_lat_long(left, top, &rectGP.m_R.top, &rectGP.m_R.left);
//	m_Coord.x_y_to_degrees_lat_long(right, bottom, &rectGP.m_R.bottom, &rectGP.m_R.right);
//
//	return rectGP;
//}


void CGViewInfo::GetSelectedRange(double* left, double* top, double* right, double* bottom)
{
	*left = m_selRect.m_R.left;
	*top = m_selRect.m_R.top;
	*right = m_selRect.m_R.right;
	*bottom = m_selRect.m_R.bottom;
}


void CGViewInfo::SetSelectedRange(double left, double top, double right, double bottom)
{
	m_selRect.SetRect(left, top, right, bottom);
}


void CGViewInfo::GetViewportRange(long* left, long* top, long* right, long* bottom)
{
	*left = m_ClientDPRect.left;
	*top = m_ClientDPRect.top;
	*right = m_ClientDPRect.right;
	*bottom = m_ClientDPRect.bottom;
}


void CGViewInfo::SetViewportRange(long left, long top, long right, long bottom)
{
	m_ClientDPRect.SetRect(left, top, right, bottom);
}


long CGViewInfo::GetEllipsoid(long* ellip)
{
	*ellip = m_nEllipsoid;
	return *ellip;
}


void CGViewInfo::SetEllipsoid(long ellip)
{
	m_nEllipsoid = ellip;
}


long CGViewInfo::GetMappingMode(long* mapMode)
{
	*mapMode = m_nMapMode;
	return *mapMode;
}


void CGViewInfo::SetMappingMode(long mapMode)
{
	m_nMapMode = mapMode;
}


double CGViewInfo::GetZoomFactor(double* zoom)
{
	*zoom = m_ZoomFactor;

	return *zoom;
}


void CGViewInfo::SetZoomFactor(double zoom)
{
	m_ZoomFactor = zoom;
}


//void CGViewInfo::Set3D(BOOL b3D)
//{
//	
//
//	m_b3D = b3D;
//
//	
//}
//
//
//void CGViewInfo::Is3D(BOOL* b3D)
//{
//	
//
//	*b3D = m_b3D;
//
//	
//}
//



/////////////////////////////////////////////////////////////////////////////
// CGViewInfo Methods


/*
void CGViewInfo::Copy(IGViewInfo* pifSrc)
{
	

	CComPtr<IGViewInfo>	ifSrc(pifSrc);

	HRESULT	hRes;
	double	d1, d2, d3, d4, d5, d6, d7;
	long	l1, l2, l3, l4, l5, l6, l7, l8, l9;
	long	k1, k2, k3;
	BOOL	b1, b2, b3, b4;

	// m_WinOrg
	hRes = ifSrc->GetWorldOrigin(&d1, &d2);
	if (hRes != S_OK) {
		return hRes;
	}
	this->SetWorldOrigin(d1, d2);

	// m_WinExt
	ifSrc->GetWorldExt(&d1, &d2);
	this->SetWorldExt(d1, d2);

	// m_selRect
	ifSrc->GetSelectedRange(&d1, &d2, &d3, &d4);
	this->SetSelectedRange(d1, d2, d3, d4);

	// m_ClientDPRect
	ifSrc->GetViewportRange(&l1, &l2, &l3, &l4);
	this->SetViewportRange(l1, l2, l3, l4);

	// m_nMapMode
	ifSrc->GetMappingMode(&l1);
	this->SetMappingMode(l1);

	// m_nEllipsoid
	ifSrc->GetEllipsoid(&l1);
	this->SetEllipsoid(l1);

	// m_ZoomFactor
	ifSrc->GetZoomFactor(&d1);
	this->SetZoomFactor(d1);

	// m_b3D
	ifSrc->Is3D(&b1);
	this->Set3D(b1);

	// m_fX_Angle, m_fY_Angle, m_fZ_Angle
	ifSrc->GetViewingAngle(&d5, &d6, &d7);
	this->SetViewingAngle(d5, d6, d7);

	// m_ptGridNum
	ifSrc->GetGridToPoint(&k1, &k2);
	this->SetGridToPoint(k1, k2);

	// m_n3DPattern
	ifSrc->Get3DPattern(&k3);
	this->Set3DPattern(k3);

	// m_bShift
	ifSrc->GetShiftFlag(&b2);
	this->SetShiftFlag(b2);

	//m_MapScale
	ifSrc->GetMapScale(&l5);
	this->SetMapScale(l5);
	//

	//m_nEncScale
	ifSrc->GetEncScale(&l6);
	this->SetEncScale(l6);
	//

	//m_nMapKind
	ifSrc->GetMapKind(&l7);
	this->SetMapKind(l7);
	//

	//m_nZoomMsg
	ifSrc->GetZoomInMsg(&l8);
	this->SetZoomInMsg(l8);
	//

	//m_nPrepareDCMsg
	ifSrc->GetPrepareDC(&l9);
	this->SetPrepareDC(l9);
	//

	//m_bAutoScaleMode
	ifSrc->GetAutoScaleMode(&b3);
	this->SetAutoScaleMode(b3);
	//

	//m_bWrapAround
	ifSrc->GetWrapAround(&b4);
	this->SetWrapAround(b4);
	//
}
*/

void CGViewInfo::SetGridToPoint(long x, long y)
{
	m_ptGridNum.x = x;
	m_ptGridNum.y = y;

}

void CGViewInfo::GetGridToPoint(long* x, long* y)
{
	*x = m_ptGridNum.x;
	*y = m_ptGridNum.y;
}

void CGViewInfo::GetViewingAngle(double* fx, double* fy, double* fz)
{
	*fx = m_fX_Angle;
	*fy = m_fY_Angle;
	*fz = m_fZ_Angle;

	
}

void CGViewInfo::SetViewingAngle(double fx, double fy, double fz)
{

	m_fX_Angle = fx;
	m_fY_Angle = fy;
	m_fZ_Angle = fz;

	
}

long CGViewInfo::GetMapScale(long* pVal)
{
	*pVal = m_MapScale;
	
	return *pVal;
}

void CGViewInfo::SetMapScale(long newVal)
{
	m_MapScale = newVal;
}

long CGViewInfo::GetZoomInMsg(long* ZoomIn)
{
	* ZoomIn = m_nZoomMsg;

	return *ZoomIn;
}

void CGViewInfo::SetZoomInMsg(long ZoomIn)
{

	switch (ZoomIn) {
	case 1: m_nZoomMsg = 1; //확대메세지
		break;
	case 2: m_nZoomMsg = 2; //축소메세지
		break;
	case 3: m_nZoomMsg = 3; //영역확대메세지
		break;
	}
}

long CGViewInfo::GetPrepareDC(long* type)
{
	* type = m_nPrepareDCMsg; //영역갱신시 재도시를 위한 메세지를 저장한다.

	return *type;
}

void CGViewInfo::SetPrepareDC(long type)
{

		switch (type) {
		case 1: m_nPrepareDCMsg = 1; //해저지형도
			break;
		case 2: m_nPrepareDCMsg = 2; //세계지도
			break;
		case 3: m_nPrepareDCMsg = 3; //해도
			break;
		case 4: m_nPrepareDCMsg = 4; //래스터, 벡터, 지형분석도등..나머지
			break;
		}
	
}


BOOL CGViewInfo::GetShiftFlag(BOOL* bShift)
{

		*bShift = m_bShift;

	return *bShift;
}

BOOL CGViewInfo::SetShiftFlag(BOOL bShift)
{

		m_bShift = bShift;

	return m_bShift;
}


void CGViewInfo::Serialize(IUnknown* pAr)
{
	

	CArchive* ar = (CArchive*)pAr;
	POINTGP		pntGP;
	SIZEGP		sizeGP;
	RECTGP		rectGP;

	if (ar->IsStoring()) {
		memcpy(&pntGP, &(m_WinOrg.m_P), sizeof(POINTGP));
		memcpy(&sizeGP, &(m_WinExt.m_S), sizeof(SIZEGP));
		memcpy(&rectGP, &(m_selRect.m_R), sizeof(RECTGP));

		ar->Write(&pntGP, sizeof(POINTGP));
		ar->Write(&sizeGP, sizeof(SIZEGP));
		ar->Write(&rectGP, sizeof(RECTGP));

		ar->Write(&m_nMapMode, sizeof(UINT));
		ar->Write(&m_nEllipsoid, sizeof(UINT));
		ar->Write(&m_MapScale, sizeof(long));
		ar->Write(&m_nEncScale, sizeof(long));
		ar->Write(&m_ZoomFactor, sizeof(double));

		ar->Write(&m_nMapKind, sizeof(long));
		ar->Write(&m_bShift, sizeof(BOOL));
		ar->Write(&m_bWrapAround, sizeof(BOOL));

		ar->Write(&m_b3D, sizeof(BOOL));
		ar->Write(&m_fX_Angle, sizeof(double));
		ar->Write(&m_fY_Angle, sizeof(double));
		ar->Write(&m_fZ_Angle, sizeof(double));
		//???		ar->Write(&m_bTerrainChk, sizeof(BOOL));
		ar->Write(&m_ptGridNum, sizeof(POINT));
		ar->Write(&m_n3DPattern, sizeof(long));
		//???		ar->Write(&m_n3DModel, sizeof(UINT));
		if (m_b3D) {
			// m_pMap3d 저장
		}

	}
	else {
		ar->Read(&pntGP, sizeof(POINTGP));
		ar->Read(&sizeGP, sizeof(SIZEGP));
		ar->Read(&rectGP, sizeof(RECTGP));

		m_WinOrg.SetPoint(&pntGP);
		m_WinExt.SetSize(&sizeGP);
		m_selRect.SetRect(&rectGP);

		ar->Read(&m_nMapMode, sizeof(UINT));
		ar->Read(&m_nEllipsoid, sizeof(UINT));
		ar->Read(&m_MapScale, sizeof(long));
		ar->Read(&m_nEncScale, sizeof(long));
		ar->Read(&m_ZoomFactor, sizeof(double));

		ar->Read(&m_nMapKind, sizeof(long));
		ar->Read(&m_bShift, sizeof(BOOL));
		ar->Read(&m_bWrapAround, sizeof(BOOL));

		ar->Read(&m_b3D, sizeof(BOOL));
		ar->Read(&m_fX_Angle, sizeof(double));
		ar->Read(&m_fY_Angle, sizeof(double));
		ar->Read(&m_fZ_Angle, sizeof(double));
		//???		ar->Read(&m_bTerrainChk, sizeof(BOOL));
		ar->Read(&m_ptGridNum, sizeof(POINT));
		ar->Read(&m_n3DPattern, sizeof(long));
		//???		ar->Read(&m_n3DModel, sizeof(UINT));
		if (m_b3D) {
			// m_pMap3d 읽기
		}
	}

	
}


void CGViewInfo::Set3DObject(IUnknown* pVal)
{

		
}

void CGViewInfo::SetWrapAround(BOOL bWrapAround)
{
		m_bWrapAround = bWrapAround;
}

BOOL CGViewInfo::GetWrapAround(BOOL* bWrapAround)
{
	* bWrapAround = m_bWrapAround;

	return *bWrapAround;
}

void CGViewInfo::SetAutoScaleMode(BOOL bAutoScaleMode)
{

	m_bAutoScaleMode = bAutoScaleMode;

	
}

BOOL CGViewInfo::GetAutoScaleMode(BOOL* bAutoScaleMode)
{

	*bAutoScaleMode = m_bAutoScaleMode;

	return *bAutoScaleMode;
}

long CGViewInfo::GetEncScale(long* encScale)
{
	* encScale = m_nEncScale;

	return *encScale;
}

void CGViewInfo::SetEncScale(long encScale)
{
		m_nEncScale = encScale;
}

long CGViewInfo::GetMapKind(long* type)
{
	*type = m_nMapKind;

	return *type;
}

void CGViewInfo::SetMapKind(long type)
{
	m_nMapKind = type;
}

//long CGViewInfo::Get3DPattern(long* type)
//{
//	*type = m_n3DPattern;
//
//	return *type;
//}
//
//void  CGViewInfo::Set3DPattern(long type)
//{
//	m_n3DPattern = type;
//}

/*
void CGViewInfo::IsEqual(IGViewInfo* pData, BOOL* bEqual)
{
	

	HRESULT	hRes;
	double	d1, d2, d3, d4, d5, d6, d7;
	long	l1, l2, l3, l4, l5, l6, l7;
	long	k1, k2, k3;
	BOOL	b1, b2, b4;
	CComPtr<IGViewInfo>	ifSrc = pData;

	*bEqual = FALSE;
	// m_WinOrg
	hRes = ifSrc->GetWorldOrigin(&d1, &d2);
	if (hRes != S_OK) {
		return hRes;
	}
	if (m_WinOrg.m_P.lon != d1 || m_WinOrg.m_P.lat != d2) {
		
	}

	// m_WinExt
	ifSrc->GetWorldExt(&d1, &d2);
	if (m_WinExt.m_S.clon != d1 || m_WinExt.m_S.clat != d2) {
		
	}

	// m_selRect
	ifSrc->GetSelectedRange(&d1, &d2, &d3, &d4);
	if (m_selRect.m_R.left != d1 || m_selRect.m_R.top != d2 || m_selRect.m_R.right != d3 || m_selRect.m_R.bottom != d4) {
		
	}

	// m_ClientDPRect
	ifSrc->GetViewportRange(&l1, &l2, &l3, &l4);
	if (m_ClientDPRect.left != l1 || m_ClientDPRect.top != l2 || m_ClientDPRect.right != l3 || m_ClientDPRect.bottom != l4) {
		
	}

	// m_nMapMode
	ifSrc->GetMappingMode(&l1);
	if (m_nMapMode != (UINT)l1) {
		
	}

	// m_nEllipsoid
	ifSrc->GetEllipsoid(&l1);
	if (m_nEllipsoid != (UINT)l1) {
		
	}

	// m_ZoomFactor
	ifSrc->GetZoomFactor(&d1);
	if (m_ZoomFactor != d1) {
		
	}

	// m_b3D
	ifSrc->Is3D(&b1);
	if (m_b3D != b1) {
		
	}

	// m_fX_Angle, m_fY_Angle, m_fZ_Angle
	ifSrc->GetViewingAngle(&d5, &d6, &d7);
	if (m_fX_Angle != d5 || m_fY_Angle != d6 || m_fZ_Angle != d7) {
		
	}

	// m_ptGridNum
	ifSrc->GetGridToPoint(&k1, &k2);
	if (m_ptGridNum.x != k1 || m_ptGridNum.y != k2) {
		
	}

	// m_n3DPattern
	ifSrc->Get3DPattern(&k3);
	if (m_n3DPattern != k3) {
		
	}

	// m_bShift
	ifSrc->GetShiftFlag(&b2);
	if (m_bShift != b2) {
		
	}

	// m_MapScale
	ifSrc->GetMapScale(&l5);
	if (m_MapScale != l5) {
		
	}

	// m_nEncScale
	ifSrc->GetEncScale(&l6);
	if (m_MapScale != l6) {
		
	}

	// m_nMapKind
	ifSrc->GetMapKind(&l7);
	if (m_nMapKind != l7) {
		
	}

	// m_bWrapAround
	ifSrc->GetWrapAround(&b4);
	if (m_bWrapAround != b4) {
		
	}


		// 3차원 도시 정보
		// m_pMap3D
		//CGMap3D	*pMap3d;
		//ifSrc->Get3DObject((IUnknown **)&pMap3d);
		//if(pMap3d==NULL) {
		//	if (m_pMap3d==NULL) {
		//		*bEqual = TRUE;
		//	}
		//	
		//}

		//if (m_pMap3d==NULL) {
		//	
		//}

		//// Now pMap3D!=NULL && m_pMap3d!=NULL
		//*bEqual = m_pMap3d.IsEqual(pMap3d);


	* bEqual = TRUE;
}
*/

void CGViewInfo::SetActiveMode(long type)
{
	m_nMode = type;
}

long CGViewInfo::GetActiveMode(long* type)
{
	* type = m_nMode;

	return *type;
}

void CGViewInfo::InitInstance(UINT nMapMode, double left, double top, double right, double bottom)
{
	// Mapping
	m_nMapMode = nMapMode;	// Mapping Mode
	m_nEllipsoid = EWGE;	// 타원체 WGS84

	if (left == 0 && top == 0 && right == 0 && bottom == 0)
		

	// 도시 영역
	m_selRect = CGRectGP(left, top, right, bottom);
	m_WinOrg = m_selRect.CenterPoint();
	m_WinExt = CGSizeGP(right - left, bottom - top);
	m_ClientDPRect.SetRectEmpty();

	// zoom & scale
	m_ZoomFactor = 0.4;	// 확대/축소 배율
	m_MapScale = 1000000;	// 축척
	m_nMapKind = 0; //육도

	//
	// 3차원 도시 정보
	m_b3D = FALSE;		// 2차원
	m_fX_Angle = 0.0f;	// 회전각
	m_fY_Angle = 40.0f;	// 상하응시각(3dLR)
	m_fZ_Angle = 40.0f;	// 응시방향(3dHL)
	m_n3DPattern = 0;   //0:1차원그물망, 1:2차원그물망, 2:색상도, 3:음영도

	m_ptGridNum = CPoint(2, 2);	// 그리드의 간격 60m

	TRACE("CGViewInfo::InitInstance; Left:%f top:%f right:%f bottom:%f\n", left, top, right, bottom);
	
}
