// CityFilterDlg.cpp : implementation file
//
#pragma once
#include "pch.h"
#include "MapControl.h"
#include "ViewingFilterDlg.h"
//#include "afxdialogex.h"


// CViewingFilterDlg dialog

IMPLEMENT_DYNAMIC(CViewingFilterDlg, CPaneDialog)

CViewingFilterDlg::CViewingFilterDlg()
{
	
}

CViewingFilterDlg::~CViewingFilterDlg()
{
}

void CViewingFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_CITYFILTER_TREE, m_ViewingFilterTreeCtrl);
}


BEGIN_MESSAGE_MAP(CViewingFilterDlg, CPaneDialog)
	ON_WM_SIZE()
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
END_MESSAGE_MAP()


// CViewingFilterDlg message handlers
void CViewingFilterDlg::AdjustWidth(int cx, int cy, UINT uiCtrlID)
{
	const int nMargin = 0;

	CWnd* pWnd = GetDlgItem(uiCtrlID);
	if (pWnd == NULL)
	{
		return;
	}

	CRect rect;
	pWnd->GetWindowRect(rect);
	ScreenToClient(rect);

	rect.right = cx - nMargin;
	rect.bottom	= cy - nMargin;

	pWnd->SetWindowPos(NULL, -1, -1, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void CViewingFilterDlg::OnSize(UINT nType, int cx, int cy)
{
	CPaneDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	//AdjustWidth(cx, cy, IDC_CITYFILTER_TREE);
}

BOOL CViewingFilterDlg::OnInitDialog()
{
	UpdateData(FALSE);

	//m_ViewingFilterTreeCtrl.Initialize(NULL);
	InitializeViewFilter();
	m_ViewingFilterTreeCtrl.Initialize(NULL);

	return TRUE;
}

LRESULT CViewingFilterDlg::HandleInitDialog(WPARAM w, LPARAM l)
{
	OnInitDialog();

	return CPaneDialog::HandleInitDialog(w,l);
}


void CViewingFilterDlg::InitializeViewFilter()
{
	InitializeViewFilterMap();
	InitializeViewOption();
	m_ViewingFilterTreeCtrl.m_pOptions = &m_OptionsView;
	m_OptionsView.BeginTrans();
	

}
//////////////////////////////////

void CViewingFilterDlg::InitializeViewOption()
{	// 적부대 전시필터 설정 초기화
	
	// 대분류
	//m_OptionsView.SetOptionItem("도시요소 필터", IDB_ITEMS, IDB_ITEMS);
	
	// 적부대
	//m_OptionsView.SetOption(CString("화생방 예상오염도"), TOPTION_ITEM::OF_FALSE);
	//m_OptionsView.SetOptionItem("화생방 예상오염도", CString("\t화생방 예상오염도").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	////m_ViewFlags.setFlag(DisplayElementFilter::CM_POLLUTON_ALL);
	//
	//m_OptionsView.SetOption("예상오염도_1", TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOptionItem("예상오염도_1", CString("\t\t예상오염도_1").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART);
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_POL1);
	//
	//m_OptionsView.SetOption("예상오염도_2", TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOptionItem("예상오염도_2", CString("\t\t예상오염도_2").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_POL2);
	//
	//m_OptionsView.SetOption("예상오염도_3", TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOptionItem("예상오염도_3", CString("\t\t예상오염도_3").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_POL3);
	//
	//m_OptionsView.SetOption(CString("화생방 예상오염도"), TOPTION_ITEM::OF_FALSE);
	//m_OptionsView.SetOptionItem("화생방 예상오염도", CString("\t화생방 예상오염도").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	////m_ViewFlags.setFlag(DisplayElementFilter::CM_POLLUTION_ALL_1);

	//m_OptionsView.SetOption("화학대피구역", TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOptionItem("화학대피구역", CString("\t\t화학대피구역").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_POL4);
	//
	//m_OptionsView.SetOption("세부격자", TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOptionItem("세부격자", CString("\t\t세부격자").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_POL5);
	//
	//m_OptionsView.SetOption("정밀", TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOptionItem("정밀", CString("\t\t정밀").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_MINUTE);
	
	//m_OptionsView.SetOption(CString("이동전력"), 0);
	//m_OptionsView.SetOptionItem("이동전력", CString("\t이동전력").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	
	/**************************************************************************
		이동전력에 대한 트리를 추가하여야 함.
	***************************************************************************/
	//m_OptionsView.SetOption("(적)연대", TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOptionItem("(적)연대", CString("\t\t(적)연대").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_ViewFlags.setFlag(MimsIDFilter::MIMS_REGIMENT);
	//
	//
	//m_OptionsView.SetOption("(적)대대", TOPTION_ITEM::OF_FALSE);
	//m_OptionsView.SetOptionItem("(적)대대", CString("\t\t(적)대대").GetBuffer(1), 
	//	TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_ViewFlags.setFlag(MimsIDFilter::MIMS_SQUADRON); 
/*
	m_OptionsView.SetOption(CString("배경지도"), 0);
	m_OptionsView.SetOptionItem("배경지도", CString("\t배경지도").GetBuffer(1), 
		TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
*/	
	/**************************************************************************
		배경지도 트리를 추가하여야 함.
	***************************************************************************/	
	m_OptionsView.SetOption("지도"			, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("1지대경계선"	, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("2지대경계선"	, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("3지대경계선"	, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("기지외곽선	"	, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("출입문"			, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("전투진지"		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("대공방어포상"	, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("활주로" 		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("비상활주로"		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("유도로"			, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("일반도로"		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("주기장"			, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("격납고"			, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("탄약고"			, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("유류고"			, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("작전지휘소"		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("비행대대"		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("정비시설"		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("통신시설"		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("보급시설"		, TOPTION_ITEM::OF_TRUE);		

	m_OptionsView.SetOption("1지대경계선"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("2지대경계선"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("3지대경계선"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("기지외곽선"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("출입문"			, TOPTION_ITEM::OF_TRUE); 
	m_OptionsView.SetOption("대공방어포상"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("활주로" 		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("비상활주로"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("유도로"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("일반도로"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("주기장"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("격납고"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("탄약고"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("유류저장소"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("작전지휘소"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("비행대대"		, TOPTION_ITEM::OF_TRUE); 
	m_OptionsView.SetOption("통신시설"		, TOPTION_ITEM::OF_TRUE); 


	m_OptionsView.SetOption("기지방호"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("외곽방어대구역"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("지역방어대구역"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("전투장갑차"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("무장순찰차"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("초소"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("특작군"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("교전"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("영창"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("임시포로수집소"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("감제고지"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("목진지"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("관측소"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("검문소"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("특임반"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("박격포(적)"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("전투진지"		, TOPTION_ITEM::OF_TRUE);
	//m_OptionsView.SetOption("텍스트"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("객체명 "			, TOPTION_ITEM::OF_TRUE);
	
	m_OptionsView.SetOption("화생방"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("화학대피구역"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("오염도"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("예측오염도"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("정밀오염도"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("실오염도"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("화생방정찰차"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("제독차"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("소방차"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("구급차"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("EOD"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("집단보호시설"	, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("화학탐지대"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("자동경보기"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("제독소"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("주요건물"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("화재"			, TOPTION_ITEM::OF_TRUE);	
	m_OptionsView.SetOption("폭파"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("불발탄"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("GAS"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("복구"			, TOPTION_ITEM::OF_TRUE);	
	//m_OptionsView.SetOption("텍스트"			, TOPTION_ITEM::OF_TRUE); 
	m_OptionsView.SetOption("객체명"			, TOPTION_ITEM::OF_TRUE);	
	
	m_OptionsView.SetOption("대공방어"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("8방향"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("페트리어트"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("발칸"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("신궁"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("미스트랄"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("고속기(적)"		, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("저속기(적)"		, TOPTION_ITEM::OF_TRUE);
	
	m_OptionsView.SetOption("공통"			, TOPTION_ITEM::OF_TRUE);
	m_OptionsView.SetOption("텍스트"			, TOPTION_ITEM::OF_TRUE);
	




/*
	m_OptionsView.SetOptionItem("지도"			, CString("\t지도"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("1지대경계선"	, CString("\t\t1지대경계선"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("2지대경계선"	, CString("\t\t2지대경계선"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("3지대경계선"	, CString("\t\t3지대경계선"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("기지외곽선	"	, CString("\t\t기지외곽선	"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("출입문"			, CString("\t\t출입문"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("전투진지"		, CString("\t\t전투진지"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("대공방어포상"	, CString("\t\t대공방어포상"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("활주로" 		, CString("\t\t활주로" 		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("비상활주로"		, CString("\t\t비상활주로"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("유도로"			, CString("\t\t유도로"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("일반도로"		, CString("\t\t일반도로"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("주기장"			, CString("\t\t주기장"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("격납고"			, CString("\t\t격납고"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("탄약고"			, CString("\t\t탄약고"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("유류고"			, CString("\t\t유류고"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("작전지휘소"		, CString("\t\t작전지휘소"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("비행대대"		, CString("\t\t비행대대"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("정비시설"		, CString("\t\t정비시설"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("통신시설"		, CString("\t\t통신시설"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("보급시설"		, CString("\t\t보급시설"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
						*/			
	
	m_OptionsView.SetOptionItem("지도"			, CString("\t지도"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("1지대경계선"	, CString("\t\t1지대경계선"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("2지대경계선"	, CString("\t\t2지대경계선"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("3지대경계선"	, CString("\t\t3지대경계선"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("기지외곽선"	, CString("\t\t기지외곽선"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("출입문"			, CString("\t\t출입문"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("대공방어포상"	, CString("\t\t대공방어포상"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("활주로" 		, CString("\t\t활주로" 		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("비상활주로"		, CString("\t\t비상활주로"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("유도로"			, CString("\t\t유도로"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("일반도로"		, CString("\t\t일반도로"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("주기장"			, CString("\t\t주기장"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("격납고"			, CString("\t\t격납고"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("탄약고"			, CString("\t\t탄약고"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("유류저장소"			, CString("\t\t유류저장소"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("작전지휘소"		, CString("\t\t작전지휘소"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("비행대대"		, CString("\t\t비행대대"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("통신시설"		, CString("\t\t통신시설"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
						

	m_OptionsView.SetOptionItem("기지방호"		, CString("\t기지방호"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("외곽방어대구역"	, CString("\t\t외곽방어대구역"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("지역방어대구역"	, CString("\t\t지역방어대구역"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("전투장갑차"		, CString("\t\t전투장갑차"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("무장순찰차"		, CString("\t\t무장순찰차"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("초소"			, CString("\t\t초소"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("특작군"			, CString("\t\t특작군"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("교전"			, CString("\t\t교전"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("영창"			, CString("\t\t영창"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("임시포로수집소"	, CString("\t\t임시포로수집소"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("감제고지"		, CString("\t\t감제고지"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("목진지"			, CString("\t\t목진지"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("관측소"			, CString("\t\t관측소"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("검문소"			, CString("\t\t검문소"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("특임반"			, CString("\t\t특임반"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("박격포(적)"		, CString("\t\t박격포(적)"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("전투진지"		, CString("\t\t전투진지"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_OptionsView.SetOptionItem("텍스트"			, CString("\t\t텍스트"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("객체명 "			, CString("\t\t객체명 "			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
													
	m_OptionsView.SetOptionItem("화생방"			, CString("\t화생방"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("화학대피구역"	, CString("\t\t화학대피구역"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("오염도"			, CString("\t\t오염도"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("예측오염도"		, CString("\t\t\t예측오염도"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("정밀오염도"		, CString("\t\t\t정밀오염도"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("실오염도"		, CString("\t\t\t실오염도"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("화생방정찰차"	, CString("\t\t화생방정찰차"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("제독차"			, CString("\t\t제독차"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("소방차"			, CString("\t\t소방차"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("구급차"			, CString("\t\t구급차"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("EOD"			, CString("\t\tEOD"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("집단보호시설"	, CString("\t\t집단보호시설"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("화학탐지대"		, CString("\t\t화학탐지대"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("자동경보기"		, CString("\t\t자동경보기"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("제독소"			, CString("\t\t제독소"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("주요건물"		, CString("\t\t주요건물"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("화재"			, CString("\t\t화재"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("폭파"			, CString("\t\t폭파"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("불발탄"			, CString("\t\t불발탄"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("GAS"			, CString("\t\tGAS"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("복구"			, CString("\t\t복구"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	//m_OptionsView.SetOptionItem("텍스트"			, CString("\t\t텍스트"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("객체명"			, CString("\t\t객체명"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
														
	m_OptionsView.SetOptionItem("대공방어"		, CString("\t대공방어"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("8방향"			, CString("\t\t8방향"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("페트리어트"		, CString("\t\t페트리어트"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("발칸"			, CString("\t\t발칸"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("신궁"			, CString("\t\t신궁"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("미스트랄"		, CString("\t\t미스트랄"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("고속기(적)"		, CString("\t\t고속기(적)"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("저속기(적)"		, CString("\t\t저속기(적)"	).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
													
	m_OptionsView.SetOptionItem("공통"			, CString("\t공통"			).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 
	m_OptionsView.SetOptionItem("텍스트"			, CString("\t\t텍스트"		).GetBuffer(1), TOPTION_ITEM::OF_TRUE, IDB_CHECKON, IDB_CHECKOFF,IDB_ON_PART); 











	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER1 				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER2 			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER3 				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER4 				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER5 				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER6 			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER7 				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER8 			);
 	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER9 				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER10				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER11			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER12			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER13			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER14			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER15			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER16				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER17				);/*
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER18					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER19					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_MAP_LAYER20					);*/
	
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE						);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_OUTAREA				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_DEFAREA					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_ARMORED_UNIT		);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_ARMED_UNIT			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_GUARD					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_SPT					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_ENGAGE				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_JAIL					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_TEMPPRISON				);
 	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_SEEHIGH				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_SEEGUARD				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_SEEPOINT				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_INSPECT				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_SPECIAL_PART				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_ENEMY_MORTAR				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_BATTLE_CAMP				);
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_TEXT					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_BASE_OBJNAME				);
	
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC						);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_SHELTERAREA			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_POLLUTION			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_POLLUTION_GUESS			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_POLLUTION_DETAIL			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_POLLUTION_REAL			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_DETEC_UNIT				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_DETOXIC_UNIT				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_FIRE_UNIT				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_FIRSTAID_UNIT			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_EOD						);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_PROTECT					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_DETECT					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_ALARM					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_DETOXIC				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_MAINBUILDING			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_FIRE					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_EXPLOSION			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_UNEXPBOMB			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_GAS					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_REPAIR				);
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_TEXT					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_NBC_OBJNAME				);
	
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF					);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_PARTAREA			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_ADB_PET			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_ADB_VAL			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_ADB_SIN			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_ADB_MIS			);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_HIGH				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_LOW				);
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_HELICOPTER		 );
	//m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_TEXT			 );
	m_ViewFlags.setFlag(DisplayElementFilter::CM_AIRDEF_OBJNAME			 );
																	
	m_ViewFlags.setFlag(DisplayElementFilter::CM_COMMON					 );
	m_ViewFlags.setFlag(DisplayElementFilter::CM_COMMON_TEXT			 );
																		
	m_OptionsView.SetOwnerWnd(this); 
}


void CViewingFilterDlg::InitializeViewFilterMap()
{
	
	m_ViewFilterMap.InitHashTable( 257 );
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP					,	DisplayElementFilter::CM_MAP					);				
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER1 			,	DisplayElementFilter::CM_MAP_LAYER1 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER2 			,	DisplayElementFilter::CM_MAP_LAYER2 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER3 			,	DisplayElementFilter::CM_MAP_LAYER3 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER4 			,	DisplayElementFilter::CM_MAP_LAYER4 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER5 			,	DisplayElementFilter::CM_MAP_LAYER5 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER6 			,	DisplayElementFilter::CM_MAP_LAYER6 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER7 			,	DisplayElementFilter::CM_MAP_LAYER7 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER8 			,	DisplayElementFilter::CM_MAP_LAYER8 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER9 			,	DisplayElementFilter::CM_MAP_LAYER9 			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER10			,	DisplayElementFilter::CM_MAP_LAYER10			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER11			,	DisplayElementFilter::CM_MAP_LAYER11			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER12			,	DisplayElementFilter::CM_MAP_LAYER12			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER13			,	DisplayElementFilter::CM_MAP_LAYER13			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER14			,	DisplayElementFilter::CM_MAP_LAYER14			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER15			,	DisplayElementFilter::CM_MAP_LAYER15			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER16			,	DisplayElementFilter::CM_MAP_LAYER16			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER17			,	DisplayElementFilter::CM_MAP_LAYER17			);
	//m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER18			,	DisplayElementFilter::CM_MAP_LAYER18			);
	//m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER19			,	DisplayElementFilter::CM_MAP_LAYER19			);
	//m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_MAP_LAYER20			,	DisplayElementFilter::CM_MAP_LAYER20			);

	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE					,	DisplayElementFilter::CM_BASE					);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_OUTAREA			,	DisplayElementFilter::CM_BASE_OUTAREA			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_DEFAREA			,	DisplayElementFilter::CM_BASE_DEFAREA			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_ARMORED_UNIT		,	DisplayElementFilter::CM_BASE_ARMORED_UNIT			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_ARMED_UNIT		,	DisplayElementFilter::CM_BASE_ARMED_UNIT		);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_GUARD				,	DisplayElementFilter::CM_BASE_GUARD				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_SPT				,	DisplayElementFilter::CM_BASE_SPT					);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_ENGAGE			,	DisplayElementFilter::CM_BASE_ENGAGE			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_JAIL				,	DisplayElementFilter::CM_BASE_JAIL				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_TEMPPRISON		,	DisplayElementFilter::CM_BASE_TEMPPRISON			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_SEEHIGH			,	DisplayElementFilter::CM_BASE_SEEHIGH				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_SEEGUARD			,	DisplayElementFilter::CM_BASE_SEEGUARD			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_SEEPOINT			,	DisplayElementFilter::CM_BASE_SEEPOINT				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_INSPECT			,	DisplayElementFilter::CM_BASE_INSPECT				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_SPECIAL_PART		,	DisplayElementFilter::CM_BASE_SPECIAL_PART			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_ENEMY_MORTAR		,	DisplayElementFilter::CM_BASE_ENEMY_MORTAR			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_BATTLE_CAMP		,	DisplayElementFilter::CM_BASE_BATTLE_CAMP			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_TEXT				,	DisplayElementFilter::CM_BASE_TEXT				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_BASE_OBJNAME			,	DisplayElementFilter::CM_BASE_OBJNAME				);

	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC					,	DisplayElementFilter::CM_NBC					);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_SHELTERAREA		,	DisplayElementFilter::CM_NBC_SHELTERAREA		);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_POLLUTION			,	DisplayElementFilter::CM_NBC_POLLUTION			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_POLLUTION_GUESS	,	DisplayElementFilter::CM_NBC_POLLUTION_GUESS	);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_POLLUTION_DETAIL	,	DisplayElementFilter::CM_NBC_POLLUTION_DETAIL	);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_POLLUTION_REAL		,	DisplayElementFilter::CM_NBC_POLLUTION_REAL		);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_DETEC_UNIT			,	DisplayElementFilter::CM_NBC_DETEC_UNIT			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_DETOXIC_UNIT		,	DisplayElementFilter::CM_NBC_DETOXIC_UNIT		);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_FIRE_UNIT			,	DisplayElementFilter::CM_NBC_FIRE_UNIT			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_FIRSTAID_UNIT		,	DisplayElementFilter::CM_NBC_FIRSTAID_UNIT		);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_EOD				,	DisplayElementFilter::CM_NBC_EOD					);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_PROTECT			,	DisplayElementFilter::CM_NBC_PROTECT			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_DETECT				,	DisplayElementFilter::CM_NBC_DETECT				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_ALARM				,	DisplayElementFilter::CM_NBC_ALARM				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_DETOXIC			,	DisplayElementFilter::CM_NBC_DETOXIC				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_MAINBUILDING		,	DisplayElementFilter::CM_NBC_MAINBUILDING		);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_FIRE				,	DisplayElementFilter::CM_NBC_FIRE				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_EXPLOSION			,	DisplayElementFilter::CM_NBC_EXPLOSION			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_UNEXPBOMB			,	DisplayElementFilter::CM_NBC_UNEXPBOMB			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_GAS				,	DisplayElementFilter::CM_NBC_GAS				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_REPAIR				,	DisplayElementFilter::CM_NBC_REPAIR					);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_TEXT				,	DisplayElementFilter::CM_NBC_TEXT				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_NBC_OBJNAME			,	DisplayElementFilter::CM_NBC_OBJNAME			);

	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF					,	DisplayElementFilter::CM_AIRDEF					);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_PARTAREA		,	DisplayElementFilter::CM_AIRDEF_PARTAREA		);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_ADB_PET			,	DisplayElementFilter::CM_AIRDEF_ADB_PET				);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_ADB_VAL			,	DisplayElementFilter::CM_AIRDEF_ADB_VAL			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_ADB_SIN			,	DisplayElementFilter::CM_AIRDEF_ADB_SIN			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_ADB_MIS			,	DisplayElementFilter::CM_AIRDEF_ADB_MIS			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_HIGH			,	DisplayElementFilter::CM_AIRDEF_HIGH			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_LOW				,	DisplayElementFilter::CM_AIRDEF_LOW					);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_HELICOPTER		,	DisplayElementFilter::CM_AIRDEF_HELICOPTER		);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_TEXT			,	DisplayElementFilter::CM_AIRDEF_TEXT			);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_AIRDEF_OBJNAME			,	DisplayElementFilter::CM_AIRDEF_OBJNAME			);

	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_COMMON					,	DisplayElementFilter::CM_COMMON					);
	m_ViewFilterMap.SetAt(DisplayElementFilter::T_CM_COMMON_TEXT			,	DisplayElementFilter::CM_COMMON_TEXT			);

} 

