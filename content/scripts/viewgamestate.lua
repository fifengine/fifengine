require 'content/scripts/statemanager'
require 'content/scripts/class'
require 'content/scripts/widgetlistener'

do

local function _get_loaded_map_control()
	local map_control = MapControl()
	for _,map in pairs{"content/maps/official_map.xml","maps/newr1.map","maps/hallded.map"} do
		if pcall(function() map_control:load( map ) end) then break end
	end
	if not map_control:getMap() then error("couldn't load *any* map") end
	return map_control
end

local LevelChooser = class(function(self)
	self.widgetListener = WidgetListener()
	
	local container = Container()
	local white = Color(255,255,255)
	local black = Color(0,64,0)
	
	self.font = TTFont('content/fonts/FreeMono.ttf', 12)
	self.font:setColor(255,255,255)
	self.bfont = TTFont('content/fonts/FreeMono.ttf', 12)
	self.bfont:setColor(0,0,0)

	self.window = Window(container,"")
	self.window:setFont(self.font)
	self.window:setTitleBarHeight(20)
	self.window:setBackgroundColor(black)
	self.window:setCaption("Level Chooser")
	
	container:setSize(260,200)
	container:setOpaque(true)
	
	container:setBaseColor(black)
	self.window:setBaseColor(black)
	
	self.window:setSize(270,240)
	self.isshown = false
	self.container = container
	
	local label = Label("Maps")
	label:setFont(self.font)
	label:setPosition(5,5)
	self.container:add( label )
	
	self.maps = ListModel()
	self.mapchooser = DropDown(self.maps)
	self.mapchooser:setFont(self.bfont)
	self.mapchooser:setSize(180,20)
	self.mapchooser:setPosition(5,20)
	
	local button = Button("Load")
	button:setFont(self.font)
	button:setPosition(200,20)
	button:setSize(50,20)
	self.widgetListener:setHandler( button, bind(self.loadMap,self) )
	container:add(button)
	
	local label = Label("Elevations")
	label:setFont(self.font)
	label:setPosition(5,45)
	self.container:add( label )
	
	self.elevations = ListModel()
	self.elevationchooser = DropDown(self.elevations)
	self.elevationchooser:setFont(self.bfont)
	self.elevationchooser:setSize(180,20)
	self.elevationchooser:setPosition(5,60)
	
	local label = Label("Layers")
	label:setFont(self.font)
	label:setPosition(5,85)
	self.container:add( label )
	
	self.layers = ListModel()
	self.layerchooser = DropDown(self.layers)
	self.layerchooser:setFont(self.bfont)
	self.layerchooser:setSize(180,20)
	self.layerchooser:setPosition(5,100)
	
	self.container:add(self.mapchooser)
	self.container:add(self.elevationchooser)
	self.container:add(self.layerchooser)
	
	self:loadMapSet{"content/etc/maps_custom_developers.txt"}
	self.widgetListener:receiveEvents()
	
	self.onLoadMap = function(filename) end
end)

function LevelChooser:loadMapSet(filelist)
	self.maps:clear()
	for _,filename in ipairs(filelist) do
		local file = io.open(filename)
		if file then
			for level in file:lines() do
				self.maps:addElement(level)
			end
		end
	end
end

function LevelChooser:show()
	if not self.isshown then
		console.print("show level chooser")
		guimanager.add( self.window )
		self.isshown = true
	end
end

function LevelChooser:hide()
	if self.isshown then
		guimanager.remove( self.window )
		self.isshown = false
	end
end

function LevelChooser:update(mapcontrol)
	local map = mapcontrol:getMap()
	local current_elevation = mapcontrol:getElevation()
	
	self.elevations:clear()
	for i = 1,map:getNumElevations() do
		local elevation = map:getElevation(i)
		local elevationName = "Elevation #" .. i
		self.elevations:addElement(elevationName)
	end
	self.elevationchooser:setSelected( current_elevation - 1 )

	self.layers:clear()
	local elevation = map:getElevation( current_elevation )
	for i = 1,elevation:getNumLayers() do
		local layer = elevation:getLayer(i)
		local layerName = "Layer #" .. i
		self.layers:addElement(layerName)
	end
	self.layerchooser:setSelected(0)
