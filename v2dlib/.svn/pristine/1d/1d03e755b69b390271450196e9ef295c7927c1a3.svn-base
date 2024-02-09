BuildType = ""
EnableLicense = 0

PrintUsage = "Usage: cscript " & Wscript.ScriptName & " [branch name]"
Const AbortMsg   = "Aborting build script..."

Set args = Wscript.Arguments

If Wscript.Arguments.Count < 1 Then
    BuildDir = "v2dlib"
ElseIf Wscript.Arguments.Count > 1 Then
    Wscript.Echo PrintUsage
    Wscript.Quit
Else
    BuildDir = args(0)
    Wscript.Echo BuildDir
End If  


Set objFSO   = CreateObject("Scripting.FileSystemObject")
Set WshShell = WScript.CreateObject("WScript.Shell")
Set objRegex = new RegExp

SVNREVNUM=""
SVNBUILDNUM=""
SVNBRANCHNAME="branches"

Const SVNROOTPATH="http://svn.ipvideosys.com/svn/svnroot"

Const DevEnvExe = "C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\ide\devenv.exe"
Const BuildLog = "build.log"

FileToZip = "v2drxlib"
const zipfile = ".zip"

InlcudeDir = "" & BuildDir & "/include"

LibDir = "" & BuildDir & "/lib"
Win32LibDir = "" & LibDir & "/win32"
Win64LibDir = "" & LibDir & "/x64"

BinDir = "" & BuildDir & "/bin"
Win32BinDir = "" & BinDir & "/win32"
Win64BinDir = "" & BinDir & "/x64"

DocDir = "" & BuildDir & "/rxdocs"

V2dRxWin32LibFile = "" & BuildDir & "/v2drxlib/Release/v2drxlib.lib"
V2dRxWin64LibFile = "" & BuildDir & "/v2drxlib/x64/Release/v2drxlib.lib"

V2dRxWin32Dll = "" & BuildDir & "/v2drxlib/Release/v2drxlib.dll"
V2dRxWin64Dll = "" & BuildDir & "/v2drxlib/x64/Release/v2drxlib.dll"

Win32RxBinary = "" & BuildDir & "/v2drx_test/Release/rx.exe"
Win64RxBinary = "" & BuildDir & "/v2drx_test/x64/Release/rx.exe"

RxSourceFile = "" & BuildDir & "/v2drx_test/rx.cpp"
GetOptFile = "" & BuildDir & "/pgetopt.c"

readmeFile = "" & BuildDir & "/v2drxlib/ReadMe.txt"
APIDoc = ".\" & "V2DRxLibAPI.pdf"

V2dRxLibLicenseFile = "" & BuildDir & "/v2drxlib/V2D Software Decoder EULA 201405.docx"

DeleteFolder BuildDir


outMsg = ""
errMsg = ""
cmd    = ""

DIM AllProjects(2)
DIM RxProjects(2)

AllProjects(0) =  DevEnvExe & " " & BuildDir & "/v2drxlib/v2drxlib.sln " _
           & "/rebuild Release|win32 /Out " & BuildLog
AllProjects(1) =  DevEnvExe & " " & BuildDir & "/v2drxlib/v2drxlib.sln " _
           & "/rebuild Release|x64 /Out " & BuildLog

RxProjects(0) =  DevEnvExe & " " & BuildDir & "/v2drx_test/rx.sln " _
           & "/rebuild Release|win32 /Out " & BuildLog
RxProjects(1) =  DevEnvExe & " " & BuildDir & "/v2drx_test/rx.sln " _
           & "/rebuild Release|x64 /Out " & BuildLog

V2DFiles = Array("bitmap.hh", "bitstream.h", "codecdefs.h", "colortransform.h", "ipputils.h","listutils.h", "pgetopt.h", "ScapProcessor.hh", "slicedecoder.hh", "srd.h","v2d.h", "v2dcommon.h", "v2dlib_compatibility.h", "vframedecoder.hh", "v2dcommon.h")


ThisDir = WshShell.CurrentDirectory
WshShell.CurrentDirectory = ThisDir 

GetBranchNameAndRevNum SVNBRANCHNAME, SVNREVNUM
If SVNBRANCHNAME = "trunk" Then
    cmd =  "/trunk"
    cmd = "svn export  "  & SVNROOTPATH & "/" & "v2dlib" & "/" & cmd & "/"  & "  " & BuildDir 
Else
    cmd =  "branches" 
    cmd = "svn export  "  & SVNROOTPATH & "/" & "v2dlib" & "/" & cmd & "/" & BuildDir
End If 

ExecuteCmd cmd, outMsg, errMsg
WshShell.CurrentDirectory = ThisDir 

CreateFolder InlcudeDir

CreateFolder LibDir
CreateFolder Win32LibDir
CreateFolder Win64LibDir

CreateFolder BinDir
CreateFolder Win32BinDir
CreateFolder Win64BinDir

CreateFolder DocDir

CopyFilesToInlcudeDir
CopyFilesToDocDir

For Each Project in AllProjects
    If (Project <> "") Then
        ExecuteCmd Project, outMsg, errMsg
    End If
Next

CopyLibFiles

For Each Project1 in RxProjects
    If (Project1 <> "") Then
        ExecuteCmd Project1, outMsg, errMsg
    End If
Next

CopyBinFiles

CreateZipFile

