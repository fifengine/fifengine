:: Users will need to change paths if they use custom MinGW or SCons version that didn't ship with the SDK
set _=%CD%
SET PATH=%Path%;%_%\applications\scons;%_%\applications\mingw\bin

:: Delete engine_build_log.txt in case that it's still in place from a former compile sessions
del %_%\engine_build_log.txt

:: Goto TRUNK and call SCons
cd \
cd %_%\..\..\..
scons debug=0 movie=1 >> "%_%\engine_build_log.txt" 2>&1

:: Return us to the directory we started from
cd %_%
