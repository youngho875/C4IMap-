// PointDisplay.h: interface for the CPointDisplay class.
// 지점표시를 위한 클래스
// 2007.4.15 YHJ
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTDISPLAY_H__86F6E501_3ECA_4A4C_9F22_3952736CCFC9__INCLUDED_)
#define AFX_POINTDISPLAY_H__86F6E501_3ECA_4A4C_9F22_3952736CCFC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_DISPLAY_POINT 100

// Display Point 구조체
struct DisplayPoint{
	int id;
	float fX;    
	float fY;
	CString strPointName;
	CString strComment;	
	CString strlat;
	CString strlong;
	DisplayPoint(){id =0; fX = 0.0; fY = 0.0; }
	DisplayPoint(int i,CString comment, CString name,float x, float y)
	{ id = i;strComment = comment; strPointName = name; fX = x; fY = y; }
};


class CPointDisplay  
{
public:
	CPointDisplay();
	virtual ~CPointDisplay();
	BOOL AddPoint(DisplayPoint& point);
	BOOL DeletePoint(CString name);
	void Reset();
	int GetCount();
	int GetID();
	void Draw(CDC* pDC);
	CString FindPointDisplay(CString  name);
	void ModifyPoint(CString key, DisplayPoint& point);
	DisplayPoint GetPoint(int id);     // 리스트 인덱스로 포인트를 구한다.
	DisplayPoint GetPointByID(int id); // 포인트의 ID로 포인트를 구한다.
	BOOL FindPointName(const CString &name);
	// 후킹된 지점에서의 지점의 존재여부를 파악한다.
	int ResearchHookPoint(double fXCoord, double fYCoord, double fCenterX, double fCenterY, double fScale);
	// 후킹된 지점에서의 지점의 존재여부를 파악하고 지점의 상세설명을 반환한다.
	int ResearchHookPoint(double fXCoord, double fYCoord, double fCenterX, double fCenterY, double fScale,CString &strComment);

	int m_nHookPoint;
	static int m_nSeqNo;

	DisplayPoint& operator=(const DisplayPoint &rhs);
private:
	// 지점 포인트 리스트
	CList<DisplayPoint, DisplayPoint&> list;
};

#endif // !defined(AFX_POINTDISPLAY_H__86F6E501_3ECA_4A4C_9F22_3952736CCFC9__INCLUDED_)
