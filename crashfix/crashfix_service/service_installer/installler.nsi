# CrashFix Service Installer Script
# To compile this script you need NSIS (NullSoft Installation System) to be previously installed

!define PRODUCT_NAME "CrashFix"
!define PRODUCT_VERSION "1.0.4"
!define PRODUCT_PUBLISHER "IP Oleg Krivtsov"
!define PRODUCT_WEB_SITE "http://crashfix.sourceforge.net"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_GLOBAL_REGKEY ""

!define UninstLog "uninstall.log"
!include "UninstallLog.nsh"
!include "MUI.nsh"
!include "WordFunc.nsh"

!addplugindir NSISPlugins ; Directory for custom NSIS plugins

SetCompressor lzma
CRCCheck on

Name "${PRODUCT_NAME}"; ${PRODUCT_VERSION}"
OutFile "crashfix-service-${PRODUCT_VERSION}-Win32.exe"

!system "echo crashfix-service-${PRODUCT_VERSION}-Win32.exe > InstallerName.txt"

ReserveFile "${NSISDIR}\Plugins\InstallOptions.dll"

InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""

;--------------------------------
;Interface Settings

Caption "${PRODUCT_NAME} ${PRODUCT_VERSION} Install"
UninstallCaption "${PRODUCT_NAME} ${PRODUCT_VERSION} Uninstall"

!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_NODESC
!define MUI_FINISHPAGE_SHOWREADME "http://crashfix.sourceforge.net/doc/faq.html"     
!define MUI_FINISHPAGE_SHOWREADME_TEXT "View FAQ"     
#!define MUI_FINISHPAGE_LINK "" 
#!define MUI_FINISHPAGE_LINK_LOCATION ""
#!define MUI_FINISHPAGE_RUN "$INSTDIR\crashfixd.exe"

;--------------------------------
;Pages

!include EmailPage.nsh
!include DeleteConfigPage.nsh

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\COPYRIGHT.TXT"
!insertmacro MUI_PAGE_DIRECTORY
Page custom SetEmailPage ValidateEmailPage
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
UninstPage custom un.SetDeleteConfigPage un.ValidateDeleteConfigPage
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH


;--------------------------------
;Languages
; First is default 
!insertmacro MUI_LANGUAGE "English"

; License data
#LicenseLangString myLicenseData ${LANG_ENGLISH} "Locale\en\eula_en.rtf"

;--------------------------------
;Custom pages


# Verifies that user has administrative priviledges
!macro VerifyUser UN
Function ${UN}VerifyUser
  Call ${UN}IsUserAdmin
  Pop $R0
  StrCmp $R0 "true" admin

  MessageBox MB_OK "You need to be an administrator to run this program." IDNO admin
    Abort
  admin:
FunctionEnd
!macroend

!insertmacro VerifyUser ""
!insertmacro VerifyUser "un."


!macro IsUserAdmin UN
Function ${UN}IsUserAdmin
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
!macroend

!insertmacro VersionCompare

!insertmacro IsUserAdmin ""
!insertmacro IsUserAdmin "un."

!define LVM_GETITEMCOUNT 0x1004
!define LVM_GETITEMTEXT 0x102D
 
Function DumpLog
  Exch $5
  Push $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $6
 
  FindWindow $0 "#32770" "" $HWNDPARENT
  GetDlgItem $0 $0 1016
  StrCmp $0 0 exit
  FileOpen $5 $5 "w"
  StrCmp $5 "" exit
    SendMessage $0 ${LVM_GETITEMCOUNT} 0 0 $6
    System::Alloc ${NSIS_MAX_STRLEN}
    Pop $3
    StrCpy $2 0
    System::Call "*(i, i, i, i, i, i, i, i, i) i \
      (0, 0, 0, 0, 0, r3, ${NSIS_MAX_STRLEN}) .r1"
    loop: StrCmp $2 $6 done
      System::Call "User32::SendMessageA(i, i, i, i) i \
        ($0, ${LVM_GETITEMTEXT}, $2, r1)"
      System::Call "*$3(&t${NSIS_MAX_STRLEN} .r4)"
      FileWrite $5 "$4$\r$\n"
      IntOp $2 $2 + 1
      Goto loop
    done:
      FileClose $5
      System::Free $1
      System::Free $3
  exit:
    Pop $6
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
    Exch $5
