#include "pch.h"
//#include "C4IMap.h"
#include "ShapeT.h"
#include "GPoint.h"
#include "GeoDC/CoordMgr.h"
#include "GeoDC/Coord.h"
#include "gdal_priv.h"
#include "ogr_feature.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include <string>
#include <iostream>

using namespace std;

CShapeT::CShapeT()
{
	OGRRegisterAll();
	SetBase();

}

CShapeT::~CShapeT()
{
	PointLayer.clear();
	MultipointLayer.clear();
	LineLayer.clear();
	PolygonLayer.clear();

	OGRCleanupAll();
}

static int id = 1;
void CShapeT::SetBase()
{
	CString filename = "";
	GDALDataset* poDS = NULL;
	S_SHP_INFO tmp;

	filename = "D:/map/small_scale_map/small_map.shp";  // D:/mapdata/worldmap/world.shp
	//filename = "D:/mapdata/worldmap/world.shp";
	poDS = static_cast<GDALDataset*>(GDALOpenEx(filename, GDAL_OF_VECTOR | GDAL_OF_READONLY, NULL, NULL, NULL));
	if (poDS != NULL)
	{
		tmp.poDS = poDS;
		tmp.IsData = true;
		tmp.GeoType = "miniMap";
	}
	else tmp.IsData = false;
	mmap_ShpInfo[id] = tmp;  ++id;

	//filename = "D:/mapdata/worldmap/world.shp";
	//poDS = static_cast<GDALDataset*>(GDALOpenEx(filename, GDAL_OF_VECTOR | GDAL_OF_READONLY, NULL, NULL, NULL));
	//if (poDS != NULL)
	//{
	//	tmp.poDS = poDS;
	//	tmp.IsData = true;
	//	tmp.GeoType = "MultiPolygon";
	//}
	//else tmp.IsData = false; 
	//mmap_ShpInfo[id] = tmp;  ++id;

	filename = "D:/map/small_scale_map/name_map.shp";
	poDS = static_cast<GDALDataset*>(GDALOpenEx(filename, GDAL_OF_VECTOR | GDAL_OF_READONLY, NULL, NULL, NULL));
	if (poDS != NULL)
	{
		tmp.poDS = poDS;
		tmp.IsData = true;
		tmp.GeoType = "point";
	}
	else tmp.IsData = false;
	mmap_ShpInfo[id] = tmp;  ++id;

}

