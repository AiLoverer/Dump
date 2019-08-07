//! \file RegProfile
//! \bfief Registry profile - allows to load/save settings to Windows registry.
//! \author Oleg Krivtsov
//! \date Feb 2012

#include "stdafx.h"
#include "RegProfile.h"

void CRegProfile::Initialize(LPCTSTR lpszProfileName)
{
	m_sPath = lpszProfileName;
	if(m_sPath.Right(1)!="\\") m_sPath+="\\";
}

bool CRegProfile::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue, BOOL bGlobal)
{
	CRegKey key;
	key.Create(bGlobal?HKEY_LOCAL_MACHINE:HKEY_CURRENT_USER,m_sPath+lpszSection);
	LONG r = key.SetDWORDValue(lpszEntry,nValue);
	key.Close();
	return (r==ERROR_SUCCESS)? true:false;
	
}

bool CRegProfile::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszData, BOOL bGlobal)
{
	CRegKey key;
	key.Create(bGlobal?HKEY_LOCAL_MACHINE:HKEY_CURRENT_USER,m_sPath+lpszSection);
	LONG r = key.SetStringValue(lpszEntry,lpszData);
	key.Close();
	return (r==ERROR_SUCCESS)? true:false;
}

bool CRegProfile::WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE lpszData, UINT uBytes, BOOL bGlobal)
{
	CRegKey key;
	key.Create(bGlobal?HKEY_LOCAL_MACHINE:HKEY_CURRENT_USER,m_sPath+lpszSection);
	LONG r = key.SetBinaryValue(lpszEntry,lpszData,uBytes);
	key.Close();
	return (r==ERROR_SUCCESS)? true:false;
}

int CRegProfile::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault, BOOL bGlobal)
{
	LONG r;
	DWORD val;
	CRegKey key;
	r = key.Open(bGlobal?HKEY_LOCAL_MACHINE:HKEY_CURRENT_USER,m_sPath+lpszSection);
	if(r!=ERROR_SUCCESS) return nDefault;
	r = key.QueryDWORDValue(lpszEntry,val);
	key.Close();
	if(r==ERROR_SUCCESS) return (int)val;
	return nDefault;
}

LPTSTR CRegProfile::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault, LPTSTR lpBuffer, UINT nBufferSize, BOOL bGlobal)
{
	LONG r;
	ULONG nChars = nBufferSize;
	CRegKey key;
	r = key.Open(bGlobal?HKEY_LOCAL_MACHINE:HKEY_CURRENT_USER,m_sPath+lpszSection);
	if(r!=ERROR_SUCCESS) return (LPTSTR)lpszDefault;
	r = key.QueryStringValue(lpszEntry,lpBuffer,&nChars);
	key.Close();
	if(r==ERROR_SUCCESS) return (LPTSTR)lpBuffer;
	return (LPTSTR)lpszDefault;
}

bool CRegProfile::GetProfileBinary (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT uBytes, BOOL bGlobal)
{
	LONG r;
	ULONG sz = uBytes;
	CRegKey key;
	r = key.Open(bGlobal?HKEY_LOCAL_MACHINE:HKEY_CURRENT_USER,m_sPath+lpszSection);
	if(r!=ERROR_SUCCESS) return FALSE;
	r = key.QueryBinaryValue(lpszEntry,pData,&sz);
	key.Close();
	if(r!=ERROR_SUCCESS) return FALSE;
	return TRUE;
}

