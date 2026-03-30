@echo off
setlocal EnableExtensions

rem =============================================================================
rem This script is for building and installing the "non-vcpkg" FIFE dependencies.
rem Its for use with Windows environments.
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

rem Determine CMake command (Override via CMAKE_PATH env var)
set "CMAKE_CMD=f:\tools\cmake\bin\cmake.exe"
if defined CMAKE_PATH (
    set "CMAKE_CMD=%CMAKE_PATH%"
)

rem Set VCPKG path (Override via VCPKG_ROOT env var)
set "VCPKG_DIR=f:\tools\vcpkg"
if defined VCPKG_ROOT (
    set "VCPKG_DIR=%VCPKG_ROOT%"
)

echo Using CMake command: %CMAKE_CMD%

rem Configure
"%CMAKE_CMD%" -S "%SCRIPT_DIR%" ^
      -B "%SCRIPT_DIR%..\out\fife-dependencies\build" ^
      -G Ninja ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%/scripts/buildsystems/vcpkg.cmake ^
      -DVCPKG_TARGET_TRIPLET=x64-windows ^
      -DCMAKE_PREFIX_PATH=%VCPKG_DIR%/installed/x64-windows
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

rem Build
"%CMAKE_CMD%" --build "%SCRIPT_DIR%..\out\fife-dependencies\build"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

rem Verify Installation
dir "%SCRIPT_DIR%..\out\fife-dependencies\install\lib\cmake\fifechan\" | findstr "fifechan"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo Build completed successfully.
