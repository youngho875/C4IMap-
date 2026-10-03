#include "pch.h"

#include "C4IMap.h"
#include "RasterMgr.h"
#include "../LIBSrc/GeoDC/Coord.h"
#include "../LIBSrc/GeoDC/CoordMgr.h"
#include "xtiffio.h"



CRasterMgr::CRasterMgr()
{
	mCoordMgr = GP_COORDMGR;
	mCoord.initialize_adaptation_data(); 
	//mCoord.initialize_coord_conversions();
	mScale = 0;
	InitCadrgInfoRtree();
	
}

CRasterMgr::~CRasterMgr()
{		
	int cntCib = mCibData.size();
	for( int cnt = 0;  cnt <  cntCib; ++cnt )
	{
		delete mCibData.at(cnt).pCib;
	}		
	int cntTiff = mTiffData.size();
	for( int cnt = 0;  cnt <  cntTiff; ++cnt )
	{
		delete mTiffData.at(cnt).pTiff;
	}
	CloseCadrgAll();
}


int cnt_ = 1;
void CRasterMgr::InitTiff()
{
	if( cnt_ != 1 )  
		return;

	++cnt_;
	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\AD_우면.tif");
	//		
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\TMP\\000AX012.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\TMP\\000AY012.tif");		
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\TMP\\000AZ012.tif");
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\TMP\\000B0012.tif");

	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\27211ne2.tif");
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cadrg_200.tif");
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\27211ne_2.tif");
	
	mvecTiffPath.push_back("D:\\mapdata\\valid.geotiff.tif");

	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\수원비행장2.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\영종포대.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\망일.tif");	


	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_1_1.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_1_2.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_1_3.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_2_1.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_2_2.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_2_3.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_3_1.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_3_2.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_3_3.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_4_1.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_4_2.tif");	
	//mvecTiffPath.push_back("C:\\전장상황도\\Adapter\\map\\Tiff\\cell29_13_1_8bit_4_3.tif");	 

	int cntTiff = mvecTiffPath.size();
	for( int cnt = 0;  cnt <  cntTiff; ++cnt )
	{	
		TIFF *image = (TIFF*)0;
		UINT32 xsize, ysize;
		UINT32 numTiePoints, numPixelPoints;
		
		double x1, y1, x2, y2;
		double *pGeoPoints = NULL;
		double *pGeoPixelScale = NULL;

		if((image = XTIFFOpen(mvecTiffPath.at(cnt), "r")) == NULL)	return;

		// Find the width and height of the image 
		TIFFGetField(image, TIFFTAG_IMAGEWIDTH, &xsize); 
		TIFFGetField(image, TIFFTAG_IMAGELENGTH, &ysize); 

		TIFFGetField(image, TIFFTAG_GEOTIEPOINTS, &numTiePoints, &pGeoPoints);
		TIFFGetField(image, TIFFTAG_GEOPIXELSCALE, &numPixelPoints, &pGeoPixelScale);
		
		x1 = pGeoPoints[3];
		y1 = pGeoPoints[4];
		x2 = x1 + xsize*pGeoPixelScale[0];
		y2 = y1 - ysize*pGeoPixelScale[1];

		 
		XTIFFClose(image);
	
		// Image Loading
		CDIBSectionLite * ptiff_tmp;		
		GeoTIFFInfoStruct* info = new GeoTIFFInfoStruct();
		HBITMAP hBitmap = CreateBitmapFromGeoTIFFile(mvecTiffPath.at(cnt), (GeoTIFFInfoStruct*)info, 1);
		
		if(hBitmap)
		{
			ptiff_tmp = new CDIBSectionLite();
			ptiff_tmp->SetBitmap(hBitmap);
			DeleteObject(hBitmap);
		 			
			//double  GeoLeft;
			//double  GeoTop;
			//double  GeoRight;
			//double  GeoBottom;			 
			//	
			//mCoord.degrees_lat_long_to_x_y(y1, x1, &GeoLeft, &GeoTop);		
			//mCoord.degrees_lat_long_to_x_y(y2, x2, &GeoRight, &GeoBottom);
			
			S_TIFF_DATA tiff_data_tmp;					
	
			//tiff_data_tmp.pTiff = ptiff_tmp;
			//tiff_data_tmp.GeoLeft = GeoLeft;
			//tiff_data_tmp.GeoTop = GeoTop;
			//tiff_data_tmp.GeoRight = GeoRight;
			//tiff_data_tmp.GeoBottom = GeoBottom;
			tiff_data_tmp.pTiff = ptiff_tmp;
			tiff_data_tmp.GeoLeft = x1;
			tiff_data_tmp.GeoTop = y1;
			tiff_data_tmp.GeoRight = x2;
			tiff_data_tmp.GeoBottom = y2;


			mTiffData.push_back(tiff_data_tmp);		 
		}	
		delete (GeoTIFFInfoStruct*)info;		
	}
}


