
'Const DEBUG_MODE = 1

Const MILLI_SECS_PER_SEC = 1000

'***************************************************
' ERROR CODES
'***************************************************
'Not using 1 here, because, shell script returns 1 if it fails because of some other problem.
Const WRONG_NO_OF_ARGUMENT_TO_SCRIPT = 2
Const FFMPEG_DIR_DIDNOT_PROVIDED = 3
Const WRONG_RESOLUTION_ARGUMENTS = 4
Const INPUT_FILE_DOESNOT_EXISTS = 5
Const WRONG_CUT_LIST_ARGUMENTS = 6
Const FFMPEG_FAILED = 7
'***************************************************

Const INPUT_FILE = 0
Const OUTPUT_DIR = 1
Const MEDIA_NAME = 2
Const FORMAT = 3
Const FFMPEG_DIR = 4
Const TEMP_DIR = 5
Const V_CODEC_NAME = 6
Const V_BITRATE = 7
Const V_QUALITY = 8
Const V_RESOLUTION = 9
Const V_FRAMERATE = 10
Const A_CODEC_NAME = 11
Const A_BITRATE = 12
Const CUT_LIST = 13
Const TRANSCODE = 14 'This argument is used to decide the output file. If it is true, then 
                     ' the output file is a transcoded video file. Other wise the ouput file is an .AVS file.

'FFMPEG arguments list
Const FF_INPUT_FILE = 0
Const FF_V_CODEC_NAME = 1
Const FF_V_BITRATE = 2
Const FF_V_QUALITY = 3
Const FF_V_FRAMERATE = 4
Const FF_FORMAT = 5
Const FF_A_CODEC_NAME = 6
Const FF_A_BITRATE = 7
Const FF_OUTPUT_FILE = 8

DIM g_ArgsList(14)
DIM g_FFMPEG_ArgsList(8)

DIM g_AVSDataLines(5)
g_AVSDataLineIndex = 0

DIM g_InstallationPath ' All Transcode tools(ipvs_ffmpeg.exe, IPVSMediaPlugin.dll etc) should be in same folder.
DIM g_OutputFileName
DIM g_OutputFileExtension
DIM g_VideoWidth
DIM g_VideoHeight

g_bSetNewResolution = False

If Wscript.Arguments.Count Mod 2 <> 0 Or Wscript.Arguments.Count < 2 Then
    FillUsageInfo 'Filling the list of command options
    DisplayUsageInfo
    Wscript.Quit WRONG_NO_OF_ARGUMENT_TO_SCRIPT
Else
    FillDefaultInfo
    ParseCmdArgs
End If

DIM g_CmdToExe
DIM g_IPVSWorkArea
DIM g_AVSFile

Set objFSO   = CreateObject("Scripting.FileSystemObject")

' If the output is avs file(not doing transcoding), then overriding this g_IPVSWorkArea with OUTPUT_DIR value.
g_IPVSWorkArea = g_ArgsList(TEMP_DIR) & "\IPVSWorkArea"

CreateIPVSWorkArea
CreateAVSData
FillAVSData

If g_ArgsList(TRANSCODE) = "true" Then
    DoTranscode
    DeleteIPVSWorkArea
End If

'*********************************************************************************************
'*********************************************************************************************

'---------------------------------------------------------------------------------------------
'  Functions Section
'---------------------------------------------------------------------------------------------

Function FillUsageInfo()
    g_ArgsList(INPUT_FILE) = " -i Input_File<absolute path> "
    g_ArgsList(OUTPUT_DIR) = " -o Output_Dir<absolute path> "
    g_ArgsList(MEDIA_NAME) = " -m Media_Name"
    g_ArgsList(FORMAT) = " -f Format(MP4, AVI or TS)"
    g_ArgsList(FFMPEG_DIR) = " -fd Ffmpeg_Dir<absolute path>"
    g_ArgsList(TEMP_DIR) = " -td Temp_Dir<absolute path>"
    g_ArgsList(V_CODEC_NAME) = " -vc Video_Codec_Name<MPEG2, MP4, or H264> "
    g_ArgsList(V_BITRATE) = " -vb Video_Bitrate<in bps> "
    g_ArgsList(V_QUALITY) = " -vq Video_Quality<2-31> "
    g_ArgsList(V_RESOLUTION) = " -s Video_Resolution<WidthXHeight> "
    g_ArgsList(V_FRAMERATE) = " -fps Video_Framerate<Fps> "
    g_ArgsList(A_CODEC_NAME) = " -ac Audio_Codec_Name<MP2/AAC> "
    g_ArgsList(A_BITRATE) = " -ab Audio_Bitrate<In kbps> "
    g_ArgsList(CUT_LIST) = " -C Cut_List<StartTime:EndTime,StartTime:EndTime etc. in milli seconds>"
    g_ArgsList(TRANSCODE) = " -tr Transcode<true or false>(Some times we just need to create avs file.)"
