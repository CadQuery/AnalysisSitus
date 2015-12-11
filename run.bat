@echo off

set CONFIG=%1

if "%CONFIG%" == "" set CONFIG=release

call %~dp0env.bat vc10 win64 %CONFIG%

start AnalysisSitus.exe
