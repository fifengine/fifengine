__/\\\\\\\\\\\\\\\___/\\\\\\\\\\\___/\\\\\\\\\\\\\\\___/\\\\\\\\\\\\\\\_        
 _\/\\\///////////___\/////\\\///___\/\\\///////////___\/\\\///////////__       
  _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________      
   _\/\\\\\\\\\\\__________\/\\\______\/\\\\\\\\\\\______\/\\\\\\\\\\\_____     
    _\/\\\///////___________\/\\\______\/\\\///////_______\/\\\///////______    
     _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________   
      _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________  
       _\/\\\_______________/\\\\\\\\\\\__\/\\\______________\/\\\\\\\\\\\\\\\_ 
        _\///_______________\///////////___\///_______________\///////////////__

== Building, Installing and Running FIFE ==

=== Notes ===
We are finally able to provide install targets for most platforms.  There may be
a few minor issues left over to work out but for the most part the install 
targets work as advertized.


=== Dependencies ===
A list of all dependencies (including accurate version numbers) that you'll need
to obtain to build FIFE on your platform can be found at the project wiki:
 * https://github.com/fifengine/fifengine/wiki/dependencies


=== SCons Build System Notes ===
FIFE utilizes the SCons (http://www.scons.org) build system. Here is a quick 
list of scons commands that can be important for building FIFE on your platform:
 * 'scons -h' will list additional compile options.
 * 'scons -c' deletes any compiled objects (scons equivalent of 'make clean').
 * 'scons' will build FIFE (default target is fife-python).

Valid targets include:
  * ext : builds the external dependencies (only REQUIRED in Linux)
  * tests : builds unittests (needed for test_fife.py)
  * fife-shared : builds a shared library (DLL on Windows)
  * fife-static : builds a static library
  * fife-python : (default) builds a python module (uses swig)
  * fife-swig : build the swig python wrappers
  * fife : builds fife-shared, fife-static and fife-python
  * install-shared : installs the shared library
  * install-static : installs the static library
  * install-python : installs the python module and library
  * install-dev : installed the shared, static and header files
  * install-all : installs the shared, static and python libraries
  * projects : build msvc, msvc9 and code-blocks (win32 and linux) project 
    files

Build options:
  * --release : builds the release versions of the binaries
  * --disable-opengl : removes opengl support
  * --disable-zip : removes ZIP archive support
  * --disable-log : disable engine logging support
  * --enable-rend-camzone : enables camera zone renderer
  * --enable-rend-grid : enables camera grid renderer
  * --enable-profile : build with profiling information
  * --prefix : installation prefix for the shared and static libraries
  * --python-prefix : installation prefix for the python library
  * --lib-dir : installation directory for the libraries


== Platform specific details ==

=== Linux ===
Detailed information how to build FIFE on Linux-based distributions can be found
at the project wiki:
 * https://github.com/fifengine/fifengine/wiki/Building-on-linux

After FIFE has been built successfully move into the <FIFE>/demos/rio_de_hola 
directory and run: 'python run.py'

=== OS X ===
Detailed information how to build FIFE on OS X systems can be found at the 
project wiki:
 * https://github.com/fifengine/fifengine/wiki/Building-on-osx

After FIFE has been built successfully move into the <FIFE>/demos/rio_de_hola 
directory and run: 'python run.py'


=== Win32 ===
The Win32 package ships with precompiled binaries. Simply unpack the package to
location of your choice. After that you'll need to install the 32bit version of
ActivePython 2.7.

Move into the <FIFE>\demos\rio_de_hola directory and start the run.py script. 
You can simply double-click with your mouse on it. It should be automatically 
associated with the Python interpreter after installing ActivePython.
