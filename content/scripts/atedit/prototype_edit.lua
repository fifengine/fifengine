require("content/scripts/class")
require("content/scripts/atedit/prototype")
require("content/scripts/atedit/dialog")
require("content/scripts/atedit/help_dialog")
require("content/scripts/atedit/param_edit")
require("content/scripts/atedit/visual_edit")

PrototypeDialog = class(Dialog,function(self,optargs)
	self.proto     = optargs[1] or Prototype{}
	self.visual    = Visual()
	self.on_accept = optargs.on_accept or function(proto) console.print(proto:tostring()) end
	self.on_cancel = optargs.on_cancel or function() end
	optargs.caption = optargs.caption or "Edit Prototype"
	optargs.size    = optargs.size or {305,380}

	Dialog.init(self,optargs)
	self:addButton("Preview",function(event)
		self:evaluate()
		self.help_dialog:setText( self.proto:tostring() )
		self.help_dialog:show()
	end)

	self:addButton("Accept", function (event)
		event.dialog:hide()
		self.on_accept(self:evaluate())
	end)
	self:addButton("Close", function (event) event.dialog:hide() self.on_cancel() end )

	local name_label = Label("Name:")
	name_label:setPosition(0,0)
	self:add( name_label )

	local visual_label = Label("--unset--")
	visual_label:setPosition(20,100)
	visual_label:setBaseColor(Color(255,255,255))
	self:add( visual_label )


	local param_start_y = 150
	local param_label = Label("Parameters:")
	param_label:setPosition(0,param_start_y)
	self:add(param_label)

	self.apply_style = false

	self.text_font = TTFont("content/fonts/FreeMono.ttf",12)
	self.text_font:setColor(0,0,0)

	self.list_font = TTFont("content/fonts/FreeMono.ttf",12)
	self.list_font:setColor(255,255,255)

	local name_field = TextField("Prototype Name")
	name_field:setPosition(90,00)
	name_field:setFont( self.text_font )
	self:add( "name", name_field )

	local dynamic_cb = CheckBox()
	dynamic_cb:setPosition(0,20)
	dynamic_cb:setCaption( "Dynamic" )
	dynamic_cb:setFont( self.font )
	self:add( "dynamic", dynamic_cb )

	local zvalue_cb = CheckBox()
	zvalue_cb:setPosition(0,40)
	zvalue_cb:setCaption( "Z Value" )
	zvalue_cb:setFont( self.font )
	self:add( "zvalue_selected", zvalue_cb )


	local zvalue_field = TextField("00001")
	zvalue_field:setPosition(90,40)
	zvalue_field:setFont( self.text_font )
	self:add( "zvalue", zvalue_field )

	local position_cb = CheckBox()
	position_cb:setPosition(0,60)
	position_cb:setCaption( "Position" )
	position_cb:setFont( self.font )
	self:add( "position_selected", position_cb )

	local positionx_field = TextField("00001")
	positionx_field:setPosition(90,60)
	positionx_field:setFont( self.text_font )
	self:add( "x", positionx_field )


	local positiony_field = TextField("00001")
	positiony_field:setPosition(140,60)
	positiony_field:setFont( self.text_font )
	self:add( "y", positiony_field )

	local visual_cb = CheckBox()
	visual_cb:setPosition(00,80)
	visual_cb:setCaption( "Visual" )
	visual_cb:setFont( self.font )
	self:add( "visual_selected", visual_cb )

	local changev_button = Button("edit visual")
	changev_button:setPosition(300-90,100)
	changev_button:setFont( self.font )
	changev_button:setBaseColor( Color(0,0,128) )
	changev_button:setSize( 90, 20 )
	changev_button:setEventId( "edit_prototype_changevisual" )
	gui.actions.edit_prototype_changevisual = bind(PrototypeDialog.on_edit_visual,self)
	self:add(changev_button)

	local edit_button = Button("edit")
	edit_button:setPosition(185,param_start_y -5)
	edit_button:setFont( self.font )
	edit_button:setBaseColor( Color(0,0,128) )
	edit_button:setSize( 45, 20 )
	edit_button:setEventId( "edit_prototype_edit_param" )
	gui.actions.edit_prototype_edit_param = bind(PrototypeDialog.on_edit_param,self)
	self:add(edit_button)

	local add_button = Button("add")
	add_button:setPosition(235,param_start_y -5)
	add_button:setFont( self.font )
	add_button:setBaseColor( Color(0,0,128) )
	add_button:setSize( 30, 20 )
	add_button:setEventId( "edit_prototype_add_param" )
	gui.actions.edit_prototype_add_param = bind(PrototypeDialog.on_add_param,self)
	self:add(add_button)

	local del_button = Button("del")
	del_button:setPosition(270,param_start_y - 5)
	del_button:setFont( self.font )
	del_button:setBaseColor( Color(0,0,128) )
	del_button:setSize( 30, 20 )
	del_button:setEventId( "edit_prototype_del_param" )
	gui.actions.edit_prototype_del_param = bind(PrototypeDialog.on_del_param,self)

	self:add(del_button)

	local scrollarea = ScrollArea()
	scrollarea:setPosition( 0, param_start_y + 20 )
	scrollarea:setSize( 300, 400 - param_start_y  - 40 )
	scrollarea:setBackgroundColor(Color(0,0,128))
	self:add(scrollarea)

	self.listmodel = ListModel()

	local listbox = ListBox(self.listmodel)
	listbox:setFont( self.list_font )
	listbox:setBackgroundColor(Color(0,0,128))
	listbox:setSize( 300, 400 - 130  - 20 )
	scrollarea:setContent( listbox )
	self:addEval("selected_param",listbox)


	self.help_dialog = HelpDialog{ text = self.proto:tostring(), caption = "Prototype preview"}
	self.param_dialog = Param.Dialog{Param{}, on_accept=bind(PrototypeDialog.addParam,self)}
	self.visual_dialog = VisualDialog{self.visual, on_accept=function(visual)
		self.visual = visual
	end}
	self:setProto(self.proto)
end)