End Function

Function DisplayUsageInfo()
    PrintUsage = "Usage: [cscript] " & Wscript.ScriptName & " ["
    For Each Arg in g_ArgsList
        PrintUsage = PrintUsage & Arg
    Next
    PrintUsage = PrintUsage & "]"

    Wscript.Echo PrintUsage
End Function

Function FillDefaultInfo
    g_ArgsList(INPUT_FILE) = "default"
    g_ArgsList(OUTPUT_DIR) = "default"
    g_ArgsList(MEDIA_NAME) = "default"
    g_ArgsList(FORMAT) = "MP4"
    g_ArgsList(FFMPEG_DIR) = "default"
    g_ArgsList(V_CODEC_NAME) = "MPEG4"
    g_ArgsList(V_BITRATE) = "2048"
    g_ArgsList(V_QUALITY) = "default"
    g_ArgsList(V_RESOLUTION) = "native"
    g_ArgsList(V_FRAMERATE) = "30"
    g_ArgsList(A_CODEC_NAME) = "AAC"
    g_ArgsList(A_BITRATE) = "96k"
    g_ArgsList(CUT_LIST) = "none"
    g_ArgsList(TRANSCODE) = "true"

        ' Get the default temp folder path from registry 
    DIM StrUserProfile
    Set wshShell = WScript.CreateObject( "WScript.Shell" )
    strUserProfile = wshShell.ExpandEnvironmentStrings( "%USERPROFILE%" )
    g_ArgsList(TEMP_DIR) = strUserProfile & "\Local Settings\Temp"
End Function

Function CreateCommandToExecute()
    DIM CmdToExe
    DIM nIndex
    nIndex = 0

    If g_ArgsList(FFMPEG_DIR) <> "default" Then
        CmdToExe = """" & g_ArgsList(FFMPEG_DIR) & "\ipvs_ffmpeg.exe" & """"
    Else 
        Wscript.Echo "Error: Commandline option -fd (FFMPEG Bin path) required"
        Wscript.Quit FFMPEG_DIR_DIDNOT_PROVIDED
    End If

    For Each ArgVal in g_FFMPEG_ArgsList
        Select Case nIndex
            Case FF_INPUT_FILE
                ArgVal = """" & g_AVSFile & """"
                PrintMsg "Input File Name", ArgVal
                CmdToExe = CmdToExe & " -i " & ArgVal

            Case FF_V_CODEC_NAME
                ArgVal = g_ArgsList(V_CODEC_NAME)
                Select Case ArgVal
                    Case "MPEG2"
                        ArgVal = "mpeg2video"
                    Case "MPEG4"
                        ArgVal = "mpeg4"
                    Case "H264"
                        ArgVal = "libx264"
                End Select
                PrintMsg "Video Codec Name", ArgVal
                CmdToExe = CmdToExe & " -vcodec " & ArgVal

            Case FF_V_BITRATE
                If g_ArgsList(V_QUALITY) = "default" Then
                    ArgVal = g_ArgsList(V_BITRATE)
                    PrintMsg "Video Bitrate ", ArgVal
                    CmdToExe = CmdToExe & " -b " & ArgVal
                End If

            Case FF_V_QUALITY
                ArgVal = g_ArgsList(V_QUALITY)
                If ArgVal <> "default" Then
                    PrintMsg "Video Quality", ArgVal
                    CmdToExe = CmdToExe & " -qscale " & ArgVal
                End If

            Case FF_V_FRAMERATE
                ArgVal = g_ArgsList(V_FRAMERATE)
                PrintMsg "Video Framerate", ArgVal
                CmdToExe = CmdToExe & " -r " & ArgVal
            
            Case FF_FORMAT
                DIM FomratArg
                ArgVal = g_ArgsList(FORMAT)
                Select Case ArgVal
                    Case "MP4"
                        FormatArg = "mp4"
                        g_OutputFileExtension = ".mp4"
                    Case "AVI"
                        FormatArg = "avi"
                        g_OutputFileExtension = ".avi"
                    Case "TS"
                        FormatArg = "mpegts"
                        g_OutputFileExtension = ".ts"
                End Select
                CmdToExe = CmdToExe & " -f " & FormatArg

            Case FF_A_CODEC_NAME
                DIM AudioFormat
				ArgVal = g_ArgsList(A_CODEC_NAME)
				Select Case ArgVal
                    Case "MP2"
					    AudioFormat = "mp2"
					Case "AAC"
					    AudioFormat = "libvo_aacenc"
				End Select

                PrintMsg "Audio Codec Name", AudioFormat
                CmdToExe = CmdToExe & " -acodec " & AudioFormat

            Case FF_A_BITRATE
                ArgVal = g_ArgsList(A_BITRATE)
                PrintMsg "Audio Bitrate", ArgVal
                CmdToExe = CmdToExe & " -ab " & ArgVal & "k "

            Case FF_OUTPUT_FILE
                ArgVal = """" & g_ArgsList(OUTPUT_DIR) & "\" & g_ArgsList(MEDIA_NAME) & g_OutputFileExtension & """"
                PrintMsg "Output File Name", ArgVal
                g_OutputFileName = ArgVal

        End Select
        nIndex = nIndex + 1
    Next

    CmdToExe = CmdToExe & " -y " & g_OutputFileName

    CreateCommandToExecute = CmdToExe
