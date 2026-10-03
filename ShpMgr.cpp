#include "pch.h"
#include "ShpMgr.h"

CShpMgr::CShpMgr()
{	
	
}

CShpMgr::~CShpMgr()
{
	close_shpfile();
}


void CShpMgr::close_shpfile_5_10()
{
	itr_shp itr_land_begin;
	itr_shp itr_land_end;
	
	itr_shp itr_air_begin;
	itr_shp itr_air_end;
	
	
	itr_land_begin = mmap_shp_land_5.begin();
	itr_land_end = mmap_shp_land_5.end();			
	close_shpfile_land( itr_land_begin, itr_land_end );
	
	itr_land_begin = mmap_shp_land_10.begin();
	itr_land_end = mmap_shp_land_10.end();			
	close_shpfile_land( itr_land_begin, itr_land_end );

}
void CShpMgr::close_shpfile()
{
	itr_shp itr_land_begin;
	itr_shp itr_land_end;
	
	itr_shp itr_air_begin;
	itr_shp itr_air_end;
	
	
	itr_land_begin = mmap_shp_land_5.begin();
	itr_land_end = mmap_shp_land_5.end();			
	close_shpfile_land( itr_land_begin, itr_land_end );
	
	itr_land_begin = mmap_shp_land_10.begin();
	itr_land_end = mmap_shp_land_10.end();			
	close_shpfile_land( itr_land_begin, itr_land_end );
	
	itr_land_begin = mmap_shp_land_25.begin();
	itr_land_end = mmap_shp_land_25.end();				
	itr_air_begin = mmap_shp_air_25.begin();
	itr_air_end = mmap_shp_air_25.end();
	close_shpfile_land( itr_land_begin, itr_land_end );
	close_shpfile_air( itr_air_begin, itr_air_end );
	
	itr_land_begin = mmap_shp_land_50.begin();
	itr_land_end = mmap_shp_land_50.end();				
	itr_air_begin = mmap_shp_air_50.begin();
	itr_air_end = mmap_shp_air_50.end();			
	close_shpfile_land( itr_land_begin, itr_land_end );
	close_shpfile_air( itr_air_begin, itr_air_end );
	
	itr_land_begin = mmap_shp_land_100.begin();
	itr_land_end = mmap_shp_land_100.end();				
	itr_air_begin = mmap_shp_air_100.begin();
	itr_air_end = mmap_shp_air_100.end();
	close_shpfile_land( itr_land_begin, itr_land_end );
	close_shpfile_air( itr_air_begin, itr_air_end );
				
	itr_air_begin = mmap_shp_air_200.begin();
	itr_air_end = mmap_shp_air_200.end();
	close_shpfile_air( itr_air_begin, itr_air_end );
}

void CShpMgr::close_shpfile_land( itr_shp & itr_land_begin,  itr_shp & itr_land_end )
{
	for( ; itr_land_begin != itr_land_end; ++ itr_land_begin )
	{
		vector< CShp >& vec_tmp = itr_land_begin->second;
		int cnt_vec = vec_tmp.size();
		for( int cnt_ = 0; cnt_ < cnt_vec; ++cnt_ )
		{
			vec_tmp.at( cnt_ ).close_dbf_file();	 
			vec_tmp.at( cnt_ ).close_shpfile();			
		}
		//	itr_land_begin->second.close_shpfile();
		//	itr_land_begin->second.close_dbf_file();
	} 
}

void CShpMgr::close_shpfile_air( itr_shp & itr_air_begin,  itr_shp & itr_air_end )
{ 
	for( ; itr_air_begin != itr_air_end; ++ itr_air_begin )
	{
		vector< CShp >& vec_tmp = itr_air_begin->second;
		int cnt_vec = vec_tmp.size();
		for( int cnt_ = 0; cnt_ < cnt_vec; ++cnt_ )
		{
			vec_tmp.at( cnt_ ).close_dbf_file();
			vec_tmp.at( cnt_ ).close_shpfile();			
		}
		//	itr_air_begin->second.close_shpfile();
		//	itr_air_begin->second.close_dbf_file();
	} 
}

