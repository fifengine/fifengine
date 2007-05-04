-- created by engine:
-- gui = { actions = {}} 

-- one callback for both buttons
gui.actions.on_button_click = function () 
  demo_toggle_visible()
end

-- create container, set a few basic variables,
-- define the color and add a button.

-- twice!

gui.screen1 = Container()
gui.screen2 = Container()
gui.screen1:setPosition(0, 0)
gui.screen2:setPosition(0, 0)
gui.screen1:setSize(640, 480)
gui.screen2:setSize(640, 480)

gui.first_button = Button('Click for screen2')
gui.first_button:setPosition(10,10)
gui.first_button:setEventId('on_button_click')
gui.screen1:add(gui.first_button)

gui.second_button = Button('return to screen1')
gui.second_button:setPosition(100, 50)
gui.second_button:setEventId('on_button_click')
gui.screen2:add(gui.second_button)

gui.c1 = Color(199, 78, 12)
gui.c2 = Color(60, 150, 210)

gui.screen1:setBaseColor(gui.c1)
gui.screen2:setBaseColor(gui.c2)

-- If I tell you that 'deactivate' and 'activate' are supposed
-- to be 'Container' objects, this function should be pretty clear.
function set_visible_container (deactivate, activate) 
  deactivate:setVisible(false)
  activate:setVisible(true)
end

-- FIXME
-- I think this creates a global var in the (local) ~namespace~ of this 
-- scriptfile... though it looks pretty global to me.
local menu_switch = 1

-- above variable is used here to make one container visible and
-- hide the other one.
function demo_toggle_visible ()
  if (menu_switch == 1) then
    set_visible_container(gui.screen2, gui.screen1)
    menu_switch = 2
  else
    set_visible_container(gui.screen1, gui.screen2)
    menu_switch = 1
  end
end

-- setup for first screen (when the script is run)
demo_toggle_visible()
-- and call the guimanager to add both containers
guimanager.add(gui.screen1)
guimanager.add(gui.screen2)
