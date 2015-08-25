#pragma comment(linker, "/version:1.2") // MUST be in the form of major.minor

#include <windows.h>
#include "libs\GetCmdLine.h"
#include "libs\StringIntrinsics.h"

#pragma comment(linker, "/entry:hideexec")
void hideexec( )
{
	PTSTR pszCmdLine = GetCmdLine();

	if (pszCmdLine && *pszCmdLine)
	{
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		BOOL fSuccess;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		fSuccess = CreateProcess(
			NULL,
			pszCmdLine,
			NULL,
			NULL,
			FALSE,
			CREATE_DEFAULT_ERROR_MODE | CREATE_NO_WINDOW,
			NULL,
			NULL,
			&si,
			&pi
		);

		if (fSuccess)
		{
			DWORD dwExitCode = 0;
			WaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			ExitProcess(dwExitCode);
		}
	}

	ExitProcess(0x100);
}
