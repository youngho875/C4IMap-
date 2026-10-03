// MilitarySymbol.cpp : implementation file
//

#include "pch.h"
#include "C4IMap.h"
#include "MilitarySymbol.h"
#include "C4IMapView.h"
#include "C4IMapDoc.h"


// CMilitarySymbol dialog

IMPLEMENT_DYNAMIC(CMilitarySymbol, CDialog)

CMilitarySymbol::CMilitarySymbol(CWnd* pParent /*=NULL*/)
	: CDialog(CMilitarySymbol::IDD, pParent)
{
	//m_pBgImg = NULL;
	m_piMapProj = NULL;
	m_piEditTool = NULL;
	m_pDoubleBuffer = NULL;
	m_strLastSymCode.Empty();
	m_bSpeedTest = FALSE;
	m_bCacheSymImg = FALSE;

	//InitProjection();
	//InitSymbolEditTool();
	//m_piEditTool->OnChangeEditableList();
}

CMilitarySymbol::~CMilitarySymbol()
{
	//if (m_pBgImg!=NULL) delete m_pBgImg;
	if (m_piEditTool!=NULL) m_piEditTool->Release();
	if (m_piMapProj!=NULL) m_piMapProj->Release();
	if (m_pDoubleBuffer!=NULL) delete m_pDoubleBuffer;
}

void CMilitarySymbol::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMilitarySymbol, CDialog)
	ON_BN_CLICKED(IDOK, &CMilitarySymbol::OnBnClickedOk)
END_MESSAGE_MAP()


// CMilitarySymbol message handlers
BOOL CMilitarySymbol::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pParentWnd = (CC4IMapView*)pParentWnd;

	return CDialog::Create(IDD, pParentWnd);
}

void CMilitarySymbol::InitGraphic(CDC* pDC)
{
	m_pDC = pDC;
}

BOOL CMilitarySymbol::SetOwnerWnd(CWnd* pWnd/*=NULL*/)
{
	m_pOwnerWnd = pWnd;	
	return TRUE;
}

void CMilitarySymbol::InitProjection()
{
	CC4IMapApp		*pApp = (CC4IMapApp*) ::AfxGetApp();
	ICOPS::CAppContext	*pAppCxt = pApp->m_pAppCxt;
	ICOPS::SGeoCoord2	gpC;
	BOOL	bRes;
	//CCoord m_Coord;
	//CCoordMgr*	m_pCoordMgr = GP_COORDMGR;

	bRes = pAppCxt->CreateObject(_T("ICOPS_Framework"), _T(".dll"), _T("CMapProjectionNone"), _T("IMapProjection"), (void **) &m_piMapProj);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("좌표투영 객채 생성 실패 : CMapProjectionNone / IMapProjection"));
		return;
	}

	// 지도 중심좌표 설정
	//double x, y, lat, lon;
	//m_pCoordMgr->ScreenXY2WorldXY(m_pCoordMgr->m_fCenterX, m_pCoordMgr->m_fCenterY, &x, &y);
	//m_Coord.x_y_to_degrees_lat_long(x, y, &lat, &lon);
	//gpC.lon = lon;
	//gpC.lat = lat;

	//float zoomfactor = m_pCoordMgr->m_fScale;
	gpC.lon = 128.0;
	gpC.lat = 37.0;
	
	m_piMapProj->SetRadius(EARTH_RADIUS);
	m_piMapProj->SetMapScale(1000000);
	m_piMapProj->SetZoomFactor(1.0f);
	m_piMapProj->SetCenter(&gpC);
}


void CMilitarySymbol::InitSymbolEditTool()
{
	CC4IMapApp *pApp = (CC4IMapApp*) ::AfxGetApp();
	ICOPS::CAppContext	*pAppCxt = pApp->m_pAppCxt;
	BOOL	bRes;

	bRes = pAppCxt->CreateObject(_T("IWarSymEditTool"), (void **) &m_piEditTool);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("IWarSymEditTool 인터페이스를 구현하는 사용자정의 객체 생성 실패"));
	}
	else
	{
		//double z = m_piMapProj->GetZoomFactor();
		//double zz =m_piMapProj->GetMapScale();
		m_piMapProj->SetZoomFactor(1.0f);
		m_piMapProj->SetMapScale(1.0f);
		m_rMPW = 1.0f;


		// Initialize IWarSymEditTool component object
		m_piEditTool->SetEditOwner(dynamic_cast<ICOPS::IWarSymEditOwner *>(((CC4IMapView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView())->m_pEditOwner));
		m_piEditTool->SetMapProjection(m_piMapProj);
		m_piEditTool->SetMeterPerPixel(m_rMPW);
		m_piEditTool->SnapToGrid(FALSE);

		m_piEditTool->SetEditingMode(RefreshOnChange);	// 부호 작도중 부호 변경시마다 화면 갱신
		//m_piEditTool->SetEditingMode(UseInverseDraw);	// 부호 작도중 부호 변경시 역상 도시 사용하여 rubber banding 기능 수행
	}
}


