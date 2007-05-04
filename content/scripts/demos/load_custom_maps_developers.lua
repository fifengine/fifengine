-- This file itself could need a clean up. But as it is, this tool is really handy.
mapfile = "content/etc/maps_custom_developers.txt"

function load_map( mapname )
	if state_manager.isActive('MapView') then
		state_manager.deactivate('MapView')
	end
	set_next_mapfile(mapname)
	state_manager.activate('MapView')
	label:setText("Current map: " .. mapname .. "\n-- More information will follow here\n-- once the bindings are more complete")
-- 	state_manager.deactivate('MapView')
end  

gui.actions.on_loadmap = function()
  load_map(level_list:getElementAt(level_drop:getSelected()))
end

gui.actions.on_stresstest = function()
  -- Stop timer
  lchooser.timer:stop()

  -- Read maps
  local file = io.open(mapfile)
  local i = 0
  lchooser.toload = {}
  for level in file:lines() do
    lchooser.toload[i] = level
    i = i + 1
  end
  lchooser.toload.n = i

  -- Define callback
  local cb = function()
      lchooser.toload.n = lchooser.toload.n - 1
      mapname = lchooser.toload[lchooser.toload.n]
      console.print("Loading " .. mapname)
      load_map(mapname)
      if lchooser.toload.n == 0 then 
        lchooser.timer:stop()
        return
      end
  end

  -- Start timer
  cb()
  lchooser.timer:setInterval(10000) -- 10 seconds
  lchooser.timer:setCallback(cb)
  lchooser.timer:start()
end

function create_level_listmodel(filename)
	local file = io.open(filename)
	local lm = ListModel()
	for level in file:lines() do
		lm:addElement(level)
	end
	return lm
end  

function create_window(title,sx,sy, font)
  gui.con = Container()
  gui.con:setSize(sx-2,sy-2)
  gui.con:setPosition(2, 2)
  
  gui.top = Window(gui.con, title)
  gui.top:setFont(font)
  gui.top:setBaseColor(dark_color)
  
  gui.top:setSize(sx, sy)
  gui.top:setPosition(0, 0)
  --gui.top:setOpaque(true)
  gui.top:setTitleBarHeight(20)
  --gui.top:setPadding(0)
  guimanager.add(gui.top)
  gui.lchooser_active = true
  return gui.con
end

gui.actions.close_level_chooser = function()
  if gui.lchooser_active == true then
    guimanager.remove(gui.top)
  end
  gui.lchooser_active = false;
end

function on_tools_button_clicked()
  if gui.lchooser_active == false then
    guimanager.add(gui.top)
    gui.lchooser_active = true
  end
end



function main_menu()
  lchooser = {}
  lchooser.timer = Timer()
  small_font = TTFont('content/fonts/FreeMono.ttf', 12)
  small_font:setColor(255, 255, 255)

  sx = 330
  sy = 400
  level_list = create_level_listmodel(mapfile)

  level_drop = DropDown(level_list)
  level_drop:setPosition(5, 5)
  level_drop:setSize(sx - 140,16)
  dark_color = Color(10, 20, 20)
  medium_color = Color(60, 70, 60)
  level_drop:setBackgroundColor(dark_color)
  level_drop:setBaseColor(medium_color)
  level_drop:setFont(small_font)

  inner = create_window("Choose a custom Map",sx,sy, small_font)
  inner:setOpaque(false)

  inner:add(level_drop)

  button = Button('Load Map')
  button:setPosition(sx - 125, 5)
  button:setBaseColor(medium_color)
  button:setEventId('on_loadmap')
  button:setSize(100,16)
  button:setFont(small_font)
  inner:add(button)

  button2 = Button('Stress test')
  button2:setPosition(sx - 125, 37)
  button2:setBaseColor(medium_color)
  button2:setEventId('on_stresstest')
  button2:setSize(100,16)
  button2:setFont(small_font)
  inner:add(button2)

  cb = CheckBox()
  cb:setPosition(5, 45 + 20)
  cb:setFont(small_font)
  cb:setCaption("Show Hex Grid")
  inner:add(cb)

  cb2 = CheckBox()
  cb2:setPosition(5, 45 + 20 + 20)
  cb2:setFont(small_font)
  cb2:setCaption("Show Tile Grid")
  inner:add(cb2)

  label = TextBox("")
  label:setPosition(5,sy - 200)
  label:setSize(sx-10,200)

  white = Color(255,255,255,255)
  --label:setBaseColor()
  label:setBackgroundColor(dark_color)
  label:setForegroundColor(white)
  label:setFont(small_font)
  inner:add(label)

  close_button = Button('close chooser')
  close_button:setBaseColor(medium_color)
  close_button:setPosition(sx - 125, 70)
  close_button:setSize(100,16)
  close_button:setFont(small_font)
  close_button:setEventId('close_level_chooser')
  inner:add(close_button)

end


function on_engine_start()
  dofile('content/scripts/bindings.lua')
  main_menu()  
end
