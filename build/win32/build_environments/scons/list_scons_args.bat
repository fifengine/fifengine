@echo off

:: run scons
python ..\..\applications\scons\scons.py -C ..\..\..\..\ -h > scons_args.txt 2>&1
