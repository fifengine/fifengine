-- this demonstrates TTFont color support
-- run it as: ./guitest content/scripts/demos/font_color1.lua

function on_engine_start()
  gui.top = Container()
  guimanager.add(gui.top)
  gui.top:setSize(640, 480)
  gui.top:setPosition((screen_width - 640 ) / 2, (screen_height - 480) / 2)
  gui.top:setOpaque(false)

  gui.dark = Color(30, 80, 40)
  gui.invisible = Color(20, 70, 20, 30)


  gui.scroll = ScrollArea()
  gui.scroll:setSize(600, 350)
  gui.scroll:setPosition(10, 10)
  gui.scroll:setBackgroundColor(gui.invisible)

  gui.text = TextBox("")
  gui.scroll:setContent(gui.text)
  gui.text:setBackgroundColor(gui.dark)
  gui.text:setOpaque(true)

  gui.top:add(gui.scroll)

  gui.another_font = TTFont('content/fonts/FreeMono.ttf', 25)
  gui.another_font:setColor(250, 200, 50)


  gui.cb = CheckBox()
  gui.cb:setPosition(40, 430)
  gui.cb:setFont(gui.another_font)
  gui.cb:setCaption("Just another checkbox")
  gui.top:add(gui.cb)

  gui.r1 = RadioButton("R-Button 1", "group1")
  gui.r2 = RadioButton("R-Button 2", "group1")

  gui.r1:setPosition(130, 400)
  gui.top:add(gui.r1)
  gui.r2:setPosition(300, 400)
  gui.top:add(gui.r2)

  slider = Slider()
  slider:setSize(100, 10)
  slider:setPosition(10, 380)
  gui.top:add(slider)

end
