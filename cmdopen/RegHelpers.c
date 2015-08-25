/**
 * Registry Helper Functions
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#include "RegHelpers.h"
#include "libs\Wow64.h"
#include "libs\SimpleString.h"

#define countof(x) (sizeof(x)/sizeof(x[0]))

// Open a key, creating it if necessary
HKEY WINAPI RegOpen( HKEY hKey, PCTSTR pszSubKey, PCTSTR pszSubst )
{
	HKEY hKeyRet = NULL;

	TCHAR szJoinedKey[0x7F];
	if (pszSubst) wnsprintf(szJoinedKey, countof(szJoinedKey), pszSubKey, pszSubst);

	if ( RegCreateKeyEx(hKey, (pszSubst) ? szJoinedKey : pszSubKey, 0, NULL,
	                    REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKeyRet,
	                    NULL) == ERROR_SUCCESS )
	{
		Wow64DisableRegReflection(hKeyRet);
		return(hKeyRet);
	}

	return(NULL);
}

BOOL WINAPI RegDelete( HKEY hKey, PCTSTR pszSubKey, PCTSTR pszSubst )
{
	LONG lResult;

	TCHAR szJoinedKey[0x7F];
	if (pszSubst) wnsprintf(szJoinedKey, countof(szJoinedKey), pszSubKey, pszSubst);

	lResult = SHDeleteKey(hKey, (pszSubst) ? szJoinedKey : pszSubKey);
	return(lResult == ERROR_SUCCESS || lResult == ERROR_FILE_NOT_FOUND);
}

BOOL WINAPI RegSetSZ( HKEY hKey, PCTSTR pszValueName, PCTSTR pszData )
{
	return(RegSetValueEx(hKey, pszValueName, 0, REG_SZ, (PBYTE)pszData, ((DWORD)SSLen(pszData) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS);
}

BOOL WINAPI RegSetDW( HKEY hKey, PCTSTR pszValueName, DWORD dwData )
{
	return(RegSetValueEx(hKey, pszValueName, 0, REG_DWORD, (PBYTE)&dwData, sizeof(DWORD)) == ERROR_SUCCESS);
}

BOOL WINAPI RegGetSZ( HKEY hKey, PCTSTR pszValueName, PTSTR pszBuffer, DWORD cbBuffer )
{
	DWORD dwType;

	if ( RegQueryValueEx(hKey, pszValueName, NULL, &dwType, (PBYTE)pszBuffer, &cbBuffer) == ERROR_SUCCESS &&
	     dwType == REG_SZ && cbBuffer >= sizeof(TCHAR) )
	{
		// The registry does not check for proper null termination, so in order
		// to prevent crashes in the case of a malformed registry entry, make
		// sure that, at the very least, the final character is a null
		*((PTSTR)((PBYTE)pszBuffer + cbBuffer - sizeof(TCHAR))) = 0;
		return(TRUE);
	}
	else
	{
		pszBuffer[0] = 0;
		return(FALSE);
	}
}

BOOL WINAPI RegGetDW( HKEY hKey, PCTSTR pszValueName, PDWORD pdwData )
{
	DWORD cbData = sizeof(DWORD);
	DWORD dwType;

	if ( RegQueryValueEx(hKey, pszValueName, NULL, &dwType, (PBYTE)pdwData, &cbData) == ERROR_SUCCESS &&
	     dwType == REG_DWORD && cbData == sizeof(DWORD) )
	{
		return(TRUE);
	}
	else
	{
		*pdwData = 0;
		return(FALSE);
	}
}

DWORD WINAPI RegUSQueryDW( HUSKEY hUSKey, PCTSTR pszValue, BOOL fIgnoreHKCU, DWORD dwDefault )
{
	DWORD dwValue;
	DWORD cbData = sizeof(DWORD);

	if ( SHRegQueryUSValue(hUSKey, pszValue, NULL, &dwValue, &cbData, fIgnoreHKCU, NULL, 0) ==
	     ERROR_SUCCESS && cbData == sizeof(DWORD) )
	{
		return(dwValue);
	}
	else
	{
		return(dwDefault);
	}
}

DWORD WINAPI RegUSGetDW( PCTSTR pszSubKey, PCTSTR pszValue, BOOL fIgnoreHKCU, DWORD dwDefault )
{
	DWORD dwValue;
	DWORD cbData = sizeof(DWORD);

	if ( SHRegGetUSValue(pszSubKey, pszValue, NULL, &dwValue, &cbData, fIgnoreHKCU, NULL, 0) ==
	     ERROR_SUCCESS && cbData == sizeof(DWORD) )
	{
		return(dwValue);
	}
	else
	{
		return(dwDefault);
	}
}
