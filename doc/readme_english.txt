==========================
FIFE 2007.1 (official final release based on FIFE engine SVN rev. 1004 & content SVN rev. 162)
==========================

Intention of this release:
==========================
Here it is: the final version of our 2007.1 milestone. This is quite an important release to us because our own map format is now in place and has been used for our own first rather huge demo map. Welcome to FIFE Island :-)

The graphics that have been used for this demo map HAVEN'T BEEN CREATED BY US! They have been created by Reiner Prokein. You can get his graphics for free at his site "Reiner's tilesets":
http://reinerstileset.4players.de/


Bug / issue reporting:
==========================
You can post problems in the help section of our forums, on the project mailing list or report them directly in our irc channel.

IRC Channel:
http://wiki.fifengine.de/index.php?title=IRC

Mailing list:
http://wiki.fifengine.de/index.php?title=Mailing_list

Forums:
http://www.fifengine.de/module-pnForum-viewforum-forum-7.phtml

Thanks for your support.

Important changes since 2007.0
==========================
* Stable custom XML map format for FIFE including our demo map to show the new features and syntax of it :-)
* FPS limiter (can be set in fife.config) to save important CPU time for future tasks (AI, pathfinding, etc.)
* Tile and object coordinates can be displayed (press "c")
* Color key overriding for Fallout transparency effects (see content/gfx/fallout_overrides.xml)
* Fixed DAT1 decoding code (bug was introduced with the DAT1 rewrite)
* Major map model classes have been exported to Lua and are now accessable via the console
* Map size dependant, configurable geometries 
* Colorkeying for the SDL renderer
* Add custom VFS sources via vfs.addSource (see content/etc/vfs_sources.txt)
* Increased startup speed with FO2 DAT files
* Enhanced camera with support for multiple viewports (see content/scripts/demos/map_control.lua)
* Built against SDL_image 1.2.5 and guichan 0.61 now (Win32 binaries)
==========================

For a complete changelog visit:
https://mirror1.cvsdude.com/trac/fife/engine/timeline

For all closed tickets since 2007.0 see:
http://mirror1.cvsdude.com/trac/fife/engine/query?status=closed&milestone=2007.1

What works:
==========================
Have a look here:
http://wiki.fifengine.de/index.php?title=Engine_features

What does not work:
==========================
* A lot, basically everything that is not listed at the wiki page that is linked above.

Custom settings
===========================
* FIFE uses a file called "fife.config" to give the user control about some engine aspects
* The file should be self-explanatory

Working with Fallout 1 maps:
==========================
In Windows:
* Copy the "master.dat" & "critter.dat" from your Fallout 1 install to your FIFE install directory
* Edit the FIFE.bat to launch FIFE via: guimap_test.exe content/scripts/demos/load_fo1_maps.lua

In Linux:
* Copy (or link) the "master.dat" & "critter.dat" from your Fallout 1 install to the directory containing guimap_test
* Launch FIFE via ./guimap_test content/scripts/demos/load_fo1_maps.lua

Working with Fallout 2 maps:
==========================
In Windows:
* Copy the "master.dat" & "critter.dat" from your Fallout 2 install to your FIFE install directory
* Edit the FIFE.bat to launch FIFE via: guimap_test.exe content/scripts/demos/load_fo2_maps.lua

In Linux:
* Copy (or link) the "master.dat" & "critter.dat" from your Fallout 2 install to the directory containing guimap_test
* Launch FIFE via ./guimap_test content/scripts/demos/load_fo2_maps.lua

Working with custom (XML) maps:
==========================
FIFE supports a stable own map format now so you can test the engine without having one of the Fallout games installed. You can add custom XML or Fallout maps to the list by editing /FIFE_dir/content/etc/maps_custom_players.txt (maps_custom_developers.txt if you start FIFE the developer lua script).

In Windows:
* Edit the FIFE.bat to launch FIFE via: guimap_test.exe content/scripts/demos/load_custom_maps_players.lua

In Linux:
* Launch FIFE via ./guimap_test content/scripts/demos/load_custom_maps_players.lua

Hint: the current demo map is "official_map.xml". The other maps in the list are slightly outdated and seem to share some markup issues. Therefore we advise to stick to the official demo map and our map format guide at the wiki if you plan to build custom maps for FIFE:
http://wiki.fifengine.de/index.php?title=Map_Format

Loading maps via the maploader interface:
==========================
Make sure that you've launched FIFE via "guimap_test.exe content/scripts/demos/load_custom_maps_players.lua" (Win32) / "./guimap_test content/scripts/demos/load_custom_maps_players.lua" (Linux). Select the map you want to view from the dropdown menu and load it. Take screenshots (via F12) or do whatever you want. If you want to load the new map press F10, and click on the "Tools" button in the console. You can load another map via the maploader interface now.

Sound troubles:
==========================
In Windows:
If you don't hear the "Maybe" song when you launch FIFE, you should install an updated OpenAL driver to resolve this issue. Simply execute the OpenALwEAX.exe that's inside your FIFE folder. Please report back at the fifengine.de forums if installing this updated driver did _NOT_ fix the sound problems for you.

FIFE searching for new members:
==========================
Last but not not least FIFE is always searching for new members that want to support the project by contributing. If you think that FIFE is worth being supported you visit the following page to get an overview over the free positions on the team:
http://wiki.fifengine.de/index.php?title=Help_wanted

You can contact the FIFE team @ fife-public@lists.sourceforge.net

The FIFE Team, 2007/04/22
