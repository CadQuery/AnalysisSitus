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

; Application folders and scripts
Source: {#ASitusDir}\win64\vc12\bin\*;                                    DestDir: {app}\app\win64\vc12\bin;                    Flags: sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\env.bat;                                             DestDir: {app}\app;
Source: {#ASitusDir}\run.bat;                                             DestDir: {app}\app;
Source: {#ASitusDir}\msbuild.bat;                                         DestDir: {app}\app;
Source: {#ASitusDir}\msvc.bat;                                            DestDir: {app}\app;
Source: {#ASitusDir}\src\*;                                               DestDir: {app}\app\src;                               Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\msvc\vc12\AnalysisSitus.vcxproj;                     DestDir: {app}\app\msvc\vc12;
Source: {#ASitusDir}\msvc\vc12\AnalysisSitus.vcxproj.filters;             DestDir: {app}\app\msvc\vc12;
Source: {#ASitusDir}\msvc\vc12\AnalysisSitus.sln;                         DestDir: {app}\app\msvc\vc12;
Source: {#ASitusDir}\res\*;                                               DestDir: {app}\app\res;                               Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\data\*;                                              DestDir: {app}\app\data;                              Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname

; Third-party products
Source: {#occt_origin_global}\src\*;                                      DestDir: {app}\products\{#occt}\src;                    Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\win64\vc12\bin\*.dll;                       DestDir: {app}\products\{#occt}\win64\vc12\bin;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\win64\vc12\bin\*.exe;                       DestDir: {app}\products\{#occt}\win64\vc12\bin;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\win64\vc12\lib\*;                           DestDir: {app}\products\{#occt}\win64\vc12\lib;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\win64\vc12\bind\*.dll;                      DestDir: {app}\products\{#occt}\win64\vc12\bind;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\win64\vc12\bind\*.pdb;                      DestDir: {app}\products\{#occt}\win64\vc12\bind;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\win64\vc12\bind\*.exe;                      DestDir: {app}\products\{#occt}\win64\vc12\bind;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\win64\vc12\libd\*;                          DestDir: {app}\products\{#occt}\win64\vc12\libd;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\inc\*;                                      DestDir: {app}\products\{#occt}\inc;                    Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_origin_global}\custom.bat;                                 DestDir: {app}\products\{#occt};
Source: {#occt_origin_global}\env.bat;                                    DestDir: {app}\products\{#occt};
;
Source: {#ad_origin_global}\win64\vc12\bin\*;                             DestDir: {app}\products\{#ad}\win64\vc12\bin;           Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ad_origin_global}\win64\vc12\lib\*;                             DestDir: {app}\products\{#ad}\win64\vc12\lib;           Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ad_origin_global}\win64\vc12\bind\*;                            DestDir: {app}\products\{#ad}\win64\vc12\bind;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ad_origin_global}\win64\vc12\libd\*;                            DestDir: {app}\products\{#ad}\win64\vc12\libd;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ad_origin_global}\include\*;                                    DestDir: {app}\products\{#ad}\include;                  Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
;
Source: {#qr_origin_global}\win64\vc12\bin\*;                             DestDir: {app}\products\{#qr}\win64\vc12\bin;           Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#qr_origin_global}\win64\vc12\lib\*;                             DestDir: {app}\products\{#qr}\win64\vc12\lib;           Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#qr_origin_global}\win64\vc12\bind\*;                            DestDir: {app}\products\{#qr}\win64\vc12\bind;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#qr_origin_global}\win64\vc12\libd\*;                            DestDir: {app}\products\{#qr}\win64\vc12\libd;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#qr_origin_global}\include\*;                                    DestDir: {app}\products\{#qr}\include;                  Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
;
Source: {#feature_origin_global}\win64\vc12\bin\feature.dll;              DestDir: {app}\products\{#feature}\win64\vc12\bin;      Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#feature_origin_global}\win64\vc12\lib\feature.lib;              DestDir: {app}\products\{#feature}\win64\vc12\lib;      Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#feature_origin_global}\win64\vc12\bind\feature.dll;             DestDir: {app}\products\{#feature}\win64\vc12\bind;     Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#feature_origin_global}\win64\vc12\libd\feature.lib;             DestDir: {app}\products\{#feature}\win64\vc12\libd;     Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#feature_origin_global_src}\*.h;                                 DestDir: {app}\products\{#feature}\include;             Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
;
Source: {#occt_3rdparties}\{#freeimage}\*;                                DestDir: {app}\products\{#freeimage};                   Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_3rdparties}\{#freetype}\*;                                 DestDir: {app}\products\{#freetype};                    Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#occt_3rdparties}\{#gl2ps}\*;                                    DestDir: {app}\products\{#gl2ps};                       Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#eigen}\*;                                        DestDir: {app}\products\{#eigen};                       Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
;
Source: {#ProductsDir}\{#qt}\bin\Qt5Core.dll;                             DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\Qt5Gui.dll;                              DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\Qt5Widgets.dll;                          DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\Qt5Cored.dll;                            DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\Qt5Guid.dll;                             DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\Qt5Widgetsd.dll;                         DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\moc.exe;                                 DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\lib\Qt5Core.lib;                             DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\lib\Qt5Gui.lib;                              DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\lib\Qt5Widgets.lib;                          DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\lib\Qt5Cored.lib;                            DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\lib\Qt5Guid.lib;                             DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\lib\Qt5Widgetsd.lib;                         DestDir: {app}\products\{#qt}\lib;
Source: {#ProductsDir}\{#qt}\include\*;                                   DestDir: {app}\products\{#qt}\include;                  Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#qt}\plugins\platforms\*;                         DestDir: {app}\products\{#qt}\plugins\platforms;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
;
Source: {#occt_3rdparties}\{#tbb}\bin\intel64\vc12\tbb.dll;               DestDir: {app}\products\{#tbb}\bin\intel64\vc12;
Source: {#occt_3rdparties}\{#tbb}\bin\intel64\vc12\tbbmalloc.dll;         DestDir: {app}\products\{#tbb}\bin\intel64\vc12;
Source: {#occt_3rdparties}\{#tbb}\lib\intel64\vc12\tbb.lib;               DestDir: {app}\products\{#tbb}\lib\intel64\vc12;
Source: {#occt_3rdparties}\{#tbb}\lib\intel64\vc12\tbbmalloc.lib;         DestDir: {app}\products\{#tbb}\lib\intel64\vc12;
Source: {#occt_3rdparties}\{#tbb}\bin\intel64\vc12\tbb_debug.dll;         DestDir: {app}\products\{#tbb}\bin\intel64\vc12;
Source: {#occt_3rdparties}\{#tbb}\bin\intel64\vc12\tbbmalloc_debug.dll;   DestDir: {app}\products\{#tbb}\bin\intel64\vc12;
Source: {#occt_3rdparties}\{#tbb}\lib\intel64\vc12\tbb_debug.lib;         DestDir: {app}\products\{#tbb}\lib\intel64\vc12;
Source: {#occt_3rdparties}\{#tbb}\lib\intel64\vc12\tbbmalloc_debug.lib;   DestDir: {app}\products\{#tbb}\lib\intel64\vc12;
Source: {#occt_3rdparties}\{#tbb}\include\*;                              DestDir: {app}\products\{#tbb}\include;                 Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\*.dll;                       DestDir: {app}\products\{#vtk}\win64\vc12\bin;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc12\lib\*;                           DestDir: {app}\products\{#vtk}\win64\vc12\lib;          Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc12\bind\*.dll;                      DestDir: {app}\products\{#vtk}\win64\vc12\bind;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc12\libd\*;                          DestDir: {app}\products\{#vtk}\win64\vc12\libd;         Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#vtk}\win64\vc12\include\*;                       DestDir: {app}\products\{#vtk}\win64\vc12\include;      Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname

; Resource files (icons, custom script, ...)
Source: "resources\asitus.ico";                                           DestDir: {app}\icons
Source: "resources\uninstall.ico";                                        DestDir: {app}\icons
Source: "resources\custom.bat";                                           DestDir: {app}\app


[Icons]
Name: {group}\{#MyAppName};                       Filename: {app}\app\run.bat;            IconFileName: {app}\icons\asitus.ico
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe};               IconFileName: {app}\icons\uninstall.ico
Name: {commondesktop}\{#MyAppName};               Filename: {app}\app\run.bat;            IconFileName: {app}\icons\asitus.ico;           Tasks: desktopicon


[InstallDelete]
Type: filesandordirs; Name: "{app}\app"
Type: filesandordirs; Name: "{app}\products"
