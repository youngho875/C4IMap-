// shpPoly.cpp: implementation of the shpPoly class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "shpPoly.h"
#include "MapDataMgr.h"
#include "GeoTrans/GGeoTrans.h"

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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

shpPolyLine::shpPolyLine()
{
	m_hFMap = m_hFMapShx = NULL;
}

shpPolyLine::~shpPolyLine()
{
	for(int i=0; i<m_paSHPPoly.GetSize(); i++)
	{
		for(int j=0; j<m_paSHPPoly[i]->m_paObject.GetSize(); j++)
		{
			if(m_paSHPPoly[i]->m_paObject[j]->m_nNumParts > 0)
				free(m_paSHPPoly[i]->m_paObject[j]->m_iParts);
			if(m_paSHPPoly[i]->m_paObject[j]->m_nNumPoints > 0)
				delete []m_paSHPPoly[i]->m_paObject[j]->m_gpPoints;
			delete m_paSHPPoly[i]->m_paObject[j];
		}
		m_paSHPPoly[i]->m_paObject.RemoveAll();
		delete m_paSHPPoly[i];
	}
	m_paSHPPoly.RemoveAll();
}

bool shpPolyLine::OpenEx(CString _filename, BOOL bIsCodeCon)
{
	if(!shpObject::Open(_filename))
 	return false;
	
	GPoint tmpPoint;
	int i, j, tmp, width, decimal;
	float fPercent = 0.0;
	char field[80];
	CString strText, strCode, strFeatureInfo, strFieldName, strPartCode[10];
	polyObject *SHPPoly;

	// File pointer..
	FILE* fpSHP = fopen(m_strSHPPath, "rb");
	FILE* fpSHX = fopen(m_strSHXPath, "rb");

	// young 삭제
	//if(m_pProgressDlg != NULL)
	//	m_pProgressDlg->ProgressStart(_filename, 1000);
	//int iCnt = m_nRecords / 100;
	//int iTotCnt = iCnt;

	//SEEK_CUR
	//Current position of file pointer. 
	//SEEK_END
	//End of file. 
	//SEEK_SET
	//Beginning of file. 

	for(i=0; i<m_nRecords; ++i)
	{
		SHPPoly = new polyObject;
		fseek(fpSHX, 100 + i * 8, SEEK_SET);
		fread(&tmp, 4, 1, fpSHX);
		SwapWord(4, &tmp);
		tmp=tmp*2;

		fseek(fpSHP, tmp+12, SEEK_SET);
		fread((MBR*)&SHPPoly->m_MBR, 32, 1, fpSHP);			// get MBR
		fread((int*)&SHPPoly->m_nNumParts, 4, 1, fpSHP);	// get parts number
		fread((int*)&SHPPoly->m_nNumPoints, 4, 1, fpSHP);	// get points number

		if(SHPPoly->m_nNumParts <= 0 || SHPPoly->m_nNumPoints <= 0)
		{
			delete SHPPoly;
			continue;
		}

		// part reading
		SHPPoly->m_iParts = (int*)malloc(sizeof(int) * SHPPoly->m_nNumParts);
		for(j=0; j<SHPPoly->m_nNumParts; ++j)
		{
			fread((int*)&tmp, 4, 1, fpSHP);
			SHPPoly->m_iParts[j] = tmp;
		}

		// point reading..
		SHPPoly->m_gpPoints = new CGPointGP[SHPPoly->m_nNumPoints];
		for(j=0; j<SHPPoly->m_nNumPoints; ++j)
		{
			fread((double*)&tmpPoint.x, 8, 1, fpSHP);
			fread((double*)&tmpPoint.y, 8, 1, fpSHP);
			SHPPoly->m_gpPoints[j].SetPoint(tmpPoint.x, tmpPoint.y);
		}
		SHPPoly->m_nID = i;

		strCode = _T("");

		// 속성 코드 조립
		if(bIsCodeCon)
		{
			for(j=0; i<10; i++)
				strPartCode[j] = _T("");
			for(j=0; j<GetFieldNumber(); ++j)
			{
				strFeatureInfo = ReadStringAttribute(i, j);
				GetFieldInfo(j, field, &width, &decimal);
				strFieldName = field;
				strFieldName.MakeUpper();

				if(strFieldName == "ACC")
					strPartCode[0] = strFeatureInfo;
				else if(strFieldName == "EXS")
					strPartCode[1] = strFeatureInfo;
				else if(strFieldName == "LOC")
					strPartCode[2] = strFeatureInfo;
				else if(strFieldName == "LTN")
					strPartCode[3] = strFeatureInfo;
				else if(strFieldName == "MED")
					strPartCode[4] = strFeatureInfo;
				else if(strFieldName == "RST")
					strPartCode[5] = strFeatureInfo;
				else if(strFieldName == "SGC")
					strPartCode[6] = strFeatureInfo;
				else if(strFieldName == "USE")
					strPartCode[7] = strFeatureInfo;
				else if(strFieldName == "WD1")
					strPartCode[8] = strFeatureInfo;
				else if(strFieldName == "WTC")
					strPartCode[9] = strFeatureInfo;
			}
			for(j=0; j<10; j++)
				strCode += strPartCode[j];

		}

		SHPPoly->m_strCode = strCode;

		AddSHPPolyArray(SHPPoly);

		// young 삭제
		//fPercent = (float)((float)((i+1)*100.0)/(float)m_nRecords);
		//if(m_pProgressDlg != NULL && iTotCnt <= i)
		//{
		//	strText = "";
		//	strText.Format("%3.1f", fPercent);
		//	strText += "% " + _filename;
		//	m_pProgressDlg->ProgressSetIt(strText);
		//}
		//if(i >= iTotCnt)
		//	iTotCnt = iTotCnt + iCnt;
	}

	fclose(fpSHP);
	fclose(fpSHX);

	//if(m_pProgressDlg != NULL)		// young 삭제
	//	m_pProgressDlg->ProgressEnd();

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void shpPolyLine::Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	m_lfZoomFactor = _zoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rctScreen.Width() / 2);
	m_scrCenterPoint.y = int(_rctScreen.Height() / 2);
	m_lfRatio = _lfRatio;

	CPoint pPoints;
	double lx, ly;

	CRect mbrRect, rectBuffer;
	CGGeoTrans trans(nEllipsoid);
	CGPointGP scrLT, scrBR;
	scrLT.SetPoint(&(_rctScreen.TopLeft()));
	scrBR.SetPoint(&(_rctScreen.BottomRight()));

	CGPointGP checkPointLT, checkPointRB;
	CGRectGP rtGP;