'+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
' Functions
'+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Sub CopyFilesToInlcudeDir
    For Each Files in V2DFiles
        Wscript.Echo "" & BuildDir & "\" & Files
        If objFSO.FileExists("" & BuildDir & "\" & Files) Then
            objFSO.CopyFile "" & BuildDir & "\" & Files, InlcudeDir & "\", True
        Else
            Wscript.Echo  "No files"
        End If
    Next
End Sub	

Sub CopyFilesToDocDir
    If objFSO.FileExists(readmeFile) Then
        Wscript.Echo "Copying ReadMe file "
        objFSO.CopyFile readmeFile, DocDir & "\", True
    End If

    APIDoc = WshShell.CurrentDirectory & APIDoc
    If objFSO.FileExists(APIDoc) Then
        Wscript.Echo "Copying API Document "
        objFSO.CopyFile APIDoc, DocDir & "\", True
    Else 
        Wscript.Echo "API Pdf " & APIDoc & " not found !!"
        Wscript.Quit
    End If
End Sub

Sub CopyLibFiles
    If objFSO.FileExists(V2dRxWin32LibFile) Then
        Wscript.Echo "Copying Win32 lib file "
        objFSO.CopyFile V2dRxWin32LibFile, Win32LibDir & "\", True
    End If

    If objFSO.FileExists(V2dRxWin64LibFile) Then
        Wscript.Echo "Copying x64 lib file "
        objFSO.CopyFile V2dRxWin64LibFile, Win64LibDir & "\", True
    End If
End Sub

Sub CopyBinFiles
    objFSO.CopyFile Win32RxBinary, Win32BinDir & "\", True
    objFSO.CopyFile Win64RxBinary, Win64BinDir & "\", True

    objFSO.CopyFile V2dRxWin32Dll, Win32BinDir & "\", True
    objFSO.CopyFile V2dRxWin64Dll, Win64BinDir & "\", True
End Sub

Sub CreateZipFile
    tozipfile = FileToZip &"_" & SVNREVNUM 
    CreateFolder tozipfile
    SourceDir = tozipfile & "\" & "src"
    CreateFolder  SourceDir

    objFSO.CopyFile RxSourceFile, SourceDir & "\", True
    objFSO.CopyFile GetOptFile, SourceDir & "\", True

    objFSO.CopyFolder  BinDir,  tozipfile & "\", True
    objFSO.CopyFolder  LibDir,  tozipfile & "\", True
    objFSO.CopyFolder  InlcudeDir,  tozipfile & "\", True
    objFSO.CopyFolder  DocDir, tozipfile & "\docs", True
	
	objFSO.CopyFile V2dRxLibLicenseFile, tozipfile & "\", True

    FileToZip =    FileToZip &"_" & SVNREVNUM & zipfile
    CreateObject("Scripting.FileSystemObject").CreateTextFile(WshShell.CurrentDirectory & "\" & FileToZip, True).Write "PK" & Chr(5) & Chr(6) & String(18, vbNullChar)

    Wscript.Echo WshShell.CurrentDirectory & "\" & FileToZip 

    Set objShell = CreateObject("Shell.Application")
    Set Source = objShell.NameSpace(WshShell.CurrentDirectory & "\" & tozipfile ).Items
    objShell.NameSpace(WshShell.CurrentDirectory & "\" & FileToZip).CopyHere(Source)

    Do Until Source.Count <= objShell.NameSpace(WshShell.CurrentDirectory & "\" & FileToZip).Items.Count
       wScript.Sleep 1000
    Loop

    DeleteFolder tozipfile

End Sub

Sub ExecuteCmd(cmd, stdOutMsg, stdErrMsg)

    stdOutMsg = ""
    stdErrMsg = ""

    Wscript.Echo cmd
    Set oExec = WshShell.Exec(cmd)
    stdOutMsg = oExec.StdOut.ReadAll
    stdErrMsg = oExec.StdErr.ReadAll

    If (stdOutMsg <> "") Then
        Wscript.Echo stdOutMsg
    End If
    If (stdErrMsg <> "") Then
        Wscript.Echo stdErrMsg
    End If

End Sub


Sub GetBranchNameAndRevNum(BranchName, RevNum)
   ' Find out if we are on the trunk or a branch

    If BuildDir = "v2dlib" Then
        BranchName = "trunk"
        cmd = "svn info  "  & SVNROOTPATH & "/" & "v2dlib" & "/" & cmd 
    Else
        BranchName = "branches"
        cmd = "svn info  "  & SVNROOTPATH & "/" & "v2dlib" & "/" & cmd 
    End If

    ExecuteCmd cmd, outMsg, errMsg  
    objRegex.IgnoreCase = True
    objRegex.Pattern = "URL: http://svn\.ipvideosys\.com/svn/svnroot/v2dlib/branches/(\w+)"
    objRegex.Global = False

    Wscript.Echo Pattern
    Set Matches = objRegex.Execute(outMsg)
    For Each Match in Matches   ' Iterate Matches collection.
        BranchName = Match.SubMatches(0)
    Next

    objRegex.IgnoreCase = True
    objRegex.Pattern = "Revision: (\d+)"
    objRegex.Global = False

    Set Matches = objRegex.Execute(outMsg)
    For Each Match in Matches   ' Iterate Matches collection.
        RevNum = Match.SubMatches(0)
    Next

End Sub

sub CreateFolder(FolderToCreate)
    If objFSO.FolderExists(FolderToCreate) Then
        objFSO.DeleteFolder FolderToCreate, True
        Wscript.Echo "Deleted folder: " & FolderToCreate
    End If
    objFSO.CreateFolder(FolderToCreate)
    Wscript.Echo "Created folder: " & FolderToCreate
End Sub	

Sub DeleteFolder(folderToDelete)
    If objFSO.FolderExists(folderToDelete) Then
         objFSO.DeleteFolder folderToDelete, True
         Wscript.Echo "Deleted folder: " & folderToDelete
    End If
End Sub