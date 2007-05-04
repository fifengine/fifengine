require("content/scripts/get_filename")



function new_prototype() 
	return {
		name = "unname",
		position = { used = 0, value = { 1000, 1000 } },
		zvalue   = { used = 0, value = 1 },
		dynamic  = { used = 0, value = 1 },
		params   = {}
	}
end

_edit_prototype = {
	proto = new_prototype(),
}

_edit_prototype.make_window = function ()
--	local font = TTFont("content/fonts/FreeMono.ttf",12)
	local font = TTFont("content/fonts/falloutd.ttf",12)
	local txtfont = TTFont("content/fonts/FreeMono.ttf",12)
	txtfont:setColor(Color(0,0,0))
	local listfont = TTFont("content/fonts/FreeMono.ttf",12)
	listfont:setColor(Color(255,255,255))
	

	local container = Container()
	local window = Window(container,"Edit Prototype")

	window:setSize(320,440)
	window:setPosition(screen_width/2 - 320/2,screen_height/2 - 200)
	window:setBaseColor(Color(80,80,255))
	window:setFont( font )
	window:setTitleBarHeight(20)

	container:setSize(320,440)
	container:setPosition(0,0)
	container:setBaseColor(Color(80,80,255))

	local name_label = Label("Name:")
	name_label:setPosition(10,10)
	name_label:setFont( font )

	local name_field = TextField("Prototype Name")
	name_field:setPosition(100,10)
	name_field:setFont( txtfont )

	local dynamic_cb = CheckBox()
	dynamic_cb:setPosition(10,30)
	dynamic_cb:setCaption( "Dynamic" )
	dynamic_cb:setFont( font )

	local zvalue_cb = CheckBox()
	zvalue_cb:setPosition(10,50)
	zvalue_cb:setCaption( "Z Value" )
	zvalue_cb:setFont( font )

	local zvalue_field = TextField("00001")
	zvalue_field:setPosition(100,50)
	zvalue_field:setFont( txtfont )

	local position_cb = CheckBox()
	position_cb:setPosition(10,70)
	position_cb:setCaption( "Position" )
	position_cb:setFont( font )

	local positionx_field = TextField("00001")
	positionx_field:setPosition(100,70)
	positionx_field:setFont( txtfont )

	local positiony_field = TextField("00001")
	positiony_field:setPosition(150,70)
	positiony_field:setFont( txtfont )

	local visual_cb = CheckBox()
	visual_cb:setPosition(10,90)
	visual_cb:setCaption( "Visual" )
	visual_cb:setFont( font )

	local visual_label = Label("--unset--")
	visual_label:setPosition(30,110)
	visual_label:setBaseColor(Color(255,255,255))
	visual_label:setFont( font )

	local changev_button = Button("edit visual")
	changev_button:setPosition(310-90,110)
	changev_button:setFont( font )
	changev_button:setBaseColor( Color(0,0,128) )
	changev_button:setSize( 90, 20 )
	changev_button:setEventId( "edit_prototype_changevisual" )

	local param_start_y = 150
	local param_label = Label("Parameters:")
	param_label:setPosition(10,param_start_y)
	param_label:setFont( font )

	local edit_button = Button("edit")
	edit_button:setPosition(195,param_start_y -5)
	edit_button:setFont( font )
	edit_button:setBaseColor( Color(0,0,128) )
	edit_button:setSize( 45, 20 )
	edit_button:setEventId( "edit_prototype_edit_param" )

	local add_button = Button("add")
	add_button:setPosition(245,param_start_y -5)
	add_button:setFont( font )
	add_button:setBaseColor( Color(0,0,128) )
	add_button:setSize( 30, 20 )
	add_button:setEventId( "edit_prototype_add_param" )

	local del_button = Button("del")
	del_button:setPosition(280,param_start_y - 5)
	del_button:setFont( font )
	del_button:setBaseColor( Color(0,0,128) )
	del_button:setSize( 30, 20 )
	del_button:setEventId( "edit_prototype_del_param" )

	local scrollarea = ScrollArea()
	scrollarea:setPosition( 10, param_start_y + 20 )
	scrollarea:setSize( 300, 400 - param_start_y  - 40 )
	scrollarea:setBackgroundColor(Color(0,0,128))

	local listmodel = ListModel()
	local listbox = ListBox(listmodel)
	listbox:setFont( listfont )
	listbox:setBackgroundColor(Color(0,0,128))
	listbox:setSize( 300, 400 - 130  - 20 )
	scrollarea:setContent( listbox )

	local close_button = Button("Cancel")
	close_button:setPosition(210,400 - 10 )
	close_button:setFont( font )
	close_button:setBaseColor( Color(0,0,128) )
	close_button:setSize( 100, 20 )
	close_button:setEventId( "edit_prototype_close" )

	local accept_button = Button("accept")
	accept_button:setPosition(105,400 - 10 )
	accept_button:setFont( font )
	accept_button:setBaseColor( Color(0,0,128) )
	accept_button:setSize( 100, 20 )
	accept_button:setEventId( "edit_prototype_accept" )

	container:add( name_label )
	container:add( name_field )
	container:add( dynamic_cb )
	container:add( zvalue_cb )
	container:add( zvalue_field )
	container:add( position_cb )
	container:add( positionx_field )
	container:add( positiony_field )
	container:add( visual_cb )
	container:add( visual_label )
	container:add( changev_button )

	container:add( edit_button )
	container:add( add_button )
	container:add( del_button )
	container:add( param_label )
	container:add( scrollarea )

	container:add( accept_button )
	container:add( close_button )

	_edit_prototype.listmodel = listmodel
	_edit_prototype.listbox   = listbox
	_edit_prototype.name_field = name_field
	_edit_prototype.dynamic_cb = dynamic_cb
	_edit_prototype.zvalue_cb = zvalue_cb
	_edit_prototype.zvalue_field = zvalue_field
	_edit_prototype.position_cb  = position_cb
	_edit_prototype.positionx_field = positionx_field
	_edit_prototype.positiony_field = positionx_field
	_edit_prototype.window    = window
