:: Users will need to change paths if they use custom MinGW, SCons, Python or SWIG versions that didn't ship with the SDK
SET _=%CD%
SET _SWIG="%_%\..\..\applications\swigwin-1.3.40"

:: To avoid path collisions the following line was commented out and replaced with a slightly different version
:: SET PATH=%Path%;%_%\applications\scons;%_%\applications\mingw\bin
SET PATH=%_%\..\..\applications\scons;%_%\..\..\applications\mingw\bin

:: Delete engine_build_log.txt in case that it's still in place from a former compile sessions
del "%_%\engine_build_log.txt"

:: Goto TRUNK and call SCons
cd \
cd "%_%\..\..\..\.."
scons debug=0 log=0 >> "%_%\engine_build_log.txt" 2>&1

:: Delete old versions of _fife.pyd and rename the compiled fife.dll to _fife.pyd
cd "%_%\..\..\..\..\engine\swigwrappers\python"
del _fife.pyd
move "..\..\fife.dll" .\_fife.pyd

:: Return us to the directory we started from
cd %_%