/** UTM 좌표를 경위도 좌표로 변환, 한반도 52 zone을 기준으로 변환 */
CGRectGP CRasterMgr::ConvertUTMtoGP(double left, double top, double right, double bottom)
{
	CGRectGP rectGP;
	CGGeoTrans trans(EWGE);
	CGPointGP gpInit, gpLast;

	CGPointUTM utmInit(left, top, '4', 'S');
	CGPointUTM utmLast(right, bottom, '4', 'S');

	trans.toPointGP(&gpInit, &utmInit);
	trans.toPointGP(&gpLast, &utmLast);

	rectGP.m_R.left = gpInit.m_P.lon;
	rectGP.m_R.top = gpInit.m_P.lat;
	rectGP.m_R.right = gpLast.m_P.lon;
	rectGP.m_R.bottom = gpLast.m_P.lat;

	return rectGP;
}


int cnt = 1;
void CRasterMgr::InitCib()
{	
	if( cnt != 1 )  return;
	++cnt;

	mvecFilePath.push_back("C:\\AFC4I\\map\\satellite\\1m\\37~38\\nj525g\\RPF\\NJ525G1\\05BVND5A.I42");
	mvecFilePath.push_back("C:\\AFC4I\\map\\satellite\\1m\\37~38\\nj525g\\RPF\\NJ525G1\\05BVNV2A.I42");

	int cntCib = mvecFilePath.size();
	for( int cnt = 0;  cnt <  cntCib; ++cnt )
	{
			CDIBSectionLite * cib_tmp = new CDIBSectionLite();
			S_CIB_DATA cib_data_tmp;					
			BITMAPINFO pBmi;
			bool isok;
			byte *buf = 0x00;
			CGRectGP GeoRect;

			buf = cib_tmp->ReadCIB(mvecFilePath.at(cnt),&pBmi, &isok, &GeoRect);
			cib_tmp->SetBitmap(&pBmi, buf);
									
			double  GeoLeft;
			double  GeoTop;
			double  GeoRight;
			double  GeoBottom;			 

			mCoord.degrees_lat_long_to_x_y(GeoRect.m_R.top, GeoRect.m_R.left, &GeoLeft, &GeoTop);		
			mCoord.degrees_lat_long_to_x_y(GeoRect.m_R.bottom, GeoRect.m_R.right, &GeoRight, &GeoBottom);
			
			cib_data_tmp.pCib = cib_tmp;
			cib_data_tmp.GeoRect = GeoRect;
			cib_data_tmp.GeoLeft = GeoLeft;
			cib_data_tmp.GeoTop = GeoTop;
			cib_data_tmp.GeoRight = GeoRight;
			cib_data_tmp.GeoBottom = GeoBottom;

			mCibData.push_back(cib_data_tmp);
			delete buf;
	}		 
}

void CRasterMgr::DrawTiff(CDC *pDC, float fCenterX, float fCenterY, float fScale,int nbrightness,bool type)
{
	InitTiff();
	int cntTiff = mTiffData.size();
	
	for( int cnt = 0; cnt < cntTiff; ++cnt )
	{					
		double  GeoLeft = mTiffData.at(cnt).GeoLeft;			
		double  GeoTop = mTiffData.at(cnt).GeoTop;				
		double  GeoRight = mTiffData.at(cnt).GeoRight;						
		double  GeoBottom = mTiffData.at(cnt).GeoBottom;						 

		mCoordMgr->WorldLonLat2ScreenXY( GeoLeft, GeoTop, &GeoLeft, &GeoTop,  fCenterX, fCenterY, fScale); 
		mCoordMgr->WorldLonLat2ScreenXY( GeoRight, GeoBottom, &GeoRight, &GeoBottom,  fCenterX, fCenterY, fScale); 

		pDC->SelectStockObject(WHITE_BRUSH);
		pDC->Rectangle(GeoLeft, GeoTop, GeoRight, GeoBottom );
		
		mTiffData.at(cnt).pTiff->DrawTiff(pDC, GeoLeft, GeoTop, GeoRight, GeoBottom ,nbrightness,type);
	}
}

