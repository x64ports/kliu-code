/**
 * Custom string loader to supports language overrides
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#include "globals.h"
#include "RegHelpers.h"
#include "libs\SimpleString.h"

#define DEFAULT_LANGID MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)

__forceinline LANGID GetLangID( )
{
	if (!g.CustLangID.fCached)
	{
		g.CustLangID.dwLangID = RegUSGetDW(REG_SUBKEYNAME, TEXT("LangID"), FALSE, DEFAULT_LANGID);

		// If there is no user override, check for and handle special cases
		if (g.CustLangID.dwLangID == DEFAULT_LANGID)
		{
			LANGID lid = GetUserDefaultUILanguage();

			if (PRIMARYLANGID(lid) == LANG_CROATIAN)
				g.CustLangID.dwLangID = lid;
			else if (PRIMARYLANGID(lid) == LANG_ENGLISH && g.uWinVer >= 0x0601)
				g.CustLangID.dwLangID = MAKELANGID(LANG_ENGLISH, SUBLANG_NT61);
		}

		g.CustLangID.fCached = TRUE;
	}

	return(LOWORD(g.CustLangID.dwLangID));
}

int WINAPI LoadStringExW( HINSTANCE hInstance, UINT uID, PWSTR pszBuffer, int cchBufferMax )
{
	PCTSTR pszName;
	HRSRC hRsrc;
	HGLOBAL hGlobal;
	PCWSTR pwsz;
	int cchCopy = 0;

	/* This check can be omitted because we "control" all calls to this function
	if (cchBufferMax == 0)
		return(0);
	*/

	// Convert the string ID into a bundle number for use as the resource ID
	pszName = MAKEINTRESOURCE((uID >> 4) + 1);

	if (!(hRsrc = FindResourceEx(hInstance, RT_STRING, pszName, GetLangID())))
	{
		// If the initial load failed, we need to explicitly try again with the
		// default user language ID (which in the resource world is "neutral")

		hRsrc = FindResourceEx(hInstance, RT_STRING, pszName, DEFAULT_LANGID);
	}

	if ( (hRsrc != NULL) &&
	     (hGlobal = LoadResource(hInstance, hRsrc)) &&
	     (pwsz = LockResource(hGlobal)) )
	{
		uID &= 0x0F;

		while (uID)
		{
			pwsz += (UINT)*pwsz + 1;
			--uID;
		}

		// Determine how much we should copy...
		cchCopy = (UINT)*pwsz;

		if (cchCopy >= cchBufferMax)
			cchCopy = cchBufferMax - 1;

		// ...and copy
		SSChainNCpyW(pszBuffer, pwsz + 1, cchCopy);
	}

	// Make sure that the string is terminated, even if we failed to find it
	pszBuffer[cchCopy] = 0;
	return(cchCopy);
}
