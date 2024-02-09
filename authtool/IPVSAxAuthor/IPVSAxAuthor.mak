# Microsoft Developer Studio Generated NMAKE File, Based on IPVSAxAuthor.dsp
!IF "$(CFG)" == ""
CFG=IPVSAxAuthor - Win32 Debug
!MESSAGE No configuration specified. Defaulting to IPVSAxAuthor - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "IPVSAxAuthor - Win32 Debug" && "$(CFG)" != "IPVSAxAuthor - Win32 Unicode Debug" && "$(CFG)" != "IPVSAxAuthor - Win32 Release MinSize" && "$(CFG)" != "IPVSAxAuthor - Win32 Release MinDependency" && "$(CFG)" != "IPVSAxAuthor - Win32 Unicode Release MinSize" && "$(CFG)" != "IPVSAxAuthor - Win32 Unicode Release MinDependency" && "$(CFG)" != "IPVSAxAuthor - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IPVSAxAuthor.mak" CFG="IPVSAxAuthor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IPVSAxAuthor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IPVSAxAuthor - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IPVSAxAuthor - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IPVSAxAuthor - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IPVSAxAuthor - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IPVSAxAuthor - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IPVSAxAuthor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" "$(OUTDIR)\IPVSAxAuthor.bsc" ".\Debug\regsvr32.trg"

!ELSE 

ALL : "libobj60 - Win32 Debug" "libmp4v2 - Win32 Debug" "libmediaindex - Win32 Debug" "libgnu - Win32 Debug" "filecreator - Win32 Debug" "$(OUTDIR)\IPVSAxAuthor.dll" "$(OUTDIR)\IPVSAxAuthor.bsc" ".\Debug\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"filecreator - Win32 DebugCLEAN" "libgnu - Win32 DebugCLEAN" "libmediaindex - Win32 DebugCLEAN" "libmp4v2 - Win32 DebugCLEAN" "libobj60 - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\FileSystemLib.obj"
	-@erase "$(INTDIR)\FileSystemLib.sbr"
	-@erase "$(INTDIR)\IPVSAxAuthor.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.pch"
	-@erase "$(INTDIR)\IPVSAxAuthor.res"
	-@erase "$(INTDIR)\IPVSAxAuthor.sbr"
	-@erase "$(INTDIR)\ProjectManager.obj"
	-@erase "$(INTDIR)\ProjectManager.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\IPVSAxAuthor.bsc"
	-@erase "$(OUTDIR)\IPVSAxAuthor.dll"
	-@erase "$(OUTDIR)\IPVSAxAuthor.exp"
	-@erase "$(OUTDIR)\IPVSAxAuthor.ilk"
	-@erase "$(OUTDIR)\IPVSAxAuthor.lib"
	-@erase "$(OUTDIR)\IPVSAxAuthor.map"
	-@erase "$(OUTDIR)\IPVSAxAuthor.pdb"
	-@erase ".\IPVSAxAuthor.h"
	-@erase ".\IPVSAxAuthor.tlb"
	-@erase ".\IPVSAxAuthor_i.c"
	-@erase ".\Debug\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /I "..\filecreator" /I "../../mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /D "_DEBUG" /D "_MBCS" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\IPVSAxAuthor.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IPVSAxAuthor.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\FileSystemLib.sbr" \
	"$(INTDIR)\IPVSAxAuthor.sbr" \
	"$(INTDIR)\ProjectManager.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\IPVSAxAuthor.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\IPVSAxAuthor.pdb" /map:"$(INTDIR)\IPVSAxAuthor.map" /debug /machine:I386 /def:".\IPVSAxAuthor.def" /out:"$(OUTDIR)\IPVSAxAuthor.dll" /implib:"$(OUTDIR)\IPVSAxAuthor.lib" /pdbtype:sept 
DEF_FILE= \
	".\IPVSAxAuthor.def"
LINK32_OBJS= \
	"$(INTDIR)\FileSystemLib.obj" \
	"$(INTDIR)\IPVSAxAuthor.obj" \
	"$(INTDIR)\ProjectManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\IPVSAxAuthor.res" \
	"..\filecreator\Debug\filecreator.lib" \
	".\libgnu\Debug\libgnu.lib" \
	"..\..\mediaindex\Debug\libmediaindex.lib" \
	".\libmp4v2\Debug\libmp4v2.lib" \
	".\libobj60\Debug\libobj60.lib"