void CMilitarySymbol::OnPrepareGraphics(Gdiplus::Graphics *pGrpx, const CRect& rcBounds)
{
	CPoint	cp;
	ICOPS::SOrthoCoord2	vc;

	// reset transform matrix
	pGrpx->ResetTransform();

	cp = rcBounds.CenterPoint();
	
	vc.x = cp.x;
	vc.y = cp.y;
	m_piMapProj->SetViewOrigin(&vc);
}


void CMilitarySymbol::DrawOverItems(Gdiplus::Graphics *pGrpx)
{
	INT_PTR		inx, count, bufSize;
	CTest_OverItem	**ppItems, *pItem;
	CC4IRTDSDoc	*pDoc = (CC4IRTDSDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	Gdiplus::PointF		*pWp;
	ICOPS::SGeoCoord2	*pGp;
	ICOPS::CTypedBuffer<Gdiplus::PointF>	bufWp;
	Gdiplus::Color	clrCtrl(70, 0, 0, 0);

	count = pDoc->m_parOverItems.GetSize();
	if (count<=0) 
		return;

	ppItems = (CTest_OverItem **) pDoc->m_parOverItems.GetData();
	for (inx=0; inx<count; inx++)
	{
		pItem = ppItems[inx];

		bufSize = pItem->GetCoordCount();
		if (bufSize>0)
		{
			pWp = bufWp.SetCount(bufSize);
			pGp = pItem->GetCoordData();
			TRACE("%f --  %f\n", pGp->lon, pGp->lat);
			m_piMapProj->Forward(pGp, pWp, bufSize);
			pItem->m_piSym->Draw(pGrpx, pWp, bufSize, m_rMPW);
		}
	}
}


void CMilitarySymbol::ApplyDrawModeToAll()
{
	INT_PTR		inx, count;
	CTest_OverItem	**ppItems;
	CC4IRTDSDoc	*pDoc = (CC4IRTDSDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	SWarSymDrawMode	*pSymDrawMode;

	count = pDoc->m_parOverItems.GetSize();
	if (count<=0) return;

	ppItems = (CTest_OverItem **) pDoc->m_parOverItems.GetData();
	for (inx=0; inx<count; inx++)
	{
		pSymDrawMode = ppItems[inx]->m_piSym->GetDrawMode();
		*pSymDrawMode = m_sDrawMode;
		ppItems[inx]->m_piSym->InvalidateSymbolImage();
	}
}


Gdiplus::Color CMilitarySymbol::GetBgColor()
{
	if (m_sDrawMode.isBgWhite)
	{
		return Gdiplus::Color(255, 254, 254, 254);
	}
	else
	{
		return Gdiplus::Color(255, 1, 1, 1);
	}
}


void CMilitarySymbol::ZoomIn(BOOL bFitToSelected)
{
	FLOAT	fZoom, fScale;

	fScale = m_piMapProj->GetMapScale();
	fZoom = m_piMapProj->GetZoomFactor();
	fZoom *= 1.2f;	//2.0;
	m_piMapProj->SetZoomFactor(fZoom);
	m_rMPW = fScale / fZoom;
	m_piEditTool->SetMeterPerPixel(m_rMPW);
	Invalidate();
}


void CMilitarySymbol::ZoomOut(BOOL bFitToSelected)
{
	FLOAT	fZoom, fScale;

	fScale = m_piMapProj->GetMapScale();
	fZoom = m_piMapProj->GetZoomFactor();
	fZoom /= 1.2f;	//2.0;
	m_piMapProj->SetZoomFactor(fZoom);
	m_rMPW = fScale / fZoom;
	m_piEditTool->SetMeterPerPixel(m_rMPW);
	Invalidate();
}


void CMilitarySymbol::MoveToSelectedCenter()
{
	Invalidate();
}


void CMilitarySymbol::Pan(int dirX, int dirY)
{
	ICOPS::SOrthoCoord2	vp;
	ICOPS::SGeoCoord2		gp;

	m_piMapProj->GetViewOrigin(&vp);
	vp.x += dirX;
	vp.y += dirY;
	m_piMapProj->Inverse(&vp, &gp, 1);
	m_piMapProj->SetCenter(&gp);
	Invalidate();
}

/*
void CMilitarySymbol::LButtonUp()
{
	CRect		rc;
	CClientDC	dc(this);
	Gdiplus::Graphics	grpx(dc.GetSafeHdc());

	GetClientRect(&rc);
	OnPrepareGraphics(&grpx, rc);

	//m_piEditTool->OnLButtonUp(&grpx, nFlags, point);
}


void CMilitarySymbol::LButtonDblClk(UINT nFlags, CPoint point)
{
	CRect		rc;
	CClientDC	dc(this);
	Gdiplus::Graphics	grpx(dc.GetSafeHdc());

	GetClientRect(&rc);
	OnPrepareGraphics(&grpx, rc);

	m_piEditTool->OnLButtonDblClk(&grpx, nFlags, point);
}


void CMilitarySymbol::RButtonDown(UINT nFlags, CPoint point)
{
	CRect		rc;
	CClientDC	dc(this);
	Gdiplus::Graphics	grpx(dc.GetSafeHdc());

	GetClientRect(&rc);
	OnPrepareGraphics(&grpx, rc);

	m_piEditTool->OnRButtonDown(&grpx, nFlags, point);
}


void CMilitarySymbol::RButtonUp(UINT nFlags, CPoint point)
{
	CRect		rc;
	CClientDC	dc(this);
	Gdiplus::Graphics	grpx(dc.GetSafeHdc());

	GetClientRect(&rc);
	OnPrepareGraphics(&grpx, rc);

	m_piEditTool->OnRButtonUp(&grpx, nFlags, point);
}


void CMilitarySymbol::MouseMove(UINT nFlags, CPoint point)
{
	CRect		rc;
	CClientDC	dc(this);
	Gdiplus::Graphics	grpx(dc.GetSafeHdc());

	GetClientRect(&rc);
	OnPrepareGraphics(&grpx, rc);

	m_piEditTool->OnMouseMove(&grpx, nFlags, point);
}


// Ctrl        : 확대/축소
// Ctrl + Alt  : 선택된 도형 중심으로 확대/축소
// Shift       : 상하이동
// Shift + Alt : 좌우이동
BOOL CMilitarySymbol::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	BOOL	bAlt;
	SHORT	keyStat;

	keyStat = GetKeyState(VK_LMENU);
	bAlt = (keyStat & 0x8000) != 0;

	if (nFlags & MK_CONTROL)
	{
		if (zDelta<0)
		{	// 확대
			ZoomIn(bAlt);
		}
		else
		{	// 축소
			ZoomOut(bAlt);
		}

		return TRUE;
	}
	else if (nFlags & MK_SHIFT)
	{
		if (bAlt)
		{	// 좌우이동
			Pan((zDelta<0 ? 1 : -1), 0);
		}
		else
		{	// 상하이동
			Pan(0, (zDelta<0 ? -1 : 1));
		}
		return TRUE;
	}

	return CView::OnMouseWheel(nFlags, zDelta, point);
}


void CMilitarySymbol::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRect		rc;
	CClientDC	dc(this);
	Gdiplus::Graphics	grpx(dc.GetSafeHdc());

	GetClientRect(&rc);
	OnPrepareGraphics(&grpx, rc);

	m_piEditTool->OnKeyDown(&grpx, nChar, nRepCnt, nFlags);
}

void CMilitarySymbol::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pDoubleBuffer!=NULL)
	{
		delete m_pDoubleBuffer;
		m_pDoubleBuffer = NULL;
	}
}


BOOL CMilitarySymbol::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	BOOL	bRes;

	bRes = m_piEditTool->OnSetCursor(pWnd, nHitTest, message);

	if (!bRes)
	{
		bRes = CView::OnSetCursor(pWnd, nHitTest, message);
	}

	return bRes;
}


void CMilitarySymbol::OnEditCopy()
{
}


void CMilitarySymbol::OnEditCut()
{
}


void CMilitarySymbol::OnEditPaste()
{
}
*/

////////////////////////////////////////////////////////////////////////////////
// Ooperations


void CMilitarySymbol::DrawMilitarySymbol(CDC *pDC)
{
	Gdiplus::Graphics	grpxDC(pDC->GetSafeHdc());
	CRect	rc;

	m_pParentWnd->GetClientRect(&rc);

#if defined(_DEBUG) && defined(new)
#undef new
#endif

	if (m_pDoubleBuffer==NULL)
	{
		m_pDoubleBuffer = new Gdiplus::Bitmap(rc.Width(), rc.Height(), &grpxDC);
	}
	else
	{
		if (m_pDoubleBuffer->GetWidth()<(UINT)rc.Width() || m_pDoubleBuffer->GetHeight()<(UINT)rc.Height())
		{
			delete m_pDoubleBuffer;
			m_pDoubleBuffer = new Gdiplus::Bitmap(rc.Width(), rc.Height(), &grpxDC);
		}
	}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


	Gdiplus::Graphics	grpx(m_pDoubleBuffer);

	//if (m_pBgImg==NULL)
	//{
	//	grpx.Clear(GetBgColor());
	//}
	//else
	//{
	//	INT		imgW, imgH, clW, clH;
	//	INT		x, y;

	//	imgW = m_pBgImg->GetWidth() / 2;
	//	imgH = m_pBgImg->GetHeight() / 2;
	//	clW = rc.Width() / 2;
	//	clH = rc.Height() / 2;
	//	x = clW - imgW;
	//	y = clH - imgH;		

	//	grpx.DrawImage(m_pBgImg, x, y);
	//}

	OnPrepareGraphics(&grpx, rc);


	//if (m_bSpeedTest)
	//{
	//	DrawSpeedTest(&grpx);
	//}
	//else
	{
		// Draw symbols
		DrawOverItems(&grpx);

		// Draw editing item or states
		m_piEditTool->DrawEditingStatus(&grpx);
	}

	// Swap buffers
	grpxDC.DrawImage(m_pDoubleBuffer, rc.left, rc.top, rc.Width(), rc.Height());
}



// command process
void CMilitarySymbol::OnSelectSymbol()
{
	BOOL	bRes;
	CTest_OverItem			*pItem;
	CC4IMapApp			*pApp = (CC4IMapApp*) ::AfxGetApp();
	ICOPS::CAppContext		*pAppCxt = pApp->m_pAppCxt;
	ICOPS::IWarSymFactoryPtr	piSymFac;
	ICOPS::IWarSymbolPtr		piSym;
	ICOPS::IWarSymEditablePtr	piSymE;
	ICOPS::IWarSymPropertyEditorPtr		piPrpEditor;

	bRes = pAppCxt->GetStockObject(_T("IWarSymFactory"), (void **) &piSymFac);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("군대부호관리 객체 얻기 실패"));
		return;
	}

	bRes = piSymFac->CreatePropertyEditor(&piPrpEditor);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("군대부호 편집 컴포넌트 객체 얻기 실패"));
		return;
	}

	piPrpEditor->SetWarSymFactory(piSymFac);
	//piPrpEditor->NeedModifier(FALSE);	// 수식정보 설정 탭을 사용하지 않으려면 FALSE로 설정
	bRes = piPrpEditor->OpenSymbolSelector(m_strLastSymCode, this, &piSym);
	if (bRes)
	{
		m_strLastSymCode = piSym->GetSymbolCode();
		pItem = new CTest_OverItem;
		pItem->AddRef();
		pItem->m_piSym = piSym;

		ICOPS::IWarSymView		*piSymView;
		ICOPS::CwsColorset		*pColorset;
		piSymView = piSym->GetSymbolView();
		if (piSymView!=NULL)
		{
			pColorset = piSymView->GetColorset();
			if (pColorset!=NULL)
			{
				pColorset->SetNormalTextColor(Gdiplus::Color(255, 0, 0, 0));	// 레이블 문자 색상
				pColorset->SetInverseTextColor(Gdiplus::Color(0, 255, 255, 255));	// 레이블 문자 배경을 투명하게
			}
			//piSymView->SetFontSizeScale(1.0f);	// 레이블 문자 크기
			//piSymView->SetLineWidthScale(2.0f);	// 선 굵기
		}

		bRes = pItem->QueryInterface(_T("IWarSymEditable"), (void **) &piSymE);
		if (bRes) m_piEditTool->SetAddingSymbol(piSymE);
	}
}