//	double xmin, xmax, ymin, ymax;
	stPolyObject *pObject;
	polyObject *pItem;

	// 객체 수만큼 그리기..
	int cnt_x = m_paSHPPoly.GetSize();
	for(int i=0; i < cnt_x; i++)
	{
		pObject = m_paSHPPoly[i];
		int cnt_y = pObject->m_paObject.GetSize();
		for(int j=0; j<cnt_y; j++)
		{
			pItem = pObject->m_paObject[j];

			/*
			xmin = pItem->m_MBR.xmin - 0.01;	// left
			xmax = pItem->m_MBR.xmax + 0.01;	// right
			ymin = pItem->m_MBR.ymin - 0.01;	// top
			ymax = pItem->m_MBR.ymax + 0.01;	// bottom

			checkPointLT.SetPoint(xmax, ymin);
			checkPointRB.SetPoint(xmin, ymax);
			rtGP.SetRect(&checkPointLT, &checkPointRB);

			if(rtGP.PtInRect(scrLT) && rtGP.PtInRect(scrBR))
			{
				;
			}
			*/

	//		else if(!_rctScreen.PtInRectHeight(checkPointLT) && !_rctScreen.PtInRectWidth(checkPointLT) 
	//		&& !_rctScreen.PtInRectHeight(checkPointRB) && !_rctScreen.PtInRectWidth(checkPointRB))
	//			continue;
	//		else
	//			;
			if(pItem->m_nNumPoints > 0)
			{
				m_Coord.degrees_lat_long_to_x_y (pItem->m_gpPoints->m_P.lat, pItem->m_gpPoints->m_P.lon, &lx, &ly); 
				m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints.x, &pPoints.y);

				//pDC->Polyline((POINT*)pItem->m_gpPoints, pItem->m_nNumPoints);
				pDC->Polyline(&pPoints, pItem->m_nNumPoints);
			}
			
		}
	}
}

// 복원도시(등고선 데이터만 해당)
void shpPolyLine::Draw(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio,
					   double dScreenScale, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	//return;
	m_lfZoomFactor = _zoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rctScreen.Width() / 2);
	m_scrCenterPoint.y = int(_rctScreen.Height() / 2);
	m_lfRatio = _lfRatio;

	CPoint pPoints;
	double lx, ly;

	CPen pen, *oldPen;
	CSize size, size1, size3;
	int iwidth;
	LOGBRUSH logBrush;
	DWORD *pStyle = NULL;

	// line width
	size.cx = iWidth1;
	pDC->DPtoLP(&size);

	int width, decimal, iQuot, iMod, ihight, itmp;
	char field[80];
	double H1, S1, L1, H2, S2, L2;
	CString strFeatureInfo, strFieldName;

	CGGeoTrans trans(nEllipsoid);

	CGPointGP scrLT, scrBR;
	scrLT.SetPoint(&(_rctScreen.TopLeft()));
	scrBR.SetPoint(&(_rctScreen.BottomRight()));

	CGPointGP gpXMin, gpXMax, gpYMin, gpYMax;
	CGPointGP checkPointLT, checkPointRB;
	CGRectGP rtGP;
	stPolyObject *pObject;
	polyObject *pItem;
	double xmin, xmax, ymin, ymax;

	for(int i=0; i<m_paSHPPoly.GetSize(); i++)
	{
		pObject = m_paSHPPoly[i];
		for(int j=0; j<pObject->m_paObject.GetSize(); j++)
		{
			pItem = pObject->m_paObject[j];

			xmin = pItem->m_MBR.xmin - 0.01;	// left
			xmax = pItem->m_MBR.xmax + 0.01;	// right
			ymin = pItem->m_MBR.ymin - 0.01;	// top
			ymax = pItem->m_MBR.ymax + 0.01;	// bottom

			checkPointLT.SetPoint(pItem->m_MBR.xmax, pItem->m_MBR.ymin);
			checkPointRB.SetPoint(pItem->m_MBR.xmin, pItem->m_MBR.ymax);
			rtGP.SetRect(&checkPointLT, &checkPointRB);

			if(rtGP.PtInRect(scrLT) && rtGP.PtInRect(scrBR))
			{
				;
			}
			else if(!_rctScreen.PtInRectHeight(checkPointLT) && !_rctScreen.PtInRectWidth(checkPointLT) 
			&& !_rctScreen.PtInRectHeight(checkPointRB) && !_rctScreen.PtInRectWidth(checkPointRB))
				continue;
			else
				;

			if(pItem->m_nNumPoints <= 0)
				continue;

		// 속성 정보 설정
			// 고도별 color 조정
			RGBtoHSL(color1, &H1, &S1, &L1);
			RGBtoHSL(color2, &H2, &S2, &L2);
			H1 *= 240.0;
			S1 *= 240.0;
			L1 *= 240.0;	// 명도 (0~240)
			L1 = 200.0;
			H2 *= 240.0;
			S2 *= 240.0;
			L2 *= 240.0;	// 명도 (0~240)
			L2 = 200.0;

			for(int k=0; k<GetFieldNumber(); ++k)
			{
				strFeatureInfo = ReadStringAttribute(pItem->m_nID, k);
				GetFieldInfo(k, field, &width, &decimal);
				strFieldName = field;
				if(strFieldName == "ZV2")
				{
					itmp = 9;
					ihight = atoi(strFeatureInfo);
					iQuot = ihight / 100;
					iMod = ihight % 100;
					iQuot--;
					if(iQuot < 0)
						iQuot = 0;
					if(iMod > 0)
						iQuot++;
					L1 -= (iQuot * itmp);
					L2 -= (iQuot * itmp);
					if(L1 > 240.0)
						L1 = 240.0;
					if(L2 > 240.0)
						L2 = 240.0;
					H1 /= 240.0;
					S1 /= 240.0;
					L1 /= 240.0;
					H2 /= 240.0;
					S2 /= 240.0;
					L2 /= 240.0;
					color1 = HSLtoRGB(H1, S1, L1);
					color2 = HSLtoRGB(H2, S2, L2);
					break;
				}
			}	// end for

			// line style
			switch(uiType1)
			{
				// solid
				case 0 : pen.CreatePen(PS_SOLID, size.cx, color1);
					break;
				// dash
				case 1 : pStyle = new DWORD[2];
						pStyle[0]= size.cx*10;
						pStyle[1]= size.cx*2;
						pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
										size.cx, &logBrush, 2, pStyle);
					break;
				// dot
				case 2 : pStyle = new DWORD[2];
						pStyle[0]= size.cx;
						pStyle[1]= size.cx;
						pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
										size.cx, &logBrush, 2, pStyle);
					break;
				// dashdot
				case 3 : pStyle = new DWORD[4];
						pStyle[0]= size.cx*10;
						pStyle[1]= size.cx;
						pStyle[2]= size.cx*2;
						pStyle[3]= size.cx;
						pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
										size.cx, &logBrush, 4, pStyle);
						break;
				// dashdotdot
				case 4 : pStyle = new DWORD[6];
						pStyle[0]= size.cx*10;
						pStyle[1]= size.cx;
						pStyle[2]= size.cx*2;
						pStyle[3]= size.cx;
						pStyle[4]= size.cx*2;
						pStyle[5]= size.cx;
						pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
										size.cx, &logBrush, 6, pStyle);
						break;
				// null line
				case 9 : break;
				// default
				default : pen.CreatePen(PS_SOLID, size.cx, color1);
					break;
			}	// end case

		// pattern 적용
			// 일반
			if(uiType2 != 999 && uiType2 <= 4)
			{
				::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
				size.cx = iWidth2;
				size.cy = iWidth2;
				pDC->DPtoLP(&size);
				iwidth = (int)(size.cx / dScreenScale);
				double Widlate = 0.21;
				int lgWidth = (int)((double)iwidth * Widlate);

				logBrush.lbStyle = BS_SOLID;
				logBrush.lbColor = color2;

				switch(uiType2)
				{
					// solid
					case 0 : pen.CreatePen(PS_SOLID, lgWidth, color2);
						break;
					// dash
					case 1 : pStyle = new DWORD[2];
							pStyle[0]= size.cx*10;
							pStyle[1]= size.cx*2;
							pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
											lgWidth, &logBrush, 2, pStyle);
						break;
					// dot
					case 2 : pStyle = new DWORD[2];
							pStyle[0]= size.cx;
							pStyle[1]= size.cx;
							pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
											lgWidth, &logBrush, 2, pStyle);
						break;
					// dashdot
					case 3 : pStyle = new DWORD[4];
							pStyle[0]= size.cx*10;
							pStyle[1]= size.cx;
							pStyle[2]= size.cx*2;
							pStyle[3]= size.cx;
							pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
											lgWidth, &logBrush, 4, pStyle);
							break;
					// dashdotdot
					case 4 : pStyle = new DWORD[6];
							pStyle[0]= size.cx*10;
							pStyle[1]= size.cx;
							pStyle[2]= size.cx*2;
							pStyle[3]= size.cx;
							pStyle[4]= size.cx*2;
							pStyle[5]= size.cx;
							pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
											lgWidth, &logBrush, 6, pStyle);
							break;
					// default
					default : pen.CreatePen(PS_SOLID, lgWidth, color2);
						break;
				}	// end case
			}	// end if

			oldPen = pDC->SelectObject(&pen);
			//pDC->Polyline((POINT*)pItem->m_gpPoints, pItem->m_nNumPoints);
			m_Coord.degrees_lat_long_to_x_y (pItem->m_gpPoints->m_P.lat, pItem->m_gpPoints->m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints.x, &pPoints.y);
			pDC->Polyline(&pPoints, pItem->m_nNumPoints);

			pDC->SelectObject(oldPen);
			pen.DeleteObject();
			delete [] pStyle;
			pStyle = NULL;

		}	// end for
	}	// end for
}

