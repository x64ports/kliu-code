ContextConsole Shell Extension
Copyright (C) Kai Liu.  All rights reserved.


I.  Building the source

    1.  Building with Microsoft Visual C++

        Prerequisites:
        * Visual C++ 8 (2005) or newer
        * Version 6 or newer of the Resource Compiler (see "Resources" below)

        Notes:
        * Use the CmdOpen.vcproj project file.
        * The Express editions do not support 64-bit compilation.

    2.  Building from the command line

        Visual C++ is not required to build CmdOpen; if you only have the free
        Windows SDK or an older version of Visual C++, you can still build from
        a command-line build environment using Microsoft's NMAKE to build the
        CmdOpen.mak makefile.

        The "official" binaries are built using the makefile from a command-line
        build environment with the following versions:

        Compiler for x86-32: Version 13.1 (from the Windows Server 2003 SP1 DDK)
        Compiler for x86-64: Version 14.0 (from the Windows Server 2003 R2 PSDK)
        Platform SDK: 5.2.3790.2075 (Windows Server 2003 R2 Platform SDK)
        Resource Compiler: Version 6.1.7600.16385

    3.  Building under other environments

        I have not tested this source with other Windows compilers, so I do not
        know how well it will compile with non-Microsoft compilers.

    4.  Resources

        The resource script (CmdOpen.rc) should be compiled with version 6 or
        newer of Microsoft's Resource Compiler.  Unfortunately, Visual C++ 2005
        and earlier only include version 5 of the Resource Compiler; users of
        Visual C++ 2008 or newer already have the correct version.

        You can get the latest version of the Resource Compiler by installing
        the Vista SDK (or any newer SDK), which can be downloaded for free from
        Microsoft.  If you want a smaller download, you can get version
        6.1.6723.1 of the Resource Compiler by downloading KB949408 [1], and
        then use 7-Zip 4.52 or newer to unpack the installer, then the .msp
        found inside, and then the CAB found inside of that.

        If you want to compile with version 5 of the Resource Compiler, you can
        do so by converting CmdOpen.rc and CmdOpenTranslations.rc from UTF-8 to
        UTF-16 (double-byte little-endian Unicode) and commenting out this line:
        #pragma code_page(65001)

        More information about this issue can be found in the comments of the
        resource script.

        [1] http://www.microsoft.com/downloads/details.aspx?FamilyID=53f9cbb4-b4af-4cf2-bfe5-260cfb90f7c3

    5.  Localizations

        Translation strings are stored as string table resources.  These tables
        can be modified by editing CmdOpenTranslations.rc.


II. License and miscellanea

    Please refer to license.txt for details about distribution and modification.

    If you would like to report a bug, make a suggestion, or contribute a
    translation, you can do so at the following URL:
    http://code.kliu.org/tracker/

    The latest version of this software can be found at:
    http://code.kliu.org/cmdopen/
