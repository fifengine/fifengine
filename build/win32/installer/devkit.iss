; TODO:
;   * Run oalinst.exe if openal DLL is not installed?
;   * Test that the installer works as non-admin on xp, vista and win7

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{20F85603-364E-44B6-A16A-6B75BD5D6968}
AppName=FIFE Win32 DevKit November 2009
AppVerName=FIFE Win32 DevKit November 2009
AppPublisher=FIFE team
AppPublisherURL=http://www.fifengine.de/
AppSupportURL=http://www.fifengine.de/
AppUpdatesURL=http://www.fifengine.de/
DefaultDirName={sd}\FIFE\build\win32\
DefaultGroupName=FIFE\Devkit
DirExistsWarning=No
EnableDirDoesntExistWarning=Yes
AllowNoIcons=yes
LicenseFile=license.rtf
Compression=lzma
SolidCompression=yes
OutputBaseFilename=FIFE_Win32_DevKit_Nov2009
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
Source: "..\applications\swigwin-1.3.40\*"; DestDir: "{app}\applications\swigwin-1.3.40"; Excludes: ".svn"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\includes\*"; DestDir: "{app}\includes"; Excludes: ".svn"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\README_DevKit_Nov2009.txt"; DestDir:"{app}"; Flags: isreadme

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
Name: "{group}\{cm:ProgramOnTheWeb,FIFE}"; Filename: "http://www.fifengine.de/"
Name: "{group}\{cm:UninstallProgram,FIFE}"; Filename: "{uninstallexe}"


