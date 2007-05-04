-- creates labels with different font sizes
-- run as: ./guitest content/scripts/demos/font_size.lua

-- BUG: something is broken for large font sizes

function on_engine_start()
  gui.top = Container()
  guimanager.add(gui.top)
  gui.top:setSize(screen_width, screen_height)
  gui.top:setPosition(0, 0)
  gui.top:setOpaque(false)



  for i = 1, 10 do
    local name = "label" .. i
    local fontname = "font" .. i
    local size = (i+5)*2
    gui.name = Label('A quick brown fox jumps over the lazy dog (size: '..size..')')
    gui.fontname = TTFont('content/fonts/FreeMono.ttf', size)
    gui.name:setPosition(30, i * 30)
    gui.name:setFont(gui.fontname)
    gui.top:add(gui.name)
  end

end
