/**
 * ContextConsole Shell Extension
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#include "globals.h"
#include "CCmdOpen.hpp"
#include "CCmdOpenClassFactory.hpp"
#include "GetShieldIcon.hpp"
#include "RegHelpers.h"
#include "libs\Wow64.h"
#include "libs\ForceFileOperation.h"

// Global bookkeeping context (declared as extern in globals.h)
GLOBALCTX g;

// Prototypes for the self-registration/install/uninstall helper functions
STDAPI DllRegisterServerEx( LPCTSTR );
__forceinline HRESULT Install( BOOL );
__forceinline HRESULT Uninstall( );
__forceinline BOOL InstallFile( LPCTSTR, LPTSTR, LPTSTR );


#ifdef _DLL
#pragma comment(linker, "/entry:DllMain")
#endif
extern "C" BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			ZeroMemory(&g, sizeof(g));
			g.hModule = hInstance;
			g.uWinVer = SwapV16(LOWORD(GetVersion()));
			#ifndef _WIN64
			if (g.uWinVer < 0x0500) return(FALSE);
			#endif
			#ifdef _DLL
			DisableThreadLibraryCalls(hInstance);
			#endif
			break;

		case DLL_PROCESS_DETACH:
			if (g.hbmpShieldIcon)
				DeleteObject(g.hbmpShieldIcon);
			break;
	}

	return(TRUE);
}

STDAPI DllCanUnloadNow( )
{
	return((g.cRef == 0) ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID *ppv )
{
	*ppv = NULL;

	if (IsEqualIID(rclsid, CLSID_CmdOpen))
	{
		LPCCMDOPENCLASSFACTORY lpCmdOpenClassFactory = new CCmdOpenClassFactory;
		if (lpCmdOpenClassFactory == NULL) return(E_OUTOFMEMORY);

		HRESULT hr = lpCmdOpenClassFactory->QueryInterface(riid, ppv);
		lpCmdOpenClassFactory->Release();
		return(hr);
	}

	return(CLASS_E_CLASSNOTAVAILABLE);
}

STDAPI DllRegisterServerEx( LPCTSTR lpszModuleName )
{
	HKEY hKey;

	// Register class
	if (hKey = RegOpen(HKEY_CLASSES_ROOT, TEXT("CLSID\\%s"), CLSID_STR_CmdOpen))
	{
		RegSetSZ(hKey, NULL, CLSNAME_STR_CmdOpen);
		RegCloseKey(hKey);
	} else return(SELFREG_E_CLASS);

	if (hKey = RegOpen(HKEY_CLASSES_ROOT, TEXT("CLSID\\%s\\InprocServer32"), CLSID_STR_CmdOpen))
	{
		RegSetSZ(hKey, NULL, lpszModuleName);
		RegSetSZ(hKey, TEXT("ThreadingModel"), TEXT("Apartment"));
		RegCloseKey(hKey);
	} else return(SELFREG_E_CLASS);

	// Register handlers
	if (hKey = RegOpen(HKEY_CLASSES_ROOT, TEXT("Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen))
	{
		RegSetSZ(hKey, NULL, CLSID_STR_CmdOpen);
		RegSetDW(hKey, TEXT("TypeID"), CLSTYPE_ID_CmdOpenBG);
		RegCloseKey(hKey);
	} else return(SELFREG_E_CLASS);

	if (hKey = RegOpen(HKEY_CLASSES_ROOT, TEXT("Directory\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen))
	{
		RegSetSZ(hKey, NULL, CLSID_STR_CmdOpen);
		RegSetDW(hKey, TEXT("TypeID"), CLSTYPE_ID_CmdOpenFolder);
		RegCloseKey(hKey);
	} else return(SELFREG_E_CLASS);

	if (hKey = RegOpen(HKEY_CLASSES_ROOT, TEXT("Drive\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen))
	{
		RegSetSZ(hKey, NULL, CLSID_STR_CmdOpen);
		RegSetDW(hKey, TEXT("TypeID"), CLSTYPE_ID_CmdOpenFolder);
		RegCloseKey(hKey);
	} else return(SELFREG_E_CLASS);

	if (g.uWinVer < 0x0600)
	{
		// [Bug 18] Do not register the "My Documents" key for NT 6.x or newer
		if (hKey = RegOpen(HKEY_CLASSES_ROOT, TEXT("CLSID\\{450D8FBA-AD25-11D0-98A8-0800361B1103}\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen))
		{
			RegSetSZ(hKey, NULL, CLSID_STR_CmdOpen);
			RegSetDW(hKey, TEXT("TypeID"), CLSTYPE_ID_CmdOpenMyDocs);
			RegCloseKey(hKey);
		}
	}
	else if (g.uWinVer >= 0x0601)
	{
		// Library folder backgrounds on NT 6.1+ are registered separately
		if (hKey = RegOpen(HKEY_CLASSES_ROOT, TEXT("LibraryFolder\\Background\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen))
		{
			RegSetSZ(hKey, NULL, CLSID_STR_CmdOpen);
			RegSetDW(hKey, TEXT("TypeID"), CLSTYPE_ID_CmdOpenBG);
			RegCloseKey(hKey);
		}
	}

	// Register approval
	if (hKey = RegOpen(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), NULL))
	{
		RegSetSZ(hKey, CLSID_STR_CmdOpen, CLSNAME_STR_CmdOpen);
		RegCloseKey(hKey);
	}

	return(S_OK);
}

STDAPI DllRegisterServer( )
{
	TCHAR szCurrentDllPath[MAX_PATH << 1];
	GetModuleFileName(g.hModule, szCurrentDllPath, countof(szCurrentDllPath));
	return(DllRegisterServerEx(szCurrentDllPath));
}

STDAPI DllUnregisterServer( )
{
	BOOL fClassRemoved = TRUE;
	BOOL fApprovalRemoved;
	DWORD dwResult;

	// Unregister class
	if (!RegDelete(HKEY_CLASSES_ROOT, TEXT("CLSID\\%s"), CLSID_STR_CmdOpen))
		fClassRemoved = FALSE;

	// Unregister handlers
	if (!Wow64CheckProcess())
	{
		/**
		 * Registry reflection sucks; it means that if we try to unregister the
		 * Wow64 extension, we'll also unregister the Win64 extension; the API
		 * to disable reflection seems to only affect changes in value, not key
		 * removals. :( This hack will disable the deletion of certain HKCR
		 * keys in the case of 32-on-64, and it should be pretty safe--unless
		 * the user had installed only the 32-bit extension without the 64-bit
		 * extension on Win64 (which should be a very rare scenario), there
		 * should be no undesirable effects to using this hack.
		 **/

		if (!RegDelete(HKEY_CLASSES_ROOT, TEXT("Directory\\Background\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen))
			fClassRemoved = FALSE;

		if (!RegDelete(HKEY_CLASSES_ROOT, TEXT("Directory\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen))
			fClassRemoved = FALSE;

		if (!RegDelete(HKEY_CLASSES_ROOT, TEXT("Drive\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen))
			fClassRemoved = FALSE;
	}

	// Any failures with the following keys should be ignored
	RegDelete(HKEY_CLASSES_ROOT, TEXT("CLSID\\{450D8FBA-AD25-11D0-98A8-0800361B1103}\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen);
	RegDelete(HKEY_CLASSES_ROOT, TEXT("LibraryFolder\\Background\\ShellEx\\ContextMenuHandlers\\%s"), CLSNAME_STR_CmdOpen);

	// Unregister approval
	dwResult = SHDeleteValue(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), CLSID_STR_CmdOpen);
	fApprovalRemoved = (dwResult == ERROR_SUCCESS || dwResult == ERROR_FILE_NOT_FOUND);

	if (!fClassRemoved) return(SELFREG_E_CLASS);
	if (!fApprovalRemoved) return(S_FALSE);

	return(S_OK);
}

STDAPI DllInstall( BOOL fInstall, LPCWSTR pszCmdLine )
{
	// To install with fCopyFile=true
	// regsvr32.exe /i /n CmdOpen.dll
	//
	// To install with fCopyFile=false
	// regsvr32.exe /i:"NoCopy" /n CmdOpen.dll
	//
	// To uninstall
	// regsvr32.exe /u /i /n CmdOpen.dll
	//
	// DllInstall can also be invoked from a RegisterDlls INF section or from
	// a UnregisterDlls INF section, if the registration flags are set to 2.
	// Consult the documentation for RegisterDlls/UnregisterDlls for details.

	return( (fInstall) ?
		Install(pszCmdLine == NULL || StrCmpIW(pszCmdLine, L"NoCopy")) :
		Uninstall()
	);
}

__forceinline
HRESULT Install( BOOL fCopyFile )
{
	TCHAR szCurrentDllPath[MAX_PATH << 1];
	GetModuleFileName(g.hModule, szCurrentDllPath, countof(szCurrentDllPath));

	TCHAR szSysDir[MAX_PATH + 0x20];
	UINT uSize = GetSystemDirectory(szSysDir, MAX_PATH);

	if (uSize && uSize < MAX_PATH)
	{
		LPTSTR lpszPath = szSysDir;
		LPTSTR lpszPathAppend = lpszPath + uSize;

		if (*(lpszPathAppend - 1) != TEXT('\\'))
			*lpszPathAppend++ = TEXT('\\');

		LPTSTR lpszTargetPath = (fCopyFile) ? lpszPath : szCurrentDllPath;

		if ( (!fCopyFile || InstallFile(szCurrentDllPath, lpszTargetPath, lpszPathAppend)) &&
		     DllRegisterServerEx(lpszTargetPath) == S_OK )
		{
			HKEY hKey;

			// DisableUNCCheck (KB156276)
			if (hKey = RegOpen(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Command Processor"), NULL))
			{
				RegSetDW(hKey, TEXT("DisableUNCCheck"), 1);
				RegCloseKey(hKey);
			}

			// Uninstaller entries
			RegDelete(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), CLSNAME_STR_CmdOpen);

			if (hKey = RegOpen(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), CLSNAME_STR_CmdOpen))
			{
				TCHAR szUninstall[MAX_PATH << 1];
				wnsprintf(szUninstall, countof(szUninstall), TEXT("regsvr32.exe /u /i /n \"%s\""), lpszTargetPath);

				DWORD dwEstimatedSize = GetCompressedFileSize(lpszTargetPath, NULL);
				dwEstimatedSize = (dwEstimatedSize != INVALID_FILE_SIZE) ? (dwEstimatedSize + 0x200) >> 10 : 0;

				static const TCHAR szURLFull[] = TEXT("http://code.kliu.org/cmdopen/");
				TCHAR szURLBase[countof(szURLFull)];
				SSStaticCpy(szURLBase, szURLFull);
				szURLBase[21] = 0; // strlen("http://code.kliu.org/")

				SSStaticCpy(lpszPathAppend, TEXT("cmd.exe"));

				RegSetSZ(hKey, TEXT("DisplayIcon"), lpszPath);
				RegSetSZ(hKey, TEXT("DisplayName"), TEXT(CMDOPEN_NAME_STR) TEXT(ARCH_NAME_TAIL));
				RegSetSZ(hKey, TEXT("DisplayVersion"), TEXT(CMDOPEN_VERSION_STR));
				RegSetDW(hKey, TEXT("EstimatedSize"), dwEstimatedSize);
				RegSetSZ(hKey, TEXT("HelpLink"), szURLFull);
				RegSetDW(hKey, TEXT("NoModify"), 1);
				RegSetDW(hKey, TEXT("NoRepair"), 1);
				RegSetSZ(hKey, TEXT("Publisher"), TEXT("Kai Liu"));
				RegSetSZ(hKey, TEXT("UninstallString"), szUninstall);
				RegSetSZ(hKey, TEXT("URLInfoAbout"), szURLBase);
				RegCloseKey(hKey);
			}

			return(S_OK);

		} // if copied & registered

	} // if valid sysdir

	return(E_FAIL);
}

__forceinline
HRESULT Uninstall( )
{
	HRESULT hr = S_OK;

	TCHAR szCurrentDllPath[MAX_PATH << 1];
	GetModuleFileName(g.hModule, szCurrentDllPath, countof(szCurrentDllPath));

	// Schedule the DLL to be deleted at shutdown/reboot
	if (!ForceDelete(szCurrentDllPath)) hr = E_FAIL;

	// Unregister
	if (DllUnregisterServer() != S_OK) hr = E_FAIL;

	// We don't need the uninstall strings any more...
	RegDelete(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s"), CLSNAME_STR_CmdOpen);

	return(hr);
}

__forceinline
BOOL InstallFile( LPCTSTR lpszSource, LPTSTR lpszDest, LPTSTR lpszDestAppend )
{
	static const TCHAR szShellExt[] = TEXT("ShellExt");
	static const TCHAR szDestFile[] = TEXT("\\") TEXT(CMDOPEN_FILENAME_STR);

	SSStaticCpy(lpszDestAppend, szShellExt);
	lpszDestAppend += countof(szShellExt) - 1;

	// Create directory if necessary
	if (GetFileAttributes(lpszDest) == INVALID_FILE_ATTRIBUTES)
		CreateDirectory(lpszDest, NULL);

	SSStaticCpy(lpszDestAppend, szDestFile);
	lpszDestAppend += countof(szDestFile) - 1;

	// No need to copy if the source and destination are the same
	if (StrCmpI(lpszSource, lpszDest) == 0)
		return(TRUE);

	return(ForceCopy(lpszSource, lpszDest));
}
