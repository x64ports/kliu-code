# MAKE_KITVER=32:3-3;64:2-2, MAKE_SKIPBIT=64


BASE_NAME=setup


!IF ("$(ARCH)" != "x86-32" && "$(ARCH)" != "x86-64") || ("$(CFG)" != "full" && "$(CFG)" != "lite")
!MESSAGE Available build configurations:
!MESSAGE NMAKE /f "$(BASE_NAME).mak" ARCH="x86-32" CFG="full"
!MESSAGE NMAKE /f "$(BASE_NAME).mak" ARCH="x86-64" CFG="full"
!MESSAGE NMAKE /f "$(BASE_NAME).mak" ARCH="x86-32" CFG="lite"
!MESSAGE NMAKE /f "$(BASE_NAME).mak" ARCH="x86-64" CFG="lite"
!MESSAGE You should also set CPPVER to match your compiler version; e.g., CPPVER=150
!ERROR A valid configuration was not specified.
!ENDIF


################################################################################
# Adjustable configuration
################################################################################


# File extension of our desired target
TARGET_EXTENSION=exe


# Output and intermediate directories
BINDIR=bin.$(ARCH)
OBJDIR=obj.$(ARCH).$(CFG)


# Additional include directories
CPPFLAGS_INC=
# Additional definitions
CPPFLAGS_DEF_EX= \
	$(LITE_DEF)


# Linker output type: Subsystem
LINKFLAGS_SUB=WINDOWS


# Additional library paths
LINK_LIBPATHS= \
	/LIBPATH:libs\$(ARCH)


# Import libraries
LINK_LIBS= \
	kernel32.lib \
	user32.lib \
	shell32.lib \
	comctl32.lib \
	advpack.lib \
	cabinet.lib


# Target definitions
TARGET_1="$(BINDIR)\$(BASE_NAME)$(CFG).$(TARGET_EXTENSION)"
LINK_OBJS_1= \
	"$(OBJDIR)\setup.obj" \
	"$(OBJDIR)\dosetup.obj" \
	"$(OBJDIR)\cabinet.obj" \
	"$(OBJDIR)\Wow64.obj" \
	"$(OBJDIR)\setup.res"


CPPFLAGS_EX=/GL
LINKFLAGS_EX=/LTCG /STUB:dosstub.bin


!IF "$(CFG)" == "lite"
LITE_DEF=/D "SETUP_LITE"
!ENDIF

!IF "$(ARCH)" == "x86-32"

CPPVER_DEFAULT=120
ARCH_DEF=_M_IX86
ARCH_LNK=IX86
NTVER_DEF=0x0500
NTVER_LNK=5.0

!ELSEIF "$(ARCH)" == "x86-64"

CPPVER_DEFAULT=140
ARCH_DEF=_M_AMD64
ARCH_LNK=AMD64
NTVER_DEF=0x0502
NTVER_LNK=5.2

!ENDIF


################################################################################
# Standard configuration
################################################################################


# Standard version-specific compiler options
!IFNDEF CPPVER
CPPVER=$(CPPVER_DEFAULT)
!ENDIF

!IF $(CPPVER) <= 130
CPPFLAGS_STD_VER=/G6
!ELSEIF $(CPPVER) == 131
CPPFLAGS_STD_VER=/G7
!ELSE
CPPFLAGS_STD_VER=/GS-
!ENDIF


# Standard compiler options
CPPFLAGS_STD=/nologo /c /MD /W3 /GF /GR- /EHs-c- /O1
# Standard definitions
CPPFLAGS_DEF_STD=/D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_WIN32_WINNT=$(NTVER_DEF)"


# Linker output type: DLL and .def
!IF "$(TARGET_EXTENSION)" == "dll"
LINKFLAGS_DLL=/DLL /DEF:"$(BASE_NAME).def"
MAKE_DOPOST_CLEANDLL=1
!ELSE
LINKFLAGS_DLL=
MAKE_DOPOST_CLEANDLL=0
!ENDIF


# Standard linker options (/OPT:NOWIN98 is implied by SUBSYSTEM version >= 5.0)
LINKFLAGS_STD=/NOLOGO /RELEASE /OPT:REF /OPT:ICF /MERGE:.rdata=.text /IGNORE:4078
# Target OS and machine options
LINKFLAGS_TARG=/SUBSYSTEM:$(LINKFLAGS_SUB),$(NTVER_LNK) /OSVERSION:$(NTVER_LNK) /MACHINE:$(ARCH_LNK)


# Code compiler
CPP=@cl.exe
CPPFLAGS=$(CPPFLAGS_STD) $(CPPFLAGS_STD_VER) $(CPPFLAGS_EX) $(CPPFLAGS_INC) $(CPPFLAGS_DEF_STD) $(CPPFLAGS_DEF_EX)
CPPFLAGSOUT=$(CPPFLAGS) /Fo"$(OBJDIR)\\"


# Resource compiler
RC=@rc.exe
RFLAGS=/l 0x409 /d "NDEBUG" /d "$(ARCH_DEF)" $(LITE_DEF)


# Linker
LINK=@link.exe
LINKFLAGS=$(LINKFLAGS_STD) $(LINKFLAGS_DLL) $(LINKFLAGS_TARG) $(LINKFLAGS_EX) $(LINK_LIBPATHS) $(LINK_LIBS)

!IF "$(TARGET_EXTENSION)" == "lib"
LINK=$(LINK) /LIB
LINKFLAGS=/NOLOGO /MACHINE:$(ARCH_LNK)
!ENDIF


# By default, do not post-process unless explicitly told to do so
!IFNDEF MAKE_DOPOST
MAKE_DOPOST=0
!ENDIF


################################################################################
# Recipes and rules
################################################################################


DEFAULT : "$(BINDIR)" "$(OBJDIR)" "res\cabinet.cab" $(TARGET_1)
!IF $(MAKE_DOPOST) > 0
	 @cd "$(BINDIR)"
	-@md5sum *.exe *.dll *.lib 1>"$(BASE_NAME).md5"
	 @cd ..
!ENDIF

"res\cabinet.cab" :
	-@mkdir res\cabinet
	-@mkdir res\cabinet\x86-32
	-@mkdir res\cabinet\x86-64
	-@copy /b /n /y ..\bin.x86-32\*.dll res\cabinet\x86-32
	-@copy /b /n /y ..\bin.x86-64\*.dll res\cabinet\x86-64
	-@cabutc res\cabinet
	-@rmdir /s /q res\cabinet

$(TARGET_1) : $(LINK_OBJS_1)
	$(LINK) @<<
	$(LINKFLAGS) $(LINK_OBJS_1) /OUT:$@
<<
!IF $(MAKE_DOPOST) > 0 && $(MAKE_DOPOST_CLEANDLL) > 0
	-@cd $(@D) & del /q $(@B).lib $(@B).exp 2>nul & cd ..
!ENDIF

"$(BINDIR)" :
	-@mkdir $@

"$(OBJDIR)" :
	-@mkdir $@

.rc{$(OBJDIR)}.res:
	$(RC) $(RFLAGS) /Fo$@ $<

.c{$(OBJDIR)}.obj::
	$(CPP) @<<
	$(CPPFLAGSOUT) $<
<<

.cpp{$(OBJDIR)}.obj::
	$(CPP) @<<
	$(CPPFLAGSOUT) $<
<<

{libs}.c{$(OBJDIR)}.obj::
	$(CPP) @<<
	$(CPPFLAGSOUT) $<
<<

{libs}.cpp{$(OBJDIR)}.obj::
	$(CPP) @<<
	$(CPPFLAGSOUT) $<
<<
