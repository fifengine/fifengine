require 'content/scripts/statemanager'
require 'content/scripts/class'

do

local function _get_loaded_map_control()
	local map_control = MapControl()
	for _,map in pairs{"content/maps/official_map.xml","maps/newr1.map","maps/hallded.map"} do
		if pcall(function() map_control:load( map ) end) then break end
	end
	if not map_control:getMap() then error("couldn't load *any* map") end
	return map_control
end

local ViewGameState = class(function(self)
	self.mapcontrol = _get_loaded_map_control()
	self.map = self.mapcontrol:getMap()
	self.camera = nil
	self.timer = Timer(0, bind(self.turn,self))
	self.mouseListener = MouseListener()
	self.mouseListener:setHandler("mouseMoved",bind(self.mouseMoved,self))
	self.mouseListener:setHandler("mouseExited",bind(self.mouseExited,self))
	self.move = { dx = 0, dy = 0 }
end)

function ViewGameState:activate()
	print("mapview activated")
	self.move = { dx = 0, dy = 0 }
	self.camera = MapCamera(self.mapcontrol)
	self.camera:setViewport(0,0,screen_width,screen_height)
	self.timer:start()
	self.mouseListener:receiveEvents()
end

function ViewGameState:deactivate()
	print("mapview deactivated")
	self.timer:stop()
	self.mouseListener:ignoreEvents()
	self.camera = nil
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

statemanager:add( "mapview", ViewGameState() )
statemanager.initial_state = "mapview"

end
