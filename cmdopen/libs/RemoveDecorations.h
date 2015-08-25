/**
 * Remove decorations (access keys and ellipses) from a UI string
 **/

#ifndef __REMOVEDECORATIONS_H__
#define __REMOVEDECORATIONS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

SIZE_T __fastcall RemoveDecorationsA( PSTR  pszA );
SIZE_T __fastcall RemoveDecorationsW( PWSTR pszW );

#ifdef UNICODE
#define RemoveDecorations RemoveDecorationsW
#else
#define RemoveDecorations RemoveDecorationsA
#endif

#ifdef __cplusplus
}
#endif

#endif
