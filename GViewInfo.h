#pragma once


// GViewInfo.h : Declaration of the CGViewInfo

#ifndef __GVIEWINFO_H_
#define __GVIEWINFO_H_



/////////////////////////////////////////////////////////////////////////////
// CGViewInfo

/**
* 지도 도시 정보 (도시 지역, 지도 선택...) 관리 ActiveX Class
* @author ???
* @version 1.0
* @since 2005.11
**/

class CGRectGP;
class CGViewInfo 
{
public:
	CGViewInfo();
	~CGViewInfo();
	
	// Properties
public:
	void GetWorldOrigin(/*[out]*/ double* lon, /*[out]*/ double* lat);
	void SetWorldOrigin(/*[in]*/ double lon, /*[in]*/ double lat);
	void GetWorldExt(/*[out]*/ double* clon, /*[out]*/ double* clat);
	void SetWorldExt(/*[in]*/ double clon, /*[in]*/ double clat);
	void GetSelectedRange(/*[out]*/ double* left, /*[out]*/ double* top, /*[out]*/ double* right, /*[out]*/ double* bottom);
	void SetSelectedRange(/*[in]*/ double left, /*[in]*/ double top, /*[in]*/ double right, /*[in]*/ double bottom);
	void GetViewportRange(/*[out]*/ long* left, /*[out]*/ long* top, /*[out]*/ long* right, /*[out]*/ long* bottom);
	void SetViewportRange(/*[in]*/ long left, /*[in]*/ long top, /*[in]*/ long right, /*[in]*/ long bottom);
	long GetEllipsoid(/*[out]*/ long* ellip);
	void SetEllipsoid(/*[in]*/ long ellip);
	long GetMappingMode(/*[out]*/ long* mapMode);
	void SetMappingMode(/*[in]*/ long mapMode);
	double GetZoomFactor(/*[out]*/ double* zoom);
	void SetZoomFactor(/*[in]*/ double zoom);
	//void Set3D(/*[in]*/ BOOL b3D);
	//void Is3D(/*[out, retval]*/ BOOL* b3D);
	//void Get3DObject(/*[out]*/ IUnknown** pVal);
	
	//CGRectGP GetSelectedRange();

	// Methods
public:
	void InitInstance(/*[in]*/ UINT nMapMode, /*[in]*/ double left, /*[in]*/ double top, /*[in]*/ double right, /*[in]*/ double bottom);
	long GetActiveMode(/*[in]*/long* type);
	void SetActiveMode(/*[in]*/long type);
	void Set3DPattern(/*[in]*/long type);
	void Get3DPattern(/*[out]*/long* type);
	void SetMapKind(/*[in]*/long type);
	long GetMapKind(/*[out]*/long* type);
	void SetEncScale(/*[in]*/long encScale);
	long GetEncScale(/*[out]*/long* encScale);
	BOOL GetAutoScaleMode(/*[out]*/BOOL* bAutoScaleMode);
	void SetAutoScaleMode(/*[in]*/BOOL bAutoScaleMode);
	BOOL GetWrapAround(/*[out]*/BOOL* bWrapAround);
	void SetWrapAround(/*[in]*/BOOL bWrapAround);
	void Set3DObject(/*[in]*/ IUnknown* pVal);
	//void Copy(/*[in]*/ IGViewInfo* pifSrc);
	void GetGridToPoint(/*[out]*/long* x, /*[out]*/long* y);
	void SetGridToPoint(/*[in]*/long x, /*[in]*/long y);
	void GetViewingAngle(/*[out]*/double* fx, /*[out]*/double* fy, /*[out]*/double* fz);
	void SetViewingAngle(/*[in]*/double fx, /*[in]*/double fy, /*[in]*/double fz);
	long GetMapScale(/*[out]*/ long* pVal);
	void SetMapScale(/*[in]*/ long newVal);
	long GetZoomInMsg(/*[out]*/long* ZoomIn);
	void SetZoomInMsg(/*[in]*/long ZoomIn);
	long GetPrepareDC(/*[out]*/long* type);
	void SetPrepareDC(/*[in]*/long type);
	BOOL GetShiftFlag(/*[out]*/BOOL* bShift);
	BOOL SetShiftFlag(/*[in]*/BOOL bShift);
	void Serialize(/*[in]*/ IUnknown* pAr);
	//void IsEqual(/*[in]*/ IGViewInfo* pData, /*[out]*/ BOOL* bEqual);

	// Attributes
protected:
	// 도시 영역
	CGPointGP	m_WinOrg;
	CGSizeGP	m_WinExt;
	CGRectGP	m_selRect;		// 선택된 도시 영역
	UINT		m_nMapMode;		// Mapping Mode
	UINT		m_nEllipsoid;	// 타원체

	long		m_MapScale;		// 지도 축척
	long    	m_nEncScale;
	double		m_ZoomFactor;	// 확대/축소 배율

	long		m_nMapKind;     //지도종류를 저장한다. 육도:0, 공도:1
	BOOL		m_bShift;
	BOOL		m_bWrapAround;	//세계지도 도시 Wrap효과

	// 3차원 도시 정보
	BOOL		m_b3D;				// 2차원(FALSE), 3차원(TRUE)
	double		m_fX_Angle;			// X축 응시각도
	double		m_fY_Angle;			// Y축 응시각도
	double		m_fZ_Angle;			// Z축 응시각도
//???	BOOL		m_bTerrainChk;	// 지형중첩 설정
	CPoint		m_ptGridNum;		// 3차원 그리드의 간격 저장(default:200,200)
	long		m_n3DPattern;		//3차원 도시패턴[색상도, 음영도, 그물망...]
//???	EGM3DType	m_n3DModel;		// 3차원 지도의 형태


// Temporary attributes
protected:
	BOOL			m_bAutoScaleMode;		//지동축척변경
	CRect			m_ClientDPRect;			// View의 Client영역
	long			m_nZoomMsg;				//Flag No. 1: 확대
											//Flag No. 2: 축소
											//Flag No. 3: 영역확대		
	long			m_nPrepareDCMsg;
	long			m_nMode;

	/*
		CSize		m_ViewportExt;	// m_bArea가 TRUE일 때에만 유효함.
		EGGRIDType	m_nGridType;	// 그리드 선택용
		BOOL		m_bShift;	// 좌표에 의한 선택 또는 지명에 의한 선택시 화면중앙으로 이동
								// 해서 도시하기 위한 flag
		BOOL		m_bScroll;	// 화면에서 스크롤이 눌렸다.
		BOOL		m_bSimple;	// 단순도시(TRUE), 복원도시(FALSE)	==> 지도로 이동

		BOOL		m_bArea;	// 소축척에의한 선택여부
		// 투영변환에 의한 도시시 짤리는 문제 해결
		CGRectGP	m_AlignRect;	// Align한 rect
	*/

	// Operations
protected:

};

#endif //__GVIEWINFO_H_