void CRasterMgr::DrawCIB(CDC *pDC, float fCenterX, float fCenterY, float fScale)
{
	InitCib(); 
	int cntCib = mCibData.size();
	for( int cnt = 0; cnt < cntCib; ++cnt )
	{					
		double  GeoLeft = mCibData.at(cnt).GeoLeft;			
		double  GeoTop = mCibData.at(cnt).GeoTop;				
		double  GeoRight = mCibData.at(cnt).GeoRight;						
		double  GeoBottom = mCibData.at(cnt).GeoBottom;						 

		mCoordMgr->WorldXY2ScreenXY( GeoLeft, GeoTop, &GeoLeft, &GeoTop,  fCenterX, fCenterY, fScale); 
		mCoordMgr->WorldXY2ScreenXY( GeoRight, GeoBottom,&GeoRight, &GeoBottom,  fCenterX, fCenterY, fScale); 
		
		mCibData.at(cnt).pCib->DrawCibEX(pDC, GeoLeft, GeoTop, GeoRight, GeoBottom );		 
	}
}

void CRasterMgr::InitTCTL()
{


	/*
	1;
	100만;
	00035012.TIF;
	C:\전장상황도\Adapter\CADRG\MAP\100만\00035012.tif;
	113.299492;
	33.103448;
	116.040602;
	31.034483
	*//*
			{ 
			 
				FILE * stream;
				char line[200];
				if( (stream = fopen("C:\\전장상황도\\Adapter\\Map\\CadrgConfig.txt","r")) != NULL )
				{
					while( fgets( line, 200, stream ) != NULL)
					{		
						CString values = line;
						CString seperator = ",";
						int position = 0;
						S_TCTL_DATA Tctl;	 

						CString No = values.Tokenize(seperator, position);
						CString Scale = values.Tokenize(seperator, position);
						CString FileName = values.Tokenize(seperator, position);
						CString FullPath = values.Tokenize(seperator, position);
						CString GeoLeft = values.Tokenize(seperator, position);
						CString GeoTop = values.Tokenize(seperator, position);
						CString GeoRight = values.Tokenize(seperator, position);
						CString GeoBottom = values.Tokenize(seperator, position);

 
						
						Tctl.GeoLeft = atof(GeoLeft.GetString());
						Tctl.GeoTop = atof(GeoTop.GetString());
						Tctl.GeoRight = atof(GeoRight.GetString());
						Tctl.GeoBottom = atof(GeoBottom.GetString());
					
						mTCTLData.push_back(Tctl);
					}
					fclose( stream );
				}

			}



	CFileFind find;
	BOOL bRet = find.FindFile(strFileName);
	if(bRet == FALSE)
		return ;

	BITMAPINFO pBmi;
	bool isok;
	byte *buf = 0x00;

	CGRectGP realrect;
	CDIBSectionLite *pImageBitmap = new CDIBSectionLite;

	buf = pImageBitmap->ReadCADRG(strFileName, &pBmi, &isok, &realrect);

	if(isok == FALSE)
	{
		delete pImageBitmap;
		if(buf != NULL)
			delete buf;
		return FALSE;
	}

	pImageBitmap->SetBitmap(&pBmi, buf);
	pRasterMap->m_pRasterMap = pImageBitmap;
	pRasterMap->m_bIsRasterDataOpen = TRUE;
	pRasterMap->m_gpRealRect = realrect;

	if(buf != NULL)
		delete buf;

	return TRUE;*/
}

