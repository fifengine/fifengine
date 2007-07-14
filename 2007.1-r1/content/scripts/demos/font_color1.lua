-- this demonstrates TTFont color support
-- run it as: ./guitest content/scripts/demos/font_color1.lua

function on_engine_start()
  gui.top = Container()
  guimanager.add(gui.top)
  gui.top:setSize(640, 480)
  gui.top:setPosition((screen_width - 640 ) / 2, (screen_height - 480) / 2)
  gui.top:setOpaque(false)


  local size = 14

  for i = 1, 20 do
    local name = "label" .. i
    local fontname = "font" .. i
    gui.name = Label('A quick brown fox jumps over the lazy dog')
    gui.fontname = TTFont('content/fonts/FreeMono.ttf', size)
    
    -- both of the following ways work:
    
    --gui.fontname:setColor(25 * i, 10 * i + 100, 20 * i)

    local col = Color(25 * i, 10 * i + 100, 20 * i)
    gui.fontname:setColor(col)

    gui.name:setPosition(30, i * 30)
    gui.name:setFont(gui.fontname)
    gui.top:add(gui.name)
  end

end
