:: Users will need to change paths if they use custom MinGW or SCons version that didn't ship with the SDK
set _=%CD%
SET PATH=%Path%;%_%\applications\scons

:: Goto TRUNK and call SCons
cd \
cd %_%\..\..\..\
scons msvcproj=1 >> "%_%\msvc_update_log.txt" 2>&1

:: Return us to the directory we started from
cd %_%