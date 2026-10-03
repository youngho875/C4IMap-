#include "pch.h"
#include "MapDataMgr.h"
#include <vector>
#include <map>
#include "io.hpp"
#include "xtiffio.h"
#include "geotiffio.h"
#include "../inc/common.h"




typedef std::map< CString ,S_LAYER_INFO > type_layer_map;
typedef type_layer_map::iterator type_itr_layer_info;
static type_layer_map mmap_layer_info;

static type_layer_map mmap_layer_info_200;
static type_layer_map mmap_layer_info_100;
static type_layer_map mmap_layer_info_50;
static type_layer_map mmap_layer_info_25;
static type_layer_map mmap_layer_info_10;
static type_layer_map mmap_layer_info_5;


typedef std::map< CString, short > type_order_map;
static type_order_map mmap_order_200;
static type_order_map mmap_order_100;
static type_order_map mmap_order_50;
static type_order_map mmap_order_25;
static type_order_map mmap_order_10;
static type_order_map mmap_order_5;

typedef std::map< short, CString > type_order2_map;
static type_order2_map mmap_order2_200;
static type_order2_map mmap_order2_100;
static type_order2_map mmap_order2_50;
static type_order2_map mmap_order2_25;
static type_order2_map mmap_order2_10;
static type_order2_map mmap_order2_5;



typedef std::vector< CString > type_order2_vec;
typedef std::vector< CString >::iterator itr_order2;
static type_order2_vec mvec_order2_200;
static type_order2_vec mvec_order2_100;
static type_order2_vec mvec_order2_50;
static type_order2_vec mvec_order2_25;
static type_order2_vec mvec_order2_10;
static type_order2_vec mvec_order2_5;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
 

CMapDataMgr::CMapDataMgr(CWnd *pWnd)
{
	m_bIsTiffRasterMapOpen = FALSE;
	m_bIsSmallMapOpen = FALSE;
	m_bIsSmallWorldMapOpen = FALSE;
	m_bIsWorldMapOpen = FALSE; 
	m_pMainWnd = NULL;
	m_pCurBaseMapInfo = NULL;
	m_pCurUserMapInfo = NULL;
	m_pSelPointSymbol = NULL;
	m_bIsSelMarking = FALSE;
	m_gpSelMarking.m_P.lat = 999.0;
	m_gpSelMarking.m_P.lon = 999.0; 
	m_enCurMapScale = MS_1_50000;
	m_bIsProgressDlgView = FALSE;
	m_lVectorMapMode = GMM_UTM;
	m_hFMap = NULL;
	m_iSatelliteMapInfoCnt = 0;
	//m_pWnd = pWnd;
}

CMapDataMgr::~CMapDataMgr()
{
	//TCHAR   inBuf[80];
	//::GetPrivateProfileString (TEXT("AFCCS_COP"), 
	//	TEXT("TYPE_BUSINESS"), 
	//	TEXT("Error: GPPS failed"),
	//	inBuf , 
	//	80, 
	//	TEXT("afccs_cop_type.ini")); 
	//CString g_type_business = inBuf;		
	//
	//if( g_type_business == _T("ATO"))
	//{
	//	CDIBSectionLite* pCDIBSL = NULL;
	//	for( int i=0; i<m_paSatelliteMapIndex.GetSize(); i++)
	//	{
	//		if(m_paSatelliteMapIndex[i] != NULL)
	//		{
	//			pCDIBSL = (CDIBSectionLite*)m_paSatelliteMapIndex[i]->m_pData;
	//			SAFE_DELETE(pCDIBSL);
	//			//by Xman94 추가.
	//			SAFE_DELETE(m_paSatelliteMapIndex[i]);
	//		}
	//	}
		m_paSatelliteMapIndex.RemoveAll();
		ClearWorldMap();
		ClearSmallMap();
		ClearSmallWorldMap();
		return;
	//}

	ClearSmallMap();
	ClearSmallWorldMap();
	ClearWorldMap();
	ClearVectorMapList();
	ClearMapProperty();
	ClearUserMapInfo();
	ClearDirectGoMapInfo();
	DeleteAllFeatureInfoList();
 

	CPointPriority* pPoint = NULL;
	CPolylinePriority* pPolyline = NULL;
	CPolygonPriority* pPolygon = NULL;
	for(int i=0; i<m_mappriority.GetSize(); i++)
	{
		if(m_mappriority[i]->m_type_geo == "point")
		{
			//delete (CPointPriority *)m_mappriority[i];
			pPoint = (CPointPriority *)m_mappriority.GetAt(i);
			SAFE_DELETE(pPoint);
		}
		else if(m_mappriority[i]->m_type_geo == "polyline")
		{
			//delete (CPolylinePriority *)m_mappriority[i];
			pPolyline = (CPolylinePriority *)m_mappriority.GetAt(i);
			SAFE_DELETE(pPolyline);
		}
		else if(m_mappriority[i]->m_type_geo == "polygon")
		{
			//delete (CPolygonPriority *)m_mappriority[i];
			pPolygon = (CPolygonPriority *)m_mappriority.GetAt(i);
			SAFE_DELETE(pPolygon);
		}
	}
	m_mappriority.RemoveAll();

	for(int i=0; i<m_pTextPriority.GetSize(); i++)
	{
		//delete m_pTextPriority[i];
		SAFE_DELETE(m_pTextPriority[i]);
	}
	m_pTextPriority.RemoveAll();

	pPoint = NULL;
	pPolyline = NULL;
	pPolygon = NULL;
	for(int i=0; i<m_defaultmappriority.GetSize(); i++)
	{
		if(m_defaultmappriority[i]->m_strFileName.Mid(0,1) == "P")
		{
			//delete (CPointPriority *)m_defaultmappriority.GetAt(i);
			pPoint = (CPointPriority *)m_defaultmappriority.GetAt(i);
			SAFE_DELETE(pPoint);
		}
		else if(m_defaultmappriority[i]->m_strFileName.Mid(0,1) == "L")
		{
			//delete (CPolylinePriority *)m_defaultmappriority.GetAt(i);
			pPolyline = (CPolylinePriority *)m_defaultmappriority.GetAt(i);
			SAFE_DELETE(pPolyline);
		}
		else if(m_defaultmappriority[i]->m_strFileName.Mid(0,1) == "A")
		{
			//delete (CPolygonPriority *)m_defaultmappriority.GetAt(i);
			pPolygon = (CPolygonPriority *)m_defaultmappriority.GetAt(i);
			SAFE_DELETE(pPolygon);
		}
	}
	m_defaultmappriority.RemoveAll();

 

	m_pCurrentMap.RemoveAll();
	m_pCurrentMapList.RemoveAll();

	stMapGroup* pMapGroup = NULL;
	stMapGroupItem* pMapGroupItem = NULL;
	for(int i=0; i<m_pMapGroup.GetSize(); i++)
	{
		pMapGroup = (stMapGroup*)m_pMapGroup[i];
		for(int j=0; j<pMapGroup->m_pGroupList.GetSize(); j++)
		{
			pMapGroupItem = (stMapGroupItem*)pMapGroup->m_pGroupList[j];
			//delete pMapGroupItem;
			SAFE_DELETE(pMapGroupItem);
		}
		pMapGroup->m_pGroupList.RemoveAll();
		//delete pMapGroup;
		SAFE_DELETE(pMapGroup);
	}
	m_pMapGroup.RemoveAll();

	for(int i=0; i<m_pDefaultTextPriority.GetSize(); i++)
	{
		//delete m_pDefaultTextPriority[i];
		//by Xman94 추가.
		SAFE_DELETE(m_pDefaultTextPriority[i]);
	}
	m_pDefaultTextPriority.RemoveAll();

	for(int i=0; i<m_pFontType.GetSize(); i++)
	{
		//delete m_pFontType[i];
		SAFE_DELETE(m_pFontType[i]);
	}
	m_pFontType.RemoveAll();

	for(int i=0; i<m_pSymbolList.GetSize(); i++)
	{
		//delete m_pSymbolList[i];
		//by Xman94 추가.
		SAFE_DELETE(m_pSymbolList[i]);
	}
	m_pSymbolList.RemoveAll();

	for(int i=0; i<m_pPatternList.GetSize(); i++)
	{
		//delete m_pPatternList[i];
		//by Xman94 추가.
		SAFE_DELETE(m_pPatternList[i]);
	}
	m_pPatternList.RemoveAll();

	for(int i=0; i<m_pSmallMapText.GetSize(); i++)
	{
		//delete m_pSmallMapText[i];
		//by Xman94 추가.
		SAFE_DELETE(m_pSmallMapText[i]);
	}
	m_pSmallMapText.RemoveAll();

	for(int i=0; i<m_pWorldMapPriority.GetSize(); i++)
	{
		//delete m_pWorldMapPriority[i];
		//by Xman94 추가.
		SAFE_DELETE(m_pWorldMapPriority[i]);
	}
	m_pWorldMapPriority.RemoveAll();

	// temp buffer clear
	m_tmpmappriority.RemoveAll();
	m_tmpTextPriority.RemoveAll();

	if(m_pCurBaseMapInfo != NULL)
	{
		ClearMapPriority(m_pCurBaseMapInfo);
		SAFE_DELETE(m_pCurBaseMapInfo);
	}

	for(int i=0; i<m_pRasterMap.GetSize(); i++)
	{
		if(m_pRasterMap[i] == NULL) continue;
		if(m_pRasterMap[i]->m_pRasterMap != NULL)
		{
			delete m_pRasterMap[i]->m_pRasterMap;
			m_pRasterMap[i]->m_pRasterMap = NULL;
		}
		delete m_pRasterMap[i];
		m_pRasterMap[i] = NULL;
	}
	m_pRasterMap.RemoveAll();

	CDIBSectionLite* pCDIBSL = NULL;
	for(int i=0; i<m_paSatelliteMapIndex.GetSize(); i++)
	{
		if(m_paSatelliteMapIndex[i] != NULL)
		{
			pCDIBSL = (CDIBSectionLite*)m_paSatelliteMapIndex[i]->m_pData;
			SAFE_DELETE(pCDIBSL);
			SAFE_DELETE(m_paSatelliteMapIndex[i]);
		}
	}
	m_paSatelliteMapIndex.RemoveAll();

	if(m_pSelPointSymbol != NULL)
		m_pSelPointSymbol = NULL;

	for(int i=0; i<m_namedata2.GetSize(); i++)
	{
		//delete m_namedata2[i];
		//by Xman94 추가.
		SAFE_DELETE(m_namedata2[i]);
	}
	m_namedata2.RemoveAll(); 
	if(m_hFMap != NULL)
		CloseHandle(m_hFMap);
}

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------//
// 소축척지도 관련 기능                                             //
//------------------------------------------------------------------//
// 소축척지도 열기

void CMapDataMgr::smallMapOpen()
{
	if(m_bIsSmallMapOpen == TRUE) 
		return;

	enSmallMapType enType;
	CString strPath, strFileName;
	CStringList *slFileName;
	//strPath = "C:\\AFC4I\\Map\\Shp\\small_scale_map\\";

	strPath = CMapConfigMgr::GetSmallmapDataPath();
	slFileName = CMapConfigMgr::GetSmallMapFileName();

	POSITION pos = slFileName->GetHeadPosition();
	while(pos)
	{
		strFileName = slFileName->GetNext(pos);
		if(strFileName.Mid(0,5) == "index")
		{
			if(strFileName.Mid(10,1) == "5")
				enType = SM_INDEXMAP_5;
			else if(strFileName.Mid(10,1) == "1")
				enType = SM_INDEXMAP_10;
		}
		else if(strFileName.Mid(0,4) == "name")
		{
			enType = SM_NAMEMAP;
		}
		else if(strFileName.Mid(0,5) == "small")
		{
			enType = SM_SMALLMAP;
		}
		else
			enType = SM_UNKNOWN;
		strFileName = strPath + strFileName;

		if(enType != SM_UNKNOWN)
			Open(enType, strFileName);
	}
	// smallmap text data open(지명리스트)
	OpenSmallMapText();

	m_bIsSmallMapOpen = TRUE;
	slFileName->RemoveAll();
}

// 소축척지도 열기
bool CMapDataMgr::Open(enSmallMapType enType, CString _filename)
{
	stSmallMap* pSmallMap = NULL;
	int nShapeType;
	CWnd* pWnd = AfxGetMainWnd();
	FILE* fpSHP = fopen(_filename, "rb");

	if(fpSHP == NULL)
		return FALSE;

	// get shp type
	fseek(fpSHP, 32L, SEEK_SET);
	fread(&nShapeType, 4, 1, fpSHP);

	// data opening..
	switch(nShapeType)
	{
	case SHPT_POINT :
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPoint;
		if(!((shpPoint*)pSmallMap->m_pObject)->Open(_filename))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	case SHPT_ARC :
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPolyLine;
		if(!((shpPolyLine*)pSmallMap->m_pObject)->Open(_filename))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	case SHPT_POLYGON :
	case 15 :
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPolygon(pWnd);
		if(!((shpPolygon*)pSmallMap->m_pObject)->Open(_filename))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	default:
		{
			fclose(fpSHP);
			return false;
		}
	}
	fclose(fpSHP);

	pSmallMap->m_enType = enType;
	pSmallMap->m_strFileName = _filename;

	switch(enType)
	{
	case SM_SMALLMAP : 
		{
			m_smallmaps.AddTail(pSmallMap);
			m_smallmapMBR = pSmallMap->m_pObject->GetMBR();
		}
		break;
	case SM_INDEXMAP_5 : 
		m_indexmaps[0].AddTail(pSmallMap);
		break;
	case SM_INDEXMAP_10 : 
		m_indexmaps[1].AddTail(pSmallMap);
		break;
	case SM_NAMEMAP : 
		m_namemaps.AddTail(pSmallMap);
		break;
	default :
		TRACE("error: CMapDataMgr::Open(...)\n");
		delete pSmallMap;
	}


	return true;
}

stSmallMap* CMapDataMgr::GetSmallMapHead()
{
	cur_position = m_smallmaps.GetHeadPosition();
	if(cur_position == NULL)
		return NULL;
	return m_smallmaps.GetHead();
}

stSmallMap* CMapDataMgr::GetSmallMapNext()
{
	if(cur_position == NULL)
		return NULL;
	return m_smallmaps.GetNext(cur_position);
}

// 소축척 세계지도 열기
void CMapDataMgr::smallworldMapOpen()
{
	if(m_bIsSmallWorldMapOpen == TRUE) 
		return;

	CString strPath, strFileName;
	CStringList *slFileName;
	//strPath = "D:\\mapdata\\worldmap\\";
	

	strPath = CMapConfigMgr::GetWorldmapDataPath();
	slFileName = CMapConfigMgr::GetSmallWorldMapFileName();

	POSITION pos = slFileName->GetHeadPosition();
	while(pos)
	{
		strFileName = slFileName->GetNext(pos);
		strFileName = strPath + strFileName;
		smallworldOpen(strFileName);
	}

	m_bIsSmallWorldMapOpen = TRUE;
	slFileName->RemoveAll();
}

// 소축척 세계지도 열기
bool CMapDataMgr::smallworldOpen(CString _filename)
{
	// 전체경로에서 파일명 추출
	//CString name = _filename.Right(_filename.GetLength() - _filename.ReverseFind('\\') - 1);

	stSmallMap* pSmallMap = NULL;
	int nShapeType;
	CWnd* pWnd = AfxGetMainWnd();
	FILE* fpSHP = fopen(_filename, "rb");

	if(fpSHP == NULL)
		return FALSE;

	// get shp type
	fseek(fpSHP, 32L, SEEK_SET);
	fread(&nShapeType, 4, 1, fpSHP);

	// data opening..
	switch(nShapeType)
	{
	case SHPT_POINT:
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPoint;
		if(!((shpPoint*)pSmallMap->m_pObject)->Open(_filename))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	case SHPT_ARC:
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPolyLine;
		if(!((shpPolyLine*)pSmallMap->m_pObject)->Open(_filename))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPolygon(pWnd);
		if(!((shpPolygon*)pSmallMap->m_pObject)->Open(_filename))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	default:
		{
			fclose(fpSHP);
			return false;
		}
	}
	fclose(fpSHP);

	pSmallMap->m_enType = SM_WORLDMAP;
	pSmallMap->m_strFileName = _filename;

	m_smallworldmaps.AddTail(pSmallMap);
	m_bIsWorldMapOpen = TRUE;

	return true;
}

stSmallMap* CMapDataMgr::GetSmallWorldMapHead()
{
	cur_position = m_smallworldmaps.GetHeadPosition();
	if(cur_position == NULL)
		return NULL;
	return m_smallworldmaps.GetHead();
}

stSmallMap* CMapDataMgr::GetSmallWorldMapNext()
{
	if(cur_position == NULL)
		return NULL;
	return m_smallworldmaps.GetNext(cur_position);
}

// 세계지도 열기
void CMapDataMgr::worldMapOpen()
{
	if(m_bIsWorldMapOpen == TRUE) return;
	CString strPath, strFileName;
	CStringList *slFileName;
	//strPath = "D:\\mapdata\\worldmap\\";

	strPath = CMapConfigMgr::GetWorldmapDataPath();
	slFileName = CMapConfigMgr::GetWorldMapFileName();

	POSITION pos = slFileName->GetHeadPosition();
	while(pos)
	{
		strFileName = slFileName->GetNext(pos);
		strFileName = strPath + strFileName;
		worldOpen(strFileName);
	}

	m_bIsWorldMapOpen = TRUE;
//	slFileName->RemoveAll();
}

// 세계지도 열기
bool CMapDataMgr::worldOpen(CString _filename)
{
	stSmallMap* pSmallMap = NULL;
	int nShapeType;
	CWnd* pWnd = AfxGetMainWnd();
	FILE* fpSHP = fopen(_filename, "rb");

	if(fpSHP == NULL)
		return FALSE;

	/*
	///Xman94 추가 2007-10-15 m_pProgrssDlg 가 NULL일 ShowWindow()함수 호출로 인한 UnHandle Exception 발생.
	if(m_pProgressDlg == NULL)
	{
	      m_pProgressDlg = new CProgressDlg;
		  if(!m_pProgressDlg->Create(theApp.m_hInstance, IDD_PROGRESS))
		  {
		      AfxMessageBox("Info: Failed to create CProgressDlg Instance");
			  delete m_pProgressDlg;
			  m_pProgressDlg = NULL;
			  return FALSE;
		  }
	}
	m_pProgressDlg->ShowWindow(SW_SHOWNOACTIVATE);
	*/

	// get shp type
	fseek(fpSHP, 32L, SEEK_SET);
	fread(&nShapeType, 4, 1, fpSHP);

	// data opening..
	switch(nShapeType)
	{
	case SHPT_POINT:
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPoint;
		if(!((shpPoint*)pSmallMap->m_pObject)->OpenMemoryMap(_filename))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	case SHPT_ARC:
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPolyLine;
		//pSmallMap->m_pObject->m_pProgressDlg = m_pProgressDlg;
		if(!((shpPolyLine*)pSmallMap->m_pObject)->OpenEx(_filename, FALSE))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	case SHPT_POLYGON:
		pSmallMap = new stSmallMap;
		pSmallMap->m_pObject = new shpPolygon(pWnd);
		//pSmallMap->m_pObject->m_pProgressDlg = m_pProgressDlg;
		if(!((shpPolygon*)pSmallMap->m_pObject)->OpenMemoryMap(_filename))
		{
			delete pSmallMap->m_pObject;
			delete pSmallMap;
			fclose(fpSHP);
			return false;
		}
		break;
	default:
		{
			fclose(fpSHP);
			return false;
		}
	}
	fclose(fpSHP);

	pSmallMap->m_enType = SM_WORLDMAP;
	pSmallMap->m_strFileName = _filename;

	m_worldmaps.AddTail(pSmallMap);

	m_bIsWorldMapOpen = TRUE;

//	pSmallMap->m_pObject->m_pProgressDlg = NULL;
//	m_pProgressDlg->ShowWindow(SW_HIDE);

	return true;
}

stSmallMap* CMapDataMgr::GetWorldMapHead()
{
	cur_position = m_worldmaps.GetHeadPosition();
	if(cur_position == NULL)
		return NULL;
	return m_worldmaps.GetHead();
}

stSmallMap* CMapDataMgr::GetWorldMapNext()
{
	if(cur_position == NULL)
		return NULL;
	return m_worldmaps.GetNext(cur_position);
}

stIndexData* CMapDataMgr::GetIndexDataHead(enSmallMapType enType)
{
	switch(enType)
	{
	case SM_INDEXMAP_5 :
		{
			cur_position = m_indexdata[0].GetHeadPosition();
			if(cur_position == NULL)
				return NULL;
			return m_indexdata[0].GetHead();
		}
	case SM_INDEXMAP_10 :
		{
			cur_position = m_indexdata[1].GetHeadPosition();
			if(cur_position == NULL)
				return NULL;
			return m_indexdata[1].GetHead();
		}
	}

	return NULL;
}

stIndexData* CMapDataMgr::GetIndexDataNext(enSmallMapType enType)
{
	if(cur_position == NULL)
		return NULL;

	switch(enType)
	{
	case SM_INDEXMAP_5 :
		{
			return m_indexdata[0].GetNext(cur_position);
		}
	case SM_INDEXMAP_10 :
		{
			return m_indexdata[1].GetNext(cur_position);
		}
	}

	return NULL;
}

stNameData* CMapDataMgr::GetNameDataHead()
{
	cur_position = m_namedata.GetHeadPosition();
	if(cur_position == NULL)
		return NULL;
	return m_namedata.GetHead();
}

stNameData* CMapDataMgr::GetNameDataNext()
{
	if(cur_position == NULL)
		return NULL;
	if(cur_position == NULL)
		return NULL;
	return m_namedata.GetNext(cur_position);
}

void CMapDataMgr::InitIndexMapData()
{
	// 최초 call 할경우 한번만 수행되도록 check
	if(m_indexdata[0].GetCount() > 0 ||
		m_indexdata[1].GetCount() > 0)
		return;

	POSITION pos;
	stSmallMap* pSmallMap = NULL;
	stIndexData* pIndexData = NULL;
	shpObject* shpDraw = NULL;
	polyObject *SHPPoly;
	int iSize;
	CPoint point;
	GPoint gpoint;
	CString tmpString, tmp, strID;
	double left, top, right, bottom;
	CGRectGP rectGP;

	for(int i=0; i<2; i++)
	{
		pos = m_indexmaps[i].GetHeadPosition();
		if(pos == NULL) 
			continue;
		pSmallMap = m_indexmaps[i].GetNext(pos);
		shpDraw = pSmallMap->m_pObject;

		iSize = ((shpPolygon*)shpDraw)->m_paSHPPoly.GetSize();
		for(int j=0; j<iSize; ++j)
		{
			left = bottom = 999;
			top = right = 0;
			SHPPoly = ((shpPolygon*)shpDraw)->m_paSHPPoly[j];
			for(int k=0; k<SHPPoly->m_nNumPoints; ++k)
			{
				gpoint.x = SHPPoly->m_gpPoints[k].m_P.lon;
				gpoint.y = SHPPoly->m_gpPoints[k].m_P.lat;

				tmpString = shpDraw->ReadStringAttribute(SHPPoly->m_nID, 1);
				if(tmpString.IsEmpty())
					continue;
				if(k == 0)
					pIndexData = new stIndexData;
				pIndexData->m_strID.Format("%d", SHPPoly->m_nID+1);
				pIndexData->m_strIndexName = tmpString;
				pIndexData->m_pPoint.AddTail(gpoint);
				if(left > gpoint.x) left = gpoint.x;
				if(right < gpoint.x) right = gpoint.x;
				if(bottom > gpoint.y) bottom = gpoint.y;
				if(top < gpoint.y) top = gpoint.y;
			}

			rectGP.SetRect(left, top, right, bottom);
			pIndexData->m_gPoint = rectGP.CenterPoint();

			if(i == 0)
				AddIndexMap(SM_INDEXMAP_5, pIndexData, TRUE);
			else if(i == 1)
				AddIndexMap(SM_INDEXMAP_10, pIndexData, TRUE);
		}
	}
}

void CMapDataMgr::InitNameMapData()
{
	// jwpark 20061113, stNameData2 구조체 이용, block 처리...
	return;

	// 최초 call 할경우 한번만 수행되도록 check
	if(m_namedata.GetCount() > 0)
		return;

	POSITION pos;
	stSmallMap* pSmallMap;
	stNameData* pNameData;
	shpObject* shpDraw;
	polyObject SHPPoint;
	int iSize;
	CPoint point;
	GPoint gpoint;
	CString tmpString, tmp, strID;

	pos = m_namemaps.GetHeadPosition();
	if(pos == NULL)
		return;
	pSmallMap = m_namemaps.GetNext(pos);
	shpDraw = pSmallMap->m_pObject;

	iSize = ((shpPoint*)shpDraw)->m_vSHPPoint.size();
	for(int i=0; i<iSize; ++i)
	{
		gpoint = ((shpPoint*)shpDraw)->m_vSHPPoint[i];
		tmpString = shpDraw->ReadStringAttribute(i, 3);
		if(tmpString.IsEmpty())
			continue;
		pNameData = new stNameData;
		pNameData->m_strID.Format("%d", i+1);
		pNameData->m_strName = tmpString;
		pNameData->m_pPoint = gpoint;
		AddNameMap(pNameData, TRUE);
	}
}

void CMapDataMgr::AddIndexMap(enSmallMapType enType, stIndexData* pItem, BOOL bIsHead/*=FALSE*/)
{
	if(pItem == NULL) return;
	switch(enType)
	{
	case SM_INDEXMAP_5 : 
		if(bIsHead == TRUE)
			m_indexdata[0].AddHead(pItem);
		else
			m_indexdata[0].AddTail(pItem);
		break;
	case SM_INDEXMAP_10 : 
		if(bIsHead == TRUE)
			m_indexdata[1].AddHead(pItem);
		else
			m_indexdata[1].AddTail(pItem);
		break;
	}
}

void CMapDataMgr::AddNameMap(stNameData* pItem, BOOL bIsHead/*=FALSE*/)
{
	if(pItem == NULL) return;
	if(bIsHead == TRUE)
		m_namedata.AddHead(pItem);
	else
		m_namedata.AddTail(pItem);
}

int CMapDataMgr::GetSmallMapCount()
{
	return m_smallmaps.GetCount();
}

int CMapDataMgr::GetIndexDataCount(enSmallMapType enType)
{
	switch(enType)
	{
	case SM_INDEXMAP_5 : return m_indexdata[0].GetCount();
	case SM_INDEXMAP_10 : return m_indexdata[1].GetCount();
	default : return -1;
	}
}

int CMapDataMgr::GetNameDataCount()
{
	return m_namedata.GetCount();
}

void CMapDataMgr::ClearSmallMap()
{
	stIndexData* pIndexData;
	for(int i=0; i<2; i++)
	{
		while(!m_indexdata[i].IsEmpty())
		{
			pIndexData = (stIndexData*)m_indexdata[i].RemoveHead();
			pIndexData->m_pPoint.RemoveAll();
			delete pIndexData;
		}
		m_indexdata[i].RemoveAll();
	}

	stNameData* pNameData;
	while(!m_namedata.IsEmpty())
	{
		pNameData = (stNameData*)m_namedata.RemoveHead();
		delete pNameData;
	}
	m_namedata.RemoveAll();

	stSmallMap* pSmallMap;
	while(!m_smallmaps.IsEmpty())
	{
		pSmallMap = (stSmallMap*)m_smallmaps.RemoveHead();
		delete pSmallMap->m_pObject;
		delete pSmallMap;
	}
	m_smallmaps.RemoveAll();

	for(int i=0; i<2; i++)
	{
		while(!m_indexmaps[i].IsEmpty())
		{
			pSmallMap = (stSmallMap*)m_indexmaps[i].RemoveHead();
			delete pSmallMap->m_pObject;
			delete pSmallMap;
		}
		m_indexmaps[i].RemoveAll();
	}

	while(!m_namemaps.IsEmpty())
	{
		pSmallMap = m_namemaps.RemoveHead();
		delete pSmallMap->m_pObject;
		delete pSmallMap;
	}
	m_namemaps.RemoveAll();
}

void CMapDataMgr::ClearSmallWorldMap()
{
	stSmallMap* pSmallMap;
	while(!m_smallworldmaps.IsEmpty())
	{
		pSmallMap = (stSmallMap*)m_smallworldmaps.RemoveHead();
		delete pSmallMap->m_pObject;
		delete pSmallMap;
	}
	m_smallworldmaps.RemoveAll();
}

void CMapDataMgr::ClearWorldMap()
{
	stSmallMap* pSmallMap;
	while(!m_worldmaps.IsEmpty())
	{
		pSmallMap = (stSmallMap*)m_worldmaps.RemoveHead();
		delete pSmallMap->m_pObject;
		delete pSmallMap;
	}
	m_worldmaps.RemoveAll();

	m_bIsWorldMapOpen = FALSE;
}

//------------------------------------------------------------------//
// 벡터 지도 데이터 관련 기능                                       //
//------------------------------------------------------------------//

// 벡터 지도 데이터 관리 리스트 추가 (insert sorting)
BOOL CMapDataMgr::AddVectorMapInfo(int iMapKey, CString strScale, CString strPath, CString strSub1, CString strSub2, 
								   double dMinX, double dMinY, double dMaxX, double dMaxY)
{
	stVectorMap* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_vectormaps.GetUpperBound();
	int iMid;
	int iCmp;

	CString strKey1, strKey2;
	strKey1.Format("%04d", iMapKey);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%04d", m_vectormaps[iMid]->m_iMapKey);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면 
	 	if(iCmp == 0)
		{
			pItem = m_vectormaps[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

// 못 찾았으면 생성
	if(pItem == NULL)
		pItem = new stVectorMap;

	pItem->m_iMapKey = iMapKey;
	if(strScale == "5만") pItem->m_enMapScale = MS_1_50000;
	else if(strScale == "10만") pItem->m_enMapScale = MS_1_100000;
	else if(strScale == "25만") pItem->m_enMapScale = MS_1_250000;
	else if(strScale == "50만") pItem->m_enMapScale = MS_1_500000;
	else if(strScale == "100만") pItem->m_enMapScale = MS_1_1000000;
	else if(strScale == "200만") pItem->m_enMapScale = MS_1_2000000;
	pItem->m_strRootPath = strPath;
	if(strSub1 == "-1")
		pItem->m_strSubPath1 = _T("");
	else
		pItem->m_strSubPath1 = strSub1;
	if(strSub2 == "-1")
		pItem->m_strSubPath2 = _T("");
	else
		pItem->m_strSubPath2 = strSub2;
//	if( pItem->m_strSubPath1 == "" )
//		AfxMessageBox("");
	CGRectGP gpRect(dMinX, dMinY, dMaxX, dMaxY);
	pItem->m_gpRect.SetRect(&gpRect);

	pItem->m_bIsVectorDataOpen = FALSE;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_vectormaps.GetUpperBound())
			m_vectormaps.Add(pItem);
		else
			m_vectormaps.InsertAt(iLow, pItem);
	}

	return TRUE;
}


/*
// 현재 사용되는 지도데이터 reset
void CMapDataMgr::ResetCurrentMap(EGMapScale enScale, CGRectGP rect)
{
	stVectorMap* pItem;
	stVObject* pSubItem;
	stFeatureInfo* pFeature;

	int cnt_current_map = m_pCurrentMapList.GetSize();

	for(int i=0; i< cnt_current_map; )
	{
		pItem = (stVectorMap*)m_pCurrentMapList[i];		
		if( pItem->m_enMapScale == enScale 	&& 	DupCheck(enScale, pItem->m_gpRect, rect))
		{
			++i;
			continue;
		}
		int cnt_obj = pItem->m_pVObject.GetSize();
		for(int j=0; j<cnt_obj; j++)
		{
			pSubItem = (stVObject*)pItem->m_pVObject[j];
			//pSubItem->m_pMapProperty.m_enMapProperty
			if(pSubItem->m_pObject != NULL)
			{
				// 지형요소속성정보 관리 리스트 동기화..
				int cnt_feature = m_pFeatureInfoList.GetSize();
				for(int k=0; k<cnt_feature; k++)
				{
					pFeature = (stFeatureInfo*)m_pFeatureInfoList[k];
					if(pFeature->m_pObject == pSubItem)
						pFeature->m_pObject = NULL;
				}  
				if(pSubItem->m_pMapProperty->m_enMapProperty == MP_POINT)			delete (shpPoint *)pSubItem->m_pObject;
				else if(pSubItem->m_pMapProperty->m_enMapProperty == MP_POLYLINE)	delete (shpPolyLine *)pSubItem->m_pObject;
				else if(pSubItem->m_pMapProperty->m_enMapProperty == MP_POLYGON)	delete (shpPolygon *)pSubItem->m_pObject;

				pSubItem->m_pObject = NULL;
				pItem->m_bIsVectorDataOpen = FALSE;
			}
		}
		for(j=0; j<pItem->m_pMapText.GetSize(); j++)
			delete pItem->m_pMapText[j];
		pItem->m_pMapText.RemoveAll();
		m_pCurrentMapList.RemoveAt(i);
	}
}
*/
// 벡터 지도 데이터 관리 리스트 삭제
void CMapDataMgr::ClearVectorMapList()
{
	stVectorMap *pVectorMap;
	stVObject *pVObject;

	for(int i=0; i<m_vectormaps.GetSize(); i++)
	{
		pVectorMap = (stVectorMap*)m_vectormaps[i];
		for(int j=0; j<pVectorMap->m_pVObject.GetSize(); j++)
		{
			pVObject = (stVObject*)pVectorMap->m_pVObject[j];
//			if(pVObject->m_pMapProperty != NULL)
//				delete pVObject->m_pMapProperty;
			if(pVObject->m_pMapProperty != NULL)
			{
				if(pVObject->m_pMapProperty->m_enMapProperty == 0 )
					delete (CPointProperty *)pVObject->m_pMapProperty;
				else if(pVObject->m_pMapProperty->m_enMapProperty == 1)
					delete (CPolylineProperty *)pVObject->m_pMapProperty;
				else if(pVObject->m_pMapProperty->m_enMapProperty == 2)
					delete (CPolygonProperty *)pVObject->m_pMapProperty;
				else
					TRACE("error: CMapDataMgr::ClearVectorMapList()\n");
			}

			if(pVObject->m_pObject != NULL)
				delete pVObject->m_pObject;
			pVObject->m_paMapPriority.RemoveAll();
			delete pVObject;
		}
		m_vectormaps[i]->m_pVObject.RemoveAll();

		for(int j=0; j<pVectorMap->m_pMapText.GetSize(); j++)
		{
			delete pVectorMap->m_pMapText[j];
		}
		m_vectormaps[i]->m_pMapText.RemoveAll();

		delete m_vectormaps[i];
	}
	m_vectormaps.RemoveAll();
}

// 벡터 지도 데이터 헤더 찾기
stVectorMap* CMapDataMgr::FindVectorMapInfo(int iMapKey)
{
	int iLow = 0;
	int iHigh = m_vectormaps.GetUpperBound();
	int iMid;
	int iCmp;

	CString strKey1, strKey2;
	strKey1.Format("%04d", iMapKey);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%04d", m_vectormaps[iMid]->m_iMapKey);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));
	 	if (iCmp == 0)
			return m_vectormaps[iMid];
		else if (iCmp > 0)
	 		iHigh = iMid - 1;
	 	else 
	 		iLow = iMid + 1;
	}

	return NULL;
}

// 지도 속성 정보 추가(점)
BOOL CMapDataMgr::AddMapProperty(int iMapKey, CString strScale, CString strFileName, UINT uiKind, BOOL 
								 bIsVisible, UINT uiShape, UINT uiSize, COLORREF crColor, CString strShape, BOOL bTransparent)
{
	CPointProperty* pItem = new CPointProperty;

// 공통 데이터 처리
	// map key
	pItem->m_iMapKey = iMapKey;
	// 축척
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map property add failed(point)!(CMapDataMgr::AddMapProperty)\n");
		delete pItem;
		return FALSE;
	}
	pItem->m_enMapScale = enScale;
	// 파일명
	pItem->m_strFileName = strFileName;
	// 속성종류
	switch(uiKind)
	{
		case 0 : pItem->m_enMapProperty = MP_POINT;
			break;
		case 1 : pItem->m_enMapProperty = MP_POLYLINE;
			break;
		case 2 : pItem->m_enMapProperty = MP_POLYGON;
			break;
		// error
		default : delete pItem;
			return FALSE;
	}
	// 복원도시여부
	pItem->m_bIsVisible = bIsVisible;

