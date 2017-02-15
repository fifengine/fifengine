# ChangeLog

## [0.4.1] - 2017-02-07

### Engine
* Corrected wrong postion handling for events.
* Added AnimationManager, issue #675
* Enhanced XML formats, issue #963
* Added events for mouse wheel left and right, issue #967
* Fixed Location::getLayerCoordinates() on hex grids, issue #970
* Removed special treatment of function keys, issue #976 
* Fixed tinyxml issue, was not being able to overwrite existing file, issue #978
* Map more SDL keys to CEGUI keys, issue #980

### Extensions
* Load default settings if settings file has no FIFE module, issue #959
* Update exception handler in ApplicationBase, issue #969
* Remove Icon from manager on source load failure, issue #972

### Misc
* Ensure that FIFE exception message is printed in Python, issue #974
* Several build fixes and improvements

## [0.4.0] - 2017-01-16

* removed Psyco JIT support (by removing "UsePsyco" and the related code)
* removed "scons" build system
* rename "cegui-0" (v0.8+) to "cegui"
* removed cegui compatibility layer for versions below v0.7.9

### Engine
* renamed overloaded createTrigger and removeTrigger methods, issue #947 + PR #950
* Got rid of various compiler warnings
* Add getCellsInCircleSegment() to CellCache and getInstancesInCircle(),
	getInstancesInCircleSegment() to Layer
* Add MultiPathfinderTest
* Add new instance function, prepareForUpdate()
* Local tinyxml files are now only added to the compile list if either
	local-tinyxml is defined for SCons or the system version of tinyxml is not found.
	[t:619]
* Add animation & color overlays [t:201], fixes [t:762], [t:764] and [t:763]
* Merge GSOC work done by vdaras.
* Replaces Guichan dependency by Fifechan
* Add support for librocket
* Add support for cegui [t:726]
* Fixed keyboard input of control characters for CEGUI by Niektory [t: 796]
* Make use of Fifechan optional
* Add overlay test

### Extensions
* Added the gl/GLU.h header to the atlas creator [t:740]
* Removing the soundmanagers dep on pychan [t:767]
* Slightly modified frange() to be more robust and fixed the ceil() call, fixes [t:725]
* Convince SWIG that Color.r (g, b, a) should be a python property [t:799]
* Inherit fife.Exception from python's (Base)Exception [t:788]
* Import cleanup in pychan.widgets [t:787]
* Fix password- and textfield not accepting initial data 
* Remove cell_pathfinding compatibility layer [t:775]
* Fix typo in fife_settings

### Misc
* Renamed all --with/--without scons options, fixes [t:797]
* Add travis configuration by jakoch [t:784]
* Modifying the versioning scheme [t:783]
* Darwin config now expects everything to be installed from source

## [0.3.5] - 2013-08-21

### Engine
* Fixed another LayerCache bug.
* Fixed blocking bug, which has affected multi cell objects.
* Fixed segmentation fault within pathfinder.
* Fixed segmentation fault within Instance::follow.
* Fixed the visible/invisible problem. [t:780]
* Fixed facing location and a related bug. [t:773]

## [0.3.4] - 2013-02-28

### Engine
* Fixed a couple of compiler errors when SDL renderer is chosen (i.e. 
  HAVE_OPENGL macro is not defined). [t:693]
* Changed the way instance rotation works. Instead of using a facing location 
  the instance now use only a rotation value. But the possibility to get and 
  set facing locations is still available. In case you set a facing location, 
  internally it is converted to a angle or in case of get, the angle is 
  converted to a location. [t:647]
* Fixed grid scaling by camera rotation. [t:695]
* Optimized camera update() function. Old caused two matrix updates.
* Changed the angle/rotation functions to use MapCoordinates instead of 
  ExactLayerCoordinates.
* Added a virtual destructor to the IResourceManager interface. [t:709]
* Fixed a problem with the usage of the size_t type. [t:702]
* Addressed some API inconsistencies. Renamed LogManager::isLoggingToPrompt() 
  to isLogToPrompt() and LogManager::isLoggingToFile() to isLogToFile(). This 
  shouldn't affect python clients as the appropriate changes have been made 
  to the python extension. [t:711]
* Addressed more API inconsistencies. Renamed (in serveral places) 
  setMouseAcceleration() to setMouseAccelerationEnabled() and 
  getMouseAcceleration() to isMouseAccelerationEnabled(). This shouldn't 
  affect python clients as the appropriate changes have been made to the 
  python extensions. [t:711]
* Fixed some problems when zooming at 200% or 300%.. etc etc. Images were not 
  exactly 2x2 or 3x3. [t:719]
* Added getMapViewPort() and getLayerViewPort() functions to camera class. 
  These functions return the position and size of the camera in map or layer 
  coordinates. CellRenderer make use of it to speed up rendering, especially 
  when many cells exist in the cache. [t:647]
* Added Fog of War support.
* Pathfinder has been completely re-written.
* Added getDesktopHeight() and getDesktopWidth() to DeviceCaps. [t:669]
* Enhanced Instance coloring. The new alpha component is used as interpolation
  factor. [t:729]
* Ensure SCons version is 2.0+. [t:735]
* Added the staticLayer feature so that layers can be rendered as one texture.
  [t:742]
* Client can now select, per Layer, between three sorting algorithm.

### Extensions
* Fix event groups in pychan; only the last group's callback used to be 
  considered.  [t:23]
* Added the getter and setter functions to the fifelog extension. [t:711]
* Replace 'dct.has_key(k)' with 'k in dct'. [t:631]
* You can now overwrite existing widget types without pychan "trying" to throw
  an exception. There is no feedback if you do overwrite an existing widget, 
  it will silently allow you to do so. fixes[t:748]
* Allow client to set focus to a child widget when using widget.execute(). 
  [t:750]
* Fixed a bug in the slider widget which caused it to ignore the step length. 
  [t:754]
* Unknown exceptions are now caught by BasicApplication and the engine is 
  exited gracefully. This prevents an endless output of exceptions from being 
  printed to the console window. [t:755]
