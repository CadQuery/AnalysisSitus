@echo off

set CONFIG=%1
set REBUILD=%2

if "%CONFIG%" == "" set CONFIG=release
if "%REBUILD%" == "" set REBUILD=build

call %~dp0env.bat vc10 win64 %CONFIG%

if "%VCVER%" == "vc10" (
  call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x64

  echo ----------------------------------------------------------------------
  echo Analysis Situs: console build
  echo ----------------------------------------------------------------------
  echo Configuration: %CONFIG%
  echo Rebuilding mode: %REBUILD%
  echo ----------------------------------------------------------------------

  MSBuild.exe msvc\vc10\AnalysisSitus.sln /p:Configuration=%CONFIG%;Platform=x64 /t:%REBUILD% /nologo /maxcpucount

  echo Building process finished. Press any key...
  pause > nul
) else (
  echo "Unsupported MS Visual Studio! Only MS VS 2010 can be used for building A-Situs"
  pause > nul
)
