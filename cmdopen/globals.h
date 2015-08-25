/**
 * ContextConsole Shell Extension
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "libs\WinIntrinsics.h"

#include <windows.h>
#include <olectl.h>
#include <shlobj.h>
#include <shlwapi.h>

#include "libs\SimpleString.h"
#include "libs\SimpleList.h"
#include "libs\SwapIntrinsics.h"
#include "LoadStringEx.h"
#include "CmdOpenTranslations.h"
#include "version.h"

// Global bookkeeping context
extern struct GLOBALCTX {
	HMODULE hModule;
	CREF cRef;
	UINT16 uWinVer;
	CUSTLANGID CustLangID;
	HBITMAP hbmpShieldIcon;
} g;

// Define the data and strings used for COM registration
static const GUID CLSID_CmdOpen = { 0x693b08da, 0xda1f, 0x4f2b, { 0xa1, 0x45, 0xc0, 0x6b, 0xdf, 0x01, 0x86, 0x8a } };
#define CLSID_STR_CmdOpen         TEXT("{693B08DA-DA1F-4f2b-A145-C06BDF01868A}")
#define CLSNAME_STR_CmdOpen       TEXT("CmdOpen Shell Extension")
#define CLSTYPE_ID_CmdOpenBG      0x0B
#define CLSTYPE_ID_CmdOpenFolder  0x0F
#define CLSTYPE_ID_CmdOpenMyDocs  0x0D

// Registry key under which we store settings
#define REG_SUBKEYNAME            TEXT("Software\\CmdOpen")

// Define helper macros
#define countof(x)                (sizeof(x)/sizeof(x[0]))
#define BYTEDIFF(a, b)            ((PBYTE)(a) - (PBYTE)(b))
#define BYTEADD(a, cb)            ((PVOID)((PBYTE)(a) + (cb)))

// Max translation string lengths; increase this as necessary
#define MAX_MENUTEXT              0x40
#define MAX_CMDTITLE              0x20

#ifdef __cplusplus
}
#endif

#endif
