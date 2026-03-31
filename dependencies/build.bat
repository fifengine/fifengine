@echo off
setlocal EnableExtensions

rem =============================================================================
rem This script is for building and installing the "non-vcpkg" FIFE dependencies.
rem Its for use with Windows environments.
rem
rem Required setup:
rem - developer command prompt initalized (vcvarsall.bat or similar)
rem - with CMake and Ninja in PATH
rem - with vcpkg installed.
rem
rem CMAKE EXECUTABLE:
rem   By default, this script uses 'cmake' from your system PATH.
rem   To override this, set the CMAKE_PATH environment variable before running.
rem   Example: set CMAKE_PATH=C:\Program Files\CMake\bin\cmake.exe
rem
rem NOTE ON PATHS:
rem   The vcpkg paths you likely need to adjust these to match your Windows
rem   vcpkg installation (e.g., C:/vcpkg/) or set VCPKG_ROOT env variable.
rem =============================================================================

rem Get the directory where this script resides
set "SCRIPT_DIR=%~dp0"

rem Initialize Visual Studio Developer Command Prompt
if exist "C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\vsdevcmd.bat" (
    set "VSDEV_CMD=C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\vsdevcmd.bat"
)
if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\vsdevcmd.bat" (
    set "VSDEV_CMD=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\vsdevcmd.bat"
)
if defined VSDEV_CMD (
    call "%VSDEV_CMD%" -arch=amd64
) else (
    echo Error: Visual Studio Developer Command Prompt not found. Please ensure you have VS18 installed.
    exit /b 1
)

rem Determine CMake command (Override via CMAKE_PATH env var)
set "CMAKE_CMD=f:\tools\cmake\bin\cmake.exe"
if defined CMAKE_PATH (
    set "CMAKE_CMD=%CMAKE_PATH%"
)

rem Set VCPKG path (Override the VCPKG_ROOT env var)
set "VCPKG_ROOT=f:\tools\vcpkg"

call %VCPKG_ROOT%\vcpkg integrate install

echo Using CMake command: %CMAKE_CMD%

rem Set build and install directories
set "BUILD_DIR=%SCRIPT_DIR%..\out\fife-dependencies\x64-windows\build"
set "INSTALL_DIR=%SCRIPT_DIR%..\out\fife-dependencies\x64-windows\install"

rem Configure
"%CMAKE_CMD%" ^
      -S %SCRIPT_DIR% ^
      -B %BUILD_DIR% ^
      -G Ninja ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake ^
      -DVCPKG_TARGET_TRIPLET=x64-windows ^
      -DCMAKE_PREFIX_PATH=%VCPKG_ROOT%/installed/x64-windows ^
      -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR% ^
      -DFIFECHAN_BUILD_FROM_SOURCE=ON ^
      --fresh
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

rem Build
"%CMAKE_CMD%" --build %BUILD_DIR% --verbose
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

rem Install
"%CMAKE_CMD%" --install %BUILD_DIR% --verbose
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

rem Verify Installation
echo "Verifying installation in: %INSTALL_DIR%\lib\cmake\fifechan\"
if not exist "%INSTALL_DIR%\lib\cmake\fifechan\" (
    echo Error: Installation directory not found!
    exit /b 1
)
dir "%INSTALL_DIR%\lib\cmake\fifechan\" | findstr "fifechan"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo Build completed successfully.
