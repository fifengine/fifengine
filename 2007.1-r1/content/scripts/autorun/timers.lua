--[[
        Timer wrapper
--]]
require 'content/scripts/class'

do

local _Timer = Timer

Timer = class(function (self,interval, cb)
	self.timer = _Timer()
	if interval ~= nil then
		self.timer:setInterval( interval )
	end
	if cb ~= nil then
		self.timer:setCallback( cb )
	end
end)

function Timer:setCallback(cb)
	self.timer:setCallback(cb)
end

function Timer:setInterval( interval )
	self.timer:setInterval( interval )
end

function Timer:start()
	self.timer:start()
end

function Timer:stop()
	self.timer:stop()
end

function Timer.singleshot(timeout, cb)
   local t = _Timer()
   t:setInterval( timeout )
   t:setCallback( function () t:stop() cb()  t:setCallback(nil) end )
   t:start()
end

end

