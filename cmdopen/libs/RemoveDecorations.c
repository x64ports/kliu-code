/**
 * Remove decorations (access keys and ellipses) from a UI string
 **/

#include <windows.h>

SIZE_T __fastcall RemoveDecorationsA( PSTR pszA )
{
	PSTR pszSrcA = pszA;
	PSTR pszDestA = pszA;

	while (*pszSrcA && *pszSrcA != '(' && *pszSrcA != '.')
	{
		if (*pszSrcA != '&')
		{
			*pszDestA = *pszSrcA;
			++pszDestA;
		}

		++pszSrcA;
	}

	*pszDestA = 0;

	return(pszDestA - pszA);
}

SIZE_T __fastcall RemoveDecorationsW( PWSTR pszW )
{
	PWSTR pszSrcW = pszW;
	PWSTR pszDestW = pszW;

	while (*pszSrcW && *pszSrcW != L'(' && *pszSrcW != L'.')
	{
		if (*pszSrcW != L'&')
		{
			*pszDestW = *pszSrcW;
			++pszDestW;
		}

		++pszSrcW;
	}

	*pszDestW = 0;

	return(pszDestW - pszW);
}
