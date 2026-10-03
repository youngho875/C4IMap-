// AFC4IConfigMgr.cpp: implementation of the CMapConfigMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "MapConfigMgr.h"
#include "GLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapConfigMgr::CMapConfigMgr(CWnd* pParentWnd)
{
	CDC* dc = pParentWnd->GetDC();
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfCharSet = DEFAULT_CHARSET;
	::EnumFontFamiliesEx(dc->m_hDC, &lf,
				(FONTENUMPROC) EnumFamScreenCallBackEx, (LPARAM) this, NULL);

	m_dMapFirstLeft = m_dMapFirstTop = m_dMapFirstZoom = -1.0;
	m_iMapFirstScale = 0;
	m_strMapInfoDataPath = m_strShpDataPath = m_strRasterDataPath = m_strSatelliteDataPath = _T("");
	m_strSymbolDataPath = m_strPatternDataPath = m_strUserMapDataPath = _T("");
	m_dwMapGeneratorSleep = 0;

	//m_strFTPServerRootPath = _T("");
	//m_strFTPServer_port = "21";
	//m_strFTPServer_subpath = "/mapdata";
	//FTPServerInfoOpen();
}

CMapConfigMgr::~CMapConfigMgr()
{
	m_slScale.RemoveAll();
	m_slCoord.RemoveAll();
	m_slSmallMapFileName.RemoveAll();
	m_slSmallWorldMapFileName.RemoveAll();
	m_slWorldMapFileName.RemoveAll();
	m_slWorldMapLFileName.RemoveAll();
	m_slWorldMapPFileName.RemoveAll();
}

