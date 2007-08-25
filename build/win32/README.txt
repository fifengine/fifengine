==========================
FIFE Win32 compile guide for the upcoming 2007.2 release
==========================

Installation / build process
==========================
1. Check out the latest engine sources from SVN:
Repository URL: https://svn1.cvsdude.com/fife/engine/trunk

2.1 Get the latest compile SDK for Win32 from the FIFE wiki:
http://wiki.fifengine.de/index.php?title=Download_section#Compile_SDKs
2.2 Get ActivePython 2.5 and install it to a location of your choice:
http://downloads.activestate.com/ActivePython/windows/2.5/ActivePython-2.5.1.1-win32-x86.msi

3. Extraction process
3.1 Extract the content of the compile SDK to:
FIFE_SVN_DIR\build\win32\
3.2 Check if there is an FIFE_SVN_DIR\build\win32\applications folder; if it's in place, you know that you extracted the SDK into the correct location. One possible issue is that you've checked out the whole engine SVN repository instead of just trunk. In this case extract the compile SDK to:
FIFE_SVN_DIR\trunk\ (if you want to compile trunk, otherwhise it needs to be extracted to the branch that you want to compile)

4. Build files for each supported IDE / compiler can be found in the sub-directories of FIFE_SVN_DIR\build\win32\ (codeblocks, mingw, msvc2005)

5. The final dynamic library (_fife.pyd) is automatically moved to:
FIFE_SVN_DIR\engine

6. Depending on the IDE / compiler you're using copy FIFE_SVN_DIR\build\win32\mingw\bin\*.* (for code::blocks, mingw) or FIFE_SVN_DIR\build\win32\msvc2005\bin\*.* (MSVC 2005) to FIFE_SVN_DIR\*.* (this ensures that all required DLLs and batch files are in place to work with FIFE)

7. You can now start your custom FIFE build via pyFIFE.bat
