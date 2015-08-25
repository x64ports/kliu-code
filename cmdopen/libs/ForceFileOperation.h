/**
 * Copy to or delete a target file, even if it is in use.
 * Copyright (C) Kai Liu.  All rights reserved.
 **/

#ifndef __FORCEFILEOPERATION_H__
#define __FORCEFILEOPERATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

BOOL WINAPI ForceFileOperationA( PCSTR  pszTarget, PCSTR  pszSource );
BOOL WINAPI ForceFileOperationW( PCWSTR pszTarget, PCWSTR pszSource );

__forceinline BOOL ForceCopyA( PCSTR  pszSource, PCSTR  pszTarget ) { return(ForceFileOperationA(pszTarget, pszSource)); }
__forceinline BOOL ForceCopyW( PCWSTR pszSource, PCWSTR pszTarget ) { return(ForceFileOperationW(pszTarget, pszSource)); }

__forceinline BOOL ForceDeleteA( PCSTR  pszFile ) { return(ForceFileOperationA(pszFile, NULL)); }
__forceinline BOOL ForceDeleteW( PCWSTR pszFile ) { return(ForceFileOperationW(pszFile, NULL)); }

#ifdef UNICODE
#define ForceCopy ForceCopyW
#define ForceDelete ForceDeleteW
#else
#define ForceCopy ForceCopyA
#define ForceDelete ForceDeleteA
#endif

#ifdef __cplusplus
}
#endif

#endif
