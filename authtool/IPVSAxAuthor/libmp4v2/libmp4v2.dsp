# Microsoft Developer Studio Project File - Name="libmp4v2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libmp4v2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmp4v2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmp4v2.mak" CFG="libmp4v2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmp4v2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libmp4v2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmp4v2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\mpeg4ip\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmp4v2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\mpeg4ip\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libmp4v2 - Win32 Release"
# Name "libmp4v2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\3gp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_amr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_avc1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_avcC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_d263.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_damr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_dref.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_elst.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_enca.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_encv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_free.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_ftyp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_hdlr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_hinf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_hnti.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_href.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_mdat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_mdhd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_meta.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_mp4s.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_mp4v.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_mvhd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_ohdr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_root.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_rtp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_s263.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_sdp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_smi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_sound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_standard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_stbl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_stdp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_stsc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_stsd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_stsz.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_tfhd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_tkhd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_treftype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_trun.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_udta.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_url.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_urn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_video.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atom_vmhd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\descriptors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\isma.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4atom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4container.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4descriptor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4file_io.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4info.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4meta.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4property.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4track.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\ocidescriptors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\odcommands.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\qosqualifiers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\rtphint.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\atoms.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\descriptors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4array.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4atom.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4container.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4descriptor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4file.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4property.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4track.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\mp4util.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\ocidescriptors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\odcommands.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\qosqualifiers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\mpeg4ip\lib\mp4v2\rtphint.h
# End Source File
# End Group
# End Target
# End Project
