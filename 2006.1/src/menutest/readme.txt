This is actually a dev/mod-readme for the 'luascript' GUI wrapper code
(basically luagui.h and a few supporting classes in the FIFE namespace);
menutest is just a simple test-loader.

Usage:
* link/copy 'content' module into this directory
* optionally link/copy 'master.dat' into this directory
* start as: ./menutest content/scripts/XXX.lua

Look at src/luascript/luagui.h for docs on currently implemented
classes; they generally wrap around the gcn class of the same name.

API: http://wiki.fifengine.de/index.php?title=LuaGui#API_documentation

Notes:
* fife::gui::gcn_top_container can be accessed by
  guimanager.add(some_widget_in_lua)
  guimanager.remove(some_widget_in_lua)

That is the only (gui-related) lua-object with ~static~ access 
("." operator); all others a runtime-created objects, where accessing 
member functions works with ":".

Furthermore you are _required_ to hold a ~reference~ to each obj in a
non-local var; if you do not, the c++ object belonging to the lua obj
will be deleted (when the lua garbage collector removes out-of-scope
variable data) -- guichan might not appreciate that.

I think something like this might work:

-- make a (empty) global table
-- Note: this name is currently hardcoded!
-- Note2: you don't need to do this any more; just assume the following
-- two tables exist!

-- gui = {}
-- make a table for the action callbacks
-- NOTE: this name is currently hardcoded!
-- gui.actions = {}


-- write a function for each eventId set on a widget;
-- you should do this for every defined 'EventId' and each
-- event-sending widgets should have a well defined id.
-- for the moment calling any non-function provokes a warning when called.

gui.actions.on_test_button = function()
  print('on_test_button')
  -- any valid lua; no parameters, no return values!
end

-- the following variable names are not predefined

-- make a container and store in the table
gui.main = Container()
-- gui['main'] = Container()  -- is the same as above

-- set a few values
gui.main:setSize(300, 200)
gui.main:setPosition(40, 20) -- this will be main-screen relative
gui.main:setVisible(true) -- you could do this later...
gui.main:setOpaque(true)

-- and add it to the gui
guimanager.add(gui.main)

-- now a widget
gui.label1 = Label('Hello World')
gui.label1:setPosition(30, 10) -- relative to where you add it

-- add it to the container
gui.main:add(gui.label1)

-- and a button
gui.button1 = Button('Click me')
gui.button1:setPosition(30, 100)
-- make a click call the function above
gui.button1:setEventId('on_test_button')
gui.main:add(gui.button1)
--------------------------

Now there is 'TwoButton':

gui.image_up = Image('button_up.png')
gui.image_down = Image('button_down.png')
gui.myButton = TwoButton(gui.image_up, gui.image_down)

FRM graphics

Should now work by simply using a valid frm path:
gui.frm_image = Image('art/xxx/yyy.frm')
 
TODO:
* more widget types (and support classes)
* error checking/handling
* expose fife::gui::m_gcn_topcontainer directly (as a predefined 
  container)

  This may happen later, but possibly another hack (static members
  in guimanager_luascript) is the easiest way.

Why I want it:
 It would give the scripted gui access to getSize() and such the
 full gui (==window) dimensions; and give stacking ability
 (moveToBottom/Top) without further code.

So what?:
 If exported as a true object, more code is required to ensure the
 wrapped object (the fife::gui::gcn_topcontainer) isn't deleted when
 the wrapper-object goes out-of-scope.

 Somehow the top-container _is_ special; applying my current strategy
 for those types leads to a static member impl. (at least consistent ;-)
