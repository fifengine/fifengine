:: User would need to edit to put the path to their Scons installation
set _=%CD%
SET PATH=%Path%;%_%\applications\scons;%_%\applications\mingw\bin

:: User would need to edit to point to where they have it stored
cd ..\..\..\
scons -c

:: Return us to the directory we started from
cd %_%
