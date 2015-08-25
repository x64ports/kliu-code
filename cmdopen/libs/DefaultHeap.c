/**
 * Wrappers for accessing the default process heap
 * Last modified: 2011/03/02
 **/

#include <windows.h>

PVOID __fastcall DefaultHeapAlloc( SIZE_T cb )
{
	return(HeapAlloc(GetProcessHeap(), 0, cb));
}

VOID __fastcall DefaultHeapFree( PVOID pv )
{
	HeapFree(GetProcessHeap(), 0, pv);
}

PVOID __fastcall DefaultHeapRealloc( PVOID pv, SIZE_T cb )
{
	if (pv && cb)
		return(HeapReAlloc(GetProcessHeap(), 0, pv, cb));
	else if (cb)
		return(DefaultHeapAlloc(cb));
	else if (pv)
		DefaultHeapFree(pv);

	return(NULL);
}
