gui.actions.on_loadxml = function()
  guimanager.remove(con)
  set_next_mapfile(level_list:getElementAt(level_drop:getSelected()))
  state_manager.activate('MapView')
end

function main_menu()
  big_font = TTFont('content/fonts/FreeMono.ttf', 36)
  big_font:setColor(15, 24, 255)

  level_list = ListModel('content/maps/scratch.xml', 'content/maps/noise1.xml', 'content/maps/noise1_a.xml')

  level_drop = DropDown(level_list)
  level_drop:setPosition(30, 100)
  dark_color = Color(10, 20, 20)
  medium_color = Color(60, 70, 60)
  level_drop:setBackgroundColor(dark_color)
  level_drop:setBaseColor(medium_color)
  level_drop:setSize(300, 40)
  
  --bg_image = Image('content/gfx/misc/lg-demo/voronoi.jpg')
  --background = Icon(bg_image)
  
  con = Container()
  con:setSize(800, 600)
  con:setOpaque(false)

  con:add(background)

  inner_col = Color(10, 80, 30, 128)

  inner = Container()
  inner:setSize(750, 550)
  inner:setPosition(25, 25)
  inner:setBaseColor(inner_col)
  con:add(inner)

  title = Label('Choose a XML level:')
  title:setFont(big_font)
  title:setPosition(30, 10)
  inner:add(title)

  inner:add(level_drop)

  button = Button('Load')
  button:setPosition(300, 200)
  button:setBaseColor(medium_color)
  button:setEventId('on_loadxml')
  inner:add(button)
  
  guimanager.add(con)
  
end


function on_engine_start()
  dofile('content/scripts/bindings.lua')
  main_menu()  
end