* Added the ability to set the horizontal_scroll_amount and 
  vertical_scroll_amount via XML for scroll areas. [t:692]
* Modified the default settings dialog box to no longer use drop downs. This 
  prevents the dropdown segfault from happening. [t:756]
* Fix typo in xml serializer.  This was leading to a crash when extra_attrs 
  were passed. [t:760]

### Misc
* Added some excludes to the fife.iss file so it doesn't include some files 
  that it shouldn't when building the windows installer. [t:601]
* Changed the python executable back to python.exe from pythonw.exe in the 
  windows installer. pythonw.exe crashes with some version of python when you 
  output something to the console screen. [t:601]
* Added PathfinderTest to fife tests. It demonstrates multilayer movement and 
  how speed and cost multiplier could be used. [t:647]
* Updated the header file location for fifechan when building with mingw on 
  windows. [t:698]
* Update to allow FIFE to find fifechan when it is installed in the libpath. 
  Tested this on archlinux. [t:698]
* Allow for parallel builds. fixes[t:696]
* Removed unused file - freebsd8-config-dist.py. [t:716]
* Updated the COPYING file to be the current copy of the LGPL 2.1 text. It 
  seemed to contain an older version of LGPL. fixes[t:718]
* Modifying code/comments/file headers that apply to this branch to conform to
  the new coding standard. Also applied the correct eol-style svn property for
  some of the new files. [t:647]
* Applying patch provided by gscai. This allows for custom command line 
  parameters to be passed to the windows build batch files. [t:737]
* Applying patch provided by gscai. This allows the use of the CXX environment
  variable to change the path to the desired C++ compiler. [t:738]
* Fixed a bug where the shooter demo would crash after a few seconds after 
  starting a new game. This also prevents the player ship from being lost off 
  the left side of the screen. [t:747]

### Docs
* Updated the doxygen configuration file to be current with the latest doxygen
  version.
* Added a process on the web host to generate doxygen documentation as well as
  epydocs for the python modules.
* Doxygen docs now include the FIFE revision number they were generated from.
* Updated a comment in quadtreerenderer.cpp that was being mistaken by doxygen
  for the FIFE namespace docs.
* Added FIFE namespace documentation to version.h
* Fixed a bunch of doxygen warning/error messages (now that we are able to see
  them). [t:550]
* Epydoc formatting fixes for xml loader files. [t:724]

## [0.3.3r3] - 2012-04-17

### Engine
* Implemented C++ map savers which is now being used by the editor. [t:491]
* Fixed instance Z so it works now as expected.  This should only be used for
  small optical corrections. (r3810)
* Added layer z offset. [t:366]
* Fixed a problem with instance setLocation(). (r3810)
* Removed sub image loaders as they are not used. (r3811)
* Object is no longer derived from IResource. (r3811)
* SDL Renderer now renders quads as expected. [t:607]
* Changed the Image creation in CoordinateRenderer. The coordinate is now 
  splitted into three images. So the TextRenderPool can reuse the most images 
  and must not create one for each coordinate. Additional the images take camera
  zoom into account, but you can also disable it. [t:612]
* Added InstanceDeleteListener to InstanceRenderer and RenderNode. [t:602]
* All images created by effects are now owned by ImageManager and freed
  if no longer used after 60 seconds. [t:611]
* Added the set method to PointType2D and PointeType3D. [t:615]
* Changed the way Atlases and SubImages?(SharedImages?) are loaded and freed. 
  SubImages now store a ImagePtr to its Atlas and the name. So a AtlasImage is 
  only unreferenced if all SubImages are removed. It is also possible to free 
  all images. Images that are required but were freed, invite self-adjusting. 
  [t:605]
* Added functions to allow the client to set the mouse position on the screen
  and retrieve it as a tuple in python.  [t:617]
* Added another version of captureScreen() that takes width and height params
  allowing you to resize the resulting image. [t:618]
* Fixed a bug that led to Segmentation Faults, if the BitsPerPixel setting was 
  different than 32. [t:623]
* Added support for True Type Collection fonts. [t:614]
* Optimized getMatchingInstances(). [t:592]
* TargetRenderer is no longer dependent on map loading. [t:636]
* Small fix in the AtlasLoader to set the filename of any objects read from 
  the atlas file to the atlas xml file instead of the image file like it was 
  doing previously. This fixes a problem seen in the map loading/saving where 
  the imports would save the image file instead of the xml description file in 
  the map. [t:491]
* Zooming now works as expected with x and y offsets. [t:667]
* We now get more information when gcn::Exception is thrown. [t:665]
* MapLoader now allows for .zip files to be loaded. ZipSource implements a 
  smarter file search so it can properly load files from a zip archive. [t:491]
* Removed the lasttime_draw reset from TargetRenderer, because this can cause 
  problems. [t:674]
* Cursor no longer moves to 0,0 on startup. [t:670]
* Removed unneeded command line option -Wl from both release and debug builds 
  as this was breaking builds on gcc 4.7 and above. [t:691]

### Extensions
* Fixed the file extensions registration for python plugin map loaders. [t:609]
* Setting module is more robust on errors concerning non-string parameters. 
  [t:626]
* SimpleXMLSerializer now saves XML files with platform specific line engines. 
  [t:630]
* Child widgets should correctly report if they are hidden or not. [t:628]
* Removing the hack to grab references to the GUIChanManager. Now we just use 
  the pychan manager to grab what we need to grab from the GUIManager. Added 4 
  new functions to pychan Manager. getConsole(), getDefaultFont(), createFont(),
  and releaseFont(). [t:644]
* Fixed deepApply() to now include hidden children in it's search. also added
  the ability to only search for visible widgets. [t:608]
* GUI Icons can now be resized at render time. [t:655]
* Widgets can now be cloned to make widget templating possible. [t:376]

### Editor
* Users can now specify an additional plugin directory for the editor. [t:598]
* Fixed a couple small editor bugs regarding map loading. [t:603] and [t:604]
* Added a plugin interface to extend the editor with savers. [t:610]
* Toolbars can be docked/undocked again.  [t:635], [t:640], and [t:641]
* The toolbox, toolbar, and statusbars statuses are now reflected correctly in
  the menu. [t:643]
