@echo off

:: cleanup the includes folder

pushd %~dp0..\..\fifengine-dependencies\includes
rd share /s /q
move lib\x86\*.dll bin
move lib\x86\*.lib lib
rd lib\x86 /s /q
mkdir licenses
move lib\x86\*.txt licenses
popd

pushd %~dp0..\..\fifengine-dependencies\includes\lib
rd x64 /s /q
rd libpng /s /q
popd