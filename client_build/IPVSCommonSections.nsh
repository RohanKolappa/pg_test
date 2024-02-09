!ifndef ___IPVSCOMMONSECTION__NSH___
!define ___IPVSCOMMONSECTION__NSH___ 

!macro JRESECTION

  Call CheckJRE
  Pop $R0

  ${If} $R0 != ""
    Call CheckJREVersion
    Pop $R1
    ${VersionCompare} ${JRE_VERSION} $R1 $R1
    ${If} $R1 == "1"
      StrCpy $R0 "" ; enforce JRE install
    ${EndIf}
  ${EndIf}

  ${If} $R0 == ""
    MessageBox MB_OKCANCEL "Setup has detected that you do not have the \
        minimum version ${JRE_VERSION_FRIENDLY} of Java required to run \
        this application. Please close any programs running Java and select \
        OK to proceed with the latest Java Runtime Environment \
        installation." IDOK 0 IDCANCEL ModifyJavaPolicyLabel
    DetailPrint "Installing JRE ..."
    Call InstallJRE
    Pop $R0
    ${If} $R0 == "Failure"
      Abort "Java installation failed. Setup will exit now"
    ${EndIf}

    ; The silent installer install Java in Program Files
    StrCpy $R0 "$PROGRAMFILES\java\jre6\bin\java.exe"
  ${EndIf}

ModifyJavaPolicyLabel:
  
!macroend

!macro COMMONSECTION UPDATE DSKSHORTCUT SMSHORTCUT

  Push $0
  Push $1

  SetOutPath "$INSTDIR"
  SetOverwrite on

  SetOutPath "${GI_DSTDIR}"

  Delete "${GI_DSTDIR}\logger.xml"
  File "${GI_SRCDIR}\logger.xml"
  
  Delete "${GI_DSTDIR}\jsx3.gui.window.html"
  File "${GI_SRCDIR}\jsx3.gui.window.html"

  RMDir /r "${JSX_DSTDIR}"
  File /r /x .svn "${JSX_SRCDIR}"

  RMDir /r "${ADDINS_DSTDIR}"
  File /r /x .svn "${ADDINS_SRCDIR}"

  
  SetShellVarContext all
  ${If} ${UPDATE} == "0"

  ${EndIf}

  SetOutPath "${JSXAPPS_DSTDIR}"

  RMDir /r "${COMMON_DSTDIR}"

  File /r /x .svn "${COMMON_SRCDIR}"
  
  !insertmacro RESTOREFILE "${PREFS_DSTFILE}" "${IPVSDATADIR}"

  Pop $1
  Pop $0

!macroend

!macro INSTALLPLAYER SRCDIR DSTDIR DLLFILE DLLFILE2 UPDATE

  SetOutPath "${DSTDIR}"
  SetOverwrite on
  RMDir /r "${DSTDIR}"
  File /x .svn "${SRCDIR}\*"

  ${If} ${UPDATE} == "0"
!ifdef SCRIPT_DEBUG
    MessageBox MB_OK "Registering ${DLLFILE}"
!endif
    RegDLL "${DLLFILE}"
    RegDLL "${DLLFILE2}"
  ${EndIf}

!macroend

!macro UNINSTALLPLAYER DSTDIR DLLFILE DLLFILE2

  UnRegDLL "${DLLFILE}"
  UnRegDLL "${DLLFILE2}"

  RMDir /r /REBOOTOK "${DSTDIR}"

!macroend

!endif #___IPVSCOMMONSECTION__NSH___
