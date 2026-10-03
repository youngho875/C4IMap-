#ifndef __INITIATOR____H__
#define __INITIATOR____H__

#include <afxtempl.h>
#include "io.hpp"



static CString m_strMapInfoDataPath;		// Info Data Path 
static CString m_strShpDataPath;			// Shp Data Path 
static CString m_strRasterDataPath;			// Raster Data Path
static CString m_strSatelliteDataPath;		// Satellite Data Path
static CString m_strSmallMapPath;			// Small Map Path 
static CString m_strWorldMapPath;			// World Map Path 
static CString m_strSymbolDataPath;			// symbol data path 
static CString m_strPatternDataPath;		// pattern data path 
static CString m_strUserMapDataPath;		// usermap data path 

 

#include <atldbcli.h> 

 
class CDB_LAYER_INFO
{
public:
	INT order;
	TCHAR id_layer[50+1];
	TCHAR nm_layer[50+1];
	TCHAR nm_layer_kor[50+1];
	TCHAR nm_file[50+1];

	TCHAR type_geo[10+1]; 
	TCHAR type_map[10+1]; 
	TCHAR scale[5+1]; 
	INT scale_min;
	INT scale_max;

	BOOL  view;
	TCHAR point_size[2+1]; 
	TCHAR point_image[50+1]; 
	TCHAR polyline_width[2+1];  
	TCHAR polyline_clr[9+1];  

	TCHAR polyline_type[10+1];  
	TCHAR polyline_etc[50+1];  
	TCHAR polygon_width[2+1];  
	TCHAR polygon_type[10+1];  
	TCHAR polygon_clr_out[9+1]; 
	
	TCHAR polygon_clr_in[9+1];  
	TCHAR polygon_pattern_image[50+1];  
	BOOL  polygon_is_pattern;  
	TCHAR polygon_is_in[3+1];  
 
	BEGIN_COLUMN_MAP(CDB_LAYER_INFO)
		COLUMN_ENTRY(1, order)
		COLUMN_ENTRY(2, id_layer) 
		COLUMN_ENTRY(3, nm_layer)
		COLUMN_ENTRY(4, nm_layer_kor)
		COLUMN_ENTRY(5, nm_file) 

		COLUMN_ENTRY(6, type_geo) 
		COLUMN_ENTRY(7, type_map)
		COLUMN_ENTRY(8, scale) 
		COLUMN_ENTRY(9, scale_min)
		COLUMN_ENTRY(10, scale_max)

		COLUMN_ENTRY(11, view)
		COLUMN_ENTRY(12, point_size) 
		COLUMN_ENTRY(13, point_image) 
		COLUMN_ENTRY(14, polyline_width)
		COLUMN_ENTRY(15, polyline_clr) 

		COLUMN_ENTRY(16, polyline_type)
		COLUMN_ENTRY(17, polyline_etc) 
		COLUMN_ENTRY(18, polygon_width) 
		COLUMN_ENTRY(19, polygon_type)
		COLUMN_ENTRY(20, polygon_clr_out) 

		COLUMN_ENTRY(21, polygon_clr_in)
		COLUMN_ENTRY(22, polygon_pattern_image) 
		COLUMN_ENTRY(23, polygon_is_pattern) 
		COLUMN_ENTRY(24, polygon_is_in) 
	END_COLUMN_MAP()
};

class CDB_USER_MAP_ATTRI
{
public:
	TCHAR nm_user_map[50+1];
	INT order;
	TCHAR id_layer[50+1];
	TCHAR nm_layer[50+1];
	TCHAR nm_layer_kor[50+1];
	TCHAR nm_file[50+1];

	TCHAR type_geo[10+1]; 
	TCHAR type_map[10+1]; 
	TCHAR scale[5+1]; 
	INT scale_min;
	INT scale_max;

	BOOL  visible;
	TCHAR point_size[2+1]; 
	TCHAR point_image[50+1]; 
	TCHAR polyline_width[2+1];  
	TCHAR polyline_clr[9+1];  

	TCHAR polyline_type[10+1];  
	TCHAR polyline_etc[50+1];  
	TCHAR polygon_width[2+1];  
	TCHAR polygon_type[10+1];  
	TCHAR polygon_clr_out[9+1]; 
	
	TCHAR polygon_clr_in[9+1];  
	TCHAR polygon_pattern_image[50+1];  
	BOOL  polygon_is_pattern;  
	TCHAR polygon_is_in[3+1];  
 
	BEGIN_COLUMN_MAP(CDB_USER_MAP_ATTRI)
		COLUMN_ENTRY(1, nm_user_map)
		COLUMN_ENTRY(2, order)
		COLUMN_ENTRY(3, id_layer) 
		COLUMN_ENTRY(4, nm_layer)
		COLUMN_ENTRY(5, nm_layer_kor)
		COLUMN_ENTRY(6, nm_file) 

		COLUMN_ENTRY(7, type_geo) 
		COLUMN_ENTRY(8, type_map)
		COLUMN_ENTRY(9, scale) 
		COLUMN_ENTRY(10, scale_min)
		COLUMN_ENTRY(11, scale_max)

		COLUMN_ENTRY(12, visible)
		COLUMN_ENTRY(13, point_size) 
		COLUMN_ENTRY(14, point_image) 
		COLUMN_ENTRY(15, polyline_width)
		COLUMN_ENTRY(16, polyline_clr) 

