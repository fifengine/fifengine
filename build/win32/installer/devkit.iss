; TODO:
;   * Run oalinst.exe if openal DLL is not installed?
;   * Test that the installer works as non-admin on xp, vista and win7

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{FE01A2E9-77CE-4293-B667-2FA8A184D4E7}
AppName=FIFE Development Kit (November 2010)
; Version is yyyy.mm
AppVerName=FIFE DevKit 2010.11
AppPublisher=FIFE Team
AppPublisherURL=http://www.fifengine.net
AppSupportURL=http://www.fifengine.net
AppUpdatesURL=http://www.fifengine.net
DefaultDirName={sd}\FIFE\build\win32\
DefaultGroupName=FIFE\Devkit
DirExistsWarning=No
EnableDirDoesntExistWarning=Yes
AllowNoIcons=yes
LicenseFile=license.rtf
Compression=lzma
SolidCompression=yes
OutputBaseFilename=FIFE_Development_Kit_Nov2010
OutputDir=.
DisableReadyPage=yes
; This will require some testing. We can't install to Program files if the user is not an admin.
; PrivilegesRequired=none

[Types]
Name: "full"; Description: "Full installation"
Name: "compact"; Description: "Compact installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "common"; Description: "Common files"; Types: full compact custom; Flags: fixed
Name: "compilers"; Description: "Compiler specific files"; Types: full custom
Name: "compilers\mingw"; Description: "MinGW"; Types: full compact custom
Name: "compilers\msvc2005"; Description: "Visual Studio 2005"; Types: full custom
Name: "compilers\msvc2008"; Description: "Visual Studio 2008"; Types: full custom

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; Common
Source: "..\applications\scons\*"; DestDir: "{app}\applications\scons"; Excludes: ".svn"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\applications\swigwin-2.0.1\*"; DestDir: "{app}\applications\swigwin-2.0.1"; Excludes: ".svn"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\includes\*"; DestDir: "{app}\includes"; Excludes: ".svn"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "README_DevKit_Nov2010.txt"; DestDir:"{app}"; Flags: isreadme

; MinGW
Source: "..\applications\mingw\*"; DestDir: "{app}\applications\mingw"; Excludes: ".svn"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\binaries\mingw\*"; DestDir: "{app}\binaries\mingw"; Excludes: ".svn"; Components: compilers\mingw; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\static_libs\mingw\*"; DestDir: "{app}\static_libs\mingw"; Excludes: ".svn"; Components: compilers\mingw; Flags: ignoreversion recursesubdirs createallsubdirs

; MSVC8
Source: "..\binaries\msvc2005\*"; DestDir: "{app}\binaries\msvc2005"; Excludes: ".svn"; Components: compilers\msvc2005; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\static_libs\msvc2005\*"; DestDir: "{app}\static_libs\msvc2005"; Excludes: ".svn"; Components: compilers\msvc2005; Flags: ignoreversion recursesubdirs createallsubdirs

; MSVC9
Source: "..\binaries\msvc2008\*"; DestDir: "{app}\binaries\msvc2008"; Excludes: ".svn"; Components: compilers\msvc2008; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\static_libs\msvc2008\*"; DestDir: "{app}\static_libs\msvc2008"; Excludes: ".svn"; Components: compilers\msvc2008; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{cm:ProgramOnTheWeb,FIFE}"; Filename: "http://www.fifengine.net/"
Name: "{group}\{cm:UninstallProgram,FIFE}"; Filename: "{uninstallexe}"


