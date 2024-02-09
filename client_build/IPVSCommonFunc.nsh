!ifndef ___IPVSCOMMONFUNC__NSH___
!define ___IPVSCOMMONFUNC__NSH___ 

!include "XML.nsh"
!include "IPVSFilePaths.nsh"

; Definitions for Java 6.0
!define JAVAEXE "java.exe"
!define JRE_INSTALLER_EXE "jre-6u14-windows-i586-iftw.exe"
!define JRE_VERSION_FRIENDLY "6.0.14"
!define JRE_VERSION "6.0.140.0"

!macro SETOUTFILE FILEPREFIX 

!if ${RELEASE_BUILD} < 1
!define OUTFILESUFFIX "Developer"
!else
!define OUTFILESUFFIX "${PRODUCT_VERSION}"
!endif

OutFile "${FILEPREFIX}-${OUTFILESUFFIX}.exe"

!macroend

; Note: Below macro is not working as expected
!macro TRIMPRODUCTVERSIONSTR PRODVERSTR

!searchreplace PRODVERSTR ${PRODVERSTR} "r" ""
!searchreplace PRODVERSTR ${PRODVERSTR} "b" ""

!macroend

!macro CREATESTARTMENUSHORTCUT PRODNAME LNKNAME TARGETNAME PARAMS ICONFILE

  ${GetParent} ${TARGETNAME} $OUTDIR
  CreateDirectory "$SMPROGRAMS\${PRODNAME}"
  CreateShortCut "$SMPROGRAMS\${PRODNAME}\${LNKNAME}" \
    ${TARGETNAME} "${PARAMS}" "${ICONFILE}"

!macroend

!macro DELETESTARTMENUSHORTCUT PRODNAME LNKNAME 
  Delete "$SMPROGRAMS\${PRODNAME}\${LNKNAME}"
!macroend

!macro CREATEDESKTOPSHORTCUT LNKNAME TARGETNAME PARAMS ICONFILE

  ${GetParent} ${TARGETNAME} $OUTDIR
  CreateShortCut "$DESKTOP\${LNKNAME}" ${TARGETNAME} "${PARAMS}"\
    "${ICONFILE}"

!macroend

!macro DELETEDESKTOPSHORTCUT LNKNAME 
  Delete "$DESKTOP\${LNKNAME}"
!macroend

!macro BACKUPFILE ABSFILEPATH BACKUPDIR
  Push $R0

  !insertmacro MAKEDIR "${BACKUPDIR}"

  ${GetFileName} ${ABSFILEPATH} $R0
!ifdef SCRIPT_DEBUG
    MessageBox MB_OK "Backing up ${ABSFILEPATH} to ${BACKUPDIR}\$R0"
!endif
  Delete "${BACKUPDIR}\$R0"
  CopyFiles /SILENT ${ABSFILEPATH} "${BACKUPDIR}\$R0"

  Pop $R0
!macroend

!macro RESTOREFILE ABSFILEPATH BACKUPDIR
  Push $R0
  ClearErrors

  ${GetFileName} ${ABSFILEPATH} $R0
!ifdef SCRIPT_DEBUG
    MessageBox MB_OK "Restoring ${ABSFILEPATH} from ${BACKUPDIR}\$R0"
!endif
  CopyFiles /SILENT "${BACKUPDIR}\$R0" ${ABSFILEPATH} 
  ClearErrors 

  Delete "${BACKUPDIR}\$R0"

  Pop $R0
!macroend

Function IsCollabClientInstalled
  Push $R0
  ClearErrors
  ReadRegStr $R0 HKLM "${PRODUCT_DIR_REGKEY}" ""
  IfErrors lbl_na
    StrCpy $R0 1
  Goto lbl_end
  lbl_na:
    StrCpy $R0 0
  lbl_end:

  Exch $R0
FunctionEnd

