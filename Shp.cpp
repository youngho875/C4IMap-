#include "pch.h"
#include "Shp.h" 
#include "GeoDC/GRgn.h"
#include "shpcommon.h"
//#include "map/mapdisp/MapDispRes.h"
//#include "map/mapcomlib/gmapinfo.h"


CShp::CShp()
{	
	m_mbr.xmin = 0;
	m_mbr.ymin = 0;
	m_mbr.xmax = 0;
	m_mbr.ymax = 0;	
	
	mtype_shape = SHPT_NULL;	
	mp_geo_point = NULL;
	mp_geo_poly_obj = NULL;
	mb_open_file = FALSE;
	mcnt_record = 0;
	
	m_dbf.connect_shp(this);
	m_idx_record_selected = -1;	  
}

//#define SAFE_DELETE(x) if(x!=NULL){ delete x; x=NULL;};

BOOL CShp::close_shpfile()
{
	if (mp_geo_point == NULL && mp_geo_poly_obj == NULL)
		return TRUE;
	//메모리 해제 내용을 필히 넣어라!!	
	switch(mtype_shape)
	{
	case SHPT_POINT:
	case SHPT_POINTZ:
	case SHPT_POINTM: 
	case SHPT_MULTIPOINTM:
		{
			if( mcnt_record > 0 )
				delete [] mp_geo_point;
			mp_geo_point = NULL;	 
			break;
		}		
	case SHPT_ARC :
	case SHPT_ARCZ:
	case SHPT_ARCM:	
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
	case SHPT_POLYGONM:
		{
			for( int cnt = 0; cnt < mcnt_record; ++cnt )
			{ 
				if( mp_geo_poly_obj[cnt].m_nNumParts > 0 )	 
					delete [] mp_geo_poly_obj[cnt].m_pParts;
				mp_geo_poly_obj[cnt].m_pParts = NULL;
			 
				if( mp_geo_poly_obj[cnt].m_nNumPoints > 0 ) 
					delete [] mp_geo_poly_obj[cnt].m_pPoints;	
				mp_geo_poly_obj[cnt].m_pPoints = NULL;
		 
			}		
			delete [] mp_geo_poly_obj;	
			mp_geo_poly_obj = NULL;
			break;	
		}			
	case SHPT_MULTIPOINT:
		{
			for( int cnt = 0; cnt < mcnt_record; ++cnt )
			{ 
				if( mp_geo_poly_obj[cnt].m_nNumPoints > 0 )
					delete [] mp_geo_poly_obj[cnt].m_pPoints;
			
				mp_geo_poly_obj[cnt].m_pPoints = NULL;
			}
			delete [] mp_geo_poly_obj;
			mp_geo_poly_obj = NULL;
			break;
		}
	default: break;
	}		
 	return TRUE;
} 

CShp::~CShp()
{	
	//if( TRUE == mb_open_file )
	//{
	//	BOOL result = close_shpfile();
	//	if( TRUE == result )
	//		TRACE(_T("#### info  : succees close shpfile	\n"));
	//	else
	//		TRACE(_T("#### info  : fail close shpfile	\n"));
	//}	
}

void CShp::swap_word( int length, void * p_word )
{	 
	unsigned char	value_site_tmp;	
	for( int idx = 0; idx < length/2; ++idx )
	{
		value_site_tmp = ((unsigned char *) p_word)[idx];
		((unsigned char *) p_word)[idx] = ((unsigned char *) p_word)[length-idx-1];
		((unsigned char *) p_word)[length-idx-1] = value_site_tmp;
	}
}

