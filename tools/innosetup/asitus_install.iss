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
AppName={#MyAppName}-min
AppVersion={#MyAppVersion}
VersionInfoVersion={#MyAppVersion}
AppVerName={#MyAppGlobalName}-min
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
Source: {#ASitusDir}\win64\vc12\bin\*.dll;                                DestDir: {app}\app\win64\vc12\bin;                      Flags: sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\win64\vc12\bin\*.exe;                                DestDir: {app}\app\win64\vc12\bin;                      Flags: sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\env.bat;                                             DestDir: {app}\app;
Source: {#ASitusDir}\run.bat;                                             DestDir: {app}\app;
Source: {#ASitusDir}\res\*;                                               DestDir: {app}\app\res;                                 Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ASitusDir}\data\*;                                              DestDir: {app}\app\data;                                Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname

; Third-party products
Source: {#ProductsDir}\{#occt}\src\*;                                      DestDir: {app}\products\{#occt}\src;                    Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#occt}\*.bat;                                      DestDir: {app}\products\{#occt};                        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
Source: {#ProductsDir}\{#freeimage}\bin\FreeImage.dll;                     DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#freetype}\bin\freetype.dll;                       DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKBin.dll;                   DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKBinL.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKBO.dll;                    DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKBool.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKBRep.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKCAF.dll;                   DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKCDF.dll;                   DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKDCAF.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKernel.dll;                 DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKG2d.dll;                   DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKG3d.dll;                   DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKGeomAlgo.dll;              DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKGeomBase.dll;              DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKIGES.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKLCAF.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKMath.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKMesh.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKOffset.dll;                DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKPrim.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKService.dll;               DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKShHealing.dll;             DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKStd.dll;                   DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKStdL.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKSTEP.dll;                  DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKSTEP209.dll;               DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKSTEPAttr.dll;              DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKSTEPBase.dll;              DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKTopAlgo.dll;               DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKTopTest.dll;               DestDir: {app}\products\{#occt}\win64\vc12\bin;
Source: {#ProductsDir}\{#occt}\win64\vc12\bin\TKXSBase.dll;                DestDir: {app}\products\{#occt}\win64\vc12\bin;
;
Source: {#ProductsDir}\{#ad}\win64\vc12\bin\ActiveDataAPI.dll;             DestDir: {app}\products\{#ad}\win64\vc12\bin;
Source: {#ProductsDir}\{#ad}\win64\vc12\bin\ActiveDataAux.dll;             DestDir: {app}\products\{#ad}\win64\vc12\bin;
Source: {#ProductsDir}\{#ad}\win64\vc12\bin\ActiveData.dll;                DestDir: {app}\products\{#ad}\win64\vc12\bin;
;
Source: {#ProductsDir}\{#qr}\win64\vc12\bin\QrBLAS.dll;                    DestDir: {app}\products\{#qr}\win64\vc12\bin;
Source: {#ProductsDir}\{#qr}\win64\vc12\bin\QrBSpl.dll;                    DestDir: {app}\products\{#qr}\win64\vc12\bin;
Source: {#ProductsDir}\{#qr}\win64\vc12\bin\QrCascade.dll;                 DestDir: {app}\products\{#qr}\win64\vc12\bin;
Source: {#ProductsDir}\{#qr}\win64\vc12\bin\QrCore.dll;                    DestDir: {app}\products\{#qr}\win64\vc12\bin;
Source: {#ProductsDir}\{#qr}\win64\vc12\bin\QrGeom2D.dll;                  DestDir: {app}\products\{#qr}\win64\vc12\bin;
Source: {#ProductsDir}\{#qr}\win64\vc12\bin\QrGeom3D.dll;                  DestDir: {app}\products\{#qr}\win64\vc12\bin;
Source: {#ProductsDir}\{#qr}\win64\vc12\bin\QrTools.dll;                   DestDir: {app}\products\{#qr}\win64\vc12\bin;
;
Source: {#ProductsDir}\{#qt}\bin\Qt5Core.dll;                              DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\Qt5Gui.dll;                               DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\bin\Qt5Widgets.dll;                           DestDir: {app}\products\{#qt}\bin;
Source: {#ProductsDir}\{#qt}\plugins\platforms\*;                          DestDir: {app}\products\{#qt}\plugins\platforms;        Flags: recursesubdirs createallsubdirs sortfilesbyextension sortfilesbyname
;
Source: {#ProductsDir}\{#tbb}\bin\intel64\vc12\tbb.dll;                    DestDir: {app}\products\{#tbb}\bin\intel64\vc12;
Source: {#ProductsDir}\{#tbb}\bin\intel64\vc12\tbbmalloc.dll;              DestDir: {app}\products\{#tbb}\bin\intel64\vc12;
;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\QVTKWidgetPlugin.dll;                    DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkalglib-7.1.dll;                       DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonColor-7.1.dll;                  DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonComputationalGeometry-7.1.dll;  DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonCore-7.1.dll;                   DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonDataModel-7.1.dll;              DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonExecutionModel-7.1.dll;         DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonMath-7.1.dll;                   DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonMisc-7.1.dll;                   DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonSystem-7.1.dll;                 DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkCommonTransforms-7.1.dll;             DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkDICOMParser-7.1.dll;                  DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersCore-7.1.dll;                  DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersExtraction-7.1.dll;            DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersGeneral-7.1.dll;               DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersGeometry-7.1.dll;              DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersHybrid-7.1.dll;                DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersImaging-7.1.dll;               DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersModeling-7.1.dll;              DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersParallel-7.1.dll;              DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersSources-7.1.dll;               DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkFiltersStatistics-7.1.dll;            DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkfreetype-7.1.dll;                     DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkglew-7.1.dll;                         DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkGUISupportQt-7.1.dll;                 DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkImagingColor-7.1.dll;                 DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkImagingCore-7.1.dll;                  DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkImagingFourier-7.1.dll;               DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkImagingGeneral-7.1.dll;               DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkImagingHybrid-7.1.dll;                DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkImagingSources-7.1.dll;               DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkInteractionStyle-7.1.dll;             DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkInteractionWidgets-7.1.dll;           DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkIOCore-7.1.dll;                       DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkIOImage-7.1.dll;                      DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkIOLegacy-7.1.dll;                     DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkjpeg-7.1.dll;                         DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkmetaio-7.1.dll;                       DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkParallelCore-7.1.dll;                 DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkpng-7.1.dll;                          DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingAnnotation-7.1.dll;          DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingContext2D-7.1.dll;           DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingContextOpenGL2-7.1.dll;      DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingCore-7.1.dll;                DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingFreeType-7.1.dll;            DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingLabel-7.1.dll;               DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingOpenGL2-7.1.dll;             DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingVolume-7.1.dll;              DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtksys-7.1.dll;                          DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtktiff-7.1.dll;                         DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkzlib-7.1.dll;                         DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkRenderingContext2D-7.1.dll;           DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkInfovisCore-7.1.dll;                  DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkInfovisLayout-7.1.dll;                DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkChartsCore-7.1.dll;                   DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkViewsCore-7.1.dll;                    DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkViewsInfovis-7.1.dll;                 DestDir: {app}\products\{#vtk}\win64\vc12\bin;
Source: {#ProductsDir}\{#vtk}\win64\vc12\bin\vtkViewsContext2D-7.1.dll;               DestDir: {app}\products\{#vtk}\win64\vc12\bin;

; Resource files (icons, custom script, ...)
Source: "resources\asitus.ico";                                           DestDir: {app}\icons
Source: "resources\uninstall.ico";                                        DestDir: {app}\icons
Source: "resources\custom.bat";                                           DestDir: {app}\app

[Icons]
Name: {group}\{#MyAppName};                       Filename: {app}\app\run.bat;        IconFileName: {app}\icons\asitus.ico
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe};           IconFileName: {app}\icons\uninstall.ico
Name: {commondesktop}\{#MyAppName};               Filename: {app}\app\run.bat;        IconFileName: {app}\icons\asitus.ico;           Tasks: desktopicon


[InstallDelete]
Type: filesandordirs; Name: "{app}\app"
Type: filesandordirs; Name: "{app}\products"
