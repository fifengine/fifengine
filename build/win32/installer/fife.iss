; TODO:
;   * Check that ActivePython 2.5 is installed?
;   * Run oalinst.exe if openal DLL is not installed?
;   * Test that the installer works as non-admin on xp, vista and win7

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{82F36A38-DEDD-4B1C-8772-246886235B27}
AppName=FIFE
AppVerName=FIFE 2009.1
AppPublisher=FIFE team
AppPublisherURL=http://www.fifengine.de/
AppSupportURL=http://www.fifengine.de/
AppUpdatesURL=http://www.fifengine.de/
DefaultDirName={pf}\FIFE
DefaultGroupName=FIFE
AllowNoIcons=yes
LicenseFile=license.rtf
Compression=lzma
SolidCompression=yes
OutputBaseFilename=FIFE 2009.1
OutputDir=.
DisableReadyPage=yes
; This will require some testing. We can't install to Program files if the user is not an admin.
; PrivilegesRequired=none

[Types]
Name: "full"; Description: "Full installation"
Name: "compact"; Description: "Compact installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "main"; Description: "Main Files"; Types: full compact custom; Flags: fixed
Name: "source"; Description: "Source code"; Types: full compact
Name: "source\sdk"; Description: "Source Developer Kit"; Types: full
Name: "clients"; Description: "Clients"; Types: full
Name: "clients\editor"; Description: "Editor"; Types: full compact
Name: "clients\rio_de_hola"; Description: "Rio de hola techdemo"; Types: full
Name: "clients\pychan_demo"; Description: "Pychan demonstration"; Types: full

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; Main
Source: "fife\*"; DestDir: "{app}"; Excludes: ".svn,*.o,*.pyc,\clients,\ext,\build,\utils,\engine\core"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\doc\README"; DestDir: "{app}\doc\"; DestName:"Readme.txt"; Flags: isreadme
Source: "fife\doc\AUTHORS"; DestDir: "{app}\doc\"; DestName:"Authors.txt"
Source: "fife\doc\COPYING"; DestDir: "{app}\doc\"; DestName:"Copying.txt"

; Source
Source: "fife\engine\core\*"; DestDir: "{app}\engine\core"; Excludes: ".svn,*.o,*.pyc"; Components: source; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\build\*"; DestDir: "{app}\build"; Excludes: ".svn,*.o,*.pyc,\win32\applications,\win32\binaries,\win32\includes,\win32\static_libs"; Components: source; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\utils\*"; DestDir: "{app}\utils"; Excludes: ".svn,*.o,*.pyc"; Components: source; Flags: ignoreversion recursesubdirs createallsubdirs
; We can probably drop this in the windows installer, saving some space
;Source: "fife\ext\*"; DestDir: "{app}\ext"; Excludes: ".svn,*.o,*.pyc"; Components: source; Flags: ignoreversion recursesubdirs createallsubdirs

; SDK
Source: "fife\build\win32\applications\*"; DestDir: "{app}\build\win32\applications"; Excludes: ".svn,*.o,*.pyc"; Components: source\sdk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\build\win32\binaries\*"; DestDir: "{app}\build\win32\binaries"; Excludes: ".svn,*.o,*.pyc"; Components: source\sdk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\build\win32\includes\*"; DestDir: "{app}\build\win32\includes"; Excludes: ".svn,*.o,*.pyc"; Components: source\sdk; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\build\win32\static_libs\*"; DestDir: "{app}\build\win32\static_libs"; Excludes: ".svn,*.o,*.pyc"; Components: source\sdk; Flags: ignoreversion recursesubdirs createallsubdirs

; Clients
Source: "fife\clients\editor\*"; DestDir: "{app}\clients\editor"; Excludes: ".svn,*.o,*.pyc"; Components: clients\editor; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\clients\rio_de_hola\*"; DestDir: "{app}\clients\rio_de_hola"; Excludes: ".svn,*.o,*.pyc"; Components: clients\rio_de_hola; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\clients\pychan_demo\*"; DestDir: "{app}\clients\rio_de_hola"; Excludes: ".svn,*.o,*.pyc"; Components: clients\pychan_demo; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{cm:ProgramOnTheWeb,FIFE}"; Filename: "http://www.fifengine.de/"
Name: "{group}\{cm:UninstallProgram,FIFE}"; Filename: "{uninstallexe}"
Name: "{group}\FIFEdit"; Filename: "{app}\clients\editor\run.py"; WorkingDir: "{app}\clients\editor"; Comment: "FIFE Editor"; Components: "clients\editor"
Name: "{group}\Rio de hola"; Filename: "{app}\clients\rio_de_hola\run.py"; WorkingDir: "{app}\clients\rio_de_hola"; Comment: "Techdemo"; Components: "clients\rio_de_hola"
Name: "{group}\Pychan demo"; Filename: "{app}\clients\pychan_demo\pychan_test.py"; WorkingDir: "{app}\clients\pychan_demo"; Comment: "FIFE GUI demo"; Components: "clients\pychan_demo"