end

_edit_prototype.make_paramwidget = function ()
	local font = TTFont("content/fonts/falloutd.ttf",12)
	local txtfont = TTFont("content/fonts/FreeMono.ttf",12)
	txtfont:setColor(Color(0,0,0))
	
	local container = Container()
	local window = Window(container,"Edit Parameter")

	window:setSize(320,150)
	window:setPosition(screen_width/2 - 320/2,screen_height/2 - 200/2)
	window:setBaseColor(Color(80,80,255))
	window:setFont( font )
	window:setTitleBarHeight(20)

	container:setSize(320,200)
	container:setPosition(0,0)
	container:setBaseColor(Color(80,80,255))

	local name_label = Label("Name")
	name_label:setPosition(10,10)
	name_label:setFont( font )

	local name_field = TextField("Param Name")
	name_field:setPosition(10,30)
	name_field:setFont( txtfont )
	_edit_prototype.param_widget_name = name_field

	local type_label = Label("Type")
	type_label:setPosition(140,10)
	type_label:setFont( font )

	local listmodel = ListModel()
	listmodel:addElement( "text" )
	listmodel:addElement( "int" )
	_edit_prototype.param_widget_types = { "text", "int" }

	local type_field = DropDown(listmodel)
	type_field:setPosition(140,30)
	type_field:setFont( txtfont )
	_edit_prototype.param_widget_type = type_field

	local value_label = Label("Value")
	value_label:setPosition(10,50)
	value_label:setFont( font )

	local value_field = TextField("Param value")
	value_field:setPosition(10,70)
	value_field:setFont( txtfont )
	value_field:setSize(300,20)

	_edit_prototype.param_widget_value = value_field


	local close_button = Button("Cancel")
	close_button:setPosition(160+5,100 )
	close_button:setFont( font )
	close_button:setBaseColor( Color(0,0,128) )
	close_button:setSize( 100, 20 )
	close_button:setEventId("edit_prototype_edit_param_close")

	local accept_button = Button("Accept")
	accept_button:setPosition(60-5,100 )
	accept_button:setFont( font )
	accept_button:setBaseColor( Color(0,0,128) )
	accept_button:setSize( 100, 20 )
	accept_button:setEventId("edit_prototype_edit_param_accept")

	container:add( name_label )
	container:add( name_field )
	container:add( type_label )
	container:add( type_field )
	container:add( value_label )
	container:add( value_field )
	container:add( close_button)
	container:add( accept_button )

	_edit_prototype.param_widget = window
