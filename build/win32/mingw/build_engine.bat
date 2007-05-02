:: User would need to edit to put the path to their Scons installation
set _=%CD%
SET PATH=%Path%;%_%\Scons;%_%\MinGW\bin

:: User would need to edit to point to where they have it stored
cd ..\..\..\
scons debug=0

:: Copy the important binaries to the bin directory
copy engine\fife_engine.exe %_%\bin\fife_engine.exe

:: Return us to the directory we started from
cd %_%