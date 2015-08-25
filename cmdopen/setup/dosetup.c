#include <windows.h>
#include "libs\Wow64.h"
#include "libs\SimpleString.h"
#include "version.h"

__forceinline BOOL CallDllInstall( PCSTR pszDllPath )
{
	typedef HRESULT (STDAPICALLTYPE *PFNDLLINSTALL)( BOOL fInstall, PCWSTR pszCmdLine );

	HMODULE hModule;
	PFNDLLINSTALL pfnDllInstall;

	if (hModule = LoadLibraryA(pszDllPath))
	{
		pfnDllInstall = (PFNDLLINSTALL)GetProcAddress(hModule, "DllInstall");

		if (pfnDllInstall && pfnDllInstall(TRUE, NULL) == S_OK)
		{
			FreeLibrary(hModule);
			return(TRUE);
		}

		FreeLibrary(hModule);
	}

	return(FALSE);
}

__forceinline void RunCommand( PSTR pszCmdLine )
{
	BOOL fSuccess;

	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	fSuccess = CreateProcessA(
		NULL,
		pszCmdLine,
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		NULL,
		&si,
		&pi
	);

	if (fSuccess)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

BOOL DoSetup( PSTR pszPath, PSTR pszPathAppend )
{
	static const CHAR szPayload32[] = "x86-32\\" SETUP_PAYLOAD_STR;
	static const CHAR szPayload64[] = "x86-64\\" SETUP_PAYLOAD_STR;
	static const CHAR szRegSvr[] = "regsvr32.exe /s /i /n ";

	// No matter what system we are on, we need to install the 32-bit version;
	// this will be used by any 32-bit apps that embed the shell (e.g., the
	// open file dialog box for a 32-bit app).
	SSStaticCpyA(pszPathAppend, szPayload32);
	if (!CallDllInstall(pszPath)) return(FALSE);

	if (Wow64CheckProcess())
	{
		PVOID pvState;
		CHAR szCmdLine[MAX_PATH << 1];

		SSChainNCpy3A(
			szCmdLine,
			szRegSvr, sizeof(szRegSvr) - 1,
			pszPath, pszPathAppend - pszPath,
			szPayload64, sizeof(szPayload64)
		);

		// We can't install the 64-bit version directly because this is a 32-bit
		// process, so we'll need to spawn a process for this; we should also
		// disable redirection so that we get the 64-bit regsvr32.exe.
		Wow64DisableFsRedirEx(&pvState);
		RunCommand(szCmdLine);
		Wow64RevertFsRedir(pvState);
	}

	return(TRUE);
}
