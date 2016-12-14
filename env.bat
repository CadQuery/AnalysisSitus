@echo off

set VCVER=%1
set ARCH=%2

if /I "%ARCH%" == "win32" set ARCH=32
if /I "%ARCH%" == "win64" set ARCH=64

if "%VCVER%" == "" set VCVER=vc12
if "%ARCH%"  == "" set ARCH=64

set CASDEB=d
set BUILD_CONFIG=debug
if not "%3" == "" (
    if /I "%3" == "Debug" (
        set CASDEB=d
        set BUILD_CONFIG=debug
    ) else if /I "%3" == "Release" (
        set CASDEB=
        set BUILD_CONFIG=release
    ) else (
        echo Error: third argument ^(%3^) should specify build mode,
        echo one of: Debug or Release
        exit
    )
)

if exist %~dp0custom.bat call %~dp0custom.bat

set DEB=%CASDEB%

rem ===========================================================================
rem Setting AS environment 
rem ===========================================================================

if "%AS_ROOT%"      == "" set "AS_ROOT=%~dp0"
if "%AS_RESOURCES%" == "" set "AS_RESOURCES=%AS_ROOT%\res"
if "%AS_TOOLS%"     == "" set "AS_TOOLS=%AS_ROOT%\tools"
if "%AS_LIB%"       == "" set "AS_LIB=%AS_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
if "%AS_BIN%"       == "" set "AS_BIN=%AS_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"
if "%AS_INC%"       == "" set "AS_INC=%AS_ROOT%\include"
if "%AS_PRODUCTS%"  == "" set "AS_PRODUCTS=%AS_ROOT%3rd-parties"

rem ===========================================================================
rem Setting OCCT environment 
rem ===========================================================================

if "%PRODUCTS_PATH%"   == "" set "PRODUCTS_PATH=%AS_PRODUCTS%"
if "%CASROOT%"         == "" set "CASROOT=%AS_PRODUCTS%\occt-vc12-64"
if "%OCCT_3RDPARTIES%" == "" set "OCCT_3RDPARTIES=%AS_PRODUCTS%"

set "CAS_INC=%CASROOT%\inc"
set "CAS_LIB64=%CASROOT%\win64\%VCVER%\lib%DEB%
set "CAS_BIN64=%CASROOT%\win64\%VCVER%\bin%DEB%

call %CASROOT%\env.bat %VCVER% %ARCH% %BUILD_CONFIG%

rem ===========================================================================
rem Setting environment for AD (Active Data)
rem ===========================================================================

if "%AD_ROOT%" == "" set "AD_ROOT=%AS_PRODUCTS%\active_data-vc12-64"
set "AD_INC=%AD_ROOT%\include"
set "AD_LIB=%AD_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "AD_BIN=%AD_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for QR (Quaoar)
rem ===========================================================================

if "%QR_ROOT%" == "" set "QR_ROOT=%AS_PRODUCTS%\quaoar-vc12-64"
set "QR_INC=%QR_ROOT%\include"
set "QR_LIB=%QR_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "QR_BIN=%QR_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting TBB environment 
rem ===========================================================================

if "%TBB_ROOT%" == "" set "TBB_ROOT=%AS_PRODUCTS%\tbb44_20151115oss"
set "TBB_INC=%TBB_ROOT%\include\tbb"
set "TBB_LIB=%TBB_ROOT%\lib\intel64\vc12"
set "TBB_BIN=%TBB_ROOT%\bin\intel64\vc12"

rem ===========================================================================
rem Setting environment for VTK
rem ===========================================================================

if "%VTK_ROOT%" == "" set "VTK_ROOT=%AS_PRODUCTS%\vtk-7.1.0-vc12-64"
set "VTK_INC=%VTK_ROOT%\include\vtk-7.1"
set "VTK_LIB=%VTK_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "VTK_BIN=%VTK_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for Qt
rem ===========================================================================

if "%QTDIR%" == "" set "QTDIR=%AS_PRODUCTS%\qt-5.7-vc12-64"
set "QT_INC=%QTDIR%\include;%QTDIR%\include\QtCore;%QTDIR%\include\QtGui;%QTDIR%\include\QtWidgets"
set "QT_LIB=%QTDIR%\lib"
set "QT_BIN=%QTDIR%\bin"

set "QT_PLUGIN_PATH=%QTDIR%/plugins"

rem ===========================================================================
rem Setting Eigen environment
rem ===========================================================================

if "%EIGEN_ROOT%" == "" set "EIGEN_ROOT=%AS_PRODUCTS%\eigen-3.2.0"

set "EIGEN_INC=%EIGEN_ROOT%"

rem ===========================================================================
rem Setting environment for Inno Setup
rem ===========================================================================

if "%INNOSETUP_ROOT%" == "" set "INNOSETUP_ROOT=%AS_PRODUCTS%\innosetup5"

rem ===========================================================================
rem Adjust PATH variable
rem ===========================================================================

set "PATH=%AS_BIN%;%CAS_BIN64%;%FREETYPE_BIN%;%AD_BIN%;%QR_BIN%;%VTK_BIN%;%QT_BIN%;%TBB_BIN%;%INNOSETUP_ROOT%;%PATH%"

rem ===========================================================================
rem Customize OCCT environment variables
rem ===========================================================================

if "%CSF_PluginDefaults%" == "" set "CSF_PluginDefaults=%AS_RESOURCES%"
if "%CSF_ResourcesDefaults%" == "" set "CSF_ResourcesDefaults=%AS_RESOURCES%"

rem ===========================================================================
rem Hint echoes
rem ===========================================================================

echo ...
echo ARCH                  = %ARCH%
echo VCVER                 = %VCVER%
echo BUILD_CONFIG          = %BUILD_CONFIG%
echo AS_ROOT               = %AS_ROOT%
echo AS_PRODUCTS           = %AS_PRODUCTS%
echo VTK_ROOT              = %VTK_ROOT%
echo VTK_INC               = %VTK_INC%
echo CASROOT               = %CASROOT%
echo CAS_BIN64             = %CAS_BIN64%
echo CASDEB                = %CASDEB%
echo CSF_PluginDefaults    = %CSF_PluginDefaults% 
echo CSF_ResourcesDefaults = %CSF_ResourcesDefaults%
echo ... 
echo PATH                  = %PATH% 
echo ...
