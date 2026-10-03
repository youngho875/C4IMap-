#include "pch.h"
#include "Initiator.h"
#include "MapDataMgr.h"

CInitiator::CInitiator()
{
	m_bDone = FALSE;
	m_bMapDone = FALSE;
}

CInitiator::~CInitiator()
{
	m_ini.Close();
	m_mapini.Close(); 
}

void CInitiator::Init( LPCTSTR lpszInitFile )
{
	//m_ini.Open( lpszInitFile, CFile::modeRead, 0, NULL, 0 );
	if( !m_ini.Init( FALSE ) )
	{
		//		gpTracer->Trace( ERROR_LOG, CHKPRI_HIGH, "ini file init failed" );
		//		TRACE("\nini file init failed\n\n");
		return;
	}
	
	if( !m_ini.Open( lpszInitFile, CFile::modeRead | CFile::typeText ) )
	{
		//		gpTracer->Trace( ERROR_LOG, CHKPRI_HIGH, "ini file open failed" );
		//		TRACE("\nini file open failed\n\n");
		return;
	}
	
	//	gpTracer->Trace( TRACE_LOG, CHKPRI_HIGH, "ini file opened" );
	
	// call OnReadItem per line read
	CString strRead, strSection, strItem, strVal;
	BOOL bSection = FALSE;
	BOOL bInit =FALSE;
	
	while( m_ini.ReadString(strRead) )
	{
		// trim 
		strRead.TrimRight();
		// if '[' and ']' are in strRead, the string is section info
		if( strRead.Find('[')!=-1 && strRead.Find(']')!=-1 )
		{
			strSection = strRead;
			strItem = strVal = _T("");
		}
		// else the string contains item and value, parse 
		else
		{
			int iLen = strRead.GetLength();
			int iEqIdx = strRead.Find('=');
			if( iEqIdx >= 1 && iLen > iEqIdx+1 )
			{
				strItem = strRead.Left(iEqIdx);
				strVal = strRead.Right(iLen-(iEqIdx+1)); 
			}
			else if( iLen == 0 )
			{
				continue;
			}
			else 
			{
				// gTrace
				//				gpTracer->Trace( ERROR_LOG, CHKPRI_HIGH, "Check ini file" );
				//				return;
				continue;
			}
		}
		// trim string
		strSection.TrimLeft();
		strSection.TrimRight();
		strItem.TrimLeft();
		strItem.TrimRight();
		strVal.TrimLeft();
		strVal.TrimRight();
		
		//zzzzz test 10.2
		static int no = 0;
		// Logically the section always exists
		if( !strSection.IsEmpty() )
		{
			OnReadItem( strSection, strItem, strVal, strItem.IsEmpty() );
			TRACE("[ %d ] %s\t%s\t%s\n",++no, strSection, strItem, strVal);
			bInit = TRUE;
		}
	}
	
	// init file close
	m_ini.Close();
	
	// set init flag
	if( bInit )
	{
		m_bDone = TRUE;
		//gpTracer->Trace( TRACE_LOG, CHKPRI_HIGH, "Initiator started" );
		//TRACE("\nInitiator started\n\n");
		// finish work after init
		OnReadEnd();
	}
	else
	{
		//		gpTracer->Trace( ERROR_LOG, CHKPRI_HIGH, "Initiator start failed" );
		//		TRACE("\nInitiator start failed\n\n");
	}
}

