#if !defined(AFX_SHPMGR_H__CAF66C82_B58D_4959_BDB7_F3E41619ED92__INCLUDED_)
#define AFX_SHPMGR_H__CAF66C82_B58D_4959_BDB7_F3E41619ED92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
 
#include "Shp.h"
 
#endif // _MSC_VER > 1000
 
class CShpMgr  
{

public:
	CShpMgr();
	virtual ~CShpMgr();

private:
 
	//우선순위, 해당 백터파일(레이어가 아니다)
	map< unsigned short, vector<CShp> > mmap_shp_land_200;	
	map< unsigned short, vector<CShp> > mmap_shp_land_100;
	map< unsigned short, vector<CShp> > mmap_shp_land_50;
	map< unsigned short, vector<CShp> > mmap_shp_land_25;
	map< unsigned short, vector<CShp> > mmap_shp_land_10;
	map< unsigned short, vector<CShp> > mmap_shp_land_5;
	map< unsigned short, vector<CShp> > mmap_shp_air_200;
	map< unsigned short, vector<CShp> > mmap_shp_air_100;
	map< unsigned short, vector<CShp> > mmap_shp_air_50;
	map< unsigned short, vector<CShp> > mmap_shp_air_25;
	typedef map< unsigned short, vector< CShp > >::iterator itr_shp;
 
 
private:
	void get_shp_itr( INT nScale, itr_shp & itr_land_begin, itr_shp & itr_land_end ,itr_shp & itr_air_begin, itr_shp & itr_air_end );
	void close_shpfile_land( itr_shp & itr_land_begin,  itr_shp & itr_land_end );
	void close_shpfile_air( itr_shp & itr_air_begin,  itr_shp & itr_air_end );


public:
 	void close_shpfile_5_10();


	//ID_FILE CHECK
	map< short, short > mmap_id;
	inline void add_id_file( unsigned short id_file ){ mmap_id[ id_file ] = id_file;}
	inline BOOL search_id_file( unsigned short id_file ) const
	{
		if( mmap_id.find( id_file ) != mmap_id.end() )	//존재한다면 TRUE
			return TRUE;
		else
			return FALSE;
	}
 	
	CShp shp_tmp;	
	void close_shpfile();

public:
	void identify_feature( CDC * p_dc, CGRectGP & rect_need, CGPointGP & pt_selected, INT nScale, INT ratio );
	void draw_shp( CDC * p_dc, CGRectGP & rect_need, INT nScale, int ratio, int type_dis_map );
	void load_shp( unsigned short id_file, std::string path_file, CString type_map, CDC * p_dc, EGMapScale nScale );
};
 
#endif // !defined(AFX_SHPMGR_H__CAF66C82_B58D_4959_BDB7_F3E41619ED92__INCLUDED_)
