// Dbf.cpp: implementation of the CDbf class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Dbf.h"
#include "Shp.h"





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDbf::CDbf()
{
	m_is_load = FALSE;
	mp_field = NULL;
	mp_label = NULL;
	m_visible_dbf = TRUE;
	mp_shp = NULL;

}

CDbf::~CDbf()
{	 
}


void CDbf::remove_space( char * rec, int length )
{
	for(int  cnt = length - 1; cnt > 0; --cnt)
	{
		if(rec[ cnt ] == ' ')
			rec[ cnt ] = 0;
		else
			break;
	}
}

BOOL CDbf::calculate_label_point()
{
	int i;
	double lx, ly;
	
	switch( mp_shp->get_type_shp() )
	{
	case SHPT_POINT:
	case SHPT_POINTZ:
	case SHPT_POINTM: 
	case SHPT_MULTIPOINTM:
		for(i=0; i<mp_shp->get_cnt_record() ; i++)
		{	
		//	mp_label[i].geo_x = mp_shp->get_geo_point_obj()[i].x;
	//		mp_label[i].geo_y = mp_shp->get_geo_point_obj()[i].y;					
		}	
		
		break;
		
	case SHPT_ARC :
	case SHPT_ARCZ: 
	case SHPT_ARCM:	
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
	case SHPT_POLYGONM:
	case SHPT_MULTIPOINT:
		for(i = 0; i < mp_shp->get_cnt_record(); ++i)
		{
			lx = (mp_shp->get_geo_poly_obj()[i].m_MBR.xmax + mp_shp->get_geo_poly_obj()[i].m_MBR.xmin) / 2;
			ly = (mp_shp->get_geo_poly_obj()[i].m_MBR.ymax + mp_shp->get_geo_poly_obj()[i].m_MBR.ymin) / 2;						
		
			mp_label[i].geo_x = lx;
			mp_label[i].geo_y = ly;			
		}	
		
		break;
	}
	return true;
} 

void CDbf::load_dbf( std::string path_file, CString type_map )
{
	TRACE("load_dbf");
	FILE *fp = NULL;
	char ver;
	char *record = NULL;
	//TCHAR *record;
	
	if( (fp = fopen( path_file.c_str(), "rb" )) == NULL )
	{	
		TRACE(_T("파일열려있음!!!!!"));
		return ;
	}
	fread( &ver, 1, 1, fp );
	if(ver != 3)
	{
		fclose(fp);
		fp = NULL;
		return ;
	}
	
	int idx_annotation = 0;
	int type_annotation = 0; //0 = int, 1 = str
 
	if( type_map == TYPE_EARTH_100 ) 
	{ 
		idx_annotation = 2;
		type_annotation = 0;
	}
	if( type_map == TYPE_EARTH_50 ) 
	{
		idx_annotation = 2;
		type_annotation = 0;
	}
	if( type_map == TYPE_EARTH_25 ) 
	{ 
		idx_annotation = 0;
		type_annotation = 0;
	}
	if( type_map == TYPE_EARTH_10 ) 
	{ 
		idx_annotation = 6;
		type_annotation = 1;
	}
	if( type_map == TYPE_EARTH_5 ) 
	{
		idx_annotation = 13;
		type_annotation = 1;		
	}	

	//if(mp_field== NULL)
	//if(1)
	//{
		// 파일 헤더 읽어오기
		fseek(fp, 0, SEEK_SET);
		fread(&mf_header, sizeof(DBF_FILEHEADER), 1, fp);
		mcnt_field = (mf_header.header_length - sizeof(DBF_FILEHEADER)-1)
			/ sizeof(DBF_FILEDDESCRIPTOR);
		mp_field = new DBF_FILEDDESCRIPTOR[mcnt_field];
		fread(mp_field, sizeof(DBF_FILEDDESCRIPTOR)*mcnt_field, 1, fp);
		int offset = mf_header.header_length ;
		fseek(fp, offset, SEEK_SET);
		
		mp_label = new SLABEL[mf_header.recordn];  
	//}
	
	for(int j=0; j < (int)mf_header.recordn ; j++)
	{
		fseek(fp, 1, SEEK_CUR);
		for(int i=0; i<mcnt_field; i++)
		{		
			int cnt_ = mp_field[i].field_length;

			record = NULL;
			record = new char[ cnt_+1 ];
			fread(record, mp_field[i].field_length, 1, fp);
			record[mp_field[i].field_length] = '\0'; 
			remove_space(record,mp_field[i].field_length); 
			
			if(i==midx_field)
				mp_label[j].label = record;
 			if(i==idx_annotation )
			{
				if(0==type_annotation )
					mp_label[j].annotation_int =  atoi(record);
				if(1==type_annotation )
					mp_label[j].annotation_str =  record;
				
				mp_label[j].type_annotation = type_annotation;
			}				

			delete[] record;
			record = NULL;
		}       
	}
	fclose(fp);	
	fp = NULL;

	//if(m_is_load == false)	
	calculate_label_point();
	
	m_is_load = true;
}