void CShapeT::OpenShapeFile()
{
	GDALDataset* poDS;
	OGRErr error;

	itr_ShpInfo itr = mmap_ShpInfo.begin();
	for (; itr != mmap_ShpInfo.end(); ++itr)
	{
		if (itr->second.IsData == false) continue;

		poDS = itr->second.poDS;
		int cnt_layer = poDS->GetLayerCount();

		OGRLayer* poLayer;
		poLayer = poDS->GetLayer(cnt_layer - 1);
		OGREnvelope Envelope;
		error = poLayer->GetExtent(&Envelope, true);
		sBoundingBox.fMaxX = Envelope.MaxX;
		sBoundingBox.fMaxY = Envelope.MaxY;
		sBoundingBox.fMinX = Envelope.MinX;
		sBoundingBox.fMinY = Envelope.MinY;

		OGRwkbGeometryType LayerGeometryType = poLayer->GetGeomType();
		int NumberOfFeatures = poLayer->GetFeatureCount(true);
		poLayer->ResetReading();

		//Point Shapefile
		if (wkbFlatten(LayerGeometryType) == wkbPoint)
		{
			OGRFeature* poFeature;
			for (int i = 0; i < NumberOfFeatures; i++)
			{
				poFeature = poLayer->GetNextFeature();
				OGRGeometry* poGeometry;
				poGeometry = poFeature->GetGeometryRef();
				if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint)
				{
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(2,3,0)
					OGRPoint* poPoint = poGeometry->toPoint();
#else
					OGRPoint* poPoint = (OGRPoint*)poGeometry;
#endif
					MyPoint2D pt;
					pt.dX = poPoint->getX();
					pt.dY = poPoint->getY();
					PointLayer.push_back(pt);
				}
				OGRFeature::DestroyFeature(poFeature);
			}
		}

		//Multipoint Shapefile
		if (wkbFlatten(LayerGeometryType) == wkbMultiPoint)
		{
			OGRFeature* poFeature = nullptr;
			MultipointFeature MultiPoint;
			for (int i = 0; i < NumberOfFeatures; i++)
			{
				poFeature = poLayer->GetNextFeature();
				OGRGeometry* poGeometry;
				poGeometry = poFeature->GetGeometryRef();
				if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPoint)
				{
					OGRMultiPoint* poMultipoint = poGeometry->toMultiPoint();
					int NumberOfGeometries = poMultipoint->getNumGeometries();
					MultiPoint.PointsOfFeature.resize(NumberOfGeometries);
					for (int j = 0; j < NumberOfGeometries; j++)
					{
						OGRGeometry* poPointGeometry = poMultipoint->getGeometryRef(j);
						OGRPoint* poPoint = (OGRPoint*)poPointGeometry;
						MyPoint2D pt;
						pt.dX = poPoint->getX();
						pt.dY = poPoint->getY();
						MultiPoint.PointsOfFeature.at(j) = pt;
					}
					MultipointLayer.push_back(MultiPoint);
				}
				OGRFeature::DestroyFeature(poFeature);
			}
		}

		//Polyline Shapefile
		if (wkbFlatten(LayerGeometryType) == wkbLineString)
		{
			OGRFeature* poFeature = nullptr;
			LineFeature Polyline;
			OGRPoint ptTemp;
			for (int i = 0; i < NumberOfFeatures; i++)
			{
				poFeature = poLayer->GetNextFeature();
				OGRGeometry* poGeometry;
				poGeometry = poFeature->GetGeometryRef();
				if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
				{
					OGRLineString* poLineString = (OGRLineString*)poGeometry;
					Polyline.LinesOfFeature.resize(1);
					int NumberOfVertices = poLineString->getNumPoints();
					Polyline.LinesOfFeature.at(0).LineString.resize(NumberOfVertices);
					for (int k = 0; k < NumberOfVertices; k++)
					{
						poLineString->getPoint(k, &ptTemp);
						MyPoint2D pt;
						pt.dX = ptTemp.getX();
						pt.dY = ptTemp.getY();
						Polyline.LinesOfFeature.at(0).LineString.at(k) = pt;
					}
					LineLayer.push_back(Polyline);
				}
				else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString)
				{
					OGRMultiLineString* poMultiLineString = (OGRMultiLineString*)poGeometry;
					int NumberOfGeometries = poMultiLineString->getNumGeometries();
					Polyline.LinesOfFeature.resize(NumberOfGeometries);
					for (int j = 0; j < NumberOfGeometries; j++)
					{
						OGRGeometry* poLineGeometry = poMultiLineString->getGeometryRef(j);
						OGRLineString* poLineString = (OGRLineString*)poLineGeometry;
						int NumberOfVertices = poLineString->getNumPoints();
						Polyline.LinesOfFeature.at(j).LineString.resize(NumberOfVertices);
						for (int k = 0; k < NumberOfVertices; k++)
						{
							poLineString->getPoint(k, &ptTemp);
							MyPoint2D pt;
							pt.dX = ptTemp.getX();
							pt.dY = ptTemp.getY();
							Polyline.LinesOfFeature.at(j).LineString.at(k) = pt;
						}
					}
					LineLayer.push_back(Polyline);
				}
				OGRFeature::DestroyFeature(poFeature);
			}
		}

		//Polygon Shapefile
		if (wkbFlatten(LayerGeometryType) == wkbPolygon)
		{
			OGRFeature* poFeature = nullptr;
			PolygonFeature Polygon;
			OGRPoint ptTemp;
			for (int i = 0; i < NumberOfFeatures; i++)
			{
				poFeature = poLayer->GetNextFeature();
				OGRGeometry* poGeometry;
				poGeometry = poFeature->GetGeometryRef();
				if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
				{
					OGRPolygon* poPolygon = (OGRPolygon*)poGeometry;
					Polygon.PolygonsOfFeature.resize(1);
					int NumberOfInnerRings = poPolygon->getNumInteriorRings();
					OGRLinearRing* poExteriorRing = poPolygon->getExteriorRing();
					Polygon.PolygonsOfFeature.at(0).Polygon.resize(NumberOfInnerRings + 1);
					Polygon.PolygonsOfFeature.at(0).Polygon.at(0).IsClockwised = poExteriorRing->isClockwise();
					int NumberOfExteriorRingVertices = poExteriorRing->getNumPoints();
					Polygon.PolygonsOfFeature.at(0).Polygon.at(0).RingString.resize(NumberOfExteriorRingVertices);
					for (int k = 0; k < NumberOfExteriorRingVertices; k++)
					{
						poExteriorRing->getPoint(k, &ptTemp);
						MyPoint2D pt;
						pt.dX = ptTemp.getX();
						pt.dY = ptTemp.getY();
						Polygon.PolygonsOfFeature.at(0).Polygon.at(0).RingString.at(k) = pt;
					}
					for (int h = 1; h <= NumberOfInnerRings; h++)
					{
						OGRLinearRing* poInteriorRing = poPolygon->getInteriorRing(h - 1);
						Polygon.PolygonsOfFeature.at(0).Polygon.at(h).IsClockwised = poInteriorRing->isClockwise();
						int NumberOfInteriorRingVertices = poInteriorRing->getNumPoints();
						Polygon.PolygonsOfFeature.at(0).Polygon.at(h).RingString.resize(NumberOfInteriorRingVertices);
						for (int k = 0; k < NumberOfInteriorRingVertices; k++)
						{
							poInteriorRing->getPoint(k, &ptTemp);
							MyPoint2D pt;
							pt.dX = ptTemp.getX();
							pt.dY = ptTemp.getY();
							Polygon.PolygonsOfFeature.at(0).Polygon.at(h).RingString.at(k) = pt;
						}
					}
					PolygonLayer.push_back(Polygon);
				}
				else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon)
				{
					OGRMultiPolygon* poMultiPolygon = (OGRMultiPolygon*)poGeometry;
					int NumberOfGeometries = poMultiPolygon->getNumGeometries();
					Polygon.PolygonsOfFeature.resize(NumberOfGeometries);
					for (int j = 0; j < NumberOfGeometries; j++)
					{
						OGRGeometry* poPolygonGeometry = poMultiPolygon->getGeometryRef(j);
						OGRPolygon* poPolygon = (OGRPolygon*)poPolygonGeometry;
						int NumberOfInnerRings = poPolygon->getNumInteriorRings();
						OGRLinearRing* poExteriorRing = poPolygon->getExteriorRing();
						Polygon.PolygonsOfFeature.at(j).Polygon.resize(NumberOfInnerRings + 1);
						Polygon.PolygonsOfFeature.at(j).Polygon.at(0).IsClockwised = poExteriorRing->isClockwise();
						int NumberOfExteriorRingVertices = poExteriorRing->getNumPoints();
						Polygon.PolygonsOfFeature.at(j).Polygon.at(0).RingString.resize(NumberOfExteriorRingVertices);
						for (int k = 0; k < NumberOfExteriorRingVertices; k++)
						{
							poExteriorRing->getPoint(k, &ptTemp);
							MyPoint2D pt;
							pt.dX = ptTemp.getX();
							pt.dY = ptTemp.getY();
							Polygon.PolygonsOfFeature.at(j).Polygon.at(0).RingString.at(k) = pt;
						}
						for (int h = 1; h <= NumberOfInnerRings; h++)
						{
							OGRLinearRing* poInteriorRing = poPolygon->getInteriorRing(h - 1);
							Polygon.PolygonsOfFeature.at(j).Polygon.at(h).IsClockwised = poInteriorRing->isClockwise();
							int NumberOfInteriorRingVertices = poInteriorRing->getNumPoints();
							Polygon.PolygonsOfFeature.at(j).Polygon.at(h).RingString.resize(NumberOfInteriorRingVertices);
							for (int k = 0; k < NumberOfInteriorRingVertices; k++)
							{
								poInteriorRing->getPoint(k, &ptTemp);
								MyPoint2D pt;
								pt.dX = ptTemp.getX();
								pt.dY = ptTemp.getY();
								Polygon.PolygonsOfFeature.at(j).Polygon.at(h).RingString.at(k) = pt;
							}
						}
					}
					PolygonLayer.push_back(Polygon);
				}
			}
			OGRFeature::DestroyFeature(poFeature);
		}
	}
}