// 1,5,cell2,1,128,128.25,42.25,42
// 2,5,cell2,2,128.25,128.5,42.25,42
// 505,10,cell2,1,128,128.25,42.25,42
// 506,10,cell2,2,128.25,128.5,42.25,42
// 1009,25,cell1,-1,126,131,41,43
// 1014,50,cell1,-1,124,131,40,43
// 1017,100,-1,-1,124,130,33,43
// "mapinfo.txt" format : indexno, map scale(5,10,25,50,100), subdir1, subdir2, minx, miny, maxx, maxy
BOOL CInitiator::InitVectorMapInfo()
{
	int iMapKey;
	CString strRootPath, strFilePath, strFileName, strData, strDeli;
	CString strScale, strSub1, strSub2, strTmp;
	double dMinX, dMinY, dMaxX, dMaxY;
	
	strDeli = ",";
	strRootPath = CMapConfigMgr::GetMapInfoDataPath();
	strFileName = "MapInfo.txt";
	strFileName = strRootPath + strFileName;
	strFilePath = CMapConfigMgr::GetShpDataPath();
	
	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strFileName, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	
	char* pDeli;
	while(m_mapini.ReadString(strData))
	{
		// index no
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		iMapKey = atoi(strTmp);
		
		// map scale
		pDeli = strtok(NULL, strDeli);
		strScale.Format("%s만", pDeli);
		
		// subdir1
		pDeli = strtok(NULL, strDeli);
		strSub1.Format("%s", pDeli);
		
		// subdir2
		pDeli = strtok(NULL, strDeli);
		strSub2.Format("%s", pDeli);
		
		// minx
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		dMinX = atof(strTmp);
		
		// maxx
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		dMaxX = atof(strTmp);
		
		// miny
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		dMinY = atof(strTmp);
		
		// maxy
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		dMaxY = atof(strTmp);
		
		//		CMapDataMgr::AddVectorMapInfo(iMapKey, strScale, strRootPath, strSub1, strSub2, dMinX, dMinY, dMaxX, dMaxY);
		CMapDataMgr::AddVectorMapInfo(iMapKey, strScale, strFilePath, strSub1, strSub2, dMinX, dMinY, dMaxX, dMaxY);
	}
	
	m_mapini.Close();
	return TRUE;
}

//1017,100,PAA010,0,1,0,2,000000000,NULL,1
// 1	100	PAA010	0	1	0	2	000000000	NULL	1
// 1	100	LAP010	1	1	000000000	1	0
// 1	100	AAL020	2	1	0	1	255255255	000000000	NULL
// "mapproperty.txt" format 
//	mapkey, 축척, 파일명, 속성종류, 복원도시여부, (점) shape, size, color, strshape, transparent
//												  (선) color, width, linestyle
//												  (면) type, width, fillcolor, linecolor, strpattern
BOOL CInitiator::InitVectorMapProperty()
{
	int iMapKey;
	CString strPath, strData, strDeli, strTmp;
	CString strScale, strFileName, strShape, strpattern;
	UINT uiKind, uiShape, uiSize, uiColor1, uiColor2, uiColor3, uiWidth, uiLineStyle, uiType;
	BOOL bIsVisible, bTransparent;
	COLORREF crColor, crFillcolor;
	
	strDeli = ",";
	strPath = CMapConfigMgr::GetMapInfoDataPath() + "MapProperty.txt";
	
	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	
	char* pDeli;
	while(m_mapini.ReadString(strData))
	{
		// mapkey
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		iMapKey = atoi(strTmp);
		// 축척
		pDeli = strtok(NULL, strDeli);
		strScale.Format("%s", pDeli);
		// 파일명
		pDeli = strtok(NULL, strDeli);
		strFileName.Format("%s", pDeli);
		strFileName.MakeUpper();
		
		// 속성종류
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		uiKind = atoi(strTmp);
		// 복원도시여부
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp == "1") bIsVisible = TRUE;
		else bIsVisible = FALSE;
		// 점 데이터 처리
		if(uiKind == 0)
		{
			// shape
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiShape = atoi(strTmp);
			// size
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiSize = atoi(strTmp);
			// color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() == 1)
			{
				uiColor1 = 0;
				uiColor2 = 0;
				uiColor3 = 0;
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
			}
			crColor = RGB(uiColor1, uiColor2, uiColor3);
			// strShape
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(uiShape != 2) strShape = "";
			else strShape = strTmp;
			// transparent
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp == "1") bTransparent = TRUE;
			else bTransparent = FALSE;
			
			// memory add
			CMapDataMgr::AddMapProperty(iMapKey, strScale, strFileName, uiKind, bIsVisible,
				uiShape, uiSize, crColor, strShape, bTransparent);
		}
		// 선 데이터
		else if(uiKind == 1)
		{
			// color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() == 1)
			{
				uiColor1 = 0;
				uiColor2 = 0;
				uiColor3 = 0;
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
			}
			crColor = RGB(uiColor1, uiColor2, uiColor3);
			// width
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth = atoi(strTmp);
			// line style
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiLineStyle = atoi(strTmp);
			
			// memory add
			CMapDataMgr::AddMapProperty(iMapKey, strScale, strFileName, uiKind, bIsVisible,
				crColor, uiWidth, uiLineStyle);
		}
		// 면 데이터
		else if(uiKind == 2)
		{
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType = atoi(strTmp);
			// width
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth = atoi(strTmp);
			// fill color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() == 1)
			{
				uiColor1 = 0;
				uiColor2 = 0;
				uiColor3 = 0;
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
			}
			crFillcolor = RGB(uiColor1, uiColor2, uiColor3);
			// line color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() == 1)
			{
				uiColor1 = 0;
				uiColor2 = 0;
				uiColor3 = 0;
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
			}
			crColor = RGB(uiColor1, uiColor2, uiColor3);
			// pattern
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(uiType != 2) strpattern = "";
			else strpattern = strTmp;
			
			// memory add
			CMapDataMgr::AddMapProperty(iMapKey, strScale, strFileName, uiKind, bIsVisible,
				uiType, uiWidth, crFillcolor, crColor, strpattern);
		}
		else
		{
			TRACE("error: mapproperty.txt file load failed!");
			m_mapini.Close();
			return FALSE;
		}
	}
	m_mapini.Close();
	return TRUE;
}

