Setup (after compiling):

copy/symlink these files into this directory:
* master.dat
* critter.dat

svn co https://svn1.cvsdude.com/fife/content
(needs at least rev21)

Usage:

./testproj

Starts 'Fallout 2'; in the main-menu click 'new game'

Dev-Notes:

The default initial script is 'content/scripts/fallout2.lua'; you can 
display a menu from that file, but to show a map you have to wait. *g*

When the engine is ready, it calls 'on_engine_start' just before 
entering the main loop; from there you can call:
-- lua
set_next_mapfile('maps/arvillag.map')
state_manager.activate('MapView')
-- end lua
