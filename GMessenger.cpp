// GMessenger.cpp: implementation of the CGMessenger class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GMessenger.h"
//#include "SgisComLib.h"
#include "GLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// disable AFX_MANAGE_STATE
#ifdef AFX_MANAGE_STATE
#undef AFX_MANAGE_STATE
#define AFX_MANAGE_STATE(x)
#pragma message( "GMessenger.cpp에서는 AFX_MANAGE_STATE()를 사용하지 않도록 설정하였습니다."  )
#endif

CWnd		*CGMessenger::m_pFrameWnd = NULL;
CStatusBar	*CGMessenger::m_pStatusWnd = NULL;
CWnd		*CGMessenger::m_pProgressWnd = NULL;
CWnd		*CGMessenger::m_pWinkWnd = NULL;

CComPtr<IUnknown> CGMessenger::m_ifExtWeb = NULL;
//CComPtr<IGModuleMgr> CGMessenger::m_ifSgisAx = NULL;


//////////////////////////////////////////////////////////////////////
// CGMessenger Operations

AFX_MODULE_STATE *m_mainState = NULL;


/*
void CGMessenger::SetModuleMgr(IGModuleMgr* ifSgisAx)
{
	if(m_ifSgisAx)
		m_ifSgisAx.Release();

	ifSgisAx->QueryInterface(IID_IGModuleMgr, (void **)&m_ifSgisAx);
}

void CGMessenger::GetModuleMgr(IGModuleMgr* ifSgisAx)
{
	m_ifSgisAx->QueryInterface(IID_IGModuleMgr, (void **)&ifSgisAx);	
}
*/

//IWebBrowser2에 대한 인터페이스를 설정한다.
//void CGMessenger::SetWebBrowser(IWebBrowser2* pIWeb)
//{
//	if(m_ifExtWeb)
//		m_ifExtWeb.Release();
//
//	HRESULT hRes;
//	hRes = pIWeb->QueryInterface(IID_IWebBrowser2, (void**)&m_ifExtWeb);
//}

void CGMessenger::SetFrameWnd(CWnd *pWnd)
{
	m_mainState = AfxGetModuleState();
	m_pFrameWnd = pWnd;
}


AFX_MODULE_STATE* CGMessenger::GetAfxModuleState()
{
	return m_mainState;
}


void CGMessenger::SetStatusWnd(CStatusBar *pStatusWnd)
{
	m_pStatusWnd = pStatusWnd;
}

//주의: 원도우 핸들을 가끔 읽어버리는 경우가 생김:사용시 주의.
void CGMessenger::ShowWinkMessage(CString strMsg, DWORD nTime)
{
	if(StartWinkMessage())
	{
		SetWinkMessage(strMsg);
		Sleep(nTime);
	}
	ClearWinkMessage();
}

BOOL CGMessenger::StartWinkMessage()
{
	ClearWinkMessage();
	CRect clientRect = CGLib::GetClientRect();
	
	int x1, y1, x2, y2;
	x1 = clientRect.CenterPoint().x-200;
	y1 = clientRect.CenterPoint().y-100;
	x2 = x1 + 300;
	y2 = y1 + 30;
	CRect rect(x1, y1, x2, y2);

	//AfxGetInstanceHandle에 대한 Instance값을 전역변수로 부터 받아온다.
	AFX_MODULE_STATE* pAfxState;
	pAfxState = CGMessenger::GetAfxModuleState();
	//AfxSetModuleState(pAfxState);
	AFX_MANAGE_STATE(pAfxState);

	//핸들 검사
	HINSTANCE hInst = AfxGetInstanceHandle();
	ASSERT(hInst != NULL);
	
	m_pWinkWnd	= new CWnd;
	if(m_pWinkWnd)	//jdkim	2005-11-15 
	{
		m_pWinkWnd->Create(_T("STATIC"), "", WS_CHILD | WS_VISIBLE , rect, m_pFrameWnd, 1234);
		m_pWinkWnd->SetWindowPos(&m_pFrameWnd->wndTopMost, 0, 0, 
								  rect.Width(), rect.Height(), SWP_SHOWWINDOW|SWP_DRAWFRAME);
		return TRUE;
	}
	return FALSE;
}

void CGMessenger::SetWinkMessage(CString strMsg)
{
	if(m_pWinkWnd == NULL || m_pWinkWnd->GetSafeHwnd() == NULL) {
		return;
	}

	CClientDC	dc(m_pWinkWnd);
	
	CRect rect; 
	m_pWinkWnd->GetClientRect(&rect);	


	//jdkim	2005-11-14	//폰트 추가, Draw3dRect
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));    

	CSize size(13,13);
	lf.lfHeight = size.cx;                     
	lf.lfWeight = FW_MEDIUM;
	strcpy(lf.lfFaceName, "굴림");     
	font.CreateFontIndirect(&lf);


	dc.SelectObject(&font);
	dc.Draw3dRect(rect, RGB(152, 153, 132), RGB(052, 053, 032));
	rect.top = rect.top + 8;
	rect.bottom = rect.bottom + 5;
	dc.SetBkMode(TRANSPARENT);

	dc.DrawText(strMsg, &rect, DT_CENTER | DT_VCENTER);


	font.DeleteObject();
}