// 1,100,afa001
// 2,100,aba040
// 3,100,aba030
// 도시요소 우선순위 데이터 열기
// 우선순위, 축척, 파일명
BOOL CInitiator::InitMapPriority()
{
	
	UINT uiPriority, uiWidth, uiWidth2, uiColor1, uiColor2, uiColor3, uiType, uiType2;
	CString strScale, strFileName, strData, strPath, strDeli, strTmp, strName, strBitmap, earth_or_air, type_geo;
	BOOL bTransparent, bIsVisible;
	COLORREF color1, color2;
	int iViewRatio;
	
	strDeli = ",";
	strPath = CMapConfigMgr::GetMapInfoDataPath() + "MapPriority.txt";
	
	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	
	char* pDeli;
	while(m_mapini.ReadString(strData))
	{
		// priority
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		uiPriority = atoi(strTmp);
		// 축척
		pDeli = strtok(NULL, strDeli);
		strScale.Format("%s", pDeli);
		// 파일명
		pDeli = strtok(NULL, strDeli);
		strFileName.Format("%s", pDeli);
		strFileName.MakeUpper();
		// Default 속성 얻어오기
		// View Ratio
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		iViewRatio = atoi(strTmp);
		// transparent
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp == "1") bTransparent = TRUE;
		else bTransparent = FALSE;
		// visible
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp == "1") bIsVisible = TRUE;
		else bIsVisible = FALSE;
		// 속성명
		pDeli = strtok(NULL, strDeli);
		strName.Format("%s", pDeli);
		
		//CHOIKYUBEOM	MapPriority 에 속성 2개 추가 ( 육도,공도 // 점,선,면 )
		pDeli = strtok(NULL, strDeli);
		type_geo.Format("%s", pDeli);

		pDeli = strtok(NULL, strDeli);
		earth_or_air.Format("%s", pDeli);
 


		// 점 데이터 처리
		if( type_geo == "point")
		{
			// radius
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth = atoi(strTmp);
			// fill color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color1 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color1 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType = atoi(strTmp);
			// bitmap file path
			pDeli = strtok(NULL, strDeli);
			if(pDeli == 0x00)
				strBitmap = _T("");
			else
				strBitmap.Format("%s", pDeli);
			
			//CMapDataMgr::AddMapPriority(uiPriority, strScale, strFileName, bTransparent, bIsVisible, strName,
			//	uiWidth, color1, uiType, strBitmap, iViewRatio);

			CMapDataMgr::AddMapPriority(
				uiPriority, strScale, strFileName, 
				bTransparent, bIsVisible, strName,
				earth_or_air, type_geo, 
				uiWidth, color1, uiType, strBitmap, iViewRatio);

		}
		// 선 데이터 처리
		else if( type_geo == "polyline")
		{
			// line width
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth = atoi(strTmp);
			// line color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color1 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color1 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType = atoi(strTmp);
			// line width2
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth2 = atoi(strTmp);
			// line color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color2 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color2 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType2 = atoi(strTmp);
			
			CMapDataMgr::AddMapPriority2(
				uiPriority, strScale, strFileName, bTransparent, bIsVisible, strName,
				earth_or_air, type_geo, 
				uiWidth, color1, uiType, uiWidth2, color2, uiType2, iViewRatio);
		}
		// 면 데이터 처리
		else if( type_geo == "polygon")
		{
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType = atoi(strTmp);
			// line width
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth = atoi(strTmp);
			// line color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color1 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color1 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// fill color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color2 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color2 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// bitmap file path
			pDeli = strtok(NULL, strDeli);
			if(pDeli == 0x00)
				strBitmap = _T("");
			else
				strBitmap.Format("%s", pDeli);
			
			CMapDataMgr::AddMapPriority3(
				uiPriority, strScale, strFileName, bTransparent, bIsVisible, strName,
				earth_or_air, type_geo, 
				uiWidth, color1, color2, uiType, strBitmap, iViewRatio);
		}
	}
	m_mapini.Close();	
	return TRUE;
}