* Object Editor plugin now displays animations as expected. [t:624]
* Added InstanceHighlighter plugin
* Added docking status saving to ToolBar widget class, added default docking 
  settings for Toolbox
* Re-arranged init order in MainWindow.initGui() to make dockareas available 
  for ToolBar
* Small change to screenshot icon to give it a bit more space on the left
* Refactored ObjectSelector
** Added auto-alignment depending on docking area (vertical/horizontal)
** Right click can be used to trigger the alignment change manually
** No annoying resizes on object selection
** Speed issues not fully solved, but pin-pointed: resizing ~200 icons is a bit
   too much for pychan
* GUI Plugins can now use auto-alignment functionality from the ToolBar class
* Small design fix in LayerTool
* Small fix for MapFileHistory
* Editor now doesn't load the same mapfile twice (gives feedback in the 
  statusbar)
* Re-enabled saving for ObjectEdit? plugin
* Implemented XMLObjectSaver which allows serialization of a fife.Object
** NOTE: only well-formed xml files are accepted
** NOTE: provides a compat-mode to scan for <object> tree roots, but also uses 
   new <asset> tree root
* Added namespace sorting to ObjectSelector
* Refactored ObjectEdit

### Misc
* Removed some unnecessary warning messages from Model. [t:645]
* Removed non-free icons from the atlas editor and added license file to the
  atlas editor. [t:637]
* Added a proper distclean build target. [t:622]
* Added getRevision() to allow client to query the revision FIFE was build from.
  This only works if svnverion is present in the users PATH.  Window users will
  always get 0. [t:671]
* Added a check to animpacker.py to convert double points in action names to 
  underlines. Additional the convert now consumes less RAM, because the 
  ImageManager free images. [t:677]

## [0.3.3r2] - 2011-11-02

### Engine
  * Added getVersion(), getMajor(), getMinor(), getSubMinor() functions and 
    exposed them to python. [t:582]
  * Added a new math function to compare floats/doubles. It returns true if the 
    difference is smaller as delta.
  * Fixed the getMatchingInstances() function. [t:579]
  * Added an bool to getMatchingInstances(). If a client need pixel accuracy 
    results it should be set to true. Otherwise it should be set to false 
    because so it skips the pixel compare and is much faster. [t:586]
  * Added two more InstanceChangeTypes, ICHANGE_BLOCK and ICHANGE_CELL. [t:574]
  * The ActionFrame for animations has been made functional. This means a minor
    API change. For details look into the changed demo scripts. [t:573]
  * Fixed the broken FIFE::Command type events. Now all event states are 
    evaluated and not just one. Example from the ticket works now as expected. 
    [t:503]
  * The DELETE macro which is defined in winnt.h is undef'ed on any windows 
    platform that defines the WIN32 macro. This fixes compilation when using 
    visual studio. [t:589]
  * Added two new mouse settings. The first one is the mouse sensitivity and 
    the second can enable mouse acceleration. The sensitivity is limited to 
    -0.99 (the slowest) and 10.0 (the fastest). If you enable mouse acceleration
    then the sensitivity is used as the max speed value. You can also change it 
    on runtime with the EventManager. [t:487]
  * Added the getPtr() functions to ImageManager. All loaders now check to see 
    if an image has been defined before creating it. This was a small bug that 
    caused the image to be loaded even if the image was defined but not loaded 
    in the first place. [t:590]
  * Fixed a bug in ImageManager::removeUnreferenced(). It would cause a segfault 
    when executed. This has been fixed.
  * Fixed the problem where the animation atlas image becomes invalid after 
    deleting all objects from model. [t:590]
  * Added a setting to allow users to deactivate the usage of the Framebuffer 
    Object in opengl backends. refs[t:581]
  * Added a setting to allow users to deactivate the usage of NPOT Textures in 
    opengl backends. [t:581]
  * Fixed a condition where if a widget is dragged to an area on the screen 
    where there is no widget the event gets lost. It also prevents the focus 
    from being dropped. [t:594]
  * Fixed a condition where the user drags from the map to a guichan widget and 
    an event is lost. [t:594]
  * Added getRealCellDimensions() function to camera. [t:592]
  * Fixed a bug with Image names in TargetRenderer. [t:595]
  * Added 1024x600 as a valid resolution. [t:597]
  
### Extensions
  * Fixed a problem where the setting value cache is not updated [t:578]
  * Fixed a problem in fife_settings.py when starting the editor because the 
    editor tried to modify the value of a setting not in the FIFE module. 
    [t:578]
  * Added 'all' to the valid LogModules setting [t:578]
  * Pychan widgets can now be shown/hidden even if they are not top level 
    containers [t:375]
  * Fixed a small bug with basicapplication. It was grabbing a reference to 
    the gui manager incorrectly.
    
