#include "pch.h"
#include "shpObject.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#include <crtdbg.h>
#ifdef malloc
#undef malloc
#endif
#define malloc(s) (_malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
#ifdef calloc
#undef calloc
#endif
#define calloc(c, s) (_calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
#ifdef realloc
#undef realloc
#endif
#define realloc(p, s) (_realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
#ifdef _expand
#undef _expand
#endif
#define _expand(p, s) (_expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__  ))
#ifdef free
#undef free
#endif
#define free(p) (_free_dbg(p, _NORMAL_BLOCK))
#ifdef _msize
#undef _msize
#endif
#define _msize(p) (_msize_dbg(p, _NORMAL_BLOCK))
#endif


shpObject::shpObject()
{
	m_objColor = RGB(0, 0, 0);
	m_nStyle = 0;
	m_nWidth = 0;

	m_nDetectPixelSize = 4;

	m_eStatus = stateNone;

	m_nShapeType = 0;

	m_brushColor = RGB(0, 200, 200);

 //	m_bSolidPolygon = false;
  	m_bSolidPolygon = true;
	m_bLabel = false;

	m_nPointType = 0;

	//m_pProgressDlg = NULL;		// young ªË¡¶

	m_strSHPPath = "";

	m_nField = 0;
}

shpObject::~shpObject()
{
	m_DBF.Close();
}

CPoint shpObject::WorldToDevice(const GPoint& _GPoint)
{
	CPoint	tmpPoint;

	tmpPoint.x = long(m_lfRatio * (_GPoint.x - m_geoCenterPoint.x) * m_lfZoomFactor + (double)m_scrCenterPoint.x);
	tmpPoint.y = long(m_lfRatio * (m_geoCenterPoint.y - _GPoint.y) * m_lfZoomFactor + (double)m_scrCenterPoint.y);

	return tmpPoint;
}

GPoint shpObject::DeviceToWorld(const CPoint& _point)
{
	GPoint	tmpGPoint;

	tmpGPoint.x = m_geoCenterPoint.x + (_point.x - m_scrCenterPoint.x) / (m_lfZoomFactor * m_lfRatio);
	tmpGPoint.y = m_geoCenterPoint.y - (_point.y - m_scrCenterPoint.y) / (m_lfZoomFactor * m_lfRatio);

	return tmpGPoint;
}

CRect shpObject::WorldToDevice(const MBR& _mbr)
{
	CRect	tmpRect;
	CPoint	tmpPT1, tmpPT2;
	GPoint pt1, pt2;

	pt1.x = _mbr.xmin;	pt1.y = _mbr.ymin;
	pt2.x = _mbr.xmax;  pt2.y = _mbr.ymax;

	tmpPT1 = WorldToDevice(pt1);
	tmpPT2 = WorldToDevice(pt2);

	tmpRect.bottom	= tmpPT1.y;
	tmpRect.left	= tmpPT1.x;
	tmpRect.top		= tmpPT2.y;
	tmpRect.right	= tmpPT2.x;

	return tmpRect;
}

MBR shpObject::DeviceToWorld(const CRect& _rect)
{
	MBR tmpMBR;
	GPoint	tmpPT1, tmpPT2;

	CPoint	pt1(_rect.left,	 _rect.bottom);
	CPoint	pt2(_rect.right, _rect.top);

	tmpPT1 = DeviceToWorld(pt1);
	tmpPT2 = DeviceToWorld(pt2);

	tmpMBR.xmax = tmpPT2.x;
	tmpMBR.xmin = tmpPT1.x;
	tmpMBR.ymax = tmpPT2.y;
	tmpMBR.ymin = tmpPT1.y;

	return tmpMBR;
}

bool shpObject::PtInRect(const GPoint& _geoInputPoint, const GPoint& _geoRectCenter)
{
	if(_geoInputPoint.x < (_geoRectCenter.x - (double)m_nDetectPixelSize / m_lfZoomFactor / m_lfRatio)) 
		return false;
	else if(_geoInputPoint.x > (_geoRectCenter.x + (double)m_nDetectPixelSize / m_lfZoomFactor / m_lfRatio)) 
		return false;
	else if(_geoInputPoint.y < ( _geoRectCenter.y - (double)m_nDetectPixelSize / m_lfZoomFactor/ m_lfRatio)) 
		return false;
	else if(_geoInputPoint.y > ( _geoRectCenter.y + (double)m_nDetectPixelSize / m_lfZoomFactor / m_lfRatio)) 
		return false;
	else 
		return true;
}

