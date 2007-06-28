-- Just a little proove-of-concept
-- The question was "How to make an inventory with LUAgui?"

function on_engine_start()
	-- 'p' for screenshots
	dofile('content/scripts/bindings.lua')	

	-- How many images?
	local imglist_count = 40
	local imglist_height = 100
	local imglist_containerheigth = imglist_height * imglist_count + 100

	-- colors
	gui.dark = Color(30, 80, 40)
	gui.invisible = Color(20, 70, 20, 30)

	-- major widget
	gui.top = Container()
	guimanager.add(gui.top)
	gui.top:setSize(640, 480)
	gui.top:setBackgroundColor(gui.invisible)
	gui.top:setPosition((screen_width - 640 ) / 2, (screen_height - 480) / 2)
	gui.top:setOpaque(false)

	-- inner widget
	gui.inner = Container()
	gui.inner:setOpaque(false)
	gui.inner:setSize(510,imglist_containerheigth)

	-- scroll area	
	gui.scroll = ScrollArea()
	gui.scroll:setSize(520, 400)
	gui.scroll:setPosition(5, 5)
	gui.scroll:setBackgroundColor(gui.invisible)
	gui.scroll:setContent(gui.inner)
	gui.scroll:setScrollPolicy(1,0)

	local y = 5
	local gfx = {}

	--[[	
	for i = 1,imglist_count do
		gfx[i] = Image('content/gfx/tiles/chess/plain_white.png')
		gfx[i] = TwoButton(gfx[i],gfx[i])
		gfx[i]:setEventId('gfx_' .. i)
		gfx[i]:setPosition(10,y)
		y = y + 5 + imglist_height
		gui.inner:add(gfx[i])
	end
	]]

	local a = imglist_count + 1	
	local b = a + 200

	-- create many entries	
	for i = 1,imglist_count do
		gfx[i] = Image('content/gfx/tiles/chess/plain_white.png')
		gfx[i] = TwoButton(gfx[i],Image('content/gfx/tiles/chess/plain_brown.png'))
		gfx[i]:setEventId('gfx_' .. i)
		gfx[i]:setPosition(10,y)
		gui.inner:add(gfx[i])

		gfx[a] = Image('content/gfx/tiles/chess/plain_white.png')
		gfx[a] = TwoButton(gfx[a],Image('content/gfx/tiles/chess/plain_brown.png'))
		gfx[a]:setEventId('gfx_' .. a)
		gfx[a]:setPosition(170,y)
		gui.inner:add(gfx[a])

		gfx[b] = Image('content/gfx/tiles/chess/plain_white.png')
		gfx[b] = TwoButton(gfx[b],Image('content/gfx/tiles/chess/plain_brown.png'))
		gfx[b]:setEventId('gfx_' .. b)
		gfx[b]:setPosition(330,y)
		gui.inner:add(gfx[b])
		y = y + 5 + imglist_height
		
		a = a + 1
	end
	
	gui.top:add(gui.scroll)
	
	gui.actions.gfx_1 = function()
		-- print('')
	end	  

end