		COLUMN_ENTRY(17, polyline_type)
		COLUMN_ENTRY(18, polyline_etc) 
		COLUMN_ENTRY(19, polygon_width) 
		COLUMN_ENTRY(20, polygon_type)
		COLUMN_ENTRY(21, polygon_clr_out) 

		COLUMN_ENTRY(22, polygon_clr_in)
		COLUMN_ENTRY(23, polygon_pattern_image) 
		COLUMN_ENTRY(24, polygon_is_pattern) 
		COLUMN_ENTRY(25, polygon_is_in) 
	END_COLUMN_MAP()
};

class CDB_SYMBOL_INFO
{
public:
	TCHAR nm_symbol[50+1];
	TCHAR nm_file[50+1]; 
 
	BEGIN_COLUMN_MAP(CDB_SYMBOL_INFO)
		COLUMN_ENTRY(1, nm_symbol)
		COLUMN_ENTRY(2, nm_file) 
 
	END_COLUMN_MAP()
};

class CDB_USER_MAP
{
public:
	INT no;
	BOOL is_default_map;
	TCHAR nm_usermap[50+1];
	BOOL bIsAutoMode;
	BOOL bIsWorldMapOpen;

	DOUBLE lMapMode;
	DOUBLE lMapScale;
	DOUBLE iMapRatio;
	DOUBLE iMapType;	
	DOUBLE dleft;

	DOUBLE dtop;
	DOUBLE dright;
	DOUBLE dbottom;
	TCHAR coord_center[100+1];
	DOUBLE dZoomFactor;  
	
	BEGIN_COLUMN_MAP(CDB_USER_MAP)
		COLUMN_ENTRY(1, no)
		COLUMN_ENTRY(2, is_default_map)
		COLUMN_ENTRY(3, nm_usermap) 
		COLUMN_ENTRY(4, bIsAutoMode)
		COLUMN_ENTRY(5, bIsWorldMapOpen)
		COLUMN_ENTRY(6, lMapMode) 

		COLUMN_ENTRY(7, lMapScale) 
		COLUMN_ENTRY(8, iMapRatio)
		COLUMN_ENTRY(9, iMapType) 
		COLUMN_ENTRY(10, dleft)
		COLUMN_ENTRY(11, dtop)

		COLUMN_ENTRY(12, dright)
		COLUMN_ENTRY(13, dbottom) 
		COLUMN_ENTRY(14, coord_center)  	
		COLUMN_ENTRY(15, dZoomFactor)
    END_COLUMN_MAP()
};


class CDB_PATTERN_INFO
{
public:
	TCHAR nm_pattern[50+1];
	TCHAR nm_file[50+1]; 
 
	BEGIN_COLUMN_MAP(CDB_PATTERN_INFO)
		COLUMN_ENTRY(1, nm_pattern)
		COLUMN_ENTRY(2, nm_file) 
 
	END_COLUMN_MAP()
};



/*
class CDB_FONT_INFO
{
public:
	TCHAR nm_pattern[50+1];
	TCHAR nm_file[50+1]; 
 
	BEGIN_COLUMN_MAP(CDB_PATTERN_INFO)
		COLUMN_ENTRY(1, nm_pattern)
		COLUMN_ENTRY(2, nm_file) 
 
	END_COLUMN_MAP()
};
*/


class CInitiator
{
public:
	CInitiator();
	~CInitiator();

protected:
	CFileIO		m_ini;
	BOOL		m_bDone;
	CFileIO		m_mapini;
	BOOL		m_bMapDone;

public:
	/** 초기화 */
	void	Init( LPCTSTR lpszInitFile );
	/** 초기화 여부 */
	BOOL	BeenInitiated(){ return m_bDone; }
	/** 벡터 기본정보 열기 */
	BOOL	InitVectorMapInfo();
	/** 벡터 파일 리스트 얻기 */
	BOOL	InitVectorMapProperty();
	/** 맵 속성정보 리스트 얻기(사용자) */
	BOOL	InitMapPriority();
	/** 맵 속성정보 리스트 얻기(default) */
	BOOL	InitDefaultMapPriority();
	/** 맵 지형정보 그룹 리스트 얻기 */
	BOOL	InitMapGroupItem();
	/** 맵 text data 속성정보 얻기(사용자) */
	BOOL	InitTextPriority();
	/** 맵 text data 속성정보 얻기(default) */
	BOOL	InitDefaultTextPriority();
	/** 세계지도 속성정보 얻기 */
	BOOL	InitWorldMapPriority();
	/** 사용자 저장지도 정보 얻기 */
	BOOL	OpenUserMapInfo();
	/** 레스터 지도 속성데이터 얻기 */
	BOOL	OpenRasterMapInfo();
	/** 위성영상 지도 속성데이터 얻기 */
	BOOL	OpenSatelliteMapInfo();
	/** 지명리스트 얻기 */
	BOOL	OpenNameData();

	void load_shp_attribute();
	void load_symbol_info();

protected:
	/** 파일 읽기 */
	virtual void	OnReadItem( LPCTSTR lpszSection, LPCTSTR lpszItem, LPCTSTR lpValue, BOOL bSection ) = 0;
	/** 파일 닫기 */
	virtual void	OnReadEnd()=0;
};

#endif