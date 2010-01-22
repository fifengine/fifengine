@echo off

:: run scons
python ..\..\applications\scons\scons.py -C ..\..\..\..\ --disable-log fife-python > engine_build_log.txt 2>&1

:: copy the needed dlls to the path where fife was installed
call copy_dlls.bat
