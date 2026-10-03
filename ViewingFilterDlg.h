#pragma once

#include "Flags.h"
#include "Options.h"
#include "OptionsTreeCtrl.h"
//#include "MimsIDFilter.h"
//#include "DisplayElement.h"		

// CViewingFilterDlg dialog

class CViewingFilterDlg : public CPaneDialog
{
	DECLARE_DYNAMIC(CViewingFilterDlg)

public:
	CViewingFilterDlg();   // standard constructor
	virtual ~CViewingFilterDlg();

// Dialog Data
	//enum { IDD = IDD_VIEWFILTER_DLG };

	COptionsTreeCtrl	m_ViewingFilterTreeCtrl;
	COptions			m_OptionsView;
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)    
	{ 
		return CPaneDialog::OnUpdateCmdUI(pTarget, FALSE);
	}

	DECLARE_MESSAGE_MAP()

	void AdjustWidth(int cx, int cy, UINT uiCtrlID);
	virtual BOOL OnInitDialog();
	afx_msg LRESULT HandleInitDialog(WPARAM w, LPARAM l);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	CFlags            m_ViewFlags;
	CMap<unsigned int,unsigned int,unsigned int,unsigned int> m_ViewFilterMap;

	void InitializeViewFilter();
	void InitializeViewOption();
	void InitializeViewFilterMap();
	
	
};
