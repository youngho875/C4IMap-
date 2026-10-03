#if !defined(AFX_SHP_H__B6AB45BC_A808_444A_B172_EB019D8E74E4__INCLUDED_)
#define AFX_SHP_H__B6AB45BC_A808_444A_B172_EB019D8E74E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
//#include "define_struct.h"
//#include "define_resource_id.h"
//#include "Dbf.h"  
#endif 

#include "../inc/define.h"
#include "Dbf.h"

//#include "MAP/mapdisp/Dbf.h"


class CShp  
{
public:
	CShp();
	virtual ~CShp();		
private:
	GeoPoint * mp_geo_point;
	PolyObject * mp_geo_poly_obj;	
	int mn_size_file;
	BOOL mb_open_file;	
	int mtype_shape;
	
	char m_shp_path[_MAX_PATH];
	char m_shx_path[_MAX_PATH];
	char m_dbf_path[_MAX_PATH];
	int mcnt_record; 	
	
	int  mr_polyline, mg_polyline, mb_polyline;    
	double m_width_polyline;
	int m_width_polyline_tmp;
	short m_type_polyline;	
	
	int  r_point, g_point, b_point;   
	int  r_polyline, g_polyline, b_polyline;    
	int  r_polygon, g_polygon, b_polygon;  	
	int  r_polygon_in, g_polygon_in, b_polygon_in;  	
	
	CRect m_rect_device;		
	double m_width_point;  
	double m_heigh_point;   
	
	unsigned short m_layer_no;
	unsigned short m_id_file;
	CString m_id_layer; 	
	EGMapScale m_scale;
	
	CDbf m_dbf;
	S_FEATURE_INFO m_feature_selected;
	BOOL m_is_selected;
	INT m_idx_record_selected;
	INT m_idx_part_selected;	// for polygon, polyline
	BOOL m_view;
	
	CString m_path_symbol;
	CString m_path_pattern;
	int xSize;
	int ySize;
	CString	m_polygon_is_in;
	BOOL m_polygon_is_pattern;
	CString m_file_name;
	
	void swap_word( int length, void * p_word );	
	
public:
	MBR m_mbr;
	inline void get_nm_file( CString & nm_file ){ nm_file = m_file_name; }
	GeoPoint * get_geo_point_obj();  	
	inline PolyObject * get_geo_poly_obj() { return mp_geo_poly_obj; }
	inline MBR get_mbr() { return m_mbr; }
	void close_dbf_file();
	BOOL close_shpfile(); 
	int get_type_shp(){ return mtype_shape; }
	int get_cnt_record(){ return mcnt_record; }	
	S_FEATURE_INFO get_feature_info();	
	BOOL IsPointNearLine(const GPoint& _a, const GPoint& _b, const GPoint& _p, double _d);
	BOOL IsPointNearLine(const CGPointGP& _a, const CGPointGP& _b, const GPoint& _p, double _d);	

	/////////////////////////////////////////////////////////////////////////////////////////
public:
	BOOL load_shp( std::string path_file, CString type_map, CDC *pDC,  EGMapScale scale, CString file_name);
	BOOL search_feature( CDC *p_dc, CGRectGP &rect_need, CGPointGP &pt_selected );
	BOOL init_symbol( CDC *pDC, CDC *mp_memDC, CDC *mp_maskDC );
	void init_attribute( CDC *pDC, S_LAYER_INFO &layer_info );
	void redraw_shp( CDC *pDC, CGRectGP & rect_need, int ratio, int type_dis_map );
	void draw_shp( CDC *pDC, CGRectGP & rect_need, int ratio, int type_dis_map );
}; 

#endif // !defined(AFX_SHP_H__B6AB45BC_A808_444A_B172_EB019D8E74E4__INCLUDED_)