void CShpMgr::get_shp_itr( INT nScale,
						  itr_shp & itr_land_begin, itr_shp & itr_land_end,
						  itr_shp & itr_air_begin, itr_shp & itr_air_end)
{
	switch( nScale )
	{
	case MS_1_25000:		 
	case MS_1_50000:
		{ 
			itr_land_begin = mmap_shp_land_5.begin();
			itr_land_end = mmap_shp_land_5.end();			
			break;
		}
	case MS_1_100000:
		{ 
			itr_land_begin = mmap_shp_land_10.begin();
			itr_land_end = mmap_shp_land_10.end();			
			break;
		}
	case MS_1_250000:
		{ 
			itr_land_begin = mmap_shp_land_25.begin();
			itr_land_end = mmap_shp_land_25.end();	
			
			itr_air_begin = mmap_shp_air_25.begin();
			itr_air_end = mmap_shp_air_25.end();
			break;
		}
	case MS_1_500000:	
		{ 
			itr_land_begin = mmap_shp_land_50.begin();
			itr_land_end = mmap_shp_land_50.end();			
			
			itr_air_begin = mmap_shp_air_50.begin();
			itr_air_end = mmap_shp_air_50.end();			
			break;
		}
	case MS_1_1000000:		
		{ 
			itr_land_begin = mmap_shp_land_100.begin();
			itr_land_end = mmap_shp_land_100.end();			
			
			itr_air_begin = mmap_shp_air_100.begin();
			itr_air_end = mmap_shp_air_100.end();
			break;
		}
	case MS_1_2000000:	
		{ 
			itr_land_begin = mmap_shp_land_200.begin();
			itr_land_end = mmap_shp_land_200.end();			
			
			itr_air_begin = mmap_shp_air_200.begin();
			itr_air_end = mmap_shp_air_200.end();
			break;
		}
	default: break;
	}
} 

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//		
//
/////////////////////////////////////////////////////////////////////////////////////////////////
void CShpMgr::identify_feature( CDC *pDC, CGRectGP & rect_need, CGPointGP & pt_selected, INT nScale, INT ratio )
{ 
	itr_shp itr_land_begin;
	itr_shp itr_land_end;	
	itr_shp itr_air_begin;
	itr_shp itr_air_end;
	
	get_shp_itr( nScale, itr_land_begin, itr_land_end, itr_air_begin, itr_air_end );
				
	std::vector< S_FEATURE_INFO > mvec_feature_info;
	
	for( ; itr_land_begin != itr_land_end; ++ itr_land_begin )
	{
		vector< CShp >& vec_tmp = itr_land_begin->second;
		int cnt_vec = vec_tmp.size();
		for( int cnt_ = 0; cnt_ < cnt_vec; ++cnt_ )
		{
			MBR mbr_tmp = vec_tmp.at( cnt_ ).get_mbr();
			CGRectGP rect_shp;
			rect_shp.m_R.left = mbr_tmp.xmin;
			rect_shp.m_R.right = mbr_tmp.xmax;
			rect_shp.m_R.top = mbr_tmp.ymax;
			rect_shp.m_R.bottom = mbr_tmp.ymin;
			
			if( rect_shp.PtInRect( pt_selected ) )
			{ 
				if( vec_tmp.at( cnt_ ).search_feature( pDC, rect_need, pt_selected ) )
				{
					mvec_feature_info.push_back( vec_tmp.at( cnt_ ).get_feature_info() );					
				}	
			}
		}
	} 
	
	
	if( nScale == MS_1_25000 ||
		nScale == MS_1_50000 ||
		nScale == MS_1_100000 ) return; //하늘 지도는 없으므로  	
	
	for( ; itr_air_begin != itr_air_end; ++ itr_air_begin )
	{
		vector< CShp > vec_tmp;
		vec_tmp = itr_air_begin->second;
		int cnt_vec = vec_tmp.size();
		for( int cnt_ = 0; cnt_ < cnt_vec; ++cnt_ )
		{
			
			MBR mbr_tmp = vec_tmp.at( cnt_ ).get_mbr();
			CGRectGP rect_shp;
			rect_shp.m_R.left = mbr_tmp.xmin;
			rect_shp.m_R.right = mbr_tmp.xmax;
			rect_shp.m_R.top = mbr_tmp.ymax;
			rect_shp.m_R.bottom = mbr_tmp.ymin;
			
			if( rect_shp.PtInRect( pt_selected ) )
			{ 
				if( vec_tmp.at( cnt_ ).search_feature( pDC, rect_need, pt_selected ) )
				{
					mvec_feature_info.push_back( vec_tmp.at( cnt_ ).get_feature_info() );					
				}	
			}
		} 
	}   
}


