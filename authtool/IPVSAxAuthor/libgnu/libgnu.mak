# Microsoft Developer Studio Generated NMAKE File, Based on libgnu.dsp
!IF "$(CFG)" == ""
CFG=libgnu - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libgnu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libgnu - Win32 Release" && "$(CFG)" != "libgnu - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libgnu.mak" CFG="libgnu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libgnu - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libgnu - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libgnu - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\libgnu.lib"


CLEAN :
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt1.obj"
	-@erase "$(INTDIR)\need_for_win32.obj"
	-@erase "$(INTDIR)\strcasestr.obj"
	-@erase "$(INTDIR)\strsep.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libgnu.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\mpeg4ip\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\libgnu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libgnu.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libgnu.lib" 
LIB32_OBJS= \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\getopt1.obj" \
	"$(INTDIR)\need_for_win32.obj" \
	"$(INTDIR)\strcasestr.obj" \
	"$(INTDIR)\strsep.obj"

"$(OUTDIR)\libgnu.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libgnu - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\libgnu.lib"


CLEAN :
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt1.obj"
	-@erase "$(INTDIR)\need_for_win32.obj"
	-@erase "$(INTDIR)\strcasestr.obj"
	-@erase "$(INTDIR)\strsep.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libgnu.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\mpeg4ip\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\libgnu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libgnu.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libgnu.lib" 
LIB32_OBJS= \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\getopt1.obj" \
	"$(INTDIR)\need_for_win32.obj" \
	"$(INTDIR)\strcasestr.obj" \
	"$(INTDIR)\strsep.obj"

"$(OUTDIR)\libgnu.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libgnu.dep")
!INCLUDE "libgnu.dep"
!ELSE 
!MESSAGE Warning: cannot find "libgnu.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libgnu - Win32 Release" || "$(CFG)" == "libgnu - Win32 Debug"
SOURCE=..\..\..\mpeg4ip\lib\gnu\getopt.c

"$(INTDIR)\getopt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\mpeg4ip\lib\gnu\getopt1.c

"$(INTDIR)\getopt1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\mpeg4ip\lib\gnu\need_for_win32.c

"$(INTDIR)\need_for_win32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\mpeg4ip\lib\gnu\strcasestr.c

"$(INTDIR)\strcasestr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\mpeg4ip\lib\gnu\strsep.c

"$(INTDIR)\strsep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

