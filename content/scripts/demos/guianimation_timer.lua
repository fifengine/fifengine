-- Just a little proove-of-concept
-- The question was "How to show animations LUAgui?"

-- important:
-- if you use images, you need Container:remove(image) - otherwise the old image will be displayed, too.

function script_gui()
	local size_x = 300
	local size_y = screen_height

	gui.right = Container()
	gui.right:setSize(size_x, size_y)
	gui.right:setPosition(screen_width - size_x, 5)
	gui.right:setOpaque(false)
	guimanager.add(gui.right)

	local pos_x = 20
	local pos_y = 30
	local mod_y	= 20
	
	size_y	= 20
	
	gui.right_button1 = Button('Image animation')
	gui.right_button1:setSize(size_x, size_y)
	gui.right_button1:setPosition(pos_x, pos_y)
	gui.right_button1:setEventId('show_img_anim')
	gui.right:add(gui.right_button1)
	gui.actions.show_img_anim = function () cleanup() animtest() end

	pos_y = pos_y + mod_y

	gui.right_button2 = Button('Image animation (drag)')
	gui.right_button2:setSize(size_x, size_y)
	gui.right_button2:setPosition(pos_x, pos_y)
	gui.right_button2:setEventId('show_img_anim_drag')
	gui.right:add(gui.right_button2)
	gui.actions.show_img_anim_drag = function () cleanup() animtest_drag() end

	pos_y = pos_y + mod_y
	
	gui.right_button3 = Button('Pong!')
	gui.right_button3:setSize(size_x, size_y)
	gui.right_button3:setPosition(pos_x, pos_y)
	gui.right_button3:setEventId('show_pong')
	gui.right:add(gui.right_button3)
	gui.actions.show_pong = function () cleanup() pong() end

	pos_y = pos_y + mod_y
	
	gui.right_button4 = Button('Widget moving')
	gui.right_button4:setSize(size_x, size_y)
	gui.right_button4:setPosition(pos_x, pos_y)
	gui.right_button4:setEventId('show_widget_movetest')
	gui.right:add(gui.right_button4)
	gui.actions.show_widget_movetest = function () cleanup() widget_movementtest() end

	pos_y = pos_y + mod_y
	
	gui.right_button5 = Button('A hitpoint bar')
	gui.right_button5:setSize(size_x, size_y)
	gui.right_button5:setPosition(pos_x, pos_y)
	gui.right_button5:setEventId('show_hitpointbar')
	gui.right:add(gui.right_button5)
	gui.actions.show_hitpointbar = function () cleanup() hitpoints() end
end

function flip_images(a,b)
  return b, a
end
	
function animtest()
	local red 	= Icon(Image('content/gfx/objects/red.png')) 
	local white = Icon(Image('content/gfx/objects/white.png'))

  	gui.anim = Container()
  	gui.anim:setPosition(10,10)
  	gui.anim:setSize(110,110)
  	gui.anim:setOpaque(false)
  	guimanager.add(gui.anim)

	anim_timer = Timer()
  	anim_timer:setInterval(1000)
  	anim_timer:setCallback( function ()
	    red, white = flip_images(red, white)
		gui.anim:add(red)
  	end )
  	anim_timer:start()
end

function animtest_drag()
	red 	= Icon(Image('content/gfx/objects/red.png')) 
	white = Icon(Image('content/gfx/objects/white.png'))

  	gui.anim = Container()
  	gui.anim:setPosition(10,10)
  	gui.anim:setSize(110,110)
  	gui.anim:setOpaque(false)

	gui.anim_drag = Window(gui.anim, 'Move me')
  	guimanager.add(gui.anim_drag)

	anim_timer = Timer()
  	anim_timer:setInterval(1000)
  	anim_timer:setCallback( function ()
	    red, white = flip_images(red, white)
		gui.anim:add(red)
  	end )
  	anim_timer:start()
end

function widget_movementtest()
	local x = 0
	local y = 50
	local size_x = 110
	local size_y = 110
	local dir = 1
	local i = 0

  	gui.anim = Container()
  	gui.anim:setPosition(10,10)
  	gui.anim:setSize(110,110)
  	gui.anim:setOpaque(false)

	gui.anim_drag = Window(gui.anim, 'moving out')
	gui.anim_drag:setPosition(x,y)	
	gui.anim_drag:setSize(size_x,size_y)
  	guimanager.add(gui.anim_drag)
  	
  	y = y + 1
	x = x + 1

	anim_timer = Timer()
  	anim_timer:setInterval(50)
  	anim_timer:setCallback( function ()
		if dir == 1 then
			gui.anim_drag:setCaption('*right*')
			gui.anim_drag:setPosition(x,y)
			x = x + 5
			if x > 200 then
				dir = 0
			end
		elseif dir == 0 then
			gui.anim_drag:setCaption('*left*')
			gui.anim_drag:setPosition(x,y)
			x = x - 5
			if x <= 1 then
				dir = 1
			end
		elseif dir == 2 then
			gui.anim_drag:setCaption('*resizing*')
			gui.anim_drag:setSize(size_x,size_y)
			size_y = size_y + 1
			if size_y >= 110 then
				dir = 1
				anim_timer:setInterval(50)
				anim_timer:start()
				if i == 90 then
					i = 0
				end
			end		
		end
		if i == 50 and dir ~= 2 then
			dir = 2
			size_y = 20
			gui.anim_drag:setCaption('*resizing*')
			i = 0
			anim_timer:setInterval(1)
			anim_timer:start()
		end
		i = i + 1
  	end )
  	anim_timer:start()
