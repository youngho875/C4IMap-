#pragma once


#include "../inc/ICOPS2010V11520/ICOPS_Framework/IMapProjection.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/ICOPS_WarSymCommon.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymbol.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymEditOwner.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymEditTool.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/CwsSymEditOwner.h"
#include "../inc/ICOPS2010V11520/ICOPS_Framework/CTypedBuffer.h"
#include "../inc/ICOPS2010V11520/ICOPS_Framework/ICOPS_struct.h"
#include "../inc/ICOPS2010V11520/ICOPS_Framework/CTypedBuffer.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/CwsColorset.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/CwsUtil.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymbol.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymView.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymEditable.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymEditOwner.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymFactory.h"
#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymPropertyEditor.h"
#include "../inc/ICOPS2010V11520/ICOPS_LogManager/CLog.h"
#include "CTest_OverItem.h"
#include <math.h>




// CMilitarySymbol dialog

class CMilitarySymbol : public CDialog
{
	DECLARE_DYNAMIC(CMilitarySymbol)

public:
	CMilitarySymbol(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMilitarySymbol();

// Dialog Data
	enum { IDD = IDD_MILISYMBOL_DLG };

	public:
	virtual BOOL Create(CWnd* pParentWnd);

//////////////////////////////////////////////////////////////////
// 투명도 관련	young
/////////////////////////////////////////////////////////////////
public:
	// 바탕 이미지
	//Gdiplus::Bitmap		*m_pBgImg;

	// 프로젝션
	ICOPS::IMapProjection	*m_piMapProj;
	double	m_rMPW;

	// 아이콘 편집
	ICOPS::IWarSymEditTool	*m_piEditTool;
	//CC4IRTDSViewEditOwner	*m_pEditOwner;
	CString		m_strLastSymCode;	// 최근에 선택된 부호 코드

	// 화면 조절
	Gdiplus::PointF		m_ptCenter;		// 화면 중심 이동
	Gdiplus::REAL		m_fZoom;		// 화면 배율
	Gdiplus::Bitmap		*m_pDoubleBuffer;	// 더블버퍼링용 이미지

	// 도시 형태
	ICOPS::SWarSymDrawMode		m_sDrawMode;
	BOOL	m_bCacheSymImg;

	// 속도 시험용 부호
	void	DrawSpeedTest(Gdiplus::Graphics *pGrpx);
	ICOPS::CTypedBuffer<ICOPS::IWarSymbol *>	m_piSyms;
	ICOPS::CTypedBuffer<Gdiplus::PointF>	m_testPts;
	BOOL	m_bSpeedTest;

private:
	CDC* m_pDC;
	CWnd* m_pOwnerWnd;

public:
	void InitGraphic(CDC* pDC);
	BOOL SetOwnerWnd(CWnd* pWnd/*=NULL*/);

	//void LButtonUp();
	//void LButtonDblClk(UINT nFlags, CPoint point);
	//void RButtonDown(UINT nFlags, CPoint point);
	//void RButtonUp(UINT nFlags, CPoint point);
	//void MouseMove(UINT nFlags, CPoint point);

// Editor operations
public:
	//void	LoadBackgroundImage();
	void	InitProjection();
	void	InitSymbolEditTool();
	void	ZoomIn(BOOL bFitToSelected);
	void	ZoomOut(BOOL bFitToSelected);
	void	MoveToSelectedCenter();
	void	Pan(int dirX, int dirY);

// Operations
public:
	void	OnPrepareGraphics(Gdiplus::Graphics *pGrpx, const CRect& rcBounds);
	void	DrawOverItems(Gdiplus::Graphics *pGrpx);
	void	ApplyDrawMode(CTest_OverItem *pItem);
	void	ApplyDrawModeToAll();
	Gdiplus::Color	GetBgColor();

	void DrawMilitarySymbol(CDC *pDC);
	void LButtonDown(UINT nFlags, CPoint point);

public:
	afx_msg void OnSelectSymbol(void);
//////////////////////////////////////////////////////////////////////////////////////
// end 군대부호	 young
/////////////////////////////////////////////////////////////////////////////////////

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