BOOL CDbf::is_load_dbf() 
{ 
	return m_is_load; 
}

void CDbf::read_dbf( std::string path_file, std::vector< s_feature_value > & vec_field_value, int idx_row )
{
	TRACE("read_dbf");
	FILE *fp = NULL;
	char ver;
	char *record = NULL;
	
	if( (fp = fopen( path_file.c_str(), "rb" )) == NULL )  return ;
	
	fread( &ver, 1, 1, fp );
	if(ver != 3)
	{
		fclose(fp);
		return ;
	}
	
	if(mp_field== NULL)	
	{
		fseek(fp, 0, SEEK_SET);
		fread(&mf_header, sizeof(DBF_FILEHEADER), 1, fp);
		mcnt_field = (mf_header.header_length - sizeof(DBF_FILEHEADER)-1)/ sizeof(DBF_FILEDDESCRIPTOR);
		mp_field = new DBF_FILEDDESCRIPTOR[mcnt_field];
		fread(mp_field, sizeof(DBF_FILEDDESCRIPTOR)*mcnt_field, 1, fp);
		int offset = mf_header.header_length ;
		fseek(fp, offset, SEEK_SET);		
	}
	
	for(int j=0; j < (int)mf_header.recordn ; j++)
	{
		fseek(fp, 1, SEEK_CUR);
		for(int i=0; i<mcnt_field; i++)
		{		
			int cnt_ = mp_field[i].field_length;
						mp_field[i].name;
			record = new char[ cnt_+1 ];
			fread(record, mp_field[i].field_length, 1, fp);
			record[mp_field[i].field_length] = '\0'; 
			remove_space(record,mp_field[i].field_length); 		 
			if( j == idx_row )
			{				
				s_feature_value tmp;
				tmp.field = mp_field[i].name;
				tmp.value = record;
				vec_field_value.push_back( tmp );
			}
			delete[] record;
		}       
	}
	fclose(fp);
}