FunctionEnd
 
 Function WriteINIStrNS
 Exch $R0 ; new value
 Exch
 Exch $R1 ; key
 Exch 2
 Exch $R2 ; ini file
 Exch 2
 Push $R3
 Push $R4
 Push $R5
 Push $R6
 Push $R7
 Push $R8
 Push $R9
 
  StrCpy $R9 0
 
  FileOpen $R3 $R2 r
  GetTempFileName $R4
  FileOpen $R5 $R4 w
 
  LoopRead:
   ClearErrors
   FileRead $R3 $R6
   IfErrors End
 
   StrCpy $R7 -1
   LoopGetVal:
    IntOp $R7 $R7 + 1
    StrCpy $R8 $R6 1 $R7
    StrCmp $R8 "" LoopRead
    StrCmp $R8 = 0 LoopGetVal
 
     StrCpy $R8 $R6 $R7
     StrCmp $R8 $R1 0 +4
 
      FileWrite $R5 "$R1=$R0$\r$\n"
      StrCpy $R9 1
      Goto LoopRead
 
    FileWrite $R5 $R6
    Goto LoopRead
 
  End:
   StrCmp $R9 1 +2
   FileWrite $R5 "$R1=$R0$\r$\n"
 
  FileClose $R5
  FileClose $R3
 
  SetDetailsPrint none
  Delete $R2
  Rename $R4 $R2
  SetDetailsPrint both
 
 Pop $R9
 Pop $R8
 
 Pop $R7
 Pop $R6
 Pop $R5
 Pop $R4
 Pop $R3
 Pop $R2
 Pop $R1
 Pop $R0
FunctionEnd
 
!define WriteINIStrNS "!insertmacro WriteINIStrNS"
!macro WriteINIStrNS Var File Key Value
 Push "${File}"
 Push "${Key}"
 Push "${Value}"
  Call WriteINIStrNS
 Pop "${Var}"
!macroend
 
 !insertmacro UninstallLog ""
 !insertmacro UninstallLog "un."
 
  Section -openlogfile
    CreateDirectory "$INSTDIR"
    IfFileExists "$INSTDIR\${UninstLog}" +3
      FileOpen $UninstLog "$INSTDIR\${UninstLog}" w
    Goto +4
      SetFileAttributes "$INSTDIR\${UninstLog}" HIDDEN
      FileOpen $UninstLog "$INSTDIR\${UninstLog}" a
      FileSeek $UninstLog 0 END
  SectionEnd

;--------------------------------
;Sections

