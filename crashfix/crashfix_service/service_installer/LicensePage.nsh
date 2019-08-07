!include nsDialogs.nsh
!include LogicLib.nsh

#Var DlgItem
#Var hwndDialog
#Var hwndDlgItem
#Var hwndEdit
Var LicenseFilePath
Var LicenseFileExists

Function SetLicensePage
  
  Call ShowLicenseDialog                  	
  
FunctionEnd

Function ValidateLicensePage
  ${NSD_GetText} $hwndEdit $LicenseFilePath  
FunctionEnd
         
Function ShowLicenseDialog

  !insertmacro MUI_HEADER_TEXT "CrashFix Configuration" "Specify the license file"

  nsDialogs::Create 1018
  Pop $hwndDialog
  StrCmp $hwndDialog "error" +1 +2
  Abort
  
  ${NSD_CreateLabel} 0u 0u 100% 12u "Specify the path to the CrashFix license file:"
  Pop $hwndDlgItem
  
  ${NSD_CreateText} 0u 12u 240u 12u ""
  Pop $hwndEdit

  IfFileExists "$INSTDIR\licenses\crashfix.lic" 0 +3
	${NSD_SetText} $hwndEdit "$INSTDIR\licenses\crashfix.lic"  
	StrCpy $LicenseFileExists 1
    
  ${NSD_CreateButton} 250u 12u 50u 12u "Browse..."
  Pop $hwndDlgItem
  
  ${NSD_OnClick} $hwndDlgItem OnLicenseBrowseClick
  
  ${NSD_CreateLabel} 0u 30u 100% 12u "If you do not have a license file yet, you may request an evaluation license:"
  Pop $hwndDlgItem
  
  ${NSD_CreateButton} 10u 45u 130u 15u "Request Evaluation License"
  Pop $hwndDlgItem
  
  ${NSD_OnClick} $hwndDlgItem OnRequestEvalLicenseClick
    
  ${NSD_CreateButton} 160u 45u 130u 15u "Generate Hardware Info File"
  Pop $hwndDlgItem
  
  ${NSD_OnClick} $hwndDlgItem OnGenerateHwinfoClick

  ${If} $LicenseFileExists != 1  
  
	  ${NSD_CreateLabel} 0u 80u 100% 12u "Note: You may skip this step now, but later you will have to specify this"
	  Pop $hwndDlgItem
	  
	  ${NSD_CreateLabel} 0u 90u 100% 12u "parameter in the configuration file manually."
	  Pop $hwndDlgItem
  
  ${EndIf}
	  
  nsDialogs::Show
  
FunctionEnd

Function OnLicenseBrowseClick

  Pop $0 
  
  Push $hwndDialog
  nsis_plugin::open_file_dialog
  Pop $0
  ${If} $0 != ""
	${NSD_SetText} $hwndEdit $0
  ${EndIf}  
  
  
FunctionEnd

Function OnRequestEvalLicenseClick

  Pop $0 
  ExecShell "open" "http://crashfixsoftware.com/index.php/download/evaluation"
 
FunctionEnd

Function OnGenerateHwinfoClick

  Pop $0 
  ExecShell "open" "$TEMP\crashfix"
 
FunctionEnd
