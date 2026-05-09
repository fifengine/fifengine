@echo off
setlocal

REM Determine script directory and repository root
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%..\"
set "REPO_ROOT=%CD%\"
popd

REM call vcvars64.bat
set "VCVARS=C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
if exist "%VCVARS%" (
    echo Calling "%VCVARS%"
    call "%VCVARS%"
) else (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    if exist "%VCVARS%" (
        echo Calling "%VCVARS%"
        call "%VCVARS%"
    ) else (
        echo Could not find vcvars64.bat in known locations.
        exit /b 1
    )
)

REM Build dependencies
call "%REPO_ROOT%dependencies\build.bat"

REM Ensure output folder exists
set "OUT_DIR=%REPO_ROOT%out\fife-wheels"
if not exist "%OUT_DIR%" (
    mkdir "%OUT_DIR%"
)

REM Build wheel
pushd "%REPO_ROOT%"
py -3 -m pip wheel . -w "%OUT_DIR%" ^
 -Ccmake.define.BUILD_SHARED_LIBS=OFF ^
 -Ccmake.define.BUILD_TESTS=OFF
if errorlevel 1 (
    echo pip wheel failed
    popd
    exit /b 1
)
popd

echo Build completed successfully.

echo Listing contents of "%OUT_DIR%":
dir "%OUT_DIR%"

endlocal