// 점 데이터 처리
	// shape
	pItem->m_uiShape = uiShape;
	// size
	pItem->m_uiSize = uiSize;
	// color
	pItem->m_crColor = crColor;
	// symbol path/file name
	pItem->m_strShape = strShape;
	// transparent
	pItem->m_bTransparent = bTransparent;

	stVectorMap* pVectorMap = FindVectorMapInfo(pItem->m_iMapKey);
	if(pVectorMap == NULL)
	{
		TRACE("error: CMapDataMgr::AddMapProperty(point) process failed!\n");
		delete pItem;
		return FALSE;
	}

	if(!AddVObject(strFileName, pVectorMap, (CMapProperty*)pItem))
	{
		delete pItem;
		return FALSE;
	}

	return TRUE;
}

// 지도 속성 정보 추가(선)
BOOL CMapDataMgr::AddMapProperty(int iMapKey, CString strScale, CString strFileName, UINT uiKind, 
								 BOOL bIsVisible, COLORREF crColor, UINT uiWidth, UINT uiLineStyle)
{
	CPolylineProperty* pItem = new CPolylineProperty;

// 공통 데이터 처리
	// map key
	pItem->m_iMapKey = iMapKey;
	// 축척
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map property add failed(polyline)!(CMapDataMgr::AddMapProperty)\n");
		delete pItem;
		return FALSE;
	}
	pItem->m_enMapScale = enScale;
	// 파일명
	pItem->m_strFileName = strFileName;
	// 속성종류
	switch(uiKind)
	{
		case 0 : pItem->m_enMapProperty = MP_POINT;
			break;
		case 1 : pItem->m_enMapProperty = MP_POLYLINE;
			break;
		case 2 : pItem->m_enMapProperty = MP_POLYGON;
			break;
		// error
		default : delete pItem;
			return FALSE;
	}
	// 복원도시여부
	pItem->m_bIsVisible = bIsVisible;

// 선 데이터 처리
	// color
	pItem->m_crColor = crColor;
	// line width
	pItem->m_uiWidth = uiWidth;
	// style
	pItem->m_uiLineStyle = uiLineStyle;

	stVectorMap* pVectorMap = FindVectorMapInfo(pItem->m_iMapKey);
	if(pVectorMap == NULL)
	{
		TRACE("error: CMapDataMgr::AddMapProperty(polyline) process failed!\n");
		delete pItem;
		return FALSE;
	}

	if(!AddVObject(strFileName, pVectorMap, (CMapProperty*)pItem))
	{
		delete pItem;
		return FALSE;
	}

	return TRUE;
}

// 지도 속성 정보 추가(면)
BOOL CMapDataMgr::AddMapProperty(int iMapKey, CString strScale, CString strFileName, UINT uiKind, 
								 BOOL bIsVisible, UINT uiType, UINT uiWidth, COLORREF crFillColor, COLORREF crLineColor, CString strPattern)
{
	CPolygonProperty* pItem = new CPolygonProperty;

// 공통 데이터 처리
	// map key
	pItem->m_iMapKey = iMapKey;
	// 축척
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map property add failed(polygon)!(CMapDataMgr::AddMapProperty)\n");
		delete pItem;
		return FALSE;
	}
	pItem->m_enMapScale = enScale;
	// 파일명
	pItem->m_strFileName = strFileName;
	// 속성종류
	switch(uiKind)
	{
		case 0 : pItem->m_enMapProperty = MP_POINT;
			break;
		case 1 : pItem->m_enMapProperty = MP_POLYLINE;
			break;
		case 2 : pItem->m_enMapProperty = MP_POLYGON;
			break;
		// error
		default : delete pItem;
			return FALSE;
	}
	// 복원도시여부
	pItem->m_bIsVisible = bIsVisible;

// 면 데이터 처리
	// type
	pItem->m_uiType = uiType;
	// line width
	pItem->m_uiWidth = uiWidth;
	// fill color
	pItem->m_crFillColor = crFillColor;
	// line color
	pItem->m_crLineColor = crLineColor;
	// pattern file path/name
	pItem->m_strPattern = strPattern;

	stVectorMap* pVectorMap = FindVectorMapInfo(pItem->m_iMapKey);
	if(pVectorMap == NULL)
	{
		TRACE("error: CMapDataMgr::AddMapProperty(polygon) process failed!\n");
		delete pItem;
		return FALSE;
	}

	if(!AddVObject(strFileName, pVectorMap, (CMapProperty*)pItem))
	{
		delete pItem;
		return FALSE;
	}

	return TRUE;
}

// 벡터 지도 shape data array에 아이템 추가 (insert sort)
BOOL CMapDataMgr::AddVObject(CString strFileName, stVectorMap* pVectorMap, CMapProperty* pMapProperty)
{
	stVObject* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = pVectorMap->m_pVObject.GetUpperBound();
	int iMid;
	int iCmp;

	CString strTrace;
	CMapPriority *pPriority2 = NULL;
	CMapPriority *pPriority1 = FindMapPriority(pMapProperty->m_enMapScale, strFileName);

	if(pPriority1 == NULL)
		return FALSE;

	CString strKey1, strKey2;
	strKey1.Format("%04d", pPriority1->m_uiPriority);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		if(pVectorMap->m_pVObject[iMid]->m_paMapPriority.GetSize() < 0)
			return FALSE;
		pPriority2 = pVectorMap->m_pVObject[iMid]->m_paMapPriority[0];
		strKey2.Format("%04d",  pPriority2->m_uiPriority);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
	//			pItem = pVectorMap->m_pVObject[iMid];
	//			bFound = TRUE;
	//			break;
			return FALSE;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	// 못 찾았으면 생성
	pItem = new stVObject;
	pItem->m_pMapProperty = pMapProperty;
	pItem->m_strFileName = strFileName;
	pItem->m_pObject = NULL;
	
	INT CNT_MAP_PRIORITY = m_mappriority.GetSize();
	for(int i=0; i < CNT_MAP_PRIORITY; i++)
	{
		if(	pPriority1->m_enMapScale == m_mappriority[i]->m_enMapScale && 
			pPriority1->m_strFileName == m_mappriority[i]->m_strFileName
			)
			pItem->m_paMapPriority.Add(m_mappriority[i]);
	}

	// 새로 생성한 것이면 삽입
	if(iLow > pVectorMap->m_pVObject.GetUpperBound())
		pVectorMap->m_pVObject.Add(pItem);
	else
		pVectorMap->m_pVObject.InsertAt(iLow, pItem);

	return TRUE;
}

// 지도 속성 정보 리스트 삭제
void CMapDataMgr::ClearMapProperty()
{

}

// 벡터 지도 데이터 Default Load(100만)
BOOL CMapDataMgr::OpenDefaultVectorMap()
{ 
	// symbol/pattern load
	OpenSymbolData();
	OpenPatternData();

	// default map load
//	stVectorMap* pItem;
//	for(int i=0; i<m_vectormaps.GetSize(); i++)
	{
//		pItem = (stVectorMap*)m_vectormaps[i];
//		if(pItem->m_enMapScale == MS_1_1000000)
//			OpenVectorMap(pItem);
	}

//	if(m_bIsProgressDlgView)
//	{
//		m_pProgressDlg->ShowWindow(SW_HIDE);
//		m_bIsProgressDlgView = FALSE;
//	} 
	return TRUE;
}

// 벡터 지도 데이터 clear(100만 제외, shpObject 만 clear)
BOOL CMapDataMgr::ClearVectorMap(CGPointGP gpoint)
{	
	return TRUE;
}

// 벡터 지도 데이터 열기
BOOL CMapDataMgr::OpenVectorMap(stVectorMap* pItem)
{
	clock_t startTimeDraw = clock();


	if(pItem == NULL)
		return FALSE;
	BOOL bRes = FALSE;
	
	CString strPath, strSubPath, strFileName, strFullName, strTmp, strDir1, strDir2;
	strDir1 = strDir2 = "";
	if(!pItem->m_strSubPath1.IsEmpty())
		strDir1 = "\\";
	if(!pItem->m_strSubPath2.IsEmpty())
		strDir2 = "\\";
	
	if( pItem->m_strSubPath1.Find("Aero",0) )
	{
		switch(pItem->m_enMapScale)
		{
		case MS_1_50000 : strSubPath = "EAR_5만\\\\";
			break;
		case MS_1_100000 : strSubPath = "EAR_10만\\\\";
			break;
		case MS_1_250000 : strSubPath = "EAR_25만\\\\";
			break;
		case MS_1_500000 : strSubPath = "EAR_50만\\\\";
			break;
		case MS_1_1000000 : strSubPath = "EAR_100만\\\\";
			break;
		case MS_1_2000000 : strSubPath = "EAR_200만\\\\";
			break;
		}
	}
	else
	{		
		switch(pItem->m_enMapScale)
		{
		case MS_1_50000 : strSubPath = "AIR_5만\\\\";
			break;
		case MS_1_100000 : strSubPath = "AIR_10만\\\\";
			break;
		case MS_1_250000 : strSubPath = "AIR_25만\\\\";
			break;
		case MS_1_500000 : strSubPath = "AIR_50만\\\\";
			break;
		case MS_1_1000000 : strSubPath = "AIR_100만\\\\";
			break;
		case MS_1_2000000 : strSubPath = "AIR_200만\\\\";
			break;
		}
	}
	
	strPath = pItem->m_strRootPath + strSubPath + pItem->m_strSubPath1 + strDir1 + pItem->m_strSubPath2 + strDir2;
	
//	int nShapeType;
	CWnd* pWnd = AfxGetMainWnd();
	//	FILE* fpSHP;
	stVObject* pObject;
	CFile file;
	CString fileExt = ".shp";
	
	float fPercent = 0.0f;
	CString strText, strText2;
	strText2 = " Loading...";
	BOOL bIsOpen;
	
	TRACE("-------------------------------------------------------------------------\n");
	int cnt_obj = pItem->m_pVObject.GetSize();
	for(int i=0; i < cnt_obj; i++)
	{
		pObject = (stVObject*)pItem->m_pVObject[i];
		strFileName = pObject->m_strFileName;
		strFullName = strPath + strFileName + fileExt;
		bIsOpen = FALSE;
		
		//CHOIKYUBEOM 데이터 타입 가져오기 //파일에서 가져오는 부분 제거  
		CMapPriority* pPriority;
		pPriority = CMapDataMgr::FindMapPriority( pItem->m_enMapScale, strFileName );
		if( pPriority == NULL )
			continue;

		CString	 type_geo = pPriority->m_type_geo;
				
		
		//분석 필요...
		if(pObject->m_paMapPriority.GetSize() == 1)
		{
			if(pObject->m_paMapPriority[0] != NULL &&
				pItem->m_enMapScale != MS_1_1000000)
			{
				if(pObject->m_paMapPriority[0]->m_bIsVisible == FALSE ||
					m_dViewRatio > (double)pObject->m_paMapPriority[0]->m_iViewRatio)
				{
					if(pObject->m_pObject != NULL)
					{
						delete pObject->m_pObject;
						pObject->m_pObject = NULL;
					}	
					continue;
				}
			}
		}
		else
		{
			if(pItem->m_enMapScale != MS_1_1000000)
			{
				for(int j=0; j<pObject->m_paMapPriority.GetSize(); j++)
				{
					if(pObject->m_paMapPriority[j] != NULL)
					{
						if(pObject->m_paMapPriority[j]->m_bIsVisible == TRUE &&
							m_dViewRatio <= (double)pObject->m_paMapPriority[j]->m_iViewRatio)
							bIsOpen = TRUE;
					}
				}				
				if(bIsOpen == FALSE)
				{
					if(pObject->m_pObject != NULL)
					{
						delete pObject->m_pObject;
						pObject->m_pObject = NULL;
					}
					continue;
				}
			}
		}
		
		// 중복 open 방지
		if(pObject->m_pObject != NULL)	continue;

		
		fPercent = (float)((float)((i+1)*100.0)/(float)pItem->m_pVObject.GetSize());
		strText = "";
		strText.Format("%3.1f%%", fPercent);
		
		switch(pItem->m_enMapScale)
		{
		case MS_1_2000000 : strText = strText + " 200만 " + pObject->m_paMapPriority[0]->m_strName + strText2;
			break;
		case MS_1_1000000 : strText = strText + " 100만 " + pObject->m_paMapPriority[0]->m_strName + strText2;
			break;
		case MS_1_500000 : strText = strText + " 50만 " + pObject->m_paMapPriority[0]->m_strName + strText2;
			break;
		case MS_1_250000 : strText = strText + " 25만 " + pObject->m_paMapPriority[0]->m_strName + strText2;
			break;
		case MS_1_100000 : strText = strText + " 10만 " + pObject->m_paMapPriority[0]->m_strName + strText2;
			break;
		case MS_1_50000 : strText = strText + " 5만 " + pObject->m_paMapPriority[0]->m_strName + strText2;
			break;
		}
		 

		TRACE( _T("\n") );
		TRACE( strFullName );
		
		if( "point" == type_geo )
		{
			pObject->m_pObject = new shpPoint;
			if(!((shpPoint*)pObject->m_pObject)->Open(strFullName))
			{
				TRACE("error: CMapDataMgr::OpenVectorMap => SHPT_POINTZ shape open failed!\n");
				delete pObject->m_pObject;
				pObject->m_pObject = NULL;
				continue;
			}
			pItem->m_bIsVectorDataOpen = TRUE;
		}
		else if( "polyline" == type_geo )
		{
			pObject->m_pObject = new shpPolyLine;
			if(!((shpPolyLine*)pObject->m_pObject)->OpenEx(strFullName, FALSE))
			{
				TRACE("error: CMapDataMgr::OpenVectorMap => SHPT_ARCZ shape open failed!\n");
				delete pObject->m_pObject;
				pObject->m_pObject = NULL;
				continue;
			}
			pItem->m_bIsVectorDataOpen = TRUE;
		}
		else if( "polygon" == type_geo )
		{
			
			pObject->m_pObject = new shpPolygon(pWnd);
			if(!((shpPolygon*)pObject->m_pObject)->Open(strFullName))
			{
				TRACE("error: CMapDataMgr::OpenVectorMap => SHPT_POLYGONZ shape open failed!\n");
				delete pObject->m_pObject;
				pObject->m_pObject = NULL;
				continue;
			}
			pItem->m_bIsVectorDataOpen = TRUE;
		}
		else
			
		{
			TRACE("error: CMapDataMgr::OpenVectorMap => unknown shape type!\n");
			delete pObject->m_pObject;
			pObject->m_pObject = NULL;
			continue;
		}	
		bRes = TRUE;
	}	 
	//OpenMapText(pItem);	

	
	clock_t finishTimeDraw = clock();
 	double drawDuration;
	drawDuration = (double)(finishTimeDraw - startTimeDraw) / CLOCKS_PER_SEC;
	CString strDrawTime;
 	strDrawTime.Format("\nOpenVectorMap 소요시간(%5.3f Sec)", drawDuration);
	TRACE("info: %s\n", strDrawTime);



	return bRes;
}

void CMapDataMgr::OpenMapText(stVectorMap* pItem)
{
	if(pItem == NULL)
		return;

	if(pItem->m_pMapText.GetSize() > 0)
		return;

	CString strPath, strSubPath, strFileName, strFullName, strTmp, strDir1, strDir2;
	strDir1 = strDir2 = "";
	if(!pItem->m_strSubPath1.IsEmpty())
		strDir1 = "\\";
	if(!pItem->m_strSubPath2.IsEmpty())
		strDir2 = "\\";

	switch(pItem->m_enMapScale)
	{
		case MS_1_50000 : strSubPath = "5만\\";
			break;
		case MS_1_100000 : strSubPath = "10만\\";
			break;
		case MS_1_250000 : strSubPath = "25만\\";
			break;
		case MS_1_500000 : strSubPath = "50만\\";
			break;
		case MS_1_1000000 : strSubPath = "100만\\";
			break;
		case MS_1_2000000 : strSubPath = "200만\\";
			break;
	}

	strPath = pItem->m_strRootPath + strSubPath + pItem->m_strSubPath1 + strDir1 + pItem->m_strSubPath2 + strDir2;

	CFileFind find;
	CFileIO file;
	CString strDeli = "\t";
	CString strData, strLeft, strTop, strName, strFullPath;
	double dx, dy;
	CString strFileExt = "*.txt";
	char* pDeli;
	int iCount;
	stMapText* pMapText;

	if(!file.Init(FALSE))
		return;

	BOOL bRet = find.FindFile(strPath + strFileExt);
	if(bRet == FALSE)
		return;

	if(bRet)
	{
		while(find.FindNextFile())
		{
			strFileName = find.GetFileName();
			strFullPath = strPath + strFileName;

			if(!file.Open((LPCTSTR)strFullPath, CFile::modeRead|CFile::typeText))
				break;
			iCount = 0;
			while(file.ReadString(strData))
			{
				if(iCount == 0)
				{
					iCount = 1;
					continue;
				}
				// 경도
				pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
				strLeft.Format("%s", pDeli);
				dx = atof(strLeft);
				// 위도
				pDeli = strtok(NULL, strDeli);
				strTop.Format("%s", pDeli);
				dy = atof(strTop);
				// text
				pDeli = strtok(NULL, strDeli);
				strName.Format("%s", pDeli);

				pMapText = new stMapText;
				pMapText->m_gpPoint.SetPoint(dx, dy);
				pMapText->m_strName = strName;
				pItem->m_pMapText.Add(pMapText);
			}
			file.Close();
		}
		strFileName = find.GetFileName();
		strFullPath = strPath + strFileName;

		if(!file.Open((LPCTSTR)strFullPath, CFile::modeRead|CFile::typeText))
		{
			file.Close();
			find.Close();
			return;
		}
		iCount = 0;
		while(file.ReadString(strData))
		{
			if(iCount == 0)
			{
				iCount = 1;
				continue;
			}
			// 경도
			pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
			strLeft.Format("%s", pDeli);
			dx = atof(strLeft);
			// 위도
			pDeli = strtok(NULL, strDeli);
			strTop.Format("%s", pDeli);
			dy = atof(strTop);
			// text
			pDeli = strtok(NULL, strDeli);
			strName.Format("%s", pDeli);

			pMapText = new stMapText;
			pMapText->m_gpPoint.SetPoint(dx, dy);
			pMapText->m_strName = strName;
			pItem->m_pMapText.Add(pMapText);
		}
		file.Close();
	}

	find.Close();
}

// 복원도시 대상 데이터 추출
BOOL CMapDataMgr::ExtractVectorMap(EGMapScale enScale, CGRectGP rect, double dViewRatio)
{
	clock_t startTimeDraw = clock();

	BOOL bRes = TRUE;
//	stVectorMap* pItem; 
	
	m_dViewRatio = dViewRatio;
	
	// 100만 데이터 처리
	if(enScale == MS_1_1000000)
	{
		ResetCurrentMap();
		if(m_pCurrentMap.GetSize() <= 0)
		{
			//pItem = FindVectorMap(enScale);
			//if(pItem != NULL)
			//	m_pCurrentMap.Add(pItem);

			//CHOIKYUBEOM 100만 지도 수량 추가로 인해..
			stVectorMap* pItem = NULL;
			for( int cnt = 0; cnt <m_vectormaps.GetSize(); ++cnt )
			{
				pItem = (stVectorMap*)m_vectormaps[ cnt ];
				if(pItem->m_enMapScale == enScale)
					m_pCurrentMap.Add(pItem);
			}
		}
	}
	else
	{
		ResetCurrentMap(enScale, rect);
		AddCurrentMap(enScale, rect);
	}



	clock_t finishTimeDraw = clock();
 	double drawDuration;
	drawDuration = (double)(finishTimeDraw - startTimeDraw) / CLOCKS_PER_SEC;
	CString strDrawTime;
 	strDrawTime.Format("\nExtractVectorMap 소요시간(%5.3f Sec)", drawDuration);
	TRACE("info: %s\n", strDrawTime);



	return bRes;
}

 


// 벡터 지도 데이터 reset
void CMapDataMgr::ResetVectorMap(stVectorMap* pItem)
{
	stVObject* pSubItem;
	stMapText* pMapText;
	for(int i=0; i<pItem->m_pVObject.GetSize(); i++)
	{
		pSubItem = (stVObject*)pItem->m_pVObject[i];
		if(pSubItem->m_pObject != NULL)
		{
			delete pSubItem->m_pObject;
			pSubItem->m_pObject = NULL;
		}
	}

	for(int i=0; i<pItem->m_pMapText.GetSize(); i++)
	{
		pMapText = (stMapText*)pItem->m_pMapText[i];
		if(pMapText != NULL)
			delete pMapText;
	}
	pItem->m_pMapText.RemoveAll();
}

// 현재 사용되는 지도데이터 reset
void CMapDataMgr::ResetCurrentMap(EGMapScale enScale, CGRectGP rect)
{
	clock_t startTimeDraw = clock();

	stVectorMap* pItem;
	stVObject* pSubItem;
	stFeatureInfo* pFeature;

	for(int i=0; i<m_pCurrentMapList.GetSize(); )// i 번째를 remove 
	{
		pItem = (stVectorMap*)m_pCurrentMapList[i];
		
		if(pItem->m_enMapScale == enScale && DupCheck(enScale, pItem->m_gpRect, rect))
		{
			i++;
			continue;
		}

		for(int j=0; j<pItem->m_pVObject.GetSize(); j++)
		{
			pSubItem = (stVObject*)pItem->m_pVObject[j];
			if(pSubItem->m_pObject != NULL)
			{
				// 지형요소속성정보 관리 리스트 동기화..
				for(int k=0; k<m_pFeatureInfoList.GetSize(); k++)
				{
					pFeature = (stFeatureInfo*)m_pFeatureInfoList[k];
					if(pFeature->m_pObject == pSubItem)
						pFeature->m_pObject = NULL;
				}

				if(pSubItem->m_pMapProperty->m_enMapProperty == MP_POINT)			delete (shpPoint *)pSubItem->m_pObject;
				else if(pSubItem->m_pMapProperty->m_enMapProperty == MP_POLYLINE)	delete (shpPolyLine *)pSubItem->m_pObject;
				else if(pSubItem->m_pMapProperty->m_enMapProperty == MP_POLYGON)	delete (shpPolygon *)pSubItem->m_pObject;


				pSubItem->m_pObject = NULL;
				pItem->m_bIsVectorDataOpen = FALSE;
			}
		}

		for(int j=0; j<pItem->m_pMapText.GetSize(); j++)
			delete pItem->m_pMapText[j];
		pItem->m_pMapText.RemoveAll();

		m_pCurrentMapList.RemoveAt(i);
	}

		clock_t finishTimeDraw = clock();
 	double drawDuration;
	drawDuration = (double)(finishTimeDraw - startTimeDraw) / CLOCKS_PER_SEC;
	CString strDrawTime;
 	strDrawTime.Format("\nResetCurrentMap 소요시간(%5.3f Sec)", drawDuration);
	TRACE("info: %s\n", strDrawTime);

}

// 현재 사용되는 지도데이터 reset
void CMapDataMgr::ResetCurrentMap()
{
	stVectorMap* pItem;
	stVObject* pSubItem;
	stFeatureInfo* pFeature;

	for(int i=0; i<m_pCurrentMapList.GetSize(); i++)
	{
		pItem = (stVectorMap*)m_pCurrentMapList[i];
		for(int j=0; j<pItem->m_pVObject.GetSize(); j++)
		{
			pSubItem = (stVObject*)pItem->m_pVObject[j];
			if(pSubItem->m_pObject != NULL)
			{
				// 지형요소속성정보 관리 리스트 동기화..
				for(int k=0; k<m_pFeatureInfoList.GetSize(); k++)
				{
					pFeature = (stFeatureInfo*)m_pFeatureInfoList[k];
					if(pFeature->m_pObject == pSubItem)
						pFeature->m_pObject = NULL;
				}
 
				if(pSubItem->m_strFileName.Mid(0,1) == "P")
					delete (shpPoint *)pSubItem->m_pObject;
				else if(pSubItem->m_strFileName.Mid(0,1) == "L")
					delete (shpPolyLine *)pSubItem->m_pObject;
				else if(pSubItem->m_strFileName.Mid(0,1) == "A")
					delete (shpPolygon *)pSubItem->m_pObject;

				pSubItem->m_pObject = NULL;
				pItem->m_bIsVectorDataOpen = FALSE;
			}
		}

		for(int j=0; j<pItem->m_pMapText.GetSize(); j++)
			delete pItem->m_pMapText[j];
		pItem->m_pMapText.RemoveAll();
	}
	m_pCurrentMapList.RemoveAll();
}

// 벡터 지도데이터 찾기
stVectorMap* CMapDataMgr::FindVectorMap(EGMapScale enScale)
{
	stVectorMap* pItem = NULL;
	for(int i=0; i<m_vectormaps.GetSize(); i++)
	{
		pItem = (stVectorMap*)m_vectormaps[i];
		if(pItem->m_enMapScale == enScale)
			return pItem;
	}
	return NULL;
}

// 현제 사용되는 벡터 지도데이터 찾기
BOOL CMapDataMgr::FindCurrentMap(EGMapScale enScale)
{
	stVectorMap* pItem = NULL;
	for(int i=0; i<m_pCurrentMapList.GetSize(); i++)
	{
		pItem = (stVectorMap*)m_pCurrentMapList[i];
		if(pItem->m_enMapScale == enScale)
			return TRUE;
	}
	return FALSE;
}

void CMapDataMgr::AddCurrentMap(EGMapScale enScale, CGRectGP rect)
{
	clock_t startTimeDraw = clock();

	stVectorMap * pItem;
 


	int cnt_vector = m_vectormaps.GetSize();	
	//CHOIKYUBEOM 지도도시 성능개선 
	//1) 해당 축척에 맞는 것만 루프 돌도록 수정하자
	//축척별로 레이어 개수를 저장하고 있자 

	/*
	//이것은 변수로 관리해하도록 수정해야 한다!!!!꼭!!! 
	int cnt_ = 0;
	switch(enScale)
	{
	case MS_1_50000 : 
		cnt_ = 0;
		cnt_vector = 186;
		break;
	case MS_1_100000 : 
		cnt_ = 187;
		cnt_vector = 352;
		break;
	case MS_1_250000 : 
		cnt_ = 353;
		//cnt_vector = cnt_vector;
		break;
	case MS_1_500000 : 
		cnt_ = 353;
		//cnt_vector = cnt_vector;
		break;
	case MS_1_1000000 : 
		cnt_ = 353;
		//cnt_vector = cnt_vector;
		break;
	case MS_1_2000000 : 
		cnt_ = 560;
		//cnt_vector = cnt_vector;
		break;
	}

	*/

	//for( cnt_ ; cnt_< cnt_vector; ++cnt_ )
	for( int  cnt_ = 0 ; cnt_< cnt_vector; ++cnt_ )
	{


	pItem = (stVectorMap*)m_vectormaps[cnt_];
 
	// 이것을 살리고..
	if(pItem->m_enMapScale != enScale || pItem->m_bIsVectorDataOpen == TRUE)	continue;
		

		//2) 활성화 되 있는 레이어만 검사하도록 하자 
	//이것은 필터창 값을 가져와서 검사해야 한다.

  
	//	stVectorMap * p_vector =  CMapDataMgr::GetVectorMapItem( cnt_ );
	CMapPriority * p_priority = GetMapPriority( cnt_ );
	
	//if( p_priority->m_bIsVisible == FALSE ) 
	//	continue;



	//3) rtree를 적용하자 		
		if(DupCheck(enScale, pItem->m_gpRect, rect))
		{
			//4) 파일 읽는 부분을 최적화 하자 
			//마지막으로 메모리 맵을 적용해 보자 
			//dbf는 필요할때만 로드할 수 있도록 해 보자 
	 
			if(OpenVectorMap(pItem))
				m_pCurrentMapList.Add(pItem);
		}
	}

	clock_t finishTimeDraw = clock();
 	double drawDuration;
	drawDuration = (double)(finishTimeDraw - startTimeDraw) / CLOCKS_PER_SEC;
	CString strDrawTime;
 	strDrawTime.Format("\nAddCurrentMap 소요시간(%5.3f Sec)", drawDuration);
	TRACE("info: %s\n", strDrawTime);
}

void CMapDataMgr::UpdateCurrentMap()
{
	stVectorMap* pItem;
	stVObject* pVObject;
	CMapPriority* pPriority;
	shpObject* pShpObject;
	BOOL bIsOpen;
	for(int i=0; i<m_pCurrentMapList.GetSize(); i++)
	{
		pItem = m_pCurrentMapList[i];
		for(int j=0; j<pItem->m_pVObject.GetSize(); j++)
		{
			pVObject = pItem->m_pVObject[j];
			bIsOpen = FALSE;
			if(pVObject != NULL)
			{
				for(int k=0; k<pVObject->m_paMapPriority.GetSize(); k++)
				{
					pPriority = pVObject->m_paMapPriority[k];
					if(pPriority->m_bIsVisible == TRUE)
					{
						bIsOpen = TRUE;
						break;
					}
				}
				if(bIsOpen == FALSE) continue;
				pShpObject = pVObject->m_pObject;
				if(pShpObject == NULL)
					OpenVectorMap(pItem);
			}
		}
	}

	//if(m_bIsProgressDlgView)
	//{
	//	m_pProgressDlg->ShowWindow(SW_HIDE);
	//	m_bIsProgressDlgView = FALSE;
	//}
}

// 영역 중복 체크(벡터)
BOOL CMapDataMgr::DupCheck(EGMapScale enScale, CGRectGP rect, CGRectGP selrect)
{
	CGPointGP gpLT, gpRT, gpLB, gpRB;
	double dcorrect = 0.001;

	switch(enScale)
	{
	case MS_1_2000000 :
	case MS_1_1000000 :
	case MS_1_500000 :
	case MS_1_250000 :
		dcorrect = 0.05;
	case MS_1_100000 :
		dcorrect = 0.05;
	case MS_1_50000 :
		dcorrect = 0.25;
	}

	rect.m_R.left -= dcorrect;
	rect.m_R.top += dcorrect;
	rect.m_R.right += dcorrect;
	rect.m_R.bottom -= dcorrect;
	gpLT.m_P.lon = selrect.m_R.left;
	gpLT.m_P.lat = selrect.m_R.top;
	gpRT.m_P.lon = selrect.m_R.right;
	gpRT.m_P.lat = selrect.m_R.top;
	gpLB.m_P.lon = selrect.m_R.left;
	gpLB.m_P.lat = selrect.m_R.bottom;
	gpRB.m_P.lon = selrect.m_R.right;
	gpRB.m_P.lat = selrect.m_R.bottom;
	if(rect.PtInRect(gpLT) || rect.PtInRect(gpRT) ||
		rect.PtInRect(gpLB) || rect.PtInRect(gpRB))
		return TRUE;

	rect.m_R.left += dcorrect;
	rect.m_R.top -= dcorrect;
	rect.m_R.right -= dcorrect;
	rect.m_R.bottom += dcorrect;
	selrect.m_R.left -= dcorrect;
	selrect.m_R.top += dcorrect;
	selrect.m_R.right += dcorrect;
	selrect.m_R.bottom -= dcorrect;

	gpLT.m_P.lon = rect.m_R.left;
	gpLT.m_P.lat = rect.m_R.top;
	gpRT.m_P.lon = rect.m_R.right;
	gpRT.m_P.lat = rect.m_R.top;
	gpLB.m_P.lon = rect.m_R.left;
	gpLB.m_P.lat = rect.m_R.bottom;
	gpRB.m_P.lon = rect.m_R.right;
	gpRB.m_P.lat = rect.m_R.bottom;
	if(selrect.PtInRect(gpLT) || selrect.PtInRect(gpRT) ||
		selrect.PtInRect(gpLB) || selrect.PtInRect(gpRB))
		return TRUE;

	return FALSE;
}

// 영역 중복 체크(레스터,위성영상)
BOOL CMapDataMgr::DupCheck2(CGRectGP rect, CGRectGP selrect, EGMapScale scale)
{
	CGPointGP gpLT, gpRT, gpLB, gpRB;
	double offset = 0.0;

	switch(scale)
	{
		case MS_1_1000000 : offset = 2;
			break;
		case MS_1_500000 : offset = 0.1;
			break;
		case MS_1_250000 : offset = 0.1;
			break;
		case MS_1_20 : offset = 0.1;
			break;
		case MS_1_10 : offset = 0.1;
			break;
		case MS_1_1 : offset = 0.01;
			break;
	}

	rect.m_R.left -= offset;
	rect.m_R.top += offset;
	rect.m_R.right += offset;
	rect.m_R.bottom -= offset;
	gpLT.m_P.lon = selrect.m_R.left;
	gpLT.m_P.lat = selrect.m_R.top;
	gpRT.m_P.lon = selrect.m_R.right;
	gpRT.m_P.lat = selrect.m_R.top;
	gpLB.m_P.lon = selrect.m_R.left;
	gpLB.m_P.lat = selrect.m_R.bottom;
	gpRB.m_P.lon = selrect.m_R.right;
	gpRB.m_P.lat = selrect.m_R.bottom;
	if(rect.PtInRect(gpLT) || rect.PtInRect(gpRT) ||
		rect.PtInRect(gpLB) || rect.PtInRect(gpRB))
		return TRUE;

	rect.m_R.left += offset;
	rect.m_R.top -= offset;
	rect.m_R.right -= offset;
	rect.m_R.bottom += offset;
	selrect.m_R.left -= offset;
	selrect.m_R.top += offset;
	selrect.m_R.right += offset;
	selrect.m_R.bottom -= offset;

	gpLT.m_P.lon = rect.m_R.left;
	gpLT.m_P.lat = rect.m_R.top;
	gpRT.m_P.lon = rect.m_R.right;
	gpRT.m_P.lat = rect.m_R.top;
	gpLB.m_P.lon = rect.m_R.left;
	gpLB.m_P.lat = rect.m_R.bottom;
	gpRB.m_P.lon = rect.m_R.right;
	gpRB.m_P.lat = rect.m_R.bottom;
	if(selrect.PtInRect(gpLT) || selrect.PtInRect(gpRT) ||
		selrect.PtInRect(gpLB) || selrect.PtInRect(gpRB))
		return TRUE;

	return FALSE;
}

// 도시요소 우선순위 정보 추가(점)
void CMapDataMgr::AddMapPriority(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
								 BOOL bIsVisible, CString strName, 
								 CString earth_or_air, CString type_geo,
								 UINT uiRadius, COLORREF crFillColor, 
								 UINT uiType, CString strBitmap, int iViewRatio)
{
	CPointPriority* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_mappriority.GetUpperBound();
	int iMid;
	int iCmp;

	CString strKey1, strKey2;
	strKey1.Format("%04d", uiPriority);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%04d", m_mappriority[iMid]->m_uiPriority);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
			pItem = (CPointPriority *)m_mappriority[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	if(!bFound)
		pItem = new CPointPriority;

	pItem->m_uiPriority = uiPriority;
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map priority add failed!(CMapDataMgr::AddMapPriority)\n");
		if(!bFound)
			delete pItem;
		return;
	}

	strFileName.MakeUpper();
	strBitmap.MakeUpper();

	pItem->m_enMapScale = enScale;
	pItem->m_strFileName = strFileName;
	pItem->m_bTransParent = bTransparent;
	pItem->m_bIsVisible = bIsVisible;
	pItem->m_strName = strName;
	pItem->m_uiRadius = uiRadius;
	pItem->m_crFillColor = crFillColor;
	pItem->m_uiType = uiType;
	pItem->m_strBitmap = strBitmap;
	pItem->m_pSymbol = NULL;
	pItem->m_iViewRatio = iViewRatio;

	pItem->m_earth_or_air = earth_or_air;
	pItem->m_type_geo = type_geo;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_mappriority.GetUpperBound())
			m_mappriority.Add(pItem);
		else
			m_mappriority.InsertAt(iLow, pItem);
	}
}