Function IsUserAdmin
  Push $R0
  Push $R1
  Push $R2

  ClearErrors
  UserInfo::GetName
  IfErrors Win9x
  Pop $R1
  UserInfo::GetAccountType
  Pop $R2

  StrCmp $R2 "Admin" 0 Continue
  StrCpy $R0 "true"
  Goto Done

  Continue:
  StrCmp $R2 "" Win9x
  StrCpy $R0 "false"
  Goto Done

  Win9x:
  StrCpy $R0 "true"

  Done:

  Pop $R2
  Pop $R1
  Exch $R0
FunctionEnd

Function un.RefreshDesktopWindow

  Var /GLOBAL ProgmanHwnd
  Var /GLOBAL ShellHwnd
  Var /GLOBAL DesktopHwnd
  
  FindWindow $ProgmanHwnd "Progman" "Program Manager"
  FindWindow $ShellHwnd "SHELLDLL_DefView" "" $ProgmanHwnd 0
  FindWindow $DesktopHwnd "SysListView32" "" $ShellHwnd 0

  System::Call "user32.dll::PostMessage(i $DesktopHwnd, i 256, i 116, i 0)"
  System::Call "user32.dll::PostMessage(i $DesktopHwnd, i 257, i 116, i 0x80000000)"

FunctionEnd

Function CheckJRE
    ;  Returns the full path of a valid java.exe on top of stack
    ;  Looks in the registry

    Push $R0
    Push $R1
    ClearErrors
!ifdef SCRIPT_DEBUG
    MessageBox MB_OK "Searching JRE in Registry"
!endif
    ReadRegStr $R1 HKLM "SOFTWARE\JavaSoft\Java Runtime Environment" \
      "CurrentVersion"
    ReadRegStr $R0 HKLM "SOFTWARE\JavaSoft\Java Runtime Environment\$R1" \
      "JavaHome"
    StrCpy $R0 "$R0\bin\${JAVAEXE}"
    IfErrors JreNotFound
    IfFileExists $R0 JreFound JreNotFound

  JreNotFound:
    StrCpy $R0 ""

  JreFound:
    ${If} $R0 != ""
!ifdef SCRIPT_DEBUG
    MessageBox MB_OK "Found JRE in $R0"
!endif
    ${EndIf}

    Pop $R1
    Exch $R0
FunctionEnd

Function CheckJREVersion
    ; Pass the "java.exe" path by $R0
    ; Returns the JRE version on top of the stack
    Push $R1

    ClearErrors
    ; Get the file version of java.exe
    ${GetFileVersion} $R0 $R1

    Exch $R1
FunctionEnd

Function InstallJRE
    ; Expects the install exe in JRE_INSTALLER_EXE
    ; Returns the installation status on top of stack
    Push $R1
    StrCpy $R1 "Success"

    SetOutPath $TEMP
    File "${SETUPDIR}\${JRE_INSTALLER_EXE}"
    ClearErrors
    Push $R2
    StrCpy $R2 '/s "/qn IEXPLORER=1 ADDLOCAL=ALL UPDATE=0 REBOOT=Suppress /L $TEMP\jre-setup.log"'
    ExecWait '"$TEMP\${JRE_INSTALLER_EXE}" $R2'
    IfErrors 0 +2
      StrCpy $R1 "Failure"
    Delete /REBOOTOK "$TEMP\${JRE_INSTALLER_EXE}"

!ifdef SCRIPT_DEBUG
    MessageBox MB_OK "JRE Installation Status: $R1"
!endif
    Pop $R2
    Exch $R1
FunctionEnd

Function ModifyJavaPolicy

    ; Modifies the java.policy to grant disk I/O access
    ; Expects the absolute path to java.exe in $R0
    ; Returns the error status on top of the stack
    Push $0
    Push $1
    Push $2
    Push $3
    Push $4
    Push $5

    StrCpy $0 "Failure"
    ClearErrors

    ; Grant Disk I/O access in java.policy
    ${WordReplace} $R0 "bin\java.exe" "lib\security\java.policy" "E+1" $1
!ifdef SCRIPT_DEBUG
    ; If error occurs $1 = 1, 2 or 3
    MessageBox MB_OK "Located java.policy @ $1"
