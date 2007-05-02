==========================
FIFE Win32 compile guide for MinGW
==========================

Installation / build process
==========================
1. Check out the latest engine sources from SVN:
svn co https://svn1.cvsdude.com/fife/engine/trunk

2. Get the latest compile SDK for mingw from the FIFE wiki:
http://wiki.fifengine.de/index.php?title=Download_section#Compile_SDKs

3. Extract the content of the compile SDK into:
FIFE_SVN_DIR\trunk\build\win32\mingw\

4. Build FIFE by starting:
4.1 build_engine.bat if you want to build the engine.
4.2 cleanup_engine.bat if you want to clean up the files that have been created during the build process.
4.3 logbuild_engine.bat if you want to log the whole build process (a file called "engine_build_log.txt" will be created)

5. The final binary (fife_engine.exe) can be found in this directory:
FIFE_SVN_DIR\trunk\build\win32\mingw\bin
