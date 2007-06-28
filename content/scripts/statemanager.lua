-- FIFE State Manager

require 'content/scripts/class'
require 'content/scripts/functional'

do

local StateManager = class(function (self)
	self.states = {}
	self.current_state = nil
	self.current_statename = nil
	self.initial_state = nil
end)

function StateManager:activate(statename)
	if statename == nil then
		return
	end
	
	if self.states[statename] == nil then
		error("no state:" + statename)
	end
	
	self:deactivate(self.current_statename)
	self.current_state = self.states[statename]
	self.current_statename = statename
	self.current_state:activate()
end


function StateManager:deactivate(statename)
	if statename == nil then
		return
	end
	
	if self.states[statename] == nil then
		return
	end
	
	self.states[statename]:deactivate()
	if self.current_statename == statename then
		self.current_state = nil
		self.current_statename = nil
	end
end

function StateManager:add(statename,state)
	print("Registered state:",statename)
	self.states[statename] = state
end

function StateManager:onEngineStart()
	if self.initial_state == nil then
		error("no initial state set.")
	end
	self:activate(self.initial_state)
end

function StateManager:onEngineStop()
	if self.current_statename then
		self:deactivate(self.current_statename)
	end
end

statemanager = StateManager()
on_engine_start = bind(statemanager.onEngineStart,statemanager)
on_engine_stop  = bind(statemanager.onEngineStop,statemanager)

end
