= frminfo =

Yet another tool to extract graphics...
It supports reading from master/critter.dat and creates bmp or png files.

It can also print the 'art/*/*.lst' file contents (from master.dat).
[This functionality is limited to support FO2 only]

== Building ==

It needs the fife core libs, SDL, SDL_image and their dependencies;
only tested on linux for now.

Only 'scons' is up-to-date; the makefile is outdated;
Scons assumes the path to the core is: ../../core

Please note that I am relying on some linker-flags to keep the
number of required libs down; this program does not really need:
* openal
* guichan
* opengl
* lua
If this trick doesn't work, you have to add the respective
libs (see fife SConstruct) to the linker command.

= Usage =

Run "frminfo -h" for usage info.