void CMapConfigMgr::OnReadItem( LPCTSTR lpszSection, LPCTSTR lpszItem, LPCTSTR lpValue, BOOL bSection )
{
	// the section must exist
	ASSERT(lpszSection);

	if( !bSection )
	{
	// 공통
		// 지도축척 리스트 얻기
		if( !strcmp( lpszItem, "scale_list1" ) || 
			!strcmp( lpszItem, "scale_list2" ) || 
			!strcmp( lpszItem, "scale_list3" ) || 
			!strcmp( lpszItem, "scale_list4" ) || 
			!strcmp( lpszItem, "scale_list5" ) ||
			!strcmp( lpszItem, "scale_list6"))
		{
			m_slScale.AddTail( lpValue );
		}
		// 좌표리스트 얻기
		else if( !strcmp( lpszItem, "coord_list1" ) || 
			!strcmp( lpszItem, "coord_list2" ) || 
			!strcmp( lpszItem, "coord_list3" ) || 
			!strcmp( lpszItem, "coord_list4" ) )
		{
			m_slCoord.AddTail( lpValue );
		}
		// 다이얼로그 배경색 정보 얻기
		else if( !strcmp( lpszItem, "dlg_bk_color" ) )
		{
			CString strDeli = ",";
			m_crDlgBKColor = GetStringToRGB(lpValue, strDeli);
		}
		// 다이얼로그 font 색 정보 얻기
		else if( !strcmp( lpszItem, "dlg_font_color" ) )
		{
			CString strDeli = ",";
			m_crDlgFontColor = GetStringToRGB(lpValue, strDeli);
		}
		// 다이얼로그 font 배경색 정보 얻기
		else if( !strcmp( lpszItem, "dlg_font_bk_color" ) )
		{
			CString strDeli = ",";
			m_crDlgFontBKColor = GetStringToRGB(lpValue, strDeli);
		}
/*
		// FTP server ip address 정보 얻기
		else if( !strcmp( lpszItem, "FTPServer_IP" ) )
		{
			m_strFTPServer_IP = lpValue;
		}
		// FTP server userid 정보 얻기
		else if( !strcmp( lpszItem, "FTPServer_ID" ) )
		{
			m_strFTPServer_ID = lpValue;
		}
		// FTP server password 정보 얻기
		else if( !strcmp( lpszItem, "FTPServer_password" ) )
		{
			CString strDeli = ",";
			m_strFTPServer_password = lpValue;
		}
		// FTP server port 정보 얻기
		else if( !strcmp( lpszItem, "FTPServer_port" ) )
		{
			CString strDeli = ",";
			m_strFTPServer_port = lpValue;
		}
		// FTP server path 정보 얻기
		else if( !strcmp( lpszItem, "FTPServer_path" ) )
		{
			CString strDeli = ",";
			m_strFTPServer_path = lpValue;
		}
*/
	// 지도
		// map version data path
		/*
		else if( !strcmp( lpszItem, "mapversiondata_path" ) )
		{
			m_strMapVersionDataPath = lpValue;
			DirectoryCreate(m_strMapVersionDataPath);
		}
		// server map version file name
		else if( !strcmp( lpszItem, "server_mapversion_filename" ) )
		{
			m_strServerMapVersionFileName = lpValue;
		}
		// local map version file name
		else if( !strcmp( lpszItem, "local_mapversion_filename" ) )
		{
			m_strLocalMapVersionFileName = lpValue;
		}
		*/
		// mapfilemaster file name
		/*
		else if( !strcmp( lpszItem, "mapfilemaster" ) )
		{
			m_strMapFileMaster = lpValue;
		}
		*/
		// map info data path
		else if( !strcmp( lpszItem, "mapinfodata_path" ) )
		{
			m_strMapInfoDataPath = lpValue;
			DirectoryCreate(m_strMapInfoDataPath);
		}
		// shp data path
		/*
		else if( !strcmp( lpszItem, "shpdata_path" ) )
		{
			m_strShpDataPath = lpValue;
			DirectoryCreate(m_strShpDataPath);
		}
		*/
		
		// raster data path
		else if( !strcmp( lpszItem, "rasterdata_path" ) )
		{
			m_strRasterDataPath = lpValue;
			DirectoryCreate(m_strRasterDataPath);
		}
		// satellite data path
		else if( !strcmp( lpszItem, "satellitedata_path" ) )
		{
			m_strSatelliteDataPath = lpValue;
			DirectoryCreate(m_strSatelliteDataPath);
		}
		// small map path
		else if( !strcmp( lpszItem, "smallmap_path" ) )
		{
			m_strSmallMapPath = lpValue;
			DirectoryCreate(m_strSmallMapPath);
		}
		// world map path
		else if( !strcmp( lpszItem, "worldmap_path" ) )
		{
			m_strWorldMapPath = lpValue;
			DirectoryCreate(m_strWorldMapPath);
		}

		// symbol data path
		else if( !strcmp( lpszItem, "symboldata_path" ) )
		{
			m_strSymbolDataPath = lpValue;
			DirectoryCreate(m_strSymbolDataPath);
		}
		// pattern data path
		else if( !strcmp( lpszItem, "patterndata_path" ) )
		{
			m_strPatternDataPath = lpValue;
			DirectoryCreate(m_strPatternDataPath);
		}
		// usermap data path
		else if( !strcmp( lpszItem, "usermapdata_path" ) )
		{
			m_strUserMapDataPath = lpValue;
			DirectoryCreate(m_strUserMapDataPath);
		}
		// 소축척지도 파일명 얻기
		else if( !strcmp( lpszItem, "small_map1" ) )
		{
			m_slSmallMapFileName.AddTail( lpValue );
		}
		// 인덱스지도(5만) 파일명 얻기
		else if( !strcmp( lpszItem, "small_map2" ) )
		{
			m_slSmallMapFileName.AddTail( lpValue );
		}
		// 인덱스지도(10만) 파일명 얻기
		else if( !strcmp( lpszItem, "small_map3" ) )
		{
			m_slSmallMapFileName.AddTail( lpValue );
		}
		// 지명지도 파일명 얻기
		else if( !strcmp( lpszItem, "small_map4" ) )
		{
			m_slSmallMapFileName.AddTail( lpValue );
		}
		// 세계지도 파일명 얻기(소축척)
		else if( !strcmp( lpszItem, "small_world_map" ) )
		{
			m_slSmallWorldMapFileName.AddTail( lpValue );
		}
		// 세계지도 파일명 얻기(면)
		else if( !strcmp( lpszItem, "world_map" ) )
		{
			m_slWorldMapFileName.AddTail( lpValue );
		}
		// 세계지도 파일명 얻기(선)
		else if( !strcmp( lpszItem, "world_map_line" ) )
		{
			m_slWorldMapFileName.AddTail( lpValue );
		}
		// 세계지도 파일명 얻기(점)
		else if( !strcmp( lpszItem, "world_map_point" ) )
		{
			m_slWorldMapFileName.AddTail( lpValue );
		}
		// 소축척지도 text data
		else if( !strcmp( lpszItem, "text_small_map" ) ) 
		{
			m_stSmallMapTextFileName = lpValue;
		}
		// 지도 좌단 좌표(최초 로드시 사용)
		else if( !strcmp( lpszItem, "map_left_point" ) ) 
		{
			m_dMapFirstLeft = atof(lpValue);
		}
		// 지도 상단 좌표(최초 로드시 사용)
		else if( !strcmp( lpszItem, "map_top_point" ) ) 
		{
			m_dMapFirstTop = atof(lpValue);
		}
		// 지도 배율(최초 로드시 사용)
		else if( !strcmp( lpszItem, "map_first_zoom" ) ) 
		{
			m_dMapFirstZoom = atof(lpValue);
		}
		// 지도 축척(최초 로드시 사용)
		else if( !strcmp( lpszItem, "map_first_scale" ) ) 
		{
			m_iMapFirstScale = atoi(lpValue);
		}
		// map min scale
		else if( !strcmp( lpszItem, "map_min_scale" ) ) 
		{
			m_map_min_scale = atoi(lpValue);
		}
		// map max scale
		else if( !strcmp( lpszItem, "map_max_scale" ) ) 
		{
			m_map_max_scale = atoi(lpValue);
		}
		// world map min scale
		else if( !strcmp( lpszItem, "worldmap_min_scale" ) ) 
		{
			m_worldmap_min_scale = atoi(lpValue);
		}
		// world map max scale
		else if( !strcmp( lpszItem, "worldmap_max_scale" ) ) 
		{
			m_worldmap_max_scale = atoi(lpValue);
		}
	
		// vector map min scale(2000000)
		else if( !strcmp( lpszItem, "vector_2000000_minscale" ) ) 
		{
			m_vector_2000000_minscale = atoi(lpValue);
		}
		// vector map max scale(2000000)
		else if( !strcmp( lpszItem, "vector_2000000_maxscale" ) ) 
		{
			m_vector_2000000_maxscale = atoi(lpValue);
		}

		// vector map min scale(1000000)
		else if( !strcmp( lpszItem, "vector_1000000_minscale" ) ) 
		{
			m_vector_1000000_minscale = atoi(lpValue);
		}
		// vector map max scale(1000000)
		else if( !strcmp( lpszItem, "vector_1000000_maxscale" ) ) 
		{
			m_vector_1000000_maxscale = atoi(lpValue);
		}
		// vector map min scale(500000)
		else if( !strcmp( lpszItem, "vector_500000_minscale" ) ) 
		{
			m_vector_500000_minscale = atoi(lpValue);
		}
		// vector map max scale(500000)
		else if( !strcmp( lpszItem, "vector_500000_maxscale" ) ) 
		{
			m_vector_500000_maxscale = atoi(lpValue);
		}
		// vector map min scale(250000)
		else if( !strcmp( lpszItem, "vector_250000_minscale" ) ) 
		{
			m_vector_250000_minscale = atoi(lpValue);
		}
		// vector map max scale(250000)
		else if( !strcmp( lpszItem, "vector_250000_maxscale" ) ) 
		{
			m_vector_250000_maxscale = atoi(lpValue);
		}
		// vector map min scale(100000)
		else if( !strcmp( lpszItem, "vector_100000_minscale" ) ) 
		{
			m_vector_100000_minscale = atoi(lpValue);
		}
		// vector map max scale(100000)
		else if( !strcmp( lpszItem, "vector_100000_maxscale" ) ) 
		{
			m_vector_100000_maxscale = atoi(lpValue);
		}
		// vector map min scale(50000)
		else if( !strcmp( lpszItem, "vector_50000_minscale" ) ) 
		{
			m_vector_50000_minscale = atoi(lpValue);
		}
		// vector map max scale(50000)
		else if( !strcmp( lpszItem, "vector_50000_maxscale" ) ) 
		{
			m_vector_50000_maxscale = atoi(lpValue);
		}
		// raster map min scale(2000000)
		else if( !strcmp( lpszItem, "raster_2000000_minscale" ) ) 
		{
			m_raster_2000000_minscale = atoi(lpValue);
		}
		// raster map max scale(2000000)
		else if( !strcmp( lpszItem, "raster_2000000_maxscale" ) ) 
		{
			m_raster_2000000_maxscale = atoi(lpValue);
		}
		// raster map min scale(1000000)
		else if( !strcmp( lpszItem, "raster_1000000_minscale" ) ) 
		{
			m_raster_1000000_minscale = atoi(lpValue);
		}
		// raster map max scale(1000000)
		else if( !strcmp( lpszItem, "raster_1000000_maxscale" ) ) 
		{
			m_raster_1000000_maxscale = atoi(lpValue);
		}
		// raster map min scale(500000)
		else if( !strcmp( lpszItem, "raster_500000_minscale" ) ) 
		{
			m_raster_500000_minscale = atoi(lpValue);
		}
		// raster map max scale(500000)
		else if( !strcmp( lpszItem, "raster_500000_maxscale" ) ) 
		{
			m_raster_500000_maxscale = atoi(lpValue);
		}
		// raster map min scale(250000)
		else if( !strcmp( lpszItem, "raster_250000_minscale" ) ) 
		{
			m_raster_250000_minscale = atoi(lpValue);
		}
		// raster map max scale(250000)
		else if( !strcmp( lpszItem, "raster_250000_maxscale" ) ) 
		{
			m_raster_250000_maxscale = atoi(lpValue);
		}
		// raster map min scale(100000)
		else if( !strcmp( lpszItem, "raster_100000_minscale" ) ) 
		{
			m_raster_100000_minscale = atoi(lpValue);
		}
		// raster map max scale(100000)
		else if( !strcmp( lpszItem, "raster_100000_maxscale" ) ) 
		{
			m_raster_100000_maxscale = atoi(lpValue);
		}
		// raster map min scale(50000)
		else if( !strcmp( lpszItem, "raster_50000_minscale" ) ) 
		{
			m_raster_50000_minscale = atoi(lpValue);
		}
		// raster map max scale(50000)
		else if( !strcmp( lpszItem, "raster_50000_maxscale" ) ) 
		{
			m_raster_50000_maxscale = atoi(lpValue);
		}
		// raster map min scale(20)
		else if( !strcmp( lpszItem, "raster_20_minscale" ) ) 
		{
			m_raster_20_minscale = atoi(lpValue);
		}
		// raster map max scale(20)
		else if( !strcmp( lpszItem, "raster_20_maxscale" ) ) 
		{
			m_raster_20_maxscale = atoi(lpValue);
		}
		// raster map min scale(10)
		else if( !strcmp( lpszItem, "raster_10_minscale" ) ) 
		{
			m_raster_10_minscale = atoi(lpValue);
		}
		// raster map max scale(10)
		else if( !strcmp( lpszItem, "raster_10_maxscale" ) ) 
		{
			m_raster_10_maxscale = atoi(lpValue);
		}
		// raster map min scale(1)
		else if( !strcmp( lpszItem, "raster_1_minscale" ) ) 
		{
			m_raster_1_minscale = atoi(lpValue);
		}
		// raster map max scale(1)
		else if( !strcmp( lpszItem, "raster_1_maxscale" ) ) 
		{
			m_raster_1_maxscale = atoi(lpValue);
		}
		//else if( !strcmp( lpszItem, "ftpserver_rootpath" ) )
		//{
		//	m_strFTPServerRootPath = lpValue;
		//}
	// 상황도
	
	// 투명도
		else if( !strcmp( lpszItem, "overlay_xml_format_path" ) ) 
		{
			m_strPathOvrXmlFormat = lpValue;
			DirectoryCreate(m_strMapVersionDataPath);
		}
	// THREAD
		else if( !strcmp( lpszItem, "mapgenerator_sleep" ) )
		{
			sscanf( lpValue, "%d", &m_dwMapGeneratorSleep );
		}
	}
}

