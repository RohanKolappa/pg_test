# Microsoft Developer Studio Generated NMAKE File, Based on linkcreator.dsp
!IF "$(CFG)" == ""
CFG=linkcreator - Win32 Debug
!MESSAGE No configuration specified. Defaulting to linkcreator - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "linkcreator - Win32 Release" && "$(CFG)" != "linkcreator - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "linkcreator.mak" CFG="linkcreator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "linkcreator - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "linkcreator - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "linkcreator - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\linkcreator.exe"


CLEAN :
	-@erase "$(INTDIR)\linkcreator.obj"
	-@erase "$(INTDIR)\linkcreator.pch"
	-@erase "$(INTDIR)\linkcreator.res"
	-@erase "$(INTDIR)\Shortcut.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\linkcreator.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\linkcreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\linkcreator.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\linkcreator.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\linkcreator.pdb" /machine:I386 /out:"$(OUTDIR)\linkcreator.exe" 
LINK32_OBJS= \
	"$(INTDIR)\linkcreator.obj" \
	"$(INTDIR)\Shortcut.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\linkcreator.res"

"$(OUTDIR)\linkcreator.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "linkcreator - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\linkcreator.exe"


CLEAN :
	-@erase "$(INTDIR)\linkcreator.obj"
	-@erase "$(INTDIR)\linkcreator.pch"
	-@erase "$(INTDIR)\linkcreator.res"
	-@erase "$(INTDIR)\Shortcut.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\linkcreator.exe"
	-@erase "$(OUTDIR)\linkcreator.ilk"
	-@erase "$(OUTDIR)\linkcreator.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\linkcreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\linkcreator.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\linkcreator.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\linkcreator.pdb" /debug /machine:I386 /out:"$(OUTDIR)\linkcreator.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\linkcreator.obj" \
	"$(INTDIR)\Shortcut.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\linkcreator.res"

"$(OUTDIR)\linkcreator.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("linkcreator.dep")
!INCLUDE "linkcreator.dep"
!ELSE 
!MESSAGE Warning: cannot find "linkcreator.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "linkcreator - Win32 Release" || "$(CFG)" == "linkcreator - Win32 Debug"
SOURCE=.\linkcreator.cpp

"$(INTDIR)\linkcreator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\linkcreator.pch"


SOURCE=.\linkcreator.rc

"$(INTDIR)\linkcreator.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Shortcut.cpp

"$(INTDIR)\Shortcut.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\linkcreator.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "linkcreator - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\linkcreator.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\linkcreator.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "linkcreator - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\linkcreator.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\linkcreator.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