/*
BOOL CMilitarySymbol::OnEraseBkgnd(CDC* pDC)
{
	// 더블버퍼링을 사용하므로 화면 깜박거림 방지를 위해 화면 지우지 않음
	return TRUE;
}


void CMilitarySymbol::OnDrawmodeBg()
{
	m_sDrawMode.isBgWhite = !m_sDrawMode.isBgWhite;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateDrawmodeBg(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.isBgWhite);
}


void CMilitarySymbol::OnDrawmodeColor()
{
	m_sDrawMode.isColorMode = !m_sDrawMode.isColorMode;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateDrawmodeColor(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.isColorMode);
}


void CMilitarySymbol::OnDrawmodeFillFrame()
{
	m_sDrawMode.fillFrame = !m_sDrawMode.fillFrame;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateDrawmodeFillFrame(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.fillFrame);
}


void CMilitarySymbol::OnDrawmodeShowFrame()
{
	m_sDrawMode.showFrame = !m_sDrawMode.showFrame;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateDrawmodeShowFrame(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.showFrame);
}


void CMilitarySymbol::OnDrawmodeShowIcon()
{
	m_sDrawMode.showIcon = !m_sDrawMode.showIcon;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateDrawmodeShowIcon(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.showIcon);
}


void CMilitarySymbol::OnLuminoDark()
{
	m_sDrawMode.luminance = ICOPS::WarSymLuminance_Dark;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateLuminoDark(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.luminance==ICOPS::WarSymLuminance_Dark);
}


void CMilitarySymbol::OnLuminoMedium()
{
	m_sDrawMode.luminance = ICOPS::WarSymLuminance_Medium;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateLuminoMedium(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.luminance==ICOPS::WarSymLuminance_Medium);
}


void CMilitarySymbol::OnLuminoLight()
{
	m_sDrawMode.luminance = ICOPS::WarSymLuminance_Light;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateLuminoLight(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.luminance==ICOPS::WarSymLuminance_Light);
}


void CMilitarySymbol::OnLuminoCustom()
{
	m_sDrawMode.luminance = ICOPS::WarSymLuminance_Custom;
	ApplyDrawModeToAll();
	Invalidate();
}


void CMilitarySymbol::OnUpdateLuminoCustom(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_sDrawMode.luminance==ICOPS::WarSymLuminance_Custom);
}
*/