End Function

Function ParseCmdArgs()
    DIM nIndex
    nIndex = 0

    For Each ArgVal In Wscript.Arguments
        nIndex = nIndex + 1
        Select Case ArgVal
            Case "-i"
                g_ArgsList(INPUT_FILE) = Wscript.Arguments(nIndex)
            Case "-o"
                g_ArgsList(OUTPUT_DIR) = Wscript.Arguments(nIndex)
            Case "-m"
                g_ArgsList(MEDIA_NAME) = Wscript.Arguments(nIndex)
            Case "-f"
                g_ArgsList(FORMAT) =  Wscript.Arguments(nIndex)
            Case "-fd"
                g_ArgsList(FFMPEG_DIR) = Wscript.Arguments(nIndex)
                g_InstallationPath = g_ArgsList(FFMPEG_DIR)
            Case "-td"
                g_ArgsList(TEMP_DIR) = Wscript.Arguments(nIndex)
            Case "-vc"
                g_ArgsList(V_CODEC_NAME) = Wscript.Arguments(nIndex)
            Case "-vb"
                g_ArgsList(V_BITRATE) = Wscript.Arguments(nIndex)
            Case "-vq"
                g_ArgsList(V_QUALITY) = Wscript.Arguments(nIndex)
            Case "-s"
                g_ArgsList(V_RESOLUTION) = Wscript.Arguments(nIndex)
                If g_ArgsList(V_RESOLUTION) <> "native" Then
                    g_bSetNewResolution = True
                    arrRes = Split(g_ArgsList(V_RESOLUTION), "X")
                    If UBound(arrRes) <> 1 Then
                        Wscript.Echo "Wrong resolution. Resolution should be as WidthXHeight."
                        Wscript.Quit WRONG_RESOLUTION_ARGUMENTS
                    End If
                    g_VideoWidth = arrRes(0)
                    g_VideoHeight = arrRes(1)
                End If
            Case "-fps"
                g_ArgsList(V_FRAMERATE) = Wscript.Arguments(nIndex)
            Case "-ac"
                g_ArgsList(A_CODEC_NAME) = Wscript.Arguments(nIndex)
            Case "-ab"
                g_ArgsList(A_BITRATE) = Wscript.Arguments(nIndex)
            Case "-C"
                g_ArgsList(CUT_LIST) = Wscript.Arguments(nIndex)
            Case "-tr"
                g_ArgsList(TRANSCODE) = Wscript.Arguments(nIndex)
        End Select
    Next
End Function

