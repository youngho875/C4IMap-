#ifndef	_GColorData_h
#define	_GColorData_h


/**
* 지형요소 색상정보 관리 class
* @author 박종원
* @version 1.0
* @since 2005.11.
**/
class CGColorData
{
public:
	CGColorData();
	CGColorData(CGColorData &colorData);
	~CGColorData();

	COLORREF		operator[] (int nInx);
	COLORREF		GetAt(int nInx);
	void			SetAt(int nInx, COLORREF newColor);
	inline int		GetSize() { return m_nColors; };
	inline COLORREF		*GetColorArray() { return m_pColors; };
	const CGColorData	&operator=(const CGColorData &right);

	void	InitInstance();
	void	Serialize(CArchive &ar);
	BOOL	Load(CString fileName);

protected:
	long		m_nColors;
	COLORREF	*m_pColors;
};



#endif	_GColorData_h