void CShapeT::shapeDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale)
{
	CCoordMgr* coord_mgr = GP_COORDMGR;
	CCoord coord;
	coord.initialize_adaptation_data();

	//CPen pen, * pPen;
	//pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	//pPen = pDC->SelectObject(&pen);
	//pDC->SelectStockObject(NULL_BRUSH);

	itr_ShpInfo itr = mmap_ShpInfo.begin();
	for (; itr != mmap_ShpInfo.end(); ++itr)
	{
		if(itr->second.IsData == false) continue;

		if(itr->second.GeoType == "point")
		{
			CPen pen, * pPen;
			pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 128));
			pPen = pDC->SelectObject(&pen);
			pDC->SelectStockObject(NULL_BRUSH);

			for (int i = 0; i < PointLayer.size(); i++)
			{
				CPoint ScreenXY;

				double geo_x = PointLayer.at(i).dX;
				double geo_y = PointLayer.at(i).dY;
				coord_mgr->WorldLonLat2ScreenXY(geo_x, geo_y, &ScreenXY.x, &ScreenXY.y, fCenterX, fCenterY, fScale);

				pDC->Ellipse(ScreenXY.x - 2, ScreenXY.y - 2, ScreenXY.x + 2, ScreenXY.y + 2);
			}
			pDC->SelectObject(pPen);
		}
		
		if (itr->second.GeoType == "miniMap")
		//if(itr->second.GeoType == "MultiPolygon")
		{
			CPen pen, * pPen;
			pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			pPen = pDC->SelectObject(&pen);
			pDC->SelectStockObject(NULL_BRUSH);

			//POINT* pPoints = NULL;
			for (int i = 0; i < PolygonLayer.size(); i++)
			{
				int cnt = PolygonLayer.at(i).PolygonsOfFeature.size();

				for (int j = 0; j < cnt; j++)
				{
					//POINT* pPoints = NULL;
					int pt = PolygonLayer.at(i).PolygonsOfFeature.at(j).Polygon.at(0).RingString.size();
					//std::cout << pt << std::endl;

					//if (_heapchk() != _HEAPOK)
					//	DebugBreak();

					POINT* pPoints = new POINT[pt];
                    double previousLongitude = 0;
					//if (_heapchk() != _HEAPOK)
					//	DebugBreak();

					for (int k = 0; k < pt; k++)
					{
						CPoint ScreenXY;

						double geo_x = PolygonLayer.at(i).PolygonsOfFeature.at(j).Polygon.at(0).RingString.at(k).dX;
						double geo_y = PolygonLayer.at(i).PolygonsOfFeature.at(j).Polygon.at(0).RingString.at(k).dY;
						if (coord_mgr->m_bFlat && k > 0) {
                            // Follow the short edge across +/-180, not a line across the map.
                            while (geo_x - previousLongitude > 180.0) geo_x -= 360.0;
                            while (geo_x - previousLongitude < -180.0) geo_x += 360.0;
                        }
                        previousLongitude = geo_x;
                        coord_mgr->WorldLonLat2ScreenXY(geo_x, geo_y, &ScreenXY.x, &ScreenXY.y, fCenterX, fCenterY, fScale);

						pPoints[k].x = ScreenXY.x;
						pPoints[k].y = ScreenXY.y;
					}
					pDC->Polygon(pPoints, pt);

					if (pPoints != NULL)
						delete[] pPoints;

				}

				//if (pPoints != NULL)
				//	delete[] pPoints;

			}
			pDC->SelectObject(pPen);

		}
	}
	//pDC->SelectObject(pPen);

}

