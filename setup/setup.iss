#include "config.iss"

#define MyAppGlobalName "Analysis Situs"
#define MyAppName "Analysis Situs"
#define MyAppDefaultDir "ASitus"
#define MyAppPublisher "Sergey Slyadnev"
#define MyAppURL "http://analysissitus.org/"

[Setup]
AppId={{07C421CF-04BC-43AD-B4FA-V1C84457B154}
AppName={#MyAppName}
AppVerName={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
OutputBaseFilename={#MyAppInstName}

OutputDir={#SETUP_OUTPUT}
DefaultDirName={pf}\{#MyAppDefaultDir}
DefaultGroupName={#MyAppGlobalName}
DisableWelcomePage=false
Compression=lzma/ultra
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
AlwaysShowDirOnReadyPage=true
AlwaysShowGroupOnReadyPage=true
AlwaysShowComponentsList=false
ChangesAssociations=yes
FlatComponentsList=false
PrivilegesRequired=admin
ExtraDiskSpaceRequired=0
AllowNoIcons=yes

; Icons/images
SetupIconFile=imgs\icon.ico
WizardImageFile=imgs\setup_image.bmp
WizardSmallImageFile=imgs\setup_image_small.bmp

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon";   Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "flagFileAssoc"; Description: "&Associate files of supported formats (BREP) with Analysis Situs"; GroupDescription: "File associations";

[Files]
Source: {#BIN_INPUT}\{#MyAppExeName};        DestDir: {app}; Flags: ignoreversion
Source: {#BIN_INPUT}\*;                      DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs;
Source: {#BIN_INPUT}\{#MyAppIcoName};        DestDir: {app};
Source: {#BIN_INPUT}\{#MyAppDesktopIcoName}; DestDir: {app};

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\icon.ico"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\{#MyAppDesktopIcoName}"; Tasks: desktopicon

[Registry]
Root: HKCR; Subkey: ".brep";                                    ValueType: string; ValueName: ""; ValueData: "AnalysisSitys"; Flags: uninsdeletevalue; Tasks: flagFileAssoc
Root: HKCR; Subkey: ".rle";                                     ValueType: string; ValueName: ""; ValueData: "AnalysisSitys"; Flags: uninsdeletevalue; Tasks: flagFileAssoc
Root: HKCR; Subkey: ".bin";                                     ValueType: string; ValueName: ""; ValueData: "AnalysisSitys"; Flags: uninsdeletevalue; Tasks: flagFileAssoc
Root: HKCR; Subkey: ".binbrep";                                 ValueType: string; ValueName: ""; ValueData: "AnalysisSitys"; Flags: uninsdeletevalue; Tasks: flagFileAssoc
Root: HKCR; Subkey: "AnalysisSitys";                            ValueType: string; ValueName: ""; ValueData: "AnalysisSitus"; Flags: uninsdeletekey
Root: HKCR; Subkey: "AnalysisSitys\DefaultIcon";                ValueType: string; ValueName: ""; ValueData: "{app}\icon.ico"
Root: HKCR; Subkey: "AnalysisSitys\FriendlyAppName";            ValueType: string; ValueName: ""; ValueData: "{#MyAppName}"
Root: HKCR; Subkey: "AnalysisSitys\shell\open\command";         ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""
Root: HKCR; Subkey: "AnalysisSitys\shell\open\FriendlyAppName"; ValueType: string; ValueName: ""; ValueData: "{#MyAppName}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[InstallDelete]
Name: {app}\Qt*.dll;      Type: filesandordirs
Name: {app}\TK*.dll;      Type: filesandordirs
Name: {group}\;           Type: filesandordirs
Name: {app}\imageformats; Type: filesandordirs
Name: {app}\platforms;    Type: filesandordirs
Name: {app}\Qt;           Type: filesandordirs
Name: {app}\resources;    Type: filesandordirs
