/**
 * Custom string loader to supports language overrides
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#ifndef __LOADSTRINGEX_H__
#define __LOADSTRINGEX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

typedef struct {
	BOOL fCached;
	DWORD dwLangID;
} CUSTLANGID, *PCUSTLANGID;

// LoadStringExW's signature and behavior is compatible with that of LoadStringW
int WINAPI LoadStringExW( HINSTANCE hInstance, UINT uID, PWSTR pszBuffer, int cchBufferMax );

// I am not going to bother with implementing this for non-Unicode, since the
// official builds are all Unicode, and any existing use of the ANSI version
// is there solely for vestigial backwards-compatibility
#define LoadStringExA LoadStringA

#ifdef UNICODE
#define LoadStringEx LoadStringExW
#else
#define LoadStringEx LoadStringExA
#endif

// Strings stored in user32.dll
#define IDS_USER32_OK             0x0320
#define IDS_USER32_CANCEL         0x0321
#define IDS_USER32_ABORT          0x0322
#define IDS_USER32_YES            0x0325
#define IDS_USER32_NO             0x0326
#define IDS_USER32_CLOSE          0x0327
#define IDS_USER32_HELP           0x0328
#define LoadStringUser(a, b, c)   LoadStringEx(GetModuleHandleA("USER32.dll"), a, b, c)

// Strings stored in shell32.dll
#define IDS_SHELL32_ADMINISTRATOR 0x5503
#define IDS_SHELL32_CMD_PROMPT    0x5606
#define LoadStringShell(a, b, c)  LoadStringEx(GetModuleHandleA("SHELL32.dll"), a, b, c)

#ifdef __cplusplus
}
#endif

#endif