void CRasterMgr::DrawTCTL(CDC *pDC, float fCenterX, float fCenterY, float fScale)
{
	int CntCadrg = mCadrgData.size();
	for( int cnt = 0;  cnt < CntCadrg; ++cnt )
	{
		double  GeoLeft = mCadrgData.at(cnt).GeoLeft;			
		double  GeoTop = mCadrgData.at(cnt).GeoTop;				
		double  GeoRight = mCadrgData.at(cnt).GeoRight;						
		double  GeoBottom = mCadrgData.at(cnt).GeoBottom;	


		
		CGPointGP TopLeftGP, BottomRightGP;

		TopLeftGP.m_P.lat  = GeoTop;
		TopLeftGP.m_P.lon  = GeoLeft;
		BottomRightGP.m_P.lat  = GeoBottom;
		BottomRightGP.m_P.lon  = GeoRight; 

	 
	//	::DTWgsToTok(&TopLeftGP, &TopLeftGP, 1);
	//	::DTWgsToTok(&BottomRightGP, &BottomRightGP, 1);
	//	GeoLeft = TopLeftGP.m_P.lon;
	//	GeoTop = TopLeftGP.m_P.lat;
	//	GeoRight = BottomRightGP.m_P.lon;
	//	GeoBottom = BottomRightGP.m_P.lat;
//

			
		CGGeoProj	GeoProject(EWGE);
		POINTPLANE Plane;
			
		CGPointGP Points;
		Points.m_P.lat = GeoTop;
		Points.m_P.lon = GeoLeft;
		GeoProject.InverseLCC(&Points, &Plane, 1);

	
	 
	/*	
		CGPointGP Points;
		Points.m_P.lat = GeoTop;
		Points.m_P.lon = GeoLeft;
		GeoProject.ForwardUTM(&Plane,  (CGPointGP*)&Points);

		Points.m_P.lat = GeoTop;
		Points.m_P.lon = GeoLeft;
		GeoProject.InverseUTM((CGPointUTM*)&Points, &Plane, 2);*/
		 

		//double  GeoLeft2;
		//double  GeoTop2;	
		//double  GeoRight2;
		//double  GeoBottom2; 


		CPoint	ScreenXY;
		CPoint	ScreenXY2;
		mCoordMgr->WorldLonLat2ScreenXY(GeoLeft	,GeoTop, &ScreenXY.x, &ScreenXY.y,  fCenterX, fCenterY, fScale); 
		mCoordMgr->WorldLonLat2ScreenXY(GeoRight ,GeoBottom, &ScreenXY2.x, &ScreenXY2.y,  fCenterX, fCenterY, fScale); 

		//mCoord.degrees_lat_long_to_x_y(GeoTop, GeoLeft	, &GeoLeft2, &GeoTop2);		
		//mCoord.degrees_lat_long_to_x_y(GeoBottom, GeoRight	, &GeoRight2, &GeoBottom2);

		
		//mCoordMgr->WorldXY2ScreenXY( GeoLeft2, GeoTop2,
		//	&ScreenXY.x, &ScreenXY.y,  fCenterX, fCenterY, fScale); 
		//
		//mCoordMgr->WorldXY2ScreenXY( GeoRight2, GeoBottom2,
		//	&ScreenXY2.x, &ScreenXY2.y,  fCenterX, fCenterY, fScale); 
		//mCadrgData.at(cnt).pCadrg->DrawTCTL( pDC, GeoLeft, GeoTop, GeoRight, GeoBottom );		 
		
		mCadrgData.at(cnt).pCadrg->DrawTCTL( pDC, ScreenXY.x, ScreenXY.y, ScreenXY2.x, ScreenXY2.y);	
		//pDC->SelectStockObject(WHITE_BRUSH);
		//pDC->Rectangle(ScreenXY.x, ScreenXY.y, ScreenXY2.x, ScreenXY2.y);
	}	

	//CGRectGP rect;
	//CGGeoTrans trans(EWGE);
}

void CRasterMgr::ExtractCadrgInfo(CString ConfigPath )
{

	CDIBSectionLite DivSection;

		FILE * stream;
		FILE * input_stream;
		char line[200];
		if( 
			((stream = fopen("C:\\전장상황도\\Adapter\\Map\\CadrgConfig_tmp.txt","r")) != NULL)  && 
			((input_stream = fopen("C:\\전장상황도\\Adapter\\Map\\CadrgConfig.txt","w")) != NULL) )
		{
			int cnt = 1; 
			while( fgets( line, 200, stream ) != NULL)
			{		
				CString values = line;
				CString seperator = ";";
				int position = 0;					 
				CString Scale = values.Tokenize(seperator, position);
				CString FileName = values.Tokenize(seperator, position);
				CString FullPath = values.Tokenize(seperator, position);FullPath.Replace("\n","");


				CString Left = "";
				CString Top = "";
				CString Right = "";
				CString Bottom = "";
						

				DivSection.ExtractCadrgInfo( FullPath, Left, Top, Right, Bottom );


				CString Value = "";
				CString No;
				No.Format(_T("%d"), cnt); ++cnt;
				Value = No + ";" + Scale + ";" + FileName + ";" + FullPath + ";" + Left + ";" + Top
					 + ";" + Right+ ";" + Bottom + "\n";
				 fputs( Value.GetString(), input_stream );	
			}
			fclose( stream );
			fclose( input_stream );		
	}
}