void CGMessenger::ClearWinkMessage()
{

	if (m_pWinkWnd!=NULL && m_pWinkWnd->GetSafeHwnd()!=NULL) {
		m_pWinkWnd->ShowWindow(SW_HIDE);
		delete m_pWinkWnd;
		m_pWinkWnd = NULL;
	} else {
		m_pWinkWnd = NULL;
	}
	
	//AfxSetModuleState(NULL); 
	AFX_MANAGE_STATE(NULL);
}

void CGMessenger::StartProgress()
{
	ClearProgressBar();
	CRect clientRect = CGLib::GetClientRect();
	
	int x1, y1, x2, y2;
	x1 = clientRect.CenterPoint().x-250;
	y1 = clientRect.CenterPoint().y-100;
	x2 = x1 + 800;
	y2 = y1 + 27;
	CRect rect(x1, y1, x2, y2);

	m_pProgressWnd = new CWnd;
	m_pProgressWnd->Create(_T("STATIC"), "", WS_CHILD | WS_VISIBLE | WS_BORDER , rect, m_pFrameWnd, 1234);
	m_pProgressWnd->SetWindowPos(&m_pFrameWnd->wndTopMost, 0, 0, 
								  rect.Width(), rect.Height(), SWP_SHOWWINDOW|SWP_DRAWFRAME);
}

void CGMessenger::StartProgress(CWnd* pWnd)
{
	ClearProgressBar();

	CRect clientRect = CGLib::GetClientRect();
	
	int x1, y1, x2, y2;
	x1 = clientRect.CenterPoint().x-250;
	y1 = clientRect.CenterPoint().y-100;
	x2 = x1 + 800;
	y2 = y1 + 27;
	CRect rect(x1, y1, x2, y2);

	m_pProgressWnd = new CWnd;
	m_pProgressWnd->Create(_T("STATIC"), "", WS_CHILD | WS_VISIBLE | WS_BORDER , rect, pWnd, 1234);
	m_pProgressWnd->SetWindowPos(&pWnd->wndTopMost, 0, 0, 
								  rect.Width(), rect.Height(), SWP_SHOWWINDOW|SWP_DRAWFRAME);
}


void CGMessenger::SetProgress(int nPercent, CString strMsg)
{
	if(m_pProgressWnd == NULL || m_pProgressWnd->GetSafeHwnd() == NULL) {
		return;
	}

	ASSERT(nPercent>=0 && nPercent<=100);

	// 상태바 지우기
	if (nPercent<=0) {
		return;
	}

	float		fPercent = ((float) nPercent) / 100.0f;
	COLORREF	barColor = RGB(51, 85, 221);
	CClientDC	dc(m_pProgressWnd);
	CString		strPercent;
	CRect		rect, progRect;

	// 상태바 크기 계산
	CRect StatusRect; 
	m_pProgressWnd->GetClientRect(&StatusRect);
	rect = StatusRect;
	rect.right = StatusRect.right / 2 ;
	rect.InflateRect(-1, -2);

	progRect.left	= rect.left+2;
	progRect.top	= rect.top+2;
	progRect.right	= rect.right+2;
	progRect.bottom = rect.bottom-1; 
	progRect.right	= (long) (((float) rect.right) * fPercent);
	dc.Rectangle(rect.left, rect.top-1, rect.right, rect.bottom+1);

	// 상태바에 채움 표시
	dc.FillSolidRect(&progRect, barColor);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(10, 10, 10));

	// 상태바에 문자 표시
	rect.left = rect.left + 2;
	rect.top = rect.top + 2;
	rect.bottom = rect.bottom ;

	if(strMsg.IsEmpty()) {
		strPercent.Format("%d %%", nPercent);
		dc.DrawText(strPercent, &rect, DT_CENTER | DT_VCENTER);
	}
	else {
		strPercent.Format("%3d %% : %s", nPercent, strMsg);
		dc.DrawText(strPercent, &rect, DT_LEFT | DT_VCENTER);
	}
}


void CGMessenger::StopProgress()
{
	ClearProgressBar();
}

void CGMessenger::ClearProgressBar()
{
	// 상태바 지우기
	if (m_pProgressWnd!=NULL && m_pProgressWnd->GetSafeHwnd()!=NULL) {
		m_pProgressWnd->ShowWindow(SW_HIDE);
		delete m_pProgressWnd;
		m_pProgressWnd = NULL;
		TRACE0("CLEARE...\n");
	} else {
		m_pProgressWnd = NULL;
	}
}

