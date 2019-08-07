!include nsDialogs.nsh
!include LogicLib.nsh

#Var DlgItem
#Var hwndDialog
#Var hwndDlgItem
#Var hwndEdit
Var DeleteConfigFiles
Var DeleteLogFiles
Var hwndDeleteConfigFiles
Var hwndDeleteLogFiles

Function un.SetDeleteConfigPage
  
  ${NSD_SetState} $hwndDeleteConfigFiles $DeleteConfigFiles
  ${NSD_SetState} $hwndDeleteLogFiles $DeleteLogFiles
  
  Call un.ShowDeleteConfigDialog                  	
  
FunctionEnd

Function un.ValidateDeleteConfigPage

  ${NSD_GetState} $hwndDeleteConfigFiles $DeleteConfigFiles
  ${NSD_GetState} $hwndDeleteLogFiles $DeleteLogFiles
  
FunctionEnd
         
Function un.ShowDeleteConfigDialog

  !insertmacro MUI_HEADER_TEXT "Uninstall Options" "Specify file delete options"

  nsDialogs::Create 1018
  Pop $hwndDialog
  StrCmp $hwndDialog "error" +1 +2
  Abort
  
  ${NSD_CreateCheckBox} 0u 0u 100% 12u "Delete configuration file"
  Pop $hwndDeleteConfigFiles
  
  ${NSD_CreateCheckBox} 0u 20u 100% 12u "Delete log file(s)"
  Pop $hwndDeleteLogFiles
      
  nsDialogs::Show
  
FunctionEnd

