@echo off

REM ===========================================================================
REM Ecosystem
REM ===========================================================================

set "CASROOT=C:\Work\OCC\master-install"
set "AD_ROOT=C:\Work\active_data"
set "QR_ROOT=C:\Work\quaoar"
set "FEAT_ROOT=C:\Work\dMU\dmu-build"
set "FEAT_INC=C:\Work\dMU\dmu\src\feature"

REM ===========================================================================
REM DEPENDS.EXE
REM ===========================================================================

set "DEPENDS_BIN=D:\OCC\utils\depends"

REM ===========================================================================
REM Delivery
REM ===========================================================================

set "DELIVERY_DIR=E:\projects\A-Situs\releases"

REM ===========================================================================
REM DF Browser
REM ===========================================================================

rem set "THIS_DATA=%~dp0data\xde\test.xbf"
rem set "THIS_DATA=D:\OCC\!Bugs\SALOME_STP27329\nmbox_nonmanifold0.xbf"
set "THIS_DATA=T:\as1_test.dmu"

set "PATH=D:\OCC\products\occt700products\meshgems-vc12-64\bin;%PATH%"