"$(OUTDIR)\IPVSAxAuthor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\Debug
TargetPath=.\Debug\IPVSAxAuthor.dll
InputPath=.\Debug\IPVSAxAuthor.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"

OUTDIR=.\DebugU
INTDIR=.\DebugU
# Begin Custom Macros
OutDir=.\DebugU
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\DebugU\regsvr32.trg"

!ELSE 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\DebugU\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\FileSystemLib.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.pch"
	-@erase "$(INTDIR)\IPVSAxAuthor.res"
	-@erase "$(INTDIR)\ProjectManager.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\IPVSAxAuthor.dll"
	-@erase "$(OUTDIR)\IPVSAxAuthor.exp"
	-@erase "$(OUTDIR)\IPVSAxAuthor.ilk"
	-@erase "$(OUTDIR)\IPVSAxAuthor.lib"
	-@erase "$(OUTDIR)\IPVSAxAuthor.pdb"
	-@erase ".\IPVSAxAuthor.h"
	-@erase ".\IPVSAxAuthor.tlb"
	-@erase ".\IPVSAxAuthor_i.c"
	-@erase ".\DebugU\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /ZI /Od /I "..\filecreator" /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\IPVSAxAuthor.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IPVSAxAuthor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\IPVSAxAuthor.pdb" /debug /machine:I386 /def:".\IPVSAxAuthor.def" /out:"$(OUTDIR)\IPVSAxAuthor.dll" /implib:"$(OUTDIR)\IPVSAxAuthor.lib" /pdbtype:sept 
DEF_FILE= \
	".\IPVSAxAuthor.def"
LINK32_OBJS= \
	"$(INTDIR)\FileSystemLib.obj" \
	"$(INTDIR)\IPVSAxAuthor.obj" \
	"$(INTDIR)\ProjectManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\IPVSAxAuthor.res"

"$(OUTDIR)\IPVSAxAuthor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\DebugU
TargetPath=.\DebugU\IPVSAxAuthor.dll
InputPath=.\DebugU\IPVSAxAuthor.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
<< 
	

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"

OUTDIR=.\ReleaseMinSize
INTDIR=.\ReleaseMinSize
# Begin Custom Macros
OutDir=.\ReleaseMinSize
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\ReleaseMinSize\regsvr32.trg"

!ELSE 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\ReleaseMinSize\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\FileSystemLib.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.pch"
	-@erase "$(INTDIR)\IPVSAxAuthor.res"
	-@erase "$(INTDIR)\ProjectManager.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\IPVSAxAuthor.dll"
	-@erase "$(OUTDIR)\IPVSAxAuthor.exp"
	-@erase "$(OUTDIR)\IPVSAxAuthor.lib"
	-@erase ".\IPVSAxAuthor.h"
	-@erase ".\IPVSAxAuthor.tlb"
	-@erase ".\IPVSAxAuthor_i.c"
	-@erase ".\ReleaseMinSize\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /O1 /I "..\filecreator" /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\IPVSAxAuthor.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IPVSAxAuthor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib filecreator.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\IPVSAxAuthor.pdb" /machine:I386 /def:".\IPVSAxAuthor.def" /out:"$(OUTDIR)\IPVSAxAuthor.dll" /implib:"$(OUTDIR)\IPVSAxAuthor.lib" 
DEF_FILE= \
	".\IPVSAxAuthor.def"
LINK32_OBJS= \
	"$(INTDIR)\FileSystemLib.obj" \
	"$(INTDIR)\IPVSAxAuthor.obj" \
	"$(INTDIR)\ProjectManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\IPVSAxAuthor.res"

"$(OUTDIR)\IPVSAxAuthor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\IPVSAxAuthor.dll
InputPath=.\ReleaseMinSize\IPVSAxAuthor.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"

OUTDIR=.\ReleaseMinDependency
INTDIR=.\ReleaseMinDependency
# Begin Custom Macros
OutDir=.\ReleaseMinDependency
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\ReleaseMinDependency\regsvr32.trg"

