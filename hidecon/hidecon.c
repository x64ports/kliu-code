#pragma comment(linker, "/version:1.2") // MUST be in the form of major.minor

#include <windows.h>

#pragma comment(linker, "/entry:hidecon")
int hidecon( )
{
	HWND hWnd = GetConsoleWindow();
	return((hWnd && ShowWindow(hWnd, SW_HIDE)) ? 0 : 1);
}
