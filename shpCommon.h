#pragma warning(disable : 4786)
#include <vector>
#include "includeGeoTrans.h"

using namespace std;

#include "GPoint.h"

/* -------------------------------------------------------------------- */
/*      Shape types (nSHPType)                                          */
/* -------------------------------------------------------------------- */
#define SHPT_NULL	0
#define SHPT_POINT	1
#define SHPT_ARC	3
#define SHPT_POLYGON	5
#define SHPT_MULTIPOINT	8
#define SHPT_POINTZ	11
#define SHPT_ARCZ	13
#define SHPT_POLYGONZ	15
#define SHPT_MULTIPOINTZ 18
#define SHPT_POINTM	21
#define SHPT_ARCM	23
#define SHPT_POLYGONM	25
#define SHPT_MULTIPOINTM 28
#define SHPT_MULTIPATCH 31

/* -------------------------------------------------------------------- */
/*      Minimum Bounding Rectangle                                      */
/* -------------------------------------------------------------------- */
//typedef struct
//{
//	double	xmin;
//	double	ymin;
//	double	xmax;
//	double	ymax;
//}MBR;

/* -------------------------------------------------------------------- */
/*      PoliLine & Polygon                                              */
/* -------------------------------------------------------------------- */

//typedef struct{
//	int				m_nID;
//	MBR				m_MBR;
//	int				m_nNumParts;
//	int				m_nNumPoints;
//	int				*m_iParts;
//	CGPointGP		*m_gpPoints;
//	CString			m_strCode;
//} polyObject;
