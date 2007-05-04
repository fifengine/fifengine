require("content/scripts/class")
require("content/scripts/functional")

--
--	This is a basic example of how to use the 'ruleset' api.
--

GObject.exec = function(object,...)
	pcall(FIFE_ExecCommand,object,unpack(arg))
end

-- Basic Game Object Wrapper Class
Object = class(function (self,object)
	self.object = object or GObject()
	self.actions = {
		idle = 0,
		walk = 1,
		run = 2,
		climb = 3,
		pickup = 4,
		use = 5,
		dodge = 6,
		hitfront = 7,
		hitback = 8,
		kick = 9,
		throw = 11,
		punch = 12
	}
	self.object._ref = FIFE_RegisterObject(self)
	self.visual_path = self.object._visual_path
end)

function Object:sleep(ticks)
	FIFE_Sleep(self.object._ref,ticks)
end

function Object:setVisual(visual)
	self.object:exec(1,0,0,0,0,visual)
end

function Object:enqueue(action,direction)
	self.object:exec(3,action,direction,0,1,"")
end

-- Demo Critter Implementation
-- Moves around randomly on the map.
Critter = class(Object,function(self,object)
	Object.init(self,object)

	-- In ten milliseconds call onWake on this object.
	self.active = false
end)

function Critter:onWake()
	-- print("Visual:",self.object._visual_id,self.object._visual_path)

	-- Put fallouts "walk" action on the queue, with a random direction
	self:enqueue(self.actions.walk,math.random(0,5))

	-- Call onWake again in 100 milliseconds,
	-- if we are not set to stop that crap.
	if self.active then self:sleep(300)	end
end

function Critter:deactivate()
	self.active = false
end

function Critter:activate()
	if not self.active then
		self.active = true
		print("Hello, " .. self.object.name)

		-- Until visual location is exported to lua,
		-- we use a simple attribute to set the visual.
		if self.visual_path then self:setVisual(self.visual_path) end
		self:sleep(10)
	end
end

-- Fallout Elevation Class
FElevation = class(function(self)
	self.critters = {}
	self.objects  = {}
	self.active   = false
end)

-- Create the Critter objects from the GObjects,
-- if not yet done.
function FElevation:createCritters()
	if #self.critters == 0 then
		for _,o in ipairs(self.objects) do
			table.insert(self.critters,Critter(o))
		end
	end
end

function FElevation:activate()
	self:createCritters()
	if not self.active then
		for _,critter in pairs(self.critters) do
			critter:activate()
		end
		self.active = true
	end
end

function FElevation:deactivate()
	if self.active then
		for _,critter in pairs(self.critters) do
			critter:deactivate()
		end
		self.active = false
	end
end

function FElevation:addObject(object)
	if self.active then
		local critter = Critter(object)
		table.insert(self.critters,critter)
		critter:activate()
	else
		table.insert(self.objects,object)
	end
end

-- Fallout Map Class
FMap = class(function(self)
	self.elevations = {}
	self.active_elevation = nil
	for i=0,10 do
		self.elevations[i] = FElevation()
	end
end)


function FMap:addObject(object)
	self.elevations[object.elevation]:addObject(object)
end

function FMap:activate( elevation_id )
	elevation = self.elevations[ elevation_id ]

	-- Already active?
	if self.active_elevation == elevation then
		return
	end

	-- Deactivate previous elevetion
	if self.active_elevation ~= nil then
		self.active_elevation:deactivate()
	end

	self.active_elevation = elevation
	self.active_elevation:activate()
end


-- Init Ruleset.
current_map = FMap()

-- Bind callbacks.
AddObject         = bind(FMap.addObject, current_map )
ActivateElevation = bind(FMap.activate, current_map )

print("RULESET STARTED")
