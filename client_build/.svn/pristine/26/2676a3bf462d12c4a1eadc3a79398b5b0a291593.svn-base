!ifndef ___IPVSGUIFUNC__NSH___
!define ___IPVSGUIFUNC__NSH___ 

!include "MUI2.nsh"
!include "IPVSCommonFunc.nsh"

Function OptionsDialogsPage

  Var /GLOBAL CBDesktopShortcut
  Var /GLOBAL CBDesktop_State
  Var /GLOBAL CBStartMenuShortcut
  Var /GLOBAL CBStartMenu_State

  !insertmacro MUI_HEADER_TEXT "Select Options" \
  "Please select the shortcuts you want for quick access to ${PRODUCT_NAME}"

  nsDialogs::Create 1018
  Pop $0

  ${If} $0 == error
    Abort
  ${EndIf}

  ${NSD_CreateCheckbox} 0 15u 100% 10u "&Create Desktop Shortcut"
  Pop $CBDesktopShortcut

  ${If} $CBDesktop_State == ${BST_CHECKED}
    ${NSD_Check} $CBDesktopShortcut
  ${EndIf}

  ${NSD_CreateCheckbox} 0 30u 100% 10u "&Create entry in Start Menu"
  Pop $CBStartMenuShortcut

  ${If} $CBStartMenu_State == ${BST_CHECKED}
    ${NSD_Check} $CBStartMenuShortcut
  ${EndIf}

  ${NSD_OnClick} $CBDesktopShortcut OptionsDialogsOnChange
  ${NSD_OnClick} $CBStartMenuShortcut OptionsDialogsOnChange

  nsDialogs::Show

FunctionEnd

Function OptionsDialogsPageLeave

  ${NSD_GetState} $CBDesktopShortcut $CBDesktop_State
  ${NSD_GetState} $CBStartMenuShortcut $CBStartMenu_State

FunctionEnd

Function OptionsDialogsOnChange

  Pop $1
  ${NSD_GetState} $CBDesktopShortcut $CBDesktop_State
  ${NSD_GetState} $CBStartMenuShortcut $CBStartMenu_State

FunctionEnd

!endif #___IPVSGUIFUNC__NSH___  
