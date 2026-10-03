// GEnvVar.h: interface for the CGEnvVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENVVAR_H__A5C4FF61_13A3_11D1_9C67_0060970F1108__INCLUDED_)
#define AFX_GENVVAR_H__A5C4FF61_13A3_11D1_9C67_0060970F1108__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



//////////////////////////////////////////////////////////////////////
// class CGEnvVar
/**
* 환경 변수 설정 관리자 클래스
* @author Peter
* @version 1.0
* @since 2005.11
**/
class CGEnvVar  
{
public:
	CGEnvVar();
	/** use environment variable name */
	CGEnvVar(CString envVar);	
					// as argument
	virtual		~CGEnvVar();
	
	CString		GetValue();					// 지정된 환경변수의 값을 얻는다. 
	
	void		SetEnvVar(CString envVar );	// NULL String으로 생성 된 객체에 대해 환경변수를 재설정 한다. 
							
protected:
	/** store environment variable name */
	CString	m_EnvVar;
	/** store environment variable value */
	CString	m_EnvValue;
	/** 환경변수에서 '%variable%' 부분을 처리한다. */
	BOOL	ExtendValue(CString &value);
	/** 환경변수 값을 찾는다. %variable% 부분은 처리하지 않는다. */	
	CString GetEnvironmentValue(CString envVar);
};

#endif // !defined(AFX_GENVVAR_H__A5C4FF61_13A3_11D1_9C67_0060970F1108__INCLUDED_)
