/**
 * ContextConsole Shell Extension
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#include "CCmdOpen.hpp"
#include "GetShieldIcon.hpp"
#include "RegHelpers.h"
#include "libs/RemoveDecorations.h"

// Determine if the UAC menu item should be shown
static __forceinline BOOL ShowElevated( )
{
	if (g.uWinVer < 0x0600) return(FALSE);

	static const TCHAR szPoliciesKey[] =
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System");

	return(!RegUSGetDW(REG_SUBKEYNAME, TEXT("NoUAC"), FALSE, FALSE) &&
	        RegUSGetDW(szPoliciesKey, TEXT("EnableLUA"), TRUE, TRUE));
}

// Load the text for the UAC menu item
static VOID WINAPI LoadUACText( LPTSTR lpszBuffer, INT cchBuffer, BOOL fClean )
{
	if (LoadStringEx(g.hModule, IDS_ELEVTEXT, lpszBuffer, cchBuffer))
	{
		if (fClean)
			RemoveDecorations(lpszBuffer);
	}
	else
	{
		LoadStringEx(g.hModule, IDS_MENUTEXT, lpszBuffer, cchBuffer);

		INT cchUsed = (INT)RemoveDecorations(lpszBuffer);
		LPTSTR lpszAppend = lpszBuffer + cchUsed;

		cchBuffer -= cchUsed + 4;

		if (cchBuffer > 0)
		{
			SSCpy2Ch(lpszAppend, TEXT(' '), TEXT('('));
			lpszAppend += 2;
			lpszAppend += LoadStringShell(IDS_SHELL32_ADMINISTRATOR, lpszAppend, cchBuffer);
			SSCpy2Ch(lpszAppend, TEXT(')'), 0);
		}
	}
}

STDMETHODIMP CCmdOpen::QueryInterface( REFIID riid, LPVOID *ppv )
{
	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppv = this;
	}
	else if (IsEqualIID(riid, IID_IShellExtInit))
	{
		*ppv = (LPSHELLEXTINIT)this;
	}
	else if (IsEqualIID(riid, IID_IContextMenu))
	{
		*ppv = (LPCONTEXTMENU)this;
	}
	else
	{
		*ppv = NULL;
		return(E_NOINTERFACE);
	}

	AddRef();
	return(S_OK);
}

STDMETHODIMP CCmdOpen::Initialize( LPCITEMIDLIST pidlFolder, LPDATAOBJECT pdtobj, HKEY hkeyProgID )
{
	// We need to check the TypeID values to determine which handler we are
	// initializing for...

	if (!hkeyProgID) return(E_INVALIDARG);

	DWORD dwClassTypeID;
	DWORD cbData = sizeof(DWORD);
	TCHAR szSubKey[64];
	wnsprintf(szSubKey, countof(szSubKey), TEXT("ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen);

	if ( SHGetValue(hkeyProgID, szSubKey, TEXT("TypeID"), NULL, (PVOID)&dwClassTypeID, &cbData) !=
	     ERROR_SUCCESS || cbData != sizeof(DWORD) )
	{
		dwClassTypeID = 0;
	}


	// We'll be needing a buffer, and let's double it just to be safe
	TCHAR szPath[MAX_PATH << 1];

	// Make sure that we are working with a fresh list
	SLDestroy(m_hList);
	m_hList = SLCreate();

	// Background: just translate the folder ID
	if (dwClassTypeID == CLSTYPE_ID_CmdOpenBG)
	{
		if (pidlFolder && SHGetPathFromIDList(pidlFolder, szPath))
			SLAddString(m_hList, szPath);
	}

	// My Documents: just look up the folder path
	else if (dwClassTypeID == CLSTYPE_ID_CmdOpenMyDocs)
	{
		if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, FALSE))
			SLAddString(m_hList, szPath);
	}

	// Normal: convert pdtobj to hDrop and then enumerate
	else if (dwClassTypeID == CLSTYPE_ID_CmdOpenFolder)
	{
		FORMATETC format = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM medium;

		if (!pdtobj || pdtobj->GetData(&format, &medium) != S_OK)
			return(E_INVALIDARG);

		if (HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal))
		{
			UINT uDrops = DragQueryFile(hDrop, -1, NULL, 0);

			for (UINT uDrop = 0; uDrop < uDrops; ++uDrop)
			{
				if ( DragQueryFile(hDrop, uDrop, szPath, countof(szPath)) &&
				     GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY )
				{
					SLAddString(m_hList, szPath);
				}
			}

			GlobalUnlock(medium.hGlobal);
		}

		ReleaseStgMedium(&medium);
	}


	// If there was any failure, the list would be empty...
	return((SLCheck(m_hList)) ? S_OK : E_INVALIDARG);
}

STDMETHODIMP CCmdOpen::QueryContextMenu( HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags )
{
	WORD cAdded = 0;

	if (!(uFlags & (CMF_DEFAULTONLY | CMF_NOVERBS)))
	{
		// Load the localized menu text
		TCHAR szMenuText[MAX_MENUTEXT];
		LoadStringEx(g.hModule, IDS_MENUTEXT, szMenuText, countof(szMenuText));

		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_FTYPE | MIIM_ID | MIIM_STRING;
		mii.wID = idCmdFirst;
		mii.dwTypeData = szMenuText;

		if (InsertMenuItem(hmenu, indexMenu, TRUE, &mii))
		{
			++cAdded;

			if (ShowElevated())
			{
				// Load the localized UAC menu text
				LoadUACText(szMenuText, countof(szMenuText), FALSE);

				mii.wID += 1;
				mii.fMask |= MIIM_BITMAP;
				mii.hbmpItem = GetShieldIcon();

				if (InsertMenuItem(hmenu, indexMenu + 1, TRUE, &mii))
					++cAdded;
			}
		}
	}

	return(MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, cAdded));
}

STDMETHODIMP CCmdOpen::InvokeCommand( LPCMINVOKECOMMANDINFO pici )
{
	// Ignore string verbs (high word must be zero)
	// The low word (command index) must be 0 or 1
	if ((UINT_PTR)pici->lpVerb > 1)
		return(E_INVALIDARG);

	// Get the path to cmd.exe
	TCHAR szCommand[MAX_PATH + 0x10];
	LPTSTR lpszCommandAppend = szCommand;

	// To be safe, quote the path
	szCommand[0] = TEXT('"');
	lpszCommandAppend += 1;

	// Get the system path
	UINT cchComSpec = GetEnvironmentVariable(TEXT("ComSpec"), lpszCommandAppend, MAX_PATH);
	lpszCommandAppend += cchComSpec;

	// Close the quotes; fall back to just "cmd.exe" if there was a failure
	if (cchComSpec && cchComSpec < MAX_PATH)
		SSCpy2Ch(lpszCommandAppend, TEXT('"'), 0);
	else
		SSStaticCpy(szCommand, TEXT("cmd.exe"));

	// Load the localized command prompt title
	TCHAR szCmdTitle[MAX_CMDTITLE];
	LoadStringShell(IDS_SHELL32_CMD_PROMPT, szCmdTitle, countof(szCmdTitle));

	// Initialize startup info
	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	// Do not set a title if we were unable to retrieve one (Win2K)
	if (szCmdTitle[0])
		si.lpTitle = szCmdTitle;

	// Now for the trivial part...
	LPCTSTR lpszPath;

	while (lpszPath = (LPCTSTR)SLGetDataAndStep(m_hList))
	{
		if ((UINT_PTR)pici->lpVerb == 0)
		{
			BOOL fSuccess = CreateProcess(
				NULL,
				szCommand,
				NULL,
				NULL,
				FALSE,
				CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE,
				NULL,
				lpszPath,
				&si,
				&pi
			);

			if (fSuccess)
			{
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
		}
		else
		{
			TCHAR szParams[MAX_PATH << 1];
			wnsprintf(szParams, countof(szParams), TEXT("/s /k pushd \"%s\""), lpszPath);
			ShellExecute(pici->hwnd, TEXT("runas"), szCommand, szParams, lpszPath, SW_SHOWNORMAL);
		}
	}

	SLDestroy(m_hList);
	m_hList = NULL;

	return(S_OK);
}

STDMETHODIMP CCmdOpen::GetCommandString( UINT_PTR idCmd, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax )
{
	/* In Vista, the build-in command has a verb of "cmd", so if we match
	 * Vista's verb, then Vista's built-in facility will override ours when
	 * it is active; this will eliminate duplicate menu entries.
	 *
	 * Since we do not support invoking by verb, there is no need to specify a
	 * verb for the UAC command.
	 */
	static const WCHAR szVerbW[] = L"cmd";

	if (idCmd > 1 || cchMax < countof(szVerbW))
		return(E_FAIL);

	switch (uFlags)
	{
		/* We do not care about or need to handle the following cases:
		case GCS_HELPTEXTA:
		case GCS_VALIDATEA:
		case GCS_VALIDATEW:
		case GCS_VERBA:
		 */

		case GCS_HELPTEXTW:
		{
			if (idCmd == 0)
			{
				LoadStringExW(g.hModule, IDS_MENUTEXT, (LPWSTR)pszName, cchMax);
				RemoveDecorationsW((LPWSTR)pszName);
			}
			else
			{
				#ifdef UNICODE
				LoadUACText((LPTSTR)pszName, cchMax, TRUE);
				#else
				return(E_FAIL);
				#endif
			}

			return(S_OK);
		}

		case GCS_VERBW:
		{
			if (idCmd == 0)
			{
				SSStaticCpyW((LPWSTR)pszName, szVerbW);
				return(S_OK);
			}
		}
	}

	return(E_FAIL);
}