// 도시요소 우선순위 정보 추가(선)
void CMapDataMgr::AddMapPriority2(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
								  BOOL bIsVisible, CString strName, 
								  CString earth_or_air, CString type_geo,
								  UINT uiLineWidth, COLORREF crLineColor, 
								  UINT uiType, UINT uiLineWidth2, COLORREF crLineColor2, UINT uiType2, int iViewRatio)
{
	CPolylinePriority* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_mappriority.GetUpperBound();
	int iMid;
	int iCmp;

	CString strKey1, strKey2;
	strKey1.Format("%04d", uiPriority);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%04d", m_mappriority[iMid]->m_uiPriority);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
			pItem = (CPolylinePriority*)m_mappriority[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	if(!bFound)
		pItem = new CPolylinePriority;

	pItem->m_uiPriority = uiPriority;
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map priority add failed!(CMapDataMgr::AddMapPriority)\n");
		if(!bFound)
			delete pItem;
		return;
	}

	strFileName.MakeUpper();

	pItem->m_enMapScale = enScale;
	pItem->m_strFileName = strFileName;
	pItem->m_bTransParent = bTransparent;
	pItem->m_bIsVisible = bIsVisible;
	pItem->m_strName = strName;
	pItem->m_uiLineWidth = uiLineWidth;
	pItem->m_crLineColor = crLineColor;
	pItem->m_uiType = uiType;
	pItem->m_uiLineWidth2 = uiLineWidth2;
	pItem->m_crLineColor2 = crLineColor2;
	pItem->m_uiType2 = uiType2;
	pItem->m_iViewRatio = iViewRatio;

	pItem->m_earth_or_air = earth_or_air;
	pItem->m_type_geo = type_geo;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_mappriority.GetUpperBound())
			m_mappriority.Add(pItem);
		else
			m_mappriority.InsertAt(iLow, pItem);
	}
}

// 도시요소 우선순위 정보 추가(면)
void CMapDataMgr::AddMapPriority3(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
								  BOOL bIsVisible, CString strName, 
								  CString earth_or_air, CString type_geo,
								  UINT uiLineWidth, COLORREF crLineColor, 
								  COLORREF crFillColor, UINT uiType, CString strBitmap, int iViewRatio)
{
	CPolygonPriority* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_mappriority.GetUpperBound();
	int iMid;
	int iCmp;

	CString strKey1, strKey2;
	strKey1.Format("%04d", uiPriority);

	// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%04d", m_mappriority[iMid]->m_uiPriority);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
			pItem = (CPolygonPriority *)m_mappriority[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	if(!bFound)
		pItem = new CPolygonPriority;

	pItem->m_uiPriority = uiPriority;
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map priority add failed!(CMapDataMgr::AddMapPriority)\n");
		if(!bFound)
			delete pItem;
		return;
	}

	strFileName.MakeUpper();
	strBitmap.MakeUpper();

	pItem->m_enMapScale = enScale;
	pItem->m_strFileName = strFileName;
	pItem->m_bTransParent = bTransparent;
	pItem->m_bIsVisible = bIsVisible;
	pItem->m_strName = strName;
	pItem->m_uiLineWidth = uiLineWidth;
	pItem->m_crLineColor = crLineColor;
	pItem->m_crFillColor = crFillColor;
	pItem->m_uiType = uiType;
	pItem->m_strBitmap = strBitmap;
	pItem->m_pPattern = NULL;
	pItem->m_iViewRatio = iViewRatio;

	pItem->m_earth_or_air = earth_or_air;
	pItem->m_type_geo = type_geo;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_mappriority.GetUpperBound())
			m_mappriority.Add(pItem);
		else
			m_mappriority.InsertAt(iLow, pItem);
	}
}

// 도시요소 우선순위 정보 찾기
CMapPriority* CMapDataMgr::FindMapPriority(EGMapScale enScale, CString strFileName)
{
	CMapPriority* pItem;
	for(int i=0; i<m_mappriority.GetSize(); i++)
	{
		pItem = (CMapPriority*)m_mappriority[i];
		if(enScale == pItem->m_enMapScale && strFileName == pItem->m_strFileName)
			return m_mappriority[i];
	}

	return NULL;
}

int CMapDataMgr::GetMapPriorityCnt()
{
	return m_mappriority.GetSize();
}

CMapPriority* CMapDataMgr::GetMapPriority(int iIndex)
{
	int cnt_map = m_mappriority.GetSize();
	
	if( iIndex >= cnt_map )
		return NULL;

	return m_mappriority[iIndex];
}

void CMapDataMgr::ClearMapPriority()
{
	for(int i=0; i<m_mappriority.GetSize(); i++)
	{
		if(m_mappriority[i]->m_type_geo == "point")
			delete (CPointPriority *)m_mappriority.GetAt(i);
		
		else if(m_mappriority[i]->m_type_geo == "polyline")
			delete (CPolylinePriority *)m_mappriority.GetAt(i);
		
		else if(m_mappriority[i]->m_type_geo == "polygon")
			delete (CPolygonPriority *)m_mappriority.GetAt(i);
	}
	m_mappriority.RemoveAll();
}

void CMapDataMgr::ClearMapPriority(stUserMapInfo *pUserMapInfo)
{
	if(pUserMapInfo == NULL)
		return;
	
	for(int i=0; i<pUserMapInfo->m_mappriority.GetSize(); i++)
	{
		if(pUserMapInfo->m_mappriority[i]->m_type_geo == "point")
			delete (CPointPriority *)pUserMapInfo->m_mappriority.GetAt(i);
		
		else if(pUserMapInfo->m_mappriority[i]->m_type_geo == "polyline")
			delete (CPolylinePriority *)pUserMapInfo->m_mappriority.GetAt(i);
		
		else if(pUserMapInfo->m_mappriority[i]->m_type_geo == "polygon")
			delete (CPolygonPriority *)pUserMapInfo->m_mappriority.GetAt(i);
	}
	pUserMapInfo->m_mappriority.RemoveAll();
	
	for(int i=0; i<pUserMapInfo->m_pTextPriority.GetSize(); i++)
		delete pUserMapInfo->m_pTextPriority[i];
	pUserMapInfo->m_pTextPriority.RemoveAll();
}

void CMapDataMgr::AddMapPriority(CMapPriority* pItem)
{
	m_mappriority.Add(pItem);
}

//void CMapDataMgr::UpdateMapPriority(CFile& file, CMapPriority* pItem)
void CMapDataMgr::UpdateMapPriority(CMapPriority* pItem)
{
	CMapPriority* pPriority;
	CPointPriority* pPoint;
	CPolylinePriority* pLine;
	CPolygonPriority* pPolygon;
	CPointPriority* pNewPoint;
	CPolylinePriority* pNewLine;
	CPolygonPriority* pNewPolygon;

	for(int i=0; i<m_mappriority.GetSize(); i++)
	{
		pPriority = (CMapPriority*)m_mappriority[i];
		if(pPriority->m_enMapScale == pItem->m_enMapScale && 
			pPriority->m_strFileName == pItem->m_strFileName &&
			pPriority->m_uiPriority == pItem->m_uiPriority)
		{
			pPriority->m_bIsVisible = pItem->m_bIsVisible;
			pPriority->m_bTransParent = pItem->m_bTransParent;
			pPriority->m_iViewRatio = pItem->m_iViewRatio;
			//
			//

			if(pPriority->m_type_geo == "point")
			{
				pPoint = (CPointPriority*)pPriority;
				pNewPoint = (CPointPriority*)pItem;
				pPoint->m_crFillColor = pNewPoint->m_crFillColor;
				pPoint->m_uiRadius = pNewPoint->m_uiRadius;
				pPoint->m_uiType = pNewPoint->m_uiType;
				pPoint->m_strBitmap = pNewPoint->m_strBitmap;
				pPoint->m_pSymbol = pNewPoint->m_pSymbol;
			}
			else if(pPriority->m_type_geo == "polyline")
			{
				pLine = (CPolylinePriority*)pPriority;
				pNewLine = (CPolylinePriority*)pItem;
				pLine->m_crLineColor = pNewLine->m_crLineColor;
				pLine->m_crLineColor2 = pNewLine->m_crLineColor2;
				pLine->m_uiLineWidth = pNewLine->m_uiLineWidth;
				pLine->m_uiLineWidth2 = pNewLine->m_uiLineWidth2;
				pLine->m_uiType = pNewLine->m_uiType;
				pLine->m_uiType2 = pNewLine->m_uiType2;
			}
			else if(pPriority->m_type_geo == "polygon")
			{
				pPolygon = (CPolygonPriority*)pPriority;
				pNewPolygon = (CPolygonPriority*)pItem;
				pPolygon->m_crFillColor = pNewPolygon->m_crFillColor;
				pPolygon->m_crLineColor = pNewPolygon->m_crLineColor;
				pPolygon->m_uiLineWidth = pNewPolygon->m_uiLineWidth;
				pPolygon->m_uiType = pNewPolygon->m_uiType;
				pPolygon->m_strBitmap = pNewPolygon->m_strBitmap;
				pPolygon->m_pPattern = pNewPolygon->m_pPattern;
			}
		}
	}
}

void CMapDataMgr::UpdateTextPriority(stTextPriority* pItem)
{
	stTextPriority* pTextPriority;
	for(int i=0; i<m_pTextPriority.GetSize(); i++)
	{
		pTextPriority = (stTextPriority*)m_pTextPriority[i];
		if(!pItem->m_strWorldMapPoint.IsEmpty())
		{
			if(pItem->m_strWorldMapPoint == pTextPriority->m_strWorldMapPoint)
			{
				pTextPriority->m_bIsVisible = pItem->m_bIsVisible;
				pTextPriority->m_clFontColor = pItem->m_clFontColor;
				pTextPriority->m_uiFontSize = pItem->m_uiFontSize;
				pTextPriority->m_uiViewRatio = pItem->m_uiViewRatio;
				pTextPriority->m_uiVisibleType = pItem->m_uiVisibleType;
				pTextPriority->m_strFontName = pItem->m_strFontName;
				pTextPriority->m_strFontScript = pItem->m_strFontScript;
				pTextPriority->m_strWorldMapPoint = pItem->m_strWorldMapPoint;
				pTextPriority->m_pFontType = pItem->m_pFontType;
			}
		}
		else
		{
			if(pItem->m_enMapScale == pTextPriority->m_enMapScale)
			{
				pTextPriority->m_bIsVisible = pItem->m_bIsVisible;
				pTextPriority->m_clFontColor = pItem->m_clFontColor;
				pTextPriority->m_uiFontSize = pItem->m_uiFontSize;
				pTextPriority->m_uiViewRatio = pItem->m_uiViewRatio;
				pTextPriority->m_uiVisibleType = pItem->m_uiVisibleType;
				pTextPriority->m_strFontName = pItem->m_strFontName;
				pTextPriority->m_strFontScript = pItem->m_strFontScript;
				pTextPriority->m_strWorldMapPoint = pItem->m_strWorldMapPoint;
				pTextPriority->m_pFontType = pItem->m_pFontType;
			}
		}
	}
}

// 도시요소 사용자 우선순위 정보 추가(점)
void CMapDataMgr::AddDefaultMapPriority(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
										BOOL bIsVisible, CString strName, UINT uiRadius, COLORREF crFillColor, 
										UINT uiType, CString strBitmap, int iViewRatio)
{
/*
	CPointPriority* pItem = new CPointPriority;
	pItem->m_uiPriority = uiPriority;
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map priority add failed!(CMapDataMgr::AddDefaultMapPriority)\n");
		delete pItem;
		return;
	}
	strFileName.MakeUpper();
	strBitmap.MakeUpper();
	pItem->m_enMapScale = enScale;
	pItem->m_strFileName = strFileName;
	pItem->m_bTransParent = bTransparent;
	pItem->m_bIsVisible = bIsVisible;
	pItem->m_strName = strName;
	pItem->m_uiRadius = uiRadius;
	pItem->m_crFillColor = crFillColor;
	pItem->m_uiType = uiType;
	pItem->m_pSymbol = NULL;
	pItem->m_iViewRatio = iViewRatio;
	pItem->m_strBitmap = strBitmap;
	m_defaultmappriority.Add((CMapPriority *)pItem);
*/

	CMapPriority* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_defaultmappriority.GetUpperBound();
	int iMid;
	int iCmp;

	CString strKey1, strKey2;
	strKey1.Format("%04d", uiPriority);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%04d", m_defaultmappriority[iMid]->m_uiPriority);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
			pItem = m_defaultmappriority[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	if(!bFound)
		pItem = new CPointPriority;

	pItem->m_uiPriority = uiPriority;
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map priority add failed!(CMapDataMgr::AddDefaultMapPriority)\n");
		if(!bFound)
			delete pItem;
		return;
	}

	strFileName.MakeUpper();
	strBitmap.MakeUpper();

	pItem->m_enMapScale = enScale;
	pItem->m_strFileName = strFileName;
	pItem->m_bTransParent = bTransparent;
	pItem->m_bIsVisible = bIsVisible;
	pItem->m_strName = strName;

	CPointPriority* pItem2 = (CPointPriority*)pItem;
	pItem2->m_uiRadius = uiRadius;
	pItem2->m_crFillColor = crFillColor;
	pItem2->m_uiType = uiType;
	pItem2->m_pSymbol = NULL;
	pItem2->m_iViewRatio = iViewRatio;
	pItem2->m_strBitmap = strBitmap;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_defaultmappriority.GetUpperBound())
			m_defaultmappriority.Add(pItem);
		else
			m_defaultmappriority.InsertAt(iLow, pItem);
	}
}

// 도시요소 우선순위 정보 추가(선)
void CMapDataMgr::AddDefaultMapPriority2(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
										 BOOL bIsVisible, CString strName, UINT uiLineWidth, COLORREF crLineColor, 
										 UINT uiType, UINT uiLineWidth2, COLORREF crLineColor2, UINT uiType2, int iViewRatio)
{
	CMapPriority* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_defaultmappriority.GetUpperBound();
	int iMid;
	int iCmp;

	CString strKey1, strKey2;
	strKey1.Format("%04d", uiPriority);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%04d", m_defaultmappriority[iMid]->m_uiPriority);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
			pItem = m_defaultmappriority[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	if(!bFound)
		pItem = new CPolylinePriority;

	pItem->m_uiPriority = uiPriority;
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map priority add failed!(CMapDataMgr::AddDefaultMapPriority)\n");
		if(!bFound)
			delete pItem;
		return;
	}

	strFileName.MakeUpper();

	pItem->m_enMapScale = enScale;
	pItem->m_strFileName = strFileName;
	pItem->m_bTransParent = bTransparent;
	pItem->m_bIsVisible = bIsVisible;
	pItem->m_strName = strName;

	CPolylinePriority* pItem2 = (CPolylinePriority*)pItem;
	pItem2->m_uiLineWidth = uiLineWidth;
	pItem2->m_crLineColor = crLineColor;
	pItem2->m_uiType = uiType;
	pItem2->m_uiLineWidth2 = uiLineWidth2;
	pItem2->m_crLineColor2 = crLineColor2;
	pItem2->m_uiType2 = uiType2;
	pItem2->m_iViewRatio = iViewRatio;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_defaultmappriority.GetUpperBound())
			m_defaultmappriority.Add(pItem);
		else
			m_defaultmappriority.InsertAt(iLow, pItem);
	}
}

// 도시요소 우선순위 정보 추가(면)
void CMapDataMgr::AddDefaultMapPriority3(UINT uiPriority, CString strScale, CString strFileName, BOOL bTransparent, 
										 BOOL bIsVisible, CString strName, UINT uiLineWidth, COLORREF crLineColor, 
										 COLORREF crFillColor, UINT uiType, CString strBitmap, int iViewRatio)
{
	CMapPriority* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_defaultmappriority.GetUpperBound();
	int iMid;
	int iCmp;

	CString strKey1, strKey2;
	strKey1.Format("%04d", uiPriority);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%04d", m_defaultmappriority[iMid]->m_uiPriority);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
			pItem = m_defaultmappriority[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	if(!bFound)
		pItem = new CPolygonPriority;

	pItem->m_uiPriority = uiPriority;
	EGMapScale enScale = ConvertMapScale(strScale);
	if(enScale == MS_ERROR)
	{
		TRACE("error: map priority add failed!(CMapDataMgr::AddDefaultMapPriority)\n");
		if(!bFound)
			delete pItem;
		return;
	}

	strFileName.MakeUpper();
	strBitmap.MakeUpper();

	pItem->m_enMapScale = enScale;
	pItem->m_strFileName = strFileName;
	pItem->m_bTransParent = bTransparent;
	pItem->m_bIsVisible = bIsVisible;
	pItem->m_strName = strName;

	CPolygonPriority* pItem2 = (CPolygonPriority*)pItem;
	pItem2->m_uiLineWidth = uiLineWidth;
	pItem2->m_crLineColor = crLineColor;
	pItem2->m_crFillColor = crFillColor;
	pItem2->m_uiType = uiType;
	pItem2->m_strBitmap = strBitmap;
	pItem2->m_pPattern = NULL;
	pItem2->m_iViewRatio = iViewRatio;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_defaultmappriority.GetUpperBound())
			m_defaultmappriority.Add(pItem);
		else
			m_defaultmappriority.InsertAt(iLow, pItem);
	}
}

// 도시요소 우선순위 정보 찾기
CMapPriority* CMapDataMgr::FindDefaultMapPriority(EGMapScale enScale, CString strFileName)
{
	CMapPriority* pItem;
	for(int i=0; i<m_defaultmappriority.GetSize(); i++)
	{
		pItem = (CMapPriority*)m_defaultmappriority[i];
		if(enScale == pItem->m_enMapScale && strFileName == pItem->m_strFileName)
		{
			return m_defaultmappriority[i];
		}
	}

	return NULL;
}

// 기본 맵 정보 리스트 개수 얻기
int CMapDataMgr::GetDefaultMapPriorityCnt()
{
	return m_defaultmappriority.GetSize();
}

// 기본 맵 정보 얻기
CMapPriority* CMapDataMgr::GetDefaultMapPriority(int iIndex)
{
	return m_defaultmappriority[iIndex];
}

// 맵 scale type 변환(user type => string)
CString CMapDataMgr::ConvertMapScale(EGMapScale enScale)
{
	CString strScale = _T("");
	switch(enScale)
	{
		case MS_1_50000 : strScale = "0005";
			break;
		case MS_1_100000 : strScale = "0010";
			break;
		case MS_1_250000 : strScale = "0025";
			break;
		case MS_1_500000 : strScale = "0050";
			break;
		case MS_1_1000000 : strScale = "0100";
			break;
		case MS_1_2000000 : strScale = "0200";
			break;
		default : TRACE("error: map scale convert failed(1)!(CMapDataMgr::ConvertMapScale)\n");
			break;
	}

	return strScale;
}

// 맵 scale type 변환(string => user type)
EGMapScale CMapDataMgr::ConvertMapScale(CString strScale)
{
	EGMapScale enScale = MS_ERROR;
	UINT uiScale = atoi(strScale);
	switch(uiScale)
	{
		case 5 : enScale = MS_1_50000;
			break;
		case 10 : enScale = MS_1_100000;
			break;
		case 25 : enScale = MS_1_250000;
			break;
		case 50 : enScale = MS_1_500000;
			break;
		case 100 : enScale = MS_1_1000000;
			break;
		case 200 : enScale = MS_1_2000000;
			break;
		default : TRACE("error: map scale convert failed(2)!(CMapDataMgr::ConvertMapScale)\n");
			break;
	}

	return enScale;
}

// 50,25,10,5만 벡터 지도 데이터 개수 얻기
int CMapDataMgr::GetCurrentMapListCnt()
{
	return m_pCurrentMapList.GetSize();
};

// 50,25,10,5만 벡터 지도 데이터 얻기
stVectorMap* CMapDataMgr::GetCurrentMapList(int iIndex)
{
	return m_pCurrentMapList[iIndex];
};

// 100만 벡터 지도 데이터 얻기
int CMapDataMgr::GetCurrentMapCnt()
{
	return m_pCurrentMap.GetSize();
};

// 100만 벡터 지도 데이터 얻기
stVectorMap* CMapDataMgr::GetCurrentMap(int iIndex)
{
	return m_pCurrentMap[iIndex];
};

// 맵 기본정보 리스트("mapproperty.txt") 자동 구성
void CMapDataMgr::CreateDefaultMapProperty(CString strScale)
{
	stVectorMap* pItem;
	CString strData, strMapProperty, strPath, strSubPath, strFileName, strFullName, strTmp, strDir1, strDir2;
	strDir1 = strDir2 = "";
	// general item
	CString strMapkey, strScale2, strFileName2, strName, strKind, strIsVisible;
	// point item
	CString strPoint[5];
	// polyline item
	CString strPolyline[3];
	// polygon item
	CString strPolygon[5];

	int nShapeType;
	FILE* fpSHP;
	CFile file;
	CFileFind find;
	CString fileExt = "*.shp";
	BOOL ret1;
	BOOL bOpen = FALSE;

	strMapProperty = "mapproperty(" + strScale + ").txt";

	TRACE("info: mapproperty file create start!!!\n");

	for(int i=0; i<m_vectormaps.GetSize(); i++)
	{
		pItem = (stVectorMap*)m_vectormaps[i];

		if(strScale == "5")
		{
			if(pItem->m_enMapScale != MS_1_50000)
				continue;
		}
		else if(strScale == "10")
		{
			if(pItem->m_enMapScale != MS_1_100000)
				continue;
		}
		else if(strScale == "25")
		{
			if(pItem->m_enMapScale != MS_1_250000)
				continue;
		}
		else if(strScale == "50")
		{
			if(pItem->m_enMapScale != MS_1_500000)
				continue;
		}
		else if(strScale == "100")
		{
			if(pItem->m_enMapScale != MS_1_1000000)
				continue;
		}
		else if(strScale == "200")
		{
			if(pItem->m_enMapScale != MS_1_2000000)
				continue;
		}
		if(!pItem->m_strSubPath1.IsEmpty())
			strDir1 = "\\";
		if(!pItem->m_strSubPath2.IsEmpty())
			strDir2 = "\\";

		strPath = pItem->m_strRootPath + strScale + "만" + "\\" + pItem->m_strSubPath1 + strDir1 + pItem->m_strSubPath2 + strDir2;
		strMapProperty = pItem->m_strRootPath + strMapProperty;

		if(!bOpen)
		{
			if(!file.Open(strMapProperty, CFile::modeCreate | CFile::modeWrite))
			{
				TRACE("error: CreateDefaultMapProperty => file open failed!\n");
				find.Close();
				return;
			}
			bOpen = TRUE;
		}

		ret1 = find.FindFile(strPath+"\\"+fileExt);
		if(ret1)
		{
			while(find.FindNextFile())
			{
				strData = "";
				strFileName = find.GetFileName();
				strFileName.MakeUpper();
				strTmp = strFileName.Left(strFileName.GetLength()-4);
				strFullName = strPath + "\\" + strFileName;

				fpSHP = fopen(strFullName, "rb");
				if(fpSHP == NULL) continue;

				// get shp type
				fseek(fpSHP, 32L, SEEK_SET);
				fread(&nShapeType, 4, 1, fpSHP);

				strMapkey.Format("%d", pItem->m_iMapKey);
				strScale2 = strScale;
				strFileName2 = strTmp;
				strName = "NULL";

				strData = strMapkey + "," + strScale2 + "," + strFileName2 + ",";

				switch(nShapeType)
				{
				case SHPT_POINT:
				case SHPT_POINTZ:
				case SHPT_POINTM:
				case SHPT_MULTIPOINT:
				case SHPT_MULTIPOINTZ:
				case SHPT_MULTIPOINTM:
						{
							strKind = "0";
							strIsVisible = "1";
							strPoint[0] = "0";
							strPoint[1] = "2";
							strPoint[2] = "000000000";
							strPoint[3] = "NULL";
							strPoint[4] = "1";

							strData += strKind + "," + strIsVisible + ",";
							strData += strPoint[0] + "," + 
										strPoint[1] + "," + 
										strPoint[2] + "," + 
										strPoint[3] + "," + 
										strPoint[4];
						}
						break;
				case SHPT_ARC:
				case SHPT_ARCZ:
				case SHPT_ARCM:
						{
							strKind = "1";
							strIsVisible = "1";
							strPolyline[0] = "000000000";
							strPolyline[1] = "1";
							strPolyline[2] = "0";

							strData += strKind + "," + strIsVisible + ",";
							strData += strPolyline[0] + "," + 
										strPolyline[1] + "," + 
										strPolyline[2];
						}
						break;
				case SHPT_POLYGON:
				case SHPT_POLYGONZ:
				case SHPT_POLYGONM:
						{
							strKind = "2";
							strIsVisible = "1";
							strPolygon[0] = "0";
							strPolygon[1] = "1";
							strPolygon[2] = "255255255";
							strPolygon[3] = "000000000";
							strPolygon[4] = "NULL";

							strData += strKind + "," + strIsVisible + ",";
							strData += strPolygon[0] + "," + 
										strPolygon[1] + "," + 
										strPolygon[2] + "," + 
										strPolygon[3] + "," + 
										strPolygon[4];
						}
						break;
				}
				fclose(fpSHP);

				strData += "\r\n";

				file.Write(strData, strData.GetLength());
			}

			strData = "";
			strFileName = find.GetFileName();
			strFileName.MakeUpper();
			strTmp = strFileName.Left(strFileName.GetLength()-4);
			strFullName = strPath + "\\" + strFileName;

			fpSHP = fopen(strFullName, "rb");
			if(fpSHP == NULL) continue;

			// get shp type
			fseek(fpSHP, 32L, SEEK_SET);
			fread(&nShapeType, 4, 1, fpSHP);

			strMapkey.Format("%d", pItem->m_iMapKey);
			strScale2 = strScale;
			strFileName2 = strTmp;
			strName = "NULL";

			strData = strMapkey + "," + strScale2 + "," + strFileName2 + ",";

			switch(nShapeType)
			{
				case SHPT_POINT:
				case SHPT_POINTZ:
				case SHPT_POINTM:
				case SHPT_MULTIPOINT:
				case SHPT_MULTIPOINTZ:
				case SHPT_MULTIPOINTM:
					{
						strKind = "0";
						strIsVisible = "1";
						strPoint[0] = "0";
						strPoint[1] = "2";
						strPoint[2] = "000000000";
						strPoint[3] = "NULL";
						strPoint[4] = "1";

						strData += strKind + "," + strIsVisible + ",";
						strData += strPoint[0] + "," + 
									strPoint[1] + "," + 
									strPoint[2] + "," + 
									strPoint[3] + "," + 
									strPoint[4];
					}
					break;
				case SHPT_ARC:
				case SHPT_ARCZ:
				case SHPT_ARCM:
					{
						strKind = "1";
						strIsVisible = "1";
						strPolyline[0] = "000000000";
						strPolyline[1] = "1";
						strPolyline[2] = "0";

						strData += strKind + "," + strIsVisible + ",";
						strData += strPolyline[0] + "," + 
									strPolyline[1] + "," + 
									strPolyline[2];
					}
					break;
				case SHPT_POLYGON:
				case SHPT_POLYGONZ:
				case SHPT_POLYGONM:
					{
						strKind = "2";
						strIsVisible = "1";
						strPolygon[0] = "0";
						strPolygon[1] = "1";
						strPolygon[2] = "255255255";
						strPolygon[3] = "000000000";
						strPolygon[4] = "NULL";

						strData += strKind + "," + strIsVisible + ",";
						strData += strPolygon[0] + "," + 
									strPolygon[1] + "," + 
									strPolygon[2] + "," + 
									strPolygon[3] + "," + 
									strPolygon[4];
					}
					break;
			}
			fclose(fpSHP);
			strData += "\r\n";
			file.Write(strData, strData.GetLength());

			TRACE("info: mapproperty file wirting.....(%s:%s)\n", strMapkey, strFullName);
		}

		find.Close();
	}

	if(bOpen == TRUE)
		file.Close();

	TRACE("info: mapproperty file create end!!!\n");
}

// 맵 영역(mapproperty.txt => left,right,top,bottom) 보정
void CMapDataMgr::UpdateMapArea()
{
	stVectorMap* pItem;
	stVObject* pSubItem;
	shpObject* pobject;

	double left, top, right, bottom;
	MBR mbr;
	CString strTmp;
	BOOL bRes = FALSE;

	CFile file;
	CString strFileName = "C:\\AFC4I\\Map\\Shp\\mapinfo(5).txt";
	CString strScale = "5";
	CString strDir1, strDir2;
	CString strLeft, strTop, strRight, strBottom;
	CString strDeli = ",";
	CString strData;

	AfxMessageBox("processing!!!!");

	if(!file.Open(strFileName, CFile::modeCreate | CFile::modeWrite))
	{
		TRACE("error: CreateDefaultMapProperty => file open failed!\n");
		return;
	}

	for(int i=0; i<m_vectormaps.GetSize(); i++)
	{
		pItem = (stVectorMap*)m_vectormaps[i];
		if(pItem->m_enMapScale != MS_1_50000)
			continue;

		left = bottom = 999.0;
		top = right = 0.0;

		OpenVectorMap2(pItem);

		for(int j=0; j<pItem->m_pVObject.GetSize(); j++)
		{
			pSubItem = (stVObject*)pItem->m_pVObject[j];
			if(pSubItem->m_pObject == NULL)
				continue;

			pobject = pSubItem->m_pObject;
			mbr = pobject->GetMBR();
			
			if(mbr.xmin < left)
				left = mbr.xmin;
			if(mbr.xmax > right)
				right = mbr.xmax;
			if(mbr.ymax > top)
				top = mbr.ymax;
			if(mbr.ymin < bottom)
				bottom = mbr.ymin;

			bRes = TRUE;
		}

		for(int j=0; j<pItem->m_pVObject.GetSize(); j++)
		{
			pSubItem = (stVObject*)pItem->m_pVObject[j];
			if(pSubItem->m_pObject != NULL)
			{
				delete pSubItem->m_pObject;
				pSubItem->m_pObject = NULL;
			}
		}

		strData = "";
		strDir1 = pItem->m_strSubPath1;
		strDir2 = pItem->m_strSubPath2;
		strLeft.Format("%f", left);
		strTop.Format("%f", top);
		strRight.Format("%f", right);
		strBottom.Format("%f", bottom);

		strData = 
			strScale + strDeli +
			strDir1 + strDeli + 
			strDir2 + strDeli + 
			strLeft + strDeli +
			strTop + strDeli +
			strRight + strDeli +
			strBottom + strDeli + "\r\n";

		if(bRes == TRUE)
			file.Write(strData, strData.GetLength());
		bRes = FALSE;
	}

	file.Close();
}

// 맵 영역 보정시 사용되는 벡터지도 열기
BOOL CMapDataMgr::OpenVectorMap2(stVectorMap* pItem)
{
	if(pItem == NULL)
		return FALSE;

	CString strPath, strSubPath, strFileName, strFullName, strTmp, strDir1, strDir2;
	strDir1 = strDir2 = "";
	if(!pItem->m_strSubPath1.IsEmpty())
		strDir1 = "\\";
	if(!pItem->m_strSubPath2.IsEmpty())
		strDir2 = "\\";

	switch(pItem->m_enMapScale)
	{
		case MS_1_50000 : strSubPath = "5만\\";
			break;
		case MS_1_100000 : strSubPath = "10만\\";
			break;
		case MS_1_250000 : strSubPath = "25만\\";
			break;
		case MS_1_500000 : strSubPath = "50만\\";
			break;
		case MS_1_1000000 : strSubPath = "100만\\";
			break;
		case MS_1_2000000 : strSubPath = "200만\\";
			break;
	}

	strPath = pItem->m_strRootPath + strSubPath + pItem->m_strSubPath1 + strDir1 + pItem->m_strSubPath2 + strDir2;

	int nShapeType;
	CWnd* pWnd = AfxGetMainWnd();
	FILE* fpSHP;
	stVObject* pObject;
	CFile file;
	CFileFind find;
	CString fileExt = ".shp";

	for(int i=0; i<pItem->m_pVObject.GetSize(); i++)
	{
		pObject = (stVObject*)pItem->m_pVObject[i];
		strFileName = pObject->m_strFileName;
		strFullName = strPath + strFileName + fileExt;

	// 중복 open 방지
		if(pObject->m_pObject != NULL)
			continue;

	// 속성이 UnVisible이면 open하지 않는다.
		if(pObject->m_pMapProperty != NULL &&
			pObject->m_pMapProperty->m_bIsVisible == FALSE)
			continue;

		fpSHP = fopen(strFullName, "rb");
		if(fpSHP == NULL) 
		{
			TRACE("error: CMapDataMgr::OpenVectorMap => shape fopen failed!\n");
			continue;
		}

		// get shp type
		fseek(fpSHP, 32L, SEEK_SET);
		fread(&nShapeType, 4, 1, fpSHP);

		// data opening..
		switch(nShapeType)
		{
				case SHPT_POINT:
				case SHPT_POINTZ:
				case SHPT_POINTM:
				case SHPT_MULTIPOINT:
				case SHPT_MULTIPOINTZ:
				case SHPT_MULTIPOINTM:
			pObject->m_pObject = new shpPoint;
			if(!((shpPoint*)pObject->m_pObject)->Open(strFullName))
			{
				TRACE("error: CMapDataMgr::OpenVectorMap => point shape open failed!\n");
				delete pObject->m_pObject;
				pObject->m_pObject = NULL;
				fclose(fpSHP);
				continue;
			}
			pItem->m_bIsVectorDataOpen = TRUE;
			break;
					
				case SHPT_ARC:
				case SHPT_ARCZ:
				case SHPT_ARCM:
			pObject->m_pObject = new shpPolyLine;
			if(!((shpPolyLine*)pObject->m_pObject)->Open(strFullName))
			{
				TRACE("error: CMapDataMgr::OpenVectorMap => polyline shape open failed!\n");
				delete pObject->m_pObject;
				pObject->m_pObject = NULL;
				fclose(fpSHP);
				continue;
			}
			pItem->m_bIsVectorDataOpen = TRUE;
			break;
				case SHPT_POLYGON:
				case SHPT_POLYGONZ:
				case SHPT_POLYGONM:
			if(pWnd == NULL)
			{
				continue;
				fclose(fpSHP);
			}
			pObject->m_pObject = new shpPolygon(pWnd);
			if(!((shpPolygon*)pObject->m_pObject)->Open(strFullName))
			{
				TRACE("error: CMapDataMgr::OpenVectorMap => polygon shape open failed!\n");
				delete pObject->m_pObject;
				pObject->m_pObject = NULL;
				fclose(fpSHP);
				continue;
			}
			pItem->m_bIsVectorDataOpen = TRUE;
			break;
		default:
			{
				TRACE("error: CMapDataMgr::OpenVectorMap => unknown shape type!\n");
				delete pObject->m_pObject;
				pObject->m_pObject = NULL;
				fclose(fpSHP);
				continue;
			}
		}
		fclose(fpSHP);

		TRACE("Info: Vector Map Load OK!(%s)\n", strFullName);
	}

	return TRUE;
}

// 지형요소관리 그룹 추가
void CMapDataMgr::AddMapGroup(CString strID, CString strName)
{
	stMapGroup* pItem = new stMapGroup;
	strID.TrimLeft();
	strID.TrimRight();
	strName.TrimLeft();
	strName.TrimRight();
	strID.MakeUpper();
	pItem->m_strID = strID;
	pItem->m_strName = strName;
	m_pMapGroup.Add(pItem);
}

// 지형요소관리 그룹 리스트 개수 얻기
int CMapDataMgr::GetMapGroupCnt()
{
	return m_pMapGroup.GetSize();
}

// 지형요소관리 그룹 얻기
stMapGroup* CMapDataMgr::GetMapGroup(int iIndex)
{
	return m_pMapGroup[iIndex];
}

// 지형요소관리 그룹(중분류) 추가
void CMapDataMgr::AddMapGroupItem(CString strGroupID, CString strID, CString strName)
{
	stMapGroup* pItem;
	stMapGroupItem* pSubItem;
	for(int i=0; i<m_pMapGroup.GetSize(); i++)
	{
		pItem = (stMapGroup*)m_pMapGroup[i];
		if(pItem->m_strID == strGroupID)
		{
			strID.TrimLeft();
			strID.TrimRight();
			strName.TrimLeft();
			strName.TrimRight();
			strID.MakeUpper();
			pSubItem = new stMapGroupItem;
			pSubItem->m_strID = strID;
			pSubItem->m_strName = strName;
			pItem->m_pGroupList.Add(pSubItem);
		}
	}
}

