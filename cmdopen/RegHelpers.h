/**
 * Registry Helper Functions
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#ifndef __REGHELPERS_H__
#define __REGHELPERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <shlwapi.h>

HKEY WINAPI RegOpen( HKEY, PCTSTR, PCTSTR );
BOOL WINAPI RegDelete( HKEY, PCTSTR, PCTSTR );
BOOL WINAPI RegSetSZ( HKEY, PCTSTR, PCTSTR );
BOOL WINAPI RegSetDW( HKEY, PCTSTR, DWORD );
BOOL WINAPI RegGetSZ( HKEY, PCTSTR, PTSTR, DWORD );
BOOL WINAPI RegGetDW( HKEY, PCTSTR, PDWORD );
DWORD WINAPI RegUSQueryDW( HUSKEY, PCTSTR, BOOL, DWORD );
DWORD WINAPI RegUSGetDW( PCTSTR, PCTSTR, BOOL, DWORD );

#ifdef __cplusplus
}
#endif

#endif
