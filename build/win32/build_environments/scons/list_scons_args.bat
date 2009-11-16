:: Users will need to change paths if they use custom MinGW, SCons, Python or SWIG versions that didn't ship with the SDK
SET _=%CD%
SET _SWIG="%_%\..\..\applications\swigwin-1.3.40"

:: To avoid path collisions the following line was commented out and replaced with a slightly different version
:: SET PATH=%Path%;%_%\applications\scons;%_%\applications\mingw\bin
SET PATH=%_%\..\..\applications\scons;%_%\..\..\applications\mingw\bin

del %_%\scons_args.txt

:: Goto TRUNK and call SCons
cd \
cd "%_%\..\..\..\.."
scons -h >> "%_%\scons_args.txt" 2>&1

:: Return us to the directory we started from
cd %_%
