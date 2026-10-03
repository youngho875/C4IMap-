#if !defined(AFX_DBF_H__750070D3_6DD6_4061_AC9F_23A307F38990__INCLUDED_)
#define AFX_DBF_H__750070D3_6DD6_4061_AC9F_23A307F38990__INCLUDED_

#pragma once

#include "MapDataMgr.h"
#include "../inc/define.h"
#include "../inc/define_struct.h"
#include "GeoDC/GDCGeo.h"
#include <string>
#include <vector>

typedef struct s_label
{
	double geo_x;
	double geo_y;
	CString label;
	int type_annotation;
	int annotation_int;  
	CString annotation_str;  
}SLABEL;

static LOGFONT m_dbf_font;

class CShp;
class CDbf
{

public:
	CDbf();
	virtual ~CDbf();	
	
private:	
	char m_dbf_path[_MAX_PATH];	
	
	DBF_FILEHEADER mf_header;		// DBF파일 헤더
	DBF_FILEDDESCRIPTOR* mp_field;	// DBF 필드 헤더 배열
	
	int mcnt_field;
	BOOL m_is_load;
	BOOL m_visible_dbf;	
	
	//CFont m_Font;

	SLABEL * mp_label;
	int midx_field;
	CShp * mp_shp;
	
	void remove_space( char * rec, int length );
	BOOL calculate_label_point();	
		
public:	 	
//	void load_dbf( std::string path_file );
//	void draw_dbf( CGDCGeo * p_dc );
	//void draw_dbf( CGDCGeo * p_dc, CGRectGP & rect_need );
	void draw_dbf( CDC *p_dc, CGRectGP &rect_need, CString scale );

	void close_dbf_file();
	void set_idx_field( int idx ){ midx_field = idx; }
	int get_idx_field() { return midx_field; }
	
	void set_visible_dbf( BOOL visible ){ m_visible_dbf = visible; }	
	BOOL get_visible_dbf() { return m_visible_dbf; }
 
	BOOL is_load_dbf();

	void connect_shp( CShp * p_shp ){ mp_shp = p_shp;}

	void load_dbf( std::string path_file, CString type_map );
	void read_dbf( std::string path_file, std::vector< s_feature_value > & vec_field_value, int idx_row );
};

#endif // !defined(AFX_DBF_H__750070D3_6DD6_4061_AC9F_23A307F38990__INCLUDED_)