// do something should be done after init
void CMapConfigMgr::OnReadEnd()
{
// set odbc datasource string
/*
	m_strDataSourceString = "ODBC;DSN=";
	m_strDataSourceString += m_strDataSource;
	m_strDataSourceString += ";UID=";
	m_strDataSourceString += m_strID;
	m_strDataSourceString += ";PWD=";
	m_strDataSourceString += m_strPWD;
*/
// do anything else
}

CString CMapConfigMgr::GetScaleHead()
{
	s_pos = cur_pos = m_slScale.GetHeadPosition();
	e_pos = m_slScale.GetTailPosition();
	m_slScale.GetNext(cur_pos);
	return m_slScale.GetHead();
}	

int CMapConfigMgr::GetScaleCount()
{
	return m_slScale.GetCount();
}

CString CMapConfigMgr::GetScaleNext()
{
	return m_slScale.GetNext(cur_pos);
}

CString CMapConfigMgr::GetCoordHead()
{
	s_pos = cur_pos = m_slCoord.GetHeadPosition();
	e_pos = m_slCoord.GetTailPosition();
	m_slCoord.GetNext(cur_pos);
	return m_slCoord.GetHead();
}	

int CMapConfigMgr::GetCoordCount()
{
	return m_slCoord.GetCount();
}