void CShapeT::PolylineshapeDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale)
{
	CCoordMgr* coord_mgr = GP_COORDMGR;
	CCoord coord;
	coord.initialize_adaptation_data();

	CPen pen, * pPen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	pPen = pDC->SelectObject(&pen);
	pDC->SelectStockObject(NULL_BRUSH);

}

void CShapeT::PointshapeDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale)
{
	CCoordMgr* coord_mgr = GP_COORDMGR;
	CCoord coord;
	coord.initialize_adaptation_data();

	CPen pen, * pPen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	pPen = pDC->SelectObject(&pen);
	pDC->SelectStockObject(NULL_BRUSH);

	itr_ShpInfo itr = mmap_ShpInfo.begin();
	//for( int cnt = 0;  cnt < cnt_path; ++cnt )
	for (; itr != mmap_ShpInfo.end(); ++itr)
	{
		if (itr->second.IsData == false) continue;

		for (int i = 0; i < PointLayer.size(); i++)
		{
			CPoint ScreenXY;

			double geo_x = PointLayer.at(i).dX;
			double geo_y = PointLayer.at(i).dY;
			coord.degrees_lat_long_to_x_y(geo_y, geo_x, &geo_x, &geo_y);
			coord_mgr->WorldXY2ScreenXY(geo_x, geo_y, &ScreenXY.x, &ScreenXY.y, fCenterX, fCenterY, fScale);

			pDC->Ellipse(ScreenXY.x - 1, ScreenXY.y - 1, ScreenXY.x + 1, ScreenXY.y + 1);
		}
	}
	pDC->SelectObject(pPen);
}

