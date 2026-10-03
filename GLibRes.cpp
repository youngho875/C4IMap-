// GLibRes.cpp : implementation of the CGLib class for resource handling
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "GLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CGLib resource utility functions

CString CGLib::LoadString(HINSTANCE hInstance, UINT nID)
{
	HINSTANCE	hAfxInst;
	CString		str;

	hAfxInst = ::AfxGetResourceHandle();
	::AfxSetResourceHandle(hInstance);
	str.LoadString(nID);
	::AfxSetResourceHandle(hAfxInst);

	return str;
}
