#pragma once

#include <map>
#include <string>
#include "pointStruct.h"


class GDALDataset;
class GDALRasterBand;

typedef struct
{
	CString FullPath;
	CString FileName;
	CString GeoType;			//점, 선, 면
	CString MApType;			//전술조치선 or 비행기지지도
	GDALDataset* poDS;
	BOOL IsData;
}S_SHP_INFO;

typedef struct
{
	CString NO;
	CString CI;
	CString LayerName;
	CString Directory;
	CString GeoType;

	CString PointColor;
	CString PointSize;
	CString PointSymbol;
	CString LineColor;
	CString LineType;

	CString LineWidth;
	CString PolygonInner;
	CString PolygonInnerColor;
	CString PolygonInnerType;
}S_SHP_CONFIG;



class CShapeT
{
public:
	CShapeT();
	~CShapeT();

private:
	std::map< int, S_SHP_INFO > mmap_ShpInfo;
	typedef std::map< int, S_SHP_INFO >::iterator itr_ShpInfo;

	vector<MyPoint2D> PointLayer;
	vector<MultipointFeature> MultipointLayer;
	vector<LineFeature> LineLayer;
	vector<PolygonFeature> PolygonLayer;
	SBoundingBox sBoundingBox;

public:
	void SetBase();
	void OpenShapeFile();
	void PointshapeDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale);
	void PolylineshapeDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale);
	//void PolygonshapeDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale);
	void IndexMapDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale);

	void shapeDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale);

	void OpenDTEDFile(std::string filename);
	double valueAt(double lat, double lon);

	//void height(double lat, double lon);

private:
	unsigned width, height;
	double originX, originY, pixelSizeX, pixelSizeY;
	std::vector<float> data;

};

