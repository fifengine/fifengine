#!/bin/bash

RE_AS_DIR=0
TODAY=$(date +"%Y/%m/%d")

if [ $# -gt 0 ]; then
  if [ "$1" == "-dir" ]; then
    RE_AS_DIR=1
    if [ $# -eq 2 ]; then
      TMPDIR=$2
    fi
  fi
fi

if [ $RE_AS_DIR -eq 0 ]; then
  TMPDIR=$(mktemp -d -t fifereleaseXXXXXX)
else
  if [ -z "$TMPDIR" ]; then
    echo "Install directory: [$HOME/fife]"
    read TMPDIR
    if [ -z "$TMPDIR" ]; then
      TMPDIR=$HOME/fife
    fi
  fi
  if [ ! -d $TMPDIR ]; then
    mkdir -p $TMPDIR || exit 1
  fi
fi

function zap_svn() {
  (cd $1 && find -type d -name .svn | xargs rm -rf)
}

function get_content() {
  ( cd $TMPDIR && \
  svn co https://svn1.cvsdude.com/fife/content/trunk/ content )
  rev=$(grep 'revision=' $TMPDIR/content/.svn/entries | sed -e 's|^.*="||' -e 's|".*||')
  SVN_DATA_REV=$rev
  if [ $RE_AS_DIR -eq 0 ]; then
    zap_svn $TMPDIR/content
    (cd $TMPDIR/content && rm -rf gfx/tiles/base_povray )
  fi
}

function get_core() {
  ( cd $TMPDIR && \
  svn co https://svn1.cvsdude.com/fife/engine/trunk/core )
  rev=$(grep 'revision=' $TMPDIR/core/.svn/entries | sed -e 's|^.*="||' -e 's|".*||')
  SVN_CORE_REV=$rev
  if [ $RE_AS_DIR -eq 0 ]; then
    zap_svn $TMPDIR/core
  fi
}

function archive_dir_as() {
  ( cd $1 && tar jcf $2 * )
}

#get_content
#archive_dir_as $TMPDIR $PWD/fife-content_rev${SVN_DATA_REV}.tar.bz2
#(cd $TMPDIR && rm -rf)

#get_core
#archive_dir_as $TMPDIR $PWD/fife-core_rev${SVN_CORE_REV}.tar.bz2
#( cd $TMPDIR && rm -rf)

function post_install() {
  cat <<EOF >${TMPDIR}/INSTALL
This is just a quick install guide. If you encouter problems while
compiling, have a look at our more detailed guides at the FIFEwiki:
http://wiki.fifengine.de/index.php?title=Linux_compile_guide#Compiling

1) compile the engine
   cd core ; scons

2) copy/symlink executable into this directory
   ln -s core/src/guimap_test/guimap_test .

3) copy 'master.dat' & 'critter.dat' into this directory; use
   lower case filenames.

4) run: ./guimap_test
   you can also load scripts, like: 
   ./guimap_test content/scripts/demos/level_chooser2.lua
EOF
  cat <<EOF >${TMPDIR}/README
==========================
FIFE 2007.1 (official src release based on FIFE engine SVN rev. ${SVN_CORE_REV} &
content SVN rev. ${SVN_DATA_REV})
==========================

FIXME: needs update here!

Intention of this release:
==========================
We've decided to release this little present as two of our coders were
hard working in the last weeks to get some new fixes and features into
FIFE. So here we go :-) Please note that there will be no groundbreaking
new features in this release because we did concentrate on refactoring
important parts of our codebase.

You can post problems in the help section of our forums:
http://www.fifengine.de/module-pnForum-viewforum-forum-7.phtml

Thanks for your support.

Important changes since last version
==========================
* Manipulate mapview by removing objects and critters (they get restored
  at elevation change)
* New stylish console (yeah!)
* Load maps & make screenshots via console
* New guichan widgets
* Scriptable LUA gui (see guitest and content/scrips)
* Better code documentation
* Taking screenshots
* New renderbackend approach
* A LOT of fixes :-)
==========================