// text data 속성 정보 추가(사용자정보)
void CMapDataMgr::AddTextPriority(CString strScale, BOOL bIsVisible, UINT uiVisibleType, UINT uiFontSize, 
								  COLORREF clFontColor, UINT uiViewRatio, CString strFontName, CString strFontScript)
{
	stTextPriority* pItem = new stTextPriority;

	pItem->m_strWorldMapPoint = _T("");

	if(strScale == "200")
		pItem->m_enMapScale = MS_1_2000000;
	else if(strScale == "100")
		pItem->m_enMapScale = MS_1_1000000;
	else if(strScale == "50")
		pItem->m_enMapScale = MS_1_500000;
	else if(strScale == "25")
		pItem->m_enMapScale = MS_1_250000;
	else if(strScale == "10")
		pItem->m_enMapScale = MS_1_100000;
	else if(strScale == "5")
		pItem->m_enMapScale = MS_1_50000;
	else if(strScale == "국가명" || strScale == "행정지역명")
	{
		pItem->m_enMapScale = MS_NOTHING;
		pItem->m_strWorldMapPoint = strScale;
	}
	else
	{
		delete pItem;
		return;
	}

	pItem->m_bIsVisible = bIsVisible;
	pItem->m_uiVisibleType = uiVisibleType;
	pItem->m_uiFontSize = uiFontSize;
	pItem->m_clFontColor = clFontColor;
	pItem->m_uiViewRatio = uiViewRatio;
	pItem->m_strFontName = strFontName;
	pItem->m_strFontScript = strFontScript;

	stFontType* pFontType;
	stFontType* pFindFontType = NULL;
	for(int i=0; i<m_pFontType.GetSize(); i++)
	{
		pFontType = (stFontType*)m_pFontType[i];
		if(pFontType->m_strName == strFontName && pFontType->m_strScript == strFontScript)
		{
			pFindFontType = pFontType;
			break;
		}
	}
	pItem->m_pFontType = pFindFontType;

	m_pTextPriority.Add(pItem);
}

// text data 속성 리스트 개수 얻기(사용자정보)
int CMapDataMgr::GetTextPriorityCnt()
{
	return m_pTextPriority.GetSize();
}

// text data 속성 얻기(사용자정보)
stTextPriority* CMapDataMgr::GetTextPriority(int iIndex)
{
	return m_pTextPriority[iIndex];
}

// text data 속성 정보 추가(default정보)
void CMapDataMgr::AddDefaultTextPriority(CString strScale, BOOL bIsVisible, UINT uiVisibleType, UINT uiFontSize, 
										 COLORREF clFontColor, UINT uiViewRatio, CString strFontName, CString strFontScript)
{
	stTextPriority* pItem = new stTextPriority;

	pItem->m_strWorldMapPoint = _T("");
	if(strScale == "200")
		pItem->m_enMapScale = MS_1_2000000;
	else if(strScale == "100")
		pItem->m_enMapScale = MS_1_1000000;
	else if(strScale == "50")
		pItem->m_enMapScale = MS_1_500000;
	else if(strScale == "25")
		pItem->m_enMapScale = MS_1_250000;
	else if(strScale == "10")
		pItem->m_enMapScale = MS_1_100000;
	else if(strScale == "5")
		pItem->m_enMapScale = MS_1_50000;
	else if(strScale == "국가명" || strScale == "행정지역명")
	{
		pItem->m_enMapScale = MS_NOTHING;
		pItem->m_strWorldMapPoint = strScale;
	}
	else
	{
		delete pItem;
		return;
	}

	pItem->m_bIsVisible = bIsVisible;
	pItem->m_uiVisibleType = uiVisibleType;
	pItem->m_uiFontSize = uiFontSize;
	pItem->m_clFontColor = clFontColor;
	pItem->m_uiViewRatio = uiViewRatio;
	pItem->m_strFontName = strFontName;
	pItem->m_strFontScript = strFontScript;

	stFontType* pFontType;
	stFontType* pFindFontType = NULL;
	for(int i=0; i<m_pFontType.GetSize(); i++)
	{
		pFontType = (stFontType*)m_pFontType[i];
		if(pFontType->m_strName == strFontName && pFontType->m_strScript == strFontScript)
		{
			pFindFontType = pFontType;
			break;
		}
	}
	pItem->m_pFontType = pFindFontType;

	m_pDefaultTextPriority.Add(pItem);
}

// text data 속성 리스트 개수 얻기(default정보)
int CMapDataMgr::GetDefaultTextPriorityCnt()
{
	return m_pDefaultTextPriority.GetSize();
}

// text data 속성 얻기(default정보)
stTextPriority* CMapDataMgr::GetDefaultTextPriority(int iIndex)
{
	return m_pDefaultTextPriority[iIndex];
}

// system font 정보 추가
static CString strPreFont = "";
void CMapDataMgr::AddSystemFontType(ENUMLOGFONT* pelf, DWORD dwType, LPCTSTR lpszScript)
{
	LOGFONT& lf = pelf->elfLogFont;
	if (lf.lfCharSet == MAC_CHARSET) // don't put in MAC fonts, commdlg doesn't either
		return;
	// Don't display vertical font for FE platform
	if ((GetSystemMetrics(SM_DBCSENABLED)) && (lf.lfFaceName[0] == '@'))
		return;

// 이전폰트와 같으면 Add하지 않도록 함.
	if(strPreFont == (CString)lf.lfFaceName) return;
	strPreFont = (CString)lf.lfFaceName;

	CString strName = (CString)lf.lfFaceName + " (" + (CString)lpszScript + ")";

	stFontType* pItem = NULL;

	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_pFontType.GetUpperBound();
	int iMid;
	int iCmp;

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		iCmp = strcmp(LPCTSTR(m_pFontType[iMid]->m_strFontType), LPCTSTR(strName));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
			pItem = m_pFontType[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	if(!bFound)
		pItem = new stFontType;

	pItem->m_strFontType = strName;
	pItem->m_dwFlags = dwType;
	pItem->m_nCharSet = lf.lfCharSet;
	pItem->m_nPitchAndFamily = lf.lfPitchAndFamily;
	pItem->m_strName = lf.lfFaceName;
	pItem->m_strScript = lpszScript;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_pFontType.GetUpperBound())
			m_pFontType.Add(pItem);
		else
			m_pFontType.InsertAt(iLow, pItem);
	}
}

// system font정보 개수 얻기
int CMapDataMgr::GetFontTypeCnt()
{
	return m_pFontType.GetSize();
}

// system font정보 얻기
stFontType* CMapDataMgr::GetFontType(int iIndex)
{
	return m_pFontType[iIndex];
}

// system font정보 갱신
void CMapDataMgr::UpdateFontType(stFontType* pFontType)
{
	stTextPriority* pTextPriority;
	for(int i=0; i<m_pTextPriority.GetSize(); i++)
	{
		pTextPriority = (stTextPriority*)m_pTextPriority[i];
		if(pTextPriority->m_strFontName == pFontType->m_strName &&
			pTextPriority->m_strFontScript == pFontType->m_strScript)
		{
			pTextPriority->m_pFontType = pFontType;
			break;
		}
	}
}

// default system font정보 갱신
void CMapDataMgr::UpdateDefaultFontType(stFontType* pFontType)
{
	stTextPriority* pTextPriority;
	for(int i=0; i<m_pDefaultTextPriority.GetSize(); i++)
	{
		pTextPriority = (stTextPriority*)m_pDefaultTextPriority[i];
		if(pTextPriority->m_strFontName == pFontType->m_strName &&
			pTextPriority->m_strFontScript == pFontType->m_strScript)
		{
			pTextPriority->m_pFontType = pFontType;
			break;
		}
	}
}

// symbol data file 열기
void CMapDataMgr::OpenSymbolData()
{
	CFileFind find;
	CString strPath, strFullPath, strFileName, strSelPointName;
	CString strFileExt = "*.bmp";
	CMapPriority* pPriority;
	CPointPriority* pPoint;
	stSymbol* pSymbol;
	HBITMAP hBitmap = NULL;
	int iLength;

	//strPath = "C:\\AFC4I\\Map\\Shp\\symbol\\";
	strPath = CMapConfigMgr::GetSymbolDataPath();
	strSelPointName = "sel_pos.bmp";
	strSelPointName.MakeUpper();

	BOOL bRet = find.FindFile(strPath + strFileExt);
	if(bRet == FALSE)
		return;

	if(bRet)
	{
		while(find.FindNextFile())
		{
			strFileName = find.GetFileName();
			strFullPath = strPath + strFileName;
			hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), (LPCTSTR)strFullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			strFileName.MakeUpper();
			pSymbol = new stSymbol;
			iLength = strFileName.GetLength() - 4;
				pSymbol->m_strName = strFileName.Left(iLength);
			pSymbol->m_hBitmap = hBitmap;
			m_pSymbolList.Add(pSymbol);

			for(int i=0; i<m_defaultmappriority.GetSize(); i++)
			{
				pPriority = (CMapPriority*)m_defaultmappriority[i];
				if(pPriority->m_strFileName.Mid(0,1) == "P")
				{
					pPoint = (CPointPriority*)pPriority;
					if(pPoint->m_strBitmap == pSymbol->m_strName)
						pPoint->m_pSymbol = pSymbol;
				}
			}

			for(int i=0; i<m_mappriority.GetSize(); i++)
			{
				pPriority = (CMapPriority*)m_mappriority[i];
				if(pPriority->m_strFileName.Mid(0,1) == "P")
				{
					pPoint = (CPointPriority*)pPriority;
					if(pPoint->m_strBitmap == pSymbol->m_strName)
						pPoint->m_pSymbol = pSymbol;
				}
			}

			if(strSelPointName == strFileName)
				m_pSelPointSymbol = pSymbol;
		}

		strFileName = find.GetFileName();
		strFullPath = strPath + strFileName;
		hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), (LPCTSTR)strFullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		strFileName.MakeUpper();
		pSymbol = new stSymbol;
		iLength = strFileName.GetLength() - 4;
		pSymbol->m_strName = strFileName.Left(iLength);
		pSymbol->m_hBitmap = hBitmap;
		m_pSymbolList.Add(pSymbol);

		for(int i=0; i<m_defaultmappriority.GetSize(); i++)
		{
			pPriority = (CMapPriority*)m_defaultmappriority[i];
			if(pPriority->m_strFileName.Mid(0,1) == "P")
			{
				pPoint = (CPointPriority*)pPriority;
				if(pPoint->m_strBitmap == pSymbol->m_strName)
					pPoint->m_pSymbol = pSymbol;
			}
		}

		for(int i=0; i<m_mappriority.GetSize(); i++)
		{
			pPriority = (CMapPriority*)m_mappriority[i];
			if(pPriority->m_strFileName.Mid(0,1) == "P")
			{
				pPoint = (CPointPriority*)pPriority;
				if(pPoint->m_strBitmap == pSymbol->m_strName)
					pPoint->m_pSymbol = pSymbol;
			}
		}

		if(strSelPointName == strFileName)
			m_pSelPointSymbol = pSymbol;
	}

	find.Close();
}

// symbol data 리스트 개수 얻기
int CMapDataMgr::GetSymbolListCnt()
{
	return m_pSymbolList.GetSize();
}

// symbol data 얻기
stSymbol* CMapDataMgr::GetSymbol(int iIndex)
{
	return m_pSymbolList[iIndex];
}

// 패턴 data 파일 열기
void CMapDataMgr::OpenPatternData()
{
	CFileFind find;
	CString strPath, strFullPath, strFileName;
	CString strFileExt = "*.bmp";
	CMapPriority* pPriority;
	CPolygonPriority* pPolygon;
	stSymbol* pPattern;
	HBITMAP hBitmap = NULL;
	int iLength;

	//strPath = "C:\\AFC4I\\Map\\Shp\\pattern\\";
	strPath = CMapConfigMgr::GetPatternDataPath();
	BOOL bRet = find.FindFile(strPath + strFileExt);
	if(bRet == FALSE)
		return;

	if(bRet)
	{
		while(find.FindNextFile())
		{
			strFileName = find.GetFileName();
			strFullPath = strPath + strFileName;
			hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), (LPCTSTR)strFullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			strFileName.MakeUpper();
			pPattern = new stSymbol;
			iLength = strFileName.GetLength() - 4;
			pPattern->m_strName = strFileName.Left(iLength);
			pPattern->m_hBitmap = hBitmap;
			m_pPatternList.Add(pPattern);

			for(int i=0; i<m_defaultmappriority.GetSize(); i++)
			{
				pPriority = (CMapPriority*)m_defaultmappriority[i];
				if(pPriority->m_type_geo == "polygon")
				{
					pPolygon = (CPolygonPriority*)pPriority;
					if(pPolygon->m_strBitmap == pPattern->m_strName)
						pPolygon->m_pPattern = pPattern;
				}
			}

			for(int i=0; i<m_mappriority.GetSize(); i++)
			{
				pPriority = (CMapPriority*)m_mappriority[i];
				if(pPriority->m_type_geo == "polygon")
				{
					pPolygon = (CPolygonPriority*)pPriority;
					if(pPolygon->m_strBitmap == pPattern->m_strName)
						pPolygon->m_pPattern = pPattern;
				}
			}
		}

		strFileName = find.GetFileName();
		strFullPath = strPath + strFileName;
		hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), (LPCTSTR)strFullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		strFileName.MakeUpper();
		pPattern = new stSymbol;
		iLength = strFileName.GetLength() - 4;
		pPattern->m_strName = strFileName.Left(iLength);
		pPattern->m_hBitmap = hBitmap;
		m_pPatternList.Add(pPattern);

		for(int i=0; i<m_defaultmappriority.GetSize(); i++)
		{
			pPriority = (CMapPriority*)m_defaultmappriority[i];
			if(pPriority->m_type_geo == "polygon")
			{
				pPolygon = (CPolygonPriority*)pPriority;
				if(pPolygon->m_strBitmap == pPattern->m_strName)
					pPolygon->m_pPattern = pPattern;
			}
		}

		for(int i=0; i<m_mappriority.GetSize(); i++)
		{
			pPriority = (CMapPriority*)m_mappriority[i];
			if(pPriority->m_type_geo == "polygon")
			{
				pPolygon = (CPolygonPriority*)pPriority;
				if(pPolygon->m_strBitmap == pPattern->m_strName)
					pPolygon->m_pPattern = pPattern;
			}
		}
	}

	find.Close();
}

// 패턴 data 리스트 개수 얻기
int CMapDataMgr::GetPatternListCnt()
{
	return m_pPatternList.GetSize();
}

// 패턴 데이터 얻기
stSymbol* CMapDataMgr::GetPattern(int iIndex)
{
	return m_pPatternList[iIndex];
}

// 지형요소 list(임시buffer) 개수 얻기
int CMapDataMgr::GetFeatureInfoListCnt()
{
	return m_pFeatureInfoList.GetSize();
}

// 지형요소 (임시buffer) 얻기
stFeatureInfo* CMapDataMgr::GetFeatureInfo(int iIndex)
{
	return m_pFeatureInfoList[iIndex];
}

// 지형요소 속성정보 보기
BOOL CMapDataMgr::InfoShow(CGDCGeo* pDC, long lMapScale, GPoint point, CGRectGP* rectGP)
{
	BOOL bRes = FALSE;
	int iCnt, iMapScale;
	iMapScale = (int)lMapScale;

	if(iMapScale == 1000000)
		iCnt = GetCurrentMapCnt();
	else
		iCnt = GetCurrentMapListCnt();

	stFeatureInfo* pFeature;
	stVectorMap* pMap;
	stVObject* pObject;
	shpObject* pShape;
	int ID;

	CString tmpString;
	CString tmp;
	GPoint gpoint;

	// buffer clear
	DeleteAllFeatureInfoList();

	int iPriorityCnt = 0;

	for(int i=0; i<iCnt; i++)
	{
		if(iMapScale == 1000000)
			pMap = (stVectorMap*)GetCurrentMap(i);
		else
			pMap = (stVectorMap*)GetCurrentMapList(i);

		for(int j=0; j<pMap->m_pVObject.GetSize(); j++)
		{
			pObject = (stVObject*)pMap->m_pVObject[j];
			if(pObject == NULL) continue;
			pShape = (shpObject*)pObject->m_pObject;
			if(pShape == NULL) continue;

// jwpark, release bug 수정요!

			if(pObject->m_paMapPriority[0]->m_bIsVisible == FALSE) continue;
			ID = pShape->Track(pDC, pObject->m_paMapPriority[0], point, rectGP);
			if(ID == -1) continue;
			pFeature = new stFeatureInfo;
			pFeature->m_uiID = ID;
			pFeature->m_pObject = pObject;
			m_pFeatureInfoList.Add(pFeature);
			bRes = TRUE;
		}
	}

	return bRes;
}

stFeatureInfo * CMapDataMgr::FindFeatureInfo(UINT uiID)
{
	for(int i=0; i<m_pFeatureInfoList.GetSize(); i++)
	{
		if(uiID == m_pFeatureInfoList[i]->m_uiID)
			return m_pFeatureInfoList[i];
	}
	return NULL;
}

// 지형요소 list(임시buffer) clear
void CMapDataMgr::DeleteAllFeatureInfoList()
{
	for(int i=0; i<m_pFeatureInfoList.GetSize(); i++)
		delete m_pFeatureInfoList[i];
	m_pFeatureInfoList.RemoveAll();
}

// 소축척지도 text data 열기
BOOL CMapDataMgr::OpenSmallMapText()
{
	if(m_pSmallMapText.GetSize() > 0)
		return TRUE;

	CString strPath, strFileName, strData, strName, strTmp;
	double dx, dy;
	stMapText *pMapText = NULL;
	CFileIO file;
	char* pDeli;
	CString strDeli = "\t";

	//strPath = "C:\\AFC4I\\Map\\Shp\\small_scale_map\\";
	strPath = CMapConfigMgr::GetSmallmapDataPath();
	strFileName = CMapConfigMgr::GetSmallMapTextFileName();
	strFileName = strPath + strFileName;

	if(!file.Init(FALSE))
		return FALSE;

	if(!file.Open((LPCTSTR)strFileName, CFile::modeRead|CFile::typeText))
		return FALSE;

	while(file.ReadString(strData))
	{
		// 경도
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		dx = atof(strTmp);
		// 위도
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		dy = atof(strTmp);
		// text
		pDeli = strtok(NULL, strDeli);
		strName.Format("%s", pDeli);

		pMapText = new stMapText;
		pMapText->m_gpPoint.SetPoint(dx, dy);
		pMapText->m_strName = strName;
		m_pSmallMapText.Add(pMapText);
	}
	file.Close();

	return TRUE;
}

// 소축척지도 text data list 개수 얻기
int CMapDataMgr::GetSmallMapTextCnt()
{
	return m_pSmallMapText.GetSize();
}

// 소축척지도 text data 얻기
stMapText* CMapDataMgr::GetSmallMapText(int iIndex)
{
	return m_pSmallMapText[iIndex];
}

// 소축척 세계지도 속성데이터 추가
void CMapDataMgr::AddSmallWorldMapPriority(UINT uiSN, long left, long top, long right, long bottom, CString strName)
{
	stWorldMapPriority *pItem = new stWorldMapPriority;
	pItem->m_uiSN = uiSN;
	pItem->m_rectGP.SetRect(left, top, right, bottom);
	pItem->m_strName = strName;
	m_pWorldMapPriority.Add(pItem);
}

// 소축척 세계지도 속성데이터 리스트 얻기
int CMapDataMgr::GetSmallWorldMapPriorityCnt()
{
	return m_pWorldMapPriority.GetSize();
}

// 소축척 세계지도 속성데이터 얻기
stWorldMapPriority* CMapDataMgr::GetSmallWorldMapPriority(int iIndex)
{
	return m_pWorldMapPriority[iIndex];
}

// 소축척 세계지도 데이터 open check
BOOL CMapDataMgr::IsWorldMapOpen()
{
	return m_bIsWorldMapOpen;
}

// 사용자 저장 지도 정보 열기
BOOL CMapDataMgr::OpenUserMapInfo()
{
	CFile file;
	CFileFind find;
	CString strPath, strFileName, strFileExt;

	strPath = CMapConfigMgr::GetUserMapDataPath();
	strFileExt = "*.afm";
	strFileName = strPath + strFileExt;

	BOOL bRet = find.FindFile(strFileName);
	if(bRet == FALSE)
		return FALSE;

	if(bRet == TRUE)
	{
		while(find.FindNextFile())
		{
			strFileName = find.GetFileName();
			strFileName = strPath + strFileName;

			if(!file.Open(strFileName, CFile::modeRead))
			{
				TRACE("error: CMapDataMgr::OpenUserMapInfo => file open failed!\n");
				return FALSE;
			}

			CArchive ar(&file, CArchive::load);
//			Serialize(FALSE, ar);
			Serialize(1, ar);

			ar.Close();
			file.Close();
		}
	}

	strFileName = find.GetFileName();
	strFileName = strPath + strFileName;

	if(!file.Open(strFileName, CFile::modeRead))
	{
		TRACE("error: CMapDataMgr::OpenUserMapInfo => file open failed!\n");
		return FALSE;
	}

	CArchive ar(&file, CArchive::load);
//	Serialize(FALSE, ar);
	Serialize(1, ar);

	ar.Close();
	file.Close();
	find.Close();

	return TRUE;
}

// 사용자 저장 지도 정보 추가
BOOL CMapDataMgr::AddUserMapInfo(CString strTime, CString strRootPath, CString strFileName, 
								 EGMapKind eMapKind, EGMapScale eMapScale, int iMapRatio, 
								 UINT uiMapMode, CGRectGP gpSelRect, BOOL bIsAutoMap, int iMapType)
{
	BOOL bRes = TRUE;
	CFile file;

	stUserMapInfo *pItem = new stUserMapInfo;
	pItem->m_strFileName = strFileName;
	pItem->m_strSaveTime = strTime;
	pItem->m_eMapKind = eMapKind;
	pItem->m_eMapScale = eMapScale;
	pItem->m_eRasterMapScale = m_enCurMapScale;
	pItem->m_iMapRatio = iMapRatio;
	pItem->m_uiMapMode = uiMapMode;
	pItem->m_gpSelRect = gpSelRect;
	pItem->m_bIsAutoMap = bIsAutoMap;
	pItem->m_bIsWorldMap = m_bIsWorldMapOpen;
	pItem->m_uiMapType = (UINT)iMapType;
	strFileName = strRootPath + strFileName;

	// file write
	try
	{
		if(!file.Open(strFileName, CFile::modeCreate|CFile::modeWrite))
		{
			TRACE("error: CMapDataMgr::AddUserMapInfo => file open failed!\n");
			return FALSE;
		}

		m_pUserMapInfo.Add(pItem);
		m_pCurUserMapInfo = pItem;

		CArchive ar(&file, CArchive::store);
//		Serialize(FALSE, ar);
		Serialize(1, ar);

		ar.Close();
		file.Close();

		bRes = TRUE;
	}
	catch(...)
	{
		m_pCurUserMapInfo = NULL;
		delete pItem;
		bRes = FALSE;
	}

	return bRes;
}

// 사용자 저장지도 리스트 개수 얻기
int CMapDataMgr::GetUserMapInfoCnt()
{
	return m_pUserMapInfo.GetSize();
}

// 사용자 저장지도 정보 얻기
stUserMapInfo* CMapDataMgr::GetUserMapInfo(int iIndex)
{
	return m_pUserMapInfo[iIndex];
}

// 사용자 저장지도 리스트 삭제
void CMapDataMgr::ClearUserMapInfo()
{
	stUserMapInfo *pUserMapInfo = NULL;
	for(int i=0; i<m_pUserMapInfo.GetSize(); i++)
	{
		pUserMapInfo = (stUserMapInfo*)m_pUserMapInfo[i];

		for(int j=0; j<pUserMapInfo->m_mappriority.GetSize(); j++)
		{
			if(pUserMapInfo->m_mappriority[j]->m_type_geo == "point")
				delete (CPointPriority *)pUserMapInfo->m_mappriority[j];

			else if(pUserMapInfo->m_mappriority[j]->m_type_geo == "polyline")
				delete (CPolylinePriority *)pUserMapInfo->m_mappriority[j];

			else if(pUserMapInfo->m_mappriority[j]->m_type_geo == "polygon")
				delete (CPolygonPriority *)pUserMapInfo->m_mappriority[j];
		}
		pUserMapInfo->m_mappriority.RemoveAll();

		for(int j=0; j<pUserMapInfo->m_pTextPriority.GetSize(); j++)
			delete pUserMapInfo->m_pTextPriority[j];
		pUserMapInfo->m_pTextPriority.RemoveAll();

		delete m_pUserMapInfo[i];
	}
	m_pUserMapInfo.RemoveAll();
}

// 바로가기 저장지도 리스트 삭제
void CMapDataMgr::ClearDirectGoMapInfo()
{
	stUserMapInfo *pUserMapInfo = NULL;
	for(int i=0; i<m_paDirectGoMapInfo.GetSize(); i++)
	{
		pUserMapInfo = (stUserMapInfo*)m_paDirectGoMapInfo[i];

		for(int j=0; j<pUserMapInfo->m_mappriority.GetSize(); j++)
		{
			if(pUserMapInfo->m_mappriority[j]->m_type_geo == "point")
				delete (CPointPriority *)pUserMapInfo->m_mappriority[j];

			else if(pUserMapInfo->m_mappriority[j]->m_type_geo == "polyline")
				delete (CPolylinePriority *)pUserMapInfo->m_mappriority[j];

			else if(pUserMapInfo->m_mappriority[j]->m_type_geo == "polygon")
				delete (CPolygonPriority *)pUserMapInfo->m_mappriority[j];
		}
		pUserMapInfo->m_mappriority.RemoveAll();

		for(int j=0; j<pUserMapInfo->m_pTextPriority.GetSize(); j++)
			delete pUserMapInfo->m_pTextPriority[j];
		pUserMapInfo->m_pTextPriority.RemoveAll();

		delete m_paDirectGoMapInfo[i];
	}
	m_paDirectGoMapInfo.RemoveAll();
}

// 파일명 중복 체크(중복이면 true)
BOOL CMapDataMgr::DupChkFileName(CString strRootPath, CString strFileName)
{
	CFileFind find;
	CFileIO file;

	if(!file.Init(FALSE))
		return TRUE;

	BOOL bRet = find.FindFile(strRootPath + strFileName);
	if(bRet == FALSE)
		return FALSE;

	return TRUE;
}

void CMapDataMgr::Serialize(int iMapType, CArchive &ar)
{
	/*
	int iLen = 0;
	int iSize = 0;
	int iViewRatio;
	char szBuf[128];
	UINT uiPriority;
	EGMapScale enMapScale;
	BOOL bTransParent, bIsVisible;
	CString strTmp, strFileName, strName;
	stUserMapInfo *pUserMapInfo = NULL;
	stTextPriority *pTextPriority = NULL;
	stTextPriority *pNewTextPriority = NULL;
	stFontType* pFontType = NULL;
	CMapPriority *pMapPriority = NULL;
	CPointPriority *pPointPriority = NULL;
	CPolylinePriority *pPolylinePriority = NULL;
	CPolygonPriority *pPolygonPriority = NULL;
	CPointPriority *pNewPointPriority = NULL;
	CPolylinePriority *pNewPolylinePriority = NULL;
	CPolygonPriority *pNewPolygonPriority = NULL;
	stSymbol *pSymbol = NULL;

	try
	{
		if(ar.IsStoring())
		{
			// base map
			if(iMapType == 0)
			{
				if(m_pCurBaseMapInfo == NULL) return;
				else pUserMapInfo = m_pCurBaseMapInfo;
			}
			// user map
			else if(iMapType == 1)
			{
				if(m_pCurUserMapInfo == NULL) return;
				else pUserMapInfo = m_pCurUserMapInfo;
			}
			// directgo map
			else if(iMapType == 2)
			{
				if(m_pCurUserMapInfo == NULL) return;
				else pUserMapInfo = m_pCurUserMapInfo;
			}

		// 사용자 저장 지도정보 화일저장
			iLen = pUserMapInfo->m_strFileName.GetLength();
			ar << iLen;
			ar.Write(pUserMapInfo->m_strFileName, iLen);
			iLen = pUserMapInfo->m_strSaveTime.GetLength();
			ar << iLen;
			ar.Write(pUserMapInfo->m_strSaveTime, iLen);
			iLen = sizeof(pUserMapInfo->m_eMapKind);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_eMapKind, iLen);
			iLen = sizeof(pUserMapInfo->m_eMapScale);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_eMapScale, iLen);
			iLen = sizeof(pUserMapInfo->m_eRasterMapScale);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_eRasterMapScale, iLen);
			iLen = sizeof(int);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_iMapRatio, iLen);
			iLen = sizeof(UINT);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_uiMapMode, iLen);
			iLen = sizeof(pUserMapInfo->m_gpSelRect);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_gpSelRect, iLen);
			iLen = sizeof(BOOL);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_bIsAutoMap, iLen);
			iLen = sizeof(BOOL);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_bIsWorldMap, iLen);
			iLen = sizeof(UINT);
			ar << iLen;
			ar.Write(&pUserMapInfo->m_uiMapType, iLen);

		// 도시요소정보 화일저장
			iSize = m_mappriority.GetSize();
			ar << iSize;
			for(int i=0; i<iSize; i++)
			{
				pMapPriority = (CMapPriority*)m_mappriority[i];

				iLen = sizeof(UINT);
				ar << iLen;
				ar.Write(&pMapPriority->m_uiPriority, iLen);
				iLen = sizeof(pMapPriority->m_enMapScale);
				ar << iLen;
				ar.Write(&pMapPriority->m_enMapScale, iLen);
				iLen = pMapPriority->m_strFileName.GetLength();
				ar << iLen;
				ar.Write(pMapPriority->m_strFileName, iLen);
				iLen = sizeof(BOOL);
				ar << iLen;
				ar.Write(&pMapPriority->m_bTransParent, iLen);
				iLen = sizeof(BOOL);
				ar << iLen;
				ar.Write(&pMapPriority->m_bIsVisible, iLen);
				iLen = pMapPriority->m_strName.GetLength();
				ar << iLen;
				ar.Write(pMapPriority->m_strName, iLen);
				iLen = sizeof(int);
				ar << iLen;
				ar.Write(&pMapPriority->m_iViewRatio, iLen);

				if(pMapPriority->m_type_geo == "point")
				{
					pPointPriority = (CPointPriority*)pMapPriority;

					iLen = sizeof(UINT);
					ar << iLen;
					ar.Write(&pPointPriority->m_uiRadius, iLen);
					iLen = sizeof(COLORREF);
					ar << iLen;
					ar.Write(&pPointPriority->m_crFillColor, iLen);
					iLen = sizeof(UINT);
					ar << iLen;
					ar.Write(&pPointPriority->m_uiType, iLen);
					iLen = pPointPriority->m_strBitmap.GetLength();
					ar << iLen;
					ar.Write(pPointPriority->m_strBitmap, iLen);

					pNewPointPriority = new CPointPriority;
					pNewPointPriority->m_bIsVisible = pPointPriority->m_bIsVisible;
					pNewPointPriority->m_bTransParent = pPointPriority->m_bTransParent;
					pNewPointPriority->m_crFillColor = pPointPriority->m_crFillColor;
					pNewPointPriority->m_enMapScale = pPointPriority->m_enMapScale;
					pNewPointPriority->m_iViewRatio = pPointPriority->m_iViewRatio;
					pNewPointPriority->m_pSymbol = NULL;
					for(int j=0; j<m_pSymbolList.GetSize(); j++)
					{
						pSymbol = (stSymbol*)m_pSymbolList[j];
						if(pSymbol->m_strName == pPointPriority->m_strBitmap)
						{
							pNewPointPriority->m_pSymbol = pSymbol;
							break;
						}
					}
					pNewPointPriority->m_strBitmap = pPointPriority->m_strBitmap;
					pNewPointPriority->m_strFileName = pPointPriority->m_strFileName;
					pNewPointPriority->m_strName = pPointPriority->m_strName;
					pNewPointPriority->m_uiPriority = pPointPriority->m_uiPriority;
					pNewPointPriority->m_uiRadius = pPointPriority->m_uiRadius;
					pNewPointPriority->m_uiType = pPointPriority->m_uiType;
					pUserMapInfo->m_mappriority.Add(pNewPointPriority);
				}
				else if(pMapPriority->m_type_geo == "polyline")
				{
					pPolylinePriority = (CPolylinePriority*)pMapPriority;

					iLen = sizeof(UINT);
					ar << iLen;
					ar.Write(&pPolylinePriority->m_uiLineWidth, iLen);
					iLen = sizeof(COLORREF);
					ar << iLen;
					ar.Write(&pPolylinePriority->m_crLineColor, iLen);
					iLen = sizeof(UINT);
					ar << iLen;
					ar.Write(&pPolylinePriority->m_uiType, iLen);
					iLen = sizeof(UINT);
					ar << iLen;
					ar.Write(&pPolylinePriority->m_uiLineWidth2, iLen);
					iLen = sizeof(COLORREF);
					ar << iLen;
					ar.Write(&pPolylinePriority->m_crLineColor2, iLen);
					iLen = sizeof(UINT);
					ar << iLen;
					ar.Write(&pPolylinePriority->m_uiType2, iLen);

					pNewPolylinePriority = new CPolylinePriority;
					pNewPolylinePriority->m_bIsVisible = pPolylinePriority->m_bIsVisible;
					pNewPolylinePriority->m_bTransParent = pPolylinePriority->m_bTransParent;
					pNewPolylinePriority->m_crLineColor = pPolylinePriority->m_crLineColor;
					pNewPolylinePriority->m_crLineColor2 = pPolylinePriority->m_crLineColor2;
					pNewPolylinePriority->m_enMapScale = pPolylinePriority->m_enMapScale;
					pNewPolylinePriority->m_iViewRatio = pPolylinePriority->m_iViewRatio;
					pNewPolylinePriority->m_strFileName = pPolylinePriority->m_strFileName;
					pNewPolylinePriority->m_strName = pPolylinePriority->m_strName;
					pNewPolylinePriority->m_uiLineWidth = pPolylinePriority->m_uiLineWidth;
					pNewPolylinePriority->m_uiLineWidth2 = pPolylinePriority->m_uiLineWidth2;
					pNewPolylinePriority->m_uiPriority = pPolylinePriority->m_uiPriority;
					pNewPolylinePriority->m_uiType = pPolylinePriority->m_uiType;
					pNewPolylinePriority->m_uiType2 = pPolylinePriority->m_uiType2;
					pUserMapInfo->m_mappriority.Add(pNewPolylinePriority);
				}
				else if(pMapPriority->m_type_geo == "polygon")
				{
					pPolygonPriority = (CPolygonPriority*)pMapPriority;

					iLen = sizeof(UINT);
					ar << iLen;
					ar.Write(&pPolygonPriority->m_uiLineWidth, iLen);
					iLen = sizeof(COLORREF);
					ar << iLen;
					ar.Write(&pPolygonPriority->m_crLineColor, iLen);
					iLen = sizeof(COLORREF);
					ar << iLen;
					ar.Write(&pPolygonPriority->m_crFillColor, iLen);
					iLen = sizeof(UINT);
					ar << iLen;
					ar.Write(&pPolygonPriority->m_uiType, iLen);
					iLen = pPolygonPriority->m_strBitmap.GetLength();
					ar << iLen;
					ar.Write(pPolygonPriority->m_strBitmap, iLen);

					pNewPolygonPriority = new CPolygonPriority;
					pNewPolygonPriority->m_bIsVisible = pPolygonPriority->m_bIsVisible;
					pNewPolygonPriority->m_bTransParent = pPolygonPriority->m_bTransParent;
					pNewPolygonPriority->m_crFillColor = pPolygonPriority->m_crFillColor;
					pNewPolygonPriority->m_crLineColor = pPolygonPriority->m_crLineColor;
					pNewPolygonPriority->m_enMapScale = pPolygonPriority->m_enMapScale;
					pNewPolygonPriority->m_iViewRatio = pPolygonPriority->m_iViewRatio;
					pNewPolygonPriority->m_pPattern = NULL;
					for(int j=0; j<m_pSymbolList.GetSize(); j++)
					{
						pSymbol = (stSymbol*)m_pSymbolList[j];
						if(pSymbol->m_strName == pPolygonPriority->m_strBitmap)
						{
							pNewPolygonPriority->m_pPattern = pSymbol;
							break;
						}
					}
					pNewPolygonPriority->m_strBitmap = pPolygonPriority->m_strBitmap;
					pNewPolygonPriority->m_strFileName = pPolygonPriority->m_strFileName;
					pNewPolygonPriority->m_strName = pPolygonPriority->m_strName;
					pNewPolygonPriority->m_uiLineWidth = pPolygonPriority->m_uiLineWidth;
					pNewPolygonPriority->m_uiPriority = pPolygonPriority->m_uiPriority;
					pNewPolygonPriority->m_uiType = pPolygonPriority->m_uiType;
					pUserMapInfo->m_mappriority.Add(pNewPolygonPriority);
				}
			}

		// text data 속성정보 화일저장
			iSize = m_pTextPriority.GetSize();
			ar << iSize;
			for(i=0; i<iSize; i++)
			{
				pTextPriority = (stTextPriority*)m_pTextPriority[i];

				iLen = sizeof(pTextPriority->m_enMapScale);
				ar << iLen;
				ar.Write(&pTextPriority->m_enMapScale, iLen);
				iLen = sizeof(BOOL);
				ar << iLen;
				ar.Write(&pTextPriority->m_bIsVisible, iLen);
				iLen = sizeof(UINT);
				ar << iLen;
				ar.Write(&pTextPriority->m_uiVisibleType, iLen);
				iLen = sizeof(UINT);
				ar << iLen;
				ar.Write(&pTextPriority->m_uiFontSize, iLen);
				iLen = sizeof(COLORREF);
				ar << iLen;
				ar.Write(&pTextPriority->m_clFontColor, iLen);
				iLen = sizeof(UINT);
				ar << iLen;
				ar.Write(&pTextPriority->m_uiViewRatio, iLen);
				iLen = pTextPriority->m_strFontName.GetLength();
				ar << iLen;
				ar.Write(pTextPriority->m_strFontName, iLen);
				iLen = pTextPriority->m_strFontScript.GetLength();
				ar << iLen;
				ar.Write(pTextPriority->m_strFontScript, iLen);

				pNewTextPriority = new stTextPriority;
				pNewTextPriority->m_bIsVisible = pTextPriority->m_bIsVisible;
				pNewTextPriority->m_clFontColor = pTextPriority->m_clFontColor;
				pNewTextPriority->m_enMapScale = pTextPriority->m_enMapScale;
				pNewTextPriority->m_pFontType = pTextPriority->m_pFontType;
				pNewTextPriority->m_strFontName = pTextPriority->m_strFontName;
				pNewTextPriority->m_strFontScript = pTextPriority->m_strFontScript;
				pNewTextPriority->m_uiFontSize = pTextPriority->m_uiFontSize;
				pNewTextPriority->m_uiViewRatio = pTextPriority->m_uiViewRatio;
				pNewTextPriority->m_uiVisibleType = pTextPriority->m_uiVisibleType;
				pUserMapInfo->m_pTextPriority.Add(pNewTextPriority);
			}

			// resource id 처리(바로가기일 경우만 사용)
			if(iMapType == 2)
			{
				iLen = sizeof(UINT);
				ar << iLen;
				ar.Write(&pUserMapInfo->m_uiID, iLen);
			}
		}
		else
		{
			pUserMapInfo = new stUserMapInfo;

		// 사용자 저장 지도정보 load
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			strTmp = CString(szBuf, iLen);
			pUserMapInfo->m_strFileName = strTmp;
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			strTmp = CString(szBuf, iLen);
			pUserMapInfo->m_strSaveTime = strTmp;
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_eMapKind, szBuf, iLen);
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_eMapScale, szBuf, iLen);
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_eRasterMapScale, szBuf, iLen);
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_iMapRatio, szBuf, iLen);
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_uiMapMode, szBuf, iLen);
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_gpSelRect, szBuf, iLen);
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_bIsAutoMap, szBuf, iLen);
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_bIsWorldMap, szBuf, iLen);
			ar >> iLen;
			ar.Read(szBuf, iLen);
			szBuf[iLen] = NULL;
			memcpy(&pUserMapInfo->m_uiMapType, szBuf, iLen);

		// 도시요소정보 load
			ar >> iSize;
			for(int i=0; i<iSize; i++)
			{
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&uiPriority, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&enMapScale, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				strFileName = (CString)szBuf;
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&bTransParent, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&bIsVisible, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				strTmp = CString(szBuf, iLen);
				strName = strTmp;
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&iViewRatio, szBuf, iLen);
	
				if(strFileName.Mid(0,1) == "P")
				{
					pPointPriority = new CPointPriority;

					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPointPriority->m_uiRadius, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPointPriority->m_crFillColor, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPointPriority->m_uiType, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					strTmp = CString(szBuf, iLen);
					pPointPriority->m_strBitmap = strTmp;
					pPointPriority->m_pSymbol = NULL;
					for(int j=0; j<m_pSymbolList.GetSize(); j++)
					{
						pSymbol = (stSymbol*)m_pSymbolList[j];
						if(pSymbol->m_strName == pPointPriority->m_strBitmap)
						{
							pPointPriority->m_pSymbol = pSymbol;
							break;
						}
					}

					pMapPriority = (CMapPriority*)pPointPriority;
				}
				else if(strFileName.Mid(0,1) == "L")
				{
					pPolylinePriority = new CPolylinePriority;

					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolylinePriority->m_uiLineWidth, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolylinePriority->m_crLineColor, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolylinePriority->m_uiType, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolylinePriority->m_uiLineWidth2, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolylinePriority->m_crLineColor2, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolylinePriority->m_uiType2, szBuf, iLen);

					pMapPriority = (CMapPriority*)pPolylinePriority;
				}
				else if(strFileName.Mid(0,1) == "A")
				{
					pPolygonPriority = new CPolygonPriority;

					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolygonPriority->m_uiLineWidth, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolygonPriority->m_crLineColor, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolygonPriority->m_crFillColor, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					memcpy(&pPolygonPriority->m_uiType, szBuf, iLen);
					ar >> iLen;
					ar.Read(szBuf, iLen);
					szBuf[iLen] = NULL;
					strTmp = CString(szBuf, iLen);
					pPolygonPriority->m_strBitmap = strTmp;

					pPolygonPriority->m_pPattern = NULL;
					for(int j=0; j<m_pSymbolList.GetSize(); j++)
					{
						pSymbol = (stSymbol*)m_pSymbolList[j];
						if(pSymbol->m_strName == pPolygonPriority->m_strBitmap)
						{
							pPolygonPriority->m_pPattern = pSymbol;
							break;
						}
					}

					pMapPriority = (CMapPriority*)pPolygonPriority;
				}

				pMapPriority->m_uiPriority = uiPriority;
				pMapPriority->m_enMapScale = enMapScale;
				pMapPriority->m_bTransParent = bTransParent;
				pMapPriority->m_bIsVisible = bIsVisible;
				pMapPriority->m_strFileName = strFileName;
				pMapPriority->m_strName = strName;
				pMapPriority->m_iViewRatio = iViewRatio;

				pUserMapInfo->m_mappriority.Add(pMapPriority);
			}

		// text data 속성정보 load
			ar >> iSize;
			for(i=0; i<iSize; i++)
			{
				pTextPriority = new stTextPriority;

				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&pTextPriority->m_enMapScale, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&pTextPriority->m_bIsVisible, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&pTextPriority->m_uiVisibleType, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&pTextPriority->m_uiFontSize, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&pTextPriority->m_clFontColor, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				memcpy(&pTextPriority->m_uiViewRatio, szBuf, iLen);
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				strTmp = CString(szBuf, iLen);
				pTextPriority->m_strFontName = strTmp;
				ar >> iLen;
				ar.Read(szBuf, iLen);
				szBuf[iLen] = NULL;
				strTmp = CString(szBuf, iLen);
				pTextPriority->m_strFontScript = strTmp;

				pTextPriority->m_pFontType = NULL;
				for(int j=0; j<m_pFontType.GetSize(); j++)
				{
					pFontType = (stFontType*)m_pFontType[j];
					if(pFontType->m_strName == pTextPriority->m_strFontName && 
						pFontType->m_strScript == pTextPriority->m_strFontScript)
					{
						pTextPriority->m_pFontType = pFontType;
						break;
					}
				}
				pUserMapInfo->m_pTextPriority.Add(pTextPriority);
			}

			if(iMapType == 0)
			{
				pUserMapInfo->m_uiID = 0;

				// jwpark, 2007-03-14
				if(m_pCurBaseMapInfo != NULL)
				{
					ClearMapPriority(m_pCurBaseMapInfo);
					delete m_pCurBaseMapInfo;
					m_pCurBaseMapInfo = NULL;
				}

				m_pCurBaseMapInfo = pUserMapInfo;
//				m_pCurUserMapInfo = pUserMapInfo;
				SetUserMapInfo(m_pCurBaseMapInfo);
				SwapMapPriority(TRUE);
			}
			else if(iMapType == 1)
			{
				pUserMapInfo->m_uiID = 0;
				m_pUserMapInfo.Add(pUserMapInfo);
			}
			else if(iMapType == 2)
			{
				ar >> iLen;
				ar.Read(szBuf, iLen);
				memcpy(&pUserMapInfo->m_uiID, szBuf, iLen);
				m_paDirectGoMapInfo.Add(pUserMapInfo);
			}
		}
	}
	catch(...)
	{
		TRACE("error: CMapDataMgr::Serialize failed!!");
	}
	*/
}