void CShpMgr::draw_shp( CDC *pDC, CGRectGP & rect_need, INT nScale, int ratio, int type_dis_map )
{
	itr_shp itr_land_begin;
	itr_shp itr_land_end;
	itr_shp itr_air_begin;
	itr_shp itr_air_end;	
	
	get_shp_itr( nScale, itr_land_begin, itr_land_end, itr_air_begin, itr_air_end );
	
	for( ; itr_land_begin != itr_land_end; ++ itr_land_begin )
	{
		vector< CShp > vec_tmp;
		vec_tmp = itr_land_begin->second;
		int cnt_vec = vec_tmp.size();
		for( int cnt_ = 0; cnt_ < cnt_vec; ++cnt_ )
		{
			if( rect_need.m_R.left > vec_tmp.at( cnt_ ).m_mbr.xmax ||
				rect_need.m_R.right < vec_tmp.at( cnt_ ).m_mbr.xmin ||
				rect_need.m_R.top < vec_tmp.at( cnt_ ).m_mbr.ymin ||
				rect_need.m_R.bottom > vec_tmp.at( cnt_ ).m_mbr.ymax )
				continue;
			
			
			vec_tmp.at( cnt_ ).draw_shp( pDC, rect_need, ratio, type_dis_map );
			//vec_tmp.at( cnt_ ).draw_dbf( p_dc );
		}
		vec_tmp.clear();
		//itr_land_begin->second.draw_shp( p_dc, rect_need );
	}
	
	if( nScale == MS_1_25000 ||
		nScale == MS_1_50000 ||
		nScale == MS_1_100000 ) return; //하늘 지도는 없으므로  	
	
	for( ; itr_air_begin != itr_air_end; ++ itr_air_begin )
	{
		vector< CShp > vec_tmp;
		vec_tmp = itr_air_begin->second;
		int cnt_vec = vec_tmp.size();
		for( int cnt_ = 0; cnt_ < cnt_vec; ++cnt_ )
		{
			if( rect_need.m_R.left > vec_tmp.at( cnt_ ).m_mbr.xmax ||
				rect_need.m_R.right < vec_tmp.at( cnt_ ).m_mbr.xmin ||
				rect_need.m_R.top < vec_tmp.at( cnt_ ).m_mbr.ymin ||
				rect_need.m_R.bottom > vec_tmp.at( cnt_ ).m_mbr.ymax )
				continue;
			
			
			vec_tmp.at( cnt_ ).draw_shp( pDC, rect_need, ratio, type_dis_map );
			//	vec_tmp.at( cnt_ ).draw_dbf( p_dc );
		}
		vec_tmp.clear();
	}  
}

