gui.actions.on_test_button = function()
  print('lua: on_test_button')
end
-- note that there is no 'on_inside_button' ... but there could be
-- it would even work, but I want show the warning; only a lua function
-- is allowed.

c_black = Color()
c_rgb= Color(100,120,60)
c_rgba = Color(50,10,230,40)

gui.c1 = Container()
gui.c1:setBorderSize(0)
guimanager.add(gui.c1)
gui.c1:setPosition(0, 0)
gui.c1:setSize(800, 600)
gui.c1:setVisible(true)
gui.c1:setOpaque(false)

--gui.bg_image = Image('hubble_m16_eagle-nebula.jpg')
--gui.background = Icon(gui.bg_image)
--gui.background:setBorderSize(0)
--gui.c1:add(gui.background)
--gui.background:setPosition(20,20)

gui.some_label = Label('Hello!')
gui.some_label:setPosition(360, 100)
gui.c1:add(gui.some_label)

gui.b = Button('test')
gui.b:setBaseColor(c_rgb)
gui.b:setPosition(10, 100)
gui.b:setEventId('on_test_button')
guimanager.add(gui.b)

gui.b2 = Button("inside")
gui.b2:setBaseColor(c_rgba)
gui.b2:setPosition(40, 200)
gui.b2:setEventId('on_inside_button')
gui.c1:add(gui.b2)
