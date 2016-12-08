@echo off

set CONFIG=%1

if "%CONFIG%" == "" set CONFIG=debug

call %~dp0env.bat vc12 win64 %CONFIG%

start exeAsBuilt.exe