void CShapeT::IndexMapDraw(CDC* pDC, float fCenterX, float fCenterY, float fScale)
{
	CCoordMgr* coord_mgr = GP_COORDMGR;
	CCoord coord;
	coord.initialize_adaptation_data();

	CPen pen, * pPen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	pPen = pDC->SelectObject(&pen);
	pDC->SelectStockObject(NULL_BRUSH);

/*
	for (const auto& polygonLayer : PolygonLayer) {
		for (const auto& feature : polygonLayer.PolygonsOfFeature) {
			int ptCount = feature.Polygon.at(0).RingString.size();
			std::vector<POINT> pPoints(ptCount);  // using vector instead of raw pointer

			for (int k = 0; k < ptCount; k++) {
				CPoint ScreenXY;
				double geo_x = feature.Polygon.at(0).RingString.at(k).dX;
				double geo_y = feature.Polygon.at(0).RingString.at(k).dY;
				coord.degrees_lat_long_to_x_y(geo_y, geo_x, &geo_x, &geo_y);
				coord_mgr->WorldXY2ScreenXY(geo_x, geo_y, &ScreenXY.x, &ScreenXY.y, fCenterX, fCenterY, fScale);

				pPoints[k].x = ScreenXY.x;
				pPoints[k].y = ScreenXY.y;
			}
			pDC->Polygon(pPoints.data(), ptCount);  // use data() for vector
		}
	}
	pDC->SelectObject(pPen);
*/

	//int p = 0;
	//POINT* pPoints = NULL;
	for (int i = 0; i < PolygonLayer.size(); i++)
	{
		//int p = 0;
		//POINT* pPoints = NULL;
		int cnt = PolygonLayer.at(i).PolygonsOfFeature.size();
		
		for (int j = 0; j < cnt; j++)
		{
			//int pt = 0;
			//POINT* pPoints = NULL;
			int pt = PolygonLayer.at(i).PolygonsOfFeature.at(j).Polygon.at(0).RingString.size();
			POINT* pPoints = new POINT[pt];

			for (int k = 0; k < pt; k++)
			{
				CPoint ScreenXY;

				double geo_x = PolygonLayer.at(i).PolygonsOfFeature.at(j).Polygon.at(0).RingString.at(k).dX;
				double geo_y = PolygonLayer.at(i).PolygonsOfFeature.at(j).Polygon.at(0).RingString.at(k).dY;
				coord.degrees_lat_long_to_x_y(geo_y, geo_x, &geo_x, &geo_y);
				coord_mgr->WorldXY2ScreenXY(geo_x, geo_y, &ScreenXY.x, &ScreenXY.y, fCenterX, fCenterY, fScale);

				pPoints[k].x = ScreenXY.x;
				pPoints[k].y = ScreenXY.y;
			}
			pDC->Polygon(pPoints, pt);

			if (pPoints != NULL)
			{
				delete[] pPoints;
				pPoints = nullptr;
			}

		}
		//pDC->Polygon(pPoints, cnt);

		//if (pPoints != NULL)
		//	delete[] pPoints;

	}
	pDC->SelectObject(pPen);

}

