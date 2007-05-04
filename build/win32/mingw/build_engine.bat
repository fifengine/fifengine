:: User would need to edit to put the path to their Scons installation
set _=%CD%
SET PATH=%Path%;%_%\Scons;%_%\MinGW\bin

:: User would need to edit to point to where they have it stored
cd \
cd %_%\..\..\..\
scons debug=0

:: Return us to the directory we started from
cd %_%