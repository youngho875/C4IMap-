#pragma once
//#include "DibMgr.h"
// CRasterMgr command target
#include <vector>
#include <map>
#include "DIBSectionLite.h"
#include "../inc/rtree.h"
#include "../LIBSrc/BMGLib/BMGGeoTIFF_DLL/BMGGeoTIFF.h"

//CIB관리
typedef struct
{
	CGRectGP GeoRect;
	CRect DpRect;
	CDIBSectionLite * pCib;		
	double  GeoLeft;
	double  GeoTop;
	double  GeoRight;
	double  GeoBottom;	

}S_CIB_DATA;

//TIFF관리
typedef struct
{
	CGRectGP GeoRect;
	CRect DpRect;
	CDIBSectionLite * pTiff;		
	double  GeoLeft;
	double  GeoTop;
	double  GeoRight;
	double  GeoBottom;	

}S_TIFF_DATA;

//TC2, TL2 관리
typedef struct
{
	CGRectGP GeoRect;
	CRect DpRect;
	CDIBSectionLite * pTiff;		

	double  GeoLeft;
	double  GeoTop;
	double  GeoRight;
	double  GeoBottom;	

}S_TCTL_DATA;


//CADRG를 관리
typedef struct
{
	CGRectGP GeoRect;
	CRect DpRect;
	CDIBSectionLite * pCadrg;		
	int No;
	double  GeoLeft;
	double  GeoTop;
	double  GeoRight;
	double  GeoBottom;	
	CString FilePath;
	BOOL Use;
}S_CADRG_DATA;


class CRasterMgr : public CObject
{
public:
	CRasterMgr();
	virtual ~CRasterMgr();
			
	CCoordMgr* mCoordMgr;
	CCoord mCoord;

	std::vector<CString> mvecFilePath;
		
	void DrawCIB(CDC *pDC, float fCenterX, float fCenterY, float fScale);	
	void InitCib();
	std::vector<CString> mvecCibPath;
	std::vector<S_CIB_DATA> mCibData;

	void InitTiff();
	void DrawTiff(CDC *pDC, float fCenterX, float fCenterY, float fScale,int nbrightness = 0,bool type = 0);			
	std::vector<CString> mvecTiffPath;
	std::vector<S_TIFF_DATA> mTiffData;

	CGRectGP ConvertUTMtoGP(double left, double top, double right, double bottom);
	
	void InitTCTL();
	void DrawTCTL(CDC *pDC, float fCenterX, float fCenterY, float fScale);		
	std::vector<CString> mvecTCTLPath;
	std::vector<S_TCTL_DATA> mTCTLData;
	
	
	//CADRG----------------------------------------------------------------------------
	//cadrg 파일 경로를 추출하는 함수(경로추출할때만 열어서 사용하세요)
	void ExtractCadrgInfo(CString ConfigPath );

	int mScale;

	//RasterMgr 초기화 할 때 rtree에 cadrg 경로를 입력한다.
	void InitCadrgInfoRtree();

	//rtree 관련 변수
	typedef CRTree< INT, DOUBLE, 2, DOUBLE > CadrgIdxType;
	CadrgIdxType mCadrg200;
	CadrgIdxType mCadrg100;
	CadrgIdxType mCadrg50;
	CadrgIdxType mCadrg25;
	CadrgIdxType mCadrg10;
	CadrgIdxType mCadrg5;
	//std::map< unsigned int, CString > mCadrgPath;
	std::map< unsigned int, S_CADRG_DATA > mCadrgInfo;

	//main view에서 drawing 명령을 받는다
	//void DrawCadrg( int Scale, double left, double top, double right, double bottom );
	void DrawCadrg(CDC *pDC, float fCenterX, float fCenterY, double Scale, double left, double top, double right, double bottom );

	//화면에 도시할 지도데이터 경로를 rtree에서 찾아온다
	void SearchCadrg( int Scale, double left, double top, double right, double bottom, std::vector<int> & Result );
	//std::vector<CString> mvecCadrgPath;
	std::vector< int > mIndex; 

	//지도데이터를 읽는다
	void ReadCadrg( int Type/*tif or tc2*/ ); 
	void ReadTCTL(CDC *pDC, float fCenterX, float fCenterY, float fScale);		
	void ReadTiff(CDC *pDC, float fCenterX, float fCenterY, float fScale);			
	
	//읽은 지도데이터 정보가 저장된다.
	std::vector<S_CADRG_DATA> mCadrgData;
	
	//읽은 지도데이터와 검색된 지도데이터 경로를 비교해 검색되지 않은 지도데이터는
	//삭제한다.
	void CloseCadrg();
	//소멸자에서 모두 삭제한다.
	void CloseCadrgAll();

	//cadrg를 지도위에 그린다.
	void DrawCadrg( int Type );
	//타입별로 아래 함수를 사용한다.(위에 선언되어 있음)
	//void DrawTCTL(CDC *pDC, float fCenterX, float fCenterY, float fScale);	 
	void DrawTiffofCadrg(CDC *pDC, float fCenterX, float fCenterY, float fScale);
	//CADRG----------------------------------------------------------------------------
};