void CShapeT::OpenDTEDFile(std::string filename) 
{
	//CCoordMgr* coord_mgr = GP_COORDMGR;
	//CCoord coord;
	//coord.initialize_adaptation_data();

	//CPoint point;
	//GPoint		ptWorldPoint;
	//double adfGeoTransform[6];
	//float buffEleveationValue[1];
	//int nSizeX, nSizeY;
	//double noData = -9999;

	//GDALAllRegister();

	// Open the file
	GDALDataset* dataset = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);
	if (dataset == NULL) 
	{
		std::cout << "Failed to open" << std::endl;
		exit(1);
	}

	// Get image metadata
	width = dataset->GetRasterXSize();
	height = dataset->GetRasterYSize();

	std::cout << "Image is " << width << " x " << height << " px" << std::endl;

	//double originX, originY, pixelSizeX, pixelSizeY;

	double geoTransform[6];
	if (dataset->GetGeoTransform(geoTransform) == CE_None) 
	{
		originX = geoTransform[0];
		originY = geoTransform[3];
		pixelSizeX = geoTransform[1];
		pixelSizeY = geoTransform[5];
	}
	else 
	{
		std::cout << "Failed read geotransform" << std::endl;
		exit(1);
	}

	std::cout << "Origin: " << originX << ", " << originY << " degrees" << std::endl;
	std::cout << "Pixel size: " << pixelSizeX << ", " << pixelSizeY << " degrees" << std::endl;

	// pixelSizeY is negative because the origin of the image is the north-east corner and positive
	// Y pixel coordinates go towards the south

	// Get the raster band (DEM has one raster band representing elevation)
	GDALRasterBand* elevationBand = dataset->GetRasterBand(1);

	// Create an array of width*height 32-bit floats (~400MB memory)
	//std::vector<float> data(width * height, 0.0f);
	
	data.assign(width * height, 0.0f);
	// Read the entire file into the array (you can change the options to read only a portion
	// of the file, or even scale it down if you want)

	std::cout << "Loading array..." << std::flush;
	elevationBand->RasterIO(GF_Read, 0, 0, width, height, &data[0], width, height, GDT_Float32, 0, 0);
	std::cout << "done" << std::endl;

	// Close the file
	GDALClose(dataset);

/*
	//GDALRasterBand* poBand;

	GDALDataset* poDataset;
	GDALAllRegister();
	poDataset = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);

	printf("Driver: %s/%s\n", poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));
	printf("Size is %dx%dx%d\n", poDataset->GetRasterXSize(), poDataset->GetRasterYSize(), poDataset->GetRasterCount());

	if (poDataset->GetProjectionRef() != NULL)
		printf("Projection is `%s'\n", poDataset->GetProjectionRef());
	if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None)
	{
		printf("Origin = (%.6f,%.6f)\n", adfGeoTransform[0], adfGeoTransform[3]);
		printf("Pixel Size = (%.6f,%.6f)\n", adfGeoTransform[1], adfGeoTransform[5]);
	}

	poDataset->GetGeoTransform(adfGeoTransform);
	band = poDataset->GetRasterBand(1);

	nSizeX = band->GetXSize();
	nSizeY = band->GetYSize();
	noData = band->GetNoDataValue();

	GDALInvGeoTransform(adfGeoTransform, inverseTransform);
*/


	//std::vector<float> buffElevation(nSizeY * nSizeX);
