@echo off

set configuration=Release
set cmake_generator="Visual Studio 15 2017"

:: init env for VS2017
::call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"

:: build Visual Studio Project
cmake ../dependencies -G %cmake_generator% -B../../fifengine-dependencies/build -DCMAKE_BUILD_TYPE=%configuration% 

:: build Dependencies
cmake --build ../../fifengine-dependencies/build --target ALL_BUILD --config %configuration% -- /verbosity:minimal /maxcpucount

echo Done.

pause