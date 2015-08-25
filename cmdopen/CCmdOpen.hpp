/**
 * ContextConsole Shell Extension
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#ifndef __CCMDOPEN_HPP__
#define __CCMDOPEN_HPP__

#include "globals.h"

class CCmdOpen : public IShellExtInit, IContextMenu
{
	protected:
		CREF m_cRef;
		HSIMPLELIST m_hList;

	public:
		CCmdOpen( ) { InterlockedIncrement(&g.cRef); m_cRef = 1; m_hList = NULL; }
		~CCmdOpen( ) { InterlockedDecrement(&g.cRef); SLDestroy(m_hList); }

		// IUnknown members
		STDMETHODIMP QueryInterface( REFIID, LPVOID * );
		STDMETHODIMP_(ULONG) AddRef( ) { return(InterlockedIncrement(&m_cRef)); }
		STDMETHODIMP_(ULONG) Release( )
		{
			// We need a non-volatile variable, hence the cRef variable
			LONG cRef = InterlockedDecrement(&m_cRef);
			if (cRef == 0) delete this;
			return(cRef);
		}

		// IShellExtInit members
		STDMETHODIMP Initialize( LPCITEMIDLIST, LPDATAOBJECT, HKEY );

		// IContextMenu members
		STDMETHODIMP QueryContextMenu( HMENU, UINT, UINT, UINT, UINT );
		STDMETHODIMP InvokeCommand( LPCMINVOKECOMMANDINFO );
		STDMETHODIMP GetCommandString( UINT_PTR, UINT, UINT *, LPSTR, UINT );
};

typedef CCmdOpen *LPCCMDOPEN;

#endif