CString CMapConfigMgr::GetCoordNext()
{
	return m_slCoord.GetNext(cur_pos);
}

COLORREF CMapConfigMgr::GetStringToRGB(LPCTSTR lpValue, CString strDeli)
{
	int icolor1, icolor2, icolor3;
	CString strRGB;

	char* pDeli = strtok((LPSTR)lpValue, strDeli);
	strRGB.Format("%s", pDeli);
	icolor1 = atoi(strRGB);

	pDeli = strtok(NULL, strDeli);
	strRGB.Format("%s", pDeli);
	icolor2 = atoi(strRGB);

	pDeli = strtok(NULL, strDeli);
	strRGB.Format("%s", pDeli);
	icolor3 = atoi(strRGB);

	return RGB(icolor1, icolor2, icolor3);
}

COLORREF CMapConfigMgr::GetDlgBKColor()
{
	return m_crDlgBKColor;
}

COLORREF CMapConfigMgr::GetDlgFontColor()
{
	return m_crDlgFontColor;
}

COLORREF CMapConfigMgr::GetDlgFontBKColor()
{
	return m_crDlgFontBKColor;
}

CStringList* CMapConfigMgr::GetSmallMapFileName()
{
	return &m_slSmallMapFileName;
}

CString CMapConfigMgr::GetSmallMapTextFileName()
{
	return m_stSmallMapTextFileName;
}

CStringList* CMapConfigMgr::GetSmallWorldMapFileName()
{
	return &m_slSmallWorldMapFileName;
}

CStringList* CMapConfigMgr::GetWorldMapFileName()
{
	return &m_slWorldMapFileName;
}

CStringList* CMapConfigMgr::GetWorldMapLFileName()
{
	return &m_slWorldMapLFileName;
}

CStringList* CMapConfigMgr::GetWorldMapPFileName()
{
	return &m_slWorldMapPFileName;
}

BOOL CALLBACK AFX_EXPORT CMapConfigMgr::EnumFamScreenCallBackEx(ENUMLOGFONTEX* pelf,
	NEWTEXTMETRICEX* /*lpntm*/, int FontType, LPVOID pThis)
{
	// don't put in non-printer raster fonts
	if (FontType & RASTER_FONTTYPE)
		return 1;
	DWORD dwData = (FontType & TRUETYPE_FONTTYPE) ? TT_FONT : 0;

	((CMapConfigMgr*)pThis)->AddFont((ENUMLOGFONT*)pelf, dwData, CString(pelf->elfScript));
	return 1;
}

void CMapConfigMgr::AddFont(ENUMLOGFONT* pelf, DWORD dwType, LPCTSTR lpszScript)
{
	CMapDataMgr::AddSystemFontType(pelf, dwType, lpszScript);
}

/** 지도 좌단 좌표 얻기 */
double CMapConfigMgr::GetMapFirstLeft()
{
	return m_dMapFirstLeft;
}

/** 지도 상단 좌표 얻기 */
double CMapConfigMgr::GetMapFirstTop()
{
	return m_dMapFirstTop;
}

/** 지도 배율 얻기 */
double CMapConfigMgr::GetMapFirstZoom()
{
	return m_dMapFirstZoom;
}

/** 지도 축척 얻기 */
int CMapConfigMgr::GetMapFirstScale()
{
	return m_iMapFirstScale;
}

/** mapversion data path get */
CString CMapConfigMgr::GetMapVersionDataPath()
{
	return m_strMapVersionDataPath;
}

/** server mapversion file name get */
CString CMapConfigMgr::GetServerMapVersionFileName()
{
	return m_strServerMapVersionFileName;
}

/** local mapversion file name get */
CString CMapConfigMgr::GetLocalMapVersionFileName()
{
	return m_strLocalMapVersionFileName;
}

/** mapinfo data path get */
CString CMapConfigMgr::GetMapInfoDataPath()
{
	return m_strMapInfoDataPath;
}

/** shp data path get */
CString CMapConfigMgr::GetShpDataPath()
{
	return m_strShpDataPath;
}

/** raster data path get */
CString CMapConfigMgr::GetRasterDataPath()
{
	return m_strRasterDataPath;
}

/** satellite data path get */
CString CMapConfigMgr::GetSatelliteDataPath()
{
	return m_strSatelliteDataPath;
}

/** smallmap data path get */
CString CMapConfigMgr::GetSmallmapDataPath()
{
	return m_strSmallMapPath;
}

/** worldmap data path get */
CString CMapConfigMgr::GetWorldmapDataPath()
{
	return m_strWorldMapPath;
}

/** symbol data path get */
CString CMapConfigMgr::GetSymbolDataPath()
{
	return m_strSymbolDataPath;
}

/** pattern data path get */
CString CMapConfigMgr::GetPatternDataPath()
{
	return m_strPatternDataPath;
}

/** usermap data path get */
CString CMapConfigMgr::GetUserMapDataPath()
{
	return m_strUserMapDataPath;
}

DWORD CMapConfigMgr::GetMapGeneratorSleep()
{
	return m_dwMapGeneratorSleep;
}

