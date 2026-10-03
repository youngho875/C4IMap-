// shpObject.h: interface for the shpObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHPOBJECT_H__85DE2690_DA15_4711_9089_C632258E05DE__INCLUDED_)
#define AFX_SHPOBJECT_H__85DE2690_DA15_4711_9089_C632258E05DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "ProgressDlg.h"

#include "shpDBF.h"
#include "../inc/common.h"
#include "../inc/define_struct.h"


/**
* 벡터지도 shape data 관리 class
* @author 박종원
* @version 1.0
* @since 2005.08.
**/
class shpObject
{
public:
	shpObject();
	virtual ~shpObject();

	/** vector map file open */
	virtual bool	Open(CString _filename);
	virtual int		Track(CWnd* pWnd, GPoint point);
	/** 선택된 아이템 영역 얻기 */
	virtual BOOL	GetSelRect(int iSelID, double &left, double &top, double &right, double &bottom);
	/** Coordinate conversion between Geo Coordinate & Screen Coordinate */
	CRect			WorldToDevice(const MBR&	_mbr);
	/** Coordinate conversion between Geo Coordinate & Screen Coordinate */
	MBR			DeviceToWorld(const CRect& _rect);
	/** Coordinate conversion between Geo Coordinate & Screen Coordinate */
	CPoint			WorldToDevice(const GPoint& _GPoint);
	/** Coordinate conversion between Geo Coordinate & Screen Coordinate */
	GPoint			DeviceToWorld(const CPoint& _point);
	/** color set */
	void			SetColor(COLORREF _color){m_objColor = _color;}
	/** brush color set */
	void			SetBrushColor(COLORREF _color){m_brushColor = _color;}
	/** width set */
	void			SetWidth(int _nWidth){m_nWidth = _nWidth;}
	/** style set */
	void			SetStyle(int _nStyle){m_nStyle = _nStyle;}
	/** line type set */
	void			SetSolid(bool _bSolid){m_bSolidPolygon = _bSolid;}
	/** label set */
	void			SetLabel(int _nField){m_nLabelField = _nField; m_bLabel = true;}
	/** label clear */
	void			RemoveLabel(){m_bLabel = false;}
	/** shape file name get */
	CString			objFilename(){return m_strSHPPath;}
	
	// DB Related & and so on..
	/** MBR value get */
	MBR				GetMBR(){return m_MBR;}
	/** shape type get */
	int				GetSHPType(){return m_nShapeType;}
	/** record count get */
	int				GetRecordsNumber(){return m_nRecords;}
	/** field count get */
	int				GetFieldNumber(){return m_nField;}
	/** attribute infomation get */
	const char*		ReadStringAttribute(int _id, int _field){return m_DBF.ReadStringAttribute(_id, _field);}
	/** field infomation get */
	DBFFieldType	GetFieldInfo(int iField, char * pszFieldName, int * pnWidth, int * pnDecimals );
	/** center point set */
	void			SetCenterPoint(GPoint gpoint){m_geoCenterPoint = gpoint;};
	/** zoomfactor set */
	void			SetZoomFactor(double zoomfactor){m_lfZoomFactor = zoomfactor;};
	/** screen center point set */
	void			SetScrCenterPoint(CRect rect);
	/** ratio set */
	void			SetRatio(double dratio){m_lfRatio = dratio;};
	/** point type set */
	void			SetPointType(UINT uiType, double dPointLen);
public:
	virtual void Draw(CDC* _pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio);
	virtual void DrawMemoryMap(CDC* _pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio);
	virtual void Draw(CDC* _pDC, int nEllipsoid, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio, 
			 CBitmap *pBitmap, COLORREF colorTransparent);
	virtual void Draw(CDC* _pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
			double dScreenScale, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2);
	virtual void Draw(CDC* _pDC, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio,
			double dScreenScale, int iScale, CString strName, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2);
	virtual void draw_polyline(CDC* _pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rect, double _lfRatio);
	virtual void DrawSelected(CDC* _pDC, int iID);
	virtual int Track(CDC *pDC, void* pObject, GPoint point, CGRectGP* rectGP);

public:
	/** Label... */
	int				m_nLabelField;
	/** DBF file... */
	shpDBF			m_DBF;
	/** hit test할 때 결과 */
	enum			hit_status	{hitNone, hitPoint, hitCurve, hitPolygon};
	/** object status type */
	enum			object_status {
					/** none status */
					stateNone, 
					/** create status */
					stateCreate, 
					/** select status */
					stateSelect, 
					/** drag status */
					stateDragCurve, 
					/** point sel status */
					stateDraCGPoint };
	/** point list */
	typedef vector<GPoint>	POINT_LIST;
	typedef vector<CGPointGP> GPOINT_LIST;

	/** progress bar dialog pointer */
	//CProgressDlg	*m_pProgressDlg;		// young 삭제

protected:
	/** datat exchange */
	void			SwapWord(int length, void* wordP);
	/** MBR value update */
	void			ReMBR(vector<GPoint>& _vPoint);
	/** point값이 영역안에 있는지 체크 */
	bool			PtInRect(const GPoint& _geoInputPoint,const GPoint& _geoRectCenter);
	/** point값이 영역(폴리곤형태)안에 있는지 체크 */
	bool			PtInPolygon(vector<GPoint>& _vPoint, GPoint _GPoint);
	/** point값이 라인에 겹쳐져 있는지 체크 */
	bool			IsPointNearLine(const GPoint& _a, const GPoint& _b, const GPoint& _p, double _d);
	/** point값이 영역안에 있는지 체크 */
	bool			PtInRect(const CGPointGP& _geoInputPoint,const GPoint& _geoRectCenter);
	/** point값이 영역(폴리곤형태)안에 있는지 체크 */
	bool			PtInPolygon(int iSize, CGPointGP *gpointgp, GPoint _GPoint);
	/** point값이 라인에 겹쳐져 있는지 체크 */
	bool			IsPointNearLine(const CGPointGP& _a, const CGPointGP& _b, const GPoint& _p, double _d);

protected:
	/** select item id */
	vector<int>		m_vSelectedID;				// selected ID
	/** Name of SHP file */
	CString			m_strSHPPath;
	/** Name of SHX file */
	CString			m_strSHXPath;
	/** Name fo DBF file */
	CString			m_strDBFPath;
	/** shape type */
    int				m_nShapeType;
	/** Number of Records */
    int				m_nRecords;
	/** Number of Fields */
    int				m_nField;
	/** Label... */
//	int				m_nLabelField;
	// Appearance of point
	/** width */
	int				m_nWidth;
	/** solid pen, dotted pen and so on.. */
	int				m_nStyle;
	/** Color */
	COLORREF		m_objColor;
	/** brush color */
	COLORREF		m_brushColor;
	bool			m_bSolidPolygon;
	MBR			m_MBR;
	/** geoCenter Point */
	GPoint			m_geoCenterPoint;
	/** screen Center point */
	CPoint			m_scrCenterPoint;
	/** ratio between screen & map */
	double			m_lfRatio;
	/** Screen zoom factor */
	double			m_lfZoomFactor;

	/** selecting & Labeling.. */
	object_status	m_eStatus;
	/** selecting & Labeling.. */
	bool			m_bLabel;
	/** Tracking & Hitting test */
	int				m_nDetectPixelSize;
	// point type
	/** point type 0:circle, 1:rectangle, 2:symbol */
	UINT			m_nPointType;
	/** point length */
	double			m_dPointLen;
};


#endif // !defined(AFX_SHPOBJECT_H__85DE2690_DA15_4711_9089_C632258E05DE__INCLUDED_)
