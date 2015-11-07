@echo off

call %~dp0env.bat %1 %2 %3

echo Running Visual Studio...

if ["%VCVER%"] == ["vc10"] (
  start "%VS100COMNTOOLS%\..\IDE\devenv.exe" "msvc\vc10\AnalysisSitus.sln"
) else (
  echo "Unsupported version of MS Visual Studio"
  exit 1
)
