:: Users will need to change paths if they use custom MinGW, SCons, Python or SWIG versions that didn't ship with the SDK
SET _=%CD%
SET _SWIG="%_%\applications\swigwin-1.3.40"

:: To avoid path collisions the following line was commented out and replaced with a slightly different version
:: SET PATH=%Path%;%_%\applications\scons;%_%\applications\mingw\bin
SET PATH=%_%\applications\scons;%_%\applications\mingw\bin

:: Delete old generated project files to ensure that they get generated from scratch again
cd %_%\build_environments
del code_blocks\fife_engine.cbp
del visual_studio_8\fife.vcproj
del visual_studio_9\fife.vcproj

:: Goto TRUNK and call SCons
cd \
cd %_%\..\..\
scons projectfiles_only=1

:: Return us to the directory we started from
cd %_%
