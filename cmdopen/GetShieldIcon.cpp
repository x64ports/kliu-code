/**
 * Load the UAC shield icon as a bitmap
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#include "globals.h"
#include <initguid.h>
#include "nt6/wincodec.h"

// From the Vista winuser.h
#ifndef IDI_SHIELD
#define IDI_SHIELD MAKEINTRESOURCE(32518)
#endif

// From the Vista wincodec.idl
static const IID IID_IWICImagingFactory = { 0xec5ec8a9, 0xc395, 0x4314, { 0x9c, 0x77, 0x54, 0xd7, 0xa9, 0x35, 0xff, 0x70 } };

// LoadIconMetric is Vista-only and needs to be GetProcAddress'ed to maintain NT5 compatibility
typedef HRESULT (WINAPI *PFNLOADICONMETRIC)( HINSTANCE hinst, PCWSTR pszName, int lims, HICON *phico );

// Convenience definitions
typedef void ** PPVOID;
typedef UINT32 ARGB;


__forceinline HBITMAP Create32BitHBITMAP( UINT cx, UINT cy, PBYTE *ppbBits )
{
	// A simple wrapper around CreateDIBSection

	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = -(LONG)cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC hDC = GetDC(NULL);
	HBITMAP hbmp = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (PPVOID)ppbBits, NULL, 0);
	ReleaseDC(NULL, hDC);

	return(hbmp);
}


HBITMAP GetShieldIcon( )
{
	/**
	 * Returned the cached bitmap if it is available
	 */

	if (g.hbmpShieldIcon)
		return(g.hbmpShieldIcon);

	/**
	 * Load the icon as a HICON
	 */

	HICON hicon;

	PFNLOADICONMETRIC pfnLoadIconMetric = (PFNLOADICONMETRIC)
		GetProcAddress(GetModuleHandleA("COMCTL32.dll"), "LoadIconMetric");

	if (!(pfnLoadIconMetric && pfnLoadIconMetric(NULL, (PCWSTR)IDI_SHIELD, 0, &hicon) == S_OK))
		return(g.hbmpShieldIcon);

	/**
	 * The components needed for the bitmap conversion
	 *
	 * Since this is being called from the main thread of a shell extension,
	 * COM has already been initialized.
	 */

	IWICImagingFactory *pFactory;
	IWICBitmap *pBitmap;
	IWICFormatConverter *pConverter;

	/**
	 * Obnoxious and annoying incantations that are needed to convert an icon
	 * into a suitable bitmap...
	 */

	if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (PPVOID)&pFactory)))
	{
		if (SUCCEEDED(pFactory->CreateBitmapFromHICON(hicon, &pBitmap)))
		{
			if (SUCCEEDED(pFactory->CreateFormatConverter(&pConverter)))
			{
				UINT cx, cy;
				PBYTE pbBits;
				HBITMAP hbmp;

				if ( SUCCEEDED(pConverter->Initialize(pBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom)) &&
				     SUCCEEDED(pConverter->GetSize(&cx, &cy)) &&
				     NULL != (hbmp = Create32BitHBITMAP(cx, cy, &pbBits)) )
				{
					UINT cbStride = cx * sizeof(ARGB);
					UINT cbBitmap = cy * cbStride;

					if (SUCCEEDED(pConverter->CopyPixels(NULL, cbStride, cbBitmap, pbBits)))
						g.hbmpShieldIcon = hbmp;
					else
						DeleteObject(hbmp);
				}

				pConverter->Release();
			}

			pBitmap->Release();
		}

		pFactory->Release();
	}

	DestroyIcon(hicon);

	return(g.hbmpShieldIcon);
}