// 사용자저장지도정보 set
void CMapDataMgr::SetUserMapInfo(stUserMapInfo *pUserMapInfo)
{
	m_pCurUserMapInfo = pUserMapInfo;

	// 사용자 저장 지도가 세계지도이면 세계지도 open
	if(pUserMapInfo->m_bIsWorldMap == TRUE)
	{
		if(m_bIsWorldMapOpen == FALSE)
			worldMapOpen();
	}
	// 사용자 저장 지도가 세계지도가 아니면 세계지도 데이터가 존재하는 경우 데이터 지운다.
	else
	{
		if(m_bIsWorldMapOpen == TRUE)
			ClearWorldMap();
	}
}

// 현제 도시된 지도정보(지형요소정보포함) 교체
BOOL CMapDataMgr::SwapMapPriority(BOOL bIsUserMap)
{
	BOOL bRes = FALSE;
	CMapPriority *pMapPriority;
	stTextPriority *pTextPriority;
	stTextPriority *pNewTextPriority;
	stVectorMap *pVectorMap;
	stVObject *pVObject;
	CMapProperty *pMapProperty;
	CPointPriority *pPoint;
	CPointPriority *pNewPoint;
	CPolylinePriority *pLine;
	CPolylinePriority *pNewLine;
	CPolygonPriority *pPolygon;
	CPolygonPriority *pNewPolygon;
	int iIndex = 0;

	// 기본지도정보를 사용자지도정보로 변환
	if(bIsUserMap == TRUE)
	{
		if(m_pCurUserMapInfo == NULL)
			return bRes;

		// 기본지도 속성정보 교체
		for(int i=0; i<m_mappriority.GetSize(); i++)
		{
			if(m_mappriority[i]->m_type_geo == "point")
				delete (CPointPriority *)m_mappriority[i];
			else if(m_mappriority[i]->m_type_geo == "polyline")
				delete (CPolylinePriority *)m_mappriority[i];
			else if(m_mappriority[i]->m_type_geo == "polygon")
				delete (CPolygonPriority *)m_mappriority[i];
		}
		m_mappriority.RemoveAll();
		for(int i=0; i<m_pCurUserMapInfo->m_mappriority.GetSize(); i++)
		{
			pMapPriority = (CMapPriority*)m_pCurUserMapInfo->m_mappriority[i];
			if(pMapPriority->m_type_geo == "point")
			{
				pPoint = (CPointPriority *)pMapPriority;
				pNewPoint = new CPointPriority;
				pNewPoint->m_bIsVisible = pMapPriority->m_bIsVisible;
				pNewPoint->m_bTransParent = pMapPriority->m_bTransParent;
				pNewPoint->m_enMapScale = pMapPriority->m_enMapScale;
				pNewPoint->m_iViewRatio = pMapPriority->m_iViewRatio;
				pNewPoint->m_strFileName = pMapPriority->m_strFileName;
				pNewPoint->m_strName = pMapPriority->m_strName;
				pNewPoint->m_uiPriority = pMapPriority->m_uiPriority;
				pNewPoint->m_crFillColor = pPoint->m_crFillColor;
				pNewPoint->m_pSymbol = pPoint->m_pSymbol;
				pNewPoint->m_strBitmap = pPoint->m_strBitmap;
				pNewPoint->m_uiRadius = pPoint->m_uiRadius;
				pNewPoint->m_uiType = pPoint->m_uiType;
				m_mappriority.Add(pNewPoint);
			}
			else if(pMapPriority->m_type_geo == "polyline")
			{
				pLine = (CPolylinePriority *)pMapPriority;
				pNewLine = new CPolylinePriority;
				pNewLine->m_bIsVisible = pMapPriority->m_bIsVisible;
				pNewLine->m_bTransParent = pMapPriority->m_bTransParent;
				pNewLine->m_enMapScale = pMapPriority->m_enMapScale;
				pNewLine->m_iViewRatio = pMapPriority->m_iViewRatio;
				pNewLine->m_strFileName = pMapPriority->m_strFileName;
				pNewLine->m_strName = pMapPriority->m_strName;
				pNewLine->m_uiPriority = pMapPriority->m_uiPriority;
				pNewLine->m_crLineColor = pLine->m_crLineColor;
				pNewLine->m_crLineColor2 = pLine->m_crLineColor2;
				pNewLine->m_uiLineWidth = pLine->m_uiLineWidth;
				pNewLine->m_uiLineWidth2 = pLine->m_uiLineWidth2;
				pNewLine->m_uiType = pLine->m_uiType;
				pNewLine->m_uiType2 = pLine->m_uiType2;
				m_mappriority.Add(pNewLine);
			}
			else if(pMapPriority->m_type_geo == "polygon")
			{
				pPolygon = (CPolygonPriority *)pMapPriority;
				pNewPolygon = new CPolygonPriority;
				pNewPolygon->m_bIsVisible = pMapPriority->m_bIsVisible;
				pNewPolygon->m_bTransParent = pMapPriority->m_bTransParent;
				pNewPolygon->m_enMapScale = pMapPriority->m_enMapScale;
				pNewPolygon->m_iViewRatio = pMapPriority->m_iViewRatio;
				pNewPolygon->m_strFileName = pMapPriority->m_strFileName;
				pNewPolygon->m_strName = pMapPriority->m_strName;
				pNewPolygon->m_uiPriority = pMapPriority->m_uiPriority;
				pNewPolygon->m_crFillColor = pPolygon->m_crFillColor;
				pNewPolygon->m_crLineColor = pPolygon->m_crLineColor;
				pNewPolygon->m_pPattern = pPolygon->m_pPattern;
				pNewPolygon->m_strBitmap = pPolygon->m_strBitmap;
				pNewPolygon->m_uiLineWidth = pPolygon->m_uiLineWidth;
				pNewPolygon->m_uiType = pPolygon->m_uiType;
				m_mappriority.Add(pNewPolygon);
			}
		}

		// 기본지도 text data 속성정보 교체
		for(int i=0; i<m_pTextPriority.GetSize(); i++)
			delete m_pTextPriority[i];
		m_pTextPriority.RemoveAll();
		for(int i=0; i<m_pCurUserMapInfo->m_pTextPriority.GetSize(); i++)
		{
			pTextPriority = (stTextPriority*)m_pCurUserMapInfo->m_pTextPriority[i];
			pNewTextPriority = new stTextPriority;
			pNewTextPriority->m_bIsVisible = pTextPriority->m_bIsVisible;
			pNewTextPriority->m_clFontColor = pTextPriority->m_clFontColor;
			pNewTextPriority->m_enMapScale = pTextPriority->m_enMapScale;
			pNewTextPriority->m_pFontType = pTextPriority->m_pFontType;
			pNewTextPriority->m_strFontName = pTextPriority->m_strFontName;
			pNewTextPriority->m_strFontScript = pTextPriority->m_strFontScript;
			pNewTextPriority->m_uiFontSize = pTextPriority->m_uiFontSize;
			pNewTextPriority->m_uiViewRatio = pTextPriority->m_uiViewRatio;
			pNewTextPriority->m_uiVisibleType = pTextPriority->m_uiVisibleType;
			m_pTextPriority.Add(pNewTextPriority);
		}

		// 지도데이터 속성정보 교체
		for(int i=0; i<m_vectormaps.GetSize(); i++)
		{
			pVectorMap = (stVectorMap*)m_vectormaps[i];
			for(int j=0; j<pVectorMap->m_pVObject.GetSize(); j++)
			{
				pVObject = (stVObject*)pVectorMap->m_pVObject[j];
				if(pVObject == NULL) continue;
				pMapProperty = (CMapProperty*)pVObject->m_pMapProperty;
				if(pMapProperty == NULL) continue;
//				pMapPriority = pVObject->m_paMapPriority[0];
				pMapPriority = FindMapPriority(pMapProperty->m_enMapScale, pMapProperty->m_strFileName);
				if(pMapPriority == NULL) continue;
				pVObject->m_paMapPriority.RemoveAll();
				for(int k=0; k<m_mappriority.GetSize(); k++)
				{
					if(m_mappriority[k]->m_enMapScale == pMapPriority->m_enMapScale &&
						m_mappriority[k]->m_strFileName == pMapPriority->m_strFileName)
						pVObject->m_paMapPriority.Add(m_mappriority[k]);
				}
			}
		}
	}
	// 사용자지도정보를 기본지도정보로 변환
	else
	{
		// temp buffer check
		if(m_tmpmappriority.GetSize() <= 0 || m_tmpTextPriority.GetSize() <= 0)
			return bRes;

		m_mappriority.RemoveAll();
		m_pTextPriority.RemoveAll();

		// 기본지도 속성정보 교체
		for(int i=0; i<m_tmpmappriority.GetSize(); i++)
		{
			pMapPriority = (CMapPriority*)m_tmpmappriority[i];
			m_mappriority.Add(pMapPriority);
		}

		// 기본지도 text data 속성정보 교체
		for(int i=0; i<m_tmpTextPriority.GetSize(); i++)
		{
			pTextPriority = (stTextPriority*)m_tmpTextPriority[i];
			m_pTextPriority.Add(pTextPriority);
		}

		// 지도데이터 속성정보 교체
		for(int i=0; i<m_vectormaps.GetSize(); i++)
		{
			pVectorMap = (stVectorMap*)m_vectormaps[i];
			for(int j=0; j<pVectorMap->m_pVObject.GetSize(); j++)
			{
				pVObject = (stVObject*)pVectorMap->m_pVObject[j];
				if(pVObject == NULL) continue;
				pMapProperty = (CMapProperty*)pVObject->m_pMapProperty;
				if(pMapProperty == NULL) continue;
				pMapPriority = FindMapPriority(pMapProperty->m_enMapScale, pMapProperty->m_strFileName);
				if(pMapPriority == NULL) continue;
//				pVObject->m_pMapPriority = pMapPriority;
//				pMapPriority = pVObject->m_paMapPriority[0];
				pVObject->m_paMapPriority.RemoveAll();
				for(int k=0; k<m_mappriority.GetSize(); k++)
				{
					if(m_mappriority[k]->m_enMapScale == pMapPriority->m_enMapScale &&
						m_mappriority[k]->m_strFileName == pMapPriority->m_strFileName)
						pVObject->m_paMapPriority.Add(m_mappriority[k]);
				}
			}
		}
	}

	return bRes;
}

BOOL CMapDataMgr::SaveBaseMapInfo(CArchive &ar, BOOL bIsAutoMode, EGMapKind eMapKind, EGMapScale eMapScale, 
								  int iMapRatio, long lMapMode, double dleft, double dtop, double dright, double dbottom, 
								  long lMapScale, BOOL bIsWorldMapOpen, int iMapType)
{
	CGRectGP gpSelRect;
	gpSelRect.SetRect(dleft, dtop, dright, dbottom);

// 현재 도시된 지도 정보를 사용자 저장 지도 정보에 추가
	if(m_pCurBaseMapInfo != NULL)
	{
		// jwpark, 2007-03-13
		ClearMapPriority(m_pCurBaseMapInfo);

		delete m_pCurBaseMapInfo;
		m_pCurBaseMapInfo = NULL;
	}

	m_pCurBaseMapInfo = new stUserMapInfo;
	m_pCurBaseMapInfo->m_strFileName = _T("");
	m_pCurBaseMapInfo->m_strSaveTime = _T("");
	m_pCurBaseMapInfo->m_eMapKind = eMapKind;
	m_pCurBaseMapInfo->m_eMapScale = eMapScale;
	m_pCurBaseMapInfo->m_eRasterMapScale = m_enCurMapScale;
	m_pCurBaseMapInfo->m_iMapRatio = iMapRatio;
	m_pCurBaseMapInfo->m_uiMapMode = (UINT)lMapMode;
	m_pCurBaseMapInfo->m_gpSelRect = gpSelRect;
	m_pCurBaseMapInfo->m_bIsAutoMap = bIsAutoMode;
	m_pCurBaseMapInfo->m_bIsWorldMap = bIsWorldMapOpen;
	m_pCurBaseMapInfo->m_uiMapType = (UINT)iMapType;

	Serialize(0, ar);

	return TRUE;
}

// by No1.. 2006-01-31 인자값 변경 CFile -> CArchive
BOOL CMapDataMgr::OpenBaseMapInfo(CArchive &ar)
{
//	Serialize(TRUE, ar);
	Serialize(0, ar);
	return TRUE;
}

stUserMapInfo* CMapDataMgr::GetBaseMapInfo()
{
	return m_pCurBaseMapInfo;
}

/** 사용자 저장지도 삭제 */
BOOL CMapDataMgr::DeleteUserMapInfo(CString strFile)
{
	stUserMapInfo *pItem;
	/* //by Xman94 2009-07-24 주석처리.
	for(int i=0; i<m_pUserMapInfo.GetSize(); i++)
	{
		pItem = (stUserMapInfo *)m_pUserMapInfo[i];
		if(pItem->m_strFileName == strFile)
		{
			for(int j=0; j<pItem->m_mappriority.GetSize(); j++)
			{
				if(pItem->m_mappriority[j]->m_strFileName.Mid(0,1) == "P")
					delete (CPointPriority *)pItem->m_mappriority[j];
				else if(pItem->m_mappriority[j]->m_strFileName.Mid(0,1) == "L")
					delete (CPolylinePriority *)pItem->m_mappriority[j];
				else if(pItem->m_mappriority[j]->m_strFileName.Mid(0,1) == "A")
					delete (CPolygonPriority *)pItem->m_mappriority[j];
			}
			pItem->m_mappriority.RemoveAll();

			for(j=0; j<pItem->m_pTextPriority.GetSize(); j++)
				delete pItem->m_pTextPriority[j];
			pItem->m_pTextPriority.RemoveAll();

			delete pItem;
			m_pUserMapInfo.RemoveAt(i);
			return TRUE;
		}
	}
	*/
	int nCnt = m_pUserMapInfo.GetSize() - 1;
	for(int i=nCnt; i >= 0; i--)
	{
		pItem = (stUserMapInfo *)m_pUserMapInfo[i];
		if(pItem->m_strFileName == strFile)
		{
			for(int j=0; j<pItem->m_mappriority.GetSize(); j++)
			{
				if(pItem->m_mappriority[j]->m_type_geo == "point")
					delete (CPointPriority *)pItem->m_mappriority[j];
				else if(pItem->m_mappriority[j]->m_type_geo == "polyline")
					delete (CPolylinePriority *)pItem->m_mappriority[j];
				else if(pItem->m_mappriority[j]->m_type_geo == "polygon")
					delete (CPolygonPriority *)pItem->m_mappriority[j];
			}
			pItem->m_mappriority.RemoveAll();
			
			for(int j=0; j<pItem->m_pTextPriority.GetSize(); j++)
				delete pItem->m_pTextPriority[j];
			pItem->m_pTextPriority.RemoveAll();
			
			delete pItem;
			m_pUserMapInfo.RemoveAt(i);
			return TRUE;
		}
	}

	return FALSE;
}

// 레스터 지도 속성정보 추가
void CMapDataMgr::AddRasterMapInfo(UINT uiSN, int iScale, CString strSubPath, CString strName, 
								   double left, double top, double right, double bottom)
{
	CString strPath, strTmp;
	strTmp = CMapConfigMgr::GetRasterDataPath();
	strPath.Format("%d만\\", iScale);
	strPath = strTmp + strPath;

	stRasterMap *pItem = new stRasterMap;
	pItem->m_iMapKey = uiSN;

	switch(iScale)
	{
		case 5 : pItem->m_enMapScale = MS_1_50000;
			break;
		case 10 : pItem->m_enMapScale = MS_1_100000;
			break;
		case 25 : pItem->m_enMapScale = MS_1_250000;
			break;
		case 50 : pItem->m_enMapScale = MS_1_500000;
			break;
		case 100 : pItem->m_enMapScale = MS_1_1000000;
			break;
		case 200 : pItem->m_enMapScale = MS_1_2000000;
			break;
		default : pItem->m_enMapScale = MS_NOTHING;
			break;
	}

	if(strSubPath != "NULL" && !strSubPath.IsEmpty())
		strPath = strPath + "\\" + strSubPath + "\\";
	
	pItem->m_strRootPath = strPath;
	pItem->m_strSubPath = strSubPath;
	pItem->m_strFileName = strName;

	if(pItem->m_enMapScale == MS_1_50000 || pItem->m_enMapScale == MS_1_100000)
	{
		//TRACE("%d\n", pItem->m_enMapScale);
		pItem->m_gpRect.SetRect(left, top, right, bottom);
	}
	else
		pItem->m_gpRect = ConvertUTMtoGP(left, top, right, bottom);

	pItem->m_bIsRasterDataOpen = FALSE;
	pItem->m_pRasterMap = NULL;

	m_pRasterMap.Add(pItem);
}

// 도시할 레스터 지도 데이터 추출
BOOL CMapDataMgr::ExtractRasterMap(BOOL bIsAutoMode, UINT uiRasterMapKind, CGRectGP rect, int iMapType, int iZoomRatio)
{
	BOOL bFirst = TRUE;
	BOOL bRes = FALSE;
	stRasterMap *pRasterMap;
	CGRectGP rectGP;
	CString strText;
	BOOL bIsOpen;
	
	int k = 0;
	int cnt = m_pRasterMap.GetSize();

	for(int i=0; i<cnt; i++)
	{
		pRasterMap = m_pRasterMap[i];
		if( uiRasterMapKind == 50000 && MS_1_50000 != pRasterMap->m_enMapScale ) continue;
		else if( uiRasterMapKind == 100000 && MS_1_100000 != pRasterMap->m_enMapScale ) continue;
		else if( uiRasterMapKind == 250000 && MS_1_250000 != pRasterMap->m_enMapScale ) continue;
		else if( uiRasterMapKind == 500000 && MS_1_500000 != pRasterMap->m_enMapScale ) continue;
		else if( uiRasterMapKind == 1000000 && MS_1_1000000 != pRasterMap->m_enMapScale ) continue;
		else if( uiRasterMapKind == 2000000 && MS_1_2000000 != pRasterMap->m_enMapScale ) continue; 
  
		if(IsLoadRasterMap(bIsAutoMode, iZoomRatio, pRasterMap))
		{
			if(uiRasterMapKind == 2000000)
				bIsOpen = TRUE;
			else
				bIsOpen = DupCheck2(pRasterMap->m_gpRect, rect, pRasterMap->m_enMapScale);
			if(bIsOpen)
			{
				// "TL2", "TC2" format open
				if(pRasterMap->m_enMapScale == MS_1_50000 || pRasterMap->m_enMapScale == MS_1_100000)
				{
					OpenNitf2Map(pRasterMap, rect);
				}
				// "tif" format open
				else
				{
					OpenRasterMap(pRasterMap, rect);
				}
				bRes = TRUE;
			}
			else
				DeleteRasterMap(pRasterMap);
		}
		else
			DeleteRasterMap(pRasterMap);
	}	
	return bRes;
}

// 레스터맵 스케일 체크
BOOL CMapDataMgr::IsLoadRasterMap(BOOL bIsAutoMode, int iRatio, stRasterMap *pRasterMap)
{
	BOOL bRes = FALSE;
	int iminscale1, imaxscale1;
	int iminscale, imaxscale;
	if(!CMapConfigMgr::GetMapMinMaxScale(iminscale1, imaxscale1))
		return bRes;
	if(iRatio < iminscale1 || iRatio > imaxscale1)
		return bRes;
	if(!CMapConfigMgr::GetRasterMapScale(pRasterMap->m_enMapScale, iminscale, imaxscale))
		return bRes;

	if(bIsAutoMode)
	{
		if(iRatio >= iminscale && iRatio <= imaxscale)
		{
			m_enCurMapScale = pRasterMap->m_enMapScale;
			bRes = TRUE;
		}
	}
	else
	{
		if(m_enCurMapScale == pRasterMap->m_enMapScale)
		{
			if(iRatio >= iminscale1 && iRatio <= imaxscale)
				bRes = TRUE;
		}
	}

	return bRes;
}

/** 레스터 지도 열기 */
BOOL CMapDataMgr::OpenRasterMap(stRasterMap *pRasterMap, CGRectGP rect)
{
	TIFF *image = (TIFF*)0;
	UINT32 xsize, ysize;
	UINT32 numTiePoints, numPixelPoints;

	double x1, y1, x2, y2;
	double *pGeoPoints = NULL;
	double *pGeoPixelScale = NULL;

	// 중복 열기 체크
	if(pRasterMap->m_bIsRasterDataOpen == TRUE && pRasterMap->m_pRasterMap != NULL)
		return FALSE;

	// tiff format file open
	CString strExt = ".tif";
	CString strFileName = pRasterMap->m_strRootPath + _T("\\") + pRasterMap->m_strFileName + strExt;

	CFileFind find;
	BOOL bRet = find.FindFile(strFileName);
	if(bRet == FALSE)
		return FALSE;

	if((image = XTIFFOpen(strFileName, "r")) == NULL)
		return FALSE;

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
	CDIBSectionLite *pImageBitmap;
	
	
	
	//GeoTIFFInfoStruct* info = new GeoTIFFInfoStruct();//..메모리 
	//HBITMAP hBitmap = CreateBitmapFromGeoTIFFile(strFileName,(GeoTIFFInfoStruct*)info, 1);

	HBITMAP hBitmap=0;
	
	if(hBitmap)
	{
		pImageBitmap = new CDIBSectionLite();
		pImageBitmap->SetBitmap(hBitmap);
		//할당된 HBITMAP을 반환한다.
		DeleteObject(hBitmap);
		pRasterMap->m_pRasterMap = pImageBitmap;
		pRasterMap->m_bIsRasterDataOpen = TRUE;
		pRasterMap->m_gpRealRect = ConvertUTMtoGP(x1, y1, x2, y2);
	}
	
	//delete (GeoTIFFInfoStruct*)info;
	
	return TRUE;
}

/** 레스터 지도 열기 NITF("TL2") format */
BOOL CMapDataMgr::OpenNitf2Map(stRasterMap *pRasterMap, CGRectGP rect)
{
	// 중복 열기 체크
	if(pRasterMap->m_bIsRasterDataOpen == TRUE && pRasterMap->m_pRasterMap != NULL)
		return FALSE;

	CString strExt;

	// tl2 format file open
	if(pRasterMap->m_enMapScale == MS_1_50000)
		strExt = ".tl2";
	else if(pRasterMap->m_enMapScale == MS_1_100000)
		strExt = ".tc2";
	else
		return FALSE;
	CString strFileName = pRasterMap->m_strRootPath + pRasterMap->m_strFileName + strExt;

	CFileFind find;
	BOOL bRet = find.FindFile(strFileName);
	if(bRet == FALSE)
		return FALSE;

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

	return TRUE;
}

/** UTM 좌표를 경위도 좌표로 변환, 한반도 52 zone을 기준으로 변환 */
CGRectGP CMapDataMgr::ConvertUTMtoGP(double left, double top, double right, double bottom)
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

/** 레스터 지도 삭제 */
void CMapDataMgr::DeleteRasterMap(stRasterMap *pRasterMap)
{
	if(pRasterMap == NULL || pRasterMap->m_pRasterMap == NULL)
	{
		pRasterMap->m_bIsRasterDataOpen = FALSE;
		return;
	}
	
	if( pRasterMap->m_pRasterMap->GetSafeHandle() && pRasterMap->m_pRasterMap != NULL )
		//delete 
		pRasterMap->m_pRasterMap->DeleteObject();

	pRasterMap->m_pRasterMap = NULL;
	pRasterMap->m_bIsRasterDataOpen = FALSE;
}

/** 레스터 지도 레코드 개수 얻기 */
int CMapDataMgr::GetRasterMapCount()
{
	return m_pRasterMap.GetSize();
}

/** 레스터 지도 레코드 얻기 */
stRasterMap * CMapDataMgr::GetRasterMap(int iIndex)
{
	return m_pRasterMap[iIndex];
}

/** 레스터 지도 도시 */
void CMapDataMgr::DrawRasterMap(CGDCGeo *pDC, stRasterMap *pRasterMap)
{
	if(pRasterMap->m_enMapScale == MS_1_50000 ||
		pRasterMap->m_enMapScale == MS_1_100000)
		pRasterMap->m_pRasterMap->DrawCADRG(pDC, pRasterMap->m_gpRect.m_R.left, 
										pRasterMap->m_gpRealRect.m_R.top,
										pRasterMap->m_gpRealRect.m_R.right,
										pRasterMap->m_gpRealRect.m_R.bottom);
	else
		pRasterMap->m_pRasterMap->Draw(pDC, pRasterMap->m_gpRealRect.m_R.left, 
										pRasterMap->m_gpRealRect.m_R.top,
										pRasterMap->m_gpRealRect.m_R.right,
										pRasterMap->m_gpRealRect.m_R.bottom);
}

//////////////////////////////////////////////////////////////////////////////////////
void CMapDataMgr::DrawRasterMap(CDC *pDC, stRasterMap *pRasterMap)
{
	if(pRasterMap->m_enMapScale == MS_1_50000 ||
		pRasterMap->m_enMapScale == MS_1_100000)
		pRasterMap->m_pRasterMap->DrawCADRG(pDC, pRasterMap->m_gpRealRect.m_R.left, 
										pRasterMap->m_gpRealRect.m_R.top,
										pRasterMap->m_gpRealRect.m_R.right,
										pRasterMap->m_gpRealRect.m_R.bottom);
	else
		pRasterMap->m_pRasterMap->Draw(pDC, pRasterMap->m_gpRealRect.m_R.left, 
										pRasterMap->m_gpRealRect.m_R.top,
										pRasterMap->m_gpRealRect.m_R.right,
										pRasterMap->m_gpRealRect.m_R.bottom);
}

void CMapDataMgr::DrawRaster(CDC *pDC, stRasterMap *pRasterMap, int nScreenWidth, int nScreenHeight,
								 float fCenterX, float fCenterY, float fScale)
{
	pRasterMap->m_pRasterMap->SetStartEnd(pRasterMap->m_gpRealRect.m_R.left, pRasterMap->m_gpRealRect.m_R.top, 
											pRasterMap->m_gpRealRect.m_R.right, pRasterMap->m_gpRealRect.m_R.bottom);

	pRasterMap->m_pRasterMap->DrawRaster(pDC, nScreenWidth, nScreenHeight, fCenterX, fCenterY, fScale);	
}

////////////////////////////////////////////////////////////////////////////////////

/** 모든 레스터 지도 삭제 */
void CMapDataMgr::RemoveAllRasterMap()
{
	for(int i=0; i<m_pRasterMap.GetSize(); i++)
	{
		if(m_pRasterMap[i]->m_pRasterMap != NULL)
		{
			delete m_pRasterMap[i]->m_pRasterMap;
			m_pRasterMap[i]->m_pRasterMap = NULL;
			m_pRasterMap[i]->m_bIsRasterDataOpen = FALSE;
		}
	}
}