/** map min/max scale get */
BOOL CMapConfigMgr::GetMapMinMaxScale(int &iminscale, int &imaxscale)
{
	if(m_map_min_scale <= 0 || m_map_min_scale > 500)
		return FALSE;
	if(m_map_max_scale <= 0 || m_map_max_scale > 5000000)
		return FALSE;

	iminscale = m_map_min_scale;
	imaxscale = m_map_max_scale;
	return TRUE;
}

/** vector map scale get */
BOOL CMapConfigMgr::GetVectorMapScale(EGMapScale enScale, int &iminscale, int &imaxscale)
{
	BOOL bRes = TRUE;

	switch(enScale)
	{
	case MS_1_2000000 : iminscale = m_vector_2000000_minscale;
						imaxscale = m_vector_2000000_maxscale;
						break;
	case MS_1_1000000 : iminscale = m_vector_1000000_minscale;
						imaxscale = m_vector_1000000_maxscale;
						break;
	case MS_1_500000 : iminscale = m_vector_500000_minscale;
						imaxscale = m_vector_500000_maxscale;
						break;
	case MS_1_250000 : iminscale = m_vector_250000_minscale;
						imaxscale = m_vector_250000_maxscale;
						break;
	case MS_1_100000 : iminscale = m_vector_100000_minscale;
						imaxscale = m_vector_100000_maxscale;
						break;
	case MS_1_50000 : iminscale = m_vector_50000_minscale;
						imaxscale = m_vector_50000_maxscale;
						break;
	default : bRes = FALSE;
		break;
	}

	return bRes;
}

/** vector map scale get */
BOOL CMapConfigMgr::GetVectorMapScale(int iscale, int &iminscale, int &imaxscale)
{
	BOOL bRes = TRUE;

	switch(iscale)
	{
	case 2000000 : iminscale = m_vector_2000000_minscale;
					imaxscale = m_vector_2000000_maxscale;
					break;
	case 1000000 : iminscale = m_vector_1000000_minscale;
					imaxscale = m_vector_1000000_maxscale;
					break;
	case 500000 : iminscale = m_vector_500000_minscale;
					imaxscale = m_vector_500000_maxscale;
					break;
	case 250000 : iminscale = m_vector_250000_minscale;
					imaxscale = m_vector_250000_maxscale;
					break;
	case 100000 : iminscale = m_vector_100000_minscale;
					imaxscale = m_vector_100000_maxscale;
					break;
	case 50000 : iminscale = m_vector_50000_minscale;
					imaxscale = m_vector_50000_maxscale;
					break;
	default : bRes = FALSE;
		break;
	}

	return bRes;
}

/** raster map scale get */
BOOL CMapConfigMgr::GetRasterMapScale(EGMapScale enScale, int &iminscale, int &imaxscale)
{
	BOOL bRes = TRUE;

	switch(enScale)
	{
	case MS_NOTHING :iminscale = m_raster_2000000_minscale;
		imaxscale = m_raster_2000000_maxscale;
		break;
	case MS_1_2000000 : iminscale = m_raster_2000000_minscale;
		imaxscale = m_raster_2000000_maxscale;
		break;
	case MS_1_1000000 : iminscale = m_raster_1000000_minscale;
		imaxscale = m_raster_1000000_maxscale;
		break;
	case MS_1_500000 : iminscale = m_raster_500000_minscale;
		imaxscale = m_raster_500000_maxscale;
		break;
	case MS_1_250000 : iminscale = m_raster_250000_minscale;
		imaxscale = m_raster_250000_maxscale;
		break;
	case MS_1_100000 : iminscale = m_raster_100000_minscale;
		imaxscale = m_raster_100000_maxscale;
		break;
	case MS_1_50000 :
	case MS_1_20 :
	case MS_1_10 :
	case MS_1_1 :
		iminscale = 500;
		imaxscale = m_raster_50000_maxscale;
		break;

	/*
	case MS_1_50000 : iminscale = m_raster_50000_minscale;
		imaxscale = m_raster_50000_maxscale;
		break;
	case MS_1_20 : iminscale = m_raster_20_minscale;
		imaxscale = m_raster_20_maxscale;
		break;
	case MS_1_10 : iminscale = m_raster_10_minscale;
		imaxscale = m_raster_10_maxscale;
		break;
	case MS_1_1 : iminscale = m_raster_1_minscale;
		imaxscale = m_raster_1_maxscale;
		break;
	*/
	default : iminscale = 0;
		imaxscale = 0;
		bRes = FALSE;
		break;
	}

	return bRes;
}

/** raster map scale get */
BOOL CMapConfigMgr::GetRasterMapScale(int iscale, int &iminscale, int &imaxscale)
{
	BOOL bRes = TRUE;

	switch(iscale)
	{
	case 2000000 : iminscale = m_raster_2000000_minscale;
					imaxscale = m_raster_2000000_maxscale;
					break;
	case 1000000 : iminscale = m_raster_1000000_minscale;
					imaxscale = m_raster_1000000_maxscale;
					break;
	case 500000 : iminscale = m_raster_500000_minscale;
					imaxscale = m_raster_500000_maxscale;
					break;
	case 250000 : iminscale = m_raster_250000_minscale;
					imaxscale = m_raster_250000_maxscale;
					break;
	case 100000 : iminscale = m_raster_100000_minscale;
					imaxscale = m_raster_100000_maxscale;
					break;
	case 50000 : iminscale = m_raster_50000_minscale;
						imaxscale = m_raster_50000_maxscale;
						break;
	case 20 : iminscale = m_raster_20_minscale;
					imaxscale = m_raster_20_maxscale;
					break;
	case 10 : iminscale = m_raster_10_minscale;
					imaxscale = m_raster_10_maxscale;
					break;
	case 1 : iminscale = m_raster_1_minscale;
					imaxscale = m_raster_1_maxscale;
					break;
	default : iminscale = 0;
		imaxscale = 0;
		bRes = FALSE;
		break;
	}

	return bRes;
}

