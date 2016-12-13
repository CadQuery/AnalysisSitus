@echo off

call %~dp0env.bat vc12 win64 debug

start exe.exe