void CRasterMgr::InitCadrgInfoRtree()
{		 
	FILE * stream;
	char line[200];
	if( (stream = fopen("C:\\전장상황도\\Adapter\\Map\\CadrgConfig.txt","r")) != NULL )
	{
		int cnt = 1;
		while( fgets( line, 200, stream ) != NULL)
		{		
			CString values = line;
			CString seperator = ";";
			int position = 0;
			S_TCTL_DATA Tctl;	 

			CString No = values.Tokenize(seperator, position);
			CString Scale = values.Tokenize(seperator, position);
			CString FileName = values.Tokenize(seperator, position);
			CString FullPath = values.Tokenize(seperator, position);
			CString GeoLeft = values.Tokenize(seperator, position);
			CString GeoTop = values.Tokenize(seperator, position);
			CString GeoRight = values.Tokenize(seperator, position);
			CString GeoBottom = values.Tokenize(seperator, position);
									
		 	DOUBLE mins[2] = { atof(GeoLeft), atof(GeoBottom) };
			DOUBLE maxs[2] = { atof(GeoRight), atof(GeoTop) };	//확인!

			if( Scale == "200만" ) mCadrg200.Insert( mins, maxs, cnt );
			if( Scale == "100만" ) mCadrg100.Insert( mins, maxs, cnt );
			if( Scale == "50만" ) mCadrg50.Insert( mins, maxs, cnt );
			if( Scale == "25만" ) mCadrg25.Insert( mins, maxs, cnt );
			if( Scale == "10만" ) mCadrg10.Insert( mins, maxs, cnt );
			if( Scale == "5만" ) mCadrg5.Insert( mins, maxs, cnt );
	  
			//mCadrgPath[ cnt ] = FullPath;

			S_CADRG_DATA CadrgTmp;
			CadrgTmp.No = cnt;
			CadrgTmp.GeoLeft =  atof(GeoLeft);
			CadrgTmp.GeoTop =  atof(GeoTop);
			CadrgTmp.GeoRight =  atof(GeoRight);
			CadrgTmp.GeoBottom =  atof(GeoBottom);
			CadrgTmp.FilePath = FullPath;
			mCadrgInfo[ cnt ] = CadrgTmp;

			++cnt;
		}
		fclose( stream );
	}		
}

void CRasterMgr::DrawCadrg(CDC *pDC, float fCenterX, float fCenterY, double Scale, double left, double top, double right, double bottom )
{	
	//지도도시 준비
	mCoordMgr->ScreenXY2WorldXY( left, top, &left, &top );
	mCoordMgr->ScreenXY2WorldXY( right, bottom, &right, &bottom );
	mCoord.x_y_to_degrees_lat_long( left, top, &top, &left );
	mCoord.x_y_to_degrees_lat_long( right, bottom, &bottom, &right );
	int lastScale = mScale;
	if( Scale < 4 ) mScale = 200;
	else if( Scale > 4 && Scale < 8 ) mScale = 100;
	else if( Scale > 8 && Scale < 16) mScale = 50;
	else if( Scale > 16 && Scale < 32 ) mScale = 25;
	else if( Scale > 32 && Scale < 90 ) mScale = 10;
	else if( Scale > 90 ) mScale = 5;
	
 
	if(lastScale != mScale) CloseCadrgAll();
	//도시할 파일 찾기
	SearchCadrg(mScale, left, top, right, bottom, mIndex );

	//불필요 지도 메모리에서 삭제
	CloseCadrg();
//	CloseCadrgAll();

	//파일 읽기
	if( mScale == 10 || mScale == 5)
		ReadTCTL(pDC, fCenterX, fCenterY, Scale);
	else
		ReadTiff(pDC, fCenterX, fCenterY, Scale);
			
	//도시
	if( mScale == 10 || mScale == 5)
		DrawTCTL(pDC, fCenterX, fCenterY, Scale);
	else
		DrawTiffofCadrg(pDC, fCenterX, fCenterY, Scale);


}