int CMapConfigMgr::GetVectorMapKind(int iScale)
{
	int iMapKind = 0;
	
	if(iScale >= m_vector_2000000_minscale && iScale <= m_vector_2000000_maxscale)
		iMapKind = 2000000;
	else if(iScale >= m_vector_1000000_minscale && iScale <= m_vector_1000000_maxscale)
		iMapKind = 1000000;
	else if(iScale >= m_vector_500000_minscale && iScale <= m_vector_500000_maxscale)
		iMapKind = 500000;
	else if(iScale >= m_vector_250000_minscale && iScale <= m_vector_250000_maxscale)
		iMapKind = 250000;
	else if(iScale >= m_vector_100000_minscale && iScale <= m_vector_100000_maxscale)
		iMapKind = 100000;
	else if(iScale >= m_vector_50000_minscale && iScale <= m_vector_50000_maxscale)
		iMapKind = 50000;

	return iMapKind;
}

int CMapConfigMgr::GetRasterMapKind(int iScale)
{
	int iMapKind = 0;

	if(iScale >= m_raster_2000000_minscale && iScale <= m_raster_2000000_maxscale)
		iMapKind = 2000000;
	else if(iScale >= m_raster_1000000_minscale && iScale <= m_raster_1000000_maxscale)
		iMapKind = 1000000;
	else if(iScale >= m_raster_500000_minscale && iScale <= m_raster_500000_maxscale)
		iMapKind = 500000;
	else if(iScale >= m_raster_250000_minscale && iScale <= m_raster_250000_maxscale)
		iMapKind = 250000;
	else if(iScale >= m_raster_100000_minscale && iScale <= m_raster_100000_maxscale)
		iMapKind = 100000;
	else if(iScale >= m_raster_50000_minscale && iScale <= m_raster_50000_maxscale)
		iMapKind = 50000;
	else if(iScale >= m_raster_20_minscale && iScale <= m_raster_20_minscale)
		iMapKind = 20;
	else if(iScale >= m_raster_10_minscale && iScale <= m_raster_10_minscale)
		iMapKind = 10;
	else if(iScale >= m_raster_1_minscale && iScale <= m_raster_1_minscale)
		iMapKind = 1;

	return iMapKind;
}

BOOL CMapConfigMgr::IsScaleOver(int iMapType, BOOL bIsAutoMode, BOOL bIsMaxScale, long lScale, 
								  double dZoomFactor, double &newfactor, CString &strMsg)
{
	BOOL bRes = TRUE;
	int scale, iminscale, imaxscale, iminscale2, imaxscale2;
	EGMapScale enScale = CMapDataMgr::GetCurrentMapScale();
	scale = (int)((double)lScale / dZoomFactor);
 	strMsg = " "; 

	// 세계지도이면 세계지도 최소/최대배율로 배율 체크
	if(CMapDataMgr::IsWorldMapOpen())
	{
		if(scale >= m_worldmap_min_scale && scale <= m_worldmap_max_scale)
			bRes = FALSE;
		else if(scale < m_worldmap_min_scale)
		{
			newfactor = (double)lScale / (double)m_worldmap_min_scale;
			strMsg = "더 이상 확대 할 수 없습니다.";
		}
		else if(scale > m_worldmap_max_scale)
		{
			newfactor = (double)lScale / (double)m_worldmap_max_scale;
			strMsg = "더 이상 축소 할 수 없습니다.";
		}

		return bRes;
	}

	// 자동축척모드이면..
	if(bIsAutoMode)
	{
		if(scale >= m_map_min_scale && scale <= m_map_max_scale)
			bRes = FALSE;
		else if(scale < m_map_min_scale)
		{
			newfactor = (double)lScale / (double)m_map_min_scale;
			strMsg = "더 이상 확대 할 수 없습니다.";
		}
		else if(scale > m_map_max_scale)
		{
			newfactor = (double)lScale / (double)m_map_max_scale;
			strMsg = "더 이상 축소 할 수 없습니다.";
		}
	}
	// 자동축척모드가 아니면..
	else
	{
		switch(iMapType)
		{
		case 0 :	// vector map
			GetVectorMapScale((int)lScale, iminscale, imaxscale);
			break;
		case 1 :	// rastor map
			GetRasterMapScale(enScale, iminscale, imaxscale);
			break;
		case 2 :	// vector + raster map
			GetVectorMapScale((int)lScale, iminscale, imaxscale);
			GetRasterMapScale(enScale, iminscale2, imaxscale2);
			if(iminscale < iminscale2)
				iminscale = iminscale2;
			if(imaxscale > imaxscale2)
				imaxscale = imaxscale2;
			break;
		}

		if(scale >= m_map_min_scale && scale <= imaxscale)
			bRes = FALSE;
		else if(scale < iminscale)
		{
			newfactor = (double)lScale / (double)m_map_min_scale;
			strMsg = "더 이상 확대 할 수 없습니다.";
		}
		else if(scale > imaxscale)
		{
			newfactor = (double)lScale / (double)imaxscale;
			strMsg = "더 이상 축소 할 수 없습니다.";
		}
	}

	return bRes;
}

