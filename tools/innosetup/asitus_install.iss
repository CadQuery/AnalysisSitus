; Created: February 2016
; Author:  Sergey Slyadnev

#include "asitus_config.iss"
#include "asitus_config_products.iss"

#define Output "delivery"
#define InputDir "..\..\..\.."

#ifndef ASitusDir
  #define ASitusDir InputDir + "\app"
#endif
#ifndef ProductsDir
  #define ProductsDir InputDir + "\products"
#endif


[Setup]
AppId={{BEC80C5C-8955-406F-869B-07C50D5A7E3A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
VersionInfoVersion={#MyAppVersion}
AppVerName={#MyAppGlobalName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={sd}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputBaseFilename={#MyBaseFileName}
OutputDir={#Output}
Compression=lzma2/ultra64
SolidCompression=yes
LZMAUseSeparateProcess=yes
WizardImageFile=resources\asitus_image_wizard.bmp
SetupIconFile=resources\asitus.ico
UninstallDisplayIcon=resources\uninstall.ico
SetupLogging=yes
DisableDirPage=auto
DisableProgramGroupPage=auto


[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
;Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked


[Files]
; README file
Source: {#ASitusDir}\README; DestName: README.txt; DestDir: {app}; Flags: isreadme sortfilesbyextension
; Ezydro application folders and scripts
Source: {#ASitusDir}\win64\vc10\bin\*;                           DestDir: {app}\app\win64\vc10\bin;                    Flags: sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\env.bat;                                    DestDir: {app}\app;
Source: {#ASitusDir}\run.bat;                                    DestDir: {app}\app;
Source: {#ASitusDir}\msbuild.bat;                                DestDir: {app}\app;
Source: {#ASitusDir}\msvc.bat;                                   DestDir: {app}\app;
Source: {#ASitusDir}\src\*;                                      DestDir: {app}\app\src;                               Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\msvc\vc10\AnalysisSitus.vcxproj;            DestDir: {app}\app\msvc\vc10;
Source: {#ASitusDir}\msvc\vc10\AnalysisSitus.vcxproj.filters;    DestDir: {app}\app\msvc\vc10;
Source: {#ASitusDir}\msvc\vc10\AnalysisSitus.sln;                DestDir: {app}\app\msvc\vc10;
Source: {#ASitusDir}\res\*;                                      DestDir: {app}\app\res;                               Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\data\*;                                     DestDir: {app}\app\data;                              Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
; Third-party products
Source: {#ProductsDir}\{#ad}\win64\vc10\bin\*;                            DestDir: {app}\products\{#ad}\win64\vc10\bin;           Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#ad}\win64\vc10\lib\*;                            DestDir: {app}\products\{#ad}\win64\vc10\lib;           Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#ad}\include\*;                                   DestDir: {app}\products\{#ad}\include;                  Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#freeimage}\*;                                    DestDir: {app}\products\{#freeimage};                   Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#freetype}\*;                                     DestDir: {app}\products\{#freetype};                    Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#gl2ps}\*;                                        DestDir: {app}\products\{#gl2ps};                       Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#eigen}\*;                                        DestDir: {app}\products\{#eigen};                       Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\src\*;                                     DestDir: {app}\products\{#occt}\src;                    Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\win64\vc10\bin\*.dll;                      DestDir: {app}\products\{#occt}\win64\vc10\bin;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\win64\vc10\bin\*.exe;                      DestDir: {app}\products\{#occt}\win64\vc10\bin;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\win64\vc10\lib\*;                          DestDir: {app}\products\{#occt}\win64\vc10\lib;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\win64\vc10\bind\*.dll;                     DestDir: {app}\products\{#occt}\win64\vc10\bind;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\win64\vc10\bind\*.pdb;                     DestDir: {app}\products\{#occt}\win64\vc10\bind;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\win64\vc10\bind\*.exe;                     DestDir: {app}\products\{#occt}\win64\vc10\bind;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\win64\vc10\libd\*;                         DestDir: {app}\products\{#occt}\win64\vc10\libd;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\inc\*;                                     DestDir: {app}\products\{#occt}\inc;                    Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\custom.bat;                                DestDir: {app}\products\{#occt};
Source: {#ProductsDir}\{#occt}\custom_vc10_64.bat;                        DestDir: {app}\products\{#occt};
Source: {#ProductsDir}\{#occt}\env.bat;                                   DestDir: {app}\products\{#occt};
Source: {#ProductsDir}\{#qr}\win64\vc10\bin\*;                            DestDir: {app}\products\{#qr}\win64\vc10\bin;           Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#qr}\win64\vc10\lib\*;                            DestDir: {app}\products\{#qr}\win64\vc10\lib;           Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#qr}\win64\vc10\bind\*;                           DestDir: {app}\products\{#qr}\win64\vc10\bind;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#qr}\win64\vc10\libd\*;                           DestDir: {app}\products\{#qr}\win64\vc10\libd;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#qr}\include\*;                                   DestDir: {app}\products\{#qr}\include;                  Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#qt}\bin\QtCore4.dll;                             DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\QtGui4.dll;                              DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\QtCored4.dll;                            DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\QtGuid4.dll;                             DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\moc.exe;                                 DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\lib\QtCore4.lib;                             DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\lib\QtGui4.lib;                              DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\lib\QtCored4.lib;                            DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\lib\QtGuid4.lib;                             DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\include\*;                                   DestDir: {app}\products\{#qt}\include;                  Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#tbb}\bin\intel64\vc10\tbb.dll;                   DestDir: {app}\products\{#tbb}\bin\intel64\vc10;
Source: {#ProductsDir}\{#tbb}\bin\intel64\vc10\tbbmalloc.dll;             DestDir: {app}\products\{#tbb}\bin\intel64\vc10;
Source: {#ProductsDir}\{#tbb}\lib\intel64\vc10\tbb.lib;                   DestDir: {app}\products\{#tbb}\lib\intel64\vc10;
Source: {#ProductsDir}\{#tbb}\lib\intel64\vc10\tbbmalloc.lib;             DestDir: {app}\products\{#tbb}\lib\intel64\vc10;
Source: {#ProductsDir}\{#tbb}\bin\intel64\vc10\tbb_debug.dll;             DestDir: {app}\products\{#tbb}\bin\intel64\vc10;
Source: {#ProductsDir}\{#tbb}\bin\intel64\vc10\tbbmalloc_debug.dll;       DestDir: {app}\products\{#tbb}\bin\intel64\vc10;
Source: {#ProductsDir}\{#tbb}\lib\intel64\vc10\tbb_debug.lib;             DestDir: {app}\products\{#tbb}\lib\intel64\vc10;
Source: {#ProductsDir}\{#tbb}\lib\intel64\vc10\tbbmalloc_debug.lib;       DestDir: {app}\products\{#tbb}\lib\intel64\vc10;
Source: {#ProductsDir}\{#tbb}\include\*;                                  DestDir: {app}\products\{#tbb}\include;                 Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc10\bin\*.dll;                       DestDir: {app}\products\{#vtk}\win64\vc10\bin;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc10\lib\*;                           DestDir: {app}\products\{#vtk}\win64\vc10\lib;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc10\bind\*.dll;                      DestDir: {app}\products\{#vtk}\win64\vc10\bind;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc10\libd\*;                          DestDir: {app}\products\{#vtk}\win64\vc10\libd;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc10\include\*;                       DestDir: {app}\products\{#vtk}\win64\vc10\include;      Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
; Resource files (icons, custom script, ...)
Source: "resources\asitus.ico";          DestDir: {app}\icons
Source: "resources\uninstall.ico";       DestDir: {app}\icons
Source: "resources\custom.bat";          DestDir: {app}\app


[Icons]
Name: {group}\{#MyAppName};                       Filename: {app}\app\run.bat;            IconFileName: {app}\icons\asitus.ico
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe};               IconFileName: {app}\icons\uninstall.ico
Name: {commondesktop}\{#MyAppName};               Filename: {app}\app\run.bat;            IconFileName: {app}\icons\asitus.ico;           Tasks: desktopicon


[InstallDelete]
Type: filesandordirs; Name: "{app}\app"
Type: filesandordirs; Name: "{app}\products"