void CShpMgr::load_shp( unsigned short id_file, std::string path_file, CString type_map, CDC *pDC, EGMapScale nScale )
{			
	//파일명 추출하기
	CHAR drive[_MAX_DRIVE];	CHAR dir[_MAX_DIR];	CHAR fname[_MAX_FNAME];	CHAR ext[_MAX_EXT];
	_splitpath( path_file.c_str(), drive, dir, fname, ext);	  	 
	
	short order = CMapDataMgr::get_order( nScale, fname ); 
	if( order == 0 ) return;
	
	
	if( TRUE == shp_tmp.load_shp( path_file, type_map, pDC, nScale, fname ) )
	{ 
		itr_shp itr_begin;
		itr_shp itr_end;
		
		if( type_map == TYPE_EARTH_200 ) 
		{
			itr_begin = mmap_shp_land_200.find( order );
			itr_end = mmap_shp_land_200.end();
		}
		else if( type_map == TYPE_EARTH_100 ) 
		{
			itr_begin = mmap_shp_land_100.find( order );
			itr_end = mmap_shp_land_100.end();
		}
		else if( type_map == TYPE_EARTH_50 ) 
		{
			itr_begin = mmap_shp_land_50.find( order );
			itr_end = mmap_shp_land_50.end();
		}
		else if( type_map == TYPE_EARTH_25 ) 
		{
			itr_begin = mmap_shp_land_25.find( order );
			itr_end = mmap_shp_land_25.end();
		}
		else if( type_map == TYPE_EARTH_10 ) 
		{
			itr_begin = mmap_shp_land_10.find( order );
			itr_end = mmap_shp_land_10.end();
		}
		else if( type_map == TYPE_EARTH_5 ) 
		{
			itr_begin = mmap_shp_land_5.find( order );
			itr_end = mmap_shp_land_5.end();
		}
		//else if( type_map == TYPE_AIR_200 ) 
		//{
		//	itr_begin = mmap_shp_air_200.find( order );
		//	itr_end = mmap_shp_air_200.end();
		//}
		//else if( type_map == TYPE_AIR_100 ) 
		//{
		//	itr_begin = mmap_shp_air_100.find( order );
		//	itr_end = mmap_shp_air_100.end();
		//}
		//else if( type_map == TYPE_AIR_50 ) 
		//{
		//	itr_begin = mmap_shp_air_50.find( order );
		//	itr_end = mmap_shp_air_50.end();
		//}
		//else if( type_map == TYPE_AIR_25 ) 
		//{
		//	itr_begin = mmap_shp_air_25.find( order );
		//	itr_end = mmap_shp_air_25.end();
		//}	
		
		if( itr_begin != itr_end )
			itr_begin->second.push_back( shp_tmp );
		else
		{
			vector< CShp > vec_tmp;
			vec_tmp.push_back( shp_tmp );
			
			if( type_map == TYPE_EARTH_200 ) 
				mmap_shp_land_200[ order ] = vec_tmp;
			else if( type_map == TYPE_EARTH_100 ) 
				mmap_shp_land_100[ order ] = vec_tmp;
			else if( type_map == TYPE_EARTH_50 )  
				mmap_shp_land_50[ order ] = vec_tmp;
			else if( type_map == TYPE_EARTH_25 )  
				mmap_shp_land_25[ order ] = vec_tmp;
			else if( type_map == TYPE_EARTH_10 )  
				mmap_shp_land_10[ order ] = vec_tmp;
			else if( type_map == TYPE_EARTH_5 ) 	 
				mmap_shp_land_5[ order ] = vec_tmp;
			//else if( type_map == TYPE_AIR_200 ) 	 
			//	mmap_shp_air_200[ order ] = vec_tmp;
			//else if( type_map == TYPE_AIR_100 ) 	 
			//	mmap_shp_air_100[ order ] = vec_tmp;
			//else if( type_map == TYPE_AIR_50 ) 	 
			//	mmap_shp_air_50[ order ] = vec_tmp;
			//else if( type_map == TYPE_AIR_25 ) 	 
			//	mmap_shp_air_25[ order ] = vec_tmp;			
		}
	}  
}

