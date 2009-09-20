; TODO:
;   * Check that ActivePython 2.5 is installed?
;   * Run oalinst.exe?
;   * Install shortcuts to the clients
;   * Write instructions on how to use this script

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
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes

[Types]
Name: "full"; Description: "Full installation"
Name: "compact"; Description: "Compact installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "main"; Description: "Main Files"; Types: full compact custom; Flags: fixed
Name: "source"; Description: "Source code"; Types: full
Name: "clients"; Description: "Clients"; Types: full
Name: "clients\editor"; Description: "Editor"; Types: full compact
Name: "clients\rio_de_hola"; Description: "Rio de hola techdemo"; Types: full
Name: "clients\pychan_demo"; Description: "Pychan demonstration"; Types: full

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; Main
Source: "fife\*"; DestDir: "{app}"; Excludes: ".svn,\clients,\ext,\build,\utils,\engine\core"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\doc\README"; DestDir: "{app}\doc\"; DestName:"Readme.txt"
Source: "fife\doc\AUTHORS"; DestDir: "{app}\doc\"; DestName:"Authors.txt"
Source: "fife\doc\COPYING"; DestDir: "{app}\doc\"; DestName:"Copying.txt"

; Source
Source: "fife\engine\core\*"; DestDir: "{app}\engine\core"; Excludes: ".svn"; Components: source; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "fife\build\*"; DestDir: "{app}\build"; Excludes: ".svn"; Components: source; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\utils\*"; DestDir: "{app}\utils"; Excludes: ".svn"; Components: source; Flags: ignoreversion recursesubdirs createallsubdirs
; We can probably drop this in the windows installer, saving some space
;Source: "fife\ext\*"; DestDir: "{app}\ext"; Excludes: ".svn"; Components: source; Flags: ignoreversion recursesubdirs createallsubdirs

; Clients
Source: "fife\clients\editor\*"; DestDir: "{app}\clients\editor"; Excludes: ".svn"; Components: clients\editor; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "fife\clients\rio_de_hola\*" DestDir: "{app}\clients\rio_de_hola"; Excludes: ".svn"; Components: clients\rio_de_hola; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "fife\clients\pychan_demo\*"; DestDir: "{app}\clients\rio_de_hola"; Excludes: ".svn"; Components: clients\pychan_demo; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{cm:ProgramOnTheWeb,FIFE}"; Filename: "http://www.fifengine.de/"
Name: "{group}\{cm:UninstallProgram,FIFE}"; Filename: "{uninstallexe}"