!ELSE 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\ReleaseMinDependency\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\FileSystemLib.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.pch"
	-@erase "$(INTDIR)\IPVSAxAuthor.res"
	-@erase "$(INTDIR)\ProjectManager.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\IPVSAxAuthor.dll"
	-@erase "$(OUTDIR)\IPVSAxAuthor.exp"
	-@erase "$(OUTDIR)\IPVSAxAuthor.lib"
	-@erase ".\IPVSAxAuthor.h"
	-@erase ".\IPVSAxAuthor.tlb"
	-@erase ".\IPVSAxAuthor_i.c"
	-@erase ".\ReleaseMinDependency\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /O1 /I "..\filecreator" /I "../../mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\IPVSAxAuthor.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IPVSAxAuthor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\IPVSAxAuthor.pdb" /machine:I386 /def:".\IPVSAxAuthor.def" /out:"$(OUTDIR)\IPVSAxAuthor.dll" /implib:"$(OUTDIR)\IPVSAxAuthor.lib" 
DEF_FILE= \
	".\IPVSAxAuthor.def"
LINK32_OBJS= \
	"$(INTDIR)\FileSystemLib.obj" \
	"$(INTDIR)\IPVSAxAuthor.obj" \
	"$(INTDIR)\ProjectManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\IPVSAxAuthor.res"

"$(OUTDIR)\IPVSAxAuthor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\IPVSAxAuthor.dll
InputPath=.\ReleaseMinDependency\IPVSAxAuthor.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"

OUTDIR=.\ReleaseUMinSize
INTDIR=.\ReleaseUMinSize
# Begin Custom Macros
OutDir=.\ReleaseUMinSize
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\ReleaseUMinSize\regsvr32.trg"

!ELSE 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\ReleaseUMinSize\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\FileSystemLib.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.pch"
	-@erase "$(INTDIR)\IPVSAxAuthor.res"
	-@erase "$(INTDIR)\ProjectManager.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\IPVSAxAuthor.dll"
	-@erase "$(OUTDIR)\IPVSAxAuthor.exp"
	-@erase "$(OUTDIR)\IPVSAxAuthor.lib"
	-@erase ".\IPVSAxAuthor.h"
	-@erase ".\IPVSAxAuthor.tlb"
	-@erase ".\IPVSAxAuthor_i.c"
	-@erase ".\ReleaseUMinSize\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /O1 /I "..\filecreator" /D "NDEBUG" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\IPVSAxAuthor.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IPVSAxAuthor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\IPVSAxAuthor.pdb" /machine:I386 /def:".\IPVSAxAuthor.def" /out:"$(OUTDIR)\IPVSAxAuthor.dll" /implib:"$(OUTDIR)\IPVSAxAuthor.lib" 
DEF_FILE= \
	".\IPVSAxAuthor.def"
LINK32_OBJS= \
	"$(INTDIR)\FileSystemLib.obj" \
	"$(INTDIR)\IPVSAxAuthor.obj" \
	"$(INTDIR)\ProjectManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\IPVSAxAuthor.res"

"$(OUTDIR)\IPVSAxAuthor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\IPVSAxAuthor.dll
InputPath=.\ReleaseUMinSize\IPVSAxAuthor.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
<< 
	

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"

OUTDIR=.\ReleaseUMinDependency
INTDIR=.\ReleaseUMinDependency
# Begin Custom Macros
OutDir=.\ReleaseUMinDependency
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\ReleaseUMinDependency\regsvr32.trg"

!ELSE 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor.tlb" ".\IPVSAxAuthor.h" ".\IPVSAxAuthor_i.c" ".\ReleaseUMinDependency\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\FileSystemLib.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.pch"
	-@erase "$(INTDIR)\IPVSAxAuthor.res"
	-@erase "$(INTDIR)\ProjectManager.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\IPVSAxAuthor.dll"
	-@erase "$(OUTDIR)\IPVSAxAuthor.exp"
	-@erase "$(OUTDIR)\IPVSAxAuthor.lib"
	-@erase ".\IPVSAxAuthor.h"
	-@erase ".\IPVSAxAuthor.tlb"
	-@erase ".\IPVSAxAuthor_i.c"
	-@erase ".\ReleaseUMinDependency\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /O1 /I "..\filecreator" /D "NDEBUG" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\IPVSAxAuthor.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IPVSAxAuthor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib filecreator.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\IPVSAxAuthor.pdb" /machine:I386 /def:".\IPVSAxAuthor.def" /out:"$(OUTDIR)\IPVSAxAuthor.dll" /implib:"$(OUTDIR)\IPVSAxAuthor.lib" /libpath:"..\filecreator\Release" 
