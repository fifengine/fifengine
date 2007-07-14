dofile("content/scripts/class.lua")

--	Wrappers for:
--		FIFE_ExecCommand(obj,cmd,p0,p1,p2,p3,string)

FIFE = {
	Directions = { 
	-- UN-Even X
		{  {-1,-1} ,{-1,0}, {0,1}, {1,0}, {1,-1},{ 0,-1 } },
	-- Even X
		{  {-1,0} ,{-1,1}, {0,1}, {1,1}, {1,0},{ 0,-1 } }
	},
	AnimationSuffix = {
		idle = "aa",
		walk = "ab",
		run = "at",
		climb = "ae",
		pickup = "ak",
		use = "al",
		dodge = "an",
		hitfront = "ao",
		hitback = "ap",
		kick = "ar",
		run = "at",
		throw = "as",
		punch = "aq"
		
	--  ...etc
	},

	AnimationState = {
		Repeat = 0,
		AnimateToEnd = 1,
		AnimateFromEnd = 2,
		ShowFrame = 3
	}
}



function isEven(n)
	return (math.cos(n * math.pi) + 1 )/2
end

function FIFE_SetVisual(obj,visual)
	FIFE_ExecCommand(obj.id,1,0,0,0,0,visual)
end

function FIFE_SetAdvancedVisual(obj,visual,direction,animstate)
	FIFE_ExecCommand(obj.id,1,direction,animstate,0,0,visual)
end

function FIFE_TriggerVisual(obj,trigger)
	-- Trigger the current visual to do sth.
	FIFE_ExecCommand(obj.id,3,trigger,0,0,0,"")
end

function FIFE_StartMovement(obj,dir,visual,ticks)
	local xy, gridspecs
	gridspecs = isEven(obj.location.x) + 1
	xy = FIFE.Directions[gridspecs][dir+1]
-- 	print("StartMovement: " .. (dir+1) .. " -> " .. xy[1] .. "," .. xy[2])
	obj.location.x = obj.location.x + xy[1]
	obj.location.y = obj.location.y + xy[2]
	gridspecs = isEven(obj.location.x) + 1
	FIFE_ExecCommand(obj.id,1,dir,FIFE.AnimationState.AnimateToEnd,0,0,visual)
	FIFE_ExecCommand(obj.id,2,xy[1],xy[2],ticks,0,"")
end

function FIFE_GetParam(location,paramid)
	return FIFE_GetParamInternal(location.grid,location.x,location.y,paramid)
end

function FIFE_SetParam(location,paramid,value)
end

function FIFE_BlockMovement(location)
	FIFE_ChangeParam( location, 0, 1)
end

function FIFE_UnblockMovement(location)
	FIFE_ChangeParam( location, 0, -1)
end

function FIFE_BlockVision(location)
	FIFE_ChangeParam( location, 0, 1)
end

List = class({},function(self)
	self.n = 0
end)

function List:append(item)
	self.n = self.n + 1
	self[self.n] = item
end

function List:empty()
	return self.n == 0
end

function List:pop()
	if self.n == 0 then return nil end

	local retval
	retval = self[self.n]
	self.n = self.n - 1
	return retval
end

ActionQueue = class(List,function(self)
	List.init(self)
end)

function ActionQueue:addAction(action)
end


GObject = class({},function(self,params)
-- 	print("New GObject: " .. params.objtype )
	self.id = params.id
	self.location = params.location
end)

function GObject:onWake(time)
	print("I awoke!")
end

StaticObject = class(GObject,function(self,params)
	GObject.init(self,params)
	FIFE_SetVisual(self,params.visual)
end)

Critter = class(GObject,function(self,params)
	print("Creating Critter:",params.name," of type ",params.objtype,
		" with visual ",params.visual)

	GObject.init(self,params)
	self.orientation = params.orientation
	self.visual = params.visual
-- 	self.visual = "art/critters/hanpwrab.frm"
	
	self.ref = FIFE_RegisterObject(self)
	FIFE_SetAdvancedVisual(self,self.visual,self.orientation)
	FIFE_Sleep(self.ref, 20000 + math.random(1000) )
end)

function Critter:onWake(ticks)
	self.orientation = math.random(6) - 1
	FIFE_StartMovement(self,self.orientation,self.visual,500)
	FIFE_Sleep(self.ref, 500)-- + math.random(1000))
end

