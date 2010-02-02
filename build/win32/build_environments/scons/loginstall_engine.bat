@echo off

:: run scons
python ..\..\applications\scons\scons.py -C ..\..\..\..\ install-python install-dlls > engine_build_log.txt 2>&1
