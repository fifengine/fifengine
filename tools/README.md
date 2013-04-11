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
FIFE Tools - April 11th, 2013
=============================

Over time we have come up with specific tools to help you generate content for
FIFE.  This file documents these tools, and where you will find them useful.
It should be kept up to date.  If you notice a tool listed under **current**
that is in fact outdated or no longer working please create an Issue on GitHub
to either update the tool or remove it altogether.

Current
-------

### Editor (also known as FIFEdit)

This is our main map editor.  It's features are currently evolving quickly so
there is no real formal documentation for it yet (we need your help!!).

### Atlas Creator

Created by one of our talented GSoC students (kozmo) this allows users to pack
a bunch of single images into an image atlas.  This not only reduces the 
number of files you need to pack with your game, it also speeds up rendering!
This is written in C++ using Qt so you will need to have the Qt development
packages installed before you can compile it!


### geometry_twister.py

Visually test map tilting and rotation values.  This is useful for determining
the camera settings you should use when creating a new map.


Outdated/Deprecated
-------------------

### animation_generator.py - removed after 0.3.4

This script requires you to set up some global variables.  It takes a list of
files and generates an animation.xml based on the global variables and file 
list.


### animpacker.py - removed after 0.3.4

Saves animations for an already loaded map.  The actual use of this file is
unknown at the time of writing this readme.  It looks like it would be used
to convert an old file format to a new one.


### object_editor.py - removed after 0.3.4

An interactive object file editor.  File formats it uses are outdated.


### object_generator.py - removed after 0.3.4

This script requires you to set up some global variables before using.  It
takes a list of files and generates object files based on the global variables
and file list.


### object_header.sh - removed after 0.3.4

Adds an xml preprocessor directive to files named object.xml that indicates to
fife that these are object files. Call with the root directory to convert 
under as an argument such as: `./object_header my_client/objects`


### pack_anim.py - removed after 0.3.4

Uses animpacker.py to convert animations to a new format.


### ruleset_tester.py - removed after 0.3.4

Never actually used so removing.