BOOL CMapConfigMgr::IsScaleOver2(int iMapType, BOOL bIsAutoMode, BOOL bIsMaxScale, long lScale, 
								  double dZoomFactor, double &newfactor, CString &strMsg)
{
	BOOL bRes = TRUE;
	int scale, iminscale, imaxscale, iminscale2, imaxscale2;
	EGMapScale enScale = CMapDataMgr::GetCurrentMapScale();
	scale = (int)dZoomFactor;
	strMsg = "";

	// 세계지도이면 세계지도 최소/최대배율로 배율 체크
	if(CMapDataMgr::IsWorldMapOpen())
	{
		if(scale >= m_worldmap_min_scale && scale <= m_worldmap_max_scale)
			bRes = FALSE;
		else if(scale < m_worldmap_min_scale)
		{
			newfactor = (double)lScale / (double)m_worldmap_min_scale;
			strMsg = "더 이상 확대 할 수 없습니다.";
		}
		else if(scale > m_worldmap_max_scale)
		{
			newfactor = (double)lScale / (double)m_worldmap_max_scale;
			strMsg = "더 이상 축소 할 수 없습니다.";
		}

		return bRes;
	}

	// 자동축척모드이면..
	if(bIsAutoMode)
	{
		if(scale >= m_map_min_scale && scale <= m_map_max_scale)
			bRes = FALSE;
		else if(scale < m_map_min_scale)
		{
			newfactor = (double)lScale / (double)m_map_min_scale;
			strMsg = "더 이상 확대 할 수 없습니다.";
		}
		else if(scale > m_map_max_scale)
		{
			newfactor = (double)lScale / (double)m_map_max_scale;
			strMsg = "더 이상 축소 할 수 없습니다.";
		}
	}
	// 자동축척모드가 아니면..
	else
	{
		switch(iMapType)
		{
		case 0 :	// vector map
			GetVectorMapScale((int)lScale, iminscale, imaxscale);
			break;
		case 1 :	// rastor map
			GetRasterMapScale(enScale, iminscale, imaxscale);
			break;
		case 2 :	// vector + raster map
			GetVectorMapScale((int)lScale, iminscale, imaxscale);
			GetRasterMapScale(enScale, iminscale2, imaxscale2);
			if(iminscale < iminscale2)
				iminscale = iminscale2;
			if(imaxscale > imaxscale2)
				imaxscale = imaxscale2;
			break;
		}

		if(scale >= m_map_min_scale && scale <= imaxscale)
			bRes = FALSE;
		else if(scale < iminscale)
		{
			newfactor = (double)lScale / (double)m_map_min_scale;
			strMsg = "더 이상 확대 할 수 없습니다.";
		}
		else if(scale > imaxscale)
		{
			newfactor = (double)lScale / (double)imaxscale;
			strMsg = "더 이상 축소 할 수 없습니다.";
		}
	}

	return bRes;
}

/* ppppp delete
BOOL CMapConfigMgr::IsScaleOver(int iMapType, BOOL bIsAutoMode, BOOL bIsMaxScale, long lScale, 
								  double dZoomFactor, double &newfactor, CString &strMsg)
{
	BOOL bRes = TRUE;
	int scale, iminscale, imaxscale, iminscale2, imaxscale2;
	EGMapScale enScale = CMapDataMgr::GetCurrentMapScale();
	scale = (int)((double)lScale / dZoomFactor);
	strMsg = "";

	// 자동축척모드이면..
	if(bIsAutoMode)
	{
		if(scale >= m_map_min_scale && scale <= m_map_max_scale)
			bRes = FALSE;
		else if(scale < m_map_min_scale)
		{
			newfactor = (double)lScale / (double)m_map_min_scale;
			strMsg = "더 이상 확대 할 수 없습니다.";
		}
		else if(scale > m_map_max_scale)
		{
			newfactor = (double)lScale / (double)m_map_max_scale;
			strMsg = "더 이상 축소 할 수 없습니다.";
		}
	}
	// 자동축척모드가 아니면..
	else
	{
		switch(iMapType)
		{
		case 0 :	// vector map
			GetVectorMapScale((int)lScale, iminscale, imaxscale);
			break;
		case 1 :	// rastor map
			GetRasterMapScale(enScale, iminscale, imaxscale);
			break;
		case 2 :	// vector + raster map
			GetVectorMapScale((int)lScale, iminscale, imaxscale);
			GetRasterMapScale(enScale, iminscale2, imaxscale2);
			if(iminscale < iminscale2)
				iminscale = iminscale2;
			if(imaxscale > imaxscale2)
				imaxscale = imaxscale2;
			break;
		}

		if(scale >= m_map_min_scale && scale <= imaxscale)
			bRes = FALSE;
		else if(scale < iminscale)
		{
			newfactor = (double)lScale / (double)m_map_min_scale;
			strMsg = "더 이상 확대 할 수 없습니다.";
		}
		else if(scale > imaxscale)
		{
			newfactor = (double)lScale / (double)imaxscale;
			strMsg = "더 이상 축소 할 수 없습니다.";
		}
	}

	return bRes;
}
*/

void CMapConfigMgr::SetCurrentMapInfo(int iScale, int iRatio)
{
	m_iScale = iScale;
	m_iRatio = iRatio;
	CGLib::SetCurrentMapInfo(iScale, iRatio);
}

void CMapConfigMgr::GetCurrentMapInfo(int &iScale, int &iRatio)
{
	iScale = m_iScale;
	iRatio = m_iRatio;
}

double CMapConfigMgr::GetCurrentZoomFactor()
{
	double zoomfactor;
//	return zoomfactor = (double)m_iScale / (double)m_iRatio;
	return zoomfactor = (double)1000000 / (double)m_iRatio;
}

CString CMapConfigMgr::GetOvrFormatPath()
{
	return m_strPathOvrXmlFormat;
}

