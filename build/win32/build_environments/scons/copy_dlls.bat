@echo off

if not defined xcopy goto :noxcopy
xcopy "..\..\binaries\mingw\*.dll" "..\..\..\..\engine\python\fife" /e /c /i /q /y
if "%1" == "install" xcopy "..\..\binaries\mingw\*.dll" "C:\Python27\Lib\site-packages\fife" /e /c /i /q /y

:noxcopy
%SystemRoot%\system32\xcopy "..\..\binaries\mingw\*.dll" "..\..\..\..\engine\python\fife" /e /c /i /q /y
if "%1" == "install" %SystemRoot%\system32\xcopy "..\..\binaries\mingw\*.dll" "C:\Python27\Lib\site-packages\fife" /e /c /i /q /y
