@echo off

set CONFIG=%1

if "%CONFIG%" == "" set CONFIG=release

call %~dp0env.bat vc12 win64 %CONFIG%

start %DEPENDS_BIN%\depends.exe %AS_BIN%\exe.exe

