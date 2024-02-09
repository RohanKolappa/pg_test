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
ReleaseDir = "" & BuildDir & "/v2dlibrary/Release"
DebugDir = "" & BuildDir & "/v2dlibrary/Debug"
Const ReleaseFolderPath = "Release"
Const CopyDllFromReleaseTo = "v2dlib_"
ReleaseFolderInRx = "" & BuildDir & "/v2dlibrary/rx/Release"
DebugFolderInRx = "" & BuildDir & "/v2dlibrary/rx/Debug"
ReleaseFolderInV2dLibrary = "" & BuildDir & "\v2dlibrary\v2dlibrary\Release"
DebugFolderInV2dLibrary = "" & BuildDir & "\v2dlibrary\v2dlibrary\Debug"
ReleaseDirx64  = "" & BuildDir & "\v2dlibrary\x64\Release"
DebugDirx64  = "" & BuildDir & "\v2dlibrary\x64\Debug"
DeleteReleaseFolderx64 = "" & BuildDir & "\v2dlibrary\v2dlibrary\x64"
DeleteReleaseFolderInRx = "" & BuildDir & "\v2dlibrary\rx\x64"
FileToZip = "v2dlib"
const zipfile = ".zip"
DeleteFolder BuildDir

outMsg = ""
errMsg = ""
cmd    = ""
		   
DIM AllProjects(4)
		  
AllProjects(0) =  DevEnvExe & " " & BuildDir & "/v2dlibrary/v2dlibrary.sln " _
           & "/rebuild Release|win32 /Out " & BuildLog

AllProjects(1) =  DevEnvExe & " " & BuildDir & "/v2dlibrary/v2dlibrary.sln " _
           & "/rebuild Release|x64 /Out " & BuildLog

AllProjects(2) =  DevEnvExe & " " & BuildDir & "/v2dlibrary/v2dlibrary.sln " _
           & "/rebuild Debug|win32 /Out " & BuildLog

AllProjects(3) =  DevEnvExe & " " & BuildDir & "/v2dlibrary/v2dlibrary.sln " _
           & "/rebuild Debug|x64 /Out " & BuildLog		   
		   

LibFile = Array(".dll", ".exe", ".lib")		   
		   
V2dLib = Array("v2dlib")

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
	
	For Each Project in AllProjects
        If (Project <> "") Then
            ExecuteCmd Project, outMsg, errMsg
	End If
	Next
    
	CleanUpFolders
		
Sub CleanUpFolders
   
    DeleteUnusedFiles(ReleaseDir)
	DeleteUnusedFiles(DebugDir)
    DeleteUnusedFiles(ReleaseDirx64)
	DeleteUnusedFiles(DebugDirx64)
  
    DeleteFolder(ReleaseFolderInRx)
	DeleteFolder(DebugFolderInRx)
    DeleteFolder(DeleteReleaseFolderInRx)
    DeleteFolder(DeleteReleaseFolderx64)
    DeleteFolder(ReleaseFolderInV2dLibrary)
	DeleteFolder(DebugFolderInV2dLibrary)
   
	CreateZipFile	 
	           
End Sub

sub DeleteUnusedFiles(FilesToDelete)

     If objFSO.FolderExists(FilesToDelete) Then
      
   	    For Each sFile In objFSO.GetFolder(FilesToDelete).Files
		'  If ((right(objFSO.GetFile(sFile), 4) <> ".dll") or (right(objFSO.GetFile(sFile), 4) <> ".exe") or (right(objFSO.GetFile(sFile), 4) <> ".lib")) Then
		   filename  = objFSO.GetExtensionName(sFile) 
		  ' If  ( objFSO.GetExtensionName(sFile) <> "dll" Or objFSO.GetExtensionName(sFile) <> "exe"  ) Then
		   ' If ( filename <> "dll") or (  filename <> "exe") Then
		   If filename = "dll" Then
		   ElseIf filename = "exe" Then
		   ElseIf filename = "lib" Then
		   ElseIf filename = "pdb" Then
		   Else 
		      objFSO.DeleteFile(sFile), True
		   End If
		Next
         
     End If
End Sub

Sub CreateZipFile
    FileToZip =    FileToZip &"_" & SVNREVNUM & zipfile 
    CreateObject("Scripting.FileSystemObject").CreateTextFile(WshShell.CurrentDirectory & "\" & FileToZip, True).Write "PK" & Chr(5) & Chr(6) & String(18, vbNullChar)

    Wscript.Echo WshShell.CurrentDirectory & "\" & FileToZip 

    Set objShell = CreateObject("Shell.Application")
    Set Source = objShell.NameSpace(WshShell.CurrentDirectory & "\" & BuildDir ).Items
    objShell.NameSpace(WshShell.CurrentDirectory & "\" & FileToZip).CopyHere(Source)

    Do Until Source.Count <= objShell.NameSpace(WshShell.CurrentDirectory & "\" & FileToZip).Items.Count
       wScript.Sleep 1000
    Loop
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
	
	
Sub DeleteFolder(folderToDelete)
    
    If objFSO.FolderExists(folderToDelete) Then
       objFSO.DeleteFolder folderToDelete, True
       Wscript.Echo "Deleted folder: " & folderToDelete
    End If
    
End Sub