// shpDBF.h: interface for the shpDBF class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SHPDBF_H__0F5DB6CD_FE5B_4E70_BBF2_4D67DF83C9D6__INCLUDED_)
#define AFX_SHPDBF_H__0F5DB6CD_FE5B_4E70_BBF2_4D67DF83C9D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include <stdio.h>

#ifdef USE_DBMALLOC
#include <dbmalloc.h>
#endif

#define MAX_COLUMN_NUMBER 64
#define MAX_COLUMN_SIZE	265

#define TRIM_DBF_WHITESPACE

/** shape DB data type */
typedef enum {
	/** string type */
	FTString,
	/** integer type */
	FTInteger,
	/** double type */
	FTDouble,
	/** invalid */
	FTInvalid
} DBFFieldType;

/**
* 벡터지도 shape DB data 관리 class
* @author 박종원
* @version 1.0
* @since 2005.05.
**/
class shpDBF
{
 private:
	/** file handler */
	FILE*	m_fp;
	/** record count */
	int		m_nRecords;
	/** record length */
	int		m_nRecordLength;
	/** header length */
	int		m_nHeaderLength;
	/** filed count */
	int		m_nFields;
	/** field offset */
	int	*	m_panFieldOffset;
	/** field size */
	int	*	m_panFieldSize;
	/** field decimal */
	int	*	m_panFieldDecimals;
	/** field type */
	char*	m_pachFieldType;
	/** header */
	char*	m_pszHeader;
	/** current record */
	int		m_nCurrentRecord;
	/** is current record modify? */
	int		m_bCurrentRecordModified;
	/** current record name */
	char*	m_pszCurrentRecord;
  	/** is no header? */
	int		m_bNoHeader;
	/** is update? */
	int		m_bUpdated;
	/** temp buf */
	void	*m_pTemp;

 public:
	shpDBF();
	~shpDBF();
	/** DB file open */
	int		Open( const char * pszDBFFile, const char * pszAccess );
	/** file create */
	int		Create( const char * pszDBFFile );
	/** header write */
	void	WriteHeader();
	/** record flush */
	void	FlushRecord();
	/** field count get */
	int		GetFieldCount();
	/** record count get */
	int		GetRecordCount();
	/** field add */
	int		AddField(const char * pszFieldName, DBFFieldType eType, int nWidth, int nDecimals );
	/** field infomation get */
  	DBFFieldType GetFieldInfo(int iField, char * pszFieldName, int * pnWidth, int * pnDecimals );
	/** attribute read */
	void*	ReadAttribute(int hEntity, int iField, char chReqType );
	/** attribute read */
	int		ReadIntegerAttribute(int iShape, int iField );
	/** attribute read */
	double	ReadDoubleAttribute(int iShape, int iField );
	/** attribute read */
	const char *ReadStringAttribute(int iShape, int iField );
	/** attribute write */
	int WriteAttribute(int iShape, int iFeild, void* pValue);
	/** attribute write */
	int WriteIntegerAttribute(int iShape, int iField, int nFieldValue );
	/** attribute write */
	int WriteDoubleAttribute(int iShape, int iField, double dFieldValue );
	/** attribute write */
	int WriteStringAttribute(int iShape, int iField, const char * pszFieldValue );
	/** current record set */
	void SetCurrentRecord(int iRecord);
	/** current record update */
	void SetCurrentRecordModify(int flag);
	/** file close */
	void Close( );
};


#endif // !defined(AFX_SHPDBF_H__0F5DB6CD_FE5B_4E70_BBF2_4D67DF83C9D6__INCLUDED_)
