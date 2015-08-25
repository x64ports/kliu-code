/**
 * ContextConsole Shell Extension
 * Copyright (C) Kai Liu.  All rights reserved.
 *
 * Please refer to readme.txt for information about this source code.
 * Please refer to license.txt for details about distribution and modification.
 **/

// Full name; this will appear in the version info and control panel
#define CMDOPEN_NAME_STR "ContextConsole Shell Extension"

// Full version: MUST be in the form of major,minor,revision,build
#define CMDOPEN_VERSION_FULL 2,1,0,1

// String version: May be any suitable string
#define CMDOPEN_VERSION_STR "2.1.0.1"

// Label version: May be any suitable string
#define CMDOPEN_VERSION_LABEL "Build 2.1.0.1"

#ifdef _DLL
// PE version: MUST be in the form of major.minor
#pragma comment(linker, "/version:2.1")
#endif

// String used in the "CompanyName" field of the version resource
#define CMDOPEN_AUTHOR_STR "code.kliu.org"

// Tail portion of the copyright string for the version resource
#define CMDOPEN_COPYRIGHT_STR "Kai Liu. All rights reserved."

// Name of the DLL
#define CMDOPEN_FILENAME_STR "CmdOpen.dll"

// String used for setup
#define CMDOPEN_SETUP_STR "ContextConsole Shell Extension Setup"

// Architecture names
#if defined(_M_IX86)
#define ARCH_NAME_TAIL " (x86-32)"
#define ARCH_NAME_FULL "x86-32"
#define ARCH_NAME_PART "x86"
#elif defined(_M_AMD64) || defined(_M_X64)
#define ARCH_NAME_TAIL " (x86-64)"
#define ARCH_NAME_FULL "x86-64"
#define ARCH_NAME_PART "x64"
#elif defined(_M_IA64)
#define ARCH_NAME_TAIL " (IA-64)"
#define ARCH_NAME_FULL "IA-64"
#define ARCH_NAME_PART ARCH_NAME_FULL
#else
#define ARCH_NAME_TAIL ""
#define ARCH_NAME_FULL "Unspecified"
#define ARCH_NAME_PART ARCH_NAME_FULL
#endif
