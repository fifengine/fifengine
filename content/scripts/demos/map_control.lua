--[[
	Test script to see if the exported
	map functions/objects work as expected
--]]

local function _get_loaded_map_control()
	map_control = MapControl()
	for _,map in pairs{"maps/newr1.map","maps/hallded.map","content/maps/scratch.xml"} do
		if pcall(function() map_control:load( map ) end) then break end
	end
	if not map_control:getMap() then error("couldn't load *any* map") end
	return map_control
end

local function _init_label()
	local font = TTFont('content/fonts/FreeMono.ttf', 25)
	font:setColor(250, 200, 50)

	local label = Label("Hello")
	label:setFont( font )
	label:setPosition(0,0)

	guimanager.add( label )
	return label
end

function demo_mapcontrol ()

	map_control = _get_loaded_map_control()
	map_control:start()

	map_camera1 = MapCamera( map_control )
	map_camera2 = MapCamera( map_control )
	map_camera3 = MapCamera( map_control )
	map_camera4 = MapCamera( map_control )

	map_camera1:setViewport(0,0,screen_width/2,screen_height/2)
	map_camera2:setViewport(screen_width/2,0,screen_width/2,screen_height/2)
	map_camera3:setViewport(0,screen_height/2,screen_width/2,screen_height/2)
	map_camera4:setViewport(screen_width/2,screen_height/2,screen_width/2,screen_height/2)

	id = 0
	function track()
		map_camera1:track(id)
		map_camera2:track(id+1)
		map_camera3:track(id+2)
		map_camera4:track(id+3)
		id = id + 4
	end

	map_timer = Timer()
	map_timer:setInterval(0)
	map_timer:setCallback( function () 
		map_control:turn() 
		map_camera1:render() map_camera2:render() 
		map_camera3:render() map_camera4:render() 
	end )
	map_timer:start()

	track_timer = Timer()
	track_timer:setInterval(2000)
	track_timer:setCallback( track )
	track_timer:start()
end

function test_mapcontrol_states()

	map_control = _get_loaded_map_control()
	map_camera = MapCamera( map_control )
	map_camera:setViewport(0,0,screen_width,screen_height - 30)
	
	render_timer = Timer( 0, function()
		map_camera:render()
		map_control:turn()
	end)

	current_map = map_control:getMap()

	-- Notice how ugly the c++ data is used here :-(
	START_ELEVATION = (current_map._START_ELEVATION or 0) + 1
	START_POSITION = current_map:getElevation(START_ELEVATION)._START_POSITION
	print("START_ELEVATION",START_ELEVATION)
	print("START_POSITION",unpack(START_POSITION))

	-- Info label
	label = _init_label()
	

	state_change = {
		function() map_control:setMap(current_map) end,
		function()
			map_control:start()
			map_camera:moveTo( unpack(START_POSITION) )
		end,
		function() map_control:stop() end,
		function() map_control:clearMap() end,
		names = { "setMap","start","stop","clearMap" }
	}

	local new_state = 0
	local state_changes = 1
	state_timer = Timer( 3000, function() 
		new_state = new_state % 4 + 1
		state_changes = state_changes + 1
		if state_changes > 30 then new_state = math.random(1,4) end
		state_change[ new_state ]()
		label:setCaption( "new state:" .. state_change.names[new_state] .. "#" .. state_changes)
	end )
	
	success_function = function()
		console.print("MapControl State Test Successfull")
		render_timer:stop()
		state_timer:stop()
	end

	console.print("MapControl State Test Started - will run 3 minutes")
	render_timer:start()
	state_timer:start()
	Timer.singleshot(3*60*1000, success_function )
end

--on_engine_start = demo_mapcontrol
on_engine_start = test_mapcontrol_states