end

_edit_prototype.make_visualwidget = function ()
	local font = TTFont("content/fonts/falloutd.ttf",12)
	local txtfont = TTFont("content/fonts/FreeMono.ttf",12)
	txtfont:setColor(Color(0,0,0))
	
	local container = Container()
	local window = Window(container,"Edit visual")

	window:setSize(320,360)
	window:setPosition(screen_width/2 - 320/2,screen_height/2 - 200/2)
	window:setBaseColor(Color(80,80,255))
	window:setFont( font )
	window:setTitleBarHeight(20)

	container:setSize(320,360)
	container:setPosition(0,0)
	container:setBaseColor(Color(80,80,255))

	local file_label = Label("File")
	file_label:setPosition(10,10)
	file_label:setFont( font )

	local file_field = TextField("./")
	file_field:setPosition(50,10)
	file_field:setFont( txtfont )
	file_field:setSize(240-50-20,20)
	_edit_prototype.visual_widget_file = file_field

	local file_button = Button("browse")
	file_button:setPosition(240,10)
	file_button:setFont( font )
	file_button:setSize(60,20)
	file_button:setBaseColor( Color(0,0,128) )
	file_button:setEventId("edit_prototype_edit_visual_browse")

	local scrollarea = ScrollArea()
	scrollarea:setPosition( 10,70 )
	scrollarea:setSize( 300, 320-20-80 )
	scrollarea:setBackgroundColor(Color(0,0,0))
	_edit_prototype.edit_visual_scrollarea = scrollarea

	local position_cb = CheckBox()
	position_cb:setPosition(10,40)
	position_cb:setCaption( "OFFSET" )
	position_cb:setFont( font )

	local positionx_field = TextField("00001")
	positionx_field:setPosition(100,40)
	positionx_field:setFont( txtfont )

	local positiony_field = TextField("00001")
	positiony_field:setPosition(150,40)
	positiony_field:setFont( txtfont )

	local close_button = Button("Cancel")
	close_button:setPosition(160+5,300 )
	close_button:setFont( font )
	close_button:setBaseColor( Color(0,0,128) )
	close_button:setSize( 100, 20 )
	close_button:setEventId("edit_prototype_edit_visual_close")

	local accept_button = Button("Accept")
	accept_button:setPosition(60-5,300 )
	accept_button:setFont( font )
	accept_button:setBaseColor( Color(0,0,128) )
	accept_button:setSize( 100, 20 )
	accept_button:setEventId("edit_prototype_edit_visual_accept")

	container:add( close_button)
	container:add( accept_button )


	container:add( file_label )
	container:add( file_field )
	container:add( file_button )

	container:add( position_cb )
	container:add( positionx_field )
	container:add( positiony_field )

	container:add( scrollarea )

	_edit_prototype.visual_widget = window
end

_edit_prototype.rebuild_list = function()
	_edit_prototype.listmodel:clear()
	_edit_prototype.proto.iparams = {}
	for _,proto in pairs(_edit_prototype.proto.params) do
		local visname = "[" .. proto.typ .. "] " .. proto.name .. "  = " .. tostring(proto.value)
		_edit_prototype.listmodel:addElement( visname )
		table.insert(_edit_prototype.proto.iparams,proto.name)
	end
end

_edit_prototype.current_param = function()
	local i = _edit_prototype.listbox:getSelected() + 1
	local name = _edit_prototype.proto.iparams[ i ]
	return name
end


gui.actions.edit_prototype_edit_param = function()
	local name = _edit_prototype.current_param()
	if name == nil then return end
	local param = _edit_prototype.proto.params[name]

	_edit_prototype.param_widget_name:setText( param.name)
	_edit_prototype.param_widget_type:setSelected(1) --fixme
	_edit_prototype.param_widget_value:setText(param.value)

	guimanager.add(_edit_prototype.param_widget)
end

gui.actions.edit_prototype_add_param = function()
	_edit_prototype.param_widget_name:setText( "" )
	_edit_prototype.param_widget_type:setSelected(1)
	_edit_prototype.param_widget_value:setText( "" )

	guimanager.add(_edit_prototype.param_widget)