end

function pong()
	local x = 0
	local y = 20
	local dir = 1

  	gui.anim = Container()
  	gui.anim:setPosition(10,10)
  	gui.anim:setSize(110,110)
  	gui.anim:setOpaque(false)

	gui.anim_drag = Window(gui.anim, 'Move me')
	gui.anim_drag:setPosition(x,y)	
  	guimanager.add(gui.anim_drag)
  	
  	y = y + 1
	x = x + 1

	anim_timer = Timer()
  	anim_timer:setInterval(50)
  	anim_timer:setCallback( function ()
		if dir == 1 then
			gui.anim_drag:setPosition(x,y)
			y = y + 3
			x = x + 3
			if x > 200 then
				dir = 0
				gui.anim_drag:setCaption('*pong*')
			end
		elseif dir == 0 then
			gui.anim_drag:setPosition(x,y)
			y = y - 3
			x = x - 3
			if x <= 1 then
				dir = 1
				gui.anim_drag:setCaption('*ping*')
			end		
		end
		
  	end )
  	anim_timer:start()
end

function hitpoints()
	local base_HP		= 305
	local current_HP	= 305
	local i				= 0
	local size_y		= 100
	local size_x		= 40
	local pos_y			= 70
	local newsize_y		= 100
	local value			= NIL
	local div			= NIL
	local color_red	= Color(202, 29, 16)

  	gui.anim = Container()
  	gui.anim:setPosition(10,pos_y)
  	gui.anim:setSize(size_x,size_y)
  	gui.anim:setBackgroundColor(color_red)
  	gui.anim:setOpaque(true)
  	
  	guimanager.add(gui.anim)

	function decrease_hp (hp)
		value = math.ceil(hp * 100 / base_HP)
		div = size_y - value
		return value
	end

	function increase_hp (hp)
		value = math.ceil(hp * 100 / base_HP)
		div = -1
		return value
	end

	anim_timer = Timer()
	anim_timer:setInterval(10)
	anim_timer:setCallback(
			function ()
				if div > 0 then
					if value ~= 0 then
						gui.anim:setSize(size_x,newsize_y) 
						gui.anim:setPosition(10,pos_y)
						newsize_y = newsize_y - 1
						value = value - 1
						pos_y = pos_y + 1 
					else
						anim_timer:stop()
					end
				else
					if value ~= div then
						gui.anim:setSize(size_x,newsize_y) 
						gui.anim:setPosition(10,pos_y)
						newsize_y = newsize_y + 1
						value = value - 1
						pos_y = pos_y - 1 
						i = i + 1
					else
						anim_timer:stop()
					end				
				end
			end
	)
	
	gui.button1 = Button('-30 HP')
	gui.button1:setPosition(70,10)
	gui.button1:setEventId('sub_30')
	
	gui.button2 = Button('-50 HP')
	gui.button2:setPosition(70,50)
	gui.button2:setEventId('sub_50')
	
	gui.button3 = Button('+80 HP')
	gui.button3:setPosition(70,90)
	gui.button3:setEventId('add_80')
	
	guimanager.add(gui.button1)
	guimanager.add(gui.button2)
	guimanager.add(gui.button3)


	gui.actions.sub_30 = function ()
		value = decrease_hp (30)
		anim_timer:start()
	end
	gui.actions.sub_50 = function ()
		value = decrease_hp (50)
		anim_timer:start()
	end	
	gui.actions.add_80 = function ()
		value = increase_hp (80)
		anim_timer:start()
	end
	
end

function cleanup()
	-- clean up or receive chaos :-p
	
	if anim_timer then
		anim_timer:stop()
		--anim_timer:setCallback()
		anim_timer = NIL
		print('\t LUA: deleted timer "anim_timer"')
	end

	if gui.anim_drag then
		gui.anim_drag:clear()
		guimanager.remove(gui.anim_drag)
		gui.anim_drag = NIL
		print('\t LUA: removed gui.anim_drag')
	end

	if gui.anim then
		guimanager.remove(gui.anim)
		gui.anim = NIL
		print('\t LUA: removed gui.anim')
	end
	
	if gui.button1 then
		guimanager.remove(gui.button1)
		guimanager.remove(gui.button2)
		guimanager.remove(gui.button3)
		gui.button1 = NIL
		gui.button2 = NIL
		gui.button3 = NIL
		print('\t LUA: removed button1 - button3')
	end
end

on_engine_start = script_gui
