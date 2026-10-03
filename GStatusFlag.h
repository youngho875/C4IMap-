#ifndef	_GStatusFlag_h
#define	_GStatusFlag_h


/**
* 지도 열기/저장 관리 class
* @author 박종원
* @version 1.0
* @since 2005.07.
**/
class CGStatusFlag
{
public:
	CGStatusFlag();
	CGStatusFlag(CGStatusFlag &colorData);
	~CGStatusFlag();

	BOOL			operator[] (int nInx);
	BOOL			GetAt(int nInx);
	void			SetAt(int nInx, BOOL state);
	inline int		GetSize() { return m_nStatus; };
	const CGStatusFlag	&operator=(const CGStatusFlag &right);

	void	InitInstance();
	void	Serialize(CArchive &ar);
	BOOL	Load(CString fileName);

protected:
	long	m_nStatus;
	BOOL	*m_pStatus;
};



#endif	_GStatusFlag_h