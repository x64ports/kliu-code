/**
 * Copy to or delete a target file, even if it is in use.
 * Copyright (C) Kai Liu.  All rights reserved.
 **/

#include <windows.h>
#include "DefaultHeap.h"
#include "SimpleString.h"

__forceinline BOOL TryFileOperationA( PCSTR pszTarget, PCSTR pszSource )
{
	if (pszSource)
		return(CopyFileA(pszSource, pszTarget, FALSE));
	else
	{
		#ifndef FFO_DELETE_ASSUME_INUSE
		return(DeleteFileA(pszTarget) || (GetLastError() & ~1) == ERROR_FILE_NOT_FOUND);
		#else
		return(FALSE);
		#endif
	}
}

__forceinline BOOL TryFileOperationW( PCWSTR pszTarget, PCWSTR pszSource )
{
	if (pszSource)
		return(CopyFileW(pszSource, pszTarget, FALSE));
	else
	{
		#ifndef FFO_DELETE_ASSUME_INUSE
		return(DeleteFileW(pszTarget) || (GetLastError() & ~1) == ERROR_FILE_NOT_FOUND);
		#else
		return(FALSE);
		#endif
	}
}

__forceinline BOOL TryFileOperationWithMoveA( PCSTR pszTarget, PCSTR pszSource, PSTR pszTemp, PSTR pszTempAppend )
{
	CHAR ch;

	SSCpy4ChA(pszTempAppend, '.', '~', 0, 0);
	pszTempAppend += 2;

	for (ch = '0'; ch <= 'Z'; ++ch)
	{
		*pszTempAppend = ch;

		if (MoveFileExA(pszTarget, pszTemp, MOVEFILE_REPLACE_EXISTING))
		{
			MoveFileExA(pszTemp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

			if (!pszSource || CopyFileA(pszSource, pszTarget, FALSE))
				return(TRUE);
		}

		if (ch == '9') ch = 'A' - 1;
	}

	return(FALSE);
}

__forceinline BOOL TryFileOperationWithMoveW( PCWSTR pszTarget, PCWSTR pszSource, PWSTR pszTemp, PWSTR pszTempAppend )
{
	WCHAR ch;

	SSCpy4ChW(pszTempAppend, L'.', L'~', 0, 0);
	pszTempAppend += 2;

	for (ch = L'0'; ch <= L'Z'; ++ch)
	{
		*pszTempAppend = ch;

		if (MoveFileExW(pszTarget, pszTemp, MOVEFILE_REPLACE_EXISTING))
		{
			MoveFileExW(pszTemp, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

			if (!pszSource || CopyFileW(pszSource, pszTarget, FALSE))
				return(TRUE);
		}

		if (ch == L'9') ch = L'A' - 1;
	}

	return(FALSE);
}

BOOL WINAPI ForceFileOperationA( PCSTR pszTarget, PCSTR pszSource )
{
	BOOL fSuccess = TryFileOperationA(pszTarget, pszSource);

	if (!fSuccess && GetFileAttributesA(pszTarget) != INVALID_FILE_ATTRIBUTES)
	{
		SIZE_T cchTarget = SSLenA(pszTarget);
		PSTR pszTemp = DefaultHeapAlloc((cchTarget + 4) * sizeof(CHAR));

		if (pszTemp)
		{
			memcpy(pszTemp, pszTarget, cchTarget * sizeof(CHAR));

			fSuccess = TryFileOperationWithMoveA(
				pszTarget,
				pszSource,
				pszTemp,
				pszTemp + cchTarget
			);

			DefaultHeapFree(pszTemp);
		}
	}

	return(fSuccess);
}

BOOL WINAPI ForceFileOperationW( PCWSTR pszTarget, PCWSTR pszSource )
{
	BOOL fSuccess = TryFileOperationW(pszTarget, pszSource);

	if (!fSuccess && GetFileAttributesW(pszTarget) != INVALID_FILE_ATTRIBUTES)
	{
		SIZE_T cchTarget = SSLenW(pszTarget);
		PWSTR pszTemp = DefaultHeapAlloc((cchTarget + 4) * sizeof(WCHAR));

		if (pszTemp)
		{
			memcpy(pszTemp, pszTarget, cchTarget * sizeof(WCHAR));

			fSuccess = TryFileOperationWithMoveW(
				pszTarget,
				pszSource,
				pszTemp,
				pszTemp + cchTarget
			);

			DefaultHeapFree(pszTemp);
		}
	}

	return(fSuccess);
}