end

gui.actions.edit_prototype_del_param = function()
	local name = _edit_prototype.current_param()
	if name == nil then return end
	_edit_prototype.proto.params[ name ] = nil
	_edit_prototype.rebuild_list()
end

_edit_prototype.set_visual = function(filename)
	_edit_prototype.visual_widget_file:setText( filename )
	local image = Icon(Image(filename))
	_edit_prototype.edit_visual_scrollarea:setContent(image)
end


gui.actions.edit_prototype_edit_visual_browse = function()
	file_dialog{ on_ok = _edit_prototype.set_visual }
end

gui.actions.edit_prototype_edit_visual_close = function()
	guimanager.remove(_edit_prototype.visual_widget)
end

gui.actions.edit_prototype_edit_visual_accept = function()
	guimanager.remove(_edit_prototype.visual_widget)
end

gui.actions.edit_prototype_changevisual = function()
	guimanager.add(_edit_prototype.visual_widget)
end


gui.actions.edit_prototype_accept = function()
	guimanager.remove(_edit_prototype.window)
	_edit_prototype.proto.name = _edit_prototype.name_field:getText()
	_edit_prototype.proto.dynamic.used = _edit_prototype.dynamic_cb:isMarked()
	_edit_prototype.proto.zvalue.used = _edit_prototype.zvalue_cb:isMarked()
	_edit_prototype.proto.zvalue.value = _edit_prototype.zvalue_field:getText()
	_edit_prototype.proto.position.used  = _edit_prototype.position_cb:isMarked()
	_edit_prototype.proto.position.value = { 
		_edit_prototype.positionx_field:getText(),
		_edit_prototype.positiony_field:getText()
	}

	_edit_prototype.on_ok( _edit_prototype.proto )
end

gui.actions.edit_prototype_close = function()
	guimanager.remove(_edit_prototype.window)
	_edit_prototype.on_cancel()
end


_edit_prototype.add_param = function(name,typ,value)
	_edit_prototype.proto.params[ name ] = { name= name, typ = typ, value = value }
	_edit_prototype.rebuild_list()
end

gui.actions.edit_prototype_edit_param_close = function()
	guimanager.remove(_edit_prototype.param_widget)
end

gui.actions.edit_prototype_edit_param_accept = function()
	_edit_prototype.add_param(
		_edit_prototype.param_widget_name:getText(),
		_edit_prototype.param_widget_types[ _edit_prototype.param_widget_type:getSelected() + 1 ],
		_edit_prototype.param_widget_value:getText())
	guimanager.remove(_edit_prototype.param_widget)
end


_edit_prototype.show_window = function()
	if _edit_prototype.window == nil then
		_edit_prototype.make_visualwidget()
		_edit_prototype.make_paramwidget()
		_edit_prototype.make_window()
	end
	guimanager.add( _edit_prototype.window )
end

proto_to_xml = function(proto)
	local fmt = string.format("<prototype name=\"%s\">\n%%s\n</prototype>", proto.name)
	local position = ""
	if proto.position.used then
		position = string.format("<position x=\"%s\" y=\"%s\" />",
			proto.position.value[1], proto.position.value[2])
	end

	local zvalue = ""
	if proto.zvalue.used then
		zvalue = string.format("<zvalue>%s<\zvalue>",proto.zvalue.value)
	end

	local dynamic = ""
	if proto.dynamic.used then
		dynamic = string.format("<dynamic/>")
	end

	local params = "\n"
	for name,param in pairs(_edit_prototype.proto.params) do
		params = params .. string.format("<param name=\"%s\" type=\"%s\">%s</param>\n",
			param.name, param.typ, param.value)

	end

	fmt = string.format(fmt,position .. zvalue .. dynamic .. params)
	return fmt
end

proto_debug = function(proto)
	console.print(proto_to_xml(proto))
end

_edit_prototype.on_ok     = proto_debug
_edit_prototype.on_cancel = function() end

edit_prototype = function( proto )
	if proto == nil then
		proto = new_prototype()
	end
	_edit_prototype.proto = proto
	_edit_prototype.show_window()
	_edit_prototype.add_param( "name","text","Hans Kaufmann")
end

on_engine_start = edit_prototype