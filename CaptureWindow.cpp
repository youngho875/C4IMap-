// CaptureWindow.cpp : implementation file
//

#include "pch.h"
#include "C4IMap.h"
#include "FilePath.h"
#include "CaptureWindow.h"
#include <atlimage.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptureWindow

CCaptureWindow::CCaptureWindow()
{
	m_hSelectorCursor = AfxGetApp()->LoadCursor(IDC_ARROW);	// 커서 모양은 나중에 바꾸기로 유영호
	m_hLastHWND		= NULL;
	hWindow = NULL;
}

CCaptureWindow::~CCaptureWindow()
{
	// LoadCursor returns a shared cursor; it must not be deleted.

	if(m_bmpCapture.m_hObject)	m_bmpCapture.DeleteObject();
}



// 특정 윈도우의 외곽에 테두리를 그리는 함수
void CCaptureWindow::InvertWindowEdge(HWND hWnd)
{
	RECT stRect;

	// 해당 윈도우의 위치를 구한다
	::GetWindowRect(hWnd, &stRect);

	// 해당 윈도우의 윈도우 DC를 구한다
	HDC hDC = ::GetWindowDC(hWnd);

	// 역상 출력 속성을 설정한다
	SetROP2(hDC, R2_NOT);

	// 시스템의 테두리 두께의 5배 굵기로 검은색 펜을 생성한다
	HPEN hPen = CreatePen(PS_INSIDEFRAME, 5 * GetSystemMetrics(SM_CXBORDER), RGB(0,0,0));

	// 생성한 펜을 설정한다
	HPEN   hOldPen   = (HPEN)SelectObject(hDC, hPen);
	// 투명 브러쉬를 설정한다
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

	// 윈도우의 외곽에 사각 테두리를 출력한다
	Rectangle(hDC, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top);

	// 이전 객체들을 되돌린다
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);

	// DC를 반환한다
	::ReleaseDC(hWnd, hDC);

	// 펜을 제거한다
	DeleteObject(hPen);
}

void CCaptureWindow::Capture()
{
    if (!::IsWindow(hWindow))
        return;

    CRect rect;
    if (!::GetWindowRect(hWindow, &rect) || rect.IsRectEmpty())
        return;

    HDC source = ::GetWindowDC(hWindow);
    if (!source)
        return;
    HDC memory = ::CreateCompatibleDC(source);
    HBITMAP bitmap = ::CreateCompatibleBitmap(source, rect.Width(), rect.Height());
    if (!memory || !bitmap)
    {
        if (bitmap) ::DeleteObject(bitmap);
        if (memory) ::DeleteDC(memory);
        ::ReleaseDC(hWindow, source);
        return;
    }
    HGDIOBJ oldBitmap = ::SelectObject(memory, bitmap);
    BOOL captured = ::BitBlt(memory, 0, 0, rect.Width(), rect.Height(), source, 0, 0, SRCCOPY);
    ::SelectObject(memory, oldBitmap);
    ::DeleteDC(memory);
    ::ReleaseDC(hWindow, source);
    if (!captured)
    {
        ::DeleteObject(bitmap);
        return;
    }

    CImage image;
    image.Attach(bitmap);
    HBITMAP clipboardBitmap = (HBITMAP)::CopyImage(bitmap, IMAGE_BITMAP, 0, 0, 0);
    if (clipboardBitmap)
    {
        if (::OpenClipboard(hWindow))
        {
            if (::EmptyClipboard() && ::SetClipboardData(CF_BITMAP, clipboardBitmap))
                clipboardBitmap = NULL;
            ::CloseClipboard();
        }
        if (clipboardBitmap) ::DeleteObject(clipboardBitmap);
    }

    CString fileName = CFilePath::GetRoot();
    fileName += FILEPATH_CAPTURE;
    fileName += COleDateTime::GetCurrentTime().Format(_T("%Y%m%d-%H%M%S.bmp"));
    CFileDialog dialog(FALSE, _T("bmp"), fileName,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
        _T("Bitmap (*.bmp)|*.bmp|JPEG (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG (*.png)|*.png|GIF (*.gif)|*.gif||"));
    if (dialog.DoModal() == IDOK && FAILED(image.Save(dialog.GetPathName())))
        AfxMessageBox(_T("Image file save failed."));
    hWindow = NULL;
}