For a complete changelog visit:
https://mirror1.cvsdude.com/trac/fife/engine/timeline

What works:
==========================
* Loading Fallout 1 .dat archives
* Loading Fallout 2 .dat archives
* Override .dat files by using unpacked local files (first useful
  'modding' feature)
* Load Fallout 1 .map files
* Load Fallout 2 .map files
* Choose resolutions from 640 * 480 up to 1920 * 1440
* Choose between SDL software rendering and OpenGL hardware accelerated
  render backends
* Play your favourite song as .ogg or .acm background music
* Map scrolling with the arrow keys
* Cycle through map elevations (-> e)
* Toggle roof tiles on / off (-> r)
* Toggle objects on / off (-> o)
* Increase (-> s) and decrease (-> x) the degree of alphablending for
  roof tiles
* Increase (-> a) and decrease (-> y) the degree of alphablending
  for objects
* Taking screenshots (-> p)
* Ingame console for tweaking and testing (-> F10)
* Tile and object grids (use "t" and "g" to toggle them off / on) if
  you use the guimap_test binary
* Critter animations
* Palette animations for Fallout .pal files (animated tiles & objects)


What does not work:
==========================
* No transparency for force fields, nightkins, etc.
* All the rest that worked in the Fallout games :-p


Custom settings
===========================
* FIFE uses a file called "fife.config" to give the user control about
some engine aspects
* The file should be self-explanatory


Working with Fallout 1 maps:
==========================
* Copy the "master.dat" & "critter.dat" from your Fallout 1 install to
  your FIFE install directory
* Launch FIFE via:
  ./guimap_test content/scripts/demos/load_fo1_maps.lua


Working with Fallout 2 maps:
==========================
* Copy the "master.dat" & "critter.dat" from your Fallout 2 install to
  your FIFE install directory
* Launch FIFE via:
  ./guimap_test content/scripts/demos/load_fo2_maps.lua


Working with custom (XML) maps:
==========================
FIFE supports a (temporary) own map format so you can test the engine
without having one of the Fallout games installed. The used graphics
for these new maps are released under GPL and are just temporary
versions. Feel free to provide us your gfx skills so we can create a
high quality free FIFE map that will be featured in one of the next
releases. You can add Fallout maps you created yourself to the list by
editing /FIFE_dir/content/etc/maps_custom.txt.

* Launch FIFE via:
 ./guimap_test content/scripts/demos/load_custom_maps.lua


Loading maps via the maploader interface:
==========================
Make sure that you've launched FIFE via 
 ./guimap_test content/scripts/demos/load_custom_maps.lua
Select the map you want to view from the dropdown menu and load it. 
Take screenshots (via the "p"-key) or do whatever you want. If you want to 
load the new map press F10, and click on the "Tools" button in the
console. You can load another map via the maploader interface now.


FIFE searching for new members:
==========================
Last but not not least FIFE is always searching for new members that
want to support the project by contributing. If you think that FIFE is
worth being supported you visit the following page to get an overview
over the free positions on the team:
http://wiki.fifengine.de/index.php?title=Help_wanted

You can contact the FIFE team @ fife-public@lists.sourceforge.net


The FIFE Team, ${TODAY}
EOF
}

get_content
get_core
post_install
OUTFILE=fife_core-r${SVN_CORE_REV}_content-r${SVN_DATA_REV}.tar.bz2
#OUTFILE=FIFE_2007.1_src.tar.bz2
if [ $RE_AS_DIR -eq 0 ]; then
  archive_dir_as $TMPDIR $PWD/$OUTFILE
  rm -rf $TMPDIR
  echo "Archived: core $SVN_CORE_REV, cnt $SVN_DATA_REV in $PWD/$OUTFILE"
else
  echo "Installed: core $SVN_CORE_REV, cnt $SVN_DATA_REV in $TMPDIR"
fi
