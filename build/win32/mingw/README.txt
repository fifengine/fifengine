==========================
FIFE Win32 compile guide for MinGW
==========================

Installation / build process
==========================
1. Check out the latest engine sources from SVN:
Repository URL: https://svn1.cvsdude.com/fife/engine/trunk

2. Get the latest compile SDK for mingw from the FIFE wiki:
http://wiki.fifengine.de/index.php?title=Download_section#Compile_SDKs

3. Extraction process
3.1 Extract the content of the compile SDK into:
FIFE_SVN_DIR\
3.2 Check if there is an FIFE_SVN_DIR\build\win32\mingw\applications folder; if it's in place, you know that you extracted the SDK into the correct location. One possible issue is that you've checked out the whole engine SVN repository instead of just trunk. In this case extract the compile SDK to:
FIFE_SVN_DIR\trunk\

4. Build FIFE by going to FIFE_SVN_DIR\build\win32\mingw\ and starting:
4.1 build_engine.bat if you want to build the engine.
4.2 cleanup_engine.bat if you want to clean up the files that have been created during the build process.
4.3 logbuild_engine.bat if you want to log the whole build process (a file called "engine_build_log.txt" will be created)

5. The final binary (fife_engine.exe) is automatically build too:
FIFE_SVN_DIR\

6. Copy FIFE_SVN_DIR\build\win32\mingw\bin\*.* to FIFE_SVN_DIR\*.* (this ensures that all required DLLs are in place to work with FIFE)

7. You can now start your custom FIFE build via FIFE.bat