///** FTP server ip address 정보 얻기 */
//CString CMapConfigMgr::GetFTPServer_IP()
//{
//	return m_strFTPServer_IP;
//}
//
///** FTP server userid 정보 얻기 */
//CString CMapConfigMgr::GetFTPServer_ID()
//{
//	return m_strFTPServer_ID;
//}
//
///** FTP server password 정보 얻기 */
//CString CMapConfigMgr::GetFTPServer_password()
//{
//	return m_strFTPServer_password;
//}
//
///** FTP server password 정보 얻기 */
//CString CMapConfigMgr::GetFTPServer_port()
//{
//	return m_strFTPServer_port;
//}
//
///** FTP server path 정보 얻기 */
//CString CMapConfigMgr::GetFTPServer_path()
//{
//	return m_strFTPServer_path;
//}
//
///** FTP server subpath 정보 얻기 */
//CString CMapConfigMgr::GetFTPServer_subpath()
//{
//	return m_strFTPServer_subpath;
//}

CString CMapConfigMgr::GetMapFileMasterFileName()
{
	return m_strMapFileMaster;
}

int CMapConfigMgr::GetMapScale()
{
	return m_iMapScale;
}

void CMapConfigMgr::SetMapScale(EGMapScale enMapScale)
{
	switch(enMapScale)
	{
	case MS_1_50000 : m_iMapScale = 50000;
		break;
	case MS_1_100000 : m_iMapScale = 100000;
		break;
	case MS_1_250000 : m_iMapScale = 250000;
		break;
	case MS_1_500000 : m_iMapScale = 500000;
		break;
	case MS_1_1000000 : m_iMapScale = 1000000;
		break;
	default :
		m_iMapScale = 0;
	}
}
/*
BOOL CMapConfigMgr::FTPServerInfoOpen()
{
	CString strFileName = "C:\\Program Files\\afc4i_update\\AutoUpdate.ini";
	
	CFileIO file;
	file.Init();
	if(!file.Open(strFileName, CFile::modeRead | CFile::typeText))
	{
		return FALSE;
	}

	BOOL bSection = FALSE;
	CString strRead, strSection, strItem, strVal;
	while(file.ReadString(strRead))
	{
		strRead.TrimRight();

		if(strRead.Find('[') != -1 && strRead.Find(']') != -1)
		{
			strSection = strRead;
			strItem = strVal = _T("");
		}
		else
		{
			int iLen = strRead.GetLength();
			int iEqIdx = strRead.Find('=');
			if(iEqIdx >= 1 && iLen > iEqIdx+1)
			{
				strItem = strRead.Left(iEqIdx);
				strVal = strRead.Right(iLen-(iEqIdx+1)); 
			}
			else if(iLen == 0)
			{
				continue;
			}
			else 
			{
				return FALSE;
			}
		}

		strSection.TrimLeft();
		strSection.TrimRight();
		strItem.TrimLeft();
		strItem.TrimRight();
		strVal.TrimLeft();
		strVal.TrimRight();

		if(!strSection.IsEmpty())
		{
			if(!strcmp(strItem, "IP"))
			{
				m_strFTPServer_IP = strVal;
			}
			else if(!strcmp(strItem, "ID"))
			{
				m_strFTPServer_ID = strVal;
			}
			else if(!strcmp(strItem, "PW"))
			{
				m_strFTPServer_password = strVal;
			}
			else if(!strcmp(strItem, "SERVER_ROOT"))
			{
				m_strFTPServer_path = strVal;
			}
		}
	}

	TRACE("*********   FIRST FTP SET   ***********\n");
	TRACE("INFO : FTP_IP < %s >, ID < %s >, PW < %s > \n"
			   , m_strFTPServer_IP
			   , m_strFTPServer_ID
			   , m_strFTPServer_password);
	file.Close();

	return TRUE;
}

BOOL CMapConfigMgr::FTPServerInfoOpen(WPARAM wParam)
{
	char strDel[] = "|";
	CString temp = (LPCTSTR)wParam;
	if(temp.GetLength() > 0)
	{
		m_strFTPServer_IP = strtok((LPSTR)(LPCTSTR)temp, strDel);
		m_strFTPServer_ID = strtok(NULL, strDel);
		m_strFTPServer_password = strtok(NULL, strDel);
		TRACE("*********   LAST FTP SET   ***********\n");
		TRACE("INFO : FTP_IP < %s >, ID < %s >, PW < %s > \n"
			   , m_strFTPServer_IP
			   , m_strFTPServer_ID
			   , m_strFTPServer_password);
		return TRUE;
	}
	return FALSE;
}
*/
void CMapConfigMgr::DirectoryCreate(CString strPath)
{
	CStringList slPath;
	if(!ExtractDir(strPath, slPath))
		return;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = FALSE;

	CString strTmp;
	CString strDir = _T("");
	BOOL bIsFirst = TRUE;
	POSITION pos = slPath.GetHeadPosition();
	while(pos)
	{
		strTmp = slPath.GetNext(pos);
		if(bIsFirst)
		{
			strDir = strTmp + "\\";
			bIsFirst = FALSE;
			continue;
		}
		strDir = strDir + strTmp + "\\";
		::CreateDirectory((LPCSTR)strDir, &sa);
	}

	slPath.RemoveAll();
}

BOOL CMapConfigMgr::ExtractDir(CString strSource, CStringList &slData)
{
	CString strDir;
	CString strData = strSource;
	int iIndex, iIndex2;
	iIndex2 = 0;

	while(TRUE)
	{
		iIndex = strData.Find("\\");
		if(iIndex == -1)
			break;
		strDir = strData.Mid(iIndex2, iIndex);
		strData = strData.Mid(iIndex+2, strData.GetLength()-iIndex+1);
		slData.AddTail(strDir);
	}

	if(slData.GetCount() > 0)
		return TRUE;
	else
		return FALSE;
}

//CString CMapConfigMgr::GetFTPServerRootPath()
//{
//	return m_strFTPServerRootPath;
//}

int CMapConfigMgr::GetWorldMapMaxRatio()
{
	return m_worldmap_max_scale;
}
