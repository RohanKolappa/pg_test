# Microsoft Developer Studio Generated NMAKE File, Based on filecreator.dsp
!IF "$(CFG)" == ""
CFG=filecreator - Win32 Debug
!MESSAGE No configuration specified. Defaulting to filecreator - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "filecreator - Win32 Release" && "$(CFG)" != "filecreator - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "filecreator.mak" CFG="filecreator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "filecreator - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "filecreator - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "filecreator - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\filecreator.lib"


CLEAN :
	-@erase "$(INTDIR)\filecreator.pch"
	-@erase "$(INTDIR)\IFileCreator.obj"
	-@erase "$(INTDIR)\MP2TSFileCreator.obj"
	-@erase "$(INTDIR)\MP4FileCreator.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\filecreator.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /I "..\IPVSAxAuthor" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\filecreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\filecreator.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\filecreator.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IFileCreator.obj" \
	"$(INTDIR)\MP2TSFileCreator.obj" \
	"$(INTDIR)\MP4FileCreator.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\filecreator.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "filecreator - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\filecreator.lib"


CLEAN :
	-@erase "$(INTDIR)\filecreator.pch"
	-@erase "$(INTDIR)\IFileCreator.obj"
	-@erase "$(INTDIR)\MP2TSFileCreator.obj"
	-@erase "$(INTDIR)\MP4FileCreator.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\filecreator.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\IPVSAxAuthor" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /Fp"$(INTDIR)\filecreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\filecreator.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\filecreator.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IFileCreator.obj" \
	"$(INTDIR)\MP2TSFileCreator.obj" \
	"$(INTDIR)\MP4FileCreator.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\filecreator.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("filecreator.dep")
!INCLUDE "filecreator.dep"
!ELSE 
!MESSAGE Warning: cannot find "filecreator.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "filecreator - Win32 Release" || "$(CFG)" == "filecreator - Win32 Debug"
SOURCE=.\IFileCreator.cpp

!IF  "$(CFG)" == "filecreator - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /I "..\IPVSAxAuthor" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\filecreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\IFileCreator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\filecreator.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "filecreator - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\IPVSAxAuthor" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /Fp"$(INTDIR)\filecreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\IFileCreator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\filecreator.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\MP2TSFileCreator.cpp

!IF  "$(CFG)" == "filecreator - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /I "..\IPVSAxAuthor" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\filecreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MP2TSFileCreator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\filecreator.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "filecreator - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\IPVSAxAuthor" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /Fp"$(INTDIR)\filecreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MP2TSFileCreator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\filecreator.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\MP4FileCreator.cpp

!IF  "$(CFG)" == "filecreator - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /I "..\IPVSAxAuthor" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\filecreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MP4FileCreator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\filecreator.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "filecreator - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\IPVSAxAuthor" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /Fp"$(INTDIR)\filecreator.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MP4FileCreator.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\filecreator.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "filecreator - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /I "..\IPVSAxAuthor" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\filecreator.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\filecreator.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "filecreator - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /I "..\IPVSAxAuthor" /I "..\..\mpeg4ip\common\video\iso-mpeg4\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNICODE" /Fp"$(INTDIR)\filecreator.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\filecreator.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

