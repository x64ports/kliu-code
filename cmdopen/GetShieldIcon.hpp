/**
 * Load the UAC shield icon as a bitmap
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

#ifndef __GETSHIELDICON_HPP__
#define __GETSHIELDICON_HPP__

// The shield icon bitmap, once retrieved, is cached until the DLL is unloaded.
// Thus, the HBITMAP returned by GetShieldIcon should NOT be destroyed, and
// there should be bookkeeping for g.hbmpShieldIcon in DllMain.

HBITMAP GetShieldIcon( );

#endif
