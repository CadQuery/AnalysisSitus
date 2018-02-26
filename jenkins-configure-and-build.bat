@echo off

REM ===========================================================================
REM Configure environment on Jenkins machine
REM ===========================================================================

if exist %~dp0jenkins-custom.bat call %~dp0jenkins-custom.bat

REM ===========================================================================
REM Copy and unpack 3-rd parties
REM ===========================================================================

mkdir 3rd-parties-arc

copy %JENKINS_3RDPARTIES_ARCHIVE_DIR%\%JENKINS_3RDPARTIES_ARCHIVE% 3rd-parties-arc
copy %JENKINS_3RDPARTIES_ARCHIVE_DIR%\%JENKINS_3RDPARTIES_FBX_ARCHIVE% 3rd-parties-arc
copy %JENKINS_3RDPARTIES_ARCHIVE_DIR%\7z.dll 3rd-parties-arc
copy %JENKINS_3RDPARTIES_ARCHIVE_DIR%\7z.exe 3rd-parties-arc
copy jenkins-install-products.bat 3rd-parties-arc

cd 3rd-parties-arc

call "jenkins-install-products.bat" ..\3rd-parties

cd ..

set "JENKINS_3RDPARTIES=%~dp03rd-parties"

REM ===========================================================================
REM Configure environment
REM ===========================================================================

set "JENKINS_JOB_DIR=%~dp0"
echo JENKINS_JOB_DIR: %JENKINS_JOB_DIR%

REM ===========================================================================
REM Prepare build and install directories for CMake
REM ===========================================================================

echo Create cmake-build-dir and cmake-install-dir

mkdir cmake-build-dir
mkdir cmake-install-dir

REM ===========================================================================
REM Run CMake from build directory: configure
REM ===========================================================================

cd cmake-build-dir

"%JENKINS_CMAKE_BIN%\cmake.exe" -G"Visual Studio 12 2013 Win64" -DCMAKE_BUILD_TYPE=Release -DUSE_CONSOLE=false -D3RDPARTY_DIR:PATH=%JENKINS_3RDPARTIES% -DINSTALL_DIR:PATH=../cmake-install-dir ../

REM ===========================================================================
REM CMake build and install
REM ===========================================================================

"%JENKINS_CMAKE_BIN%\cmake.exe" --build . --config Release --target INSTALL

REM ===========================================================================
REM Prepare installer
REM ===========================================================================

cd ..

xcopy /S /Y cmake-build-dir\setup .\setup

"%JENKINS_3RDPARTIES%\innosetup5\ISCC.exe" .\setup\setup.iss