///////////////////////////////////////////////////////////////////////////////////////
//
//		
//
//////////////////////////////////////////////////////////////////////////////////////
void CDbf::draw_dbf( CDC *pDC, CGRectGP &rect_need, CString scale )
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	if( m_is_load == false)	return;	
	
	CPoint pt;
	pDC->SetBkMode(TRANSPARENT); 
	pDC->SetTextAlign(TA_LEFT);	
	
	CPoint	tmpPoint;
	LOGFONT logFont;
	logFont = CMapDataMgr::get_dbf_font();
	CFont font;
	CSize fSize;	
	if(logFont.lfFaceName[0] == 0)
	{
		fSize.cx = 11;
		fSize.cy = 11;
		pDC->DPtoLP(&fSize);
		font.CreateFont(fSize.cx, 0,
			0, 0, 0,
			FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS , "Times New Roman (영어)");
	}
	else
	{
		fSize.cx = logFont.lfWidth;
		fSize.cy = logFont.lfHeight;		
		pDC->DPtoLP(&fSize);
		font.CreateFont(fSize.cx, 0, logFont.lfEscapement, logFont.lfOrientation, logFont.lfWeight
						,logFont.lfItalic, logFont.lfUnderline, logFont.lfStrikeOut, logFont.lfCharSet
						,logFont.lfOutPrecision, logFont.lfClipPrecision, logFont.lfQuality, logFont.lfPitchAndFamily, logFont.lfFaceName);
	}
	

	CFont* def_font = pDC->SelectObject(&font);
	CGPointGP point_geo;
	CString lpszString = "";
	int nCount = 0;
	CPoint	ptPoint( 0, 0 );
	double lx, ly;

	for(unsigned int i=0; i< mf_header.recordn; ++i)
	{		
		//label의 x,y좌표 설정
		double x = mp_label[i].geo_x;
		double y = mp_label[i].geo_y;

		if (rect_need.m_R.left > x || 
			rect_need.m_R.right < x || 
 			rect_need.m_R.top + 0.1 < y || 
			rect_need.m_R.bottom > y )
					continue;
		
		//현재 축척 얻기.
		//축척별 dbf idx 얻기 ..는 dbf 읽을때 해야 한다. 
		//idx filtering
		
/*
		if( mp_label[i].type_annotation == 0 ) //int
		{
			if( mp_label[i].annotation_int == 1 ||
				mp_label[i].annotation_int == 2 ||
				mp_label[i].annotation_int == 4 ||
				mp_label[i].annotation_int == 11 ||
				mp_label[i].annotation_int == 19 ||
				mp_label[i].annotation_int == 20 ||
				mp_label[i].annotation_int == 22 ||
				mp_label[i].annotation_int == 23 ||
				mp_label[i].annotation_int == 24 ||
				mp_label[i].annotation_int == 29 ||
				mp_label[i].annotation_int == 31 ||
				mp_label[i].annotation_int == 36 ||
				mp_label[i].annotation_int == 38 )
			{

			}
			else
			{
				continue;
			}	
		}

		if( mp_label[i].type_annotation == 1 ) //str
		{
			if( mp_label[i].annotation_str == "강명" ||
				mp_label[i].annotation_str == "내륙수부" ||
				mp_label[i].annotation_str == "바다명" || 
				mp_label[i].annotation_str == "산명" ||
				mp_label[i].annotation_str == "일반시명" ||
				mp_label[i].annotation_str == "항" ||

				mp_label[i].annotation_str == "강" ||
				mp_label[i].annotation_str == "계곡" ||
				mp_label[i].annotation_str == "교도소" ||
				mp_label[i].annotation_str == "굴뚝" || 

				mp_label[i].annotation_str == "기상대" ||
				mp_label[i].annotation_str == "댐" || 

				mp_label[i].annotation_str == "바다" ||
				mp_label[i].annotation_str == "법정지명" ||
				mp_label[i].annotation_str == "산" ||


				mp_label[i].annotation_str == "수압관" ||
				mp_label[i].annotation_str == "암석지대" ||
				mp_label[i].annotation_str == "역" ||

				mp_label[i].annotation_str == "일반철도" ||
				mp_label[i].annotation_str == "저수지" ||
				mp_label[i].annotation_str == "절" ||

				mp_label[i].annotation_str == "컨베이어" || 
				mp_label[i].annotation_str == "터널" ||
				mp_label[i].annotation_str == "IC명"

				)
			{

			}
			else
			{
				continue;
			}	
		}
		*/

		
		if( scale == "100" )
		{
			if( mp_label[i].annotation_int == 1 ||
				mp_label[i].annotation_int == 19 ||
				mp_label[i].annotation_int == 20 ||
				mp_label[i].annotation_int == 22 ||
				mp_label[i].annotation_int == 31  )
			{}else{continue;}	
		}
		
		if( scale == "50" )
		{
			if( mp_label[i].annotation_int == 3 ||
				mp_label[i].annotation_int == 19 ||
				mp_label[i].annotation_int == 20 ||
				mp_label[i].annotation_int == 24 ||
				mp_label[i].annotation_int == 29 || 
				mp_label[i].annotation_int == 38)
			{}else{continue;}	
		}
		
		if( scale == "25" )
		{
			if( 
				mp_label[i].annotation_int == 4 ||
				mp_label[i].annotation_int == 6 ||
				mp_label[i].annotation_int == 8 ||
				mp_label[i].annotation_int == 11 )//||
			//	mp_label[i].annotation_int == 27 )
			{}else{continue;}	
		}
		
		
		if( scale == "10" )
		{
			if( 	
				mp_label[i].annotation_str == "경계명" ||
				mp_label[i].annotation_str == "법정지명" ||
				mp_label[i].annotation_str == "일반시명"  )
			{}else{continue;}	
		}
		
		if( scale == "5" )
		{
			if( 	
				mp_label[i].annotation_str == "강명" ||
				mp_label[i].annotation_str == "바다명" || 
				mp_label[i].annotation_str == "산명" ||
				mp_label[i].annotation_str == "일반시명" ||
				mp_label[i].annotation_str == "항" ||
				mp_label[i].annotation_str == "강" ||
				mp_label[i].annotation_str == "계곡" ||
				mp_label[i].annotation_str == "댐" || 
				mp_label[i].annotation_str == "바다" ||
				mp_label[i].annotation_str == "법정지명" ||
				mp_label[i].annotation_str == "산" ||
				mp_label[i].annotation_str == "역" ||
				mp_label[i].annotation_str == "일반철도" ||
				mp_label[i].annotation_str == "저수지"   )
			{}else{continue;}	
		} 

		lpszString = (CString)mp_label[i].label;
	 	if( lpszString == "" ) continue;
		
		nCount = mp_label[i].label.GetLength();	
		point_geo.m_P.lat = y;
		point_geo.m_P.lon = x;

		//p_dc->GeoPtoLP( &ptPoint, &point_geo );
		m_Coord.degrees_lat_long_to_x_y (point_geo.m_P.lat, point_geo.m_P.lon, &lx, &ly); 
		m_CoordMgr->WorldXY2ScreenXY(lx, ly, &ptPoint.x, &ptPoint.y);

		lpszString.Replace("\n", "");
		lpszString.Replace("\t", "");
		lpszString.Replace(" ", "");
	
		TRACE("DRAW DBF\n");

		pDC->TextOut( ptPoint.x, ptPoint.y, (LPCTSTR)lpszString, nCount );
	}	

	pDC->SelectObject(def_font);
	font.DeleteObject(); 
}
//#define SAFE_DELETE(x) if(x!=NULL){ delete x; x=NULL;};

void CDbf::close_dbf_file()
{
//	if( mcnt_field > 0 )
//		delete mp_field;
	mp_field = NULL;
		
//	if(mp_label != NULL)
	{
//	 	delete  [] mp_label;
		mp_label = NULL;
	}  
}