void CRasterMgr::SearchCadrg( int Scale, double left, double top, double right, double bottom, std::vector<int> & Result )
{
	DOUBLE mins[2] = { left, bottom };
	DOUBLE maxs[2] = { right, top }; 

	if(Result.size())
		Result.clear();

	if( mScale == 200 )	mCadrg200.Search( mins, maxs, Result );	 
	if( mScale == 100 )	mCadrg100.Search( mins, maxs, Result );	 
	if( mScale == 50 )	mCadrg50.Search( mins, maxs, Result );	 
	if( mScale == 25 )	mCadrg25.Search( mins, maxs, Result );	 
	if( mScale == 10 )	mCadrg10.Search( mins, maxs, Result );	 
	if( mScale == 5 )	mCadrg5.Search( mins, maxs, Result );	 
}

void CRasterMgr::ReadTCTL(CDC *pDC, float fCenterX, float fCenterY, float fScale)
{
	int CntIndex = mIndex.size();

	std::map< unsigned int, S_CADRG_DATA >::iterator itr_begin;
	CFileFind FileFind;
	
	for(UINT seti = 0; seti < mCadrgData.size();seti++)
	{
		mCadrgData.at(seti).Use = FALSE;
	}

	for( int cnt = 0;  cnt < CntIndex; ++cnt )
	{		
		BOOL IsBe = FALSE;
		itr_begin = mCadrgInfo.find( mIndex.at(cnt));
				 
		//중복검사 : 메모리에 로드된 지도데이터와 새롭게 찾은 지도데이터가 일치한다면 continue;
		int NoNew = mIndex.at(cnt);
		std::vector<S_CADRG_DATA>::iterator itr = mCadrgData.begin();	
		if(mCadrgData.size())
		{
			for( itr; itr != mCadrgData.end(); ++itr )
			{
				int NoOld = itr->No;
				//BOOL IsOld = FALSE;
			 
				if( NoOld == NoNew )
					{
						IsBe = TRUE;
						itr->Use = TRUE;
						break;
				}
			}
		}
		
		if( IsBe == TRUE ) continue;



		CString FilePath;
		if( itr_begin != mCadrgInfo.end() )
			FilePath = itr_begin->second.FilePath;
		else 
			continue;

		if( !FileFind.FindFile(FilePath) ) continue;
			

		BITMAPINFO pBmi;
		bool Isok;
		byte * pBuf = 0x00;

		CGRectGP GeoRect;
		CDIBSectionLite * pCadrg = new CDIBSectionLite();
		pBuf = pCadrg->ReadCADRG(FilePath, &pBmi, &Isok, &GeoRect);
		if(Isok == FALSE)	delete pCadrg;
		else	pCadrg->SetBitmap(&pBmi, pBuf); 			
		delete pBuf;

		S_CADRG_DATA CadrgTmp;
		CadrgTmp.No = itr_begin->first;
		//CadrgTmp.GeoLeft = GeoRect.m_R.left;//itr_begin->second.GeoLeft;
		//CadrgTmp.GeoTop = GeoRect.m_R.top;//itr_begin->second.GeoTop;
		//CadrgTmp.GeoRight = GeoRect.m_R.right;//itr_begin->second.GeoRight;
		//CadrgTmp.GeoBottom = GeoRect.m_R.bottom;//itr_begin->second.GeoBottom;
		CadrgTmp.GeoLeft = itr_begin->second.GeoLeft;
		CadrgTmp.GeoTop = itr_begin->second.GeoTop;
		CadrgTmp.GeoRight = itr_begin->second.GeoRight;
		CadrgTmp.GeoBottom = itr_begin->second.GeoBottom;

		CadrgTmp.FilePath = FilePath;
		CadrgTmp.pCadrg = pCadrg;
		CadrgTmp.Use = TRUE;
		mCadrgData.push_back(CadrgTmp);
	}	
}

