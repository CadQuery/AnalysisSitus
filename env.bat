@echo off

set VCVER=%1
set ARCH=%2

if /I "%ARCH%" == "win32" set ARCH=32
if /I "%ARCH%" == "win64" set ARCH=64

if "%VCVER%" == "" set VCVER=vc10
if "%ARCH%"  == "" set ARCH=64

set CASDEB=
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
if "%AS_LIB%"       == "" set "AS_LIB=%AS_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
if "%AS_BIN%"       == "" set "AS_BIN=%AS_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"
if "%AS_INC%"       == "" set "AS_INC=%AS_ROOT%\include"
if "%AS_PRODUCTS%"  == "" set "AS_PRODUCTS=%AS_ROOT%products"

rem ===========================================================================
rem Setting OCCT environment 
rem ===========================================================================

if "%PRODUCTS_PATH%" == "" set "PRODUCTS_PATH=%AS_PRODUCTS%"
if "%CASROOT%"       == "" set "CASROOT=%AS_PRODUCTS%\occt"

set "CAS_INC=%CASROOT%\inc"
set "CAS_LIB32=%CASROOT%\win32\%VCVER%\lib%DEB%"
set "CAS_BIN32=%CASROOT%\win32\%VCVER%\bin%DEB%"
set "CAS_LIB64=%CASROOT%\win64\%VCVER%\lib%DEB%"
set "CAS_BIN64=%CASROOT%\win64\%VCVER%\bin%DEB%"

rem Call OCCT env.bat -- all 3-rd party binaries will be added to PATH
call %CASROOT%\env.bat %VCVER% win%ARCH% %BUILD_CONFIG%

rem ===========================================================================
rem Setting environment for AD (Active Data)
rem ===========================================================================

if "%AD_ROOT%" == "" set "AD_ROOT=%AS_PRODUCTS%\active_data"
set "AD_INC=%AD_ROOT%\include"
set "AD_LIB=%AD_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "AD_BIN=%AD_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for QR (Quaoar)
rem ===========================================================================

if "%QR_ROOT%" == "" set "QR_ROOT=%AS_PRODUCTS%\quaoar"
set "QR_INC=%QR_ROOT%\include"
set "QR_LIB=%QR_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "QR_BIN=%QR_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting TBB environment 
rem ===========================================================================

if "%TBB_ROOT%" == "" set "TBB_ROOT=%AS_PRODUCTS%\tbb44_20150728oss"
set "TBB_INC=%TBB_ROOT%\include"
set "TBB_LIB=%TBB_ROOT%\lib\intel64\vc10"
set "TBB_BIN=%TBB_ROOT%\bin\intel64\vc10"

rem ===========================================================================
rem Setting environment for VTK
rem ===========================================================================

if "%VTK_ROOT%" == "" set "VTK_ROOT=%AS_PRODUCTS%\vtk-6.1.0"
set "VTK_INC=%VTK_ROOT%\win%ARCH%\%VCVER%\include\vtk-6.1"
set "VTK_LIB=%VTK_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "VTK_BIN=%VTK_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for Qt
rem ===========================================================================

if "%QTDIR%" == "" set "QTDIR=%AS_PRODUCTS%\qt-4.7.4-20140912"
set "QT_INC=%QTDIR%\include;%QTDIR%\include\QtGui;%QTDIR%\include\QtCore"
set "QT_LIB=%QTDIR%\lib"
set "QT_BIN=%QTDIR%\bin"

rem ===========================================================================
rem Adjust PATH variable
rem ===========================================================================

set "PATH=%AS_BIN%;%AD_BIN%;%VTK_BIN%;%QT_BIN%;%PATH%"

::echo PATH=%PATH%

rem ===========================================================================
rem Customize OCCT environment variables
rem ===========================================================================

set "CSF_PluginDefaults=%SPE_RESOURCES%"
set "CSF_ResourcesDefaults=%SPE_RESOURCES%"

rem ===========================================================================
rem Hint echoes
rem ===========================================================================

echo ...
echo ARCH         = %ARCH%
echo VCVER        = %VCVER%
echo BUILD_CONFIG = %BUILD_CONFIG%
echo AS_ROOT      = %AS_ROOT%
echo AS_PRODUCTS  = %AS_PRODUCTS%
echo CASROOT      = %CASROOT%
echo CASDEB       = %CASDEB%
echo QT_INC       = %QT_INC%
echo ...