Section "CrashFix Service"

  SectionIn RO
  SetShellVarContext all
  
  # --- Uninstall previous version (if exists)
  
  ReadRegStr $0 HKLM Software\CrashFix "InstallDir"
  ${If} $0 != ""
  
	DetailPrint "Previous CrashFix installation detected in directory: $0"
	
	DetailPrint "Removing previous version of CrashFix service"
	
	ExecDos::exec /TOSTACK "$0\bin\crashfixd.exe --remove"
	Pop $0 # return value
	${If} $0 != "0"
	Pop $1 # Get error message
	  DetailPrint $1  
	  MessageBox MB_OK "Error removing previous version of CrashFix service: $1"
	${EndIf}
	
	# Wait for a second to ensure crashfixd.exe has been unlocked
	Sleep 1000
	
	# --- Remove files from Uninstall.log ---
  
	Push $0
	Call UninstallLog 
	
  ${Endif}  
  
  # --- Copy files ---
  
  DetailPrint "Copying new files"  

  SetOverwrite on
  ${SetOutPath} $INSTDIR
  ${File} "..\" "COPYRIGHT.TXT"
  
  ${SetOutPath} $INSTDIR\bin
  ${File} "..\bin\" "crashfixd.exe"
  #${File} "..\bin\" "libeay32.dll"
  ${File} "..\bin\" "msvcp100.dll"
  ${File} "..\bin\" "msvcr100.dll"  
  ${File} "..\bin\" "uploader.exe"
  ${File} "..\bin\" "uploader_gui.exe"
      
  ${SetOutPath} $INSTDIR\conf  
  File /oname=crashfixd.conf "..\conf\crashfixd.conf.win32"   
  
  nsis_plugin::write_config_string "$INSTDIR\conf\crashfixd.conf" "WEB_ROOT_DIR" "$WebAppInstallDir"
  nsis_plugin::write_config_string "$INSTDIR\conf\crashfixd.conf" "WEBMASTER_EMAIL" "$AdminEmailAddress"
  nsis_plugin::write_config_string "$INSTDIR\conf\crashfixd.conf" "SMTP_SERVER" "$SMTPServer"
  nsis_plugin::write_config_string "$INSTDIR\conf\crashfixd.conf" "SMTP_LOGIN" "$SMTPUser"
  nsis_plugin::write_config_string "$INSTDIR\conf\crashfixd.conf" "SMTP_PASSWORD" "$SMTPPassword"  
  
  ${If} $LicenseFilePath != ""
	CopyFiles /SILENT /FILESONLY $LicenseFilePath $INSTDIR\licenses\crashfix.lic
  ${Endif}
  
  DetailPrint "Creating directories"  
  
  ${CreateDirectory} "$INSTDIR\licenses"
  ${CreateDirectory} "$INSTDIR\logs"
  
  # --- Write start menu shortcuts
  
  DetailPrint "Writing start menu shortcuts"  
  
  ${CreateDirectory} "$SMPROGRAMS\${PRODUCT_NAME}"
  ${CreateDirectory} "$SMPROGRAMS\${PRODUCT_NAME}\Configure CrashFix Service"
  ${CreateDirectory} "$SMPROGRAMS\${PRODUCT_NAME}\Control CrashFix Service"
  ${CreateDirectory} "$SMPROGRAMS\${PRODUCT_NAME}\Tools"
  ${CreateShortCut} "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall CrashFix.lnk" "$INSTDIR\Uninstall.exe" "" "" ""
  ${CreateShortCut} "$SMPROGRAMS\${PRODUCT_NAME}\Online Documentation.lnk" "http://crashfix.sourceforge.net/doc/index.html" "" "" ""
  ${CreateShortCut} "$SMPROGRAMS\${PRODUCT_NAME}\Configure CrashFix Service\Edit the crashfixd.conf Configuration File.lnk" "$INSTDIR\conf\crashfixd.conf" "" "" ""
  ${CreateShortCut} "$SMPROGRAMS\${PRODUCT_NAME}\Control CrashFix Service\Start.lnk" "$INSTDIR\bin\crashfixd.exe" "--start" "" ""
  ${CreateShortCut} "$SMPROGRAMS\${PRODUCT_NAME}\Control CrashFix Service\Stop.lnk" "$INSTDIR\bin\crashfixd.exe" "--stop" "" ""
  ${CreateShortCut} "$SMPROGRAMS\${PRODUCT_NAME}\Control CrashFix Service\Restart.lnk" "$INSTDIR\bin\crashfixd.exe" "--restart" "" ""
  ${CreateShortCut} "$SMPROGRAMS\${PRODUCT_NAME}\Tools\Debug Info Uploader.lnk" "$INSTDIR\bin\uploader_gui.exe" "" "" ""
    
  # --- Write registry keys ---
  
  DetailPrint "Writing registry keys"  
  
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "InstallLocation" "$INSTDIR"
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "HelpLink" "http://crashfixsoftware.com/support"
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "URLInfoAbout" "http://crashfixsoftware.com"
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "Comments" "CrashFix service processes crash report files"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoRepair" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "EstimatedSize" 2530

  WriteRegStr HKLM "SOFTWARE\${PRODUCT_NAME}" "StartMenuFolder" "${PRODUCT_NAME}"
  WriteRegStr HKLM "SOFTWARE\${PRODUCT_NAME}" "InstallDir" "$INSTDIR"
  WriteRegStr HKLM "SOFTWARE\${PRODUCT_NAME}" "Version" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "SOFTWARE\${PRODUCT_NAME}" "InstallLanguage" $LANGUAGE
    
  # --- Write uninstaller ---
  
  DetailPrint "Creating uninstaller"  
  
  ${WriteUninstaller} "$INSTDIR\Uninstall.exe"
  
  # --- Install service ---
  
  DetailPrint "Installing CrashFix service"
  
  ExecDos::exec /TOSTACK "$INSTDIR\bin\crashfixd.exe --install"
  Pop $0 # return value
  ${If} $0 != "0"
	Pop $1 # Get error message
	DetailPrint $1  
	MessageBox MB_OK "Error installing CrashFix service: $1"
  ${EndIf}
  
  # --- Start service ---
  
  DetailPrint "Starting CrashFix service"
  
  ExecDos::exec /TOSTACK "$INSTDIR\bin\crashfixd.exe --start"
  Pop $0 # return value
  ${If} $0 != "0"
	Pop $1 # Get error message
	DetailPrint $1  
	MessageBox MB_OK "Could not start CrashFix service. You will have to start the service manually.$\r$\n$\r$\n$1"
	ExecShell "open" "$INSTDIR\logs\monitor.log"
  ${EndIf}
  
  # --- Finalize ---
 
  DetailPrint "Finalizing installation"

  # --- Dump the log window to file 
  Push "$INSTDIR\install.log"  
  Call DumpLog
  ${AddItem} "$INSTDIR\install.log"
  
  DetailPrint "Installation completed"
  
SectionEnd

;--------------------------------
; Uninstaller
;--------------------------------
Section Uninstall

  # --- Removing service ---
  
  DetailPrint "Removing CrashFix service"
	
  ExecDos::exec /TOSTACK "$INSTDIR\bin\crashfixd.exe --remove"
  Pop $0 # return value
  ${If} $0 != "0"
	Pop $1 # Get error message
	DetailPrint $1  
	MessageBox MB_OK "Error removing CrashFix service: $1"
  ${EndIf}

  # --- Remove registry keys ---
  
  DetailPrint "Deleting registry keys"
  
  DeleteRegKey HKLM "SOFTWARE\${PRODUCT_NAME}"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  
  # --- Remove config and logs (optional)
  
  ${If} $DeleteConfigFiles == ${BST_CHECKED}  
	RMDir /r /REBOOTOK "$INSTDIR\conf"
  ${EndIf}
  
  ${If} $DeleteLogFiles == ${BST_CHECKED}  
	RMDir /r /REBOOTOK "$INSTDIR\logs"
  ${EndIf}
  
  # --- Remove files from Uninstall.log ---
  
  Push $INSTDIR
  Call un.UninstallLog   
 
SectionEnd

Function .onInit
  InitPluginsDir
FunctionEnd

Function un.onInit
   InitPluginsDir   
FunctionEnd


