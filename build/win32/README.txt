==========================
FIFE Win32 compile guide for building the 2009.0 release version
==========================

Installation / build process
==========================
1. Check out the latest engine sources from SVN:
Repository URL: http://fife.svn.cvsdude.com/engine/trunk

2.1 Get the latest compile SDK for Win32 from the FIFE wiki:
http://wiki.fifengine.de/index.php?title=Download_section#Compile_SDKs
2.2 Get ActivePython 2.5 and install it to a location of your choice:
http://downloads.activestate.com/ActivePython/windows/2.5/ActivePython-2.5.1.1-win32-x86.msi

3. Extraction process
3.1 Extract the content of the compile SDK to:
<FIFE>\build\win32\
3.2 Check if there is an <FIFE>\build\win32\applications folder; if it's in place, you know that you extracted the SDK into the correct location. One possible issue is that you've checked out the whole engine SVN repository instead of just trunk. In this case extract the compile SDK to:
<FIFE>\trunk\build\win32 (if you want to compile trunk, otherwhise it needs to be extracted into the build\win32 directory of the branch that you want to compile)


4.1 Run <FIFE>\trunk\build\win32\update_project_files.bat.
4.2 Build files for each supported IDE can be found in the sub-directories of <FIFE>\build\win32\build_environments (code_blocks, scons, visual_studio_8, visual_studio_9).

5. The final dynamic library (_fife.pyd) is automatically moved to:
<FIFE>\engine

6. Depending on the IDE you have utilized for building FIFE you either need to run <FIFE>\build\win32\update_dlls_mingw.bat (in case you built FIFE with code::blocks or scons), <FIFE>\build\win32\update_dlls_msvc2005.bat (Visual Studio 2005) or <FIFE>\build\win32\update_dlls_msvc2008.bat (Visual Studio 2008).

7. You can now start your custom FIFE build by launching the python scripts that reside in the subdirectories of <FIFE>\clients.