DEF_FILE= \
	".\IPVSAxAuthor.def"
LINK32_OBJS= \
	"$(INTDIR)\FileSystemLib.obj" \
	"$(INTDIR)\IPVSAxAuthor.obj" \
	"$(INTDIR)\ProjectManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\IPVSAxAuthor.res"

"$(OUTDIR)\IPVSAxAuthor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\IPVSAxAuthor.dll
InputPath=.\ReleaseUMinDependency\IPVSAxAuthor.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
<< 
	

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"

OUTDIR=.\IPVSAxAuthor___Win32_Release
INTDIR=.\IPVSAxAuthor___Win32_Release
# Begin Custom Macros
OutDir=.\IPVSAxAuthor___Win32_Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor___Win32_Release\regsvr32.trg"

!ELSE 

ALL : "libobj60 - Win32 Release" "libmp4v2 - Win32 Release" "libmediaindex - Win32 Release" "libgnu - Win32 Release" "filecreator - Win32 Release" "$(OUTDIR)\IPVSAxAuthor.dll" ".\IPVSAxAuthor___Win32_Release\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"filecreator - Win32 ReleaseCLEAN" "libgnu - Win32 ReleaseCLEAN" "libmediaindex - Win32 ReleaseCLEAN" "libmp4v2 - Win32 ReleaseCLEAN" "libobj60 - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\FileSystemLib.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.obj"
	-@erase "$(INTDIR)\IPVSAxAuthor.pch"
	-@erase "$(INTDIR)\IPVSAxAuthor.res"
	-@erase "$(INTDIR)\ProjectManager.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\IPVSAxAuthor.dll"
	-@erase "$(OUTDIR)\IPVSAxAuthor.exp"
	-@erase "$(OUTDIR)\IPVSAxAuthor.ilk"
	-@erase "$(OUTDIR)\IPVSAxAuthor.lib"
	-@erase ".\IPVSAxAuthor.h"
	-@erase ".\IPVSAxAuthor.tlb"
	-@erase ".\IPVSAxAuthor_i.c"
	-@erase ".\IPVSAxAuthor___Win32_Release\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /O1 /Ob0 /I "..\filecreator" /I "../../mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /D "NDEBUG" /D "_MBCS" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\IPVSAxAuthor.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\IPVSAxAuthor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\IPVSAxAuthor.pdb" /machine:I386 /def:".\IPVSAxAuthor.def" /out:"$(OUTDIR)\IPVSAxAuthor.dll" /implib:"$(OUTDIR)\IPVSAxAuthor.lib" 
DEF_FILE= \
	".\IPVSAxAuthor.def"
LINK32_OBJS= \
	"$(INTDIR)\FileSystemLib.obj" \
	"$(INTDIR)\IPVSAxAuthor.obj" \
	"$(INTDIR)\ProjectManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\IPVSAxAuthor.res" \
	"..\filecreator\Release\filecreator.lib" \
	".\libgnu\Release\libgnu.lib" \
	"..\..\mediaindex\Release\libmediaindex.lib" \
	".\libmp4v2\Release\libmp4v2.lib" \
	".\libobj60\Release\libobj60.lib"

"$(OUTDIR)\IPVSAxAuthor.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\IPVSAxAuthor___Win32_Release
TargetPath=.\IPVSAxAuthor___Win32_Release\IPVSAxAuthor.dll
InputPath=.\IPVSAxAuthor___Win32_Release\IPVSAxAuthor.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
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

MTL_PROJ=

!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("IPVSAxAuthor.dep")
!INCLUDE "IPVSAxAuthor.dep"
!ELSE 
!MESSAGE Warning: cannot find "IPVSAxAuthor.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "IPVSAxAuthor - Win32 Debug" || "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug" || "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize" || "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency" || "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize" || "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency" || "$(CFG)" == "IPVSAxAuthor - Win32 Release"
SOURCE=.\FileSystemLib.cpp

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"


