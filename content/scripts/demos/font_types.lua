require "gui_utils"

function on_engine_start()
  local x, y
  x = 50
  y = 50
  gui.top = Container()
  guimanager.add(gui.top)
  gui.top:setSize(screen_width, screen_height)
  gui.top:setPosition(0, 0)
  gui.top:setOpaque(false)

  local red_color = Color(255, 20, 20)

  -- gui.font1 = TTFont('content/fonts/FreeMono.ttf', 18)
  gui.font1 = make_font { file = 'content/fonts/FreeMono.ttf',
    size = 18, color = red_color }

  --gui.font2 = AAFont('font0.aaf')
  gui.font2 = make_font { file = 'font0.aaf',
    color = { r = 30, g = 200, b = 128 } }

  --gui.font3 = ImageFont('content/fonts/rpgfont.png', ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"')  
  gui.font3 = make_font { file = 'content/fonts/rpgfont.png',
    glyphs = ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"'
  }


  gui.label1 = Label('This is a Truetype font')
  gui.label1:setPosition(x, y + 20)
  gui.label1:setFont(gui.font1)
  gui.top:add(gui.label1)
  
  gui.label2 = Label('This is a Fallout AAF font')
  gui.label2:setPosition(x, y + 60)
  gui.label2:setFont(gui.font2)
  gui.top:add(gui.label2)

  gui.label3 = Label('This is a Guichan image font')
  gui.label3:setPosition(x, y + 100)
  gui.label3:setFont(gui.font3)
  gui.top:add(gui.label3)

end
