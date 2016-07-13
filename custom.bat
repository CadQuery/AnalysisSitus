@echo off

REM ===========================================================================
REM Ecosystem
REM ===========================================================================

set "CASROOT=C:\Work\cadpro\win\occt-vc12-64"
set "CR_ROOT=C:\Work\cadpro\win\occ_cr-vc12-64"
set "AD_ROOT=C:\Work\cadpro\win\active_data-vc12-64"
set "FEAT_ROOT=C:\Work\cadpro\dmu-build"
set "FEAT_INC=C:\Work\cadpro\dmu\src\feature"

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
set "THIS_DATA=D:\as1.dmu"

set "PATH=C:\Work\cadpro\win\meshgems-vc12-64\bin;%PATH%"