// 복원도시(도로 데이터만 해당)
void shpPolyLine::Draw(CDC* pDC, double _ZoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio,
						double dScreenScale, int iScale, CString strName, UINT uiType1, COLORREF color1, int iWidth1, UINT uiType2, COLORREF color2, int iWidth2)
{
	m_lfZoomFactor = _ZoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rctScreen.Width() / 2);
	m_scrCenterPoint.y = int(_rctScreen.Height() / 2);
	m_lfRatio = _lfRatio;

	CString strFeatureInfo, strFieldName, strCode, strPartCode[10];

	CGPointGP scrLT, scrBR;
	scrLT.SetPoint(&(_rctScreen.TopLeft()));
	scrBR.SetPoint(&(_rctScreen.BottomRight()));
	stPolyObject *pObject;
	enLineForm enType;

	for(int i=0; i<m_paSHPPoly.GetSize(); ++i)
	{
		pObject = m_paSHPPoly[i];
		enType = LF_UNKNOWN;


		////CHOIKYUBEOM LOAD DATA DRAW TEST
		enType = LF_FORM1;
		shpDrawRoad(pDC, enType, pObject, dScreenScale, scrLT, scrBR, _rctScreen, 
				iWidth1, color1, uiType1, iWidth2, color2, uiType2);
		continue;






		switch(iScale)
		{
		// 5만
		case 1 : 
			if(strName == "도로(기본)" && 
				(pObject->m_strCode != "128820100801" && pObject->m_strCode != "128250010001" &&
				pObject->m_strCode != "1582010001" && pObject->m_strCode != "128820200801" &&
				pObject->m_strCode != "128812100501" && pObject->m_strCode != "128250010001" &&
				pObject->m_strCode != "1581010001" && pObject->m_strCode != "128810200501" &&
				pObject->m_strCode != "12881020002" && pObject->m_strCode != "12881020002" &&
				pObject->m_strCode != "128820104801" && pObject->m_strCode != "128252010401" &&
				pObject->m_strCode != "1582010401" && pObject->m_strCode != "1288411041601" &&
				pObject->m_strCode != "128254110401" && pObject->m_strCode != "1584010401" &&
				pObject->m_strCode != "12886110401" && pObject->m_strCode != "128256110401" &&
				pObject->m_strCode != "1586110401" && pObject->m_strCode != "12888110401" &&
				pObject->m_strCode != "128258110401" && pObject->m_strCode != "1588110401"))
			{
				enType = LF_FORM3;
			}
			// 1등도로(기호)
			else if(strName == "1등도로(기호)" && pObject->m_strCode == "128820100801")
			{
				enType = LF_FORM3;
			}
			// 1등도로(건설중)
			else if(strName == "1등도로(건설중)" && pObject->m_strCode == "1582010001")
			{
				enType = LF_FORM3;
			}
			// 2등도로(기호)
			else if(strName == "2등도로(기호)" && pObject->m_strCode == "128820200801")
			{
				enType = LF_FORM3;
			}
			// 2등도로(건설중)
			else if(strName == "2등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 3등도로(기호)
			else if(strName == "3등도로(기호)" && 
				(pObject->m_strCode == "128812100501" || pObject->m_strCode == "128250010001"))
			{
				enType = LF_FORM3;
			}
			// 3등도로(건설중)
			else if(strName == "3등도로(건설중)" && pObject->m_strCode == "1581010001")
			{
				enType = LF_FORM3;
			}
			// 4등도로(기호)
			else if(strName == "4등도로(기호)" && pObject->m_strCode == "128810200501")
			{
				enType = LF_FORM3;
			}
			// 4등도로(건설중)
			else if(strName == "4등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 5등도로(기호)
			else if(strName == "5등도로(기호)" && pObject->m_strCode == "12881020002")
			{
				enType = LF_FORM3;
			}
			// 5등도로(건설중)
			else if(strName == "5등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 6등도로(기호)
			else if(strName == "6등도로(기호)" && pObject->m_strCode == "12881020002")
			{
				enType = LF_FORM3;
			}
			// 6등도로(건설중)
			else if(strName == "6등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2차선고속도로(기호)
			if(strName == "2차선고속도로" && 
				(pObject->m_strCode == "128820104801" || pObject->m_strCode == "128252010401"))
			{
				enType = LF_FORM5;
			}
			// 2차선고속도로(건설중)
			else if(strName == "2차선고속도로(건설중)" && pObject->m_strCode == "1582010401")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(기호)
			else if(strName == "4차선고속도로" && 
				(pObject->m_strCode == "1288411041601" || pObject->m_strCode == "128254110401"))
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(건설중)
			else if(strName == "4차선고속도로(건설중)" && pObject->m_strCode == "1584010401")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(기호)
			else if(strName == "6차선고속도로" && 
				(pObject->m_strCode == "12886110401" || pObject->m_strCode == "128256110401"))
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(건설중)
			else if(strName == "6차선고속도로(건설중)" && pObject->m_strCode == "1586110401")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(기호)	
			else if(strName == "8차선고속도로" && 
				(pObject->m_strCode == "12888110401" || pObject->m_strCode == "128258110401"))
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(건설중)
			else if(strName == "8차선고속도로(건설중)" && pObject->m_strCode == "1588110401")
			{
				enType = LF_FORM5;
			}
			break;
		// 10만
		case 2 : 
			if(strName == "도로(기본)" && 
				(pObject->m_strCode != "12881210501" && pObject->m_strCode != "128812100501" &&
				pObject->m_strCode != "128810200501" && pObject->m_strCode != "12881020002" &&
				pObject->m_strCode != "12881020002" && pObject->m_strCode != "1581010001" &&
				pObject->m_strCode != "1081000000" && pObject->m_strCode != "1282510113001" &&
				pObject->m_strCode != "1288411041601"))
			{
				enType = LF_FORM3;
			}
			// 1등도로(기호)
			else if(strName == "1등도로(기호)" && pObject->m_strCode == "12881210501")
			{
				enType = LF_FORM3;
			}
			// 1등도로(건설중)
			else if(strName == "1등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2등도로(기호)
			else if(strName == "2등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2등도로(건설중)
			else if(strName == "2등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 3등도로(기호)
			else if(strName == "3등도로(기호)" && 
				(pObject->m_strCode == "128812100501" || pObject->m_strCode == "1282510113001" ||
				pObject->m_strCode == "1288411041601"))
			{
				enType = LF_FORM3;
			}
			// 3등도로(건설중)
			else if(strName == "3등도로(건설중)" && pObject->m_strCode == "1581010001")
			{
				enType = LF_FORM3;
			}
			// 4등도로(기호)
			else if(strName == "4등도로(기호)" && pObject->m_strCode == "128810200501")
			{
				enType = LF_FORM3;
			}
			// 4등도로(건설중)
			else if(strName == "4등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 5등도로(기호)
			else if(strName == "5등도로(기호)" && pObject->m_strCode == "12881020002")
			{
				enType = LF_FORM3;
			}
			// 5등도로(건설중)
			else if(strName == "5등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 6등도로(기호)
			else if(strName == "6등도로(기호)" && pObject->m_strCode == "12881020002")
			{
				enType = LF_FORM3;
			}
			// 6등도로(건설중)
			else if(strName == "6등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2차선고속도로(기호)
			if(strName == "2차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 2차선고속도로(건설중)
			else if(strName == "2차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(기호)
			else if(strName == "4차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(건설중)
			else if(strName == "4차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(기호)
			else if(strName == "6차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(건설중)
			else if(strName == "6차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(기호)
			else if(strName == "8차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(건설중)
			else if(strName == "8차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			break;
		// 25만
		case 3 : 
			if(strName == "도로(기본)" && 
				(pObject->m_strCode != "128822113401" && pObject->m_strCode != "128812113501" &&
				pObject->m_strCode != "128822113501" && pObject->m_strCode != "128822314401" &&
				pObject->m_strCode != "128812314501" && pObject->m_strCode != "128822315401" &&
				pObject->m_strCode != "128822314501" && pObject->m_strCode != "128812113401" &&
				pObject->m_strCode != "128812314401" && pObject->m_strCode != "128822214402" &&
				pObject->m_strCode != "128822214502" && pObject->m_strCode != "128841113401"))
			{
				enType = LF_FORM3;
			}
			// 1등도로(기호)
			else if(strName == "1등도로(기호)" && pObject->m_strCode == "128822113401")
			{
				enType = LF_FORM3;
			}
			// 1등도로(건설중 / 지하)
			else if(strName == "1등도로(건설중)" && 
				(pObject->m_strCode == "128812113501" || pObject->m_strCode == "128822113501"))
			{
				enType = LF_FORM3;
			}
			// 2등도로(기호)
			else if(strName == "2등도로(기호)" && pObject->m_strCode == "128822314401")
			{
				enType = LF_FORM3;
			}
			// 2등도로(건설중)
			else if(strName == "2등도로(건설중)" && 
				(pObject->m_strCode == "128812314501" || pObject->m_strCode == "128822315401"))
			{
				enType = LF_FORM3;
			}
			// 3등도로(기호)
			else if(strName == "3등도로(기호)" && 
				(pObject->m_strCode == "128822314501" || pObject->m_strCode == "128812113401"))
			{
				enType = LF_FORM3;
			}
			// 3등도로(건설중)
			else if(strName == "3등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 4등도로(기호)
			else if(strName == "4등도로(기호)" && pObject->m_strCode == "128812314401")
			{
				enType = LF_FORM3;
			}
			// 4등도로(건설중)
			else if(strName == "4등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 5등도로(기호)
			else if(strName == "5등도로(기호)" && pObject->m_strCode == "128822214402")
			{
				enType = LF_FORM3;
			}
			// 5등도로(건설중)
			else if(strName == "5등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 6등도로(기호)
			else if(strName == "6등도로(기호)" && pObject->m_strCode == "128822214502")
			{
				enType = LF_FORM3;
			}
			// 6등도로(건설중)
			else if(strName == "6등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2차선고속도로(기호)
			if(strName == "2차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 2차선고속도로(건설중)
			else if(strName == "2차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(기호)
			else if(strName == "4차선고속도로" && pObject->m_strCode == "128841113401")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(건설중)
			else if(strName == "4차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(기호)
			else if(strName == "6차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(건설중)
			else if(strName == "6차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(기호)
			else if(strName == "8차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(건설중)
			else if(strName == "8차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			break;
		// 50만
		case 4 : 
			if(strName == "도로(기본)" && 
				(pObject->m_strCode != "1282" && pObject->m_strCode != "1281" &&
				pObject->m_strCode != "151"))
			{
				enType = LF_FORM3;
			}
			// 1등도로(기호)
			else if(strName == "1등도로(기호)" && pObject->m_strCode == "1282")
			{
				enType = LF_FORM3;
			}
			// 1등도로(건설중)
			else if(strName == "1등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2등도로(기호)
			else if(strName == "2등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2등도로(건설중)
			else if(strName == "2등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 3등도로(기호)
			else if(strName == "3등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 3등도로(건설중)
			else if(strName == "3등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 4등도로(기호)
			else if(strName == "4등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 4등도로(건설중)
			else if(strName == "4등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 5등도로(기호)
			else if(strName == "5등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 5등도로(건설중)
			else if(strName == "5등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 6등도로(기호)
			else if(strName == "6등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 6등도로(건설중)
			else if(strName == "6등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2차선고속도로(기호)
			if(strName == "2차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 2차선고속도로(건설중)
			else if(strName == "2차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(기호)
			else if(strName == "4차선고속도로" && pObject->m_strCode == "1281")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(건설중)
			else if(strName == "4차선고속도로(건설중)" && pObject->m_strCode == "151")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(기호)
			else if(strName == "6차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(건설중)
			else if(strName == "6차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(기호)
			else if(strName == "8차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(건설중)
			else if(strName == "8차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			break;
		// 100만
		case 5 : 
			if(strName == "도로(기본)" && 
				(pObject->m_strCode != "128214" && pObject->m_strCode != "128114" &&
				pObject->m_strCode != "15114"))
			{
				enType = LF_FORM3;
			}
			// 1등도로(기호)
			else if(strName == "1등도로(기호)" && pObject->m_strCode == "128214")
			{
				enType = LF_FORM3;
			}
			// 1등도로(건설중)
			else if(strName == "1등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2등도로(기호)
			else if(strName == "2등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2등도로(건설중)
			else if(strName == "2등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 3등도로(기호)
			else if(strName == "3등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 3등도로(건설중)
			else if(strName == "3등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 4등도로(기호)
			else if(strName == "4등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 4등도로(건설중)
			else if(strName == "4등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 5등도로(기호)
			else if(strName == "5등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 5등도로(건설중)
			else if(strName == "5등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 6등도로(기호)
			else if(strName == "6등도로(기호)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 6등도로(건설중)
			else if(strName == "6등도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM3;
			}
			// 2차선고속도로(기호)
			if(strName == "2차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 2차선고속도로(건설중)
			else if(strName == "2차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(기호)
			else if(strName == "4차선고속도로" && pObject->m_strCode == "128114")
			{
				enType = LF_FORM5;
			}
			// 4차선고속도로(건설중)
			else if(strName == "4차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(기호)
			else if(strName == "6차선고속도로" && pObject->m_strCode == "15114")
			{
				enType = LF_FORM5;
			}
			// 6차선고속도로(건설중)
			else if(strName == "6차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(기호)
			else if(strName == "8차선고속도로" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			// 8차선고속도로(건설중)
			else if(strName == "8차선고속도로(건설중)" && pObject->m_strCode == "")
			{
				enType = LF_FORM5;
			}
			break;
		}	// end case

		if(enType != LF_UNKNOWN)
		{
			shpDrawRoad(pDC, enType, pObject, dScreenScale, scrLT, scrBR, _rctScreen, 
				iWidth1, color1, uiType1, iWidth2, color2, uiType2);
		}
	} // end for
}

// 도로 복원도시
void shpPolyLine::shpDrawRoad(CDC* pDC, enLineForm lineform, stPolyObject *pObject, double dScreenScale, CGPointGP scrLT, CGPointGP scrBR, CGRectGP _rctScreen, 
							int iWidth1, COLORREF color1, UINT uiType1, int iWidth2, COLORREF color2, UINT uiType2)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	CPoint pPoints, pPoints1;
	double lx, ly, dx, dy;

	CPen pen, *oldPen;
	CSize size, size1, size3;
	int iwidth, lgWidth;
	LOGBRUSH logBrush;
	double Widlate;
	DWORD *pStyle = NULL;
	polyObject *pItem;
	CGPointGP gpXMin, gpXMax, gpYMin, gpYMax;
	CGPointGP checkPointLT, checkPointRB;
	CGRectGP rtGP;
	int i;
	double xmin, xmax, ymin, ymax;

	switch(lineform)
	{
	// 단선
	case LF_FORM1 : 

		size.cx = iWidth1;
		size.cy = iWidth1;
		pDC->DPtoLP(&size);
		iwidth = (int)(size.cx / dScreenScale);

		Widlate = 0.21;
		lgWidth = (int)((double)iwidth * Widlate);

		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = color2;

		switch(uiType1)
		{
			// solid
			case 0 : pen.CreatePen(PS_SOLID, lgWidth, color1);
				break;
			// dash
			case 1 : pStyle = new DWORD[2];
					pStyle[0]= size.cx*2;
					pStyle[1]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									lgWidth, &logBrush, 2, pStyle);
				break;
			// dot
			case 2 : pStyle = new DWORD[2];
					pStyle[0]= size.cx;
					pStyle[1]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									lgWidth, &logBrush, 2, pStyle);
				break;
			// dashdot
			case 3 : pStyle = new DWORD[4];
					pStyle[0]= size.cx*10;
					pStyle[1]= size.cx;
					pStyle[2]= size.cx*2;
					pStyle[3]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									lgWidth, &logBrush, 4, pStyle);
					break;
			// dashdotdot
			case 4 : pStyle = new DWORD[6];
					pStyle[0]= size.cx*10;
					pStyle[1]= size.cx;
					pStyle[2]= size.cx*2;
					pStyle[3]= size.cx;
					pStyle[4]= size.cx*2;
					pStyle[5]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									lgWidth, &logBrush, 6, pStyle);
					break;
			// default
			default : pen.CreatePen(PS_SOLID, lgWidth, color1);
				break;
		}	// end case

		oldPen = pDC->SelectObject(&pen);

		for(i=0; i<pObject->m_paObject.GetSize(); i++)
		{
			pItem = pObject->m_paObject[i];

			xmin = pItem->m_MBR.xmin - 0.01;	// left
			xmax = pItem->m_MBR.xmax + 0.01;	// right
			ymin = pItem->m_MBR.ymin - 0.01;	// top
			ymax = pItem->m_MBR.ymax + 0.01;	// bottom

			checkPointLT.SetPoint(pItem->m_MBR.xmax, pItem->m_MBR.ymin);
			checkPointRB.SetPoint(pItem->m_MBR.xmin, pItem->m_MBR.ymax);
			rtGP.SetRect(&checkPointLT, &checkPointRB);

			if(rtGP.PtInRect(scrLT) && rtGP.PtInRect(scrBR))
			{
				;
			}
			else if(!_rctScreen.PtInRectHeight(checkPointLT) && !_rctScreen.PtInRectWidth(checkPointLT) 
			&& !_rctScreen.PtInRectHeight(checkPointRB) && !_rctScreen.PtInRectWidth(checkPointRB))
				continue;
			else
				;

			if(pItem->m_nNumPoints <= 0)
				continue;
			//pDC->Polyline((POINT*)pItem->m_gpPoints, pItem->m_nNumPoints);
			
			m_Coord.degrees_lat_long_to_x_y (pItem->m_gpPoints->m_P.lat, pItem->m_gpPoints->m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints.x, &pPoints.y);
			pDC->Polyline(&pPoints, pItem->m_nNumPoints);

		}

		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		delete [] pStyle;
		pStyle = NULL;
		
		break;
	// 복선(2선)
	case LF_FORM2 : 
		break;
	// 기둥형태(일반)
	case LF_FORM3 : 
	// 기둥형태(3선)
	case LF_FORM5 : 
		// 외곽 처리
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		size.cx = iWidth1;
		size.cy = iWidth1;
		pDC->DPtoLP(&size);
		logBrush.lbColor = color1;
		logBrush.lbHatch = 10;
		logBrush.lbStyle = BS_SOLID;

		// line style
		switch(uiType1)
		{
			// solid
			case 0 : pen.CreatePen(PS_INSIDEFRAME, size.cx, color1);
				break;
			// dash
			case 1 : pStyle = new DWORD[2];
					pStyle[0]= size.cx*2;
					pStyle[1]= float(size.cx)*0.7;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									size.cx, &logBrush, 2, pStyle);
				break;
			// dot
			case 2 : pStyle = new DWORD[2];
					pStyle[0]= size.cx;
					pStyle[1]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									size.cx, &logBrush, 2, pStyle);
				break;
			// dashdot
			case 3 : pStyle = new DWORD[4];
					pStyle[0]= size.cx*10;
					pStyle[1]= size.cx;
					pStyle[2]= size.cx*2;
					pStyle[3]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									size.cx, &logBrush, 4, pStyle);
					break;
			// dashdotdot
			case 4 : pStyle = new DWORD[6];
					pStyle[0]= size.cx*10;
					pStyle[1]= size.cx;
					pStyle[2]= size.cx*2;
					pStyle[3]= size.cx;
					pStyle[4]= size.cx*2;
					pStyle[5]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									size.cx, &logBrush, 6, pStyle);
					break;
			// null line
			case 9 : break;
			// default
			default : pen.CreatePen(PS_SOLID, size.cx, color1);
				break;
		}
		oldPen = pDC->SelectObject(&pen);

		for(i=0; i<pObject->m_paObject.GetSize(); i++)
		{
			pItem = pObject->m_paObject[i];

			xmin = pItem->m_MBR.xmin - 0.01;	// left
			xmax = pItem->m_MBR.xmax + 0.01;	// right
			ymin = pItem->m_MBR.ymin - 0.01;	// top
			ymax = pItem->m_MBR.ymax + 0.01;	// bottom

			checkPointLT.SetPoint(pItem->m_MBR.xmax, pItem->m_MBR.ymin);
			checkPointRB.SetPoint(pItem->m_MBR.xmin, pItem->m_MBR.ymax);
			rtGP.SetRect(&checkPointLT, &checkPointRB);

			if(rtGP.PtInRect(scrLT) && rtGP.PtInRect(scrBR))
			{
				;
			}
			else if(!_rctScreen.PtInRectHeight(checkPointLT) && !_rctScreen.PtInRectWidth(checkPointLT) 
			&& !_rctScreen.PtInRectHeight(checkPointRB) && !_rctScreen.PtInRectWidth(checkPointRB))
				continue;
			else
				;
			if(pItem->m_nNumPoints <= 0)
				continue;

			//pDC->Polyline((POINT*)pItem->m_gpPoints, pItem->m_nNumPoints);
			m_Coord.degrees_lat_long_to_x_y (pItem->m_gpPoints->m_P.lat, pItem->m_gpPoints->m_P.lon, &dx, &dy); 
			m_CoordMgr->WorldXY2ScreenXY(dx, dy, &pPoints1.x, &pPoints1.y);
			pDC->Polyline(&pPoints1, pItem->m_nNumPoints);

		}

		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		delete [] pStyle;
		pStyle = NULL;

		// 내부 처리
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		size.cx = iWidth2;
		size.cy = iWidth2;
		pDC->DPtoLP(&size);
		logBrush.lbColor = color2;
		logBrush.lbHatch = 10;
		logBrush.lbStyle = BS_SOLID;

		switch(uiType2)
		{
			// solid
			case 0 : pen.CreatePen(PS_SOLID, size.cx, color2);
				break;
			// dash
			case 1 : pStyle = new DWORD[2];
					pStyle[0]= size.cx*3;
					pStyle[1]= size.cx;
//					pStyle[0]= size.cx;
//					pStyle[1]= size.cx*2;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									size.cx, &logBrush, 2, pStyle);
				break;
			// dot
			case 2 : pStyle = new DWORD[2];
					pStyle[0]= size.cx;
					pStyle[1]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									size.cx, &logBrush, 2, pStyle);
				break;
			// dashdot
			case 3 : pStyle = new DWORD[4];
					pStyle[0]= size.cx*10;
					pStyle[1]= size.cx;
					pStyle[2]= size.cx*2;
					pStyle[3]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									size.cx, &logBrush, 4, pStyle);
					break;
			// dashdotdot
			case 4 : pStyle = new DWORD[6];
					pStyle[0]= size.cx*10;
					pStyle[1]= size.cx;
					pStyle[2]= size.cx*2;
					pStyle[3]= size.cx;
					pStyle[4]= size.cx*2;
					pStyle[5]= size.cx;
					pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
									size.cx, &logBrush, 6, pStyle);
					break;
			// default
			default : pen.CreatePen(PS_SOLID, size.cx, color2);
				break;
		}	// end case

		oldPen = pDC->SelectObject(&pen);

		for(i=0; i<pObject->m_paObject.GetSize(); i++)
		{
			pItem = pObject->m_paObject[i];

			xmin = pItem->m_MBR.xmin - 0.01;	// left
			xmax = pItem->m_MBR.xmax + 0.01;	// right
			ymin = pItem->m_MBR.ymin - 0.01;	// top
			ymax = pItem->m_MBR.ymax + 0.01;	// bottom

			checkPointLT.SetPoint(pItem->m_MBR.xmax, pItem->m_MBR.ymin);
			checkPointRB.SetPoint(pItem->m_MBR.xmin, pItem->m_MBR.ymax);
			rtGP.SetRect(&checkPointLT, &checkPointRB);

			if(rtGP.PtInRect(scrLT) && rtGP.PtInRect(scrBR))
			{
				;
			}
			else if(!_rctScreen.PtInRectHeight(checkPointLT) && !_rctScreen.PtInRectWidth(checkPointLT) 
			&& !_rctScreen.PtInRectHeight(checkPointRB) && !_rctScreen.PtInRectWidth(checkPointRB))
				continue;
			else
				;
			if(pItem->m_nNumPoints <= 0)
				continue;
			//pDC->Polyline((POINT*)pItem->m_gpPoints, pItem->m_nNumPoints);
			m_Coord.degrees_lat_long_to_x_y (pItem->m_gpPoints->m_P.lat, pItem->m_gpPoints->m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints.x, &pPoints.y);
			pDC->Polyline(&pPoints, pItem->m_nNumPoints);
		}
		
		pDC->SelectObject(oldPen);
		pen.DeleteObject();

		delete [] pStyle;
		pStyle = NULL;
		
		if(lineform == LF_FORM5)
		{
			size.cx = 1;
			pDC->DPtoLP(&size);
			pen.CreatePen(PS_SOLID, size.cx, color1);
			oldPen = pDC->SelectObject(&pen);
			for(i=0; i<pObject->m_paObject.GetSize(); i++)
			{
				pItem = pObject->m_paObject[i];

				xmin = pItem->m_MBR.xmin - 0.01;	// left
				xmax = pItem->m_MBR.xmax + 0.01;	// right
				ymin = pItem->m_MBR.ymin - 0.01;	// top
				ymax = pItem->m_MBR.ymax + 0.01;	// bottom

				checkPointLT.SetPoint(pItem->m_MBR.xmax, pItem->m_MBR.ymin);
				checkPointRB.SetPoint(pItem->m_MBR.xmin, pItem->m_MBR.ymax);
				rtGP.SetRect(&checkPointLT, &checkPointRB);

				if(rtGP.PtInRect(scrLT) && rtGP.PtInRect(scrBR))
				{
					;
				}
				else if(!_rctScreen.PtInRectHeight(checkPointLT) && !_rctScreen.PtInRectWidth(checkPointLT) 
				&& !_rctScreen.PtInRectHeight(checkPointRB) && !_rctScreen.PtInRectWidth(checkPointRB))
					continue;
				else
					;
				if(pItem->m_nNumPoints <= 0)
					continue;
				//pDC->Polyline((POINT*)pItem->m_gpPoints, pItem->m_nNumPoints);
				m_Coord.degrees_lat_long_to_x_y (pItem->m_gpPoints->m_P.lat, pItem->m_gpPoints->m_P.lon, &lx, &ly); 
				m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints.x, &pPoints.y);
				pDC->Polyline(&pPoints, pItem->m_nNumPoints);

			}

			pDC->SelectObject(oldPen);
			pen.DeleteObject();
		}

		break;
	// 기둥형태(내부DOT)
	case LF_FORM4 : 
		break;
	}
}

void shpPolyLine::DrawSelected(CDC* pDC, int iID)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	double lx, ly;
	CPoint pPoints;

	stPolyObject *pObject;
	polyObject *pItem;

	// 객체 수만큼 그리기..
	for(int i=0; i<m_paSHPPoly.GetSize(); ++i)
	{
		pObject = m_paSHPPoly[i];
		for(int j=0; j<pObject->m_paObject.GetSize(); j++)
		{
			pItem = pObject->m_paObject[j];
			if(pItem->m_nID != iID)
				continue;
			//pDC->Polyline((POINT*)pItem->m_gpPoints, pItem->m_nNumPoints);
			m_Coord.degrees_lat_long_to_x_y (pItem->m_gpPoints->m_P.lat, pItem->m_gpPoints->m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoints.x, &pPoints.y);
			pDC->Polyline(&pPoints, pItem->m_nNumPoints);

		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

shpObject::hit_status shpPolyLine::HitTest(GPoint _gPoint, CRect _rctScreen, int& _ID, int& _PointID)
{
	GPOINT_LIST::iterator iter;

	CGPointGP gpointgp;
	GPoint gpoint;
	CRect mbrRect, rectBuffer;
	stPolyObject *pObject;
	polyObject *pItem;

	mbrRect = WorldToDevice(m_MBR);
	if(!rectBuffer.IntersectRect(&mbrRect, &_rctScreen))
		return hitNone;;
	// 점 위를 먼저 본다.
	for(int i=0; i<m_paSHPPoly.GetSize(); i++)
	{
		pObject = m_paSHPPoly[i];
		for(int j=0; j<pObject->m_paObject.GetSize(); j++)
		{
			pItem = pObject->m_paObject[j];

			mbrRect = WorldToDevice(pItem->m_MBR);
			if(!rectBuffer.IntersectRect(&mbrRect, &_rctScreen))
				continue;

			for(int k=0; k<pItem->m_nNumPoints; k++)
			{
				gpoint.x = pItem->m_gpPoints[k].m_P.lon;
				gpoint.y = pItem->m_gpPoints[k].m_P.lat;
				// mouse pointer가 점 근처에 있나 본다.
				if(PtInRect(gpoint, _gPoint))
				{
					_ID = pItem->m_nID;
					_PointID = j;
					return hitPoint;
				}
			}
		}
	}

	// 선위를 본다.
	for(int i=0; i<m_paSHPPoly.GetSize(); i++)
	{
		pObject = m_paSHPPoly[i];
		for(int j=0; j<pObject->m_paObject.GetSize(); j++)
		{
			pItem = pObject->m_paObject[j];

			// 화면 밖이면 처리하지 않는다.
			mbrRect = WorldToDevice(pItem->m_MBR);
			if(!rectBuffer.IntersectRect(&mbrRect, &_rctScreen))
				continue;

			// 파트별 포인트의 개수를 계산..
			int* pParts = (int*)malloc(sizeof(int) * pItem->m_nNumParts);
			for(int k=0; k<pItem->m_nNumParts; ++k)
			{
				if(k == pItem->m_nNumParts - 1)
					pParts[k] = pItem->m_nNumPoints - pItem->m_iParts[j];
				else
					pParts[k] = pItem->m_iParts[j + 1] - pItem->m_iParts[j];
			}	// for k

			// 파트별로 회전하며 선위를 뒤진다..
			for(int k=0; k<pItem->m_nNumParts; ++k)
			{
				*iter = &pItem->m_gpPoints[pItem->m_iParts[j]];

				CGPointGP first = *iter++;
				CGPointGP second;

				for(int l=0; l<(pParts[k] - 1); ++l)
				{
					second = *iter;
					if (IsPointNearLine(first, second, _gPoint,(double)m_nDetectPixelSize/m_lfZoomFactor/m_lfRatio))
					{
						_ID = pItem->m_nID;
						_PointID = -1;
						return hitCurve;
					}
					first = second;
					++iter;
				}	// for l
			}	// for k
			free(pParts);
		}	// for j
	}

    return hitNone;
}

int shpPolyLine::Track(CWnd* pWnd, GPoint point)
{
	m_vSelectedID.clear();

	// 점을 찍었는지 선을 찍었는지 hit test 한다.
	int ID;				// 오브젝트 아이디...
	int PointID;		// 몇번재 포인트인가..
	
	CRect rect;
	CPoint node;

	CDC* dc=pWnd->GetDC();
	pWnd->GetClientRect(rect);

	hit_status hit_test = HitTest(point, rect, ID, PointID);

	if ((hit_test != hitPoint) && (hit_test != hitCurve))
	// 아무데도 아니다.
	{
		if (m_eStatus == stateSelect)
		{
			m_eStatus = stateNone;

			// 다시 그리라고 한다.
			pWnd->InvalidateRect(NULL);
		}
		return -1;
	}

	m_vSelectedID.push_back(ID);
	// 원래 선택이 되어있지 않았다면, 선택된 것을 표시하기 위해 한번 그려준다.
	if (m_eStatus == stateNone)
	{
		m_eStatus = stateSelect;
		pWnd->InvalidateRect(NULL);				

		// 선택되어 있지 않았을 때는 점을 옮기는 것은 지원 안한다.
		hit_test = hitCurve;
		return ID;
	}

	// 상태를 선택한 것으로 변화시킨다.
	m_eStatus = stateSelect;
	pWnd->ReleaseDC(dc);
	// 대충 그리라고 한다.
	pWnd->InvalidateRect(NULL);

	return ID;
}




void shpPolyLine::RGBtoHSL(COLORREF rgb, double *H, double *S, double *L)
{
   double delta;
   double r = (double)GetRValue(rgb) / 255;
   double g = (double)GetGValue(rgb) / 255;
   double b = (double)GetBValue(rgb) / 255;
   double cmax = max(r, max(g, b));
   double cmin = min(r, min(g, b));
   *L = (cmax + cmin) / 2.0;
   if (cmax == cmin) 
   {
      *S = 0;
      *H = 0; // it's really undefined
   } 
   else 
   {
      if (*L < 0.5) 
         *S = (cmax - cmin) / (cmax + cmin);
      else
         *S = (cmax - cmin) / (2.0 - cmax - cmin);
      delta = cmax - cmin;
      if (r == cmax)
          *H = (g - b) / delta;
      else if (g == cmax)
          *H = 2.0 + ( b - r) / delta;
      else
          *H = 4.0 + (r - g) / delta;
      *H /= 6.0;
      if(*H < 0.0)
		  *H += 1;
   }
}

COLORREF shpPolyLine::HSLtoRGB(double H, double S, double L)
{
#ifdef FROM_GRgbHsl	// from CommonMathb
	double	r, g, b;
	double	m1, m2;

	if (S == 0) 
	{
		r = g = b = L;
	}
	else 
	{
		if (L <= 0.5) 
		{
			m2 = L * (1.0 + S);
		}
		else 
		{
			m2 = L + S - L * S;
		}

		m1 = 2.0 * L - m2;
		r = HuetoRGB(m1, m2, H + 1.0 / 3.0);
		g = HuetoRGB(m1, m2, H);
		b = HuetoRGB(m1, m2, H - 1.0 / 3.0);
	}

	return RGB((BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));

#else	// from CommonMathb.cpp

  double r,g,b;
  double m1, m2;

  if (S == 0) 
  {
     r = g = b = L;
  } 
  else 
  {
     if (L <= 0.5)
        m2 = L * (1.0 + S);
     else
        m2 = L + S - L * S;
     m1 = 2.0 * L - m2;
     r = HuetoRGB(m1, m2, H + 1.0 / 3.0);
     g = HuetoRGB(m1, m2, H);
     b = HuetoRGB(m1, m2, H - 1.0 / 3.0);
  }
  return RGB((BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));

#endif // FROM_GRgbHsl
}

double shpPolyLine::HuetoRGB(double m1, double m2, double h)
{
   if (h < 0) h += 1.0;
   if (h > 1) h -= 1.0;
   if (6.0 * h < 1 )
      return ( m1 + (m2 - m1) * h * 6.0);
   if (2.0 * h < 1)
      return m2;
   if (3.0 * h < 2.0)
      return (m1 + (m2 - m1) * ((2.0 / 3.0) - h) * 6.0);
   return m1;
}

void shpPolyLine::AddSHPPolyArray(polyObject *pItem)
{
	stPolyObject *pObject = NULL;
	for(int i=0; i<m_paSHPPoly.GetSize(); i++)
	{
		if(pItem->m_strCode == m_paSHPPoly[i]->m_strCode)
		{
			pObject = m_paSHPPoly[i];
			break;
		}
	}

	if(pObject == NULL)
	{
		pObject = new stPolyObject;
		pObject->m_strCode = pItem->m_strCode;
		pObject->m_paObject.Add(pItem);
		m_paSHPPoly.Add(pObject);
	}
	else
		pObject->m_paObject.Add(pItem);
}

bool shpPolyLine::OpenMemoryMap(CString _filename)
{
	if(!shpObject::Open(_filename))
		return false;

	HANDLE hFile;
	hFile = CreateFile(_filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile)
		return FALSE;
	m_hFMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	hFile = CreateFile(m_strSHXPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile)
		return FALSE;
	m_hFMapShx = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	if(m_hFMap == NULL || m_hFMapShx == NULL)
		return false;

	return true;
}

void shpPolyLine::DrawMemoryMap(CDC* pDC, int nEllipsoid, double _zoomFactor, GPoint _ptMapCenter, const CGRectGP& _rctScreen, double _lfRatio)
{
	if(m_hFMap == NULL || m_hFMapShx == NULL)
		return;

	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	m_lfZoomFactor = _zoomFactor;
	m_geoCenterPoint = _ptMapCenter;
	m_scrCenterPoint.x = int(_rctScreen.Width() / 2);
	m_scrCenterPoint.y = int(_rctScreen.Height() / 2);
	m_lfRatio = _lfRatio;

	CRect mbrRect, rectBuffer;
	CGGeoTrans trans(nEllipsoid);
	CGPointGP scrLT, scrBR;
	scrLT.SetPoint(&(_rctScreen.TopLeft()));
	scrBR.SetPoint(&(_rctScreen.BottomRight()));

	int iTmp, iSeek, iSeek2, iNumParts, iNumPoints;
	double xmin, xmax, ymin, ymax;
	CGPointGP checkPointLT, checkPointRB;
	CGRectGP rtGP;
	GPoint tmpPoint;
	MBR mbr;
	CPoint *pPoints;
	CPoint pPoint;
	CGPointGP gpPoint;
	double lx, ly;


	unsigned char *pData = (unsigned char *)MapViewOfFile(m_hFMap, FILE_MAP_READ, 0, 0, 0);
	unsigned char *pDataShx = (unsigned char *)MapViewOfFile(m_hFMapShx, FILE_MAP_READ, 0, 0, 0);

	if(pData == NULL || pDataShx == NULL)
		return;

	// 객체 수만큼 그리기..
	for(int i=0; i<m_nRecords; ++i)
	{
		iSeek = 100 + (i * 8);
		memcpy(&iTmp, pDataShx+iSeek, 4);
		SwapWord(4, &iTmp);
		iTmp = iTmp * 2;
		iSeek2 = iTmp + 12;

		memcpy(&mbr, pData+iSeek2, 32);
		xmin = mbr.xmin + 0.01;
		xmax = mbr.xmax - 0.01;
		ymin = mbr.ymin + 0.01;
		ymax = mbr.ymax - 0.01;
		checkPointLT.SetPoint(xmax, ymin);
		checkPointRB.SetPoint(xmin, ymax);
		rtGP.SetRect(&checkPointLT, &checkPointRB);

		if(rtGP.PtInRect(scrLT) && rtGP.PtInRect(scrBR))
		{
			;
		}
		else if(!_rctScreen.PtInRectHeight(checkPointLT) && !_rctScreen.PtInRectWidth(checkPointLT) 
		&& !_rctScreen.PtInRectHeight(checkPointRB) && !_rctScreen.PtInRectWidth(checkPointRB))
			continue;
		else
			;

		memcpy(&iNumParts, pData+iSeek2+32, 4);
		memcpy(&iNumPoints, pData+iSeek2+32+4, 4);

		if(iNumParts <= 0 || iNumPoints <= 0)
			continue;

		iSeek = iSeek2+32+4+4;

		pPoints = new CPoint[iNumPoints];
		// point reading..
		for(int j=0; j<iNumPoints; ++j)
		{
			memcpy(&gpPoint.m_P.lon, pData+iSeek, 8);
			iSeek += 8;
			memcpy(&gpPoint.m_P.lat, pData+iSeek, 8);
			iSeek += 8;
			//pDC->GeoPtoLP(&pPoint, &gpPoint);

			m_Coord.degrees_lat_long_to_x_y (gpPoint.m_P.lat, gpPoint.m_P.lon, &lx, &ly); 
			m_CoordMgr->WorldXY2ScreenXY(lx, ly, &pPoint.x, &pPoint.y);

			pPoints[j].x = pPoint.x;
			pPoints[j].y = pPoint.y;
		}
		pDC->Polyline(pPoints, iNumPoints);
		delete []pPoints;
		pPoints = NULL;

/*
		gpPoints = new CGPointGP[iNumPoints];
		// point reading..
		for(int j=0; j<iNumPoints; ++j)
		{
			memcpy(&tmpPoint.x, pData+iSeek, 8);
			iSeek += 8;
			memcpy(&tmpPoint.y, pData+iSeek, 8);
			iSeek += 8;
			gpPoints[j].SetPoint(tmpPoint.x, tmpPoint.y);
		}
		_pDC->Polyline(gpPoints, iNumPoints);
		delete []gpPoints;
		gpPoints = NULL;		
*/
	}

	if(pData)
		UnmapViewOfFile(pData);
	if(pDataShx)
		UnmapViewOfFile(pDataShx);
}

void shpPolyLine::DrawLabel(CDC* pDC)
{
/* jwpark imsi block
	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));

	int nFontSize;
	nFontSize = 14;
	CSize cSize(nFontSize, nFontSize);
	_pDC->DPtoLP(&cSize);

	lf.lfHeight = (int)(cSize.cx);
	//lf.lfWidth = 0;
	lf.lfEscapement =  0;
	lf.lfOrientation =  0;
	lf.lfWeight = FW_REGULAR;
	lf.lfCharSet = ANSI_CHARSET;  //한글 지원
	CFont font;
	font.CreateFontIndirect(&lf);

	CFont *def_font	= _pDC->SelectObject(&font);
	int iOldRop2 = _pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = _pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = _pDC->SetTextColor(RGB(0,0,0));
	_pDC->SetTextAlign(TA_LEFT);

	CGPointGP gpoint;
	int nSize = m_vSHPPoly.size();
	for(int i=0; i<nSize; ++i)
	{
		CString tmpString = m_DBF.ReadStringAttribute(i, m_nLabelField);
		gpoint.SetPoint(m_vSHPPoly[i].m_gpPoints[0].m_P.lon, m_vSHPPoly[i].m_gpPoints[0].m_P.lat);
		_pDC->TextOut(&gpoint, tmpString);
	}

	_pDC->SetTextColor(oldColor2);
	_pDC->SelectObject(def_font);
	font.DeleteObject();
*/
}