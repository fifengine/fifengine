:: Users will need to change paths if they use custom MinGW or SCons version that didn't ship with the SDK
set _=%CD%
SET PATH=%Path%;%_%\applications\scons;%_%\applications\mingw\bin

:: Goto TRUNK and call SCons
cd \
cd %_%\..\..\..\
scons debug=0

:: Return us to the directory we started from
cd %_%