void CRasterMgr::ReadTiff(CDC *pDC, float fCenterX, float fCenterY, float fScale)
{
	int CntIndex = mIndex.size();
	std::map< unsigned int, S_CADRG_DATA >::iterator itr_begin;
	CFileFind FileFind;
	for(UINT seti = 0; seti < mCadrgData.size();seti++)
	{
		mCadrgData.at(seti).Use = FALSE;
	}

	for( int cnt = 0;  cnt < CntIndex; ++cnt )
	{	

		BOOL IsBe = FALSE;
		itr_begin = mCadrgInfo.find( mIndex.at(cnt));
				 
		//중복검사 : 메모리에 로드된 지도데이터와 새롭게 찾은 지도데이터가 일치한다면 continue;
		int NoNew = mIndex.at(cnt);
		std::vector<S_CADRG_DATA>::iterator itr = mCadrgData.begin();	 
		for( itr; itr != mCadrgData.end(); ++itr )
		{
			int NoOld = itr->No;
			//BOOL IsOld = FALSE;
			 
			if( NoOld == NoNew )
				{
					IsBe = TRUE;
					itr->Use = TRUE;
					break;
			}
			
		}
		
		if( IsBe == TRUE ) continue;


		CString FilePath;
		if( itr_begin != mCadrgInfo.end() )
			FilePath = itr_begin->second.FilePath;
		else 
			continue;

		if( !FileFind.FindFile(FilePath) ) continue;

		TIFF *image = (TIFF*)0;
		UINT32 xsize, ysize;
		UINT32 numTiePoints, numPixelPoints;
		
		double x1, y1, x2, y2;
		double *pGeoPoints = NULL;
		double *pGeoPixelScale = NULL;

		if((image = XTIFFOpen(FilePath, "r")) == NULL)	return;

		// Find the width and height of the image 
		TIFFGetField(image, TIFFTAG_IMAGEWIDTH, &xsize); 
		TIFFGetField(image, TIFFTAG_IMAGELENGTH, &ysize); 

		TIFFGetField(image, TIFFTAG_GEOTIEPOINTS, &numTiePoints, &pGeoPoints);
		TIFFGetField(image, TIFFTAG_GEOPIXELSCALE, &numPixelPoints, &pGeoPixelScale);

		x1 = pGeoPoints[3];
		y1 = pGeoPoints[4];
		x2 = x1 + xsize*pGeoPixelScale[0];
		y2 = y1 - ysize*pGeoPixelScale[1];
				 
		XTIFFClose(image);
	
		// Image Loading
		CDIBSectionLite * pCadrg;		
		GeoTIFFInfoStruct* info = new GeoTIFFInfoStruct();
		HBITMAP hBitmap = CreateBitmapFromGeoTIFFile(FilePath, (GeoTIFFInfoStruct*)info, 1);

		if(hBitmap)
		{
			pCadrg = new CDIBSectionLite();
			pCadrg->SetBitmap(hBitmap);
			DeleteObject(hBitmap);
		 			
			double  GeoLeft;
			double  GeoTop;
			double  GeoRight;
			double  GeoBottom;			 
				
			mCoord.degrees_lat_long_to_x_y(y1, x1, &GeoLeft, &GeoTop);		
			mCoord.degrees_lat_long_to_x_y(y2, x2, &GeoRight, &GeoBottom);
			
	/*		S_TIFF_DATA tiff_data_tmp;			
			tiff_data_tmp.pTiff = ptiff_tmp;
			tiff_data_tmp.GeoLeft = GeoLeft;
			tiff_data_tmp.GeoTop = GeoTop;
			tiff_data_tmp.GeoRight = GeoRight;
			tiff_data_tmp.GeoBottom = GeoBottom;
			mTiffData.push_back(tiff_data_tmp);	*/	 

			S_CADRG_DATA CadrgTmp;
			CadrgTmp.No = itr_begin->first;
			CadrgTmp.GeoLeft = itr_begin->second.GeoLeft;
			CadrgTmp.GeoTop = itr_begin->second.GeoTop;
			CadrgTmp.GeoRight = itr_begin->second.GeoRight;
			CadrgTmp.GeoBottom = itr_begin->second.GeoBottom;
			CadrgTmp.FilePath = FilePath;
			CadrgTmp.pCadrg = pCadrg;
			CadrgTmp.Use = TRUE;
			mCadrgData.push_back(CadrgTmp);
		}	
		delete (GeoTIFFInfoStruct*)info;		
	}
}