/*
	double dLong, dLat;
	//int col_x, col_y;
	//for (int y = 0; y < nSizeY; y++)
	//{
	//	for (int x = 0; x < nSizeX; x++)
	//	{
			coord_mgr->ScreenXY2WorldXY(point.x, point.y, &ptWorldPoint.x, &ptWorldPoint.y, coord_mgr->m_fCenterX, coord_mgr->m_fCenterY, coord_mgr->m_fScale);
			coord.x_y_to_degrees_lat_long(ptWorldPoint.x, ptWorldPoint.y, &dLat, &dLong);

			//col_x = (int)((dLong - adfGeoTransform[0]) / (adfGeoTransform[1] + 1));
			//col_y = (int)((adfGeoTransform[0] - dLat) / (adfGeoTransform[1] + 1));

			//poBand->RasterIO(GF_Read, 0, 0, columns, rows, buffElevation.data(), columns, rows, GDT_Int16, 0, 0); // read the cell value
	//	}
	//}

	


			int  height = valueAt(poBand, dLat, dLong);

			//std::cout <<  "°íµµ : %d"  << height << std::endl;
*/
	
/*

	int col_x, col_y;

	col_x = (int)((dLong - adfGeoTransform[0]) / (adfGeoTransform[1] + 1));
	col_y = (int)((adfGeoTransform[0] - dLat) / (adfGeoTransform[1] + 1));


	//col_x = (int)((geo_x - adfGeoTransform[0]) / adfGeoTransform[1]);
	//col_y = (int)((adfGeoTransform[3] - geo_y) / -adfGeoTransform[5]);



	poBand->RasterIO(GF_Read, 0, 0, columns, rows, buffElevation.data(), columns, rows, GDT_Int16, 0, 0); // read the cell value
*/

}

double CShapeT::valueAt(double pointLat, double pointLon)
{
/*
	int x = static_cast<int>(floor(inverseTransform[0] + inverseTransform[1] * lon + inverseTransform[2] * lat));
	int y = static_cast<int>(floor(inverseTransform[3] + inverseTransform[4] * lon + inverseTransform[5] * lat));

	int32_t pixelValue;
	band->RasterIO(GF_Read, x, y, 1, 1, &pixelValue, 1, 1, GDT_Int32, 0, 0);

	//band->GetColorTable()->GetColorEntry(pixelValue);
	//int bSuccess;
	//double dfOffset = GDALGetRasterOffset(band, &bSuccess);
	//double dfScale = GDALGetRasterScale(band, &bSuccess);

	//pixelValue = pixelValue * dfScale + dfOffset;
	//height( lat,  lon);
*/


	double pixelX = (pointLon - originX) / pixelSizeX;
	double pixelY = (pointLat - originY) / pixelSizeY;

	std::cout << pointLat << "," << pointLon << " maps to pixel " << pixelX << "," << pixelY << std::endl;

	if (pixelX < 0 || pixelX > width || pixelY < 0 || pixelY > height) {
		std::cout << "Point out of bounds!" << std::endl;
		exit(1);
	}

	double elev = data[int(pixelX) + int(pixelY) * width];

	//std::cout << "Elevation there is " << elev << "m" << std::endl;

	return elev;


	//return pixelValue;
}


/*
#include <../proj9/proj.h> 
#include <iostream>

void CShapeT::height(double lat, double lon)
{
	auto proj_context = proj_context_create();
	//double lat = 44;
	//double lon = -78;
	double alt = 200;

	auto lla_to_ecef_proj = proj_create(proj_context, "+proj=latlong +ellps=WGS84 +datum=WGS84 "
		"+to +proj=geocentric");

	auto coord_4 = proj_trans(lla_to_ecef_proj, PJ_DIRECTION::PJ_FWD, PJ_COORD{ lat,lon, alt, 0 });

	std::cout << coord_4.v[0] << std::endl;
	std::cout << coord_4.v[1] << std::endl;
	std::cout << coord_4.v[2] << std::endl;

	proj_destroy(lla_to_ecef_proj);
	proj_context_destroy(proj_context);

}
*/