/*
BOOL CShp::load_shp( unsigned short no_layer, 
CString id_layer, 
unsigned short id_file, 
std::string path_file,
CString type_map, CGDCGeo * p_dc,  EGMapScale scale  )
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CShp::load_shp( std::string path_file, CString type_map, CDC *pDC,  EGMapScale scale, CString file_name  )
{ 		
	FILE * fp_shp = NULL;
	FILE * fp_shx = NULL;
	
	m_file_name = file_name;
	int site_tmp; 
	int length_filename;	
	
	strcpy(m_shp_path, path_file.c_str() );	 
	length_filename = strlen(m_shp_path) - 4;	 
	
	for(int cnt = 0; cnt<length_filename; ++cnt)
	{ 
		m_shx_path[cnt] = m_shp_path[cnt];
		m_dbf_path[cnt] = m_shp_path[cnt];
		if(cnt == length_filename-1)
		{
			m_shx_path[cnt+1]=0;
			m_dbf_path[cnt+1]=0;
			strcat(m_shx_path,".shx" );  
			strcat(m_dbf_path,".dbf" );
		}		
	}	 	
	if( (fp_shp  = fopen( m_shp_path, "rb" )) == NULL ) // C4996
		return FALSE;
	
	if( (fp_shx  = fopen( m_shx_path, "rb" )) == NULL )  
	{ 
		fclose(fp_shp);
		return FALSE;
	} 
	
	fseek(fp_shx, 24L, SEEK_SET);
	fread(&site_tmp, 4, 1, fp_shx);
	swap_word(4, &site_tmp);				// Byte Order Changing
	mn_size_file = site_tmp * 2;			// get file size
	mcnt_record = (mn_size_file - 100)/8;	//get a number of records
	fseek(fp_shp, 32L, SEEK_SET);
	fread(&mtype_shape, 4, 1, fp_shp); 
	fread(&m_mbr, sizeof(MBR), 1, fp_shp);
	
	switch(mtype_shape)
	{
	case SHPT_POINT:
	case SHPT_POINTZ:
	case SHPT_POINTM: 
	case SHPT_MULTIPOINTM:
		{
			if( mcnt_record > 0 )
				mp_geo_point = new GeoPoint[mcnt_record];
			for( int i = 0; i < mcnt_record; ++i)
			{
				// SHX에서는 레코드 정보의 첫 4바이트에 해당하는 feature의 좌표정보가 시작되는
				// 바이트 수 를 알려 준다.
				fseek(fp_shx, 100 + i * 8, SEEK_SET);
				fread(&site_tmp, 4, 1, fp_shx);
				swap_word(4, &site_tmp);
				site_tmp=site_tmp * 2;
				fseek(fp_shp, site_tmp + 12, SEEK_SET);
				fread((GeoPoint*)&mp_geo_point[i], 16, 1, fp_shp);
			}
			break;
		}		
	case SHPT_ARC :
	case SHPT_ARCZ:
	case SHPT_ARCM:	
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
	case SHPT_POLYGONM:
		{
			if( mcnt_record > 0 )
				mp_geo_poly_obj = new PolyObject[mcnt_record];			
			for( int i = 0; i < mcnt_record; ++i)
			{
				fseek(fp_shx, 100+i*8, SEEK_SET);
				fread(&site_tmp, 4, 1, fp_shx);
				swap_word(4, &site_tmp);
				site_tmp=site_tmp*2;
				
				fseek(fp_shp, site_tmp+12, SEEK_SET);
				fread((MBR*)&mp_geo_poly_obj[i].m_MBR, 32, 1, fp_shp);	//get MBR
				fread(&mp_geo_poly_obj[i].m_nNumParts, 4, 1, fp_shp);		//get parts number
				fread(&mp_geo_poly_obj[i].m_nNumPoints, 4, 1, fp_shp);	 //get points number
				
				if( mp_geo_poly_obj[i].m_nNumParts > 0 )
				{					
					mp_geo_poly_obj[i].m_pParts = NULL;
					mp_geo_poly_obj[i].m_pParts = new int[mp_geo_poly_obj[i].m_nNumParts];
					mp_geo_poly_obj[i].m_pPoints = NULL;
					mp_geo_poly_obj[i].m_pPoints = new GeoPoint[mp_geo_poly_obj[i].m_nNumPoints];
				}
				
				for(int j=0;j<mp_geo_poly_obj[i].m_nNumParts;++j)
					fread(&mp_geo_poly_obj[i].m_pParts[j], 4, 1, fp_shp);				
				
				for(int j=0;j<mp_geo_poly_obj[i].m_nNumPoints;++j)
					fread(&mp_geo_poly_obj[i].m_pPoints[j], 16, 1, fp_shp);
			}			
			break;
		}
		
	case SHPT_MULTIPOINT:
		if( mcnt_record > 0 )
			mp_geo_poly_obj = new PolyObject[mcnt_record];			
		for(int  i=0;i<mcnt_record;++i)
		{
			fseek(fp_shx, 100+i*8, SEEK_SET);
			fread(&site_tmp, 4, 1, fp_shx);
			swap_word(4, &site_tmp);
			site_tmp=site_tmp*2;			
			fseek(fp_shp, site_tmp+12, SEEK_SET);
			fread((MBR*)&mp_geo_poly_obj[i].m_MBR, 32, 1, fp_shp);	//get  MBR
			fread(&mp_geo_poly_obj[i].m_nNumPoints, 4, 1, fp_shp);		// get points number
			
			if( mp_geo_poly_obj[i].m_nNumPoints > 0 )
			{
				mp_geo_poly_obj[i].m_pPoints = NULL;
				mp_geo_poly_obj[i].m_pPoints = new GeoPoint[mp_geo_poly_obj[i].m_nNumPoints];
			}
			
			for(int j=0;j<mp_geo_poly_obj[i].m_nNumPoints;++j)
				fread(&mp_geo_poly_obj[i].m_pPoints[j], 16, 1, fp_shp);
		}	
		break;
	}	
	fclose(fp_shx);	
	fclose(fp_shp);

	fp_shx = NULL;
	fp_shp = NULL;
	
	//파일명 추출하기
	//CHAR drive[_MAX_DRIVE];	CHAR dir[_MAX_DIR];	CHAR fname[_MAX_FNAME];	CHAR ext [_MAX_EXT];
	//_splitpath( m_shp_path, drive, dir, fname, ext);	 
	//m_file_name = fname;	
	
	mb_open_file = TRUE;
	
	//속성 설정 
	m_scale = scale;

	S_LAYER_INFO layer_info; 
	CMapDataMgr::get_vector_lyaer_info( m_file_name, (short)m_scale, layer_info );
	if( layer_info.point_size == "99" )
	{
		//if( !m_dbf.is_load_dbf() )		
	//	if(1)
	//	{
			//m_dbf.set_idx_field( 3 );
			m_dbf.set_idx_field( atoi(layer_info.polyline_width) );
			m_dbf.load_dbf( m_dbf_path, type_map );					
	//	}	
	//	m_dbf.set_visible_dbf( TRUE );	
	}		
	//속성 설정 
	m_scale = scale;
	return TRUE;
}

BOOL CShp::search_feature( CDC *pDC, CGRectGP & rect_need, CGPointGP & pt_selected )
{	
	CCoord m_Coord;
	CCoordMgr*	m_pCoordMgr = GP_COORDMGR;

	S_LAYER_INFO layer_info;	
	CMapDataMgr::get_vector_lyaer_info( m_file_name, (short)m_scale, layer_info );
	S_MAP_INFO mapInfo = CMapDataMgr::get_current_map_info();	
	if( layer_info.nm_layer_kor == "배경" ||
		layer_info.nm_layer_kor == "지명" ||
		layer_info.nm_layer_kor == "바닷물" ||
		layer_info.nm_layer_kor == "등고선" ||
		layer_info.nm_layer_kor == "도로" ||
		layer_info.nm_layer_kor == "행정지구"  )
		return FALSE;
	
	if( layer_info.visible == 0 )
		return FALSE;
	
	switch(mtype_shape)
	{ 		
	case SHPT_POINT:
	case SHPT_POINTZ:
	case SHPT_POINTM:
	case SHPT_MULTIPOINTM:
		{ 						
			for( int cnt = 0; cnt < mcnt_record ; ++cnt )
			{    
				CSize size;
				CGSizeGP pt_geo;
				CPoint LpTopLeft, LpBottomRight;
				CGPointGP gpTopLeft, gpBottomRight;	
				CPoint LpWinOrg;	// 논리좌표의 윈도우 원점
				double left, top, right, bottom;

				size.cx = 8;
				size.cy = 8;
				//p_dc->DPtoGeoP( &pt_geo, &size );	
					
				/////////////////////////////////////////////////////////////////
				//	DPtoGeoP(size, size)
				/////////////////////////////////////////////////////////////////
				LpWinOrg = pDC->GetWindowOrg();

				LpTopLeft.x = LpWinOrg.x;
				LpTopLeft.y = LpWinOrg.y;

				LpBottomRight.x = LpWinOrg.x + size.cx;
				LpBottomRight.y = LpWinOrg.y + size.cy;
	
				m_pCoordMgr->ScreenXY2WorldXY(LpTopLeft.x, LpTopLeft.y, &left, &top);
				m_pCoordMgr->ScreenXY2WorldXY(LpBottomRight.x, LpBottomRight.y, &right, &bottom);
				m_Coord.x_y_to_degrees_lat_long(left, top, &gpTopLeft.m_P.lat, &gpTopLeft.m_P.lon);
				m_Coord.x_y_to_degrees_lat_long(right, bottom, &gpBottomRight.m_P.lat, &gpBottomRight.m_P.lon);

				////////////////////////////////////////////////////////////////// end

				pt_geo.m_S.clon = gpBottomRight.m_P.lon - gpTopLeft.m_P.lon;
				pt_geo.m_S.clat = gpBottomRight.m_P.lat - gpTopLeft.m_P.lat;
				
				CRgn rgn_;
				//CGRgn rgn_( p_dc );
				rgn_.CreateRectRgn(mp_geo_point[cnt].x - pt_geo.m_S.clon, 
					mp_geo_point[cnt].y - pt_geo.m_S.clat,
					mp_geo_point[cnt].x + pt_geo.m_S.clon,
					mp_geo_point[cnt].y + pt_geo.m_S.clat	);				
				
				double lx, ly;
				CPoint pt;
				m_Coord.degrees_lat_long_to_x_y(pt_selected.m_P.lat, pt_selected.m_P.lon, &lx, &ly);
				m_pCoordMgr->WorldXY2ScreenXY(lx, ly, &pt.x, &pt.y);

				//BOOL is_visible = rgn_.PtInRegion( pt_selected );
				BOOL is_visible = rgn_.PtInRegion(pt);
				if( is_visible )
				{				
					S_SELECTED_FEATURE  feature;
					feature.type_geo = "POINT";
					feature.id_selected_feature = CMapDataMgr::get_feature_cnt(); //선택된 feature 수 
					feature.scale = m_scale;
					feature.idx_row = cnt; 				 
					feature.nm_file_kor = layer_info.nm_layer_kor;
					CDbf dbf_tmp; 				
					dbf_tmp.read_dbf( m_dbf_path, feature.vec_field_value, feature.idx_row );
					
					
					CMapDataMgr::add_feature( feature );
					
					m_is_selected = TRUE; 
					CBrush brush;
					brush.CreateSolidBrush(RGB(10,0,220)); 
					pDC->FrameRgn( &rgn_, &brush, 4, 4 );					
					
					return m_is_selected;						
				}
			}
			break;
		}
		
	case SHPT_ARC :
	case SHPT_ARCZ:
	case SHPT_ARCM:	
		{
			//전체 레코드를 검색한다. 
			for( int cnt = 0; cnt < mcnt_record; ++cnt )
			{
				if( rect_need.m_R.left > mp_geo_poly_obj[cnt].m_MBR.xmax  ||
					rect_need.m_R.right < mp_geo_poly_obj[cnt].m_MBR.xmin ||
					rect_need.m_R.top < mp_geo_poly_obj[cnt].m_MBR.ymin ||
					rect_need.m_R.bottom > mp_geo_poly_obj[cnt].m_MBR.ymax )
					continue;
								
				int NumPoints = mp_geo_poly_obj[cnt].m_nNumPoints;
				int NumParts = mp_geo_poly_obj[cnt].m_nNumParts; 
				
				double lx, ly;
				CGPointGP* pTempPoint = new CGPointGP[NumPoints]; 				
				CPoint* p_point	= new CPoint[NumPoints];	
				//CPoint* p_point1	= new CPoint[NumPoints];
				
				int* pVertex = new int[NumParts];				
				CGPointGP pt_geo;
				
				for( int cnt_y = 0; cnt_y < NumPoints; ++cnt_y )
				{
					pTempPoint[cnt_y].m_P.lon = mp_geo_poly_obj[cnt].m_pPoints[cnt_y].x;
					pTempPoint[cnt_y].m_P.lat = mp_geo_poly_obj[cnt].m_pPoints[cnt_y].y;					
					//pDC->GeoPtoLP( &p_point[cnt_y], &pTempPoint[cnt_y]);	

					m_Coord.degrees_lat_long_to_x_y (pTempPoint[cnt_y].m_P.lat, pTempPoint[cnt_y].m_P.lon, &lx, &ly); 
					m_pCoordMgr->WorldXY2ScreenXY(lx, ly, &p_point[cnt_y].x, &p_point[cnt_y].y);
					

				}
				
				//파트별 버택스 수 구하기
				for(int cnt_z = 0; cnt_z < NumParts;  ++cnt_z )	
				{
					if(cnt_z == NumParts - 1)	
						pVertex[cnt_z] = NumPoints - mp_geo_poly_obj[cnt].m_pParts[cnt_z];
					else 
						pVertex[cnt_z] = mp_geo_poly_obj[cnt].m_pParts[cnt_z+1] - mp_geo_poly_obj[cnt].m_pParts[cnt_z];
				}				
				
				for(int cnt_s = 0; cnt_s < NumParts; ++cnt_s )
				{
					int cnt_vertex = 0; 
					cnt_vertex = pVertex[cnt_s];
					CGPointGP* pt_ = NULL;
					pt_ = new CGPointGP[ cnt_vertex ]; 		
					
					//해당 파트의 포인트들을 추출한다. 					
					BOOL is_near_line = FALSE;
					for( int cnt_d = 0; cnt_d < cnt_vertex - 1; ++cnt_d )
					{
						int  p_part = mp_geo_poly_obj[cnt].m_pParts[ cnt_s ] + cnt_d;						
						pt_[cnt_d].m_P.lon =  pTempPoint[ (p_part) ].m_P.lon;
						pt_[cnt_d].m_P.lat =  pTempPoint[ (p_part) ].m_P.lat;
						
						pt_[cnt_d + 1].m_P.lon =  pTempPoint[ (p_part + 1) ].m_P.lon;
						pt_[cnt_d + 1].m_P.lat =  pTempPoint[ (p_part + 1) ].m_P.lat;
						
						GPoint pt_g; 
						pt_g.x = pt_selected.m_P.lon;
						pt_g.y = pt_selected.m_P.lat;						
						
						if( is_near_line == FALSE )
						{							
							//is_near_line = IsPointNearLine(pt_[cnt_d], pt_[cnt_d + 1], pt_g, (double)4/mapInfo.lMapScale/mapInfo.iMapRatio);
							is_near_line = IsPointNearLine(pt_[cnt_d], pt_[cnt_d + 1], pt_g, (double)mapInfo.lMapScale/mapInfo.iMapRatio/60.0);
							//is_near_line = IsPointNearLine(pt_[cnt_d], pt_[cnt_d + 1], pt_g, 0.02);						

						}
					} 
					
					if( is_near_line )
					{		
						m_idx_record_selected = cnt;
						S_SELECTED_FEATURE  feature;
						feature.type_geo = "POLYLINE";
						feature.id_selected_feature = CMapDataMgr::get_feature_cnt();	//선택된 feature 수 
						feature.scale = m_scale;
						feature.idx_row = cnt;
						feature.nm_file_kor = layer_info.nm_layer_kor;
						
						CDbf dbf_tmp; 
						dbf_tmp.read_dbf( m_dbf_path, feature.vec_field_value, feature.idx_row );
						
						CMapDataMgr::add_feature( feature );
						
						//	CGRgn rgn_( p_dc );
						//	rgn_.CreatePolygonRgn( pt_, cnt_vertex, ALTERNATE );						
						//	CBrush brush;
						//	brush.CreateSolidBrush(RGB(5,5,120)); 
						//	p_dc->FrameRgn( &rgn_, &brush, 1, 1 );	
						//	p_dc->SelectObject( &brush );
						
						CSize size_pen( 5, 1 );					 
						pDC->DPtoLP( &size_pen );
						CPen	m_pen;
						m_pen.CreatePen( 0, size_pen.cx, RGB( 255, 0, 10 ));	
						CPen  *pOldPen;       	  
						
						pOldPen = (CPen *)pDC->SelectObject(&m_pen); 						
						::PolyPolyline( pDC->m_hDC, p_point, (DWORD*)pVertex, NumParts);
						pDC->SelectObject( pOldPen); 
						
						
						delete[] pt_;
						delete[] pTempPoint;
						delete[] p_point;
						delete[] pVertex;
						
						m_is_selected = TRUE; 
						return m_is_selected;					
					}  
					delete[] pt_;
				}
				delete[] pTempPoint;
				delete[] pVertex;
				delete[] p_point;
			}
		} break;
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
	case SHPT_POLYGONM:
		{
			//전체 레코드를 검색한다. 
			for( int cnt = 0; cnt < mcnt_record; ++cnt )
			{
				if( rect_need.m_R.left > mp_geo_poly_obj[cnt].m_MBR.xmax  ||
					rect_need.m_R.right < mp_geo_poly_obj[cnt].m_MBR.xmin ||
					rect_need.m_R.top < mp_geo_poly_obj[cnt].m_MBR.ymin ||
					rect_need.m_R.bottom > mp_geo_poly_obj[cnt].m_MBR.ymax )
					continue;
								
				int NumPoints = mp_geo_poly_obj[cnt].m_nNumPoints;
				int NumParts = mp_geo_poly_obj[cnt].m_nNumParts; 
				
				CGPointGP* pTempPoint = new CGPointGP[NumPoints]; 
				int* pVertex = new int[NumParts];
				
				double lx, ly;
				CPoint* p_point	= new CPoint[NumPoints];	
				//CPoint* p_point1	= new CPoint[NumPoints];	
				CGPointGP pt_geo;
								
				for( int cnt_y = 0; cnt_y < NumPoints; ++cnt_y )
				{
					pTempPoint[cnt_y].m_P.lon = mp_geo_poly_obj[cnt].m_pPoints[cnt_y].x;
					pTempPoint[cnt_y].m_P.lat = mp_geo_poly_obj[cnt].m_pPoints[cnt_y].y;
					//pDC->GeoPtoLP( &p_point[cnt_y], &pTempPoint[cnt_y]);	

					m_Coord.degrees_lat_long_to_x_y (pTempPoint[cnt_y].m_P.lat, pTempPoint[cnt_y].m_P.lon, &lx, &ly); 
					m_pCoordMgr->WorldXY2ScreenXY(lx, ly, &p_point[cnt_y].x, &p_point[cnt_y].y);

				} 			
				
				//파트별 버택스 수 구하기
				for(int cnt_z = 0; cnt_z < NumParts;  ++cnt_z )	
				{
					// 마지막 Part의 버텍스 수는 전체 버텍스 수에서 마지막 Part의 인덱스를 빼줌
					if(cnt_z == NumParts - 1)	
						pVertex[cnt_z] = NumPoints - mp_geo_poly_obj[cnt].m_pParts[cnt_z];
					// 버텍스는 다음 Part의 인덱스와 현재 Part의 인덱스 차와 같음
					else 
						pVertex[cnt_z] = mp_geo_poly_obj[cnt].m_pParts[cnt_z+1] - mp_geo_poly_obj[cnt].m_pParts[cnt_z];
				}				
				
				//파트의 수만큼 루프를 돈다
				for(int cnt_s = 0; cnt_s < NumParts; ++cnt_s )
				{
					int cnt_vertex = 0;
					cnt_vertex  = pVertex[cnt_s];
					CGPointGP * pt_ = NULL;
					pt_ = new CGPointGP[ cnt_vertex ]; 		
					
					//해당 파트의 포인트들을 추출한다. 
					for( int cnt_d = 0; cnt_d < cnt_vertex; ++cnt_d )
					{
						int  p_part = mp_geo_poly_obj[cnt].m_pParts[ cnt_s ] + cnt_d;						
						pt_[cnt_d].m_P.lon =  pTempPoint[ (p_part) ].m_P.lon;
						pt_[cnt_d].m_P.lat =  pTempPoint[ (p_part) ].m_P.lat;
					}					
					
					//CGRgn rgn_( p_dc );
					CGRgn rgn_;
					rgn_.CreatePolygonRgn( pt_, cnt_vertex, ALTERNATE ); 
					BOOL is_visible = rgn_.PtInRegion( pt_selected );
					//포함된다면 
					if( is_visible )
					{										  
						m_idx_record_selected = cnt;
						m_idx_part_selected = cnt_s;	//파트단위가 아니라 feature단위로 drawing할 것이므로 무의미
						
						
						S_SELECTED_FEATURE  feature;
						feature.type_geo = "POLYGON";
						feature.id_selected_feature = CMapDataMgr::get_feature_cnt();	//선택된 feature 수 
						feature.scale = m_scale;
						feature.idx_row = cnt;
						feature.nm_file_kor = layer_info.nm_layer_kor;
												
						/*
						for( int cnt_y = 0; cnt_y < NumPoints; ++cnt_y )	//FEATURE의 POINT를 전부 입력한다.
						{
						CCoord coord;
						coord.X = pTempPoint[cnt_y].m_P.lon;
						coord.Y = pTempPoint[cnt_y].m_P.lat;
						feature.vec_vertax.push_back( coord );								
						}
						for( int cnt_z = 0; cnt_z < NumParts; ++cnt_z )	
						{ 
						feature.vec_part.push_back( pVertex[cnt_z] );							
						}
						*/
						
						CDbf dbf_tmp; 
						dbf_tmp.read_dbf( m_dbf_path, feature.vec_field_value, feature.idx_row );
						
						CMapDataMgr::add_feature( feature );
						
						
						CSize size_pen( 5, 1 );					 
						pDC->DPtoLP( &size_pen );
						CPen	m_pen; CPen  *pOldPen;       	
						m_pen.CreatePen( 0, size_pen.cx, RGB( 16, 220, 50 ));	
						
						CBrush brush; CBrush  *pOldbrush;   
						brush.CreateSolidBrush( RGB(210,210,210)); 
						
						pOldPen = (CPen *)pDC->SelectObject(&m_pen); 
						pOldbrush = (CBrush *)pDC->SelectObject(&brush); 			
						::PolyPolygon( pDC->m_hDC, p_point, pVertex, NumParts);
						//::PolyPolyline( p_dc->m_hDC, p_point, (DWORD*)pVertex, NumParts);
						pDC->SelectObject( pOldPen); 
						pDC->SelectObject( pOldbrush); 
						
						
						delete[] pt_; 
						delete[] pTempPoint;
						delete[] pVertex;
						delete[] p_point;
						m_is_selected = TRUE; 
						//	CBrush brush;
						//	brush.CreateSolidBrush(RGB(5,120,0)); 
						//	p_dc->FrameRgn( &rgn_, &brush, 4, 4 );	
						
						return m_is_selected;					
					}  
					delete[] pt_;
				}
				delete[] pTempPoint;
				delete[] pVertex;
				delete[] p_point;
			}
		} break;
		
		
	default: break;
	}	 
	m_is_selected = TRUE;
	return m_is_selected;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CShp::IsPointNearLine(const GPoint& _a, const GPoint& _b, const GPoint& _p, double _d)
{
	// check distance from the point to the line by x and by y
	double dx = _a.x - _b.x;
	double dy = _a.y - _b.y;
	double r = sqrt(dx*dx + dy*dy);
	
	// compute coordinates relative to the origin of the line
	double x1 = _p.x - _b.x;
	double y1 = _p.y - _b.y;
	
	// compute coordinates relative to the line
	double x2 = (x1*dx + y1*dy)/r;
	double y2 = (-x1*dy + y1*dx)/r;
	
	return (x2>=0) && (x2<=r) && (y2<=_d) && (y2 >= -_d);
}

