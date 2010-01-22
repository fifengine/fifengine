@echo off

:: run scons
python ..\..\applications\scons\scons.py -C ..\..\..\..\ fife-python

:: copy the needed dlls to the path where fife was installed
call copy_dlls.bat
