visible = false

function toggle_console ()
	if (visible) then
		guimanager.remove(gui.top)
		visible = false
	else
		guimanager.add(gui.top)
		visible = true
	end
end

function show_console ()
	if (visible == false) then
		guimanager.add(gui.top)
	end
end

function hide_console ()
	if (visible) then
		guimanager.remove(gui.top)
	end
end

function c_print (v)
	gui.box:addRow(v)
end

gui.actions.LineEntered = function()
	text = gui.input:getText()
	chunk = loadstring(text)
	gui.box:addRow(text)
	status, message = pcall(chunk)
	if (message ~= nil) then
		gui.box:addRow(message)
	end
	hs,vs = gui.scrollarea:getMaxScroll()
	gui.scrollarea:setScrollAmount(0,vs)
	gui.input:setText('')
end

gui.top = Container()
gui.top:setSize(480, 360)
gui.top:setPosition((screen_width - 480 ) / 2, (screen_height - 360) / 2)
gui.top:setOpaque(true)

baseColor = Color()
baseColor:setRed(0)
baseColor:setGreen(0)
baseColor:setBlue(0)
baseColor:setAlpha(127)
foregroundColor = Color()
foregroundColor:setRed(255)
foregroundColor:setGreen(255)
foregroundColor:setBlue(255)
foregroundColor:setAlpha(255)
backgroundColor = Color()
backgroundColor:setRed(0)
backgroundColor:setGreen(0)
backgroundColor:setBlue(0)
backgroundColor:setAlpha(127)

gui.top:setBaseColor(baseColor)
gui.top:setForegroundColor(foregroundColor)
gui.top:setBackgroundColor(backgroundColor)

baseColor:setRed(0)
baseColor:setGreen(0)
baseColor:setBlue(0)
baseColor:setAlpha(0)
backgroundColor:setRed(0)
backgroundColor:setGreen(0)
backgroundColor:setBlue(0)
backgroundColor:setAlpha(0)

gui.box = TextBox('use \'c_print()\' to print to console')
gui.box:setSize(480, 320)
gui.box:isFocusable(false)
gui.box:setEditable(false)
gui.box:setBaseColor(baseColor)
gui.box:setForegroundColor(foregroundColor)
gui.box:setBackgroundColor(backgroundColor)

baseColor:setRed(0)
baseColor:setGreen(255)
baseColor:setBlue(0)
baseColor:setAlpha(125)

gui.scrollarea = ScrollArea();
gui.scrollarea:setSize(480, 320)
gui.scrollarea:setPosition(0, 0)
gui.scrollarea:setBaseColor(baseColor)
gui.scrollarea:setForegroundColor(foregroundColor)
gui.scrollarea:setBackgroundColor(backgroundColor)
gui.scrollarea:setContent(gui.box);

backgroundColor:setRed(127)
backgroundColor:setGreen(0)
backgroundColor:setBlue(0)
backgroundColor:setAlpha(127)

gui.top:add(gui.scrollarea)
gui.input = TextField('') 
gui.input:setSize(480, 40)
gui.input:setPosition(0, 320)
gui.input:isFocusable(true)
gui.input:setBaseColor(baseColor)
gui.input:setForegroundColor(foregroundColor)
gui.input:setBackgroundColor(backgroundColor)
gui.input:setEventId('LineEntered')
gui.top:add(gui.input)