bool shpObject::PtInRect(const CGPointGP& _geoInputPoint,const GPoint& _geoRectCenter)
{
	if(_geoInputPoint.m_P.lon < (_geoRectCenter.x - (double)m_nDetectPixelSize / m_lfZoomFactor / m_lfRatio)) 
		return false;
	else if(_geoInputPoint.m_P.lon > (_geoRectCenter.x + (double)m_nDetectPixelSize / m_lfZoomFactor / m_lfRatio)) 
		return false;
	else if(_geoInputPoint.m_P.lat < ( _geoRectCenter.y - (double)m_nDetectPixelSize / m_lfZoomFactor/ m_lfRatio)) 
		return false;
	else if(_geoInputPoint.m_P.lat > ( _geoRectCenter.y + (double)m_nDetectPixelSize / m_lfZoomFactor / m_lfRatio)) 
		return false;
	else 
		return true;
}

bool shpObject::PtInPolygon(vector<GPoint>& _vPoint, GPoint _GPoint)
{
	int i, j;
	bool c = false;
	int size = _vPoint.size();

    for (i = 0, j = (size - 1); i < size; j = i++) 
	{
		if ((((_vPoint[i].y <= _GPoint.y) && (_GPoint.y < _vPoint[j].y)) || ((_vPoint[j].y <= _GPoint.y) && ( _GPoint.y < _vPoint[i].y))) && (_GPoint.x < (_vPoint[j].x - _vPoint[i].x) * (_GPoint.y - _vPoint[i].y) / (_vPoint[j].y - _vPoint[i].y) + _vPoint[i].x))
			c = !c;
	}
	return c;
}

bool shpObject::PtInPolygon(int iSize, CGPointGP *gpointgp, GPoint _GPoint)
{
	int i, j;
	bool c = false;
	int size = iSize;
    for (i = 0, j = (size - 1); i < size; j = i++)
	{
		if ((
			(
			(gpointgp[i].m_P.lat <= _GPoint.y) && (_GPoint.y < gpointgp[j].m_P.lat)
			) 
			|| 
			(
			(gpointgp[j].m_P.lat <= _GPoint.y) && ( _GPoint.y < gpointgp[i].m_P.lat)
			)
			)
			&& 
			(
			_GPoint.x < 
			(gpointgp[j].m_P.lon - gpointgp[i].m_P.lon) 
			* (_GPoint.y - gpointgp[i].m_P.lat) 
			/ (gpointgp[j].m_P.lat - gpointgp[i].m_P.lat)
			+ gpointgp[i].m_P.lon)
			)
			c = !c;
	}
	return c;
}

bool shpObject::IsPointNearLine(const GPoint& _a, const GPoint& _b, const GPoint& _p, double _d)
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

bool shpObject::IsPointNearLine(const CGPointGP& _a, const CGPointGP& _b, const GPoint& _p, double _d)
{
	// check distance from the point to the line by x and by y
	double dx = _a.m_P.lon - _b.m_P.lon;
	double dy = _a.m_P.lat - _b.m_P.lat;
//	double r = sqrt(dx*dx + dy*dy);
	double r = sqrt((dx*dx) + (dy*dy));

	// compute coordinates relative to the origin of the line
	double x1 = _p.x - _b.m_P.lon;
	double y1 = _p.y - _b.m_P.lat;

	// compute coordinates relative to the line
//	double x2 = (x1*dx + y1*dy)/r;
//	double y2 = (-x1*dy + y1*dx)/r;
	double x2 = ((x1*dx) + (y1*dy))/r;
	double y2 = ((-x1*dy) + (y1*dx))/r;

	return (x2>=0) && (x2<=r) && (y2<=_d) && (y2 >= -_d);
}

void shpObject::ReMBR(vector<GPoint>& _vPoint)
{
	m_MBR.xmax = m_MBR.ymax = -99999999.;
	m_MBR.xmin = m_MBR.ymin = +99999999.;

	for(int i = 0; i < _vPoint.size(); ++i)
	{
		m_MBR.xmax = (m_MBR.xmax < _vPoint[i].x) ? _vPoint[i].x : m_MBR.xmax; 
		m_MBR.xmin = (m_MBR.xmin > _vPoint[i].x) ? _vPoint[i].x : m_MBR.xmin;
		m_MBR.ymax = (m_MBR.ymax < _vPoint[i].y) ? _vPoint[i].y : m_MBR.ymax; 
		m_MBR.ymin = (m_MBR.ymin > _vPoint[i].y) ? _vPoint[i].y : m_MBR.ymin;
	}
}

