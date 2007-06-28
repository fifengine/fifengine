require("content/scripts/class")
require("content/scripts/atedit/prototype")
require("content/scripts/atedit/dialog")
require("content/scripts/atedit/file_dialog")
require("content/scripts/atedit/visual")


VisualDialog = class(Dialog,function(self,optargs)
	optargs = optargs or {}
	self.visual    = optargs[1] or Visual{}
	self.on_accept = optargs.on_accept or function(visual) console.print(visual:tostring()) end
	self.on_cancel = optargs.on_cancel or function() end
	optargs.caption = optargs.caption or "Edit Visual"
	optargs.size    = optargs.size or {405,380}

	Dialog.init(self,optargs)
	self:addButton("Accept", function (event)
		event.dialog:hide()
		self.on_accept(self:evaluate())
	end)
	self:addButton("Close", function (event) event.dialog:hide() self.on_cancel() end )

	local file_label = Label("File")
	self:add(file_label )


	self.scrollarea = ScrollArea()
	self.scrollarea:setPosition( 0,60 )
	self.scrollarea:setSize( 405, 320 )
	self.scrollarea:setBackgroundColor(Color(0,0,0))
	self:add( self.scrollarea )

	self.apply_style = false
	self.text_font = TTFont("content/fonts/FreeMono.ttf",12)
	self.text_font:setColor(0,0,0)

	local file_button = Button("browse")
	file_button:setPosition(330,0)
	file_button:setFont( self.font )
	file_button:setSize(60,20)
	file_button:setBaseColor( Color(0,0,128) )
	file_button:setEventId("edit_prototype_edit_visual_browse")
	gui.actions.edit_prototype_edit_visual_browse = bind(VisualDialog.on_browse,self)
	self:add(file_button )

	local file_field = TextField("./")
	file_field:setPosition(40,00)
	file_field:setFont( self.text_font )
	file_field:setSize(270,20)
	self:add( "source", file_field )

	local position_cb = CheckBox()
	position_cb:setPosition(0,30)
	position_cb:setCaption( "OFFSET" )
	position_cb:setFont( self.font )
	self:add( "offset_selected",position_cb )

	local positionx_field = TextField("00001")
	positionx_field:setPosition(90,30)
	positionx_field:setFont( self.text_font )
	self:add( "x_off", positionx_field )

	local positiony_field = TextField("00001")
	positiony_field:setPosition(140,30)
	positiony_field:setFont( self.text_font )
	self:add( "y_off", positiony_field)

	self.fileBrowser = FileDialog{ on_select = function(filename)
		self.evals.source[1]:setText( filename )
		self:evaluate()
	end}
	self:setVisual(self.visual)
end)

function VisualDialog:evaluate()
	Dialog.evaluate(self)
	self.values.offset = nil
	if self.values.offset_selected then self.offset = { self.values.x_off,self.values.y_off } end
	self.visual = Visual( self.values )
	self.scrollarea:setContent( self.visual:toIcon() )
	return self.visual
end

function VisualDialog:setVisual(visual)
	self.visual = visual
	self:genericSet("x_off",visual.offset[1])
	self:genericSet("y_off",visual.offset[2])
	self:genericSet("source",visual.source)
	self:evaluate()
end

function VisualDialog:on_browse()
	self.fileBrowser:exec()
end

on_engine_start = function()
	visdialog = VisualDialog()
	visdialog:exec()

end