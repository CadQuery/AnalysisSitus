@echo off

if "%1" == "" (
  echo This script unpacks Analysis Situs pre-requisites to the specified installation directory.
  echo
  echo Usage:
  echo install_products.bat full_path_to_installation_directory
  echo
  exit 0
) 

set ARCH_DIR=%~dp0
set TARGET_DIR=%1

set CURDIR=%CD%
cd /d %ARCH_DIR%

if not exist 7z.exe (
  echo
  echo 7z.exe is not found in the archive directory %ARCH_DIR%. Cannot proceed.
  echo
  exit 1
)

echo Installing Analysis Situs pre-requisites to %TARGET_DIR%

echo Installing Analysis Situs pre-requisites to %TARGET_DIR% > install_products.log
echo ==================================================================== >> install_products.log

if exist %TARGET_DIR%\* (
  echo Installation directory %TARGET_DIR% already exists and is not empty, cleaning its contents >> install_products.log
  rd %TARGET_DIR% /s /q >> install_products.log
  if errorlevel 1 goto error
  md %TARGET_DIR% >> install_products.log
  if errorlevel 1 goto error
)
if not exist %TARGET_DIR% (
  echo Creating installation directory %TARGET_DIR% >> install_products.log
  md %TARGET_DIR% >> install_products.log
  if errorlevel 1 goto error 
)

setlocal enabledelayedexpansion  
for %%a in (*.zip) do (
  set arc_name=%%~na

  echo arc_name = !arc_name!
  echo Installing %%a... >> install_products.log

  7z.exe x -aoa -o%TARGET_DIR% !arc_name!.zip >> install_products.log
  if errorlevel 1 goto error
)
endlocal

echo ==================================================================== >> install_products.log
echo Installation completed >> install_products.log
echo Installation completed, see %ARCH_DIR%install_products.log for details
goto end:

:error
echo ==================================================================== >> install_products.log
echo Installation failed >> install_products.log
echo Installation failed, see %ARCH_DIR%install_products.log for details

:end
cd /d %CURDIR%