BOOL CShp::IsPointNearLine(const CGPointGP& _a, const CGPointGP& _b, const GPoint& _p, double _d)
{
	// check distance from the point to the line by x and by y
	double dx = _a.m_P.lon - _b.m_P.lon;
	double dy = _a.m_P.lat - _b.m_P.lat; 
	double r = sqrt((dx*dx) + (dy*dy));
	
	// compute coordinates relative to the origin of the line
	double x1 = _p.x - _b.m_P.lon;
	double y1 = _p.y - _b.m_P.lat;
	
	double x2 = ((x1*dx) + (y1*dy))/r;
	double y2 = ((-x1*dy) + (y1*dx))/r;
	
	return (x2>=0) && (x2<=r) && (y2<=_d) && (y2 >= -_d);
}

S_FEATURE_INFO CShp::get_feature_info()
{	
	return m_feature_selected;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CShp::init_attribute( CDC * pDC, S_LAYER_INFO &layer_info )
{	  
	//S_LAYER_INFO layer_info;
	CGRectGP m_rect_geo; 
		
	//BOOL result = CMapDataMgr::get_vector_lyaer_info( m_file_name, (short)m_scale, layer_info ); //여러곳에서 사용중이다. 최소화하도록 보완!
	//if( result == FALSE ) return;	
	m_view = layer_info.visible; 
		
	switch(mtype_shape)
	{
	case SHPT_POINT:
	case SHPT_POINTZ:
	case SHPT_POINTM: 
	case SHPT_MULTIPOINTM:
		{	
			m_path_symbol = "C:\\AFC4I\\map\\symbol\\";			
			if( layer_info.point_image == "NULL" )
				layer_info.point_image = "24_brown_x_6px";			//기본심볼 교체!!1
			m_path_symbol += layer_info.point_image + ".bmp";			
			break;
		}		
	case SHPT_ARC :
	case SHPT_ARCZ:
	case SHPT_ARCM:
		{     
			mr_polyline = atoi(layer_info.polyline_clr.Left(3)), mg_polyline = atoi(layer_info.polyline_clr.Mid(3,3)), mb_polyline = atoi(layer_info.polyline_clr.Right(3));   	
			
			if( layer_info.polyline_type == "SOLID")
				m_type_polyline = PS_SOLID;
			else if( layer_info.polyline_type == "DASH" )/* -------  */
				m_type_polyline = PS_DASH;
			else if( layer_info.polyline_type == "DOT" )/* .......  */
				m_type_polyline = PS_DOT;
			else if( layer_info.polyline_type == "DASHDOT" )/* _._._._  */
				m_type_polyline = PS_DASHDOT;
			else if( layer_info.polyline_type == "DASHDOTDOT" )/* _.._.._  */
				m_type_polyline = PS_DASHDOTDOT;
						
			CSize size( atoi(layer_info.polyline_width), 1 ); 
			pDC->DPtoLP( &size );	
			m_width_polyline = size.cx * 0.8;
			
			break;
		}		
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
	case SHPT_POLYGONM:
		{ 			
			//패턴이미지 사용 유무 
			m_polygon_is_pattern = layer_info.polygon_is_pattern;
			
			//패턴 이미지 경로 설정 
			m_path_pattern = "C:\\AFC4I\\map\\pattern\\";		
			if( layer_info.polygon_pattern_image == "NULL" )
				layer_info.polygon_pattern_image = "Pattern3";		
			m_path_pattern += layer_info.polygon_pattern_image + ".bmp";
						
			//내부채움 사용 유무 
			if( layer_info.polygon_is_in == "YES" ) m_polygon_is_in = "YES";
			else m_polygon_is_in = "NO";					
						
			//외부, 내부 색상  설정 
			r_polygon = atoi(layer_info.polygon_clr_out.Left(3)), g_polygon = atoi(layer_info.polygon_clr_out.Mid(3,3)), b_polygon = atoi(layer_info.polygon_clr_out.Right(3));   	
			r_polygon_in = atoi(layer_info.polygon_clr_in.Left(3)), g_polygon_in = atoi(layer_info.polygon_clr_in.Mid(3,3)), b_polygon_in = atoi(layer_info.polygon_clr_in.Right(3));   	
						
			//라인 타입 설정 
			if( layer_info.polyline_type == "SOLID")
				m_type_polyline = PS_SOLID;
			else if( layer_info.polyline_type == "DASH" )/* -------  */
				m_type_polyline = PS_DASH;
			else if( layer_info.polyline_type == "DOT" )/* .......  */
				m_type_polyline = PS_DOT;
			else if( layer_info.polyline_type == "DASHDOT" )/* _._._._  */
				m_type_polyline = PS_DASHDOT;
			else if( layer_info.polyline_type == "DASHDOTDOT" )/* _.._.._  */
				m_type_polyline = PS_DASHDOTDOT;
			
			//라인 굵기 설정					
			CSize size( atoi(layer_info.polygon_width), 1 ); 
			pDC->DPtoLP( &size );	
			m_width_polyline = size.cx * 0.8;			
			break;
		}
	default: break;
	}
} 

BOOL CShp::init_symbol( CDC *pDC, CDC *mp_memDC, CDC *mp_maskDC )
{  
	CBitmap maskBitmap;
	CBitmap bmpImage;
	BITMAP  bitmap_info; 
	
	CBitmap * bitmap = NULL;  
	
	HINSTANCE hInstance = ::AfxGetResourceHandle();
	HBITMAP m_hBitmap = (HBITMAP)::LoadImage(hInstance, (LPCTSTR)m_path_symbol, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bitmap =  CBitmap::FromHandle( m_hBitmap );
	bitmap->GetBitmap( &bitmap_info );
	
	CDC tempDC;
	mp_maskDC->CreateCompatibleDC(pDC);
	mp_memDC->CreateCompatibleDC(pDC);
	tempDC.CreateCompatibleDC(pDC);
	
	tempDC.SelectObject(bitmap);
	int nWidth = bitmap_info.bmWidth;		
	int nHeight = bitmap_info.bmHeight;
	
	int result = 0;
	result = bmpImage.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	
	mp_memDC->SelectObject(&bmpImage);
	
	mp_memDC->SetStretchBltMode(BLACKONWHITE);
	mp_memDC->StretchBlt(0, 0, nWidth, nHeight, &tempDC, 0, 0, nWidth, nHeight, SRCCOPY); 
	
	maskBitmap.CreateBitmap(nWidth, nHeight, 1, 1, NULL);
	mp_maskDC->SelectObject(&maskBitmap); 
	mp_memDC->SetBkColor(RGB(255,255,255));
	
	mp_maskDC->SetStretchBltMode(BLACKONWHITE);
	mp_maskDC->StretchBlt(0, 0, nWidth, nHeight, mp_memDC, 0, 0, nWidth, nHeight, SRCCOPY); 
	
	mp_memDC->SetBkColor(RGB(0,0,0)); 
	mp_memDC->SetTextColor(RGB(255,255,255));
	mp_memDC->SetStretchBltMode(BLACKONWHITE);
	mp_memDC->StretchBlt(0, 0, nWidth, nHeight, mp_maskDC, 0, 0, nWidth, nHeight, SRCAND);
	
	xSize = nWidth;
	ySize = nHeight; 

	DeleteObject( m_hBitmap );
	m_hBitmap = NULL;
	tempDC.DeleteDC();
	maskBitmap.DeleteObject();
	bmpImage.DeleteObject();
 
	return TRUE;
}

void CShp::redraw_shp( CDC *pDC, CGRectGP &rect_need, int ratio, int type_dis_map )
{
	//init_attribute( p_dc );
	draw_shp( pDC, rect_need, ratio, type_dis_map );
}

void CShp::draw_shp( CDC *pDC, CGRectGP &rect_need, int ratio, int type_dis_map )
{ 	
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	S_LAYER_INFO layer_info; 
	CMapDataMgr::get_vector_lyaer_info( m_file_name, (short)m_scale, layer_info );
	
	//visible  check
	m_view = CMapDataMgr::get_visibie_vector_layer( m_scale, m_file_name );	
	if(  m_view == FALSE ) 
		return;	
		
	//지명 도시 
	if( layer_info.point_size == "99" )
	{
		if( type_dis_map != 2 )
			m_dbf.draw_dbf( pDC, rect_need, layer_info.scale ); 	
		return; 
	}

	init_attribute( pDC, layer_info );

	if( layer_info.scale_max >= ratio )// && layer_info.scale_max >= ratio )
	{		
		//지도 도시 
		pDC->SetBkMode(TRANSPARENT);
		switch(mtype_shape)
		{
		case SHPT_POINT:
		case SHPT_POINTZ:
		case SHPT_POINTM:
		case SHPT_MULTIPOINTM:
			{
				CDC *mp_memDC = NULL;
				CDC *mp_maskDC = NULL;		
				mp_maskDC = new CDC();
				mp_memDC = new CDC();
				
				//심볼에 마스크 적용한다 
				init_symbol( pDC, mp_memDC, mp_maskDC );			
				
				double lx, ly;
				CPoint pt_s;
				//CPoint screenpt;
				CSize size;
				CGPointGP pt_geo;
				size.cx = xSize;
				size.cy = ySize;
				pDC->DPtoLP( &size );			
				pDC->SetBkColor(RGB(255,255,255));
				
				for( int cnt = 0; cnt < mcnt_record ; ++cnt )
				{   
					pt_geo.m_P.lon = mp_geo_point[cnt].x;
					pt_geo.m_P.lat = mp_geo_point[cnt].y;
					//p_dc->GeoPtoLP( &pt_s, &pt_geo );

					m_Coord.degrees_lat_long_to_x_y (pt_geo.m_P.lat, pt_geo.m_P.lon, &lx, &ly); 
					m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pt_s.x, &pt_s.y);
					
					pDC->StretchBlt(
						pt_s.x - size.cx / 2,
						pt_s.y + size.cy / 2,
						size.cx, -size.cy, 
						mp_maskDC, 0, 0, 
						xSize, ySize, 
						SRCAND);
					pDC->StretchBlt(
						pt_s.x - size.cx / 2,
						pt_s.y + size.cy / 2,
						size.cx, -size.cy, 
						mp_memDC, 0, 0, 
						xSize, ySize, 
						SRCPAINT);		
					
				}
				delete mp_maskDC;
				delete mp_memDC;			
				break;
			}		
		case SHPT_ARC :
		case SHPT_ARCZ:
		case SHPT_ARCM:	
			{    
				CPen m_pen_polyline;  			
				CPen  *pOldPen = NULL;    
				
				LOGBRUSH logBrush;
				logBrush.lbStyle = BS_SOLID;
				logBrush.lbColor =  RGB(mr_polyline,mg_polyline,mb_polyline);
				
				DWORD * pStyle = NULL;		
				pStyle = new DWORD[6];
				
				switch( m_type_polyline )
				{
					// solid
				case 0 : 
					m_pen_polyline.CreatePen(PS_SOLID, static_cast<int>(m_width_polyline), logBrush.lbColor);
					break;
					
					// dash
				case 1 : pStyle = new DWORD[2];
					pStyle[0]= static_cast<unsigned long>(m_width_polyline*10);
					pStyle[1]= static_cast<unsigned long>(m_width_polyline*2);
					m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
						static_cast<int>(m_width_polyline), &logBrush, 2, pStyle);		
					break;
					
					// dot
				case 2 : pStyle = new DWORD[2];
					pStyle[0]= static_cast<unsigned long>(m_width_polyline);
					pStyle[1]= static_cast<unsigned long>(m_width_polyline);
					m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
							static_cast<int>(m_width_polyline), &logBrush, 2, pStyle);		
					break;
					
					// dashdot
				case 3 : pStyle = new DWORD[4];
					pStyle[0]= static_cast<unsigned long>(m_width_polyline*10);
					pStyle[1]= static_cast<unsigned long>(m_width_polyline);
					pStyle[2]= static_cast<unsigned long>(m_width_polyline*2);
					pStyle[3]= static_cast<unsigned long>(m_width_polyline);
					m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
							static_cast<int>(m_width_polyline), &logBrush, 4, pStyle);		
					break;
					
					// dashdotdot
				case 4 : pStyle = new DWORD[6];
					pStyle[0]= static_cast<unsigned long>(m_width_polyline*10);
					pStyle[1]= static_cast<unsigned long>(m_width_polyline);
					pStyle[2]= static_cast<unsigned long>(m_width_polyline*2);
					pStyle[3]= static_cast<unsigned long>(m_width_polyline);
					pStyle[4]= static_cast<unsigned long>(m_width_polyline*2);
					pStyle[5]= static_cast<unsigned long>(m_width_polyline);
					m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
							static_cast<int>(m_width_polyline), &logBrush, 6, pStyle);		
					break;
					
				case 9 : break;
					// default
				default : 
					m_pen_polyline.CreatePen(PS_SOLID, static_cast<int>(m_width_polyline), logBrush.lbColor);
					break;
				}
				delete [] pStyle;
				pOldPen = (CPen *)pDC->SelectObject(&m_pen_polyline); 
				
				CGPointGP pt_geo;
				for(int i = 0; i < mcnt_record; ++i)
				{				
					if( rect_need.m_R.left > mp_geo_poly_obj[i].m_MBR.xmax  ||
						rect_need.m_R.right < mp_geo_poly_obj[i].m_MBR.xmin ||
						rect_need.m_R.top + 0.1 < mp_geo_poly_obj[i].m_MBR.ymin ||
						rect_need.m_R.bottom > mp_geo_poly_obj[i].m_MBR.ymax )
						continue;
					
					int NumPoints = mp_geo_poly_obj[i].m_nNumPoints;
					int NumParts = mp_geo_poly_obj[i].m_nNumParts;
					
					double x, y;
					CPoint* pTempPoint = NULL;
					//CPoint* screenPoint = NULL;
					
					if( NumPoints > 0 )
					{
						pTempPoint	= new CPoint[NumPoints];
						//screenPoint = new CPoint[NumPoints];
					}
					for(int j = 0; j < NumPoints; ++j)
					{
						pt_geo.m_P.lat = mp_geo_poly_obj[i].m_pPoints[j].y;
						pt_geo.m_P.lon = mp_geo_poly_obj[i].m_pPoints[j].x;					
						//p_dc->GeoPtoLP( &pTempPoint[j], &pt_geo);	

						m_Coord.degrees_lat_long_to_x_y (pt_geo.m_P.lat, pt_geo.m_P.lon, &x, &y); 
						m_CoordMgr->WorldXY2ScreenXY(x, y, &pTempPoint[j].x, &pTempPoint[j].y);

					}			
					
					
					int * pVertex = NULL;
					if( NumParts > 0 )	pVertex = new int[NumParts];

					for(int j = 0; j < NumParts; ++j)	
					{
						if(  j == (NumParts - 1) )	
							pVertex[j] = NumPoints - mp_geo_poly_obj[i].m_pParts[j];
						else 		
							pVertex[j] = mp_geo_poly_obj[i].m_pParts[ j+1 ] - mp_geo_poly_obj[i].m_pParts[j];						
					} 
					::PolyPolyline( pDC->m_hDC, pTempPoint, (DWORD*)pVertex, NumParts);
					
					if( NumParts > 0 ) delete[] pVertex;
					if( NumPoints > 0 ) delete[] pTempPoint;
				}
				pDC->SelectObject(pOldPen);
				m_pen_polyline.DeleteObject();
				break;
			}		
		case SHPT_POLYGON:
		case SHPT_POLYGONZ:
		case SHPT_POLYGONM:
			{				
				CBrush m_brush_polygon, * pOldBrush = NULL;   
				CPen m_pen_polyline, *oldPen =	NULL;  
				
				int iOldRop = pDC->GetROP2();
				int iOldBkMode = pDC->GetBkMode();				
				
				if( m_polygon_is_pattern != 0 && type_dis_map != 2 )
				{
					CBitmap * bitmap = NULL;    
					HINSTANCE hInstance = ::AfxGetResourceHandle();
					HBITMAP	m_hBitmap = NULL;
					m_hBitmap = (HBITMAP)::LoadImage(hInstance, (LPCTSTR)m_path_pattern, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					if( m_hBitmap )	
					{
						bitmap = CBitmap::FromHandle( m_hBitmap );
						
						LOGBRUSH logBrush;
						::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
						m_brush_polygon.CreatePatternBrush( bitmap ); 
						m_brush_polygon.GetLogBrush(&logBrush);
						m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_ENDCAP_FLAT, 5, &logBrush); 
						
						oldPen =  pDC->SelectObject(&m_pen_polyline);
						pOldBrush = (CBrush *)pDC->SelectObject(&m_brush_polygon); 
						
						iOldRop = pDC->SetROP2(R2_MASKPEN);
						iOldBkMode = pDC->SetBkMode(TRANSPARENT);	
						
						DeleteObject( m_hBitmap ); 
						bitmap->DeleteObject();
					}
					m_hBitmap = NULL;
					bitmap = NULL;
				}
				else
				{
					LOGBRUSH logBrush;
					logBrush.lbStyle = BS_SOLID;
					logBrush.lbColor = RGB(r_polygon,g_polygon,b_polygon);
					
					DWORD * pStyle = NULL;		
					pStyle = new DWORD[6];
					
					switch( m_type_polyline )
					{
						// solid
					case 0 : 
						m_pen_polyline.CreatePen(PS_SOLID, static_cast<int>(m_width_polyline), logBrush.lbColor);
						break;
						
						// dash
					case 1 : pStyle = new DWORD[2];
						pStyle[0]= static_cast<unsigned long>(m_width_polyline*10);
						pStyle[1]= static_cast<unsigned long>(m_width_polyline*2);
						m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
								static_cast<int>(m_width_polyline), &logBrush, 2, pStyle);		
						break;
						
						// dot
					case 2 : pStyle = new DWORD[2];
						pStyle[0]= static_cast<unsigned long>(m_width_polyline);
						pStyle[1]= static_cast<unsigned long>(m_width_polyline);
						m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
								static_cast<int>(m_width_polyline), &logBrush, 2, pStyle);		
						break;
						
						// dashdot
					case 3 : pStyle = new DWORD[4];
						pStyle[0]= static_cast<unsigned long>(m_width_polyline*10);
						pStyle[1]= static_cast<unsigned long>(m_width_polyline);
						pStyle[2]= static_cast<unsigned long>(m_width_polyline*2);
						pStyle[3]= static_cast<unsigned long>(m_width_polyline);
						m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
								static_cast<int>(m_width_polyline), &logBrush, 4, pStyle);		
						break;
						
						// dashdotdot
					case 4 : pStyle = new DWORD[6];
						pStyle[0]= static_cast<unsigned long>(m_width_polyline*10);
						pStyle[1]= static_cast<unsigned long>(m_width_polyline);
						pStyle[2]= static_cast<unsigned long>(m_width_polyline*2);
						pStyle[3]= static_cast<unsigned long>(m_width_polyline);
						pStyle[4]= static_cast<unsigned long>(m_width_polyline*2);
						pStyle[5]= static_cast<unsigned long>(m_width_polyline);
						m_pen_polyline.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
								static_cast<int>(m_width_polyline), &logBrush, 6, pStyle);		
						break;						
					case 9 : 
						break;
					default : 
						m_pen_polyline.CreatePen(PS_SOLID, 	static_cast<int>(m_width_polyline), logBrush.lbColor);
						break;
					}
					delete []pStyle;
					
					oldPen = (CPen *)pDC->SelectObject(&m_pen_polyline); 					
					m_brush_polygon.CreateSolidBrush(RGB(r_polygon_in,g_polygon_in,b_polygon_in));  				
					
					if( m_polygon_is_in == "YES" && type_dis_map != 2 )
						pOldBrush = (CBrush *)pDC->SelectObject(&m_brush_polygon); 
					else
					{
						iOldRop = pDC->SetROP2(R2_MASKPEN);
						iOldBkMode = pDC->SetBkMode(TRANSPARENT);	
					}
				}			
				

				CGPointGP pt_geo;
				for(int i = 0; i < mcnt_record; ++i)
				{ 					
					if( rect_need.m_R.left > mp_geo_poly_obj[i].m_MBR.xmax ||
						rect_need.m_R.right < mp_geo_poly_obj[i].m_MBR.xmin ||
						rect_need.m_R.top + 0.1 < mp_geo_poly_obj[i].m_MBR.ymin ||
						rect_need.m_R.bottom > mp_geo_poly_obj[i].m_MBR.ymax )
						continue;
					
					int NumPoints = mp_geo_poly_obj[i].m_nNumPoints;
					int NumParts = mp_geo_poly_obj[i].m_nNumParts;
					
					
					double lx, ly;
					CPoint *pTempPoint = NULL;
					//CPoint *screenPoint = NULL;
					if( NumPoints > 0 )
					{
						pTempPoint	= new CPoint[NumPoints];
						//screenPoint = new CPoint[NumPoints];
					}
					for(int j = 0; j < NumPoints; ++j)
					{
						pt_geo.m_P.lon = mp_geo_poly_obj[i].m_pPoints[j].x;
						pt_geo.m_P.lat = mp_geo_poly_obj[i].m_pPoints[j].y;
						//p_dc->GeoPtoLP( &pTempPoint[j], &pt_geo);	
						m_Coord.degrees_lat_long_to_x_y (pt_geo.m_P.lat, pt_geo.m_P.lon, &lx, &ly); 
						m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pTempPoint[j].x, &pTempPoint[j].y);

					}			
					
					int * pVertex = NULL;
					if( NumParts > 0 )
						pVertex = new int[NumParts];
					
					for(int j = 0; j <NumParts; ++j)	
					{
						if(  j == (NumParts - 1))	
							pVertex[j] = NumPoints - mp_geo_poly_obj[i].m_pParts[j];
						else 		
							pVertex[j] = mp_geo_poly_obj[i].m_pParts[ j+1 ] - mp_geo_poly_obj[i].m_pParts[j];						
					} 
					
					::PolyPolygon( pDC->m_hDC, pTempPoint, pVertex, NumParts);				
					
					if( NumParts > 0 ) delete[] pVertex;
					if( NumPoints > 0 ) delete[] pTempPoint;
				}
				if( oldPen->GetSafeHandle() )
					pDC->SelectObject(oldPen);				
				if( m_polygon_is_in == "YES" &&  pOldBrush->GetSafeHandle() )	
					pDC->SelectObject(pOldBrush);	 
				
				pDC->SetROP2(iOldRop);
				pDC->SetBkMode(iOldBkMode);				
				
				m_pen_polyline.DeleteObject();
				m_brush_polygon.DeleteObject();
				break;
			}			
		}
	}	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CShp::close_dbf_file()	
{
	m_dbf.close_dbf_file();
}

GeoPoint * CShp::get_geo_point_obj() 
{ 
	return mp_geo_point; 
}
