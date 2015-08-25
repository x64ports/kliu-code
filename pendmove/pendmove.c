#pragma comment(linker, "/version:1.4") // MUST be in the form of major.minor

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

__forceinline
VOID EnableProtectedPendmoves( )
{
	// Windows automatically deletes this registry value after the system
	// processes pending file operations after reboot.

	#define REGSTR_SM  TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager")
	#define REGSTR_APR TEXT("AllowProtectedRenames")

	HKEY hKey = NULL;

	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGSTR_SM, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS &&
		 hKey != NULL )
	{
		DWORD dwData = 1;
		RegSetValueEx(hKey, REGSTR_APR, 0, REG_DWORD, (PBYTE)&dwData, sizeof(DWORD));
		RegCloseKey(hKey);
	}
}

__forceinline
DWORD ProcessStatus( )
{
	DWORD dwErrorCode = GetLastError();
	PTSTR pszErrorMessage = NULL;

	DWORD cchMessage = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwErrorCode,
		0,
		(PTSTR)&pszErrorMessage,
		0,
		NULL
	);

	if (cchMessage)
		_tprintf(TEXT("%s"), pszErrorMessage);
	else
		_tprintf(TEXT("Error %08X\n"), dwErrorCode);

	LocalFree(pszErrorMessage);
	return(dwErrorCode);
}

UINT __fastcall EntryMain( UINT argc, PTSTR *argv )
{
	if ((argc & -2) == 2)  // equivalent to (argc == 2 || argc == 3)
	{
		EnableProtectedPendmoves();
		SetLastError(ERROR_SUCCESS);

		MoveFileEx(
			argv[1],
			(argc == 2) ? NULL : argv[2],
			MOVEFILE_REPLACE_EXISTING | MOVEFILE_DELAY_UNTIL_REBOOT
		);

		return(ProcessStatus());
	}
	else
	{
		_tprintf(TEXT("Usage: pendmove source [destination]\n"));
		return(~1);
	}
}
