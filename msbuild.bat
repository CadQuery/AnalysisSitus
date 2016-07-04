@echo off

set CONFIG=%1
set REBUILD=%2

if "%CONFIG%" == "" set CONFIG=release
if "%REBUILD%" == "" set REBUILD=build

call %~dp0env.bat vc12 win64 %CONFIG%

if "%VCVER%" == "vc12" (
  call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x64

  echo ----------------------------------------------------------------------
  echo Analysis Situs: console build
  echo ----------------------------------------------------------------------
  echo Configuration: %CONFIG%
  echo Rebuilding mode: %REBUILD%
  echo ----------------------------------------------------------------------

  MSBuild.exe msvc\vc12\AnalysisSitus.sln /p:Configuration=%CONFIG%;Platform=x64 /t:%REBUILD% /nologo /maxcpucount

  echo Building process finished. Press any key...
  pause > nul
) else (
  echo "Unsupported MS Visual Studio! Only MS VS 2013 can be used for building A-Situs"
  pause > nul
)