void CMapDataMgr::ProgressDlgHide()
{
	//if(m_pProgressDlg != NULL)
	//	m_pProgressDlg->ShowWindow(SW_HIDE);
}

stSymbol* CMapDataMgr::GetSelPointSymbol()
{
	return m_pSelPointSymbol;
}

/** Marking flag set */
void CMapDataMgr::SetSelMarkingFlag(BOOL bIsSelMarking, CGPointGP gpoint)
{
	m_bIsSelMarking = bIsSelMarking;
	m_gpSelMarking = gpoint;
}

/** Marking flag value get */
BOOL CMapDataMgr::GetSelMarkingFlag(CGPointGP &gpoint)
{
	gpoint = &m_gpSelMarking;
	return m_bIsSelMarking;
}

/** small map Rect value get*/
CGRectGP CMapDataMgr::GetSmallMapRect()
{
	CGRectGP rectGP(m_smallmapMBR.xmin, m_smallmapMBR.ymax, m_smallmapMBR.xmax, m_smallmapMBR.ymin);
	return rectGP;
}

/*
92,5,PAT050,0,0,통신건물,3,000000000,1,
93,5,PAT080,1,1,통신탑,5,255255255,2,17_tower_9px
44,5,LFA020,0,0,휴전선,3,128064000,3,0,000000000,999
69,5,LAN050,0,0,철로 인입선,1,000000000,0,1,000000000,7
1,5,ABA040,0,1,바닷물,0,1,074073255,074073255,
8,5,ABH135,0,1,논,1,0,255255255,255255255,pattern10
*/
/** write mappriority file */
void CMapDataMgr::WriteMapPriority(CFile &file, CMapPriority *pPriority)
{
	if(pPriority == NULL) return;

	CPointPriority *pPoint;
	CPolylinePriority *pLine;
	CPolygonPriority *pPoly;
	CString strDeli, strSN, strScale, strFileName, strTransparent, strIsVisible, strName, strRadius, strViewRatio, 
		strFillColor, strType, strPath, strLineWidth, strLineWidth2, strLineColor, strLineColor2, 
		strType2, strTmp, strData, type_geo, earth_or_air;
	BYTE btR, btG, btB;
	strDeli = ",";

	// file write
	try
	{		
		//CHOIKYUBEOM 속성 추가 저장 (파일에 쓰기)
		type_geo = pPriority->m_type_geo;				
		earth_or_air = pPriority->m_earth_or_air;

		// 점 데이터 처리
		if(pPriority->m_type_geo == "point")
		{
			pPoint = (CPointPriority*)pPriority;
			strSN.Format("%d", pPoint->m_uiPriority);
			switch(pPoint->m_enMapScale)
			{	
			    case MS_1_2000000 : strScale = "200"; break;
				case MS_1_1000000 : strScale = "100"; break;
				case MS_1_500000 : strScale = "50"; break;
				case MS_1_250000 : strScale = "25"; break;
				case MS_1_100000 : strScale = "10"; break;
				case MS_1_50000 : strScale = "5"; break;
				default : strScale = "0"; break;
			}
			strFileName = pPoint->m_strFileName;
			strViewRatio.Format("%d", pPoint->m_iViewRatio);
			strTransparent = (pPoint->m_bTransParent) ? "1" : "0";
			strIsVisible = (pPoint->m_bIsVisible) ? "1" : "0";
			strName = pPoint->m_strName;



			strRadius.Format("%d", pPoint->m_uiRadius);

			btR = GetRValue(pPoint->m_crFillColor);
			btG = GetGValue(pPoint->m_crFillColor);
			btB = GetBValue(pPoint->m_crFillColor);
			strFillColor.Format("%03d%03d%03d", btR, btG, btB);

			strType.Format("%d", pPoint->m_uiType);
			strPath = pPoint->m_strBitmap;

			strData = strSN + strDeli +
				strScale + strDeli +
				strFileName + strDeli +
				strViewRatio + strDeli +
				strTransparent + strDeli +
				strIsVisible + strDeli +
				strName + strDeli +
				type_geo + strDeli +
				earth_or_air + strDeli +
				strRadius + strDeli +
				strFillColor + strDeli +
				strType + strDeli +
				strPath + "\r\n";

			file.Write(strData, strData.GetLength());
		}
		// 선 데이터 처리
		else if(pPriority->m_type_geo == "polyline")
		{
			pLine = (CPolylinePriority*)pPriority;
			strSN.Format("%d", pLine->m_uiPriority);
			switch(pLine->m_enMapScale)
			{
				case MS_1_2000000 : strScale = "200"; break;				
				case MS_1_1000000 : strScale = "100"; break;
				case MS_1_500000 : strScale = "50"; break;
				case MS_1_250000 : strScale = "25"; break;
				case MS_1_100000 : strScale = "10"; break;
				case MS_1_50000 : strScale = "5"; break;
				default : strScale = "0"; break;
			}
			strFileName = pLine->m_strFileName;
			strViewRatio.Format("%d", pLine->m_iViewRatio);
			strTransparent = (pLine->m_bTransParent) ? "1" : "0";
			strIsVisible = (pLine->m_bIsVisible) ? "1" : "0";
			strName = pLine->m_strName;

			strLineWidth.Format("%d", pLine->m_uiLineWidth);

			btR = GetRValue(pLine->m_crLineColor);
			btG = GetGValue(pLine->m_crLineColor);
			btB = GetBValue(pLine->m_crLineColor);
			strLineColor.Format("%03d%03d%03d", btR, btG, btB);
			strType.Format("%d", pLine->m_uiType);

			strLineWidth2.Format("%d", pLine->m_uiLineWidth2);

			btR = GetRValue(pLine->m_crLineColor2);
			btG = GetGValue(pLine->m_crLineColor2);
			btB = GetBValue(pLine->m_crLineColor2);
			strLineColor2.Format("%03d%03d%03d", btR, btG, btB);
			strType2.Format("%d", pLine->m_uiType2);

			strData = strSN + strDeli +
				strScale + strDeli +
				strFileName + strDeli +
				strViewRatio + strDeli +
				strTransparent + strDeli +
				strIsVisible + strDeli +
				strName + strDeli +
				type_geo + strDeli +
				earth_or_air + strDeli +

				strLineWidth + strDeli +
				strLineColor + strDeli +
				strType + strDeli +
				strLineWidth2 + strDeli +
				strLineColor2 + strDeli +
				strType2 + "\r\n";

			file.Write(strData, strData.GetLength());
		}
		// 면 데이터 처리
		else if(pPriority->m_type_geo == "polygon")
		{
			pPoly = (CPolygonPriority*)pPriority;
			strSN.Format("%d", pPoly->m_uiPriority);
			switch(pPoly->m_enMapScale)
			{
				case MS_1_2000000 : strScale = "200"; break;
				case MS_1_1000000 : strScale = "100"; break;
				case MS_1_500000 : strScale = "50"; break;
				case MS_1_250000 : strScale = "25"; break;
				case MS_1_100000 : strScale = "10"; break;
				case MS_1_50000 : strScale = "5"; break;
				default : strScale = "0"; break;
			}
			strFileName = pPoly->m_strFileName;
			strViewRatio.Format("%d", pPoly->m_iViewRatio);
			strTransparent = (pPoly->m_bTransParent) ? "1" : "0";
			strIsVisible = (pPoly->m_bIsVisible) ? "1" : "0";
			strName = pPoly->m_strName;

			strType.Format("%d", pPoly->m_uiType);
			strLineWidth.Format("%d", pPoly->m_uiLineWidth);

			btR = GetRValue(pPoly->m_crLineColor);
			btG = GetGValue(pPoly->m_crLineColor);
			btB = GetBValue(pPoly->m_crLineColor);
			strLineColor.Format("%03d%03d%03d", btR, btG, btB);

			btR = GetRValue(pPoly->m_crFillColor);
			btG = GetGValue(pPoly->m_crFillColor);
			btB = GetBValue(pPoly->m_crFillColor);
			strFillColor.Format("%03d%03d%03d", btR, btG, btB);

			strPath = pPoly->m_strBitmap;

			strData = strSN + strDeli +
				strScale + strDeli +
				strFileName + strDeli +
				strViewRatio + strDeli +
				strTransparent + strDeli +
				strIsVisible + strDeli +
				strName + strDeli +
				type_geo + strDeli +
				earth_or_air + strDeli +

				strType + strDeli +
				strLineWidth + strDeli +
				strLineColor + strDeli +
				strFillColor + strDeli +
				strPath + "\r\n";

			file.Write(strData, strData.GetLength());
		}
	}
	catch(...)
	{
		TRACE("error: WriteMapPriority\n");
	}
}

/*
100,1,0,11,000000000,300,돋움,한글
50,1,0,11,000000000,300,돋움,한글
25,1,0,11,000000000,300,돋움,한글
10,1,0,11,000000000,300,돋움,한글
5,1,0,11,000000000,300,돋움,한글
*/
/** write textpriority file */
void CMapDataMgr::WriteTextPriority(CFile &file, stTextPriority *pPriority)
{
	if(pPriority == NULL) return;

	CString strData, strDeli, strScale, strVisible, strVisibleType, 
		strFontSize, strFontColor, strRatio, strFontName, strFontScript;
	BYTE btR, btG, btB;
	strDeli = ",";

	try
	{
		if(pPriority->m_strWorldMapPoint.IsEmpty())
		{
			switch(pPriority->m_enMapScale)
			{
				case MS_1_2000000 : strScale = "200"; break;
				case MS_1_1000000 : strScale = "100"; break;
				case MS_1_500000 : strScale = "50"; break;
				case MS_1_250000 : strScale = "25"; break;
				case MS_1_100000 : strScale = "10"; break;
				case MS_1_50000 : strScale = "5"; break;
				default : strScale = "0"; break;
			}
		}
		else
			strScale = pPriority->m_strWorldMapPoint;
		strVisible = (pPriority->m_bIsVisible) ? "1" : "0";
		strVisibleType.Format("%d", pPriority->m_uiVisibleType);
		strFontSize.Format("%d", pPriority->m_uiFontSize);

		btR = GetRValue(pPriority->m_clFontColor);
		btG = GetGValue(pPriority->m_clFontColor);
		btB = GetBValue(pPriority->m_clFontColor);
		strFontColor.Format("%03d%03d%03d", btR, btG, btB);

		strRatio.Format("%d", pPriority->m_uiViewRatio);
		strFontName = pPriority->m_strFontName;
		strFontScript = pPriority->m_strFontScript;

		strData = strScale + strDeli +
			strVisible + strDeli +
			strVisibleType + strDeli +
			strFontSize + strDeli +
			strFontColor + strDeli +
			strRatio + strDeli +
			strFontName + strDeli +
			strFontScript + "\r\n";

		file.Write(strData, strData.GetLength());
	}
	catch(...)
	{
		TRACE("error: WriteTextPriority\n");
	}
}

// 도시할 위성영상 지도 데이터 추출
BOOL CMapDataMgr::ExtractSatelliteMap(BOOL bIsAutoMode, CGRectGP rect, int iMapType, int iZoomRatio)
{
	//CBAMgr::SetDispMapType(1);
	BOOL bFirst = TRUE;
	BOOL bRes = FALSE;
	stSatelliteMapIndex *pIndex = NULL;
	CDIBSectionLite *pDIBSectionLite = NULL;
	CGRectGP rectGP;
	CString strText;	

	// ckb 위성영상 도시 테스트 위해 !!!
	// 레스터 지도 항상 도시일 경우
	//iMapType = 1 ;	
		
	//iMapType = 0 : 벡터
	//iMapType = 1 : 레스터
	//iMapType = 2 : 벡터 + 레스터
	if(iMapType != 0)
	{	
		int cnt_satellte_index = m_paSatelliteMapIndex.GetSize();
 

		for(int i=0; i < cnt_satellte_index ; i++)
		{
			pIndex = m_paSatelliteMapIndex[i];
			if(IsLoadSatelliteMap(bIsAutoMode, iZoomRatio, pIndex))
			{
				if(DupCheck2(pIndex->m_gpRect, rect, pIndex->m_enMapScale))
				{
					if(pIndex->m_enMapScale == MS_1_50000)
					{
						if(OpenSatelliteMap1m(pIndex, rect))
						{
							/*
							strText.Format("%d. ", pIndex->m_iMapKey);
							strText = strText + " Loading...";
							if(bFirst == TRUE && m_pProgressDlg )
							{
								m_pProgressDlg->ShowWindow(SW_SHOWNOACTIVATE); 
								m_pProgressDlg->ProgressStart(strText, 100);
								bFirst = FALSE;
								m_pProgressDlg->ProgressSetIt(strText);
							}
						 	m_pProgressDlg->ProgressSetIt(strText);
							*/
						}
						bRes = TRUE;
					}
					else
					{
						//20110907 이규철, 위성영상 1미터만 도시하기로 해서 아래 코드를 바꿔놓는댜
						//if(OpenSatelliteMap(pIndex, rect)) -> if(OpenSatelliteMap1m(pIndex, rect))
						//if(OpenSatelliteMap(pIndex, rect))
						if(OpenSatelliteMap1m(pIndex, rect))
						{
							/*
							strText.Format("%d. ", pIndex->m_iMapKey);
							strText = strText + " Loading...";
							if(bFirst == TRUE   && m_pProgressDlg )
							{
								m_pProgressDlg->ShowWindow(SW_SHOWNOACTIVATE); 
								m_pProgressDlg->ProgressStart(strText, 100);
								bFirst = FALSE;
								m_pProgressDlg->ProgressSetIt(strText);
							}
						//	m_pProgressDlg->ProgressSetIt(strText);
						*/
						}
						bRes = TRUE;
					}
				}
				else
				{
					if(pIndex->m_pData != NULL)
					{
						pDIBSectionLite = (CDIBSectionLite *)(pIndex->m_pData);
						delete pDIBSectionLite;
						pIndex->m_pData = NULL;
					}
				}
			}
			else
			{
				if(pIndex->m_pData != NULL)
				{
					pDIBSectionLite = (CDIBSectionLite *)(pIndex->m_pData);
					delete pDIBSectionLite;
					pIndex->m_pData = NULL;
				}
			}
		}
	}
	else
	{
		//RemoveAllSatelliteMapData();
	}

	
	if(bRes) 
	{
		RemoveAllRasterMap();
	//	if( m_pProgressDlg )
	//		m_pProgressDlg->ShowWindow(SW_HIDE);
	}
	

	return bRes;
}

/** 위성영상 스케일 체크 */
BOOL CMapDataMgr::IsLoadSatelliteMap(BOOL bIsAutoMode, int iRatio, stSatelliteMapIndex *pIndex)
{
	BOOL bRes = FALSE;
	int iminscale1, imaxscale1;
	int iminscale, imaxscale;
	if(!CMapConfigMgr::GetMapMinMaxScale(iminscale1, imaxscale1))
		return bRes;
	if(iRatio < iminscale1 || iRatio > imaxscale1)
		return bRes;
	if(!CMapConfigMgr::GetRasterMapScale(pIndex->m_enMapScale, iminscale, imaxscale))
		return bRes;

	if(bIsAutoMode)
	{	
		
		//if(iRatio >= iminscale && iRatio <= imaxscale)
		if(iRatio >= iminscale && iRatio <= 2500 && pIndex->m_enMapScale == MS_1_1)
		{
			//m_enCurMapScale = pIndex->m_enMapScale;
			m_enCurMapScale = pIndex->m_enMapScale;
			bRes = TRUE;
		}		
	}
	else
	{
		//if(m_enCurMapScale == pIndex->m_enMapScale)
		if(m_enCurMapScale == MS_1_50000 && iRatio <= 2500)
		{
			if(iRatio >= iminscale1 && iRatio <= imaxscale)
				bRes = TRUE;
		}
	}
	return bRes;
}

// 위성영상 지도 열기(10m, 20m)
BOOL CMapDataMgr::OpenSatelliteMap(stSatelliteMapIndex *pIndex, CGRectGP rect)
{
	// 중복 열기 체크
	if(pIndex->m_pData != NULL)
		return FALSE;

	CString strRootPath, strSubPath, strFileName;
	GetSatelliteMapInfo(pIndex, strRootPath, strSubPath, strFileName);

	// tiff format file open
	CString strExt = ".tif";
	strFileName = strRootPath + strFileName + strExt;

	CFileFind find;
	BOOL bRet = find.FindFile(strFileName);
	if(bRet == FALSE)
		return FALSE;

	int xsize, ysize;
	TIFF *tif = (TIFF*)NULL;

	UINT16 numTiePoints, numPixelPoints;
	double x1, y1, x2, y2;
	double *pGeoPoints;
	double *pGeoPixelScale;

	tif = XTIFFOpen(strFileName,"r");
	if(tif == NULL)
		return FALSE;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &xsize);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &ysize);

	TIFFGetField(tif, TIFFTAG_GEOTIEPOINTS, &numTiePoints, &pGeoPoints);
	TIFFGetField(tif, TIFFTAG_GEOPIXELSCALE, &numPixelPoints, &pGeoPixelScale);

	x1 = pGeoPoints[3];
	y1 = pGeoPoints[4];
	x2 = x1 + xsize*pGeoPixelScale[0];
	y2 = y1 - ysize*pGeoPixelScale[1];

	XTIFFClose(tif);

	// Image Loading
	CDIBSectionLite *pImageBitmap;
	//GeoTIFFInfoStruct info;
	//HBITMAP hBitmap = CreateBitmapFromGeoTIFFile(strFileName, &info, 1);
	HBITMAP hBitmap=0;

	if(hBitmap)
	{
		pImageBitmap = new CDIBSectionLite;
		pImageBitmap->SetBitmap(hBitmap);
		//할당된 HBITMAP을 반환한다.
		DeleteObject(hBitmap);
		pIndex->m_pData = pImageBitmap;
		pIndex->m_gpRealRect = ConvertUTMtoGP(x1, y1, x2, y2);
	}

	return TRUE;
}

// 위성영상 지도 열기(1m)
BOOL CMapDataMgr::OpenSatelliteMap1m(stSatelliteMapIndex *pIndex, CGRectGP rect)
{
	// 중복 열기 체크
	if(pIndex->m_pData != NULL)
		return FALSE;

	CString strRootPath, strSubPath, strFileName;
	GetSatelliteMapInfo(pIndex, strRootPath, strSubPath, strFileName);

	// tiff format file open
	CString strExt = ".i42";
	strFileName = strRootPath + strFileName + strExt;

	CFileFind find;
	BOOL bRet = find.FindFile(strFileName);
	if(bRet == FALSE)
		return FALSE;

	BITMAPINFO pBmi;
	bool isok;
	byte *buf = 0x00;

	CGRectGP realrect;
	CDIBSectionLite *pImageBitmap = new CDIBSectionLite;
	buf = pImageBitmap->ReadCIB(strFileName, &pBmi, &isok, &realrect);

	if(isok == FALSE)
	{
		delete pImageBitmap;
		//by Xman94 추가.
		pImageBitmap = NULL;
		if(buf != NULL)
		{
			delete buf;
			//by Xman94 추가.
			buf = NULL;
		}
		return FALSE;
	}

	pImageBitmap->SetBitmap(&pBmi, buf);
	pIndex->m_pData = pImageBitmap;
	pIndex->m_gpRealRect = realrect;

	if(buf != NULL)
	{
		delete buf;
		//by Xman94 추가.
		buf = NULL;
	}

	return TRUE;
}

/** 위성영상 지도 도시 */
void CMapDataMgr::DrawSatelliteMap(CGDCGeo *pDC, stSatelliteMapIndex *pIndex)
{
	CDIBSectionLite *pDIBSectionLite = (CDIBSectionLite *)pIndex->m_pData;
	if(pIndex->m_enMapScale == MS_1_1)
		pDIBSectionLite->DrawCIB(pDC, pIndex->m_gpRealRect.m_R.left, 
								pIndex->m_gpRealRect.m_R.top,
								pIndex->m_gpRealRect.m_R.right,
								pIndex->m_gpRealRect.m_R.bottom);
	else
		pDIBSectionLite->Draw(pDC, pIndex->m_gpRealRect.m_R.left, 
							pIndex->m_gpRealRect.m_R.top,
							pIndex->m_gpRealRect.m_R.right,
							pIndex->m_gpRealRect.m_R.bottom);
}

void CMapDataMgr::DrawSatelliteMap(CDC *pDC, stSatelliteMapIndex *pIndex)
{
	CDIBSectionLite *pDIBSectionLite = (CDIBSectionLite *)pIndex->m_pData;
	if(pIndex->m_enMapScale == MS_1_1)
		pDIBSectionLite->DrawCIB(pDC, pIndex->m_gpRealRect.m_R.left, 
								pIndex->m_gpRealRect.m_R.top,
								pIndex->m_gpRealRect.m_R.right,
								pIndex->m_gpRealRect.m_R.bottom);
	else
		pDIBSectionLite->Draw(pDC, pIndex->m_gpRealRect.m_R.left, 
							pIndex->m_gpRealRect.m_R.top,
							pIndex->m_gpRealRect.m_R.right,
							pIndex->m_gpRealRect.m_R.bottom);
}

/** 위성영상 지도 모두 지우기 */
void CMapDataMgr::RemoveAllSatelliteMapData()
{	
	/*
	CDIBSectionLite *pItem;
	for(int i=0; i<m_paSatelliteMapIndex.GetSize(); i++)
	{
		if(m_paSatelliteMapIndex[i]->m_pData != NULL)
		{
			pItem = (CDIBSectionLite *)m_paSatelliteMapIndex[i]->m_pData;
			delete pItem;
			m_paSatelliteMapIndex[i]->m_pData = NULL;
		}
	}
	*/

	CDIBSectionLite* pCDIBSL = NULL;
	for( int i=0; i<m_paSatelliteMapIndex.GetSize(); i++)
	{ 
		if(m_paSatelliteMapIndex[i] != NULL)
		{
			pCDIBSL = (CDIBSectionLite*)m_paSatelliteMapIndex[i]->m_pData;
			SAFE_DELETE(pCDIBSL);
			
			SAFE_DELETE(m_paSatelliteMapIndex[i]);
		}
	}
	m_paSatelliteMapIndex.RemoveAll();
}

/** map area calc (left/top base) */
CGRectGP CMapDataMgr::CalcMapSelArea(CGDCGeo *pDC, CSize size, CGPointGP gpoint)
{
	CGRectGP rectGP;
	CRect rect;
	CPoint point, pointRB;
	CGPointGP gpointRB;

	// left, top set
	rectGP.m_R.left = gpoint.m_P.lon;
	rectGP.m_R.top = gpoint.m_P.lat;

	// 지리좌표 -> 화면좌표로 변환
	pDC->GeoPtoDP(&point, &gpoint);

	// right, bottom calc
	pointRB.x = point.x + size.cx;
	pointRB.y = point.y + size.cy;

	// 계산된 우하단 화면좌료를 지리좌표로 변환
	pDC->DPtoGeoP(&gpointRB, &pointRB);

	rectGP.m_R.right = gpointRB.m_P.lon;
	rectGP.m_R.bottom = gpointRB.m_P.lat;

	return rectGP;
}

/** LT point clac */
CGPointGP CMapDataMgr::CalcMapLTPoint(CGDCGeo *pDC, CSize size, CGRectGP &rectGP)
{
	CGPointGP center;
	CPoint point;
	CRect rect;
	double left, top, right, bottom;

	pDC->GeoPtoDP(&rect, &rectGP);
	left = rect.left;
	top = rect.top;
	right = rect.right;
	bottom = rect.bottom;

	point.x = long((left+right) / 2);
	point.y = long((top+bottom) / 2);

	point.x = point.x - (size.cx/2);
	point.y = point.y - (size.cy/2);

	pDC->DPtoGeoP(&center, &point);

	return center;
}

/** GeoTiff format map data open, rect value input */
BOOL CMapDataMgr::OpenGeoTiffData(CString strFileName, double &left, double &right, double &top, double &bottom)
{
	BOOL bRes = TRUE;

	int xsize, ysize;
	TIFF *tif=(TIFF*)NULL;
	UINT16 numTiePoints, numPixelPoints;
	double *pGeoPoints = NULL;
	double *pGeoPixelScale = NULL;

	tif = XTIFFOpen(strFileName,"r");
	if(tif == NULL)
		return FALSE;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &xsize);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &ysize);
	TIFFGetField(tif, TIFFTAG_GEOTIEPOINTS, &numTiePoints, &pGeoPoints);
	TIFFGetField(tif, TIFFTAG_GEOPIXELSCALE, &numPixelPoints, &pGeoPixelScale);

	if(pGeoPoints != NULL)
	{
		left = pGeoPoints[3];
		top = pGeoPoints[4];
	}

	if(pGeoPixelScale != NULL)
	{
		right = left + xsize*pGeoPixelScale[0];
		bottom = top - ysize*pGeoPixelScale[1];
	}

	XTIFFClose(tif);

	return bRes;
}

/** Nitf format map data open, rect value input */
BOOL CMapDataMgr::OpenNitfData(CString strExt, CString strFileName, double &left, double &right, double &top, double &bottom)
{
	BOOL bRes = TRUE;   // 리턴 값.
	BITMAPINFO pBmi;    // 비트맵 정보
	bool isok;          // ReadCIB() & ReadCADRG() 함수 성공여부.
	byte *buf = 0x00;   // 비트맵 버퍼
	strExt.MakeUpper();

	CFileFind find;
	BOOL bRet = find.FindFile(strFileName);
	if(bRet == FALSE)   // 파일이 존재하지 않으면 FALSE 리턴.
		return FALSE;

	CGRectGP realrect;
	CDIBSectionLite *pImageBitmap = new CDIBSectionLite;  
	if(strExt == "I42")  // 1m 영상이면.
		buf = pImageBitmap->ReadCIB(strFileName, &pBmi, &isok, &realrect);
	else if(strExt == "TL2" || strExt == "TC2")  // 10m & 20m 영상이면.
		buf = pImageBitmap->ReadCADRG(strFileName, &pBmi, &isok, &realrect);

	if(isok == FALSE)  // 함수 실패시 메모리 해제 후 FALSE 리턴.
	{
		delete pImageBitmap;
		if(buf != NULL)
			delete buf;
		return FALSE;
	}

	if(buf != NULL)
		delete buf;
	delete pImageBitmap;

	left = realrect.m_R.left;      // 위성영상 영역.
	right = realrect.m_R.right;
	top = realrect.m_R.top;
	bottom = realrect.m_R.bottom;

	return bRes;
}

