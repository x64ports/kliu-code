/**
 * ContextConsole Shell Extension
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#include "CCmdOpenClassFactory.hpp"
#include "CCmdOpen.hpp"

STDMETHODIMP CCmdOpenClassFactory::QueryInterface( REFIID riid, LPVOID *ppv )
{
	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppv = this;
	}
	else if (IsEqualIID(riid, IID_IClassFactory))
	{
		*ppv = (LPCLASSFACTORY)this;
	}
	else
	{
		*ppv = NULL;
		return(E_NOINTERFACE);
	}

	AddRef();
	return(S_OK);
}

STDMETHODIMP CCmdOpenClassFactory::CreateInstance( LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppv )
{
	*ppv = NULL;

	if (pUnkOuter) return(CLASS_E_NOAGGREGATION);

	LPCCMDOPEN lpCmdOpen = new CCmdOpen;
	if (lpCmdOpen == NULL) return(E_OUTOFMEMORY);

	HRESULT hr = lpCmdOpen->QueryInterface(riid, ppv);
	lpCmdOpen->Release();
	return(hr);
}
