//! \file RegProfile
//! \bfief Registry profile - allows to load/save settings to Windows registry.
//! \author Oleg Krivtsov
//! \date Feb 2012

#pragma once
#include "stdafx.h"

//! \class CRegProfile
//! \brief Registry profile - allows to load/save settings to Windows registry.
class CRegProfile
{
public:

	//! Initializes profile.
	//! @param[in] lpszProfileName Registry key name.
	void Initialize(LPCTSTR lpszProfileName);	

	//! Writes an integer number to profile.
	//! @param[in] lpszSection Profile section name.
	//! @param[in] lpszEntry Profile entry name.
	//! @param[in] nValue Entry value.
	//! @param[in] bGlobal defines what registry branch to use. If TRUE, HKLM is used, otherwise HKCU is used.
	bool WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue, BOOL bGlobal=FALSE);
	
	//! Writes a string to profile.
	//! @param[in] lpszSection Profile section name.
	//! @param[in] lpszEntry Profile entry name.
	//! @param[in] lpszData Entry value.
	//! @param[in] bGlobal defines what registry branch to use. If TRUE, HKLM is used, otherwise HKCU is used.
	bool WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszData, BOOL bGlobal=FALSE);
	
	//! Writes a binary buffer to profile.
	//! @param[in] lpszSection Profile section name.
	//! @param[in] lpszEntry Profile entry name.
	//! @param[in] lpData Entry value.
	//! @param[in] bGlobal defines what registry branch to use. If TRUE, HKLM is used, otherwise HKCU is used.
	bool WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE lpData, UINT uBytes, BOOL bGlobal=FALSE);
	
	//! Reads an integer number from profile.
	//! \return An integer number (if such entry found) or default value (if entry not found).
	//! @param[in] lpszSection Profile section name.
	//! @param[in] lpszEntry Profile entry name.
	//! @param[in] nDefault Default value used if entry not found in profile.
	//! @param[in] bGlobal defines what registry branch to use. If TRUE, HKLM is used, otherwise HKCU is used.
	int GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault, BOOL bGlobal=FALSE);
	
	//! Reads a string from profile.
	LPTSTR GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault, LPTSTR lpBuffer, UINT nBufferSize, BOOL bGlobal=FALSE);
	
	//! Reads a binary buffer from profile.
	bool GetProfileBinary (LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT uBytes, BOOL bGlobal=FALSE);
	
private:

	CString m_sPath; //!< Registry key name.
};