"$(INTDIR)\FileSystemLib.obj"	"$(INTDIR)\FileSystemLib.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"


"$(INTDIR)\FileSystemLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"


"$(INTDIR)\FileSystemLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"


"$(INTDIR)\FileSystemLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"


"$(INTDIR)\FileSystemLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"


"$(INTDIR)\FileSystemLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"


"$(INTDIR)\FileSystemLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ENDIF 

SOURCE=.\IPVSAxAuthor.cpp

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"


"$(INTDIR)\IPVSAxAuthor.obj"	"$(INTDIR)\IPVSAxAuthor.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch" ".\IPVSAxAuthor_i.c" ".\IPVSAxAuthor.h"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"


"$(INTDIR)\IPVSAxAuthor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"


"$(INTDIR)\IPVSAxAuthor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"


"$(INTDIR)\IPVSAxAuthor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"


"$(INTDIR)\IPVSAxAuthor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"


"$(INTDIR)\IPVSAxAuthor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"


"$(INTDIR)\IPVSAxAuthor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch" ".\IPVSAxAuthor_i.c" ".\IPVSAxAuthor.h"


!ENDIF 

SOURCE=.\IPVSAxAuthor.idl
MTL_SWITCHES=/tlb ".\IPVSAxAuthor.tlb" /h "IPVSAxAuthor.h" /iid "IPVSAxAuthor_i.c" /Oicf 

".\IPVSAxAuthor.tlb"	".\IPVSAxAuthor.h"	".\IPVSAxAuthor_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


SOURCE=.\IPVSAxAuthor.rc

"$(INTDIR)\IPVSAxAuthor.res" : $(SOURCE) "$(INTDIR)" ".\IPVSAxAuthor.tlb"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ProjectManager.cpp

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"


"$(INTDIR)\ProjectManager.obj"	"$(INTDIR)\ProjectManager.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch" ".\IPVSAxAuthor.h"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"


"$(INTDIR)\ProjectManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"


"$(INTDIR)\ProjectManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"


"$(INTDIR)\ProjectManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"


"$(INTDIR)\ProjectManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"


"$(INTDIR)\ProjectManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch"


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"


"$(INTDIR)\ProjectManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\IPVSAxAuthor.pch" ".\IPVSAxAuthor.h"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /ZI /Od /I "..\filecreator" /I "../../mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /D "_DEBUG" /D "_MBCS" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\IPVSAxAuthor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /ZI /Od /I "..\filecreator" /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\IPVSAxAuthor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\filecreator" /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\IPVSAxAuthor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\filecreator" /I "../../mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\IPVSAxAuthor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\filecreator" /D "NDEBUG" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\IPVSAxAuthor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"

CPP_SWITCHES=/nologo /MD /W3 /O1 /I "..\filecreator" /D "NDEBUG" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\IPVSAxAuthor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /O1 /Ob0 /I "..\filecreator" /I "../../mediaindex" /I "..\..\mpeg4ip\lib\mp4v2" /I "..\..\mpeg4ip\include" /D "NDEBUG" /D "_MBCS" /D "UNICODE" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Fp"$(INTDIR)\IPVSAxAuthor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\IPVSAxAuthor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"

"filecreator - Win32 Debug" : 
   cd "\depot\authtool\filecreator"
   $(MAKE) /$(MAKEFLAGS) /F .\filecreator.mak CFG="filecreator - Win32 Debug" 
   cd "..\IPVSAxAuthor"

"filecreator - Win32 DebugCLEAN" : 
   cd "\depot\authtool\filecreator"
   $(MAKE) /$(MAKEFLAGS) /F .\filecreator.mak CFG="filecreator - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\IPVSAxAuthor"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"

"filecreator - Win32 Release" : 
   cd "\depot\authtool\filecreator"
   $(MAKE) /$(MAKEFLAGS) /F .\filecreator.mak CFG="filecreator - Win32 Release" 
   cd "..\IPVSAxAuthor"

