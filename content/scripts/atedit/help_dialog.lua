require("content/scripts/class")
require("content/scripts/atedit/dialog")

HelpDialog = class(Dialog, function (self,optargs)

	-- Check args
	self.filename = optargs.filename or nil
	self.text     = optargs.text or nil

	if self.filename == nil and self.text == nil then
		error("HelpDialog needs either filename or text as parameter",2)
	end

	self.text = self.text or vfs.read(self.filename)
	if self.text == nil then
		error("HelpDialog - no text given",2)
	end

	optargs.size = optargs.size or {screen_width/2,screen_height/2}
	optargs.caption = optargs.caption or "Help"

	-- Init Dialog
	Dialog.init(self,optargs)
	self:addButton( "Close", function (event) event.dialog:hide() end )

	-- Init widgets
	self.text_font = TTFont("content/fonts/FreeMono.ttf",12)
	self.text_font:setColor(255,255,255)

	self.textbox = TextBox(self.text)
	self.textbox:setBackgroundColor(Color(0,0,128))
	self.textbox:setBaseColor(Color(0,0,255))
	self.textbox:setForegroundColor(Color(255,255,255))
	self.textbox:setSize(self.size[1],self.size[1]-10)
	self.textbox:setFont( self.text_font )

	self.scrollarea = ScrollArea()
	self.scrollarea:setBackgroundColor(Color(0,0,128))
	self.scrollarea:setContent( self.textbox )
	self.scrollarea:setSize(unpack(self.size))

	self:add( self.scrollarea )
end)

function HelpDialog:setText(text)
	self.text = text
	self.textbox:setText( text )
end

on_engine_start = function()
	help_dlg = HelpDialog{ filename = "./content/etc/welcome.txt" }
	help_dlg:exec()
end