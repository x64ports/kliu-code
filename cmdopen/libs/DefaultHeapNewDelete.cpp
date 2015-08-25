/**
 * Forces the new and delete operators to use the default process heap
 * Last modified: 2011/03/02
 **/

extern "C" {
	void * __fastcall DefaultHeapAlloc( size_t cb );
	void   __fastcall DefaultHeapFree( void *pv );
}

void * __cdecl operator new( size_t cb ) {
	return(DefaultHeapAlloc(cb));
}

void __cdecl operator delete( void *pv ) {
	DefaultHeapFree(pv);
}