BOOL CInitiator::InitDefaultMapPriority()
{
	UINT uiPriority, uiWidth, uiWidth2, uiColor1, uiColor2, uiColor3, uiType, uiType2;
	CString strScale, strFileName, strData, strPath, strDeli, strTmp, strName, strBitmap;
	BOOL bTransparent, bIsVisible;
	COLORREF color1, color2;
	int iViewRatio;
	
	strDeli = ",";
	strPath = CMapConfigMgr::GetMapInfoDataPath() + "DefaultMapPriority.txt";
	
	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	
	char* pDeli;
	
	while(m_mapini.ReadString(strData))
	{
		// priority
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		uiPriority = atoi(strTmp);
		// 축척
		pDeli = strtok(NULL, strDeli);
		strScale.Format("%s", pDeli);
		// 파일명
		pDeli = strtok(NULL, strDeli);
		strFileName.Format("%s", pDeli);
		strFileName.MakeUpper();
		
		// Default 속성 얻어오기
		// View Ratio
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		iViewRatio = atoi(strTmp);
		// transparent
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp == "1") bTransparent = TRUE;
		else bTransparent = FALSE;
		// visible
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp == "1") bIsVisible = TRUE;
		else bIsVisible = FALSE;
		// 속성명
		pDeli = strtok(NULL, strDeli);
		strName.Format("%s", pDeli);
		
		// 점 데이터 처리
		if(strFileName.Left(1) == "P")
		{
			// radius
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth = atoi(strTmp);
			// fill color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color1 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color1 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType = atoi(strTmp);
			// bitmap file path
			pDeli = strtok(NULL, strDeli);
			if(pDeli == 0x00)
				strBitmap = _T("");
			else
				strBitmap.Format("%s", pDeli);
			
			CMapDataMgr::AddDefaultMapPriority(uiPriority, strScale, strFileName, bTransparent, bIsVisible, strName,
				uiWidth, color1, uiType, strBitmap, iViewRatio);
		}
		// 선 데이터 처리
		else if(strFileName.Left(1) == "L")
		{
			// line width
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth = atoi(strTmp);
			// line color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color1 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color1 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType = atoi(strTmp);
			// line width2
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth2 = atoi(strTmp);
			// line color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color2 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color2 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType2 = atoi(strTmp);
			
			CMapDataMgr::AddDefaultMapPriority2(uiPriority, strScale, strFileName, bTransparent, bIsVisible, strName,
				uiWidth, color1, uiType, uiWidth2, color2, uiType2, iViewRatio);
		}
		// 면 데이터 처리
		else if(strFileName.Left(1) == "A")
		{
			// type
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiType = atoi(strTmp);
			// line width
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			uiWidth = atoi(strTmp);
			// line color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color1 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color1 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// fill color
			pDeli = strtok(NULL, strDeli);
			strTmp.Format("%s", pDeli);
			if(strTmp.GetLength() < 9)
			{
				color2 = RGB(255,255,255);
			}
			else
			{
				uiColor1 = atoi(strTmp.Mid(0,3));
				uiColor2 = atoi(strTmp.Mid(3,3));
				uiColor3 = atoi(strTmp.Mid(6,3));
				color2 = RGB(uiColor1, uiColor2, uiColor3);
			}
			// bitmap file path
			pDeli = strtok(NULL, strDeli);
			if(pDeli == 0x00)
				strBitmap = _T("");
			else
				strBitmap.Format("%s", pDeli);
			
			CMapDataMgr::AddDefaultMapPriority3(uiPriority, strScale, strFileName, bTransparent, bIsVisible, strName,
				uiWidth, color1, color2, uiType, strBitmap, iViewRatio);
		}
	}	
	m_mapini.Close();
	
	return TRUE;
}