"filecreator - Win32 ReleaseCLEAN" : 
   cd "\depot\authtool\filecreator"
   $(MAKE) /$(MAKEFLAGS) /F .\filecreator.mak CFG="filecreator - Win32 Release" RECURSE=1 CLEAN 
   cd "..\IPVSAxAuthor"

!ENDIF 

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"

"libgnu - Win32 Debug" : 
   cd ".\libgnu"
   $(MAKE) /$(MAKEFLAGS) /F .\libgnu.mak CFG="libgnu - Win32 Debug" 
   cd ".."

"libgnu - Win32 DebugCLEAN" : 
   cd ".\libgnu"
   $(MAKE) /$(MAKEFLAGS) /F .\libgnu.mak CFG="libgnu - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"

"libgnu - Win32 Release" : 
   cd ".\libgnu"
   $(MAKE) /$(MAKEFLAGS) /F .\libgnu.mak CFG="libgnu - Win32 Release" 
   cd ".."

"libgnu - Win32 ReleaseCLEAN" : 
   cd ".\libgnu"
   $(MAKE) /$(MAKEFLAGS) /F .\libgnu.mak CFG="libgnu - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"

"libmediaindex - Win32 Debug" : 
   cd "\depot\mediaindex"
   $(MAKE) /$(MAKEFLAGS) /F .\libmediaindex.mak CFG="libmediaindex - Win32 Debug" 
   cd "..\authtool\IPVSAxAuthor"

"libmediaindex - Win32 DebugCLEAN" : 
   cd "\depot\mediaindex"
   $(MAKE) /$(MAKEFLAGS) /F .\libmediaindex.mak CFG="libmediaindex - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\authtool\IPVSAxAuthor"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"

"libmediaindex - Win32 Release" : 
   cd "\depot\mediaindex"
   $(MAKE) /$(MAKEFLAGS) /F .\libmediaindex.mak CFG="libmediaindex - Win32 Release" 
   cd "..\authtool\IPVSAxAuthor"

"libmediaindex - Win32 ReleaseCLEAN" : 
   cd "\depot\mediaindex"
   $(MAKE) /$(MAKEFLAGS) /F .\libmediaindex.mak CFG="libmediaindex - Win32 Release" RECURSE=1 CLEAN 
   cd "..\authtool\IPVSAxAuthor"

!ENDIF 

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"

"libmp4v2 - Win32 Debug" : 
   cd ".\libmp4v2"
   $(MAKE) /$(MAKEFLAGS) /F .\libmp4v2.mak CFG="libmp4v2 - Win32 Debug" 
   cd ".."

"libmp4v2 - Win32 DebugCLEAN" : 
   cd ".\libmp4v2"
   $(MAKE) /$(MAKEFLAGS) /F .\libmp4v2.mak CFG="libmp4v2 - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"

"libmp4v2 - Win32 Release" : 
   cd ".\libmp4v2"
   $(MAKE) /$(MAKEFLAGS) /F .\libmp4v2.mak CFG="libmp4v2 - Win32 Release" 
   cd ".."

"libmp4v2 - Win32 ReleaseCLEAN" : 
   cd ".\libmp4v2"
   $(MAKE) /$(MAKEFLAGS) /F .\libmp4v2.mak CFG="libmp4v2 - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "IPVSAxAuthor - Win32 Debug"

"libobj60 - Win32 Debug" : 
   cd ".\libobj60"
   $(MAKE) /$(MAKEFLAGS) /F .\libobj60.mak CFG="libobj60 - Win32 Debug" 
   cd ".."

"libobj60 - Win32 DebugCLEAN" : 
   cd ".\libobj60"
   $(MAKE) /$(MAKEFLAGS) /F .\libobj60.mak CFG="libobj60 - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Unicode Release MinDependency"

!ELSEIF  "$(CFG)" == "IPVSAxAuthor - Win32 Release"

"libobj60 - Win32 Release" : 
   cd ".\libobj60"
   $(MAKE) /$(MAKEFLAGS) /F .\libobj60.mak CFG="libobj60 - Win32 Release" 
   cd ".."

"libobj60 - Win32 ReleaseCLEAN" : 
   cd ".\libobj60"
   $(MAKE) /$(MAKEFLAGS) /F .\libobj60.mak CFG="libobj60 - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 


!ENDIF 