function PrototypeDialog:setProto(proto)
	self.proto = proto
	self:genericSet("name",proto.name)
	self:genericSet("zvalue",proto.zvalue or 1)
	self:genericSet("zvalue_selected",proto.zvalue and true)
	self:genericSet("position_selected",proto.position and true)
	self:genericSet("visual_selected",proto.visual and true)
	self:genericSet("x",proto.position and proto.position[1] or 0)
	self:genericSet("y",proto.position and proto.position[2] or 0)
	self:rebuildList()
end

function PrototypeDialog:rebuildList()
	self.paramList = {}
	self.listmodel:clear()
	for _,param in ipairs(self.proto.paramList) do
		table.insert(self.paramList,param)
		self.listmodel:addElement((string.gsub("[$type] '$name' = '$value'","$(%w+)",param)))
	end
end

function PrototypeDialog:evaluate()
	Dialog.evaluate(self)
	self:rebuildList()
	local test = function(a,b) if a then return b else return nil end end
	self.proto = Prototype{
		name = self.values.name,
		dynamic = test(self.values.dynamic, true),
		zvalue  = test(self.values.zvalue_selected, self.values.zvalue),
		position  = test(self.values.position_selected,{self.values.x,self.values.y}),
		visual    = test(self.values.visual_selected,self.visual),
		params = self.paramList
	}
	return self.proto
end

function PrototypeDialog:addParam(param)
	self.proto:addParam(param)
	self:rebuildList()
end

function PrototypeDialog:on_add_param()
	self.param_dialog:exec()
end

function PrototypeDialog:on_edit_param()
	self:evaluate()
	local iselected = self.values.selected_param + 1
	local param = self.paramList[iselected]
	if not param then return end
	self.param_dialog:setParam(param)
	self.param_dialog:exec()
end

function PrototypeDialog:on_edit_visual()
	self.visual_dialog:setVisual(self.visual)
	self.visual_dialog:exec()
end

function PrototypeDialog:on_del_param()
	self:evaluate()
	local iselected = self.values.selected_param + 1
	local param = self.paramList[iselected]
	if not param then return end
	self.proto:delParam(param.name)
	self:rebuildList()
end


on_engine_start = function()
	proto = Prototype{"Animal",params = {{ name="species", type="text", value = "canine"}} }
	proto_dlg = PrototypeDialog{proto}
	proto_dlg:show()

end