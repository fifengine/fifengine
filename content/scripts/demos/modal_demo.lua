
gui.actions.make_top_modal = function()
  local ok = gui.top:requestModalFocus()
  if ok == false then
    print('FAILED TO GET MODAL FOCUS: GREEN')
  end
end

gui.actions.release_top_modal = function()
  gui.top:releaseModalFocus()
end

gui.actions.make_modal_error = function()
  local ok = gui.top:requestModalFocus()
  if ok == false then
    print('FAILED TO GET MODAL FOCUS: GREEN')
  end

  local ok = gui.top2:requestModalFocus()
  if ok == false then
    print('FAILED TO GET MODAL FOCUS: RED')
  end
end

function on_engine_start()
  green = Color(0, 255, 0)
  red = Color(255, 0, 0)

  gui.top = Window()
  guimanager.add(gui.top)
  gui.top:setSize(screen_width/3, screen_height/2)
  gui.top:setPosition(0, 0)
  gui.top:setBaseColor(green)
  gui.b1 = Button('test1')
  gui.b1:setEventId('test1')
  gui.top:add(gui.b1)

  gui.b1_make_modal = Button('make modal')
  gui.b1_release_modal = Button('release modal')
  gui.b1_make_modal:setPosition(0, 100)
  gui.top:add(gui.b1_make_modal)
  gui.b1_make_modal:setEventId('make_top_modal')
  gui.b1_release_modal:setEventId('release_top_modal')
  gui.b1_release_modal:setPosition(0, 200)
  gui.top:add(gui.b1_release_modal)

  
  gui.top2 = Window()
  guimanager.add(gui.top2)
  gui.top2:setSize(screen_width/3, screen_height/2)
  gui.top2:setPosition(screen_width/3*2, 0)
  gui.top2:setBaseColor(red)
  gui.b2 = Button('test2')
  gui.b2:setEventId('test2')
  gui.top2:add(gui.b2)

  gui.b2_make_modal = Button('make both modal')
  gui.b2_make_modal:setEventId('make_modal_error')
  gui.b2_make_modal:setPosition(0, 100)
  gui.top2:add(gui.b2_make_modal)



end
