#if !defined(AFX_CAPTUREWINDOW_H__D9F07177_E232_49F4_A86A_73D27F47912B__INCLUDED_)
#define AFX_CAPTUREWINDOW_H__D9F07177_E232_49F4_A86A_73D27F47912B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaptureWindow.h : header file
//

// 윈도우 캡춰 클래스
class CCaptureWindow
{
// Construction
public:
	CCaptureWindow();

// Attributes
public:

	HWND						m_hLastHWND;			// 윈도우 선택시 마지막 마우스가 이동된 윈도우 핸들
	HCURSOR						m_hSelectorCursor;		// 윈도우 선택시 마우스 커서

	CBitmap						m_bmpCapture;			// 캡춰된 윈도우 화면
	HWND                        hWindow;

// Operations
public:
	
	CBitmap* GetCapturedBitmap() { return &m_bmpCapture; }	// 캡춰된 비트맵을 리턴하는 함수
	void InvertWindowEdge(HWND hWnd);						// 특정 윈도우의 외곽에 테두리를 그리는 함수
	void SetWindow(HWND hWnd) { hWindow = hWnd;}			// Capture할 위도우의 핸들을 저장하는 함수
	void Capture();											// 화면을 캠쳐하고 저장한다.

// Implementation			
public:
	virtual ~CCaptureWindow();

};


#endif // !defined(AFX_CAPTUREWINDOW_H__D9F07177_E232_49F4_A86A_73D27F47912B__INCLUDED_)
