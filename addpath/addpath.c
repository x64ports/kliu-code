#pragma comment(linker, "/version:1.3") // MUST be in the form of major.minor

#include <windows.h>
#include <shlwapi.h>
#include "libs\GetArgv.h"

#define REGSTR_ENV TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment")
#define CCHMAX_BUFFER 0x1000

typedef struct {
	TCHAR szNew[CCHMAX_BUFFER];
	TCHAR szOld[CCHMAX_BUFFER];
} BUFFER, *PBUFFER;

PTSTR WINAPI PrepPath( PTSTR pszPath )
{
	// Appending a semicolon to the strings not only make their joining a bit
	// easier, but it also ensures that when determining if a path already
	// exists, the search will not be affected by subpath matches
	StrTrim(pszPath, TEXT(" \t;"));
	StrCatBuff(pszPath, TEXT(";"), CCHMAX_BUFFER);
	return(pszPath);
}

__forceinline
UINT WINAPI EntryMain( UINT argc, PTSTR *argv, PBUFFER pBuffer )
{
	UINT uRetCode = 1;
	HKEY hKey = NULL;

	if (argc != 3 || (StrCmpI(argv[1], TEXT("pre")) && StrCmpI(argv[1], TEXT("post"))) || argv[2][0] == 0)
	{
		MessageBox(
			NULL,
			TEXT("Usage:\naddpath pre \"path\"\naddpath post \"path\""),
			TEXT("Usage Error"),
			MB_OK | MB_ICONWARNING
		);

		return(~0);
	}

	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_ENV, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey) == ERROR_SUCCESS &&
	     hKey != NULL )
	{
		DWORD cbBuffer = sizeof(pBuffer->szOld);
		DWORD dwType;

		StrCpyN(pBuffer->szNew, argv[2], CCHMAX_BUFFER);

		if ( RegQueryValueEx(hKey, TEXT("Path"), NULL, &dwType, (PBYTE)pBuffer->szOld, &cbBuffer) == ERROR_SUCCESS &&
		     cbBuffer >= 2 && (dwType == REG_SZ || dwType == REG_EXPAND_SZ) &&
		     StrStrI(PrepPath(pBuffer->szOld), PrepPath(pBuffer->szNew)) == NULL )
		{
			PTSTR pszHead, pszTail;

			if (StrCmpI(argv[1], TEXT("pre")) == 0)
			{
				pszHead = pBuffer->szNew;
				pszTail = pBuffer->szOld;
			}
			else
			{
				pszHead = pBuffer->szOld;
				pszTail = pBuffer->szNew;
			}

			StrCatBuff(pszHead, pszTail, CCHMAX_BUFFER);
			StrTrim(pszHead, TEXT(";"));

			cbBuffer = (lstrlen(pszHead) + 1) * sizeof(TCHAR);

			if (RegSetValueEx(hKey, TEXT("Path"), 0, dwType, (PBYTE)pszHead, cbBuffer) == ERROR_SUCCESS)
			{
				SendMessageTimeout(
					HWND_BROADCAST,
					WM_SETTINGCHANGE,
					0,
					(LPARAM)TEXT("Environment"),
					SMTO_ABORTIFHUNG,
					5000,
					NULL
				);

				uRetCode = 0;
			}
			else
			{
				uRetCode = 2;
			}
		}

		RegCloseKey(hKey);
	}

	return(uRetCode);
}

#pragma comment(linker, "/entry:addpath")
void addpath( )
{
	UINT argc = 0;
	PTSTR *argv = GetArgv(&argc);
	PBUFFER pBuffer = LocalAlloc(LMEM_FIXED, sizeof(BUFFER));

	UINT uExitCode = EntryMain(argc, argv, pBuffer);

	LocalFree(pBuffer);
	LocalFree(argv);
	ExitProcess(uExitCode);
}