/*
// 성능시험용 부호목록 파일을 읽어서 화면에 도시
void CMilitarySymbol::OnTestSpeed()
{
	BOOL		bRes;
	INT_PTR		nRes;
	CString		strPath, strCode, strMsg;
	CStringArray	straCodes;
	CStdioFile		file;

	CFileDialog		dlg(TRUE);

	nRes = dlg.DoModal();
	if (nRes!=IDOK) return;

	strPath = dlg.GetPathName();

	bRes = file.Open(strPath, CFile::modeRead | CFile::typeText | CFile::shareDenyNone);
	if (!bRes)
	{
		::AfxMessageBox(_T("파일을 열 수 없습니다."), MB_OK | MB_ICONSTOP);
		return;
	}

	while (file.ReadString(strCode))
	{
		straCodes.Add(strCode);
	}


	// 부호 생성
	INT_PTR		inx, count, countSym;
	ICOPS::IWarSymbol	**ppSyms, *piSym;
	ICOPS::IWarSymView	*piSymView;
	ICOPS::IWarSymFactory	*piSymFac;

	CC4IRTDSApp				*pApp = (CC4IRTDSApp *) ::AfxGetApp();
	ICOPS::CAppContext			*pAppCxt = pApp->m_pAppCxt;

	bRes = pAppCxt->GetStockObject(_T("IWarSymFactory"), (void **) &piSymFac);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("군대부호관리 객체 얻기 실패"));
		return;
	}

	count = straCodes.GetSize();
	ppSyms = m_piSyms.SetCount(count);

	countSym = 0;
	for (inx=0; inx<count; inx++)
	{
		strCode = straCodes.GetAt(inx);

		bRes = piSymFac->CreateSymbol(strCode, &piSym);
		if (bRes)
		{
			piSym->SetSymbolSize(7.0f);
			piSymView = piSym->GetSymbolView();
			if (piSymView!=NULL)
			{
				piSymView->CacheSymbolImage(m_bCacheSymImg);
				piSymView->SetAffiliation(strCode.GetAt(1));
				piSymView->LinkModifierInfo();
				piSymView->LinkFrameSymbol();
			}
			ppSyms[countSym] = piSym;
			countSym++;
		}
	}

	// 부호 도시 위치 준비
	INT_PTR		inxX, inxY, inxSym;
	Gdiplus::PointF		*pts;

	// 화면에 균등배분 ( 가로 37개, 세로 27개)
	CRect	rc;
	Gdiplus::REAL	w, h, dx, dy, sx, sy, x, y;
	INT_PTR		cx, cy;

	m_bSpeedTest = TRUE;
	GetClientRect(&rc);
	w = (Gdiplus::REAL) rc.Width();
	h = (Gdiplus::REAL) rc.Height();

	if (w==0.0f) w = 1000.0f;
	if (h==0.0f) h = 1000.0f;

	//cx = 38;
	//cy = 27;
	cy = (INT_PTR) (sqrt(countSym * h / w) + 0.5f) + 1;
	cx = (INT_PTR) (cy * w / h) - 1;

	dx = w / (cx + 1);
	dy = h / (cy + 1);
	sx = 0.5f * dx;
	sy = 0.5f * dy;

	inxSym = 0;
	pts = m_testPts.SetCount(countSym);
	strMsg.Format(_T("도시속도 시험용 부호 생성을 완료했습니다. (%d 개)"), countSym);

	x = sx;
	for (inxX=0; inxX<cx && inxSym<countSym; inxX++)
	{
		y = sy;
		for (inxY=0; inxY<cy && inxSym<countSym; inxY++)
		{
			pts[inxSym].X = x;
			pts[inxSym].Y = y;
			inxSym++;

			if (inxSym>=countSym)
			{
				::AfxMessageBox(strMsg, MB_OK);
				Invalidate();
				return;
			}

			y += dy;
		}

		x += dx;
	}

	::AfxMessageBox(strMsg, MB_OK);
	Invalidate();
}


void CMilitarySymbol::OnTestClear()
{
	INT_PTR		inx, count;
	ICOPS::IWarSymbol	**ppSyms;

	count = m_piSyms.GetCount();
	ppSyms = m_piSyms.GetBuffer();

	for (inx=0;inx<count; inx++)
	{
		ppSyms[inx]->Release();
	}
	m_piSyms.SetCount(0);
	m_testPts.SetCount(0);

	m_bSpeedTest = FALSE;
	Invalidate();
}


void CMilitarySymbol::DrawSpeedTest(Gdiplus::Graphics *pGrpx)
{
	INT_PTR		inx, count;
	Gdiplus::PointF		*pts;
	ICOPS::IWarSymbol	**ppiSyms;

	count = m_piSyms.GetCount();
	ppiSyms = m_piSyms.GetBuffer();
	pts = m_testPts.GetBuffer();

	for (inx=0; inx<count; inx++)
	{
		ppiSyms[inx]->Draw(pGrpx, &pts[inx], 1, 1.0);
	}
}


void CMilitarySymbol::OnEditDelete()
{
	// 선택된 도형 삭제
	m_piEditTool->DeleteSelected(TRUE);
	Invalidate();
}


void CMilitarySymbol::OnEditProperty()
{
	CString		strSymCode;
	BOOL		bRes;
	CTest_OverItem		*pItem;
	ICOPS::IWarSymEditable			**ppSelected;
	ICOPS::IWarSymPropertyEditor	*piPrpEditor = NULL;

	CC4IRTDSApp			*pApp = (CTest_WarSymApp *) ::AfxGetApp();
	ICOPS::CAppContext		*pAppCxt = pApp->m_pAppCxt;
	ICOPS::IWarSymFactory	*piSymFac;

	// 선택된 도형 얻기
	ppSelected = m_piEditTool->GetSelected();
	if (ppSelected==NULL) return;	// 선택된 도형 없음

	pItem = NULL;
	//pItem = dynamic_cast<CTest_OverItem *>(ppSelected[0]);
	bRes = ppSelected[0]->QueryInterface(_T("CTest_OverItem"), (void **) &pItem);
	if (!bRes || pItem==NULL)
	{
		ASSERT(FALSE);
		return;	// 선택된 도형의 자료형이 다름
	}

	bRes = pAppCxt->GetStockObject(_T("IWarSymFactory"), (void **) &piSymFac);
	if (!bRes)
	{
		pItem->Release();
		return;
	}

	bRes = piSymFac->CreatePropertyEditor(&piPrpEditor);
	if (!bRes)
	{
		piSymFac->Release();
		return;
	}

	piPrpEditor->SetWarSymFactory(piSymFac);
	piPrpEditor->SetTargetSymbol(pItem->m_piSym);
	//piPrpEditor->NeedModifier(FALSE);	// 수식정보 설정 탭을 사용하지 않으려면 FALSE로 설정
	piPrpEditor->OpenEditor(_T("부호속성 설정"), this);

	pItem->Release();
	piPrpEditor->Release();
	piSymFac->Release();

	Invalidate();
}


void CMilitarySymbol::OnCacheSymimg()
{
	INT_PTR		inx, count;
	ICOPS::IWarSymbol	**ppSyms;

	m_bCacheSymImg = !m_bCacheSymImg;

	count = m_piSyms.GetCount();
	ppSyms = m_piSyms.GetBuffer();

	for (inx=0;inx<count; inx++)
	{
		ppSyms[inx]->CacheSymbolImage(m_bCacheSymImg);
	}

	CTest_WarSymDoc	*pDoc = GetDocument();
	CTest_OverItem	*pOverItem;

	count = pDoc->m_parOverItems.GetSize();
	if (count>0)
	{
		for (inx=0; inx<count; inx++)
		{
			pOverItem = pDoc->m_parOverItems.GetAt(inx);
			pOverItem->m_piSym->CacheSymbolImage(m_bCacheSymImg);
		}
	}

	Invalidate();
}


void CMilitarySymbol::OnUpdateCacheSymimg(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bCacheSymImg);
}


void CMilitarySymbol::OnCopySelected()
{
	BOOL	bRes;
	INT_PTR		inx, count;
	ICOPS::IWarSymEditable	**ppiEditables;
	CTest_OverItemPtr	pOverItem, pNewOverItem;
	CTest_WarSymDoc	*pDoc = GetDocument();

	count = m_piEditTool->GetSelectedCount();
	ppiEditables = m_piEditTool->GetSelected();

	for (inx=0; inx<count; inx++)
	{
		pOverItem = NULL;
		pNewOverItem = NULL;
		bRes = ppiEditables[inx]->QueryInterface(_T("CTest_OverItem"), (void **) &pOverItem);
		if (bRes)
		{
			bRes = pOverItem->Clone(&pNewOverItem);
			if (bRes)
			{
				pDoc->m_parOverItems.Add(pNewOverItem);
				pNewOverItem->AddRef();
			}
		}
	}

	m_piEditTool->OnChangeEditableList();
}


void CMilitarySymbol::OnUpdateCopySelected(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_piEditTool->GetSelectedCount()>0);
}


void CMilitarySymbol::OnEditRefreshMode()
{
	m_piEditTool->SetEditingMode(RefreshOnChange);
}


void CMilitarySymbol::OnUpdateRefreshMode(CCmdUI *pCmdUI)
{
	EWarSymEditingMode	eEditMode;

	eEditMode = m_piEditTool->GetEditingMode();
	pCmdUI->SetCheck(eEditMode==RefreshOnChange);
}


void CMilitarySymbol::OnInverseMode()
{
	m_piEditTool->SetEditingMode(UseInverseDraw);
}


void CMilitarySymbol::OnUpdateInverseMode(CCmdUI *pCmdUI)
{
	EWarSymEditingMode	eEditMode;

	eEditMode = m_piEditTool->GetEditingMode();
	pCmdUI->SetCheck(eEditMode==UseInverseDraw);
}
*/

void CMilitarySymbol::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnSelectSymbol();
	//CDialog::OnOK();
}

/*
ICOPS::IWarSymFactoryPtr piSymFac;
ICOPS::IWarSymbol *piSym;


Gdiplus::Graphics gpx(memDC->m_hDC);
theApp.m_pAppCxt->GetStockObject(_T("IWarSymFactory"), (void **) &piSymFac);
piSymFac->CreateSymbol(_T("SFGAES---------"), &piSym);
piSym->Draw(&gpx,(Gdiplus::PointF*)¢er,1,1);
*/