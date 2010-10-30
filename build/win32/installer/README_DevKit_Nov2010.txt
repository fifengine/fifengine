===============================
FIFE Win32 DevKit November 2010
===============================


Notes:
===============================
* Building with Code::Blocks in combination with MinGW is currently broken. This might be resolved by a future SVN commit that fixes the FIFE Code::Blocks build script.
* Building FIFE with SCons in combination with MinGW will only work in release mode! In case you want to debug FIFE, you'll have to build FIFE in release mode as well!
** Here's the catch: to get it working with FIFE built in debug mode, we would have to build Python and all of its libraries from source as there is no precompiled debug package of Python that works with MinGW. Gdb should provide somewhat useful backtraces even when FIFE is built in release mode. If that doesn't work out well due the missing debugging symbols, you still have the choice to use the MSVC debugger or to debug on a Linux system, where building FIFE in debug mode works fine. Sorry for the trouble but there isn't much we can do until there is an easy way to build Python from source via MinGW or (semi-)official precompiled debug packages of Python for Win32 are available.
* Debugging finally works again with MSVC 2005 & MSVC 2008! Yay!
* FIFE will still only work in combination with a 32bit version of Python on Windows systems.
** To provide support for 64bit Python versions, we would have to provide precompiled 64bit versions of all of FIFE's dependencies. That's a rather huge undertaking and considering the small demand for a 64bit Windows version of FIFE, we don't support it yet and prolly won't support it in the near future.


Changes:
===============================
* Custom built SDL_d.dll for MSVC 2005 & MSVC 2008 to fix debugging support for these compilers
* Updated mingw to 4.5.1 sjlj from http://tdm-gcc.tdragon.net/download
* Updated Python to 2.7
** Python 2.7 is now the only officially supported version that is guaranteed to work with FIFE on Microsoft Windows systems!
** If you want to use an older Python version with FIFE, you can do so at your own risk by copying the correct headers and libs into the corresponding directories and by editing the build scripts for your compiler of choice:
*** build\win32\win32-config.py for mingw
*** build\win32\build_environments\visual_studio_8\engine_template.xml for MSVC 2005
*** build\win32\build_environments\visual_studio_9\engine_template.xml for MSVC 2008
* Updated scons to 2.0.1
* Updated SWIG to 2.0.1


Installation / build process
===============================
1. Check out the latest engine sources from SVN or update your SVN checkout in case you have already checked out from SVN before:
Repository URL: http://fife.svn.cvsdude.com/engine/trunk

2. Get ActivePython 2.7 (x86 version) and install it to a location of your choice:
http://downloads.activestate.com/ActivePython/releases/2.7.0.2/ActivePython-2.7.0.2-win32-x86.msi

3. In case you've built FIFE with an older compile SDK in the past, either run the uninstaller or run <FIFE>\build\win32\delete_sdk.bat to get rid of the old SDK files

4. Extraction process
4.1 Extract the content of the compile SDK to:
<FIFE>\build\win32\
4.2 Check if there is an <FIFE>\build\win32\applications folder; if it's in place, you know that you extracted the SDK into the correct location. One possible issue is that you've checked out the whole engine SVN repository instead of just trunk. In this case extract the compile SDK to:
<FIFE>\trunk\build\win32 (if you want to compile trunk, otherwhise it needs to be extracted into the build\win32 directory of the branch that you want to compile)


5.1 Run <FIFE>\trunk\build\win32\update_project_files.bat.
5.2 Build files for each supported IDE can be found in the sub-directories of <FIFE>\build\win32\build_environments (code_blocks, scons, visual_studio_8, visual_studio_9). See http://wiki.fifengine.de/Guides_%26_tutorials#Win32 for more information.

6. The final dynamic library (_fife.pyd) is automatically moved to:
<FIFE>\engine\python\fife

7. You can now start your custom FIFE build by launching the python scripts that reside in the subdirectories of <FIFE>\demos
