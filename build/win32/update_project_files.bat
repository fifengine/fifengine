@echo off

SET _=%CD%

:: Delete old generated project files to ensure that they get generated from scratch again
cd %_%\build_environments
del code_blocks\fife.cbp
del visual_studio_8\fife.vcproj
del visual_studio_9\fife.vcproj

:: run scons
cd %_%
python applications\scons\scons.py -C ..\..\ projects