!endif
    IfErrors ModifyJavaPolicy_lbl_end 0

    ;Check if access already exists (e.g. this is an update)
    FileOpen $3 $1 r
    IfErrors ModifyJavaPolicy_lbl_end 0

    StrCpy $4 0
    ${While} $4 == 0
        FileRead $3 $5
        IfErrors 0 CheckIPVSStr
          FileClose $3 
!ifdef SCRIPT_DEBUG
          MessageBox MB_OK "Reached end of $1, Barco Access not found"
!endif
          ${Break}

CheckIPVSStr:
        StrCmp $5 "// Begin Barco Modifications$\r$\n" 0 ContStrSearch
!ifdef SCRIPT_DEBUG
        MessageBox MB_OK "Barco Access exists in $1"          
!endif
        FileClose $3
        StrCpy $0 "Success"
        Goto ModifyJavaPolicy_lbl_end

ContStrSearch:
    ${EndWhile}

    ClearErrors
    FileOpen $2 $1 a  
    IfErrors 0 WriteJavaPolicyFile 
!ifdef SCRIPT_DEBUG
      MessageBox MB_OK "Could not open java.policy."
!endif
    Goto ModifyJavaPolicy_lbl_end

WriteJavaPolicyFile:
    FileSeek $2 0 END
    FileWrite $2 '$\r$\n// Begin Barco Modifications$\r$\n'  
    FileWrite $2 'grant {$\r$\n'
    FileWrite $2 \
      '$\tpermission java.io.FilePermission "\\IPVSBackup\\*", "read,write";$\r$\n'
    FileWrite $2 \
      '$\tpermission java.util.PropertyPermission "user.dir", "read, write";$\r$\n'
    FileWrite $2 '};$\r$\n'
    FileWrite $2 '// End Barco Modifications$\r$\n'
    FileClose $2
    
    StrCpy $0 "Success"

ModifyJavaPolicy_lbl_end:
    Pop $5
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Exch $0

FunctionEnd

!macro RESETUPDATEFLAG PREFIX
    Delete "$TEMP\${PREFIX}-update.txt"
!macroend

!macro SETUPDATEFLAG PREFIX
    
    Push $0

    FileOpen $0 "$TEMP\${PREFIX}-update.txt" w
    FileClose $0 
    !ifdef SCRIPT_DEBUG
       MessageBox MB_OK "SETUPDATEFLAG TO :$TEMP\${PREFIX}-update.txt"
    !endif
    Pop $0
!macroend

!macro CHECKUPDATEFLAG PREFIX STATUS

    StrCpy ${STATUS} "0"
    IfFileExists "$TEMP\${PREFIX}-update.txt" 0 check_updateflg_end
    StrCpy ${STATUS} "1"

check_updateflg_end:
!macroend

!macro UNSELECTCOMPONENT COMPONENT_INDEX
    
    Push $1
    Push $2

    SectionGetFlags ${COMPONENT_INDEX} $1
    IntOp $2 ${SF_SELECTED} ~
    IntOp $1 $1 & $2
    SectionSetFlags ${COMPONENT_INDEX} $1

    Pop $2
    Pop $1

!macroend

!macro SELECTCOMPONENT COMPONENT_INDEX
    
    Push $1

    SectionGetFlags ${COMPONENT_INDEX} $1
    IntOp $1 $1 | ${SF_SELECTED}
    !ifdef SCRIPT_DEBUG
      ; show SF_SELECTED
      MessageBox MB_OK "SF_SELECTED = ${SF_SELECTED}"
    !endif
    SectionSetFlags ${COMPONENT_INDEX} $1

    Pop $1

!macroend

!macro IFDIREXISTS DIRNAME STATUS
    
    IfFileExists ${DIRNAME} 0 +3
    StrCpy ${STATUS} "1"
    Goto +2
    StrCpy ${STATUS} "0"

!macroend

!macro SELECTCOMPONENTIFEXISTS COMPONENT_INDEX DIRNAME
    
    Push $1
    !insertmacro IFDIREXISTS ${DIRNAME} $1
    ${If} $1 == "1"
        !insertmacro SELECTCOMPONENT ${COMPONENT_INDEX}
    ${Else}
        !insertmacro UNSELECTCOMPONENT ${COMPONENT_INDEX}
    ${EndIf} 
    Pop $1

