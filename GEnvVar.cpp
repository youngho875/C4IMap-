// GEnvVar.cpp: implementation of the CGEnvVar class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GEnvVar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	EnvBufSize	256

// 모든 환경변수는 실제 상황, 서버 저장을 기본값으로 한다.
// 시스템 초기화시 훈련 상황인가를 판단하여 해당 환경변수값을 setenv를 사용하여 변경한다.
//
// 운용 모드에 따라 변경되어야 할 환경변수
//		SGIS_OP_MODE :
//			실제 : 'real'
//			훈련 : 'exercise'
//		SGIS_USER_DIR :
//			실제 : 'I:'
//			훈련 : 'J:'
//		SGIS_DB_LOCAL :
//			실제 : %SGIS_FILE_LOCAL%\\db\\SgisDBR.mdb
//			훈련 : %SGIS_FILE_LOCAL%\\db\\SgisDBE.mdb
// 자료 위치에 따라 변경되어야 할 환경변수
//		MAP_HOME :
//			서버 : %SGIS-SV%\\map
//			로컬 : %SGIS_FILE_BASE%\Map


// 체계지원 설정 환경변수
//	C4I_HOME	: 프로그램 설치 디렉토리  ( C:\c4i\bin )
//	C4I_SITE	: 프로그램 설치 부대 번호

static CString	SdefEnvValue[][2] = {

	// 시스템 기본 설정
	{ CString("AFC4I_SGIS_HOME"),		CString("C:\\AFC4I") },
	{ CString("AFC4I_SGIS_MAP_INFO"),	CString("%AFC4I_SGIS_HOME%\\Map\\MapInfo") },
	{ CString("AFC4I_SGIS_SEA_INFO"),    CString("%AFC4I_SGIS_HOME%\\SeaInfo") },			//해도에 대한 텍스트정보를 읽는다.
	{ CString("AFC4I_SGIS_RELATED_INFO"), CString("%AFC4I_SGIS_HOME%\\RelatedInfo") },

	{ CString("MAP_DTED2"),			CString("%AFC4I_SGIS_HOME%\\MAP\\Dted2\\IRAQ") },		// 3차원 고도 자료 디렉토리 (Dted Level 2)
	{ CString("MAP_IMAGE"),			CString("%AFC4I_SGIS_HOME%\\MAP\\Image") },				// 
	{ CString("MAP_LOCAL"),			CString("%AFC4I_SGIS_HOME%\\MAP\\LocalMap") },			// 

	// 환경변수 선언 종료
	{ CString(), CString() }	// last string is empty
};

//////////////////////////////////////////////////////////////////////
// class CGEnvVar

CGEnvVar::CGEnvVar()
{
	m_EnvVar.Empty();
	m_EnvValue.Empty();
}

CGEnvVar::CGEnvVar(CString envVar) : m_EnvVar(envVar)
{
	m_EnvVar = envVar;

	m_EnvValue = GetEnvironmentValue(m_EnvVar);

	if(!m_EnvValue.IsEmpty()) {
		if(ExtendValue(m_EnvValue)==FALSE) {
			m_EnvValue.Empty();
		}
	}
}

CGEnvVar::~CGEnvVar()
{
}

void CGEnvVar::SetEnvVar( CString envVar )
{
	m_EnvVar = envVar;

	m_EnvValue = GetEnvironmentValue(m_EnvVar);

	if(!m_EnvValue.IsEmpty()) {
		if(ExtendValue(m_EnvValue)==FALSE) {
			m_EnvValue.Empty();
		}
	}
}

CString CGEnvVar::GetValue()
{
	return m_EnvValue;
}

CString CGEnvVar::GetEnvironmentValue(CString envVar)
{
	CString	valueStr;
	LPTSTR	valuePtr;
	DWORD	bufLen, prevBufLen;

	bufLen = EnvBufSize;
	prevBufLen = bufLen;
	while(bufLen>0 && bufLen>=prevBufLen) {	// check variable existent
						// and buffer length
		valuePtr = valueStr.GetBuffer(bufLen);
		prevBufLen = bufLen;
		bufLen = GetEnvironmentVariable(envVar, valuePtr, bufLen);
		valueStr.ReleaseBuffer();
	}

	if(bufLen==0) {	// environment variable not found
		// use default value
		int	i;

		valueStr.Empty();	// reset value
		for(i=0; !SdefEnvValue[i][0].IsEmpty(); i++) {
			if(SdefEnvValue[i][0]==envVar) {
				// if found, set value
				valueStr = SdefEnvValue[i][1];
				break;
			}
		}
	}

	return valueStr;
}

BOOL CGEnvVar::ExtendValue(CString &value)
{
	int	varStart, varEnd;
	CString	extValue, tmpValue;
	CString	extEnvVar, extEnvVal;

	extValue = value;
	while((varStart=extValue.Find('%'))!=-1) {
		tmpValue = extValue.Right(extValue.GetLength() - (varStart+1));
		varEnd = tmpValue.Find('%');
		if(varEnd==-1) {
			TRACE1("환경변수 값이 잘못되어 있습니다. '%s'", value);
			return FALSE;
		}

		extEnvVar = tmpValue.Left(varEnd);
		extEnvVal = GetEnvironmentValue(extEnvVar);

#ifdef	_DEBUG
CString	str1 = extValue.Left(varStart);
CString str2 = tmpValue.Right(tmpValue.GetLength() - (varEnd+1));
#endif	_DEBUG

		extValue = extValue.Left(varStart)
				+ extEnvVal
				+ tmpValue.Right(tmpValue.GetLength() - (varEnd+1));
	}

	value = extValue;
	return TRUE;
}

#undef	EnvBufSize