end

function LevelChooser:loadMap(event)
	map = tostring(self.maps:getElementAt(self.mapchooser:getSelected()))
	--HA?
	map = map:sub(0,#map - 1)
	console.print("load map '" .. map .. "'")
	self.onLoadMap(map)
end

local ViewGameState = class(function(self)
	self.mapcontrol = _get_loaded_map_control()
	self.map = self.mapcontrol:getMap()
	self.camera = nil
	self.timer = Timer(0, bind(self.turn,self))
	self.mouseListener = MouseListener()
	self.mouseListener:setHandler("mouseMoved",bind(self.mouseMoved,self))
	self.mouseListener:setHandler("mouseExited",bind(self.mouseExited,self))
	self.keyListener = KeyListener()
	self.keyListener:setHandler("keyPressed",bind(self.keyPressed,self))
	self.keyListener:setHandler("keyReleased",bind(self.keyReleased,self))
	
	self.levelchooser = LevelChooser()
	self.levelchooser.onLoadMap = bind(self.loadMap,self)
	self.move = { dx = 0, dy = 0 }
end)

function ViewGameState:activate()
	print("mapview activated")
	self.move = { dx = 0, dy = 0 }
	self.camera = MapCamera(self.mapcontrol)
	self.camera:setViewport(0,0,screen_width,screen_height)
	self.timer:start()
	self.mouseListener:receiveEvents()
	self.keyListener:receiveEvents()
	self.levelchooser:update(self.mapcontrol)
end

function ViewGameState:deactivate()
	print("mapview deactivated")
	self.timer:stop()
	self.mouseListener:ignoreEvents()
	self.keyListener:ignoreEvents()
	self.camera = nil
end

function ViewGameState:loadMap(filename)
	self.mapcontrol:load( filename )
	if self.mapcontrol:getMap() then
		self.map = self.mapcontrol:getMap()
	end
	self.mapcontrol:setElevation(1)
	self.levelchooser:update(self.mapcontrol)
end

function ViewGameState:turn()
	self.camera:moveBy(self.move.dx,self.move.dy)
	self.mapcontrol:turn()
	self.camera:render()
end

function ViewGameState:mouseExited(event)
	self.move = { dx = 0, dy = 0 }
	console.print("mouse exited.")
end

function ViewGameState:mouseMoved(event)
	self.move = { dx = 0, dy = 0 }
	if event.x < 20 and event.x > 0 then
		self.move.dx = - 20
	end
	if event.y < 20 and event.y > 0 then
		self.move.dy = - 20
	end
	if event.x > screen_width - 20 and event.x < screen_width then
		self.move.dx = 20
	end
	if event.y > screen_height - 20 and event.y < screen_height then
		self.move.dy = 20
	end
end

function ViewGameState:keyPressed(event)
	console.print("keypressed:" .. tostring(event.key.value) .. "/" .. tostring(event.key.string))
	if event.key.string == "IKey::RIGHT" then
		self.move.dx = 20
	end
	if event.key.string == "IKey::LEFT" then
		self.move.dx = -20
	end
	if event.key.string == "IKey::UP" then
		self.move.dy = -20
	end
	if event.key.string == "IKey::DOWN" then
		self.move.dy = 20
	end
	if event.key.string == "l" then
		if not self.levelchooser.isshown then
			self.levelchooser:show()
		else 
			self.levelchooser:hide()
		end
	end
end

function ViewGameState:keyReleased(event)
	console.print("keyreleased:" .. tostring(event.key.value) .. "/" .. tostring(event.key.string))
	if event.key.string == "IKey::RIGHT" then
		self.move.dx = 0
	end
	if event.key.string == "IKey::LEFT" then
		self.move.dx = 0
	end
	if event.key.string == "IKey::UP" then
		self.move.dy = 0
	end
	if event.key.string == "IKey::DOWN" then
		self.move.dy = 0
	end
end

statemanager:add( "mapview", ViewGameState() )
statemanager.initial_state = "mapview"

end
