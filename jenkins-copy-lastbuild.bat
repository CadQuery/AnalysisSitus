@echo off

REM ===========================================================================
REM Configure environment on Jenkins machine
REM ===========================================================================

if exist %~dp0jenkins-custom.bat call %~dp0jenkins-custom.bat

REM ===========================================================================
REM Copy to shared team dir
REM ===========================================================================

echo date=%date%

REM timestamp without spaces
for /F "tokens=1,2,3 delims= " %%i in ("%date%") do set da_nospaces=%%j
echo da_nospaces=%da%

REM timestamp without slashes
for /F "tokens=1,2,3 delims=/" %%i in ("%da_nospaces%") do set da_nospaces_noslashes=%%k-%%i-%%j
echo da_nospaces_noslashes=%da_nospaces_noslashes%

REM timestamp with time
for /F "usebackq tokens=1,2,3 delims=: " %%i in (`time /T`) do set timestamp=%da_nospaces_noslashes%-T%%i%%j
echo timestamp=%timestamp%

set "TEAMDIR=%JENKINS_LAST_BUILD_DIR%\%timestamp%"
if not exist %TEAMDIR% md %TEAMDIR%
xcopy /Y cmake-install-dir\setup %TEAMDIR%

REM ===========================================================================
REM Copy to shared publix dir
REM ===========================================================================

xcopy /Y cmake-install-dir\setup %JENKINS_PUBLIC_DIR%