BOOL CInitiator::InitMapGroupItem()
{
	CString strDeli, strPath, strData, strGroupID, strGroupName, strID, strName;
	
	///CHOIKYUBEOM COMMENT
	///순서 바뀌면 안된다. 
	{
		strDeli = ",";
		strPath = CMapConfigMgr::GetMapInfoDataPath() + "MapGroup.txt";
		
		if(!m_mapini.Init(FALSE))
			return FALSE;

		if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
			return FALSE;

		
		char* pDeli;
		while(m_mapini.ReadString(strData))
		{
			// ID
			pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
			strGroupID.Format("%s", pDeli);
			// Name
			pDeli = strtok(NULL, strDeli);
			strGroupName.Format("%s", pDeli);
			CMapDataMgr::AddMapGroup(strGroupID, strGroupName);
		}
		m_mapini.Close();
	}	
	{		
		strDeli = ",";
		strPath = CMapConfigMgr::GetMapInfoDataPath() + "MapGroupList.txt";
		
		if(!m_mapini.Init(FALSE))
			return FALSE;
		
		if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
			return FALSE;
		
		while(m_mapini.ReadString(strData))
		{
			// GroupID
			char* pDeli;
			pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
			strGroupID.Format("%s", pDeli);
			// ID
			pDeli = strtok(NULL, strDeli);
			strID.Format("%s", pDeli);
			// Name
			pDeli = strtok(NULL, strDeli);
			strName.Format("%s", pDeli);
			CMapDataMgr::AddMapGroupItem(strGroupID, strID, strName);
		}
		m_mapini.Close();
	}
	return TRUE;
}

BOOL CInitiator::InitTextPriority()
{
	CString strDeli, strPath, strData, strScale, strTmp, strFontName, strFontScript;
	BOOL bIsVisible;
	UINT uiVisibleType, uiFontSize, uiColor1, uiColor2, uiColor3, uiViewRatio;
	COLORREF clFontColor;
	
	strDeli = ",";
	strPath = CMapConfigMgr::GetMapInfoDataPath() + "TextPriority.txt";
	
	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	
	char* pDeli;
	while(m_mapini.ReadString(strData))
	{
		// scale
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strScale.Format("%s", pDeli);
		// is visible
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp == "1")
			bIsVisible = TRUE;
		else
			bIsVisible = FALSE;
		// visible type
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		uiVisibleType = atoi(strTmp);
		// font size
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		uiFontSize = atoi(strTmp);
		// font color
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp.GetLength() < 9)
		{
			clFontColor = RGB(0,0,0);
		}
		else
		{
			uiColor1 = atoi(strTmp.Mid(0,3));
			uiColor2 = atoi(strTmp.Mid(3,3));
			uiColor3 = atoi(strTmp.Mid(6,3));
			clFontColor = RGB(uiColor1, uiColor2, uiColor3);
		}
		
		// view ratio
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		uiViewRatio = atoi(strTmp);
		
		// 글꼴
		pDeli = strtok(NULL, strDeli);
		if(pDeli == 0x00)
			strFontName = _T("");
		else
			strFontName.Format("%s", pDeli);
		pDeli = strtok(NULL, strDeli);
		if(pDeli == 0x00)
			strFontScript = _T("");
		else
			strFontScript.Format("%s", pDeli);
		
		CMapDataMgr::AddTextPriority(strScale, bIsVisible, uiVisibleType, uiFontSize, 
			clFontColor, uiViewRatio, strFontName, strFontScript);
	}
	m_mapini.Close();
	return TRUE;
}

