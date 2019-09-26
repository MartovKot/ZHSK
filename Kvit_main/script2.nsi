; Script generated by the HM NIS Edit Script Wizard.
!include 'LogicLib.nsh'

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "ZHSK"
!define PRODUCT_VERSION "1.7.10.1"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\kvit.exe"
!define QT_INSTALL "D:\Qt\Qt5.5.1\5.5\mingw492_32"
;!define QT_INSTALL "E:\Qt\Qt5.4.0\5.4\mingw491_32"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
;!define MUI_FINISHPAGE_RUN "$INSTDIR\kvit.exe"
!insertmacro MUI_PAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "Russian"

; MUI end ------

!macro BackupDir FILE_DIR BACKUP_TO
 IfFileExists "${BACKUP_TO}" +2
  CreateDirectory "${BACKUP_TO}"
 IfFileExists "${FILE_DIR}" 0 +2
  CopyFiles "${FILE_DIR}\*.*" "${BACKUP_TO}"
;MessageBox MB_OK "backup"
!macroend

!macro RestoreDir BUP_DIR RESTORE_TO
 IfFileExists "${RESTORE_TO}" +2
  CreateDirectory "${RESTORE_TO}"
 IfFileExists "${BUP_DIR}" 0 +2
  CopyFiles "${BUP_DIR}\*.*" "${RESTORE_TO}"
  RMDir /r ${BUP_DIR}
  
  
  
!macroend

!macro BackupFile FILE_DIR FILE BACKUP_TO
 IfFileExists "${BACKUP_TO}\*.*" +2
  CreateDirectory "${BACKUP_TO}"
 IfFileExists "${FILE_DIR}\${FILE}" 0 +2
  Rename "${FILE_DIR}\${FILE}" "${BACKUP_TO}\${FILE}"
!macroend

!macro RestoreFile BUP_DIR FILE RESTORE_TO
 IfFileExists "${BUP_DIR}\${FILE}" 0 +2
  Rename "${BUP_DIR}\${FILE}" "${RESTORE_TO}\${FILE}"
!macroend

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "setup.exe"
InstallDir "$EXEDIR"
;InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show


Section "" ;No components page, name is not important

IfFileExists $INSTDIR\kvit.exe 0 +6
  !insertmacro BackupFile $INSTDIR "kvitdb.qsl" $TEMP
  !insertmacro BackupDir "$INSTDIR\kvit" "$TEMP\kvit"
  RMDir /r $INSTDIR
  CreateDirectory $INSTDIR
  !insertmacro RestoreFile $TEMP "kvitdb.qsl" $INSTDIR
  !insertmacro RestoreDir "$TEMP\kvit" "$INSTDIR\kvit"
  ;MessageBox MB_OK "�������"
SectionEnd ; end the section

Section "zhsk" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
;  File ".\release\kvit.exe"
;File "D:\Qt-project\build-Kvit_main-Desktop_Qt_5_2_1_MinGW_32bit-Release\release\kvit.exe"
  ;File "E:\QT-sourse\build-Kvit_main-Desktop_Qt_5_3_MinGW_32bit-Release\release\kvit.exe"
;  File "E:\QT-sourse\build-ZHSK-Desktop_Qt_5_4_0_MinGW_32bit-Release\Kvit_main\release\kvit.exe"
  File "C:\zhsk\build-ZHSK-Desktop_Qt_5_5_1_MinGW_32bit-Release\Kvit_main\release\kvit.exe"
  
  CreateDirectory "$SMPROGRAMS\ZHSK"
  CreateShortCut "$SMPROGRAMS\ZHSK\ZHSK.lnk" "$INSTDIR\kvit.exe"
  CreateShortCut "$DESKTOP\ZHSK.lnk" "$INSTDIR\kvit.exe"
  SetOverwrite try
  SetOutPath "$INSTDIR\blank"
  File "blank\blank_main.html"
  File "blank\blank_s.html"
  File "blank\blank_u.html"
  SetOutPath "$INSTDIR\ico"
  File "ico\base_calendar_32.png"
  File "ico\cash_register_6356.ico"
  File "ico\forward.ico"
  File "ico\leftendblue.ico"
  File "ico\left_blue.ico"
  File "ico\left_end.ico"
  ;File "ico\preferences-desktop-cryptography_2990.ico"
  File "ico\previous.ico"
  File "ico\print.ico"
  File "ico\print2.ico"
  ;File "ico\print21.ico"
  File "ico\print3.ico"
  File "ico\rightendblue.ico"
  File "ico\right_blue.ico"
  File "ico\right_end.ico"
  
  SetOutPath "$INSTDIR\update_db"
  File "update_db\*.sql"

SectionEnd

Section "libs" SEC02
  SetOverwrite ifnewer
  SetOutPath "$INSTDIR"
  File "${QT_INSTALL}\bin\Qt5Core.dll"
  File "${QT_INSTALL}\bin\Qt5Gui.dll"
  File "${QT_INSTALL}\bin\Qt5Network.dll"
  File "${QT_INSTALL}\bin\Qt5PrintSupport.dll"
  File "${QT_INSTALL}\bin\Qt5Widgets.dll"
  File "${QT_INSTALL}\bin\Qt5Sql.dll"
  File "${QT_INSTALL}\bin\Qt5WebKitWidgets.dll"
  File "${QT_INSTALL}\bin\Qt5Multimedia.dll"
  File "${QT_INSTALL}\bin\Qt5MultimediaWidgets.dll"
  File "${QT_INSTALL}\bin\Qt5OpenGL.dll"
  File "${QT_INSTALL}\bin\Qt5Sensors.dll"
  File "${QT_INSTALL}\bin\Qt5WebKit.dll"
  File "${QT_INSTALL}\bin\Qt5Positioning.dll"
  File "${QT_INSTALL}\bin\Qt5Qml.dll"
  File "${QT_INSTALL}\bin\Qt5Quick.dll"
  File "${QT_INSTALL}\bin\Qt5WebChannel.dll"
  
  File "${QT_INSTALL}\bin\libgcc_s_dw2-1.dll"
  File "${QT_INSTALL}\bin\libwinpthread-1.dll"
  File "${QT_INSTALL}\bin\libstdc++-6.dll"
  File "${QT_INSTALL}\bin\icuin54.dll"
  File "${QT_INSTALL}\bin\icuuc54.dll"
  File "${QT_INSTALL}\bin\icudt54.dll"

  SetOutPath "$INSTDIR\bearer"
  
  File "${QT_INSTALL}\plugins\bearer\qgenericbearer.dll"
  File "${QT_INSTALL}\plugins\bearer\qnativewifibearer.dll"
  
  SetOutPath "$INSTDIR\platforms"
  
  File "${QT_INSTALL}\plugins\platforms\qwindows.dll"
  
  SetOutPath "$INSTDIR\sqldrivers"

  File "${QT_INSTALL}\plugins\sqldrivers\qsqlite.dll"
  
  SetOutPath "$INSTDIR\imageformats"
  
  File "${QT_INSTALL}\plugins\imageformats\qico.dll"
  
SectionEnd

;Section -Post
;  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\kvit.exe"
;SectionEnd