!macroend

!macro HIDEBACKBUTTON
    Push $1

    GetDlgItem $1 $HWNDPARENT 3 ; Back button ID
    ShowWindow $1 ${SW_HIDE}
    
    Pop $1
!macroend

!macro DELETEINSTDIR PARENTDIR
    Push $R0
    Push $R1

    ${GetParent} $INSTDIR $R0
    ${GetBaseName} $R0 $R1
    RMDir "$INSTDIR"
    ${If} $R1 == "${PARENTDIR}"
        RMDir $R0
!ifdef SCRIPT_DEBUG
        MessageBox MB_OK "Removed parent installation dir:$\r$\n $R0"
!endif
    ${EndIf}

    Pop $R1
    Pop $R0
!macroend

!macro DELETECLIENT_REGKEYS
    ClearErrors
    DeleteRegKey HKLM "Software\IpVideoSystems"
    DeleteRegKey HKCU "IpVideoSystems"
    ClearErrors
!macroend

!macro GETPRODUCTVERSION FILE VERSION

    Push $3
    Push $4
    ClearErrors

    StrCpy ${VERSION} "0"

	${xml::LoadFile} "${FILE}" $3
!ifdef SCRIPT_DEBUG
	MessageBox MB_OK "xml::LoadFile$\n$$3=$3"
!endif

    ${If} $3 != 0
        SetErrors 
        Goto getproductversion_lbl_end
    ${EndIf}

	${xml::RootElement} $3 $4
!ifdef SCRIPT_DEBUG
	MessageBox MB_OK "xml::RootElement$\n$$3=$3$\n$$4=$4"
!endif

    ${If} $4 != 0
        SetErrors 
        Goto getproductversion_lbl_end
    ${EndIf}

	${xml::FirstChildElement} "version" $3 $4
!ifdef SCRIPT_DEBUG
	MessageBox MB_OK "xml::FirstChildElement$\n$$3=$3$\n$$4=$4"
!endif

    ${If} $4 != 0
        SetErrors 
        Goto getproductversion_lbl_end
    ${EndIf}

	${xml::GetText} $3 $4
!ifdef SCRIPT_DEBUG
	MessageBox MB_OK "xml::GetText$\n$$3=$3$\n$$4=$4"
!endif

    ${If} $4 != 0
        SetErrors 
        Goto getproductversion_lbl_end
    ${EndIf}

    StrCpy ${VERSION} $3
    
getproductversion_lbl_end:
	${xml::Unload}

    Pop $4
    Pop $3
!macroend

!macro COMPAREVERSION VERSION1 VERSION2 STATUS

    ; Compare product version1 and version2, returns the status in STATUS
    ; 0: ver1 == ver2, 1: ver1 > ver2, 2: ver1 < ver2
    Push $3
    Push $4

    ${WordReplace} ${VERSION1} "r" "" "+" $3
    ${WordReplace} $3 "b" "" "+" $3
    
    ${WordReplace} ${VERSION2} "r" "" "+" $4
    ${WordReplace} $4 "b" "" "+" $4

    ${VersionCompare} $3 $4 ${STATUS}
    ${If} $0 == 0
          MessageBox MB_OK "Version comparison: $3 == $4"
    ${ElseIf} $0 == 1
          MessageBox MB_OK "Version comparison: $3 > $4"
    ${ElseIf} $0 == 2
          MessageBox MB_OK "Version $3 < $4"
    ${EndIf}
    
    Pop $4
    Pop $3
!macroend

!macro MAKEDIR DIRNAME
    Push $3
    ClearErrors

    IfFileExists "${DIRNAME}" 0 +3
    StrCpy $3 "1"
    Goto +2
    StrCpy $3 "0"

    ${If} $3 == "0"
        CreateDirectory "${DIRNAME}"
    ${EndIf}

    Pop $3
!macroend

!endif #___IPVSCOMMONFUNC__NSH___    