BOOL CInitiator::InitDefaultTextPriority()
{
	CString strDeli, strPath, strData, strScale, strTmp, strFontName, strFontScript;;
	BOOL bIsVisible;
	UINT uiVisibleType, uiFontSize, uiColor1, uiColor2, uiColor3, uiViewRatio;
	COLORREF clFontColor;
	
	strDeli = ",";
	strPath = CMapConfigMgr::GetMapInfoDataPath() + "DefaultTextPriority.txt";
	//AfxMessageBox(strPath);
	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	
	char* pDeli;
	while(m_mapini.ReadString(strData))
	{
		// scale
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strScale.Format("%s", pDeli);
		// is visible
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp == "1")
			bIsVisible = TRUE;
		else
			bIsVisible = FALSE;
		// visible type
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		uiVisibleType = atoi(strTmp);
		// font size
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		uiFontSize = atoi(strTmp);
		// font color
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		if(strTmp.GetLength() < 9)
		{
			clFontColor = RGB(0,0,0);
		}
		else
		{
			uiColor1 = atoi(strTmp.Mid(0,3));
			uiColor2 = atoi(strTmp.Mid(3,3));
			uiColor3 = atoi(strTmp.Mid(6,3));
			clFontColor = RGB(uiColor1, uiColor2, uiColor3);
		}
		
		// view ratio
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		uiViewRatio = atoi(strTmp);
		
		// 글꼴
		pDeli = strtok(NULL, strDeli);
		if(pDeli == 0x00)
			strFontName = _T("");
		else
			strFontName.Format("%s", pDeli);
		pDeli = strtok(NULL, strDeli);
		if(pDeli == 0x00)
			strFontScript = _T("");
		else
			strFontScript.Format("%s", pDeli);
		CMapDataMgr::AddDefaultTextPriority(strScale, bIsVisible, uiVisibleType, uiFontSize,
			clFontColor, uiViewRatio, strFontName, strFontScript);
	}
	m_mapini.Close();
	return TRUE;
}

BOOL CInitiator::InitWorldMapPriority()
{
	UINT uiSN;
	long left, top, right, bottom;
	CString strDeli, strPath, strData, strTmp, strName;
	
	strDeli = ",";
	//strPath = CMapConfigMgr::GetMapInfoDataPath() + "WorldMapPriority.txt";

	strPath = "D:\\work\\상황도\\Debug\\Bin\\WorldMapPriority.txt";
	


	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	
	char* pDeli;
	while(m_mapini.ReadString(strData))
	{
		// SN
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		uiSN = atoi(strTmp);
		// left
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		left = atol(strTmp);
		// top
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		top = atol(strTmp);
		// right
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		right = atol(strTmp);
		// bottom
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		bottom = atol(strTmp);
		// name
		pDeli = strtok(NULL, strDeli);
		strName.Format("%s", pDeli);
		
		CMapDataMgr::AddSmallWorldMapPriority(uiSN, left, top, right, bottom, strName);
	}
	m_mapini.Close();
	return TRUE;
}

BOOL CInitiator::OpenUserMapInfo()
{
 	BOOL bRes = CMapDataMgr::OpenUserMapInfo();
	BOOL bRes2 = CMapDataMgr::OpenDirectGoMapInfo();
 	if(bRes == FALSE || bRes2 == FALSE)
 		return FALSE;
	return TRUE;
}

BOOL CInitiator::OpenRasterMapInfo()
{
	UINT uiSN;
	int iScale;
	double left, top, right, bottom;
	CString strDeli, strPath, strData, strTmp, strRootPath, strSubPath, strName;
	strDeli = ",";
	
	// 레스터 지도 정보 파일 처리
	strPath = CMapConfigMgr::GetMapInfoDataPath() + "RasterMapInfo.txt";
	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	

	char* pDeli;
	while(m_mapini.ReadString(strData))
	{
		// SN
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		uiSN = atoi(strTmp);
		// scale
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		iScale = atoi(strTmp);
		// file sub path
		pDeli = strtok(NULL, strDeli);
		strSubPath.Format("%s", pDeli);
		// file name
		pDeli = strtok(NULL, strDeli);
		strName.Format("%s", pDeli);
		// left
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		left = atof(strTmp);
		// right
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		right = atof(strTmp);
		// top
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		top = atof(strTmp);
		// bottom
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		bottom = atof(strTmp);
		
		CMapDataMgr::AddRasterMapInfo(uiSN, iScale, strSubPath, strName, left, top, right, bottom);
	}
	m_mapini.Close();
	
 	//OpenSatelliteMapInfo();
	return TRUE;
}

BOOL CInitiator::OpenSatelliteMapInfo()
{
	HANDLE hFile, hFMap;
	//	CString strSMapInfoFile = CMapConfigMgr::GetMapInfoDataPath() + "SatelliteMapInfo.txt";
	CString strSMapInfoFile = CMapConfigMgr::GetMapInfoDataPath() + "SMapInfo.idx";
	hFile = CreateFile(strSMapInfoFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile)
		return FALSE;
	
	hFMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	
	CMapDataMgr::SetSatelliteMapInfo(hFMap);
	//zzzzz test 2008-10-13
	if(hFile)
		CloseHandle(hFile);
	return TRUE;
}

