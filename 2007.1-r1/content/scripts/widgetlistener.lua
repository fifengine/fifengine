require 'content/scripts/class'

do

WidgetCapture = class(function(self,widget,handler)
	self.eventId = "Capture#" .. tostring(math.random())
	self.widget = widget
	self.widget:setEventId(self.eventId)
	self.handler = handler
end)

WidgetListener = class(function(self)
	self.captures = {}
	self.widgets = {}
	self.widgetListener = _WidgetListener()
	self.widgetListener:setHandler("onWidgetAction",function(event)
		widget = self.widgets[event.action]
		if widget == nil then return end
		capture = self.captures[widget]
		if capture == nil then return end
		if capture.handler then
			event.widget = widget
			capture.handler( event )
		end
	end)
end)

function WidgetListener:setHandler(widget,handler)
	if self.captures[widget] == nil then
		self.captures[widget] = WidgetCapture(widget,handler)
		self.widgets[self.captures[widget].eventId] = widget
	else
		if handler == nil then
			self.widgets[self.captures[widget].eventId]=nil
			self.captures[widget] = nil
		else
			self.captures[widget].handler = handler
		end
	end
end

function WidgetListener:getHandler(widget)
	if self.captures[widget] == nil then
		return nil
	end
	return self.captures[widget].handler
end

function WidgetListener:receiveEvents()
	self.widgetListener:receiveEvents()
end

function WidgetListener:ignoreEvents()
	self.widgetListener:ignoreEvents()
end

end