:: Users will need to change paths if they use custom MinGW, SCons, Python or SWIG versions that didn't ship with the SDK
SET _=%CD%
SET PATH=%Path%;%_%\applications\scons;

:: Delete doxygen_log.txt in case that it's still in place from a former compile sessions
del %_%\doxygen_log.txt

:: Goto TRUNK and call SCons
cd \
cd %_%\..\..
scons docs=1 >> "%_%\doxygen_log.txt" 2>&1

:: Return us to the directory we started from
cd %_%
