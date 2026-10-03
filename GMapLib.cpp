// GMapLib.cpp: implementation of the CGMapLib class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GMapLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int				CGMapLib::m_nMapKind=-1;
int				CGMapLib::m_nMapScale=25000;
double			CGMapLib::m_dZoomFactor=1.0;
BOOL			CGMapLib::m_bMapDraw = FALSE;
BOOL			CGMapLib::m_bIndexMap = FALSE;

long			CGMapLib::m_lObjectID = 0;
BOOL			CGMapLib::m_bMapSaved = FALSE;
BOOL			CGMapLib::m_bNewFeatureComplete = FALSE;
BOOL			CGMapLib::m_bMapEditActivate = FALSE;
BOOL			CGMapLib::m_bMapActivate = FALSE;
long			CGMapLib::m_lGeoType=0;						//지도편집시 그려지는 그래픽 정보를 Global변수로 저장한다.
CString			CGMapLib::m_strSelectedMapName="";
BOOL			CGMapLib::m_bEditStart = FALSE;
BOOL			CGMapLib::m_bMapQuery = FALSE;
BOOL			CGMapLib::m_bMark = FALSE;
SRECT			CGMapLib::m_rect;

BOOL			CGMapLib::m_bModified = FALSE;
BOOL			CGMapLib::m_bItemText = FALSE;
int				CGMapLib::m_Row = 0;
BOOL			CGMapLib::m_bGridInvalidate = FALSE;
int				CGMapLib::m_nMapHistory = 0;
int				CGMapLib::m_nMapHistoryIndexPattern=0;

VARIANT			CGMapLib::m_vValue ;

clock_t			CGMapLib::m_startClock;
clock_t			CGMapLib::m_endClock;

#define  ZOOM_IN_MAX	2.5
#define  ZOOM_OUT_MIN	0.7
int	CGMapLib::GetAutoScaleChange(double zoomFactor)
{
	int nScale;

	if(m_nMapKind == 0) { //해도
		switch(m_nMapScale) {
		case 25000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 50000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 25000;
			else
				nScale = m_nMapScale;
			break;
		case 50000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 100000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 25000;
			else
				nScale = m_nMapScale;
			break;
		case 100000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 250000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 50000;
			else
				nScale = m_nMapScale;
			break;
		case 250000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 500000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 100000;
			else
				nScale = m_nMapScale;
			break;
		case 500000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 1000000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 250000;
			else
				nScale = m_nMapScale;
			break;
		case 1000000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 1000000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 500000;
			else
				nScale = m_nMapScale;
			break;
		}
	}

	if(m_nMapKind == 1) { //공도
		switch(m_nMapScale) {
		case 250000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 500000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 250000;
			else
				nScale = m_nMapScale;
			break;
		case 500000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 1000000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 250000;
			else
				nScale = m_nMapScale;
			break;
		case 1000000:
			if(zoomFactor <= ZOOM_OUT_MIN)
				nScale = 1000000;
			else if(zoomFactor >= ZOOM_IN_MAX)
				nScale = 500000;
			else
				nScale = m_nMapScale;
			break;
		}
	}

	return nScale;
}


CString CGMapLib::GetVariant(VARIANT vValue)
{
	CString strValue;
	char buff[100];
	switch(vValue.vt)
	{
		case VT_BSTR:				
			strValue = vValue.bstrVal;
			break;
		case VT_I2:
			sprintf(buff, "%d", vValue.iVal);
			strValue = buff;
			break;
		case VT_I4:
			sprintf(buff, "%d", vValue.lVal);
			strValue = buff;
			break;
		case VT_R4:
			sprintf(buff, "%f", vValue.fltVal);
			strValue = buff;
			break;
		case VT_R8:
			if((float)vValue.dblVal < -1000000000)
				vValue.dblVal = 0.0;

			sprintf(buff, "%f", (float)vValue.dblVal);
			strValue = buff;
			break;
	}
	return strValue;
}


void CGMapLib::StartClock()
{
	m_startClock = clock();
}

void CGMapLib::EndClock(CString strMsg)
{
	m_endClock = clock();

	double Duration;	
	Duration = (double)(m_endClock - m_startClock) / CLOCKS_PER_SEC;
	CString tmp;
	tmp.Format("[소요시간 : %5.8f Seconds ] => [%s]",  Duration, strMsg);
	TRACE("%s\n", tmp);
}