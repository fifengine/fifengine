-- created by engine:
-- gui = { actions = {}} 

gui.actions.on_button_click = function () 
  demo_toggle_active()
end

-- identical to 'toggle_visible_container.lua' ...
-- the point is to test/demonstrate the same functionality
-- but by removing/adding the widgets instead of setting 
-- their visibility.

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

--
-- start to read again!
--

function change_active_container (deactivate, activate)
  guimanager.remove(deactivate)
  guimanager.add(activate)
end

-- FIXME
-- I think this creates a global var in the (local) ~namespace~ of this 
-- scriptfile... though it looks pretty global to me.
local menu_switch = 1

function demo_toggle_active()
  if (menu_switch == 1) then
    change_active_container(gui.screen1, gui.screen2)
    menu_switch = 2
  else
    change_active_container(gui.screen2, gui.screen1)
    menu_switch = 1
  end
end

guimanager.add(gui.screen1)