bool shpObject::Open(CString _filename)
{
	FILE		*fpSHP;						//SHP File Pointer
    FILE		*fpSHX;						//SHX File Pointer

	int tmp = 0;
	
	m_strSHPPath = _filename;	//SHP file path

	tmp = _filename.GetLength()-4;
	m_strSHXPath = _filename.Mid(0, tmp) + ".shx";	//SHX file path
	m_strDBFPath = _filename.Mid(0, tmp) + ".dbf";	// dbf file path

	// Get Field number..
	m_DBF.Open((LPCTSTR)m_strDBFPath, "rb");
	m_nField = m_DBF.GetFieldCount();

	//open SHP/SHX file
	fpSHP = fopen(m_strSHPPath, "rb");
	fpSHX = fopen(m_strSHXPath, "rb");

	if(fpSHP == NULL || fpSHX == NULL)
	{
		//AfxMessageBox("SHP File Error!!", IDOK | MB_ICONSTOP);
		TRACE("SHP File Error!!\n");
		return false;
	}
	
	// Now read a Header of SHP/SHX
	// Get File size & Number of records
	fseek(fpSHX, 24L, SEEK_SET);
	fread(&tmp, 4, 1, fpSHX);
	SwapWord(4, &tmp);	// Byte Order Changing
	int nFileSize = tmp * 2;	// get file size
	m_nRecords = (nFileSize - 100) / 8;	//get a number of records


	// get shp type
	fseek(fpSHP, 32L, SEEK_SET);
	fread(&m_nShapeType, 4, 1, fpSHP);
	

	// Check a invalid shp type
	// if(!(m_nShapeType == 1 || m_nShapeType == 3 || m_nShapeType == 5 || m_nShapeType == 8)) 
	if(!(m_nShapeType == SHPT_POINT || m_nShapeType == SHPT_ARC || m_nShapeType == SHPT_POLYGON || 
		m_nShapeType == SHPT_MULTIPOINT|| m_nShapeType == SHPT_POINTZ ||
		m_nShapeType == SHPT_ARCZ|| m_nShapeType == SHPT_POLYGONZ 		
		)) 
	{
		fclose(fpSHP);
		fclose(fpSHX);
		return false;
	}

	// get a MBR
	fread(&m_MBR, sizeof(MBR), 1, fpSHP);

	fclose(fpSHP);
	fclose(fpSHX);
	
	tmp = 0;
	fpSHP = NULL;
	fpSHX = NULL;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void shpObject::Draw(CDC* _pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio)
{
	// virtual functions..
}

void shpObject::DrawMemoryMap(CDC* _pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio)
{
	// virtual functions..
}

void shpObject::Draw(CDC* _pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio, 
					 CBitmap *pBitmap, COLORREF colorTransparent)
{
	// virtual functions..
}

void shpObject::Draw(CDC* _pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
					double dScreenScale, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2)
{
	// virtual functions..
}

void shpObject::Draw(CDC* _pDC, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
					double dScreenScale, int iScale, CString strName, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2)
{
	// virtual functions..
}

void shpObject::draw_polyline(CDC* _pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio)
{
	// virtual functions..
}
void shpObject::DrawSelected(CDC* _pDC, int iID)
{
	// virtual functions..
}

int shpObject::Track(CDC *pDC, void* pObject, GPoint point, CGRectGP* rectGP)
{
	return 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int shpObject::Track(CWnd* pWnd, GPoint point)
{
	return 1;
}

DBFFieldType shpObject::GetFieldInfo(int iField, char* pszFieldName, int* pnWidth, int* pnDecimals)
{
	return m_DBF.GetFieldInfo(iField, pszFieldName, pnWidth, pnDecimals);
}

void shpObject::SwapWord(int length, void* wordP)   
{
	int		i;
    unsigned char	temp;

    for( i=0; i<length/2; i++ )
    {
		temp = ((unsigned char *) wordP)[i];
		((unsigned char *)wordP)[i] = ((unsigned char *) wordP)[length-i-1];
		((unsigned char *)wordP)[length-i-1] = temp;
    }
}

void shpObject::SetScrCenterPoint(CRect rect)
{
//	m_scrCenterPoint.x = int(rect.Width() / 2);
//	m_scrCenterPoint.y = int(rect.Height() / 2);
	m_scrCenterPoint.x = (rect.right-rect.left)/2;
	m_scrCenterPoint.y = (rect.bottom-rect.top)/2;
}

void shpObject::SetPointType(UINT uiType, double dPointLen)
{
	m_nPointType = uiType;
	m_dPointLen = dPointLen;
}

BOOL shpObject::GetSelRect(int iSelID, double &left, double &top, double &right, double &bottom)
{
	return TRUE;
}