@echo off

:: run scons
python ..\..\applications\scons\scons.py -C ..\..\..\..\ install-python > engine_build_log.txt 2>&1
