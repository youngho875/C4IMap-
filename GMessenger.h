// GMessenger.h: interface for the CGMessenger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMESSENGER_H__37A0F512_5F33_4B06_A472_2CDD24A23714__INCLUDED_)
#define AFX_GMESSENGER_H__37A0F512_5F33_4B06_A472_2CDD24A23714__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
* AfC4ISgis class 와 SgisAx class간의 메신져  클래스.
* @author Peter
* @version 1.0
* @since 2005.11
**/
class CGMessenger  
{
// Attributes
public:
	enum { ToDialog, ToStatus };
	enum { ToCoord1, ToCoord2 };

protected:
	static CWnd			*m_pFrameWnd;
	static CStatusBar	*m_pStatusWnd;
	static CWnd			*m_pProgressWnd;
	static CWnd			*m_pWinkWnd;
	static CComPtr<IUnknown> m_ifExtWeb;

//	static CComPtr<IGModuleMgr> m_ifSgisAx;
	
// Operations
public:
	static AFX_MODULE_STATE* GetAfxModuleState();

	//static void		SetWebBrowser(IWebBrowser2* pIWeb);
	//static IWebBrowser2Ptr  GetWebBrowser()
	//{
	//	IWebBrowser2* pIWeb;
	//	m_ifExtWeb->QueryInterface(IID_IWebBrowser2, (void**)&pIWeb);
	//	return pIWeb;
	//}


	static void		SetFrameWnd(CWnd *pWnd);
	static void		SetStatusWnd(CStatusBar *pStatusWnd);

	//Wink Message
	static void		ShowWinkMessage(CString strMsg, DWORD nTime = 1000);
	static BOOL		StartWinkMessage();
	static void		SetWinkMessage(CString strMsg);
	static void		ClearWinkMessage();

	//Progress
	static void		StartProgress();
	static void		StartProgress(CWnd *pWnd);
	static void		SetProgress(int nPercent, CString strMsg);
	static void		StopProgress();

	static int		SetMessage(UINT nID, int eTarget, CString strParam = CString(""));
	static int		SetMessage(CString strMsg, int eTarget, UINT nType = MB_OK | MB_ICONINFORMATION);
	static LRESULT	SendToMainframe(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	static BOOL		PostToMainframe(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	static int		SetPaneMessage(int eTarget, CString strParam = CString(""));

protected:
	static CString	MakeErrorMsg(CString strMsg, CString strParam);
	static void		ClearProgressBar();
};


#endif // !defined(AFX_GMESSENGER_H__37A0F512_5F33_4B06_A472_2CDD24A23714__INCLUDED_)
