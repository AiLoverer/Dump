!include nsDialogs.nsh
!include LogicLib.nsh

Var DlgItem
Var hwndDialog
Var hwndDlgItem
Var hwndEditAdminEmailAddress
Var hwndEditSMTPServer
Var hwndEditSMTPUser
Var hwndEditSMTPPassword
Var AdminEmailAddress
Var SMTPServer
Var SMTPUser
Var SMTPPassword
Var EmailParamsExist

Function SetEmailPage
  
  Call ShowEmailDialog                  	
  
FunctionEnd

Function ValidateEmailPage
  ${NSD_GetText} $hwndEditAdminEmailAddress $AdminEmailAddress
  ${NSD_GetText} $hwndEditSMTPServer $SMTPServer
  ${NSD_GetText} $hwndEditSMTPUser $SMTPUser
  ${NSD_GetText} $hwndEditSMTPPassword $SMTPPassword
FunctionEnd

#----------------------------------------
#  New-style offer page (created with nsDialogs)
#----------------------------------------
         
Function ShowEmailDialog

  !insertmacro MUI_HEADER_TEXT "CrashFix Configuration" "Specify admin E-mail address"

  nsDialogs::Create 1018
  Pop $hwndDialog
  StrCmp $hwndDialog "error" +1 +2
  Abort
  
  ${NSD_CreateLabel} 0u 0u 100% 12u "Specify the E-mail address of system administrator. CrashFix service may send an alert E-mail"
  Pop $hwndDlgItem
  
  ${NSD_CreateLabel} 0u 10u 100% 12u "to the provided address on undesired event."
  Pop $hwndDlgItem
  
  ${NSD_CreateLabel} 0u 25u 60u 12u "E-mail address:"
  Pop $hwndDlgItem
  
  ${NSD_CreateText} 65u 25u 150u 12u "admin@localhost"
  Pop $hwndEditAdminEmailAddress
  
  nsis_plugin::read_config_string "$INSTDIR\conf\crashfixd.conf" "WEBMASTER_EMAIL" 
  Pop $0  
  ${If} $0 != ""	  	  
	${NSD_SetText} $hwndEditAdminEmailAddress $0
	StrCpy $EmailParamsExist 1
  ${EndIf}
    
  ${NSD_CreateLabel} 0u 45u 100% 12u "Please specify SMTP server parameters below:"
  Pop $hwndDlgItem
	
  ${NSD_CreateLabel} 0u 61u 60u 12u "Host and port:"
  Pop $hwndDlgItem
  
  ${NSD_CreateText} 65u 60u 150u 12u "127.0.0.1:25"
  Pop $hwndEditSMTPServer
  
  nsis_plugin::read_config_string "$INSTDIR\conf\crashfixd.conf" "SMTP_SERVER" 
  Pop $0  
  ${If} $0 != ""	  	  
	${NSD_SetText} $hwndEditSMTPServer $0
  ${EndIf}
  
  ${NSD_CreateLabel} 0u 75u 60u 12u "Username:"
  Pop $hwndDlgItem
  
  ${NSD_CreateText} 65u 75u 150u 12u ""
  Pop $hwndEditSMTPUser
  
  nsis_plugin::read_config_string "$INSTDIR\conf\crashfixd.conf" "SMTP_LOGIN" 
  Pop $0  
  ${If} $0 != ""	  	  
	${NSD_SetText} $hwndEditSMTPUser $0
  ${EndIf}
  
  ${NSD_CreateLabel} 0u 90u 60u 12u "Password:"
  Pop $hwndDlgItem
  
  ${NSD_CreateText} 65u 90u 150u 12u ""
  Pop $hwndEditSMTPPassword
  
  nsis_plugin::read_config_string "$INSTDIR\conf\crashfixd.conf" "SMTP_PASSWORD" 
  Pop $0  
  ${If} $0 != ""	  	  
	${NSD_SetText} $hwndEditSMTPPassword $0
  ${EndIf}
	
  ${If} $EmailParamsExist != 1
	
	  ${NSD_CreateLabel} 0u 110u 100% 12u "Note: You may skip this step now, but later you will have to specify this"
	  Pop $hwndDlgItem
	  
	  ${NSD_CreateLabel} 0u 120u 100% 12u "parameter in the configuration file manually."
	  Pop $hwndDlgItem
 
  ${EndIf}
	  
  nsDialogs::Show
  
FunctionEnd




