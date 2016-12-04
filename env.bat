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
if "%CASROOT%"         == "" set "CASROOT=%AS_PRODUCTS%\occt"
if "%OCCT_3RDPARTIES%" == "" set "OCCT_3RDPARTIES=%AS_PRODUCTS%"

set "CAS_INC=%CASROOT%\inc"
set "CAS_LIB64=%CASROOT%\win64\%VCVER%\lib%DEB%
set "CAS_BIN64=%CASROOT%\win64\%VCVER%\bin%DEB%

call %CASROOT%\env.bat %VCVER% %ARCH% %BUILD_CONFIG%

rem ===========================================================================
rem Setting environment for AD (Active Data)
rem ===========================================================================

if "%AD_ROOT%" == "" set "AD_ROOT=%AS_PRODUCTS%\active_data"
set "AD_INC=%AD_ROOT%\include"
set "AD_LIB=%AD_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "AD_BIN=%AD_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for Feature / Mesh libraries
rem ===========================================================================

if "%FEAT_ROOT%" == "" set "FEAT_ROOT=%AS_PRODUCTS%\feature"
if "%FEAT_INC%"  == "" set "FEAT_INC=%FEAT_ROOT%\include"
if "%FEAT_LIB%"  == "" set "FEAT_LIB=%FEAT_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
if "%FEAT_BIN%"  == "" set "FEAT_BIN=%FEAT_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

if "%MESH_ROOT%" == "" set "MESH_ROOT=%AS_PRODUCTS%\mesh"
if "%MESH_INC%"  == "" set "MESH_INC=%MESH_ROOT%\include"
if "%MESH_LIB%"  == "" set "MESH_LIB=%MESH_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
if "%MESH_BIN%"  == "" set "MESH_BIN=%MESH_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for MeshGems
rem ===========================================================================

if "%MESHGEMS_BIN%" == "" set "MESHGEMS_BIN=%AS_PRODUCTS%\meshgems-vc12-64\bin"

rem ===========================================================================
rem Setting environment for Canonical Recognition library
rem ===========================================================================

if "%CR_ROOT%" == "" set "CR_ROOT=%AS_PRODUCTS%\occ_cr-vc12-64"
if "%CR_INC%"  == "" set "CR_INC=%CR_ROOT%\inc"
if "%CR_LIB%"  == "" set "CR_LIB=%CR_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
if "%CR_BIN%"  == "" set "CR_BIN=%CR_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for QR (Quaoar)
rem ===========================================================================

if "%QR_ROOT%" == "" set "QR_ROOT=%AS_PRODUCTS%\quaoar"
set "QR_INC=%QR_ROOT%\include"
set "QR_LIB=%QR_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "QR_BIN=%QR_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for SPE
rem ===========================================================================

if "%SPE_ROOT%" == "" set "SPE_ROOT=%AS_PRODUCTS%\spe"
set "SPE_INC=%SPE_ROOT%\include"
set "SPE_LIB=%SPE_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "SPE_BIN=%SPE_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting TBB environment 
rem ===========================================================================

if "%TBB_ROOT%" == "" set "TBB_ROOT=%AS_PRODUCTS%\tbb44_20150728oss"
set "TBB_INC=%TBB_ROOT%\include\tbb"
set "TBB_LIB=%TBB_ROOT%\lib\intel64\vc12"
set "TBB_BIN=%TBB_ROOT%\bin\intel64\vc12"

rem ===========================================================================
rem Setting environment for VTK
rem ===========================================================================

if "%VTK_ROOT%" == "" set "VTK_ROOT=%AS_PRODUCTS%\vtk-7.0.0"
set "VTK_INC=%VTK_ROOT%\include\vtk-7.0"
set "VTK_LIB=%VTK_ROOT%\win%ARCH%\%VCVER%\lib%DEB%"
set "VTK_BIN=%VTK_ROOT%\win%ARCH%\%VCVER%\bin%DEB%"

rem ===========================================================================
rem Setting environment for PCL
rem ===========================================================================

if "%PCL_ROOT%" == "" set "PCL_ROOT=D:\projects\nnnp\pcl1.8.0"
set "PCL_INC=%PCL_ROOT%\include\pcl-1.8"
set "PCL_LIB=%PCL_ROOT%\lib"
set "PCL_BIN=%PCL_ROOT%\bin"

rem ===========================================================================
rem Setting environment for Qt
rem ===========================================================================

if "%QTDIR%" == "" set "QTDIR=%AS_PRODUCTS%\qt-5.5.1
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
rem Setting Boost environment
rem ===========================================================================

if "%BOOST_ROOT%" == "" set "BOOST_ROOT=D:\projects\nnnp\pcl1.8.0\3rdParty\Boost"

set "BOOST_INC=%BOOST_ROOT%/include"

rem ===========================================================================
rem Setting FLANN environment
rem ===========================================================================

if "%FLANN_ROOT%" == "" set "FLANN_ROOT=%AS_PRODUCTS%\flann-1.8.4-vc12-64"

set "FLANN_INC=%FLANN_ROOT%\include"
set "FLANN_LIB=%FLANN_ROOT%\lib%DEB%"
set "FLANN_BIN=%FLANN_ROOT%\bin%DEB%"

rem ===========================================================================
rem Setting HDF5 environment
rem ===========================================================================

if "%HDF5_ROOT%" == "" set "HDF5_ROOT=%AS_PRODUCTS%\hdf5"

set "HDF5_INC=%HDF5_ROOT%\include"
set "HDF5_LIB=%HDF5_ROOT%\lib"
set "HDF5_BIN=%HDF5_ROOT%\bin"

rem ===========================================================================
rem Setting environment for Inno Setup
rem ===========================================================================

if "%INNOSETUP_ROOT%" == "" set "INNOSETUP_ROOT=%AS_PRODUCTS%\issrc-5.5.6"

rem ===========================================================================
rem Adjust PATH variable
rem ===========================================================================

set "PATH=%AS_BIN%;%CAS_BIN64%;%FREETYPE_BIN%;%MESHGEMS_BIN%;%SPE_BIN%;%AD_BIN%;%FEAT_BIN%;%MESH_BIN%;%QR_BIN%;%VTK_BIN%;%PCL_BIN%;%QT_BIN%;%TBB_BIN%;%CR_BIN%;%INNOSETUP_ROOT%;%PATH%"

rem ===========================================================================
rem Customize OCCT environment variables
rem ===========================================================================

rem if "%CSF_PluginDefaults%" == "" set "CSF_PluginDefaults=%AS_RESOURCES%"
rem if "%CSF_ResourcesDefaults%" == "" set "CSF_ResourcesDefaults=%AS_RESOURCES%"

set "CSF_PluginDefaults=C:\Work\dMU\dmu\src\core\resources"
set "CSF_ResourcesDefaults=C:\Work\dMU\dmu\src\core\resources"

rem ===========================================================================
rem Hint echoes
rem ===========================================================================

echo ...
echo ARCH                  = %ARCH%
echo VCVER                 = %VCVER%
echo BUILD_CONFIG          = %BUILD_CONFIG%
echo AS_ROOT               = %AS_ROOT%
echo AS_PRODUCTS           = %AS_PRODUCTS%
echo CASROOT               = %CASROOT%
echo CAS_BIN64             = %CAS_BIN64%
echo CASDEB                = %CASDEB%
echo CSF_PluginDefaults    = %CSF_PluginDefaults% 
echo CSF_ResourcesDefaults = %CSF_ResourcesDefaults% 
echo PATH                  = %PATH% 
echo FLANN_ROOT            = %FLANN_ROOT%
echo ...
