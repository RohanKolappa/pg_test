'Option Explicit

PrintUsage = "Usage: [cscript] " & WScript.ScriptName & ".zip( file fullpath )" & "MediaStorePath" & "dbFileName"

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


Set objectFSO = CreateObject("Scripting.FileSystemObject")
Set shellApp = CreateObject("Shell.Application")

If WScript.Arguments.Count <> 3 Then
   WScript.Echo PrintUsage
   WScript.Quit
Else
   ZipFolder = WScript.Arguments(0)
   MediaStorePath = WScript.Arguments(1)
   dbFileName = WScript.Arguments(2)
End If

dbFilePath = dbFilePath & MediaStorePath & "\db\" & dbFileName
blobFolderPath = blobFolderPath & MediaStorePath & "\blob\" & dbFileName

If objectFSO.FileExists( dbFilePath ) = false Then
   WScript.Echo "db File not found."
   WScript.Quit
End If

If objectFSO.FolderExists( blobFolderPath ) = false Then
   WScript.Echo "Corresponding blob folder not found."
   WScript.Quit
End If

If objectFSO.FolderExists( ZipFolder ) = false Then
    WScript.Echo "Folder doesn't exists."
    WScript.Echo ZipFolder
    WScript.Quit
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
    WScript.Echo "Created :" 
Else
    WScript.Echo "Failed in creating :"
End If

WScript.Echo ZipFile

If objectFSO.FolderExists( tempFolderPath ) = True Then
    objectFSO.DeleteFolder tempFolderPath, True
End If


'-------------------------------------------------------------------------------------------
'---------------------------------------------------------------------------------------------
Function AddFilesToZip( FolderPath, ZipFile)
    DIM objItem, objFolder
    DIM intSrcItems, intSkipped 
    DIM dZip
	Set dZip = shellApp.NameSpace(ZipFile)
    For Each objItem in shellApp.NameSpace( FolderPath ).Items
        If objItem.IsFolder Then
            ' Check if the subfolder is empty, and if
            ' so, skip it to prevent an error message
            Set objFolder = objectFSO.GetFolder( objItem.Path )
            If objFolder.Files.Count + objFolder.SubFolders.Count <> 0 Then
               dZip.CopyHere objItem
            End If
        Else
            dZip.CopyHere objItem
        End If
    Next
    
    Set objFolder = Nothing
        
    intSrcItems = shellApp.NameSpace( FolderPath  ).Items.Count
    Do Until intSrcItems <= dZip.Items.Count
        WScript.Sleep 300
    Loop
    
	Set dZip = Nothing
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