void CRasterMgr::DrawTiffofCadrg(CDC *pDC, float fCenterX, float fCenterY, float fScale)
{	
	int cntTiff = mCadrgData.size();
	for( int cnt = 0; cnt < cntTiff; ++cnt )
	{					
		double  GeoLeft = mCadrgData.at(cnt).GeoLeft;			
		double  GeoTop = mCadrgData.at(cnt).GeoTop;				
		double  GeoRight = mCadrgData.at(cnt).GeoRight;						
		double  GeoBottom = mCadrgData.at(cnt).GeoBottom;						 

		double  GeoLeft2;
		double  GeoTop2;	
		double  GeoRight2;
		double  GeoBottom2; 

		mCoord.degrees_lat_long_to_x_y(GeoTop, GeoLeft	, &GeoLeft2, &GeoTop2);		
		mCoord.degrees_lat_long_to_x_y(GeoBottom, GeoRight	, &GeoRight2, &GeoBottom2);

		CPoint	ScreenXY;
		CPoint	ScreenXY2;
		mCoordMgr->WorldXY2ScreenXY( GeoLeft2, GeoTop2, &ScreenXY.x, &ScreenXY.y,  fCenterX, fCenterY, fScale); 
		mCoordMgr->WorldXY2ScreenXY( GeoRight2, GeoBottom2, &ScreenXY2.x, &ScreenXY2.y,  fCenterX, fCenterY, fScale); 

		mCadrgData.at(cnt).pCadrg->DrawTiff(pDC, ScreenXY.x, ScreenXY.y, ScreenXY2.x, ScreenXY2.y);
	}
}

void CRasterMgr::CloseCadrg()
{
	//if(!mCadrgData.size()) return;
	//std::vector<S_CADRG_DATA>::iterator itr = mCadrgData.begin();
	//int CntCadrgIndex = mIndex.size(); 
	//for( itr; itr < mCadrgData.end(); ++itr )
	//{
	//	int NoOld = itr->No;
	//	BOOL IsOld = TRUE;
	//	for( int cnt_x = 0; cnt_x < CntCadrgIndex; ++cnt_x )
	//	{
	//		int NoNew = mIndex.at(cnt_x);
	//		if( NoOld == NoNew )
	//			IsOld = FALSE;
	//	}
	// 
	//	if( IsOld == TRUE ) 
	//	{
	//		mCadrgData.erase(itr);  
	//		
	//		break;//수정필요함. 다른것들도 지워야 하는데 하나만 걸리면 무조건 break 해버림
	//	}
	//}


	//int cntCadrg = mCadrgData.size();
	//if(!cntCadrg) return;
	//int CntCadrgIndex = mIndex.size(); 
	//std::vector<S_CADRG_DATA>::iterator itr = mCadrgData.begin();
	//for( int cnt = cntCadrg - 1;  cnt >= 0  ; cnt-- )
	//{
	//	int NoOld = mCadrgData.at(cnt).No;
	//	BOOL IsOld = TRUE;
	//	for( int cnt_x = 0; cnt_x < CntCadrgIndex; ++cnt_x )
	//	{
	//		int NoNew = mIndex.at(cnt_x);
	//		if( NoOld == NoNew )
	//		{
	//			IsOld = FALSE;
	//			break;
	//		}
	//	}
	//	if( IsOld == TRUE ) 
	//	{
	//		delete mCadrgData.at(cnt).pCadrg;
	//		mCadrgData.erase(itr+cnt);

	//	}
	//}

	int cntCadrg = mCadrgData.size();
	int CntIndex = mIndex.size();
	if(cntCadrg<CntIndex*2) return;//약간의 여유를 줌<40
	std::vector<S_CADRG_DATA>::iterator itr = mCadrgData.begin();
	for( int cnt = cntCadrg - 1;  cnt >= 0  ; cnt-- )
	{
		if( mCadrgData.at(cnt).Use == FALSE ) 
		{
			delete mCadrgData.at(cnt).pCadrg;
			mCadrgData.erase(itr+cnt);
		
		}
	}
}

void CRasterMgr::CloseCadrgAll()
{
	int cntCadrg = mCadrgData.size();
	for( int cnt = 0;  cnt <  cntCadrg; ++cnt )
	{
		delete mCadrgData.at(cnt).pCadrg;
	}
	mCadrgData.clear();
}