TestCritter = class(GObject,function(self,params)
	GObject.init(self,params)
	self.animations = {}
	self.animations.idle = "art/critters/nmwarraa.frm"
	self.animations.walk = "art/critters/nmwarrab.frm"
	self.animations.kick = "art/critters/nmwarrar.frm"

	self.orientation = params.orientation

	self.ref = FIFE_RegisterObject(self)
	self.visual = self.animations.idle
	self.stateFunction = self.beIdle

	FIFE_SetAdvancedVisual(self,self.visual,self.orientation,
		FIFE.AnimationState.Repeat)
	FIFE_Sleep(self.ref, 2000 + math.random(1000) )
end)
function TestCritter:beIdle(ticks)
	self.visual = self.animations.idle
	if math.random(5) == 1 then
		FIFE_SetAdvancedVisual(self,self.visual,self.orientation,
			FIFE.AnimationState.ShowFrame)
	else 
		FIFE_SetAdvancedVisual(self,self.visual,self.orientation,
			FIFE.AnimationState.Repeat)
	end
	if math.random(5) == 1 then 
		self.stateFunction = self.walkAround
	end
	if math.random(5) == 2 then 
		self.stateFunction = self.execKick
	end	
	return 1000
end

function TestCritter:walkAround(ticks)
	self.orientation = math.random(6)
	if self.orientation > 5 then self.orientation = 0 end
	self.visual = self.animations.walk
	FIFE_StartMovement(self,self.orientation,self.visual,300)

	self.stateFunction = self.beIdle
	return 500
end

function TestCritter:execKick(ticks)
	self.visual = self.animations.kick
	FIFE_SetAdvancedVisual(self,self.visual,self.orientation,FIFE.AnimationState.AnimateToEnd)

	self.stateFunction = self.beIdle
	return 1000
end



function TestCritter:onWake(ticks)
	FIFE_Sleep(self.ref, self:stateFunction(ticks))-- + math.random(1000))
end

Ruleset = class({},function(self)
-- 	self.id = FIFE_RegisterRuleset(self)
	print("Ruleset starting up!")
	self.ref = FIFE_RegisterObject(self)
	self.active_elevation = nil
	self.map_elevation = {}
	self.gobjectTypes = {}

	self.gobjectTypes["Critter"] = TestCritter
	self.gobjectTypes["Wall"] = StaticObject
	self.gobjectTypes["Misc"] = StaticObject
	self.gobjectTypes["Scenery"] = StaticObject
	self.gobjectTypes["Portal"] = StaticObject
	self.gobjectTypes["Elevator"] = StaticObject
	self.gobjectTypes["Ladder"] = StaticObject
	self.gobjectTypes["Container"] = StaticObject

	self.orientation = 5
-- 	self:onWake(0)
end)

function Ruleset:onWake(ticks)
	self.orientation = self.orientation 
	if self.orientation > 5 then self.orientation = 0 end
	print("Orientation: " .. (self.orientation +1))
	FIFE_Sleep(self.ref,20000)
end


function Ruleset:addElevation(elevation)
	print("Adding Elevation: " .. elevation)
	self.map_elevation[ elevation ] = { params = List(), gobjects = List() }
end

function Ruleset:addParamSet(params)
	self.map_elevation[ params.location.elevation ].params:append( params )
end
	

function Ruleset:createObject(params)
-- 	print("Creating Object:",params.name," of type ",params.objtype,
-- 		" with visual ",params.visual)
	if params.location.elevation == nil then return end
	if not self.map_elevation[ params.location.elevation ] then
		self:addElevation(params.location.elevation)
	end
	self:addParamSet( params )
end

function Ruleset:activateElevation(elevation)
	local params, gobject
	print("Activating Elevation: " .. elevation .. " Let the Random Walk begin!")

	if self.active_elevation ~= nil then
		if self.active_elevation == elevation then return end

		print("De-Activating Elevation " .. self.active_elevation)
		self.map_elevation[ self.active_elevation ].gobjects = List()
	end

	print("Initializing " .. self.map_elevation[elevation].params.n .. " Objects")

	for _,params in self.map_elevation[elevation].params do

		if self.gobjectTypes[params.objtype] == nil then
			print("Unknown Object Type: " .. params.objtype)
		else
			local gobject 
			gobject = self.gobjectTypes[params.objtype](params)
			self.map_elevation[elevation].gobjects:append(gobject)
		end
	end
	print("Initialization done.")
	self.active_elevation = elevation
end

ruleset = Ruleset()
