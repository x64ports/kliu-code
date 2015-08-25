/**
 * Wrappers for accessing the default process heap
 * Last modified: 2011/03/02
 **/

#ifndef __DEFAULTHEAP_H__
#define __DEFAULTHEAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

PVOID __fastcall DefaultHeapAlloc( SIZE_T cb );
 VOID __fastcall DefaultHeapFree( PVOID pv );
PVOID __fastcall DefaultHeapRealloc( PVOID pv, SIZE_T cb );

#ifdef __cplusplus
}
#endif

#endif
