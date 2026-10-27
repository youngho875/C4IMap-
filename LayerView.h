#pragma once

#include "ViewList.h"
#include <map>
#include <vector>


typedef std::map<int, CString>			M_INT_STR;
typedef M_INT_STR::iterator				M_INT_STR_iter;
typedef std::vector<CString>			VEC_STR;
typedef VEC_STR::iterator				VEC_STR_iter;



class CLayerToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};


class CLayerView : public CDockablePane
{
public:
	CLayerView();
	virtual ~CLayerView();

	void AdjustLayout();
	void OnChangeVisualStyle();
	CListCtrl* GetView();
	//void SetMsgMgr(CMapXMsgProcBase_List* pMsgBaseList);
	void RefreshMsgList();

protected:
	void msgBaseListPatrol(BOOL bPatrol);
private:

protected:
	CLayerToolBar	 m_wndCOPToolBar;
	CViewList		 m_wndCOPView;
	CImageList		 m_COPViewImages;
	UINT			 m_nCurrSort;

	
	M_INT_STR		m_vnListNewAddNum;

private:
    bool m_refreshingOverlay=false;
	//CMapXMsgProcBase_List* m_pMsgBaseList;

	void FillClassView();


	// 재정의입니다.
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCOPAddMemberFunction();
	afx_msg void OnCOPAddMemberVariable();
	afx_msg void OnCOPDefinition();
	afx_msg void OnCOPProperties();
	afx_msg void OnNewFolder_COP();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);

	afx_msg void OnLvnItemchangedCopLayer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOverlayLabelEdited(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkListDebugDlg(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

