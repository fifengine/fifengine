require("content/scripts/class")

collectgarbage("stop")

Dialog = class(function(self,optargs)
	self.caption = optargs.caption or "Dialog"
	self.font    = optargs.font or TTFont("content/fonts/falloutd.ttf",12)
	self.size    = optargs.size 
	self.color   = optargs.color or Color(0,0,255)
	self.apply_style = true
	self.button_color   = optargs.button_color or Color(0,0,128)
	self.caption_height = optargs.caption_height or 20

	self.border_size = optargs.border_size or 10
	self.button_height = 20
	self.button_width  = 80
	self.evals = {}
	self.buttons = {}

	self.main_container = Container()

	self.window = Window(self.main_container, self.caption )
	self.window:setFont( self.font )
	self.window:setBaseColor(self.color)
	self.window:setTitleBarHeight(self.caption_height)


	self.container = Container()
	self.container:setPosition(self.border_size,self.border_size )
	self.container:setBaseColor(self.color)

	self.button_container = Container()
	self.button_container:setBaseColor(self.color)

	self.main_container:setPosition( 0, 0)--self.caption_height )
	self.main_container:setBaseColor(self.color)
	self.main_container:add( self.container )
	self.main_container:add( self.button_container )

	self:resize( self.size )
end)

function Dialog:addButton(name, callback)
	local button = Button(name)
	local eventid ="dialog_on_" .. name .. tostring(math.random()):sub(3,20)
	button:setFont(self.font)
	button:setSize(self.button_width,self.button_height)
	button:setBaseColor(self.button_color)
	button:setEventId(eventid)

	local nbuttons = table.getn(self.buttons)
	button:setPosition((self.button_width+self.border_size)*nbuttons, 0)
	self.button_container:add( button )
	table.insert(self.buttons,{button = button, name = name, callback = callback, eventid=eventid})

	self:resize(self.size)
	return button
end

function Dialog:resize(size)

	if size[1] < 2*self.button_width + 3*self.border_size then
		size[1] = 2*self.button_width + 3*self.border_size
	end

	self.win_size = { 
		size[1] + self.border_size*2,
		size[2] + self.border_size*3 + self.button_height+self.caption_height
	}
	self.size = size

	local nbuttons = table.getn(self.buttons)
	local buttons_w = (self.border_size + self.button_width)*nbuttons - self.border_size
	self.button_container:setPosition(
		(self.win_size[1]-buttons_w)/2,
		self.win_size[2] - self.button_height - self.border_size - self.caption_height)
	self.button_container:setSize(self.size[1],self.button_height)

	self.window:setSize(unpack(self.win_size))
	self.main_container:setSize(unpack(self.win_size))
	self.container:setSize(unpack(self.size))
end

function Dialog:add(name,widget)
	if widget == nil then
		widget = name
		name = nil
	end

	if self.apply_style then
		if widget.setFont then	widget:setFont( self.font ) end
		widget:setBaseColor( self.color )
	end

	self.container:add( widget )
	if name == nil then return end
	self:addEval(name,widget)
end

function Dialog:addEval(name,widget)
	local function get_getter(t)
		if t.isMarked ~= nil    then return t.isMarked   end
		if t.getText ~= nil     then return t.getText    end
		if t.getSelected ~= nil then return t.getSelected end
		console.print("don't know how to 'eval' " .. tostring(t))
		return nil
	end
	local function get_setter(t)
		if t.setMarked ~= nil    then return t.setMarked   end
		if t.setText ~= nil     then return t.setText    end
		if t.setSelected ~= nil then return t.setSelected end
		console.print("don't know how to 'eval' " .. tostring(t))
		return nil
	end
	self.evals[ name ] =  { widget,get_getter(widget),get_setter(widget) }
end

function Dialog:evaluate()
	self.values = {}
	for name,t in pairs(self.evals) do
		if t[2] ~= nil then
			self.values[ name ] = t[2](t[1])
		end
	end
	return self.values
end

function Dialog:genericSet(name,...)
	self.evals[ name ][3](self.evals[ name ][1],unpack(arg))
end

function Dialog:show()
	for _,button in ipairs(self.buttons) do
		gui.actions[button.eventid] = function()
-- 			self:hide()
			self:evaluate()
			if button.callback then
				button.callback{values = self.values, dialog = self, name = button.name }
			end
		end
	end
	guimanager.add(self.window)
end

function Dialog:hide()
	guimanager.remove(self.window)
	for _,button in ipairs(self.buttons) do
		gui.actions[button.eventid] = nil
	end
end

function Dialog:exec()
	self:show()
-- 	self.window:requestModalFocus()
end

on_engine_start = function()
	d = Dialog{ size = {200,200} }
	d:addButton("OK")
	d:exec()
end