BOOL CInitiator::OpenNameData()
{
	UINT uiSN;
	double dX, dY;
	CString strDeli, strPath, strData, strTmp, strName;
	strDeli = ",";
	
	strPath = CMapConfigMgr::GetMapInfoDataPath() + "NameList.txt";
	if(!m_mapini.Init(FALSE))
	{
		return FALSE;
	}
	
	if(!m_mapini.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}
	
	char* pDeli;
	while(m_mapini.ReadString(strData))
	{
		// SN
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		uiSN = atoi(strTmp);
		// name
		pDeli = strtok(NULL, strDeli);
		strName.Format("%s", pDeli);
		// X
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		dX = atof(strTmp);
		// Y
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		dY = atof(strTmp);
		
		CMapDataMgr::AddNameMap2(uiSN, strName, dX, dY);
	}
	m_mapini.Close();
	return TRUE;
}

void CInitiator::load_symbol_info()
{ 
}
 
void CInitiator::load_shp_attribute()
{	
	CDataSource			datasource;
	HRESULT hr_success;
	hr_success = datasource.OpenFromInitializationString(OLESTR("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=C:\\AFC4I\\bin\\create_info.mdb;"));
	
	if( FAILED( hr_success ) )	return;	

	{
		CSession s_layer_info;
		CCommand<CAccessor<CDB_LAYER_INFO> > cmd_layer_info;	
		const CString Q_LAYER_INFO = "SELECT * FROM LAYER ORDER BY (ORDER)";
		
		hr_success = s_layer_info.Open(datasource); 
		hr_success = cmd_layer_info.Open(s_layer_info,Q_LAYER_INFO);  
		
		if( !FAILED( hr_success ) ) 
		{
			cmd_layer_info.MoveFirst();
			S_LAYER_INFO layer_info; 
			while (TRUE)
			{		
				layer_info.order = cmd_layer_info.order;
				layer_info.id_layer = cmd_layer_info.id_layer;
				layer_info.nm_file = cmd_layer_info.nm_file;
				layer_info.nm_layer = cmd_layer_info.nm_layer;				
				layer_info.scale_min = cmd_layer_info.scale_min;

				layer_info.scale_max = cmd_layer_info.scale_max;
				layer_info.visible = cmd_layer_info.view;
				layer_info.scale = cmd_layer_info.scale;
				layer_info.type_geo = cmd_layer_info.type_geo;
				layer_info.type_map = cmd_layer_info.type_map;

				layer_info.nm_layer_kor = cmd_layer_info.nm_layer_kor;				
				layer_info.point_image = cmd_layer_info.point_image;
				layer_info.point_size = cmd_layer_info.point_size;				
				layer_info.polyline_clr = cmd_layer_info.polyline_clr;
				layer_info.polyline_etc = cmd_layer_info.polyline_etc;

				layer_info.polyline_type = cmd_layer_info.polyline_type;
				layer_info.polyline_width = cmd_layer_info.polyline_width;								
				layer_info.polygon_clr_in = cmd_layer_info.polygon_clr_in;
				layer_info.polygon_clr_out = cmd_layer_info.polygon_clr_out;
				layer_info.polygon_is_in = cmd_layer_info.polygon_is_in;

				layer_info.polygon_pattern_image = cmd_layer_info.polygon_pattern_image;
				layer_info.polygon_type = cmd_layer_info.polygon_type;
				layer_info.polygon_width = cmd_layer_info.polygon_width;
				layer_info.polygon_is_pattern = cmd_layer_info.polygon_is_pattern;
				
				//layer_info.is_edit = TRUE; //처음에 한번은 속성 설정하도록 한다.				
				CMapDataMgr::add_vector_layer_info( layer_info, atoi(layer_info.scale) );
				CMapDataMgr::add_order_info( layer_info.order, layer_info.nm_layer, atoi(layer_info.scale) );
				
				
				if(cmd_layer_info.MoveNext() != S_OK) break;
			}
		}		
		cmd_layer_info.Close();
		s_layer_info.Close();
	}

	{
		CSession s_pattern_info;
		CCommand<CAccessor<CDB_PATTERN_INFO> > cmd_pattern_info;	
		const CString Q_pattern_INFO = "SELECT * FROM PATTERN_INFO";
		
		hr_success = s_pattern_info.Open(datasource); 
		hr_success = cmd_pattern_info.Open( s_pattern_info, Q_pattern_INFO );  
		
		if( !FAILED( hr_success ) ) 
		{
			cmd_pattern_info.MoveFirst();
			S_PATTERN_INFO pattern_info; 
			while (TRUE)
			{		
				pattern_info.nm_pattern = cmd_pattern_info.nm_pattern;
				pattern_info.nm_file = cmd_pattern_info.nm_file;	 
				
				CMapDataMgr::add_pattern_info( pattern_info.nm_pattern, pattern_info.nm_file );
				
				if(cmd_pattern_info.MoveNext() != S_OK) break;
			}
		}
		cmd_pattern_info.Close();
		s_pattern_info.Close();		
	}

	{		
		CSession s_symbol_info;
		CCommand<CAccessor<CDB_SYMBOL_INFO> > cmd_symbol_info;	
		const CString Q_SYMBOL_INFO = "SELECT * FROM SYMBOL_INFO";
		
		hr_success = s_symbol_info.Open(datasource); 
		hr_success = cmd_symbol_info.Open( s_symbol_info, Q_SYMBOL_INFO );  
		
		if( !FAILED( hr_success ) ) 
		{
			cmd_symbol_info.MoveFirst();
			S_SYMBOL_INFO symbol_info; 
			while (TRUE)
			{		
				symbol_info.nm_symbol = cmd_symbol_info.nm_symbol;
				symbol_info.nm_file = cmd_symbol_info.nm_file;	 
				
				CMapDataMgr::add_symbol_info( symbol_info.nm_symbol, symbol_info.nm_file );
				
				if(cmd_symbol_info.MoveNext() != S_OK) break;
			}
		}
		cmd_symbol_info.Close();    
		s_symbol_info.Close();     
	}

	{
		CSession s_user_map_info;
		CCommand<CAccessor<CDB_USER_MAP> > cmd_user_map_info;	
		const CString Q_USER_MAP_INFO = "SELECT * FROM USER_MAP";
		
		hr_success = s_user_map_info.Open(datasource); 
		hr_success = cmd_user_map_info.Open( s_user_map_info, Q_USER_MAP_INFO );  
		
		if( !FAILED( hr_success ) ) 
		{
			cmd_user_map_info.MoveFirst();
			S_MAP_INFO user_map_info; 
			while (TRUE)
			{		 
				user_map_info.is_default_map = cmd_user_map_info.is_default_map;
				user_map_info.nm_usermap = cmd_user_map_info.nm_usermap;
				user_map_info.bIsAutoMode = cmd_user_map_info.bIsAutoMode;
				user_map_info.bIsWorldMapOpen = cmd_user_map_info.bIsWorldMapOpen;
				user_map_info.lMapMode = cmd_user_map_info.lMapMode;

				user_map_info.lMapScale = cmd_user_map_info.lMapScale;
				user_map_info.iMapRatio = cmd_user_map_info.iMapRatio;
				user_map_info.iMapType = cmd_user_map_info.iMapType;				
				user_map_info.dleft = cmd_user_map_info.dleft;
				user_map_info.dtop = cmd_user_map_info.dtop;

				user_map_info.dright = cmd_user_map_info.dright;
				user_map_info.dbottom = cmd_user_map_info.dbottom;				
				user_map_info.dZoomFactor = cmd_user_map_info.dZoomFactor;  		
				user_map_info.coord_center = cmd_user_map_info.coord_center; 
				 
				CMapDataMgr::add_user_map_info( user_map_info );
				if(cmd_user_map_info.MoveNext() != S_OK) break;
			}
		}
		cmd_user_map_info.Close();    
		s_user_map_info.Close();     
	}

/*
	CSession s_cmd;
	hr_success = s_cmd.Open(datasource); 
	CCommand<CNoAccessor,CNoRowset> cmd_update;
	CString	Q_LAYER_UPDATE = "DELETE FROM TBL1";	
	hr_success = cmd_update.Open( s_cmd, Q_LAYER_UPDATE );
	Q_LAYER_UPDATE = "INSERT INTO TBL1 ( aad ) VALUES( '2222' )";	
	hr_success = cmd_update.Open( s_cmd, Q_LAYER_UPDATE );
	cmd_update.Close();
	s_cmd.Close();
*/
	datasource.Close();
}