### Editor
  * Fixed a bug in the layertool GUI. It was using the quit.png icon as its 
    delete layer image instead of the delete_layer.png icon. [t:583]
  * Replacing the editor's icon files with free ones. [t:584]
  * fixed a bug in pychan.internal (hide() method didn't handle deletion 
    correctly, added a guard & docstring to explain why this is bad)
  * added new functionality to Plugin base class (read settings from editor's 
    settings file)
  * added afterDock / afterUndock methods to Panel widget; did some code 
    cleanup & improvements to meet coding standard
  * added documentation to the editor (mostly in docking functionality)
  * updated both MapFileHistory? and LayerTool? plugins to use the new Plugin 
    functionality - their docking status is now saved & loaded again
  * added new event 'onOpenMapFile' to the signalmanager which sends the path of
    the requested mapfile
  * added MapFileHistory? plugin
  * records previous loaded mapfiles and loads them on editor startup via 
    settings xml
  * selecting a mapfile either loads the map or updates outdated entries (e.g. 
    if a map has been deleted)
  * I added a max value of 5 entries, can be adjusted in the plugin file though
  * fixed minor issue with moving multiple selected instances
  * fixed "move"-mode of editor - it now doesn't try to select / deselect cells
    (that's the task of "selection mode")
  * fifedit now can handle instances on hex grids again
  * deselection of cells is now possible (previous implementation never could 
    work)
  * switched instance selection from pixel-perfect to location (click on a cell
    -> you select your instance, even if the cursor didn't scratch the instance 
    image)
  
### Misc
  * Fixed the Free Software Foundation address in COPYING as it was outdated.
  * Added some more tiles to the rpg tileset in fife_test.
  * Added a Show Hide test to the PychanTest module in fife_test.
  * Small fix for the base class Test in the fife_test application. The create 
    function reversed the engine and application parameters from what is 
    actually passed in by the TestManager. The PychanTest was unaffected because
    it had the parameters in the correct order as opposed to reversed in the 
    base class.
  * Commented out a whole lot of debug statements to make the debug output in 
    FIFE useful again. I left them in the code in case anyone still needed them.
  * Added more tests to the fife_test testing platform. [t:683]   
   
## [0.3.3] - 2011-10-06 

### Engine
  * Many optimizations/improvements to the OpenGL renderer.  Depth testing was 
    modified and now works properly with alpha blending enabled.  Alpha testing
    is used to discard pixels. [t:556]
  * Optimized color overlays. [t:539]
  * Customizable alpha threshold value for outlines / outlining [t:569]
  * Added support for drawing/modifying images. [t:553]
  * Re-implemented the Resouce Pools (ImagePool AnimationPool, etc) as 
    ImangeManager, SoundClipManager, etc. [t:504]  
  * Setting module no longer crashes when settings dialog has not been loaded.
    [t:555]
  * Fixed the problem that the objects of the renderers don't get deallocated 
    when the camera deconstructor was called. Affected renderers: Instance-, 
    Cellsection-, Generic- and LightRenderer.
  * Added the missing reset() and removeAll() functions to the GenericRender 
    and LightRenderer.
  * Removed the Engine::preInit() function.
  * Removed EngineSetting::getPossibleResolutions() and validate() functions
  * EngineSettings no longer throws any exceptions. Instead default values as 
    assigned and a warning message is sent to the logger.
  * Some optimizations to LayerCache and Camera
  * Removed some unnecessary includes.
  * Changed when some exceptions are thrown. [t:511]  
  * Added setBackgroundColor(), so a client can change the clearing color for 
    the screen. [t:307]
  * Added camera overlay functions. Clients can now add a color, image and 
    animation overlay to the camera [t:460]
  * Made sure that the fife logger is part of the FIFE namespace.
  * Modifying all integer definitions to conform to the C99 standard.
  * Added the getMinMaxCoordinates() function to the map class. It retrieves 
    the minimum/maximum coordinates of instances on the map.
  * Changed Model::getNumMaps() to Model::getMapCount().
  * Changed Map::getNumLayers() to Map::getLayerCount().
  * Changed Animation::getNumFrames() to Animation::getFrameCount().
  * FifeClass is no longer derived from IReferenceCounted.
  * ResourceClass now implements the reference counting routines.
  * Singletons are no longer derived from FifeClass.
  * Modified the HexGrid? and SquareGrid? clone process to work as expected 
    (even though it's not really used). This doesn't fix the obvious problems 
    with CellGrid? and the way grids are created. This will have to be looked 
    at closer in the future.
  * Made the quad tree depth easily changeable (instead of hard coded to 2)  .
  * SquareGrid? now uses diagonals by default.
  * Model now logs to the LM_MODEL log module.
  * Small modifications to Exception to make it output to the log correctly. 
    Now we can read the exception output.
  * Changed a few sqrt to match the new math definition.
  * Added layers getMinMaxCoordinate() function to python.
  * Renaming the only 2 .hpp files to conform to standards.
  * Optimized the lightrenderer a bit.
  * Fixed a build error on updating project files. This was caused by not 
    installing the mingw part in the devkit installer.
  * Added code to support different versions of boost filesystem. [t:529]
  * Fixed some color variables (Uint8, int32_t to uint8_t).
  * Added some code to the python build environment to support python 2.4+.
    [t:465]
  * Fixed a segfault in the view_tests.py unittest.
  * Settings now list valid screen modes. [t:494]
  * the default image loader now throws exceptions if the image fails to load.
  * Fixed a bug in RawData::read where an incorrect calculation was done to 
    check the validity of a passed in parameter. This caused a problem with 
    loading asset files when the file did not have an appended new line at the 
    bottom. Also modified the RawData::getDataInBytes, RawData::readString, and
    RawData::read methods so that they do not create a temporary data array and
    instead read directly into the output buffer, this should be more efficient.
    Added a quick comment to xmlobject.py describing why a raise call is 
    subverted by an earlier return statement. [t:516]
  * Removes an unnecessary call from endFrame().
  * Fixed some unittests.
  * Added a framerate limiter. [t:525]
  * Replaced the Generic- and LightRendererNode with RendererNode.
  * Fixed the distance calculation in routepather.cpp.
  * Added the OffRenderer. This is the only renderer that does not need a map 
    or camera. The functions are identical to those of the GenericRenderer, 
    but it use screen coordinates. This renderer is only intended for cases 
    where no map is loaded, eg for an intro.
  * Changed the swig video_tests.py to use the new renderer.
  * Removed the primitives and a few other functions from the python interface. 
    These are no longer needed.
  * Added the swig cursor unit test.
  * Added some getter functions to cursor along with some documentation.
  * RenderItem now stores an ImagePtr.
  * Fixed SharedPtr destructor. [t:561]
  * Added the getInstancesIn() function to Layer. This picks the Instances from
    the InstanceTree.
  * Fixed getMatchingInstances(). The picked instances be checked with the 
    instances of the layer. To compare the instances, getInstancesIn() is used.
    [t:562]
  * Unnecessary InstanceActivity? are now detected and deleted. Inactive 
    Instances are removed from Layer m_active_instances. [t:564]
  * Changed from using RawData::getDataInBytes to use RawData::readString in 
    the animation/atlas/object/map loaders because the readString function 
    appends a null terminator onto the end of the data contents. Without the 
    null terminator errors were sometimes seen when trying to load map assets. 
  * Added some error log printing indicating when map assets fail to load.
    [t:567]
  * Modified the RawData::read function so that it no longer appends a null 
    terminator as this was causing the python xml loaders to throw a 
    ParseError exception. Also fixed the RawData::readString function so that 
    it now appends the null terminator in the proper place. [t:567]
  * Made scons check for the dev libraries of whatever python the user ran 
    scons with. [t:523]
  * Fixed a  problem where pychan will throw an exception when trying to print 
    UTF8 characters to the console. [t:571]
  * Fixed and optimized a few functions in location.cpp. [t:563] and [t:568]
  * Added option --lib-dir to scons build system to specify a shared library 
    install location. The shared library generated will now contain an soname 
    that will be libfife.so.major. Also changed the generated shared library 
    name to be libfife.so.major.minor.release. Added a version.h file to 
    engine/core that will contain the current fife major, minor, and subminor 
    versions and should be updated for each fife release. [t:575]  
  * Fixed minor bug in getMatchingInstances(). This means that sometimes a wrong
    choice was made, because alpha value was not correct.
  * Change the way you set FIFE::Cursor to allow cursor to save a reference to 
    the image or animation. Added a function to reset the cursor drag to 
    nothing.
  * Added GLee - currently it's compiled with fife as .cpp (scons doesn't get 
    compiled .c files).
  * Initial support for c++ map/object/animation loaders. [t:491]
  * Added dependency tinyxml for reading xml files. This is stored in 
    engine/core/ext/tinyxml directory. The scons scripts will attempt to use 
    system version of tinyxml if available otherwise the local one will be used.
    [t:491]
  * Added image Atlas support. [t:542] 
  * Added a zoom flag to the image/animation functions of the GenericRenderer. 
    So a client can disable the zoom for images. [t:543]
  * Fixed a bug that caused the generated SWIG interface file for fife to not 
    compile when building with gcc 4.6.1. [t:546]
  * Began work on de-coupling FIFE from guichan. [t:549]
  * Changed Guichan graphics renderer to utilize vertex array instead of 
    immediate mode. [t:549]
  * Removed Abstract* classes in favor of I* classes. This is to conform to our 
    coding standards. [t:550]
  * After a new layer is added it is now activated for all renderers. This fixes
    the problem of no instances being rendered for a newly created layer. 
    [t:551]
  * Added the option to change the alpha value for instance picking.
  * Added a setting for backbuffer clearing. So clients can deactivate it if 
    they have reasons for this.
  * Removed the old setImageChunkingSize() setting because its useless.
  * Changed the pump function. Model update gets only called if a map is loaded.
  * Adding a quick and dirty shared pointer implementation that does not rely 
    on any 3rd party libraries. This will need to be extensively tested and can 
    always be swapped out for another implementation including 
    boost::shared_ptr.
  * Added the unittest for SharedPtr.
  * Instances now inherit blocking property from objects [t:514]
  * Settings are now validated. [t:528]
  * VFS now allows paths outside of launched applications when using a python
    client. [t:544]
  * Reworked the lighting module.  [t:557]
  * The Windows icon is now freed correctly. [t:530]
    
### Demos
  * Fixed a bug in the shooter demo that caused a crash. [t:495]
  * Updating the rio settings-dist.xml file to include the log level filter 
    setting.
  * Added animated percentagebar widget to gui animation demo.
  * Pychan demo now closes when you press escape or when you close the window. 
    [t:531]
  * Shooter demo now generates a hichscore file if one does not exist.

### Documentation
  * Documented the shooter World.reset() function.
  * Updating the dependency graphs. [t:554]
  * Documented all valid FIFE settings. [t:527]

### Extensions
  * Updated basicapplication.py to initialize the logger before settings so 
    that setting messages will be logged.
  * Removed some deprecated functions in pychan.
  * Removed the deprecated ClickLabel widget from pychan. [t:552]
  * Added changeBlending() to python.

### Tools
  * Editor no longer crashes when there are too many instances on the map when
    using the OpenGL renderer. [t:500]
  * Added Atlas Creator tool. Currently it's a standalone application written 
    in Qt. [t:541]  

## [0.3.2] - 2010-11-19

### Engine
  * Removed the Camreazone renderer
  * Added light support (OpenGL only) by merging the light branch.  Speacial
    thanks to Helios for all his hard work in this area (#364)
  * Added a percentage bar widget (#411)
  * Modified some size_t definitions to more appropriate ones (#152)
  * Instance rotation changes are reported to the instance change listeners 
    (#475)
  * Fixed the GenericRenderingNode to no longer cause a segfault (#446)
  * Fixed an API inconsistency in SoundEmitter (#461)
  * There is no longer a memory leak when adding coloring overlays to instance 
    (#472)
  * instance.get2dGfxVisual().setVisible(False) now works as expected (#471)
  * Removed the last 0(n) loop through instances when rendering (#419)
  * Generic renderer now only renders objects on the specified layer (#458)
  * Fixed a memory leak when deleting maps.  The camera was not being
    deleted
  * Removed the NotFound exceptions in layer.getInstance() and 
    model.getObjects() to be more consistent with the rest of the API
  * Fixed a bug in RenderBackendSDL, the clip area was not properly set
  * Renamed the backquad and border commands of the FloatingTextRenderer
  * Added the TransparentArea commands to the InstanceRenderer
  * Improved InstanceRenderer::addTransparentArea.  You can now pass a list
    of namespaces to be marked as transparent
  * Updated Instance to no longer derive from ResourceClass. It now derives from 
    FifeClass directly
  * Improved the floatingtextrenderer. Now you can set an alpha value for font, 
    background quad and border. Split quad and border into own commands
  * Added a triangle function to backend and genericrenderer, also added the 
    functions to set an alpha value for geometric figures
  * floatingtextrenderer now can change colors of the font via 
    setColor(uint r, uint b, uint g)
  * Added the colorbuffer patch with a small change due to SDL. Performance 
    boost between under OpenGL
  * Improved the most renderers with setColor() function to allow the user to
    modify the color used to render the primitives
  * Fixed the genericrenderer.  It is now tested whether the images are in the 
    viewport
  * Fixed the gridrenderer to only render the grin within the viewport
  * Changed the vertex functions in opengl/renderbackendopengl.cpp from 
    vertex3f() to vertex2f()
  * Moved the layer visibility and transparency, from instancerenderer.cpp to 
    layercache.cpp and improved it (r3395) 
  * Corrected the h,w calculation for zooming in layercache.cpp (r3369)
  * Fixed the bug, that x,y camera position result in an screen offset. (r3369)
  * GLImage now only creates one "texture chunk" (r3445, r3452)
  * Added nextPow2() math function (r3445)
  * Fixed color mask definitions and use them throughout the engine (r3445)
  * Moved the GLEnable and GLDisable structures to be globally accessible 
    (r3445)
  * Templatized Rect to extend it's functionality beyond integers (r3447)
  * Added some typedefs: Rect, FloatRect, DoubleRect (r3447)
  * Removed the unused "trigger" system as it was unused (r3449)
  * Applied GreyGhosts patch to fife_settings. Allows the user to load the gui 
    XML whenever they want. Also returns a reference to the dialog so it can be 
    worked with outside of fife_settings. (r3451)
  * Removed unused files containing the unused GUIEventListener class (r3453)
  * Removed old and unused functions: set/getSourceWidget() (r3455)
  * Added drawRectangle() and fillRectangle() functions to the renderers (r3456)
  * Modified FIFE::FloatingTextRenderer? to use the new functions (r3456)
  * Updated some data types in FIFE::FloatingTextRenderer to use integer types 
    defined in fife_stdint.h (r3456)
  * Added FIFE::DeviceCaps and FIFE::ScreenMode to detect valid screen modes
  * Added the ability to query the current running screen mode (r3467)
  * Added a method to detect the closest supported screen mode (not 
    complete yet). If no matching screen modes are detected an exception is 
    thrown. (r3467)
  * Small change to the way the screen is initialized. The screen mode now 
    MUST be in the supported screen mode list before the screen will 
    initialize. (r3467)
  * Added clearResourceLoaders() to the Pool class. This can be used if 
    different resourceloaders have to be added at a later point in time 
    (running editor for example, a plugin might want to have it's own loader 
    added) (r3469)
  * Added the ability to normalize a 2D and 3D point (r3470)
  * You can now rotate a 2D point around the origin or a specific point (r3470)
  * Major improvements to fife_math.h and added corresponding Python bindings. 
    Users now have access to FIFE's internal math functions. These functions 
    are recommended to be used by all clients if required. Note: this may cause 
    some problems with certain compilers. I hope this wont have to be reverted. 
    TODO: remove the static constant globals somehow. (r3471)
  * Adopted the new math functions for all subsystems (r3471)
  * Added the ability to change screen modes on the fly. This works both in 
    OpenGL and SDL modes. (#315)
  * Added IEngineChangeListener so the client can update the cameras viewport 
    if the screen mode has been changed. I chose to do it this way because the 
    engine has no way to know which camera it should update. It will be up to 
    the client to do it. (#315)
  * The cursor surface is now correctly freed when exiting.  (r3483)
  * Added DeviceCaps::getNearestScreenMode() for the client to request a 
    supported screen mode. (#315)

### Build System
  * Removed libpng from ext (#385)
  * Debug is now automatically enabled when you build with profiling enabled
    (r3292)

### Editor
  * Re-arranged the ObjectEdit gui to avoid misunderstandings
  * Can now place instances (which are visible) on new maps again (r3392)  
  
### Extensions
  * Added the SimpleXMLSerializer extension
  * Removed the XML portion out of fife_settings in favor of SimpleXMLSerializer
  * Made the serializer used in fife_settings "pluggable"
  * fife_settings widgets are now updated when the settings are reset to default
  * Can now specify the default settings filename instead of using 
    settings-dist.xml
  * Fixed the write_instances function so it saves the stack position variable
    of instance visual when saving maps
  * Added getVolume() to the SoundManager
  * Added debug flag to loaders to enable/disable annoying print spam on 
    maploading
  * BasicApplication now initializes pychan (r3388)
  * loaders extension can now handle multiple loaders for different filetypes. 
    A difference between map and object files is now being made! (r3469)
  * Modified the sound manager to better take care of sound clips (r3476)
  
### Docs
  * Added some documentation to the filebrowser module
  
### Pychan
  * Timers are now handled correctly by pychan (#483)
  * Can now modify the step length for sliders (#482)
  * Built in pychan dialogs now work (r3384)
  
### Demos
  * Renamed <fife>/demos/pychan_demo/pychan_test.py to pychan_demo.py
  * Added a new pychan demo - gui animations
  * Added the Shooter demo
  * Made the dynamic widget test in the pychan_demo work/look a little better 
    (r3484)
  
### Misc
  * Fixed some compiler warning messages
  * Added license information for the gimp icon
  
## [0.3.1] - 2010-03-01

### Engine
  * Updated a lot of the python source files to have the correct headers
  * Fixed SWIG wrappers that didn't handle simple output parameters (#340)
  * Added color key support for both OpenGL and SDL renderer (#451, #146)
  * Changed the EventManager to use deque rather than vectors for storing 
    listeners (#454)
  * Added the ability to prepend event listeners to the front of the listener 
    list (#454)
  * Updated instance rotation values to be valid in all cases (#361)
  * VFS no longer crashes when entering a restricted path (#413)
  * Filename case is no longer transformed to be lower case (#418)
  * OpenGL renderer now clears the backbuffer correctly (#392)
  * Added the ability to render Vertexes (#455)
  * Camera names no longer clash when loading multiple maps (#342)
  * All layers are no longer activated when calling Camera::resetRenderers() 
    (#457)
  * Word wrapping in a pychan label now works as expected (#408)
  * SWIG wrappers now handle all simple output parameters (#340)
  * Added ActionVisual::getActionImageAngles()
  * Added Cursor::getX() and Cursor::getY()

### Build System
  * FIFE now builds as expected on win32 using the FIFE dev kit
  * Win32 batch scripts no longer overwrite the users PATH variable (#429)
  * Requied DLLs are now installed as required on win32 (#430)
  * CXXFLAGS should now be honored
  * Fixed a problem with the dependency checks when using pkg-config on linux
  * Added OpenBSD support

### Editor
  * Can now re-load a map (#381)
  * Opening more than one map and then closing them no longer causes a crash 
    (#441)
  * Undo/Redo works properly if you select different layers (#443)
  * Docked widgets no longer shrink when you click inside them (#444)
  * Object editor can now skip to first and last frames of an animation (#448)
  * Object editor sends some messages for the user to the editor status bar
  * Cleaned up object editor by adding preMapClosed and preMapShown events
  * Object editor now uses getActionImageAngles() to fetch animation angles
  * Object editor animation views should now work with all clients
  * Object offsets can now be saved to the object.xml file
  * Object offsets can now be manipulated by using the mousewheel
  * Added undo/redo support for the moving of instances
  
### Extensions
  * File Browser now refreshes correctly (#391)
  
### Docs
  * Fixed the Linux:Scons tutorial (#422)

### Pychan
  * Fixed a couple python import lines (#440)
  * Added Widget.requestFocus()
 
### Rio De Hola
  * Fixed the path to the AUTHORS file so the credits are displayed correctly
  
### Misc
  * Added color tester to pychan demo
  * Demos now look for a local copy of FIFE before importing the installed 
    version of FIFE

## [0.3.0] - 2010-01-20

### Engine
  * Moved to Python 2.6 (#345)
  * Upgraded guichan in ext to 0.8.2 (#420)

  * Added UTF8 support
  * SCons build system has been re-written (#328)
  * Support for native mouse cursors
  * The background of Label widgets will be painted
  * Added adaptive pathfinding (#347)
  * Modified CellSelectionRenderer to support multiple cell selection

  * Added method Layer::getInstancesAt()
  * Added SoundEmitter::setCallback()
  * Added a setRolloff() function to the SoundEmitter class.
  * Added function to fife_utils: getUserDataDirectory. Returns the proper path 
    to save setting and data files depending on OS. 
  * Removed UnitTest++ from ext (#421)
  * Changed the way the fife python module is imported
  * The serializers will use LogManager instead of printing directly to the 
    console

  * GuiManager: Proper translation of Guichan key events to FIFE key events
  * Proper size calculation for ToggleButton
  * Fixed a layer visibility bug where the toggleInstancesVisible and 
    setInstanceVisible functions were not working correctly. (#335)
  * Fixed building FIFE on OS X (#360)
  * Fixed setActionRuntime() (#373)
  * Fixed unreliable time handling when changing time factors.
  * Fixed a bug in the OpenGl renderer where the last chunk of images would not 
    be displayed when image_height % chunk_size was 0.
  * Fixed layer and instance transparency (#378)
  * Fixed a lot of small bugs

### Rio de hola
  * Made rio_de_hola save its settings to the proper OS folder.
  * Added a prefix to cameras in Rio de Hola to avoid name clashes
  * Fixed second camera in rio so multiple Camera.setLocation calls are handled 
    correctly
  * More documentation has been written
  
### Editor
  * Rewrote editor:
    - Improved GUI:
      - Resizable windows
      - Docking
      - Toolbars, menubar, statusbar
      - Model/View approach to toolbar and menubar buttons
    - New signal event system based on the one from django
    - Support for opening multiple maps
    - Better plugin support
  * New plugin: LayerTool
  * New plugin: LightEdit
  * New plugin: CameraEdit
  * Enhanced ObjectSelector
  * Enhanced ObjectEdit
  * New tool: ObjectPicker. Click on an instance on the map to set the current 
    object to the instance object
  * Ability to select cells and perform actions on them (move, delete, fill, 
    ...)
  * New type of scrolling: Press middle mouse button and move the mouse
  * New icons based on Bluesphere
  * Made the editor save its settings to the proper OS folder.
  * Fixed editor bug where the map didn't render after saving (#333)
  * Editor can now re-open a map (#381)
  * Fixed a problem navigating directory structure with the filebrowser (#377)
  * Filebrowser now refreshes when expected (#391)
  * Fixed a lot of bugs, and made a lot of new ones ;-)
  
### PyChan
  * New greedy layout engine: Widgets is now able to expand in addition to 
    shrinking
  * New events: mouseWheelMovedUp and mouseWheelMovedDown

  * Added the position_technique attribute
  * Added Widget.removeAllChildren() method
  * Added a new method Widget.getNamedChildren
  * New method: Widget.insertChild()
  * New method: Widget.insertChildBefore()
  * New property: is_focusable
  * New properties: min_size, max_size
  * Moved helptext property from C++ to pychan
  * New togglebutton widget

  * ScrollArea: Size is not static anymore (can shrink and expand)
  * Allow objects with a call attribute to be used as event callbacks
  * Removed unused TimerListener
  * Switched to new content structure (#336)
  * Made the distribute and mapEvents methods faster
  * Added a small decorator for profiling rarely called functions
  * Optimized adaptLayout()
  * Added fast path for Widget.findChild(name=name). Roughly 10x faster

  * Fixed an input problem when a dropdown box is clicked on (#337)
  * Fixed pychan event_map to support callable objects (#374)
  * Fixed the pychan demo (#363)
  * Fixed a pychan problem where unknown characters were displayed when holding 
    the shift key (#383)
  * Lots of bug fixes
  
### Documentation
  * Fixed some doxygen documentation problems (#343)
  * Created python coding standards article (#372)
  * Created "Bug Report" template (#409)
  
### Misc
  * New versioning scheme (x.y.z) instead of (yyyy.r)
  * Moved extensions and fife.py to <FIFE>/engine/python/fife
  * Moved rio_de_hola and pychan_demo to <FIFE>/demos
  * Moved some misc scripts to <FIFE>/tools
  * Moved the FIFE editor to <FIFE>/editor
  * Added a Win32 installer for FIFE and the win32 SDK

## [2009.0] - 2009-01-07

### Engine
  * Merged input rework branch
  * Possibility to hide instances/instance visuals
  * Added input and focus events
  * Modified clicklabel to provide a mouse listener for hover events
  * Use strings as grouping in generic renderer instead of numbers
  * Omission of a camera viewport in xml map files now indicate that the camera 
    should use the full window
  * Added removeActionListener to SWIG export
  * Some improvements to Scons build scripts
  * Prepared basicapplication to use WindowTitle and WindowIcon from clients' 
    settings.py
  * Logger now ignores non-existing log modules (warns to prompt)
  * Pools now have a function called purgeLoadedResources which deletes all 
    loaded resources that have a reference count of zero.
  * Movie: Switched to using FIFE::Image instead of Overlays
  * Reworked matrix vector multiplication
  * OpenAL-soft patch for OSS support

Optimizations:
  * Added hasing to pool to improve techdemo loading time
  * Lazy loading for animations
  * Combined events to reduce lag
  * Small optimizations for instance renderer

Bugs:
  * Initialize instance visuals properly
  * Fixed false positives when searching for instances in a rectangle
  * Fixed a horrendous misconception in Pool. The map was sorted by pointer 
    comparison.
  * Fix for textwrapping
  * Fixed bug where instances couldn't change layer
  * Fixed a small bug when initializing OpenGL
  * Fixed a scons issue where libraries checked for C instead of C++
  * Fixed a shutdown order problem
  * Fixed segfault on shutdown when profiling was activated (#334)
  * Fixed: Editors camera doesn't show map after saving (#333)
  * Fixed: FIFE crashes with a segmentation fault (#297)

### Pychan
  * Modified pychan to accept RGBA colors
  * More documentation for layout system
  * adaptLayout now acts a bit smarter
  * Fix for scrollarea bug

### Rio de hola 
  * Improved some graphics and added some new ones
  * Profiling support

### Editor
  * Scrolling support
  * Added an undo feature to the map-editor
  * Object editor plugin
  * Some bug fixes

### Documentation
  * Updated the map format article to reflect the datasets changes (#323)

### Misc
  * Removed old testmaps
  
## [2008.1] - 2008-06-01

### Model improvements & simplifications
 * Elevations removed (Fallout legacy).
 * Support for namespaces:
  * New resource infrastructure migrated into whole engine. This enables:
   * uniform pooling for all resources that need to be pooled.
   * uniform loading for all resources (model stuff, images, sounds...).
   * uniform saving for resources which support saving (model stuff).
  * Datasets are now stored in a flat list in model instead of dataset 
    hierarchy.
  * Changes in model are now bookkeeped. Observer interfaces provided for change 
    listeners.
   * This enables e.g. reacting only to changes instead of polling on every 
     round.
   * Also critical enabler for triggers.
  * Instances now support initial rotation value.
  * Objects support default actions.
  * Further model cleanup.

### XML format changes
 * All resources are now defined with principle 1 file / 1 resource.
 * Importing of objects can now be done also based on directories where they are 
   contained.
 * Relative paths are now used instead of absolute ones.

### Loaders
 * All loading happens now from python (tinyxml dependency removed).
 * Improved error reporting.

### Editor
 * Possibility to move instances on map.
 * Possibility to rotate instances on map.
 * Possibility to zoom the map (OpenGL mode only).
 * Painting with currently selected instance (e.g. large ground areas).
 * Importing of multiple objects based on given directory.
 * Initial map creation wizard (still work in progress).

### Rio de hola
 * More assets:
  * Graphics, sounds, music...
  * Two new maps matching the storyline.
  * Rio de hola now conforms to package structure
  * Popup menu integration, demonstration of actions (kick, talk).

### Pychan improvements
 * Hiding of scrollbars.
 * Text wrapping.
 * A lot of smaller improvements. 

### Functionality
 * Pixel perfect instance picking (taking transparency into account).
 * Ability to get list of instances from given screen rectangle.
 * Distance calculations made easy, exposed to scripts.
 * Instance outlining.
 * Instance coloring.
 * Screenshots are now saved in png format (instead of bmp).
 * Stereo sound support in linux (introduced with the help of openal-soft).

### Video / view improvements
 * Both renderbackend and separate images can now render graphical primitives 
   (e.g. lines, dots..).
 * Each camera now has separate set of renderers. Allows truly adjusting cameras 
   independently.
 * Zooming capability (currently properly supported only by OpenGL backend).
 * Support for drag cursor definitions.
 * OpenGL graphics chunking size is now definable in settings. Should help e.g. 
   with large backgrounds.
 * GenericRenderer adding customised rendering capabilities for scripts.

### Misc. changes
 * Initial package structure defined for FIFE clients.
 * Updated guichan to the latest release 0.8.1.
 * Usage of unittest++ instead of boost unittests.
 * Lots of bug fixes.

### Non-code related changes
 * Switch from GPL 2.0 to LGPL 2.1 (or newer).

[Unreleased]: https://github.com/fifengine/fifengine/compare/0.3.5...HEAD
[0.3.5]: https://github.com/fifengine/fifengine/compare/0.3.4...0.3.5
[0.3.4]: https://github.com/fifengine/fifengine/compare/0.3.3r3...0.3.4
[0.3.3r3]: https://github.com/fifengine/fifengine/compare/0.3.3r2...0.3.3r3
[0.3.3r2]: https://github.com/fifengine/fifengine/compare/0.3.3...0.3.3r2
[0.3.3]: https://github.com/fifengine/fifengine/compare/0.3.2...0.3.3
[0.3.2]: https://github.com/fifengine/fifengine/compare/0.3.1...0.3.2
[0.3.1]: https://github.com/fifengine/fifengine/compare/0.3.0...0.3.1
[0.3.0]:  https://github.com/fifengine/fifengine/compare/2009.0...0.3.0
[2009.0]: https://github.com/fifengine/fifengine/compare/2008.1...2009.0
[2008.1]: https://github.com/fifengine/fifengine/compare/2008.1
