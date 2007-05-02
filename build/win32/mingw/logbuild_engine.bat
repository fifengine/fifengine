:: User would need to edit to put the path to their Scons installation
set _=%CD%
SET PATH=%Path%;%_%\Scons;%_%\MinGW\bin

del %_%\engine_build_log.txt

:: User would need to edit to point to where they have it stored
cd \
cd %_%\..\..\..
scons debug=0 >> "%_%\engine_build_log.txt" 2>&1

:: Copy the important binaries to the bin directory
copy engine\fife_engine.exe %_%\bin\fife_engine.exe

:: Return us to the directory we started from
cd %_%
