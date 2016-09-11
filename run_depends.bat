@echo off

set CONFIG=%1

if "%CONFIG%" == "" set CONFIG=release

call %~dp0env.bat vc10 win64 %CONFIG%

rem start %DEPENDS_BIN%\depends.exe %EZY_BIN%\EzyApp.dll

start %DEPENDS_BIN%\depends.exe C:\Work\analysis_situs\win64\vc12\bin\AnalysisSitus.exe