Function CreateIPVSWorkArea()
    If g_ArgsList(TRANSCODE) = "false" Then
        g_IPVSWorkArea = g_ArgsList(OUTPUT_DIR)
    End If

    If Not objFSO.FolderExists(g_IPVSWorkArea) Then
        g_IPVSWorkArea = objFSO.CreateFolder(g_IPVSWorkArea)
    End If

    If objFSO.FileExists(g_ArgsList(INPUT_FILE)) Then
        g_AVSFile = g_IPVSWorkArea & "\" & g_ArgsList(MEDIA_NAME) & ".avs"
    Else
	    If objFSO.FolderExists(g_ArgsList(INPUT_FILE)) Then
            g_AVSFile = g_IPVSWorkArea & "\" & g_ArgsList(MEDIA_NAME) & ".avs"
		Else
		    Wscript.Echo "Input file : " & g_argsList(INPUT_FILE) & " Doesn't exists."
            Wscript.Quit INPUT_FILE_DOESNOT_EXISTS
		End If
    End If
End Function

Function DeleteIPVSWorkArea()
    If objFSO.FolderExists(g_IPVSWorkArea) Then
        objFSO.DeleteFile(g_IPVSWorkArea & "\*.*")
        objFSO.DeleteFolder(g_IPVSWorkArea)
    End If
End Function

Function FillDefaultAVSData()
    g_AVSDataLines(g_AVSDataLineIndex) = "Load_Stdcall_Plugin(" & """" & g_InstallationPath & "\IPVSMediaPlugin.dll" & """" & ")" 
    g_AVSDataLineIndex = g_AVSDataLineIndex + 1

    g_AVSDataLines(g_AVSDataLineIndex) = "IPVSMediaSource(" & """" & g_ArgsList(INPUT_FILE) & """" & "," & g_ArgsList(V_FRAMERATE)
    If g_bSetNewResolution = True Then
        g_AVSDataLines(g_AVSDataLineIndex) = g_AVSDataLines(g_AVSDataLineIndex) & "," & g_VideoWidth & "," & g_VideoHeight & ")"
    Else
        g_AVSDataLines(g_AVSDataLineIndex) = g_AVSDataLines(g_AVSDataLineIndex) &  ")"  
    End If
    g_AVSDataLineIndex = g_AVSDataLineIndex + 1
End Function

Function FillCutListIfAvailable
    Tokens = Split(g_ArgsList(CUT_LIST), ",")
    
    If Tokens(0) <> "none" Then
        nIndex = 0
        For Each Token In Tokens
            If nIndex > 0 Then
                g_AVSDataLines(g_AVSDataLineIndex) = g_AVSDataLines(g_AVSDataLineIndex) & " + "
            End If
            nIndex = nIndex + 1
            TimeValues = Split(Token, ":")
            
            If UBound(TimeValues) <> 1 Then
                Wscript.Echo "Wrong number of arguments."
                Wscript.Echo "It should be -C startime:endTime,startTime:endTime etc."
                Wscript.Quit WRONG_CUT_LIST_ARGUMENTS
            End If

            StartTime = TimeValues(0)
            EndTime = TimeValues(1)
            StartFrame = CLng((g_ArgsList(V_FRAMERATE)/MILLI_SECS_PER_SEC) * StartTime)
            EndFrame = CLng((g_ArgsList(V_FRAMERATE)/MILLI_SECS_PER_SEC) * EndTime)

            g_AVSDataLines(g_AVSDataLineIndex) = g_AVSDataLines(g_AVSDataLineIndex) & "Trim(" & StartFrame & "," & EndFrame & ")"
        Next
    End If
End Function

Function CreateAVSData()
    FillDefaultAVSData
    FillCutListIfAvailable
End Function

Function FillAVSData()
    
    Set avsFile = ObjFSO.CreateTextFile(g_AVSFile, True)
    
    For Each line In g_AVSDataLines
        avsFile.WriteLine(line)
    Next

End Function

Function DoTranscode()
    g_CmdToExe = CreateCommandToExecute()
    
    PrintMsg "Command to Execute", g_CmdToExe
    
    Set objShell = CreateObject("WScript.Shell")
    iErrorCode = objShell.Run(g_CmdToExe, 0, True)
    
    If iErrorCode <> 0 Then
        Wscript.Echo "Transcoding failed."
        Wscript.Quit FFMPEG_FAILED
    End If
    
    Set objShell = Nothing
End Function

Function PrintMsg(Str, Val)
    If DEBUG_MODE <> 0 Then
        Wscript.Echo Str & " : " & Val
    End If
End Function