/** raster map 종류 get */
UINT CMapDataMgr::GetRasterMapKind(BOOL bIsAutoMode, int iRatio)
{
	UINT uiMapKind = 0;
	int iminscale, imaxscale;

	if(!bIsAutoMode)
	{
		switch(m_enCurMapScale)
		{
		case MS_1_2000000 : uiMapKind = 2000000;
			break;
		case MS_1_1000000 : uiMapKind = 1000000;
			break;
		case MS_1_500000 : uiMapKind = 500000;
			break;
		case MS_1_250000 : uiMapKind = 250000;
			break;
		case MS_1_100000 : uiMapKind = 100000;
			break;
		case MS_1_50000 : uiMapKind = 50000;
			break;
		case MS_1_20 : uiMapKind = 20;
			break;
		case MS_1_10 : uiMapKind = 10;
			break;
		case MS_1_1 : uiMapKind = 1;
			break;
		}

		return uiMapKind;
	}

	if(!CMapConfigMgr::GetRasterMapScale(MS_1_2000000, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 2000000;

	if(!CMapConfigMgr::GetRasterMapScale(MS_1_1000000, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 1000000;

	if(!CMapConfigMgr::GetRasterMapScale(MS_1_500000, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 500000;

	if(!CMapConfigMgr::GetRasterMapScale(MS_1_250000, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 250000;

	if(!CMapConfigMgr::GetRasterMapScale(MS_1_100000, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 100000;

	if(!CMapConfigMgr::GetRasterMapScale(MS_1_50000, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 50000;
	/*
	if(!CMapConfigMgr::GetRasterMapScale(MS_1_20, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 20;

	if(!CMapConfigMgr::GetRasterMapScale(MS_1_10, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 10;

	if(!CMapConfigMgr::GetRasterMapScale(MS_1_1, iminscale, imaxscale))
		return uiMapKind;
	if(iRatio >= iminscale && iRatio <= imaxscale)
		uiMapKind = 1;
	*/
	return uiMapKind;
}

EGMapScale CMapDataMgr::GetCurrentMapScale()
{
	return m_enCurMapScale;
}

void CMapDataMgr::SetCurrentMapScale(stUserMapInfo *pUserMapInfo)
{
	if(pUserMapInfo->m_bIsAutoMap == 1)
		return;

	m_enCurMapScale = pUserMapInfo->m_eRasterMapScale;
}

/** 벡터 지도 draw 데이터 리스트 clear */
void CMapDataMgr::InitVectorDrawData()
{
//	for(int i=0; i<3; i++)
//	{
//		for(int j=0; j<m_paVectorDrawData[i].GetSize(); j++)
//			delete m_paVectorDrawData[i].GetAt(j);
//		m_paVectorDrawData[i].RemoveAll();
//	}
}

stRasterMap *CMapDataMgr::FindRasterMap(EGMapScale enScale)
{
	stRasterMap *pItem;
	for(int i=0; i<m_pRasterMap.GetSize(); i++)
	{
		pItem = m_pRasterMap[i];
		if(pItem->m_enMapScale = enScale)
			return pItem;
	}

	return NULL;
}

stSatelliteMapIndex *CMapDataMgr::FindSatelliteMap(EGMapScale enScale)
{
	stSatelliteMapIndex *pItem;
	for(int i=0; i<m_paSatelliteMapIndex.GetSize(); i++)
	{
		pItem = m_paSatelliteMapIndex[i];
		if(pItem->m_enMapScale = enScale)
			return pItem;
	}

	return NULL;
}

void CMapDataMgr::GetVectorMapRect(CStringList &slPath, double &left, double &top, double &right, double &bottom)
{
	BOOL bIsFirst = TRUE;
	CString strPath, strTmp;
	POSITION pos = slPath.GetHeadPosition();
	while(pos)
	{
		strTmp = slPath.GetNext(pos);
		strTmp.MakeUpper();
		if(bIsFirst && (strTmp == "V" || strTmp == "R" || strTmp == "S"))
		{
			bIsFirst = FALSE;
			continue;
		}
		strPath += strTmp;
	}

	strPath.MakeUpper();

	stVectorMap *pItem;
	for(int i=0; i<m_vectormaps.GetSize(); i++)
	{
		pItem = m_vectormaps[i];
		strTmp = EGMapScaleToString(pItem->m_enMapScale);
		strTmp = strTmp + pItem->m_strSubPath1 + pItem->m_strSubPath2;
		strTmp.MakeUpper();
		if(strTmp == strPath)
		{
			left = pItem->m_gpRect.m_R.left;
			top = pItem->m_gpRect.m_R.top;
			right = pItem->m_gpRect.m_R.right;
			bottom = pItem->m_gpRect.m_R.bottom;
			return;
		}
	}

	left = top = right = bottom = 0.0;
}

void CMapDataMgr::GetRasterMapRect(CStringList &slPath, double &left, double &top, double &right, double &bottom)
{
	double left2, top2, right2, bottom2;
	CString strPath, strTmp;
	left2 = bottom2 = 999.0;
	right2 = top2 = 0.0;
	
	BOOL bIsFirst = TRUE;
	POSITION pos = slPath.GetHeadPosition();
	while(pos)
	{
		strTmp = slPath.GetNext(pos);
		strTmp.MakeUpper();
		if(bIsFirst && (strTmp == "V" || strTmp == "R" || strTmp == "S"))
		{
			bIsFirst = FALSE;
			continue;
		}
		strPath = strPath + strTmp + "\\";
	}

	strPath.MakeUpper();

	stRasterMap *pItem;
	for(int i=0; i<m_pRasterMap.GetSize(); i++)
	{
		pItem = m_pRasterMap[i];
		strTmp = EGMapScaleToString(pItem->m_enMapScale);
		if(pItem->m_strSubPath.IsEmpty() || pItem->m_strSubPath == "NULL")
			strTmp = strTmp + "\\";
		else
			strTmp = strTmp + "\\" + pItem->m_strSubPath + "\\";
		strTmp.MakeUpper();
		if(strTmp == strPath)
		{
			if(left2 > pItem->m_gpRect.m_R.left)
				left2 = pItem->m_gpRect.m_R.left;
			if(top2 < pItem->m_gpRect.m_R.top)
				top2 = pItem->m_gpRect.m_R.top;
			if(right2 < pItem->m_gpRect.m_R.right)
				right2 = pItem->m_gpRect.m_R.right;
			if(bottom2 > pItem->m_gpRect.m_R.bottom)
				bottom2 = pItem->m_gpRect.m_R.bottom;
		}
	}

	left = left2;
	top = top2;
	right = right2;
	bottom = bottom2;
}


void CMapDataMgr::GetSatelliteMapRect(CStringList &slPath, double &left, double &top, double &right, double &bottom)
{
	double left2, top2, right2, bottom2;
	CString strPath, strPath2, strRootPath, strSubPath, strFileName, strTmp;
	left2 = bottom2 = 999.0;
	right2 = top2 = 0.0;

	BOOL bIsFirst = TRUE;
	POSITION pos = slPath.GetHeadPosition();
	while(pos)
	{
		strTmp = slPath.GetNext(pos);
		strTmp.MakeUpper();
		if(bIsFirst && (strTmp == "V" || strTmp == "R" || strTmp == "S"))
		{
			bIsFirst = FALSE;
			continue;
		}
		strPath = strPath + strTmp + "\\";
	}

	strPath.MakeUpper();


	stSatelliteMapIndex *pItem;
	for(int i=0; i<m_paSatelliteMapIndex.GetSize(); i++)
	{
		pItem = m_paSatelliteMapIndex[i];
		GetSatelliteMapInfo(pItem, strRootPath, strSubPath, strFileName);
		strTmp = EGMapScaleToString(pItem->m_enMapScale);
		if(strSubPath.IsEmpty() || strSubPath == "NULL")
			strTmp = strTmp + "\\";
		else
			strTmp = strTmp + "\\" + strSubPath + "\\";
		strTmp.MakeUpper();
		if(strTmp == strPath)
		{
			if(left2 > pItem->m_gpRect.m_R.left)
				left2 = pItem->m_gpRect.m_R.left;
			if(top2 < pItem->m_gpRect.m_R.top)
				top2 = pItem->m_gpRect.m_R.top;
			if(right2 < pItem->m_gpRect.m_R.right)
				right2 = pItem->m_gpRect.m_R.right;
			if(bottom2 > pItem->m_gpRect.m_R.bottom)
				bottom2 = pItem->m_gpRect.m_R.bottom;
		}
	}

	left = left2;
	top = top2;
	right = right2;
	bottom = bottom2;
}

CString CMapDataMgr::EGMapScaleToString(EGMapScale scale)
{
	CString strTmp;

	switch(scale)
	{
	case MS_1_50000 :
		strTmp = "5";
		break;
	case MS_1_100000 :
		strTmp = "10";
		break;
	case MS_1_250000 :
		strTmp = "25";
		break;
	case MS_1_500000 :
		strTmp = "50";
		break;
	case MS_1_1000000 :
		strTmp = "100";
		break;
	case MS_1_2000000 :
		strTmp = "200";
		break;
	case MS_1_1 :
		strTmp = "1";
		break;
	case MS_1_10 :
		strTmp = "10";
		break;
	case MS_1_20 :
		strTmp = "20";
		break;
	}

	return strTmp;
}

// 벡터 지도 데이터 레코드 갯수 얻기
int CMapDataMgr::GetVectorMapCount()
{
	return m_vectormaps.GetSize();
}

stVectorMap * CMapDataMgr::GetVectorMapItem(int iIndex)
{
	if(iIndex > m_vectormaps.GetSize())
		return NULL;
	return m_vectormaps[iIndex];
}

void CMapDataMgr::SetSatelliteMapInfo(HANDLE hFMap)
{
	if(hFMap == NULL)
		return;

	m_hFMap = hFMap;
	char *pData = (char *)MapViewOfFile(m_hFMap, FILE_MAP_READ, 0, 0, 0);
	memcpy(&m_iSatelliteMapInfoCnt, pData, sizeof(int));
	if(m_iSatelliteMapInfoCnt <= 0)
		return;

	stSatelliteMapIndex *pItem;
	int iCnt, iIndex, itemsize, iItemIndex, iMapKey;
	CGRectGP rect;
	EGMapScale enMapScale;
	iCnt = 0;
	iIndex = sizeof(int);


	while(TRUE)
	{
		if(iCnt == m_iSatelliteMapInfoCnt)
			break;

		iItemIndex = iIndex;

	// size get
		iIndex += sizeof(int);
	// mapkey get
		memcpy(&iMapKey, pData+iIndex, sizeof(int));
		iIndex += sizeof(int);
	// scale get
		memcpy(&enMapScale, pData+iIndex, sizeof(EGMapScale));
		iIndex += sizeof(EGMapScale);
	// rootpath get
		memcpy(&itemsize, pData+iIndex, sizeof(int));
		iIndex += sizeof(int);
		iIndex += itemsize;
	// filename get
		memcpy(&itemsize, pData+iIndex, sizeof(int));
		iIndex += sizeof(int);
		iIndex += itemsize;
	// rect get
		memcpy(&rect, pData+iIndex, sizeof(CGRectGP));
		iIndex += sizeof(CGRectGP);

		pItem = new stSatelliteMapIndex;
		pItem->m_iIndex = iItemIndex;
		pItem->m_iMapKey = iMapKey;
		pItem->m_enMapScale = enMapScale;
		if(enMapScale == MS_1_10 || enMapScale == MS_1_20)
			pItem->m_gpRect = ConvertUTMtoGP(rect.m_R.left, rect.m_R.top, rect.m_R.right, rect.m_R.bottom);
		else
			pItem->m_gpRect = rect;
		pItem->m_pData = NULL;
		m_paSatelliteMapIndex.Add(pItem);

		iCnt++;
	//	TRACE("\tiCount = %d, ( %d )\n", iCnt, m_iSatelliteMapInfoCnt);
	}

	UnmapViewOfFile(pData);
}

BOOL CMapDataMgr::GetSatelliteMapInfo(stSatelliteMapIndex *pIndex, CString &strRootPath, 
									  CString &strSubPath, CString &strName)
{
	int iCnt = 0;
	int itemsize, iIndex;
	CString strPath2, strFileName;
	CGRectGP gpRect, gpRealRect;
	BOOL bIsExist = FALSE;
	EGMapScale enMapScale;

	if(m_hFMap == NULL)
		return FALSE;

	char *pData = (char *)MapViewOfFile(m_hFMap, FILE_MAP_READ, 0, 0, 0);
	if(pData == NULL)
		return FALSE;

	iIndex = pIndex->m_iIndex;

	// size get
//	memcpy(&iSize, pData+iIndex, sizeof(int));
	iIndex += sizeof(int);
	// mapkey get
//	memcpy(&iMapKey, pData+iIndex, sizeof(int));
	iIndex += sizeof(int);
	// mapscale get
	memcpy(&enMapScale, pData+iIndex, sizeof(EGMapScale));
	iIndex += sizeof(EGMapScale);
	// rootpath get
	memcpy(&itemsize, pData+iIndex, sizeof(int));
	iIndex += sizeof(int);
	strPath2 = CString(pData+iIndex, itemsize);
	iIndex += itemsize;
	// filename get
	memcpy(&itemsize, pData+iIndex, sizeof(int));
	iIndex += sizeof(int);
	strFileName = CString(pData+iIndex, itemsize);
	iIndex += itemsize;
	// rect get
//	memcpy(&rect, pData+iIndex, sizeof(CGRectGP));
	iIndex += sizeof(CGRectGP);

	int iScale;
	switch(enMapScale)
	{
	case MS_1_1 : iScale = 1;
		break;
	case MS_1_10 : iScale = 10;
		break;
	case MS_1_20 : iScale = 20;
		break;
	}

	CString strPath, strTmp;
	strTmp = CMapConfigMgr::GetSatelliteDataPath();
	strPath.Format("%dm\\", iScale);
	strPath = strTmp + strPath + strPath2 + "\\";
	
	//zzzzz test 2008-10-07
	TRACE("==> 위성영상(압축) : %s (default path : %s, 폴더: %s\n", strPath, strTmp, strPath2);

	strRootPath = strPath;
	strSubPath = strPath2;
	strName = strFileName;

	UnmapViewOfFile(pData);

	return FALSE;
}

int CMapDataMgr::GetSatelliteMapIndexCount()
{
	return m_paSatelliteMapIndex.GetSize();
}

stSatelliteMapIndex * CMapDataMgr::GetSatelliteMapIndex(int iIndex)
{
	return m_paSatelliteMapIndex[iIndex];
}


BOOL CMapDataMgr::OnReGeneratorSatelliteMapInfo(int iTotCount)
{
	int iSN, iScale;
	double left, top, right, bottom;
	CString strDeli, strPath, strData, strTmp, strRootPath, strSubPath, strName;
	char* pDeli;
	strDeli = ",";

	CStdioFile stdfile;
	CFile file;
//	CString strSMapInfoFile = CMapConfigMgr::GetMapVersionDataPath() + "SMapInfo(new).idx";
	CString strSMapInfoFile = CMapConfigMgr::GetMapInfoDataPath() + "SMapInfo(new).idx";
	if(!file.Open(strSMapInfoFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		return FALSE;
	}
	void *lpbuf = NULL;

	int iIndex, itemsize;
//	int iCount = 155824;
	int iCount = iTotCount+1;
	char szbuf[1000];
	int			iSize;
	int			iMapKey;
	EGMapScale	enMapScale;
	CString		strFileName;
	CGRectGP	gpRect;
	CGRectGP	gpRealRect;

	strPath = CMapConfigMgr::GetMapInfoDataPath() + "SatelliteMapInfo(new).txt";
	if(!stdfile.Open((LPCTSTR)strPath, CFile::modeRead|CFile::typeText))
	{
		return FALSE;
	}

	BOOL bFirst = TRUE;

	while(stdfile.ReadString(strData))
	{
		// SN
		pDeli = strtok((LPSTR)(LPCTSTR)strData, strDeli);
		strTmp.Format("%s", pDeli);
		iSN = atoi(strTmp);
		// scale
		pDeli = strtok(NULL, strDeli);
		strTmp.Format("%s", pDeli);
		iScale = atoi(strTmp);
		// file path
		pDeli = strtok(NULL, strDeli);
		strRootPath.Format("%s", pDeli);
		if(strRootPath == "NULL")
			strRootPath = "";
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

		iMapKey = iSN;
		switch(iScale)
		{
			case 1 : enMapScale = MS_1_1;
				break;
			case 10 : enMapScale = MS_1_10;
				break;
			case 20 : enMapScale = MS_1_20;
				break;
			default : enMapScale = MS_NOTHING;
				break;
		}

		strFileName = strName;
		gpRect.SetRect(left,top,right,bottom);

		iSize = sizeof(int) + 
				sizeof(int) + 
				sizeof(EGMapScale) + 
				sizeof(int) + 
				strRootPath.GetLength() + 
				sizeof(int) + 
				strFileName.GetLength() + 
				sizeof(CGRectGP);

		iIndex = 0;
		memset(szbuf, ' ', sizeof(szbuf));
		// total count write
		if(bFirst)
		{
			memcpy(szbuf, &iCount, sizeof(int));
			iIndex += sizeof(int);
			iSize += iIndex;
			bFirst = FALSE;
		}

		// size
		memcpy(szbuf+iIndex, &iSize, sizeof(int));
		iIndex += sizeof(int);
		// mapkey
		memcpy(szbuf+iIndex, &iMapKey, sizeof(int));
		iIndex += sizeof(int);
		// mapscale
		memcpy(szbuf+iIndex, &enMapScale, sizeof(EGMapScale));
		iIndex += sizeof(EGMapScale);
		// rootpath string size
		itemsize = strRootPath.GetLength();
		memcpy(szbuf+iIndex, &itemsize, sizeof(int));
		iIndex += sizeof(int);
		// rootpath
		memcpy(szbuf+iIndex, strRootPath, strRootPath.GetLength());
		iIndex += strRootPath.GetLength();
		// filename string size
		itemsize = strFileName.GetLength();
		memcpy(szbuf+iIndex, &itemsize, sizeof(int));
		iIndex += sizeof(int);
		// filename
		memcpy(szbuf+iIndex, strFileName, strFileName.GetLength());
		iIndex += strFileName.GetLength();
		// rect
		memcpy(szbuf+iIndex, &gpRect, sizeof(CGRectGP));
		iIndex += sizeof(CGRectGP);
		file.Write(&szbuf, iSize);

	}
	stdfile.Close();
	file.Close();

	return TRUE;
}

// 바로가기 지도정보 열기
BOOL CMapDataMgr::OpenDirectGoMapInfo()
{
	CFile file;
	CFileFind find;
	CString strPath, strFileName, strFileExt;

	strPath = CMapConfigMgr::GetMapInfoDataPath();
//	strFileExt = "*.afm";
	strFileExt = "directgo_*.afm";
	strFileName = strPath + strFileExt;

	BOOL bRet = find.FindFile(strFileName);
	if(bRet == FALSE)
		return FALSE;

	if(bRet == TRUE)
	{
		while(find.FindNextFile())
		{
			strFileName = find.GetFileName();
			strFileName = strPath + strFileName;

			if(!file.Open(strFileName, CFile::modeRead))
			{
				TRACE("error: CMapDataMgr::OpenDirectGoMapInfo => file open failed!\n");
				return FALSE;
			}

			CArchive ar(&file, CArchive::load);
//			Serialize(FALSE, ar);
			Serialize(2, ar);

			ar.Close();
			file.Close();
		}
	}

	strFileName = find.GetFileName();
	strFileName = strPath + strFileName;

	if(!file.Open(strFileName, CFile::modeRead))
	{
		TRACE("error: CMapDataMgr::OpenDirectGoMapInfo => file open failed!\n");
		return FALSE;
	}

	CArchive ar(&file, CArchive::load);
//	Serialize(FALSE, ar);
	Serialize(2, ar);

	ar.Close();
	file.Close();
	find.Close();

	return TRUE;
}

// 바로가기 지도정보 추가
BOOL CMapDataMgr::AddDirectGoMapInfo(UINT uiID, int iIndex, CString strTime, CString strRootPath, EGMapKind eMapKind, 
									 EGMapScale eMapScale, int iMapRatio, UINT uiMapMode, 
									 CGRectGP gpSelRect, BOOL bIsAutoMap, int iMapType)
{
	BOOL bRes = FALSE;
	CFile file;
	CString strFileName, strTmp;
	BOOL bFound = FALSE;
	int iLow = 0;
	int iHigh = m_paDirectGoMapInfo.GetUpperBound();
	int iMid;
	int iCmp = 0;

	stUserMapInfo *pItem = NULL;
	CString strKey1, strKey2;
	strKey1.Format("%05d", uiID);
	strFileName.Format("directgo_%d.afm", iIndex);

// binary search
	while (iLow <= iHigh)
	{
	 	iMid = (iLow+iHigh) / 2;
		strKey2.Format("%05d", m_paDirectGoMapInfo[iMid]->m_uiID);
		iCmp = strcmp(LPCTSTR(strKey2), LPCTSTR(strKey1));

	// 같은게 있으면
	 	if(iCmp == 0)
		{
			pItem = m_paDirectGoMapInfo[iMid];
			bFound = TRUE;
			break;
		}
		else if(iCmp > 0)
		{
	 		iHigh = iMid - 1;
		}
	 	else
		{
	 		iLow = iMid + 1;
		}
	}

	if(!bFound)
		pItem = new stUserMapInfo;

	pItem->m_uiID = uiID;
	pItem->m_strFileName = strFileName;
	pItem->m_strSaveTime = strTime;
	pItem->m_eMapKind = eMapKind;
	pItem->m_eMapScale = eMapScale;
	pItem->m_eRasterMapScale = m_enCurMapScale;
	pItem->m_iMapRatio = iMapRatio;
	pItem->m_uiMapMode = uiMapMode;
	pItem->m_gpSelRect = gpSelRect;
	pItem->m_bIsAutoMap = bIsAutoMap;
	pItem->m_bIsWorldMap = m_bIsWorldMapOpen;
	pItem->m_uiMapType = (UINT)iMapType;

// 새로 생성한 것이면 삽입
	if(!bFound)
	{
		if(iLow > m_paDirectGoMapInfo.GetUpperBound())
			m_paDirectGoMapInfo.Add(pItem);
		else
			m_paDirectGoMapInfo.InsertAt(iLow, pItem);
	}

	strFileName.Format("directgo_%d.afm", iIndex);
	CString strPath = CMapConfigMgr::GetMapInfoDataPath() + strFileName;

	// file write
	try
	{
		if(!file.Open(strPath, CFile::modeCreate|CFile::modeWrite))
		{
			TRACE("error: CMapDataMgr::AddDirectGoMapInfo => file open failed!\n");
			return FALSE;
		}

		m_pCurUserMapInfo = pItem;
		CArchive ar(&file, CArchive::store);
		Serialize(2, ar);
		ar.Close();
		file.Close();
		bRes = TRUE;
	}
	catch(...)
	{
		delete pItem;
		bRes = FALSE;
	}

	return bRes;
}

int CMapDataMgr::GetDirectGoMapInfoCnt()
{
	return m_paDirectGoMapInfo.GetSize();
}

stUserMapInfo * CMapDataMgr::GetDirectGoMapInfo(int iIndex)
{
	if(iIndex < 0 || iIndex > m_paDirectGoMapInfo.GetSize())
		return NULL;
	return m_paDirectGoMapInfo[iIndex];
}

stUserMapInfo * CMapDataMgr::FindDirectGoMapInfo(UINT uiID)
{
	if(uiID <= 0)
		return NULL;
	stUserMapInfo *pItem;
	for(int i=0; i<m_paDirectGoMapInfo.GetSize(); i++)
	{
		pItem = m_paDirectGoMapInfo[i];
		if(uiID == pItem->m_uiID)
			return m_paDirectGoMapInfo[i];
	}
	return NULL;
}

void CMapDataMgr::SetVectorMapMode(long lVectorMapMode)
{
	m_lVectorMapMode = lVectorMapMode;
}

long CMapDataMgr::GetVectorMapMode()
{
	return m_lVectorMapMode;
}

void CMapDataMgr::AddNameMap2(int iSN, CString strName, double dX, double dY)
{
//	CGPointGP pointGP;
	stNameData2 *pItem = new stNameData2;
	pItem->m_strID.Format("%d", iSN);
	pItem->m_strName = strName;
	pItem->m_gpPoint.m_P.lon = dX;
	pItem->m_gpPoint.m_P.lat = dY;

	m_namedata2.Add(pItem);
}

int CMapDataMgr::GetNameDataCount2()
{
	return m_namedata2.GetSize();
}

stNameData2 * CMapDataMgr::GetNameData2(int iIndex)
{
	if(iIndex < 0 || iIndex > m_namedata2.GetSize())
		return NULL;
	return m_namedata2[iIndex];
}

COLORREF CMapDataMgr::SetHSLColor(COLORREF color, double H, double S, double L, int type)
{
	BYTE r	= (BYTE) ((color & 0x000000FF));
	BYTE g	= (BYTE) ((color & 0x0000FF00) >> 8);
	BYTE b	= (BYTE) ((color & 0x00FF0000) >> 16);					

	double	h=0, s=0, l=0;

// jwpark, 2007-01-23
	if(L < 0.41667)
		L = 0.41667;
	else if(L > 0.958333)
		L = 0.958333;

	// by No1.. 무체색일 경우 밝기만 조정한다.
	if(r == g && r == b && g == b)
	{
		// 밝기 조정
		BYTE AddByte = 0xff;

// jwpark, 2007-01-18
//		if(L < (float) 1.0 && L > (float) 0.4958)
		if(L <= (float) 1.0 && L > (float) 0.4958)
		{
			AddByte = (BYTE) ( (float) ( (255.0-r) / 100.0 * (L * 100.0) ) );
			r = g = b = min((r+AddByte), 255); 
		}
		else if(L < (float) 0.4958)
		{
			AddByte = (BYTE) ( (float) ( r / 100.0 * ((1.0-L) * 100.0) ) );
			r = g = b = max((r-AddByte), 0); 
		}

		color = (unsigned long) (b << 16)  | (unsigned long) (g << 8) | r;
	}
	else
	{
		RGBtoHSL(color, &h, &s, &l);
// jwpark, 2007-01-23
//		switch(type)
//		{
//		case 0: color = HSLtoRGB(h,S,L); break;		// 전체
//		case 1:	color = HSLtoRGB(h,s,L); break; 	// 밝기
//		case 2:	color = HSLtoRGB(h,S,l); break;		// 색도
//		default: break;
//		}
		switch(type)
		{
		case 0: color = HSLtoRGB(h,s,L); break;		// 전체
		case 1:	color = HSLtoRGB(h,s,L); break; 	// 밝기
		case 2:	color = HSLtoRGB(h,S,l); break;		// 색도
		default: break;
		}
	}

	return color;
}

COLORREF CMapDataMgr::SetHSLColor(int mode, COLORREF color, double H, double S, double L)
{
	BYTE r	= (BYTE) ((color & 0x000000FF));
	BYTE g	= (BYTE) ((color & 0x0000FF00) >> 8);
	BYTE b	= (BYTE) ((color & 0x00FF0000) >> 16);					

	double h=0, s=0, l=0;

	if(L < 0.41667)
		L = 0.41667;
	else if(L > 0.958333)
		L = 0.958333;

	// 글자 == 밝기만 조정
	if(mode == 3)
	{
		BYTE AddByte = 0xff;
		if(L <= (float) 1.0 && L > (float) 0.4958)
		{
			AddByte = (BYTE) ( (float) ( (255.0-r) / 100.0 * (L * 100.0) ) );
			r = g = b = min((r+AddByte), 255); 
		}
		else if(L < (float) 0.4958)
		{
			AddByte = (BYTE) ( (float) ( r / 100.0 * ((1.0-L) * 100.0) ) );
			r = g = b = max((r-AddByte), 0); 
		}
		color = (unsigned long) (b << 16)  | (unsigned long) (g << 8) | r;
	}
	// 라인, 채움 == 색도만 조절
	else
	{
		RGBtoHSL(color, &h, &s, &l);
		color = HSLtoRGB(h,S,l);
	}

	return color;
}

void CMapDataMgr::RGBtoHSL(COLORREF rgb, double *H, double *S, double *L)
{
   double delta;
   double r = (double)GetRValue(rgb) / 255;
   double g = (double)GetGValue(rgb) / 255;
   double b = (double)GetBValue(rgb) / 255;
   
   double cmax = max(r, max(g, b));
   double cmin = min(r, min(g, b));
   
   *L = (cmax + cmin) / 2.0;

   if (cmax == cmin) 
   {
      *S = 0;
      *H = 0; // it's really undefined
   } 
   else 
   {
      if (*L < 0.5) 
         *S = (cmax - cmin) / (cmax + cmin);
      else
         *S = (cmax - cmin) / (2.0 - cmax - cmin);
      delta = cmax - cmin;
      if (r == cmax)
          *H = (g - b) / delta;
      else if (g == cmax)
          *H = 2.0 + ( b - r) / delta;
      else
          *H = 4.0 + (r - g) / delta;
      *H /= 6.0;
      if(*H < 0.0)
		  *H += 1;
   }
}

COLORREF CMapDataMgr::HSLtoRGB(double H, double S, double L)
{
#ifdef FROM_GRgbHsl	// from CommonMathb
	double	r, g, b;
	double	m1, m2;

	if (S == 0) 
	{
		r = g = b = L;
	}
	else 
	{
		if (L <= 0.5) 
		{
			m2 = L * (1.0 + S);
		}
		else 
		{
			m2 = L + S - L * S;
		}

		m1 = 2.0 * L - m2;
		r = HuetoRGB(m1, m2, H + 1.0 / 3.0);
		g = HuetoRGB(m1, m2, H);
		b = HuetoRGB(m1, m2, H - 1.0 / 3.0);
	}

	return RGB((BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));

#else	// from CommonMathb.cpp

  double r,g,b;
  double m1, m2;

  if (S == 0) 
  {
     r = g = b = L;
  } 
  else 
  {
     if (L <= 0.5)
        m2 = L * (1.0 + S);
     else
        m2 = L + S - L * S;
     m1 = 2.0 * L - m2;
     r = HuetoRGB(m1, m2, H + 1.0 / 3.0);
     g = HuetoRGB(m1, m2, H);
     b = HuetoRGB(m1, m2, H - 1.0 / 3.0);
  }
  return RGB((BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));

#endif // FROM_GRgbHsl
}

double CMapDataMgr::HuetoRGB(double m1, double m2, double h)
{
   if (h < 0) h += 1.0;
   if (h > 1) h -= 1.0;
   if (6.0 * h < 1 )
      return ( m1 + (m2 - m1) * h * 6.0);
   if (2.0 * h < 1)
      return m2;
   if (3.0 * h < 2.0)
      return (m1 + (m2 - m1) * ((2.0 / 3.0) - h) * 6.0);
   return m1;
}

// type : 1-선,2-채움, 3-글자, default:전체
// HSLSet : 0-전체, 1-밝기, 2-색도
void CMapDataMgr::SetChromView(int iHSLSet, int iType, double H, double S, double L)
{
	switch(iType)
	{
	case 1 : 
		SetChromLineColor(iHSLSet, H, S, L);
		break;
	case 2 : 
		SetChromFillColor(iHSLSet, H, S, L);
		break;
	case 3 : 
		SetChromFontColor(iHSLSet, H, S, L);
		break;
	default : 
		SetChromLineColor(iHSLSet, H, S, L);
		SetChromFillColor(iHSLSet, H, S, L);
		SetChromFontColor(iHSLSet, H, S, L);
		break;
	}
}

void CMapDataMgr::SetChromLineColor(int iHSLSet, double H, double S, double L)
{
	CMapPriority *pPriority;
	CPolylinePriority *pLine;
	COLORREF color1, color2;

	for(int i=0; i<m_mappriority.GetSize(); i++)
	{
		pPriority = m_mappriority[i];
		if(pPriority->m_strFileName.Mid(0,1) != "L")
			continue;

		pLine = (CPolylinePriority *)pPriority;
//		color1 = SetHSLColor(pLine->m_crLineColor, H, S, L, iHSLSet);
//		color2 = SetHSLColor(pLine->m_crLineColor2, H, S, L, iHSLSet);
		color1 = SetHSLColor(1, pLine->m_crLineColor, H, S, L);
		color2 = SetHSLColor(1, pLine->m_crLineColor2, H, S, L);
		pLine->m_crLineColor = color1;
		pLine->m_crLineColor2 = color2;
	}
}

void CMapDataMgr::SetChromFillColor(int iHSLSet, double H, double S, double L)
{
	CMapPriority *pPriority;
	CPointPriority* pPoint;
	CPolygonPriority *pPolygon;
	COLORREF color1, color2;

	for(int i=0; i<m_mappriority.GetSize(); i++)
	{
		pPriority = m_mappriority[i];
		if(pPriority->m_strFileName.Mid(0,1) == "L")
			continue;

		if(pPriority->m_strFileName.Mid(0,1) == "P")
		{
			pPoint = (CPointPriority *)pPriority;
//			color1 = SetHSLColor(pPoint->m_crFillColor, H, S, L, iHSLSet);
			color1 = SetHSLColor(2, pPoint->m_crFillColor, H, S, L);
			pPoint->m_crFillColor = color1;
		}
		else if(pPriority->m_strFileName.Mid(0,1) == "A")
		{
			pPolygon = (CPolygonPriority *)pPriority;
//			color1 = SetHSLColor(pPolygon->m_crFillColor, H, S, L, iHSLSet);
//			color2 = SetHSLColor(pPolygon->m_crLineColor, H, S, L, iHSLSet);
			color1 = SetHSLColor(2, pPolygon->m_crFillColor, H, S, L);
			color2 = SetHSLColor(2, pPolygon->m_crLineColor, H, S, L);
			pPolygon->m_crFillColor = color1;
			pPolygon->m_crLineColor = color2;
		}
	}
}

void CMapDataMgr::SetChromFontColor(int iHSLSet, double H, double S, double L)
{
	stTextPriority* pTextPriority;
	COLORREF color;
	for(int i=0; i<m_pTextPriority.GetSize(); i++)
	{
		pTextPriority = m_pTextPriority[i];
//		color = SetHSLColor(pTextPriority->m_clFontColor, H, S, L, iHSLSet);
		color = SetHSLColor(3, pTextPriority->m_clFontColor, H, S, L);
		pTextPriority->m_clFontColor = color;
	}
}

void CMapDataMgr::set_flag_raster_view( s_flag_raster_view flag_view )
{

	m_flag_raster_view = flag_view;
		
	if( m_flag_raster_view.visible_5 == FALSE &&
		m_flag_raster_view.visible_10 == FALSE &&
		m_flag_raster_view.visible_25 == FALSE &&
		m_flag_raster_view.visible_50 == FALSE &&
		m_flag_raster_view.visible_100 == FALSE &&
		m_flag_raster_view.visible_200 == FALSE
		)
		m_flag_raster_view.visible = FALSE;
 	else
 		m_flag_raster_view.visible = TRUE;	 
}

s_flag_raster_view CMapDataMgr::get_flag_raster_view()
{
	return m_flag_raster_view;
}

void CMapDataMgr::set_flag_satellite_view( s_flag_satellite_view flag_view )
{
	m_flag_satellite_view = flag_view;
	
	if( m_flag_satellite_view.visible_1m == FALSE &&
		m_flag_satellite_view.visible_1m == FALSE &&
		m_flag_satellite_view.visible_1m == FALSE &&
		m_flag_satellite_view.visible_1m == FALSE
		)
		m_flag_satellite_view.visible = FALSE;
	else
		m_flag_satellite_view.visible = TRUE;	
}

s_flag_satellite_view CMapDataMgr::get_flag_satellite_view()
{
	return m_flag_satellite_view;
}


std::vector< S_PATTERN_INFO > mvec_pattern_info;
static S_PATTERN_INFO m_pattern_info_tmp;
void CMapDataMgr::add_pattern_info( CString & nm_pattern, CString & nm_file )
{
	m_pattern_info_tmp.nm_pattern = nm_pattern;
	m_pattern_info_tmp.nm_file = nm_file;
	mvec_pattern_info.push_back( m_pattern_info_tmp );
}

void CMapDataMgr::get_pattern_info( itr_pattern_info &itr_begin, itr_pattern_info &itr_end )
{ 
	itr_begin = mvec_pattern_info.begin();
	itr_end = mvec_pattern_info.end();
}


std::vector< S_SYMBOL_INFO > mvec_symbol_info;
static S_SYMBOL_INFO m_symbol_info_tmp;
void CMapDataMgr::add_symbol_info( CString & nm_symbol, CString & nm_file )
{
	m_symbol_info_tmp.nm_symbol = nm_symbol;
	m_symbol_info_tmp.nm_file = nm_file;
	mvec_symbol_info.push_back( m_symbol_info_tmp );
}

void CMapDataMgr::get_symbol_info( itr_symbol_info &itr_begin, itr_symbol_info &itr_end )
{ 
	itr_begin = mvec_symbol_info.begin();
	itr_end = mvec_symbol_info.end();
}
void CMapDataMgr::delete_vector_layer_info( )
{
	mmap_layer_info_5.clear();
	mmap_layer_info_10.clear();
	mmap_layer_info_25.clear();
	mmap_layer_info_50.clear();
	mmap_layer_info_100.clear();
	mmap_layer_info_200.clear();	
}

void CMapDataMgr::add_vector_layer_info( S_LAYER_INFO & layer_info, short scale )
{
	switch( scale )
	{ 
	case 5 : 
		mmap_layer_info_5[layer_info.nm_layer] = layer_info;
		break; 
	case 10 : 
		mmap_layer_info_10[layer_info.nm_layer] = layer_info;
		break; 
	case 25 : 
		mmap_layer_info_25[layer_info.nm_layer] = layer_info;
		break; 
	case 50 : 
		mmap_layer_info_50[layer_info.nm_layer] = layer_info;
		break; 
	case 100 : 
		mmap_layer_info_100[layer_info.nm_layer] = layer_info;
		break; 
	case 200 : 
		mmap_layer_info_200[layer_info.nm_layer] = layer_info;
		break; 
	default: break;
	}	 
}
void CMapDataMgr::add_order_info( int order, CString nm_file, short scale )
{	  
	switch( scale )
	{ 
	case 5 : 
		mvec_order2_5.push_back( nm_file );
		break; 
	case 10 : 
		mvec_order2_10.push_back( nm_file );
		break; 
	case 25 : 
		mvec_order2_25.push_back( nm_file );
		break; 
	case 50 : 
		mvec_order2_50.push_back( nm_file );
		break; 
	case 100 : 
		mvec_order2_100.push_back( nm_file );
		break; 
	case 200 : 
		mvec_order2_200.push_back( nm_file );
		break; 
	default: break;
	}

 	switch( scale )
	{ 
	case 5 : 
		mmap_order2_5[order] = nm_file;break; 
	case 10 : 
		mmap_order2_10[order] = nm_file;break; 
	case 25 : 
		mmap_order2_25[order] = nm_file;break; 
	case 50 : 
		mmap_order2_50[order] = nm_file;break; 
	case 100 : 
		mmap_order2_100[order] = nm_file;break; 
	case 200 : 
		mmap_order2_200[order] = nm_file;break; 
	default: break;
	}

	switch( scale )
	{ 
	case 5 : 
		mmap_order_5[nm_file] = order;break; 
	case 10 : 
		mmap_order_10[nm_file] = order;break; 
	case 25 : 
		mmap_order_25[nm_file] = order;break; 
	case 50 : 
		mmap_order_50[nm_file] = order;break; 
	case 100 : 
		mmap_order_100[nm_file] = order;break; 
	case 200 : 
		mmap_order_200[nm_file] = order;break; 
	default: break;
	}
}

BOOL CMapDataMgr::get_vector_lyaer_info( CString nm_layer, short scale, S_LAYER_INFO & layer_info )
{ 	
	try
	{
		type_itr_layer_info itr_begin;
		type_itr_layer_info itr_end;		

		switch( scale )
		{ 
		case 1 : 
			{
				itr_begin = mmap_layer_info_5.find( nm_layer );
				itr_end = mmap_layer_info_5.end();
				break; 
			}
		case 2 : 		
			{
				itr_begin = mmap_layer_info_10.find( nm_layer );
				itr_end = mmap_layer_info_10.end();
				break; 
			}
		case 3 : 
			{
				itr_begin = mmap_layer_info_25.find( nm_layer );
				itr_end = mmap_layer_info_25.end();
				break; 
			}
		case 4 : 
			{
				itr_begin = mmap_layer_info_50.find( nm_layer );
				itr_end = mmap_layer_info_50.end();
				break; 
			}
		case 5 : 
			{
				itr_begin = mmap_layer_info_100.find( nm_layer );
				itr_end = mmap_layer_info_100.end();
				break; 
			}
		case 6 : 
			{
				itr_begin = mmap_layer_info_200.find( nm_layer );
				itr_end = mmap_layer_info_200.end();
				break; 
			}
		default: break;
		}		

		if( itr_begin != itr_end )
		{
			layer_info = itr_begin->second;
			return TRUE;
		}
		else
			return FALSE;

	}
	catch (CException* e)
	{
		
	}	
}
/*
BOOL CMapDataMgr::get_order_info( short scale,  std::vector<CString>::iterator  &itr_begin,std::vector<CString>::iterator  &itr_end )
{ 	
	
	switch( scale )
	{ 	
	case 1 : 
		{ 
			itr_begin = mvec_order_5.begin();
			itr_end = mvec_order_5.end();
			break; 
		}
	case 2 :
		{
			itr_begin = mvec_order_10.begin();
			itr_end = mvec_order_10.end();
			break; 
		}
	case 3 :
		{			
			itr_begin = mvec_order_25.begin();
			itr_end = mvec_order_25.end();
			break; 
		}
	case 4 :
		{
			itr_begin = mvec_order_50.begin();
			itr_end = mvec_order_50.end();
			break; 
		}
	case 5 :
		{
			itr_begin = mvec_order_100.begin();
			itr_end = mvec_order_100.end();
			break; 
		}
	case 6 :
		{
			itr_begin = mvec_order_200.begin();
			itr_end = mvec_order_200.end();
			break; 
		}
	default: break;
	}	
	return TRUE;
}*/ 

int CMapDataMgr::get_cnt_order( short scale )
{
	int cnt_order = 0;

	switch( scale )
	{ 	
	case 1 : cnt_order = mmap_order_5.size(); break; 		
	case 2 :cnt_order = mmap_order_10.size(); break; 		
	case 3 :cnt_order = mmap_order_25.size(); break; 		
	case 4 :cnt_order = mmap_order_50.size(); break; 		
	case 5 :cnt_order = mmap_order_100.size(); break; 		
	case 6 :cnt_order = mmap_order_200.size(); break; 		
	default: break;
	}	
	return cnt_order;
}

BOOL CMapDataMgr::get_order_info( short scale,  itr_order2 &itr_begin, itr_order2 &itr_end )
{ 	 
	switch( scale )
	{ 	
	case 1 : 
		{ 
			itr_begin = mvec_order2_5.begin(); int cnt = mvec_order2_5.size();
			itr_end = mvec_order2_5.end();
			break; 
		}
	case 2 :
		{
			itr_begin = mvec_order2_10.begin();
			itr_end = mvec_order2_10.end();
			break; 
		}
	case 3 :
		{			
			itr_begin = mvec_order2_25.begin();
			itr_end = mvec_order2_25.end();
			break; 
		}
	case 4 :
		{
			itr_begin = mvec_order2_50.begin();
			itr_end = mvec_order2_50.end();
			break; 
		}
	case 5 :
		{
			itr_begin = mvec_order2_100.begin();
			itr_end = mvec_order2_100.end();
			break; 
		}
	case 6 :
		{
			itr_begin = mvec_order2_200.begin();
			itr_end = mvec_order2_200.end();
			break; 
		}
	default: break;
	}	
	return TRUE;
} 

short CMapDataMgr::get_order( short scale, CString nm_file )
{
	short order = 0; mmap_order_5[ nm_file ];
	switch( scale )
	{ 	
 	case 1 :  order = mmap_order_5[ nm_file ];break; 
 	case 2 :  order = mmap_order_10[ nm_file ];break; 
 	case 3 :  order = mmap_order_25[ nm_file ];break; 
 	case 4 :  order = mmap_order_50[ nm_file ];break; 
 	case 5 :  order = mmap_order_100[ nm_file ];break; 
 	case 6 :  order = mmap_order_200[ nm_file ];break; 
	default: break;
	} 
	return order;
}

void CMapDataMgr::set_visibie_vector_layer( int order, int scale, BOOL visible )
{
	CString nm_file = "";
	switch( scale )
	{ 	
	case 1 :  nm_file = mmap_order2_5[ order ];break; 
	case 2 :  nm_file = mmap_order2_10[ order ];break; 
	case 3 :  nm_file = mmap_order2_25[ order ];break; 
	case 4 :  nm_file = mmap_order2_50[ order ];break; 
	case 5 :  nm_file = mmap_order2_100[ order ];break; 
	case 6 :  nm_file = mmap_order2_200[ order ];break; 
	default: break;
	} 


	type_itr_layer_info itr_begin;
	type_itr_layer_info itr_end;

	switch( scale )
	{ 
	case 1 : 
		{
			itr_begin = mmap_layer_info_5.find( nm_file );
			itr_end = mmap_layer_info_5.end();
			break; 
		}
	case 2 : 		
		{
			itr_begin = mmap_layer_info_10.find( nm_file );
			itr_end = mmap_layer_info_10.end();
			break; 
		}
	case 3 : 
		{
			itr_begin = mmap_layer_info_25.find( nm_file );
			itr_end = mmap_layer_info_25.end();
			break; 
		}
	case 4 : 
		{
			itr_begin = mmap_layer_info_50.find( nm_file );
			itr_end = mmap_layer_info_50.end();
			break; 
		}
	case 5 : 
		{
			itr_begin = mmap_layer_info_100.find( nm_file );
			itr_end = mmap_layer_info_100.end();
			break; 
		}
	case 6 : 
		{
			itr_begin = mmap_layer_info_200.find( nm_file );
			itr_end = mmap_layer_info_200.end();
			break; 
		}
	default: break;
	}

	if( itr_begin != itr_end )
	{
		itr_begin->second.visible = visible;
	}
}

BOOL CMapDataMgr::is_edit_layer_info( int scale, CString nm_file )
{
	type_itr_layer_info itr_begin;
	type_itr_layer_info itr_end;

	switch( scale )
	{ 
	case 1 : 
		{
			itr_begin = mmap_layer_info_5.find( nm_file );
			itr_end = mmap_layer_info_5.end();
			break; 
		}
	case 2 : 		
		{
			itr_begin = mmap_layer_info_10.find( nm_file );
			itr_end = mmap_layer_info_10.end();
			break; 
		}
	case 3 : 
		{
			itr_begin = mmap_layer_info_25.find( nm_file );
			itr_end = mmap_layer_info_25.end();
			break; 
		}
	case 4 : 
		{
			itr_begin = mmap_layer_info_50.find( nm_file );
			itr_end = mmap_layer_info_50.end();
			break; 
		}
	case 5 : 
		{
			itr_begin = mmap_layer_info_100.find( nm_file );
			itr_end = mmap_layer_info_100.end();
			break; 
		}
	case 6 : 
		{
			itr_begin = mmap_layer_info_200.find( nm_file );
			itr_end = mmap_layer_info_200.end();
			break; 
		}
	default: break;
	}

	if( itr_begin != itr_end )
	{
		return itr_begin->second.is_edit;
	}
	return FALSE;
}


void CMapDataMgr::done_edit_layer_info( int scale, CString nm_file )
{
	type_itr_layer_info itr_begin;
	type_itr_layer_info itr_end;

	switch( scale )
	{ 
	case 1 : 
		{
			itr_begin = mmap_layer_info_5.find( nm_file );
			itr_end = mmap_layer_info_5.end();
			break; 
		}
	case 2 : 		
		{
			itr_begin = mmap_layer_info_10.find( nm_file );
			itr_end = mmap_layer_info_10.end();
			break; 
		}
	case 3 : 
		{
			itr_begin = mmap_layer_info_25.find( nm_file );
			itr_end = mmap_layer_info_25.end();
			break; 
		}
	case 4 : 
		{
			itr_begin = mmap_layer_info_50.find( nm_file );
			itr_end = mmap_layer_info_50.end();
			break; 
		}
	case 5 : 
		{
			itr_begin = mmap_layer_info_100.find( nm_file );
			itr_end = mmap_layer_info_100.end();
			break; 
		}
	case 6 : 
		{
			itr_begin = mmap_layer_info_200.find( nm_file );
			itr_end = mmap_layer_info_200.end();
			break; 
		}
	default: break;
	}

	if( itr_begin != itr_end )
	{
		itr_begin->second.is_edit = FALSE;
	} 
}



BOOL CMapDataMgr::get_visibie_vector_layer( int scale, CString nm_file )
{ 
	type_itr_layer_info itr_begin;
	type_itr_layer_info itr_end;

	switch( scale )
	{ 
	case 1 : 
		{
			itr_begin = mmap_layer_info_5.find( nm_file );
			itr_end = mmap_layer_info_5.end();
			break; 
		}
	case 2 : 		
		{
			itr_begin = mmap_layer_info_10.find( nm_file );
			itr_end = mmap_layer_info_10.end();
			break; 
		}
	case 3 : 
		{
			itr_begin = mmap_layer_info_25.find( nm_file );
			itr_end = mmap_layer_info_25.end();
			break; 
		}
	case 4 : 
		{
			itr_begin = mmap_layer_info_50.find( nm_file );
			itr_end = mmap_layer_info_50.end();
			break; 
		}
	case 5 : 
		{
			itr_begin = mmap_layer_info_100.find( nm_file );
			itr_end = mmap_layer_info_100.end();
			break; 
		}
	case 6 : 
		{
			itr_begin = mmap_layer_info_200.find( nm_file );
			itr_end = mmap_layer_info_200.end();
			break; 
		}
	default: break;
	}

	if( itr_begin != itr_end )
	{
		return itr_begin->second.visible;
	}
	return FALSE;
}




CString CMapDataMgr::get_nm_file( int order, int scale )
{
	CString nm_file = "";
	switch( scale )
	{ 	
	case 1 :  nm_file = mmap_order2_5[ order ];break; 
	case 2 :  nm_file = mmap_order2_10[ order ];break; 
	case 3 :  nm_file = mmap_order2_25[ order ];break; 
	case 4 :  nm_file = mmap_order2_50[ order ];break; 
	case 5 :  nm_file = mmap_order2_100[ order ];break; 
	case 6 :  nm_file = mmap_order2_200[ order ];break; 
	default: break;
	} 

 
	return nm_file;
}
 
void CMapDataMgr::update_lyaer_info( CString nm_layer, short scale, S_LAYER_INFO & layer_info )
{ 	
	type_itr_layer_info itr_begin;
	type_itr_layer_info itr_end;

	switch( scale )
	{ 
	case 1 : 
		{			
			mmap_layer_info_5[ nm_layer ] = layer_info;			break; 
		}
	case 2 : 		
		{
			mmap_layer_info_10[ nm_layer ] = layer_info;		break; 
		}
	case 3 : 
		{
			mmap_layer_info_25[ nm_layer ] = layer_info;		break; 
		}
	case 4 : 
		{
			mmap_layer_info_50[ nm_layer ] = layer_info;		break; 
		}
	case 5 : 
		{
			mmap_layer_info_100[ nm_layer ] = layer_info;		break; 
		}
	case 6 : 
		{
			mmap_layer_info_200[ nm_layer ] = layer_info;		break; 
		}
	default: break;
	}	
	
 	return;
	


	HRESULT hr_success; 	 
	
	CCommand<CNoAccessor,CNoRowset> cmd_update;
	CDataSource	datasource;

	hr_success = datasource.OpenFromInitializationString
		(OLESTR("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=C:\\AFC4I\\bin\\create_info.mdb;"));
	if( FAILED( hr_success ) )return;	 		
	
	CSession s_cmd;
	hr_success = s_cmd.Open(datasource); 
 	if( FAILED( hr_success ) )return;	 		
	
	CString scale_min; scale_min.Format("%d",layer_info.scale_min);
	CString scale_max; scale_max.Format("%d",layer_info.scale_max);
	CString view; view.Format("%d",layer_info.visible);
	
 	CString polygon_is_pattern; 

	if( layer_info.polygon_is_pattern == TRUE )
		polygon_is_pattern.Format("%d",layer_info.polygon_is_pattern); 
	else
		polygon_is_pattern.Format("%d",FALSE); 

	CString order; order.Format("%d",layer_info.order);

	CString Q_LAYER_UPDATE =  "UPDATE LAYER SET ";
	
	Q_LAYER_UPDATE = Q_LAYER_UPDATE + 		
		"ID_LAYER = '"	+ layer_info.id_layer + "'," +
		"NM_LAYER = '"	+ layer_info.nm_layer + "'," +
		"NM_LAYER_KOR = '"	+ layer_info.nm_layer_kor + "'," +
		"NM_SHP = '"	+ layer_info.nm_file + "'," +
		"TYPE_GEO = '"	+ layer_info.type_geo + "'," +
		
		"TYPE_MAP = '"	+ layer_info.type_map + "'," +
		"SCALE = '"	+ layer_info.scale + "'," +
		"RANGE_MIN = "	+ scale_min + "," +
		"RANGE_MAX = "	+ scale_max + "," +
		"VISIBLE = "	+ view + "," +
		
		"POINT_SIZE = '"	+ layer_info.point_size + "'," +
		"POINT_IMAGE = '"	+ layer_info.point_image + "'," +
		"POLYLINE_WIDTH = '"	+ layer_info.polyline_width + "'," +
		"POLYLINE_CLR = '"	+ layer_info.polyline_clr + "'," +
		"POLYLINE_TYPE = '"	+ layer_info.polyline_type + "'," +
		
		"POLYLINE_ETC = '"	+ ""/*layer_info.polyline_etc*/ + "'," +
		"POLYGON_WIDTH = '"	+ layer_info.polygon_width + "'," +
		"POLYGON_TYPE = '"	+ layer_info.polygon_type + "'," +
		"POLYGON_CLR_OUT = '"	+ layer_info.polygon_clr_out + "'," +
		"POLYGON_CLR_IN = '"	+ layer_info.polygon_clr_in + "'," +
		
		"POLYGON_PATTERN_IMAGE = '"	+ layer_info.polygon_pattern_image + "'," +
		"POLYGON_IS_PATTERN = "	+ polygon_is_pattern + "," +
		"POLYGON_IS_IN = '"	+ layer_info.polygon_is_in + "'" +
		
		" WHERE ( ORDER = " + order + ")";  
	
	hr_success = cmd_update.Open( s_cmd, Q_LAYER_UPDATE );
	
	cmd_update.Close();
	s_cmd.Close();
	datasource.Close(); 
}
	
void CMapDataMgr::delete_user_map_info_on_mdb( CString nm_user_map )
{	
	itr_user_map_info itr_begin;
	itr_user_map_info itr_end;
	get_user_map_info( itr_begin, itr_end );
	
	for( ; itr_begin != itr_end; ++itr_begin )
	{
		if( nm_user_map == (*itr_begin).nm_usermap )
		{
			mvec_user_map_info.erase( itr_begin ); 
			
			HRESULT hr_success; 	 
			
			CCommand<CNoAccessor,CNoRowset> cmd_update;
			CDataSource	datasource;
			
			hr_success = datasource.OpenFromInitializationString
				(OLESTR("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=C:\\AFC4I\\bin\\create_info.mdb;"));
			if( FAILED( hr_success ) )return;	 		
			
			CSession s_cmd;
			hr_success = s_cmd.Open(datasource); 
			if( FAILED( hr_success ) )return;	 				
			
			
			CString Q_USER_MAP_DELETE =  "DELETE FROM USER_MAP WHERE ( NM_USERMAP = '";
			Q_USER_MAP_DELETE = Q_USER_MAP_DELETE + nm_user_map + "')";
						
			hr_success = cmd_update.Open( s_cmd, Q_USER_MAP_DELETE );
			
			Q_USER_MAP_DELETE =  "DELETE FROM USER_MAP_ATTRI WHERE ( NM_USERMAP = '";
			Q_USER_MAP_DELETE = Q_USER_MAP_DELETE + nm_user_map + "')";
						
			hr_success = cmd_update.Open( s_cmd, Q_USER_MAP_DELETE );


			cmd_update.Close();
			s_cmd.Close();
			datasource.Close(); 
			return;
		}		
	}	
}
static CString m_nm_user_map;
void CMapDataMgr::save_current_map_info_on_mdb( CString nm_user_map )
{	
	m_nm_user_map = nm_user_map;
	HRESULT hr_success; 	 
	
	CCommand<CNoAccessor,CNoRowset> cmd_update;
	CDataSource	datasource;

	hr_success = datasource.OpenFromInitializationString
		(OLESTR("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=C:\\AFC4I\\bin\\create_info.mdb;"));
	if( FAILED( hr_success ) )return;	 		
	
	CSession s_cmd;
	hr_success = s_cmd.Open(datasource); 
 	if( FAILED( hr_success ) )return;
 
	CString Q_LAYER_UPDATE;  

	Q_LAYER_UPDATE = "INSERT INTO USER_MAP ( IS_DEFAULT_MAP, NM_USERMAP, IS_AUTO_MAP, IS_WORLD_MAP, MODE, SCALE, RATIO, TYPE, COORD_LEFT, COORD_TOP, COORD_RIGHT, COORD_BOTTOM, COORD_CENTER, ZOOM_FACTOR ) VALUES ( FALSE, '";
	

	CString bIsAutoMode; bIsAutoMode.Format("%d", m_current_map_info.bIsAutoMode);
	CString bIsWorldMapOpen;bIsWorldMapOpen.Format("%d", m_current_map_info.bIsWorldMapOpen);
	CString lMapMode;lMapMode.Format("%d", m_current_map_info.lMapMode);
	CString lMapScale;lMapScale.Format("%d", m_current_map_info.lMapScale);
	CString iMapRatio;iMapRatio.Format("%f", m_current_map_info.iMapRatio);
	CString iMapType;iMapType.Format("%d", m_current_map_info.iMapType);

	CString dleft;dleft.Format("%f", m_current_map_info.dleft);
	CString dtop;dtop.Format("%f", m_current_map_info.dtop);
	CString dright;dright.Format("%f", m_current_map_info.dright);
	CString dbottom;dbottom.Format("%f", m_current_map_info.dbottom);

	CString dZoomFactor; dZoomFactor.Format("%f", m_current_map_info.dZoomFactor);
	CString coord_center = m_current_map_info.coord_center;

	Q_LAYER_UPDATE = Q_LAYER_UPDATE + 
		nm_user_map + "'," + 
		bIsAutoMode + "," +	
		bIsWorldMapOpen + "," +		
		lMapMode + "," +
		lMapScale + "," +
		iMapRatio + "," +
		iMapType + "," +	
		dleft + "," +
		dtop + "," +
		dright + "," +
		dbottom + ",'" +
		coord_center + "'," +
		dZoomFactor + " )";	 
	TRACE( Q_LAYER_UPDATE );	
	hr_success = cmd_update.Open( s_cmd, Q_LAYER_UPDATE );
 	if( FAILED( hr_success ) )
	{
		AfxMessageBox("");
		return;	
	}

	cmd_update.Close();
	s_cmd.Close();
	datasource.Close(); 

	m_current_map_info.nm_usermap = nm_user_map;
	add_user_map_info( m_current_map_info );
		
	{ 	
		type_itr_layer_info itr_begin;
		type_itr_layer_info itr_end;

		itr_begin = mmap_layer_info_5.begin();
		itr_end = mmap_layer_info_5.end();
			
		for( itr_begin; itr_begin != itr_end; ++itr_begin )
		{			
			save_user_map_attri_on_mdb( (S_LAYER_INFO)itr_begin->second );
		}
 
		itr_begin = mmap_layer_info_10.begin();
		itr_end = mmap_layer_info_10.end();
			
		for( itr_begin; itr_begin != itr_end; ++itr_begin )
		{			
			save_user_map_attri_on_mdb( (S_LAYER_INFO)itr_begin->second );
		}
 		itr_begin = mmap_layer_info_25.begin();
		itr_end = mmap_layer_info_25.end();
			
		for( itr_begin; itr_begin != itr_end; ++itr_begin )
		{			
			save_user_map_attri_on_mdb( (S_LAYER_INFO)itr_begin->second );
		}
 		itr_begin = mmap_layer_info_50.begin();
		itr_end = mmap_layer_info_50.end();
			
		for( itr_begin; itr_begin != itr_end; ++itr_begin )
		{			
			save_user_map_attri_on_mdb( (S_LAYER_INFO)itr_begin->second );
		}
 
 		itr_begin = mmap_layer_info_100.begin();
		itr_end = mmap_layer_info_100.end();
			
		for( itr_begin; itr_begin != itr_end; ++itr_begin )
		{			
			save_user_map_attri_on_mdb( (S_LAYER_INFO)itr_begin->second );
		}
 		itr_begin = mmap_layer_info_200.begin();
		itr_end = mmap_layer_info_200.end();
			
		for( itr_begin; itr_begin != itr_end; ++itr_begin )
		{			
			save_user_map_attri_on_mdb( (S_LAYER_INFO)itr_begin->second );
		} 
	}	
	save_user_map_attri_font_on_mdb(nm_user_map);
	MessageBox(NULL, _T("저장이 완료 되었습니다."), _T("확인"), MB_OK);
}

//사용자 지도 저장 시 폰트 저장
//추가 20110803
void CMapDataMgr::save_user_map_attri_font_on_mdb(CString nm_user_map)
{
	HRESULT hr_success; 	 		
	CCommand<CNoAccessor,CNoRowset> cmd_update;
	CDataSource	datasource;
	hr_success = datasource.OpenFromInitializationString
		(OLESTR("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=C:\\AFC4I\\bin\\create_info.mdb;"));
	if( FAILED( hr_success ) )return;		
	

	CSession s_cmd;
	hr_success = s_cmd.Open(datasource);
	if( FAILED( hr_success ) )return;

	CString Q_FONT_UPDATE;
 	Q_FONT_UPDATE = "INSERT INTO USER_MAP_ATTRI_FONT ( NM_USERMAP, fHeight, fWidth, fEscapement, fOrientation, fWeight, fItalic, fUnderline, fStrikeOut, fCharSet, fOutPrecision, fClipPrecision, fQuality, fPitchAndFamily, fFaceName) VALUES (";
	//Q_FONT_UPDATE = "INSERT INTO USER_MAP_ATTRI_FONT ( NM_USERMAP, fHeight, fWidth, fEscapement ) VALUES (";
 
 	CString fHeight;fHeight.Format("%d", m_label_font.lfHeight); 
 	CString fWidth;fWidth.Format("%d", m_label_font.lfWidth);
 	CString fEscapement;fEscapement.Format("%d", m_label_font.lfEscapement);
 	CString fOrientation;fOrientation.Format("%d", m_label_font.lfOrientation);
 	CString fWeight;fWeight.Format("%d", m_label_font.lfWeight);
 	CString fItalic;fItalic.Format("%d", m_label_font.lfItalic);
 	CString fUnderline;fUnderline.Format("%d", m_label_font.lfUnderline);
 	CString fStrikeOut;fStrikeOut.Format("%d", m_label_font.lfStrikeOut);
 	CString fCharSet;fCharSet.Format("%d", m_label_font.lfCharSet);
 	CString fOutPrecision;fOutPrecision.Format("%d", m_label_font.lfOutPrecision);
 	CString fClipPrecision;fClipPrecision.Format("%d", m_label_font.lfClipPrecision);
 	CString fQuality;fQuality.Format("%d", m_label_font.lfQuality);
 	CString fPitchAndFamily;fPitchAndFamily.Format("%d", m_label_font.lfPitchAndFamily);
 	CString fFaceName;fFaceName.Format("%d", m_label_font.lfFaceName);
 
  
 														
	Q_FONT_UPDATE = Q_FONT_UPDATE + "'" +
 					nm_user_map + "'," + 
 					fHeight + "," + 
 					fWidth + "," + 
 					fEscapement + "," + 
 					fOrientation + "," + 
 					fWeight + "," + 
 					fItalic + "," + 
 					fUnderline + "," + 
 					fStrikeOut + "," + 
 					fCharSet + "," + 
 					fOutPrecision + "," + 
 					fClipPrecision + "," + 
 					fQuality + "," + 
 					fPitchAndFamily + "," + 
					fFaceName + ")"; 
	
//	Q_FONT_UPDATE = Q_FONT_UPDATE + "'" +
//  					nm_user_map + "'," +
// 					fHeight + "," +
// 					fWidth + "," +
					fEscapement  + ")"; 
 
 	TRACE( Q_FONT_UPDATE );
 	hr_success = cmd_update.Open( s_cmd, Q_FONT_UPDATE );
  	if( FAILED( hr_success ) )
 	{
 		return;	 
	}
}


void CMapDataMgr::save_user_map_attri_on_mdb( S_LAYER_INFO & layer_info )
{ 
		HRESULT hr_success; 	 		
		CCommand<CNoAccessor,CNoRowset> cmd_update;
		CDataSource	datasource;
		hr_success = datasource.OpenFromInitializationString
			(OLESTR("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=C:\\AFC4I\\bin\\create_info.mdb;"));
		if( FAILED( hr_success ) )return;		
		

		CSession s_cmd;
		hr_success = s_cmd.Open(datasource);
		if( FAILED( hr_success ) )return;		
		
		CString scale_min; scale_min.Format("%d",layer_info.scale_min);
		CString scale_max; scale_max.Format("%d",layer_info.scale_max);
		CString view; view.Format("%d",layer_info.visible);
		CString polygon_is_pattern; 


		
		if( layer_info.polygon_is_pattern == TRUE )
			polygon_is_pattern.Format("%d",layer_info.polygon_is_pattern); 
		else
			polygon_is_pattern.Format("%d",FALSE); 
		
		CString order; order.Format("%d",layer_info.order);
		
		CString Q_LAYER_UPDATE =  "INSERT INTO USER_MAP_ATTRI ( NM_USERMAP, ORDER_, ID_LAYER, NM_LAYER, NM_LAYER_KOR, NM_SHP, TYPE_GEO, TYPE_MAP,SCALE, RANGE_MIN, RANGE_MAX, VISIBLE, POINT_SIZE, POINT_IMAGE, POLYLINE_WIDTH, POLYLINE_CLR, POLYLINE_TYPE,POLYLINE_ETC,POLYGON_WIDTH, POLYGON_TYPE, POLYGON_CLR_OUT, POLYGON_CLR_IN, POLYGON_PATTERN_IMAGE, POLYGON_IS_PATTERN, POLYGON_IS_IN ) VALUES ("; 

		Q_LAYER_UPDATE = Q_LAYER_UPDATE + 
			"'" + m_nm_user_map + "'," +
			order + "," +
			"'"	+ layer_info.id_layer + "'," +
			"'"	+ layer_info.nm_layer + "'," +
			"'"	+ layer_info.nm_layer_kor + "'," +
			"'"	+ layer_info.nm_file + "'," +
			"'"	+ layer_info.type_geo + "'," +
			
			"'"	+ layer_info.type_map + "'," +
			"'"	+ layer_info.scale + "'," +
			""	+ scale_min + "," +
			""	+ scale_max + "," +
			""	+ view + "," +
			
			"'"	+ layer_info.point_size + "'," +
			"'"	+ layer_info.point_image + "'," +
			"'"	+ layer_info.polyline_width + "'," +
			"'"	+ layer_info.polyline_clr + "'," +
			"'"	+ layer_info.polyline_type + "'," +
			
			"'"	+ ""/*layer_info.polyline_etc*/ + "'," +
			"'"	+ layer_info.polygon_width + "'," +
			"'"	+ layer_info.polygon_type + "'," +
			"'"	+ layer_info.polygon_clr_out + "'," +
			"'"	+ layer_info.polygon_clr_in + "'," +
			
			"'"	+ layer_info.polygon_pattern_image + "'," +
			""	+ polygon_is_pattern + "," +
			"'"	+ layer_info.polygon_is_in + "')";
				
		hr_success = cmd_update.Open( s_cmd, Q_LAYER_UPDATE );
	//	TRACE( Q_LAYER_UPDATE );
		cmd_update.Close();
		s_cmd.Close();
		datasource.Close();
}

void CMapDataMgr::save_current_map_info_on_mem( S_MAP_INFO & map_info )
{	
	m_current_map_info = map_info;
	
	m_current_map_info.lMapKind = map_info.lMapKind;
	int iScale = (int)map_info.lMapScale;
	//int iScale = (int)map_info.iMapRatio;
	/*
	MS_NOTHING = -1,	// 지정되지 않음
	MS_1_25000 = 0,	// 1:2.5만(공도에서 사용)
	MS_1_50000,		// 1:5만
	MS_1_100000,	// 1:10만
	MS_1_250000,	// 1:25만
	MS_1_500000,	// 1:50만
	MS_1_1000000,	// 1:100만
	MS_1_2000000,	// 1:200만
	MS_1_1,			// 1m급 위성영상
	MS_1_10,		// 10m급 위성영상
	MS_1_20,		// 20m급 위성영상
	MS_ERROR,		// error
	*/
	
	if(0 > iScale)
		m_enCurMapScale = MS_NOTHING;	
	else if(0 < iScale && iScale<= 50000)	
		m_enCurMapScale = MS_1_50000;			
	else if(50000< iScale && iScale<= 100000)
		m_enCurMapScale = MS_1_100000;
	else if(100000< iScale && iScale<= 250000)
		m_enCurMapScale = MS_1_250000;
	else if(250000 < iScale && iScale<= 500000)
		m_enCurMapScale = MS_1_500000;
	else if(500000 < iScale && iScale<= 1000000)
		m_enCurMapScale = MS_1_1000000;
	else if(1000000 < iScale && iScale<= 2000000)
		m_enCurMapScale = MS_1_2000000;
	else
		m_enCurMapScale = MS_NOTHING;		
}



S_MAP_INFO CMapDataMgr::get_current_map_info()
{
	return m_current_map_info;
}

void CMapDataMgr::add_user_map_info( S_MAP_INFO & map_info )
{
	mvec_user_map_info.push_back( map_info );
}

 
void CMapDataMgr::get_user_map_info( itr_user_map_info &itr_begin, itr_user_map_info &itr_end )
{
	itr_begin = mvec_user_map_info.begin();
	itr_end = mvec_user_map_info.end();
}


	/*DWORD error = GetLastError();
	if(error)
	{
		LPVOID lpMsgBuf=NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL );
		if(lpMsgBuf)
		{
			// Display the string.
			MessageBox( NULL, (LPCTSTR)lpMsgBuf, "", MB_OK | MB_ICONINFORMATION );
			// Free the buffer.
			LocalFree( lpMsgBuf );
		}
	}
	*/
 

void CMapDataMgr::set_identify( BOOL identify )
{ 
	m_is_identify = identify;
}

BOOL CMapDataMgr::is_identify()
{ 
	return m_is_identify; 
}

void CMapDataMgr::set_identify_coord( CPoint pt_coord )
{ 
	m_pt_selected_coord = pt_coord;
}

CPoint CMapDataMgr::get_identify_coord()
{
	return m_pt_selected_coord;
}

void CMapDataMgr::load_user_map( CString nm_user_map )
{	
	CDataSource			datasource;
	HRESULT hr_success;
	hr_success = datasource.OpenFromInitializationString(OLESTR("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=C:\\AFC4I\\bin\\create_info.mdb;"));
	
	if( FAILED( hr_success ) )	return;	

	{
		CSession s_layer_info;
		CCommand<CAccessor<CDB_USER_MAP_ATTRI> > cmd_layer_info;	
		CString Q_LAYER_INFO = "SELECT * FROM USER_MAP_ATTRI WHERE ( NM_USERMAP ='";
		Q_LAYER_INFO = Q_LAYER_INFO + nm_user_map + "')";

		hr_success = s_layer_info.Open(datasource); 
		if( FAILED( hr_success ) ) 
		{
			AfxMessageBox("FAILED s_layer_info.Open(datasource); ");
			return;	
		}
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
				layer_info.visible = cmd_layer_info.visible;
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
				layer_info.is_edit = TRUE;

				CMapDataMgr::add_vector_layer_info( layer_info, atoi(layer_info.scale) );
				//CMapDataMgr::add_order_info( layer_info.order, layer_info.nm_layer, atoi(layer_info.scale) );
				
				if(cmd_layer_info.MoveNext() != S_OK) break;
			}
		}		
		cmd_layer_info.Close();
		s_layer_info.Close();
	}
	datasource.Close(); 
}


void CMapDataMgr::add_feature( S_SELECTED_FEATURE & feature )
{
	mvec_selected_feature.push_back( feature );	
}

int CMapDataMgr::get_feature_cnt()
{
	return mvec_selected_feature.size();	
}

void CMapDataMgr::get_feature( itr_feature &itr_begin, itr_feature &itr_end )
{
	itr_begin = mvec_selected_feature.begin();
	itr_end = mvec_selected_feature.end();
}
void CMapDataMgr::delete_feature()
{
	mvec_selected_feature.clear();	
}


void CMapDataMgr::set_dbf_font(LOGFONT font)
{		
	memcpy(&m_label_font, &font, sizeof(LOGFONT));
}

LOGFONT CMapDataMgr::get_dbf_font()
{
	//memcpy(&font, &m_label_font, sizeof(LOGFONT));
	return m_label_font;
}

void CMapDataMgr::set_current_map_kind(EGMapKind eMapKind)
{
	m_current_map_info.lMapKind = eMapKind;
}

void CMapDataMgr::worldmapShpDraw(CDC* pDC, double dZoomFactor, int iViewType)
{
	CCoord m_Coord;
	CCoordMgr* m_pCoordMgr = GP_COORDMGR;

	CWnd* pWnd = AfxGetMainWnd();

	//if (m_pMapInfo == NULL)
	//	return;

	stTextPriority* pTextPriority = NULL;
	stTextPriority* pTextPriority1 = NULL;
	stTextPriority* pTextPriority2 = NULL;

	//CRect rect;
	CRect rc;
	GPoint gp, gpOri;
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;

	COLORREF					m_clBackColor[2];
	long						m_lEllipsoid;

	double m_dZoomFactor; //= dZoomFactor;

	m_lEllipsoid = EWGE;
	m_clBackColor[0] = RGB(145, 177, 190);	//배경 색상 설정 
	m_clBackColor[1] = RGB(187, 196, 190);	//배경 색상 설정 


	//rect = CGLib::GetClientRect();
	dZoomFactor /= 100;
	dZoomFactor *= 2.5;

	m_dZoomFactor = dZoomFactor;

	int iShpType;
	double left, top, right, bottom;
	//m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);

	CGRectGP rectGP;//(left, top, right, bottom);

	pWnd->GetClientRect(&rc);
	//pView->ScreenToClient(&rc);

	m_pCoordMgr->ScreenXY2WorldXY(rc.left, rc.top, &left, &top);
	m_pCoordMgr->ScreenXY2WorldXY(rc.right, rc.bottom, &right, &bottom);
	m_Coord.x_y_to_degrees_lat_long(left, top, &rectGP.m_R.top, &rectGP.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(right, bottom, &rectGP.m_R.bottom, &rectGP.m_R.right);


	gp.x = gpOri.x - (gpOri.x - rectGP.m_R.left);
	gp.y = gpOri.y - (gpOri.y - rectGP.m_R.top);

	CPen pen, * oldPen;
	CBrush brush, * oldbrush;
	CSize size;

	double dViewRatio;
	CString strFileName;
	int iIndex;
	shpObject* shpDraw;
	stSmallMap* smallMap = CMapDataMgr::GetWorldMapHead();
	while (smallMap)
	{
		smallMap = CMapDataMgr::GetWorldMapNext();
		if (smallMap == NULL) break;
		shpDraw = smallMap->m_pObject;

		iShpType = shpDraw->GetSHPType();
		if (iViewType != iShpType) continue;

		strFileName = _T("");

		switch (iShpType)
		{
		case SHPT_POINT:
		{
			for (int i = 0; i < CMapDataMgr::GetTextPriorityCnt(); i++)
			{
				pTextPriority = CMapDataMgr::GetTextPriority(i);
				if (pTextPriority->m_strWorldMapPoint == "국가명")
					pTextPriority1 = pTextPriority;
				else if (pTextPriority->m_strWorldMapPoint == "행정지역명")
					pTextPriority2 = pTextPriority;
			}

			size.cx = 1;
			pDC->DPtoLP(&size);
			pen.CreatePen(PS_SOLID, size.cx, RGB(255, 0, 0));
			brush.CreateSolidBrush(RGB(255, 0, 0));
			oldPen = pDC->SelectObject(&pen);
			oldbrush = pDC->SelectObject(&brush);

			iIndex = smallMap->m_strFileName.ReverseFind('\\');
			if (iIndex != -1)
				strFileName = smallMap->m_strFileName.Right(smallMap->m_strFileName.GetLength() - (iIndex + 1));
			if (strFileName.Left(9) == "eur_point" ||
				strFileName.Left(9) == "noa_point" ||
				strFileName.Left(9) == "sas_point" ||
				strFileName.Left(9) == "soa_point")
			{
				shpDraw->SetPointType(0, 4);
				if (pTextPriority1 != NULL)
				{
					dViewRatio = 100.0 / m_dZoomFactor;
					if (pTextPriority1->m_bIsVisible == FALSE ||
						dViewRatio > (double)pTextPriority1->m_uiViewRatio)
					{
						pDC->SelectObject(oldPen);
						pDC->SelectObject(oldbrush);
						pen.DeleteObject();
						brush.DeleteObject();
						break;
					}

					if (pTextPriority1->m_pFontType == NULL)
					{
						((shpPoint*)shpDraw)->SetFontType(ANSI_CHARSET);
						((shpPoint*)shpDraw)->SetFaceName("굴림");
					}
					else
					{
						((shpPoint*)shpDraw)->SetFontType(pTextPriority1->m_pFontType->m_nCharSet);
						((shpPoint*)shpDraw)->SetFaceName(pTextPriority1->m_pFontType->m_strName);
					}

					((shpPoint*)shpDraw)->SetFontSize(pTextPriority1->m_uiFontSize);
					((shpPoint*)shpDraw)->SetFontStrikeOut(FALSE);
					((shpPoint*)shpDraw)->SetFontColor(pTextPriority1->m_clFontColor);
					((shpPoint*)shpDraw)->SetVisibleType(pTextPriority1->m_uiVisibleType);
				}
				shpDraw->SetLabel(0);
			}
			else
			{
				shpDraw->SetPointType(0, 2);
				if (pTextPriority2 != NULL)
				{
					dViewRatio = 100.0 / m_dZoomFactor;
					if (pTextPriority2->m_bIsVisible == FALSE ||
						dViewRatio > (double)pTextPriority2->m_uiViewRatio)
					{
						pDC->SelectObject(oldPen);
						pDC->SelectObject(oldbrush);
						pen.DeleteObject();
						brush.DeleteObject();
						break;
					}

					if (pTextPriority1->m_pFontType == NULL)
					{
						((shpPoint*)shpDraw)->SetFontType(ANSI_CHARSET);
						((shpPoint*)shpDraw)->SetFaceName("굴림");
					}
					else
					{
						((shpPoint*)shpDraw)->SetFontType(pTextPriority2->m_pFontType->m_nCharSet);
						((shpPoint*)shpDraw)->SetFaceName(pTextPriority2->m_pFontType->m_strName);
					}

					((shpPoint*)shpDraw)->SetFontSize(pTextPriority2->m_uiFontSize);
					((shpPoint*)shpDraw)->SetFontStrikeOut(FALSE);
					((shpPoint*)shpDraw)->SetFontColor(pTextPriority2->m_clFontColor);
					((shpPoint*)shpDraw)->SetVisibleType(pTextPriority2->m_uiVisibleType);
				}
				shpDraw->SetLabel(1);
			}

			shpDraw->DrawMemoryMap(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
			pDC->SelectObject(oldPen);
			pDC->SelectObject(oldbrush);
			pen.DeleteObject();
			brush.DeleteObject();
		}
		break;
		case SHPT_ARC:
		{
			iIndex = smallMap->m_strFileName.ReverseFind('\\');
			if (iIndex != -1)
				strFileName = smallMap->m_strFileName.Right(smallMap->m_strFileName.GetLength() - (iIndex + 1));
			if (strFileName.Left(6) == "LAP030" ||
				strFileName.Left(6) == "LAP050" ||
				strFileName.Left(6) == "LAN010")
			{
				size.cx = 1;
				pDC->DPtoLP(&size);
				pen.CreatePen(PS_SOLID, size.cx, RGB(171, 86, 0));
				brush.CreateSolidBrush(RGB(171, 86, 0));
			}
			else
			{
				size.cx = 1;
				pDC->DPtoLP(&size);
				pen.CreatePen(PS_SOLID, size.cx, RGB(64, 64, 64));
				brush.CreateSolidBrush(RGB(64, 64, 64));
			}

			oldPen = pDC->SelectObject(&pen);
			oldbrush = pDC->SelectObject(&brush);
			shpDraw->Draw(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
			pDC->SelectObject(oldPen);
			pDC->SelectObject(oldbrush);
			pen.DeleteObject();
			brush.DeleteObject();
		}
		break;
		case SHPT_POLYGON:
		case SHPT_POLYGONZ:
		{
			size.cx = 1;
			pDC->DPtoLP(&size);
			//pDC->FillSolidRect(&rectGP, m_clBackColor[0]);
			pDC->FillSolidRect(rectGP.m_R.left, rectGP.m_R.top, rectGP.Width(), rectGP.Height(), m_clBackColor[0]);

			//				pen.CreatePen(PS_SOLID, size.cx, m_clBackColor[0]);
			pen.CreatePen(PS_SOLID, size.cx, RGB(175, 97, 97));
			brush.CreateSolidBrush(m_clBackColor[1]);
			oldPen = pDC->SelectObject(&pen);
			oldbrush = pDC->SelectObject(&brush);
			shpDraw->DrawMemoryMap(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
			pDC->SelectObject(oldPen);
			pDC->SelectObject(oldbrush);
			pen.DeleteObject();
			brush.DeleteObject();
		}
		break;
		}
	}
}