```
__/\\\\\\\\\\\\\\\___/\\\\\\\\\\\___/\\\\\\\\\\\\\\\___/\\\\\\\\\\\\\\\_        
 _\/\\\///////////___\/////\\\///___\/\\\///////////___\/\\\///////////__       
  _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________      
   _\/\\\\\\\\\\\__________\/\\\______\/\\\\\\\\\\\______\/\\\\\\\\\\\_____     
    _\/\\\///////___________\/\\\______\/\\\///////_______\/\\\///////______    
     _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________   
      _\/\\\__________________\/\\\______\/\\\______________\/\\\_____________  
       _\/\\\_______________/\\\\\\\\\\\__\/\\\______________\/\\\\\\\\\\\\\\\_ 
        _\///_______________\///////////___\///_______________\///////////////__
```
        
FIFE - [![Build Status](https://travis-ci.org/fifengine/fifengine.png?branch=master)](https://travis-ci.org/fifengine/fifengine)
------------
Flexible Isometric Free Engine (FIFE) is a multi-platform isometric game engine 
written in C++. It comes with python bindings allowing users to create games 
using python as well as C++. The engine is designed to be flexible to allow you
to extend it and add any feature you can imagine to your project.


Table of contents
-----------------
1. [Features]        (#1-features)
2. [License]         (#2-license)
3. [FIFE Clients]    (#3-fife-clients)
4. [Known Issues]    (#4-known-issues)
5. [Troubleshooting] (#5-troubleshooting)
6. [Feedback]        (#6-feedback)
7. [Closing Notes]   (#7-closing-notes)


1) Features
-----------
A complete list of the features that FIFE offers can be found at the project 
wiki:
 * https://github.com/fifengine/fifengine/wiki/Features


2) License
----------
The source code (*.cpp, *.h & *.py) is licensed under LGPL 2.1 or newer:
 * http://www.gnu.org/licenses/lgpl-2.1.html

Content was taken from a lot a lot of different 3rd party sources. Therefore 
each client directory comes with a separate LICENSE file that states the origin
of the content, the author and the actual license it was published under.


3) FIFE Clients
---------------

### Editor
The editor tool can be found within the `<FIFE>/tools/editor` directory. You
can launch it by running `run.py`.  It is used to edit map files for the tech
demo (rio do hola).  Other clients extend it and use it to edit their maps.

### Rio De Hola
Rio de hola is a technology demo showing off many of the FIFE features.  It is 
located in the `<FIFE>/demos/rio_de_hola` directory and can be launched by 
running run.py.  It was at one time meant to be an example game but we have 
moved away from that idea and it is now more of a technology demo and a 
playground for developers to test their code.  It does serve as a good starting
point for people wishing to play around with FIFE or base your game off of.

### Shooter Demo
The Shooter demo was an attempt to show the versatility and flexibility of FIFE.
It is a simple side scrolling shooter that has a main menu, one level and an
end boss.  Try your luck and see if you can defeat the boss!

### PyChan Demo
Last but not least there is an example client residing in 
`<FIFE>/demos/pychan_demo` that shows how the pychan GUI library works.
Start the GUI demo application by running `pychan_demo.py`.

### Configuring the Editor and Demos
The engine utilizes special settings files for configuring FIFE. This file is 
called `settings.xml` and resides in the `~/.fife` directory (in 
`<User>\Application Data\fife` for Windows users).  The Shooter Demo and the
PyChan demo are exceptions.  They both store their `settings.xml` file in their
root directories.

NOTE that the `settings.xml` file is auto generated and wont be there until you
run the demos for the first time.  FIFE automatically fills the settings file
with default values.  For more information on FIFE settings please see the 
wiki page: https://github.com/fifengine/fifengine/wiki/Engine-settings


4) Known issues
---------------
### Documentation
 * Bug in `dep_analyzer.py` which prevents graphs from being generated (#554)
 * Atlas Creator needs to be documented (#559)

### Engine
 * No fallback solution for special characters (#302)
 * `Camera`s must be attached to layers (#305)
 * Loaders have a circular import (#478)
 * Unit-tests are broken (#423)
 * Mouse gets stuck in top left corner after alt+tab in fullscreen modes (#593)

### Guichan/Pychan
 * No UTF8 support using vanilla guichan source (#432)
 * Pychan dropdown menus crash FIFE-based client on win32 (#508)
 * `Button` doesn't react to click when drawn below mouse pointer (#387)
 * Pychan dropdowns refuse to open if there is no space below (#445)
 * Borders are not rendered on edges of widgets that touch their parent 
   container's edge. (#522)
 * `GuiImage` unable to find loader for JPEG in `ImagePool` (#548)
 * Some strings are cut short in some widgets (#585)
 * `applyOnlySuitable` fails on `functools.partial` (#638)
 * Default style will sometimes override custom style (#656)
 * `adjustSize()` sometimes sets width to invalid values (#666)
 * Guichan adds hardcoded value to user setting of `base_color` attribute (#678)

### Build
 * Uninstaller doesn't remove `fife` module (#526)
 * Improve build system shared library support on linux (#575)
 * Cannot build a usable debug version of `_fife_d.pyd` with mingw
 * `_fife.so` linked with absurd rpath (#620)
 * Unable to make distclean (#622)

### Editor
 * Editor code does not match coding standards (#371)
 * Map editor clobbers custom attributes (#379)
 * History bugs (#452)
 * Placing an instance on top of another then undoing resets the original 
   instances properties (#459)
 * Should be able to set the default zoom level for a camera (#467)
 * Misc FIFE Editor crashes (user input validation/exception handling missing) 
   (#537)
 * Editor file dialog uses botched relative paths (#616)


5) Troubleshooting
------------------

### Video Card/Driver Issues
There are some known driver/card combinations that exhibit some problems with
FIFE.  Users have reported seeing blank screens or fuzzy images.  If this is
happening to you please report it on our forums in the Help and troubleshooting
section here: http://forums.fifengine.net/index.php?board=4.0

Please include your Card/Driver/OS information in your post.  Currently the fix
is to modify your `settings.xml` file and set both GLUseFramebuffer and GLUseNPOT
to `False` (they are `True` by default).

### Audio
Win32 users tend to suffer from problems with the OpenAL drivers. If you don't 
hear sound while running a FIFE client, run `oalinst.exe` that ships with the
Win32 FIFE Development Kit.  This is the latest OpenAL driver for Win32.


6) Feedback
-----------
We appreciate every kind of feedback concerning the release, the project in 
general and the bundled techdemo. Feedback is a great way to help us to improve 
FIFE. If you would like to get in contact with us and provide feedback you can 
either visit our IRC channel or our forums:
 * #fife on freenode.net
 * http://forums.fifengine.net/index.php?board=5.0


7) Closing Notes
----------------
Have fun with the release and let us know what you think about it!
-- The FIFE team.

http://www.fifengine.net/

2013/04/09
