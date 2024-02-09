'Option Explicit

PrintUsage = "Usage: [cscript] " & Wscript.ScriptName & ".zip( file fullpath )" & "MediaStorePath" & "dbFileName"

DIM ZipFolder
DIM MediaStorePath
DIM dbFileName

DIM ZipFile
DIM dbFilePath
DIM blobFolderPath

DIM tempBlobPath
DIM tempDBPath
DIM tempFolderPath

DIM ZipFileBasePath
DIM ZipFileBaseName


Set objectFSO   = CreateObject("Scripting.FileSystemObject")
Set WshShell = WScript.CreateObject("WScript.Shell")
Set shellApp = CreateObject("Shell.Application")

If Wscript.Arguments.Count <> 3 Then
   Wscript.Echo PrintUsage
   Wscript.Quit
Else
   ZipFolder = Wscript.Arguments(0)
   MediaStorePath = Wscript.Arguments(1)
   dbFileName = Wscript.Arguments(2)
End If

dbFilePath = dbFilePath & MediaStorePath & "\db\" & dbFileName
blobFolderPath = blobFolderPath & MediaStorePath & "\blob\" & dbFileName

If objectFSO.FileExists( dbFilePath ) = false Then
   Wscript.Echo "db File not found."
   Wscript.Quit
End If

If objectFSO.FolderExists( blobFolderPath ) = false Then
   Wscript.Echo "Corresponding blob folder not found."
   Wscript.Quit
End If

If objectFSO.FolderExists( ZipFolder ) = false Then
    Wscript.Echo "Folder doesn't exists."
    Wscript.Echo ZipFolder
    Wscript.Quit
End If

ZipFile = ZipFile & ZipFolder & "\" & dbFileName & ".zip"

If objectFSO.FileExists(ZipFile) = true Then
   objectFSO.DeleteFile ZipFile, True
End If

objectFSO.CreateTextFile ZipFile, True

ZipFileBaseName = objectFSO.GetBaseName(ZipFile)
ZipFileBasePath = objectFSO.GetParentFolderName( ZipFile )

tempFolderPath = ZipFileBasePath & "\2dot4media"

If objectFSO.FolderExists(tempFolderPath) = false Then
    objectFSO.CreateFolder(tempFolderPath)
    tempFolderPath = tempFolderPath & "\2dot4media"
    If objectFSO.FolderExists(tempFolderPath) = false Then
        objectFSO.CreateFolder(tempFolderPath)
    End If
End If

If objectFSO.FolderExists(tempFolderPath & "\db") = false Then
    objectFSO.CreateFolder(tempFolderPath & "\db")
End If

tempDBPath = tempDBPath & tempFolderPath & "\db\"  & dbFileName

objectFSO.CopyFile dbFilePath, tempDBPath

DIM firstHyphenPos
DIM dbFileNameWithoutIP
DIM dbPathWithoutIP

firstHyphenPos = instr(dbFileName, "_")
dbFileNameWithoutIP = mid(dbFileName, firstHyphenPos + 1)

dbPathWithoutIP = dbPathWithoutIP & tempFolderPath & "\db\" & dbFileNameWithoutIP


objectFSO.MoveFile tempDBPath, dbPathWithoutIP

dbPath = objectFSO.GetParentFolderName(dbFileNameWithoutIP)
dbPath = objectFSO.GetParentFolderName(dbPath)

If objectFSO.FolderExists(tempFolderPath & "\blob") = false Then
    objectFSO.CreateFolder(tempFolderPath & "\blob")
End If

tempBlobPath = tempBlobPath & tempFolderPath & "\blob\"  & dbFileNameWithoutIP

objectFSO.CopyFolder blobFolderPath, tempBlobPath, True
Wscript.Sleep 200

'Creating the empty zip file first. And then, adding the files.
CreateEmptyZipFile ZipFile

tempFolderPath = objectFSO.GetParentFolderName(tempFolderPath)

'Adding files/folders to the zip file.
DIM res

res = AddFilesToZip( tempFolderPath, ZipFile )
If res = 0 Then
    Wscript.Echo "Created :" 
Else
    WScript.Echo "Failed in creating :"
End If

Wscript.Echo ZipFile

If objectFSO.FolderExists( tempFolderPath ) = True Then
    objectFSO.DeleteFolder tempFolderPath, True
End If


'-------------------------------------------------------------------------------------------
'---------------------------------------------------------------------------------------------
Function AddFilesToZip( FolderPath, ZipFile)
    DIM objItem, objFolder
    DIM intSrcItems, intSkipped
    
    For Each objItem in shellApp.NameSpace( FolderPath ).Items
        If objItem.IsFolder Then
            ' Check if the subfolder is empty, and if
            ' so, skip it to prevent an error message
            Set objFolder = objectFSO.GetFolder( objItem.Path )
            If objFolder.Files.Count + objFolder.SubFolders.Count <> 0 Then
                shellApp.NameSpace( ZipFile ).CopyHere objItem
            End If
        Else
            shellApp.NameSpace( ZipFile ).CopyHere objItem
        End If
    Next
    
    Set objFolder = Nothing
        
    intSrcItems = shellApp.NameSpace( FolderPath  ).Items.Count
    Do Until shellApp.NameSpace( ZipFile ).Items.Count + intSkipped = intSrcItems
        WScript.Sleep 200
    Loop
    
    Set shellApp = Nothing
    
    AddFilesToZip = 0
End Function

Function CreateEmptyZipFile( ZipFile )
    DIM objTxt
    Const ForWriting = 2
    
    Set objTxt = objectFSO.OpenTextFile( ZipFile, ForWriting, True )
    objTxt.Write "PK" & Chr(5) & Chr(6) & String( 18, Chr(0) )
    objTxt.Close
    Set objTxt = Nothing
    
End Function