int CGMessenger::SetMessage(UINT nID, int eTarget, CString strParam)
{
	CString		strMsg;
	UINT		nType;
	HINSTANCE	hInst;
	BOOL		bRet;

	CWinApp	*pApp = AfxGetApp();

	if (pApp!=NULL) {
		hInst = ::AfxGetResourceHandle();
		// ::AfxSetResourceHandle(theApp.m_hInstance);
		bRet = strMsg.LoadString(nID);
		// ::AfxSetResourceHandle(hInst);
	}
	else {
		// ::LoadString(theApp.m_hInstance, nID, strMsg.GetBuffer(512), 512);
		strMsg.ReleaseBuffer();
	}

	if (!bRet) {
		// 문자를 찾지 못함
		ASSERT(FALSE);
		return -1;
	}

	nType = 0;
	switch (strMsg[0]) {
		case 'E' : nType += MB_ICONSTOP; break;
		case 'W' : nType += MB_ICONEXCLAMATION; break;
		case 'I' : nType += MB_ICONINFORMATION; break;
		case 'Q' : nType += MB_ICONQUESTION;
	}
	switch (strMsg[8]) {
		case '0' : nType += MB_OK; break;
		case '1' : nType += MB_OKCANCEL; break;
		case '2' : nType += MB_ABORTRETRYIGNORE; break;
		case '3' : nType += MB_YESNOCANCEL; break;
		case '4' : nType += MB_YESNO; break;
		case '5' : nType += MB_RETRYCANCEL;
	}

	strMsg = MakeErrorMsg(strMsg, strParam);

	return SetMessage(strMsg, eTarget, nType);
}


int CGMessenger::SetMessage(CString strMsg, int eTarget, UINT nType)
{
	int			nRet;

	switch (eTarget) {
		case ToDialog :
			if (m_pFrameWnd!=NULL && m_pFrameWnd->GetSafeHwnd()!=NULL) {
				nRet = m_pFrameWnd->MessageBox(strMsg, "공군C4I체계/시현기능", nType);
			}
			else {
				nRet = ::MessageBox(NULL, strMsg, "공군C4I체계/시현기능", nType);
			}
			break;
		case ToStatus :
			{
				if(m_ifExtWeb == NULL)	break;

				IWebBrowser2* ifWeb;
				HRESULT hRes = m_ifExtWeb->QueryInterface(&ifWeb);
				if(SUCCEEDED(hRes)) {
					CString strCoord;
					strCoord.Format("%s", strMsg);
					ifWeb->put_StatusText((BSTR)CComBSTR(strCoord));
				}

				ifWeb->Release();
			}
			break;
	}

	return nRet;
}


LRESULT CGMessenger::SendToMainframe(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pFrameWnd!=NULL && m_pFrameWnd->GetSafeHwnd()!=NULL) {
		AFX_MANAGE_STATE(m_mainState);
		return m_pFrameWnd->SendMessage(message, wParam, lParam);
	}

	return NULL;
}


BOOL CGMessenger::PostToMainframe(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pFrameWnd!=NULL && m_pFrameWnd->GetSafeHwnd()!=NULL) {
		return m_pFrameWnd->PostMessage(message, wParam, lParam);
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CGMessenger private Operations

CString CGMessenger::MakeErrorMsg(CString strMsg, CString strParam)
{
	CString		strRet;
	CString		strCode;
	CString		tmpStr;
	int			inx;
	int			len;

	if(strMsg.Find("%s")!=-1) {
#ifdef	_DEBUG
		if(strParam.IsEmpty()) {
			TRACE0("추가 문자열을 지정 하십시요.");
			ASSERT(FALSE);
		}
#endif // _DEBUG
		strMsg.Format(strMsg, strParam);
	}

	tmpStr.Empty();
	len = strMsg.GetLength();
	for (inx=0; inx<len; inx++) {
		if (strMsg[inx]=='\\' && strMsg[inx+1]=='n') {
			tmpStr += '\n';
			inx += 1;
		}
		else {
			tmpStr += strMsg[inx];
		}
	}

	strCode = tmpStr.Left(7);
	strMsg = tmpStr.Right(tmpStr.GetLength() - 10);
	strRet.Format("%s : %s", strCode, strMsg);

	return strRet;
}



int CGMessenger::SetPaneMessage(int eTarget, CString strParam)
{
	int nRet = 0;

	ASSERT((eTarget == ToCoord1) || (eTarget == ToCoord2));
	
	if (m_pStatusWnd!=NULL && m_pStatusWnd->GetSafeHwnd()!=NULL) {
		m_pStatusWnd->SetPaneText(eTarget + 1, strParam);
	}

	return nRet;	
}

