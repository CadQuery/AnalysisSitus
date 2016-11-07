@echo off

echo ===========================================================================
echo Set environment

call %~dp0env.bat vc10 win64 release

echo ===========================================================================
echo Delivery production

if "%DELIVERY_DIR%" == "" set DELIVERY_DIR=%AS_ROOT%

FOR /F "usebackq tokens=3" %%A IN ( `FINDSTR /R ASitus_Version_STRING %AS_ROOT%\src\exe\exe_Version.h` ) do (
  set "ASITUS_VERSION_STRING=%%~A"
)

if /I "%ASITUS_VERSION_STRING%" == "" (
  echo Error: software version is not defined.
  goto END
)

echo Analysis Situs v%ASITUS_VERSION_STRING%
echo ---------------------------------------------------------------------------
echo App      : %AS_ROOT%
echo Products : %AS_PRODUCTS%
echo Output   : %DELIVERY_DIR%
echo ---------------------------------------------------------------------------

echo [Inno Setup] Packaging...
if exist "%AS_TOOLS%\innosetup" (
  iscc.exe /O"%DELIVERY_DIR%" "/DMyAppVersion=%ASITUS_VERSION_STRING%" ^
             "/DASitusDir=%AS_ROOT%" "/DProductsDir=%AS_PRODUCTS%" ^
             "%AS_TOOLS%\innosetup\asitus-min_install.iss" > delivery.log 2>&1
)
echo [Inno Setup] Packaging done

echo Packaging finished. Check delivery.log for details...
pause > nul

:END
echo ===========================================================================
