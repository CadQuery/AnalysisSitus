@echo off

set CONFIG=%1

if "%CONFIG%" == "" set CONFIG=release

call %~dp0env.bat vc10 win64 %CONFIG%

rem start %DEPENDS_BIN%\depends.exe %EZY_BIN%\EzyApp.dll

start %DEPENDS_BIN%\depends.exe D:\projects\analysis_situs\win64\vc10\bind\AnalysisSitus.exe
rem start %DEPENDS_BIN%\depends.exe D:\projects\analysis_situs\products\occt\win64\vc10\bind\TKernel.dll
