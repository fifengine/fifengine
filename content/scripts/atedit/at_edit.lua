require("content/scripts/class")
require("content/scripts/atedit/dialog")
require("content/scripts/atedit/help_dialog")
require("content/scripts/atedit/file_dialog")
require("content/scripts/atedit/prototype_edit")
require("content/scripts/atedit/archetype")

function Message(caption,msg)
	local dialog = HelpDialog{ caption = caption, text = msg, size={300,70} }
	dialog:show()
-- 	dialog.window:requestModalFocus()
end

function ErrorMessage(msg)
	Message("ERROR",msg)
end

ATEdit = class(Dialog,function(self,optargs)
	optargs         = optargs or {}
	optargs.size    = optargs.size or {500,500}
	optargs.caption = optargs.caption or "[FIFE] Archetype Editor 0.1"

	Dialog.init(self,optargs)
	self:addButton( "Help", bind(ATEdit.on_help,self) )
	self:addButton( "Load", bind(ATEdit.on_load,self) )
	self:addButton( "Save", bind(ATEdit.on_save,self))
	self:addButton( "Export", bind(ATEdit.on_export,self))
	self:addButton( "Close", bind(ATEdit.on_close,self))

	self.edit_proto = Label("Prototypes")
	self:add( self.edit_proto )

	self.text_font = TTFont("content/fonts/FreeMono.ttf",12)
	self.text_font:setColor(0,0,0)
	self:initProtoEdit()
	self.at = nil

	self:rebuildUI()
	if optargs.filename then
		Timer.singleshot(1,bind(ATEdit.load,self,optargs.filename))
	end
end)

function ATEdit:initProtoEdit()
	local pe = {}
	pe.widget    = Container()
	pe.widget:setPosition(0,30)
	pe.widget:setSize(500,480)

	pe.find_label = Label("Find:")
	pe.find_label:setPosition(0,0)
	pe.find_label:setFont( self.font )
	pe.widget:add(pe.find_label)

	pe.find_field = TextField("")
	pe.find_field:setFont( self.text_font )
	pe.find_field:setPosition(40,0)
	pe.find_field:setSize(365-40,20)
	pe.widget:add(pe.find_field)
	self:addEval("proto_find", pe.find_field )

	local edit_button = Button("edit")
	edit_button:setPosition(375,0)
	edit_button:setFont( self.font )
	edit_button:setBaseColor( Color(0,0,128) )
	edit_button:setSize( 45, 20 )
	edit_button:setEventId( "atedit_edit_proto" )
	gui.actions.atedit_edit_proto= bind(ATEdit.on_edit_proto,self)
	pe.widget:add(edit_button)

	local add_button = Button("add")
	add_button:setPosition(430,0)
	add_button:setFont( self.font )
	add_button:setBaseColor( Color(0,0,128) )
	add_button:setSize( 30, 20 )
	add_button:setEventId( "atedit_add_proto" )
	gui.actions.atedit_add_proto = bind(ATEdit.on_add_proto,self)
	pe.widget:add(add_button)

	local del_button = Button("del")
	del_button:setPosition(470,0)
	del_button:setFont( self.font )
	del_button:setBaseColor( Color(0,0,128) )
	del_button:setSize( 30, 20 )
	del_button:setEventId( "atedit_del_proto" )
	gui.actions.atedit_del_proto = bind(ATEdit.on_del_proto,self)
	pe.widget:add(del_button)

	pe.listmodel = ListModel()
	pe.listbox   = ListBox(pe.listmodel)
-- 	pe.listbox:setFont( self.list_font )
	pe.listbox:setBackgroundColor(Color(0,0,128))

	pe.listbox:setSize(500,450)
	self:addEval("proto_selected", pe.listbox )

	pe.scrollarea = ScrollArea()
	pe.scrollarea:setPosition(0,30)
	pe.scrollarea:setSize(500,420)
	pe.scrollarea:setBackgroundColor(Color(0,0,128))

	pe.scrollarea:setContent(pe.listbox)
	pe.widget:add( pe.scrollarea )
	self:add( pe.widget )

	self.proto_edit = pe
	self.proto_dialog = PrototypeDialog{ on_accept = bind(ATEdit.addProto,self) }

	self.findAsYouType = Timer()
	self.findAsYouType:setInterval( 500 )
	self.findAsYouType:setCallback( bind(ATEdit.rebuildUI, self) )

end

function ATEdit:rebuildUI()
	self:evaluate()
	self.proto_edit.listmodel:clear()
	self.protoList = {}
	if self.at ==  nil then return end
	for i,proto in ipairs(self.at.protoList) do
		local valid, state = pcall(string.find,proto.name,self.values.proto_find)
		if valid and state then
			self.proto_edit.listmodel:addElement( proto.name )
			table.insert(self.protoList,i)
		else
			print(state)
		end
	end
end
function ATEdit:getSelectedProto()
	self:evaluate()
	if self.at == nil then return nil end
	local i = self.values.proto_selected
	if i == nil then return nil end
	return self.protoList[i + 1]
end

function ATEdit:addProto(proto)
	if self.at == nil then return end
	self.at:addPrototype(proto)
	self:rebuildUI()
end

function ATEdit:show()
	Dialog.show(self)
	self.findAsYouType:start()
end

function ATEdit:hide()
	Dialog.hide(self)
	self.findAsYouType:start()
end

function ATEdit:on_add_proto(event)
	self.proto_dialog:show()
end

function ATEdit:on_edit_proto(event)
	local i = self:getSelectedProto()
	if not i then return end
	local proto = self.at.protoList[i]
	self.proto_dialog:setProto(proto)
	self.proto_dialog:show()
end

function ATEdit:on_del_proto(event)
	local i = self:getSelectedProto()
	if not i then return end
	local proto = self.at.protoList[i]
	self.at:delPrototype(proto.name)
	self:rebuildUI()
end

function ATEdit:on_help(event)
	local dialog = HelpDialog{ caption = "ATEdit - HELP", text = ATEdit.helpText }
	dialog:show()
end

function ATEdit:load(filename)
	self.at = Archetype(filename)
	self.at:load()
	--Message("INFO","Archetype loaded '" .. filename .. "'")
	self:rebuildUI()
end

function ATEdit:on_load(event)
	local on_select = function(filename)
		Timer.singleshot(100,bind(ATEdit.load,self,filename))
	end
	local filedialog = FileDialog{ caption="Select File to load ", on_select = on_select }
	filedialog:exec()
end

function ATEdit:on_export(event)
	if self.at == nil then
		ErrorMessage("No archetype loaded.")
	else
		local on_select = function(filename)
			self.at:export(filename)
			local dialog = HelpDialog{ caption = "Exported to: " .. filename, filename=filename}
			dialog:show()
			
		end
		local filedialog = FileDialog{ caption="Select File to export to", on_select = on_select }
		filedialog:exec()
	end
end

function ATEdit:on_save(event)
	if self.at == nil then
		ErrorMessage("No archetype loaded.")
	else
		self.at:save()
		Message("INFO","Archetype save to '" .. self.at.filename .. "'")
	end
end

function ATEdit:on_close(event)
	self:hide()
	engine.stop()
end

on_engine_start = function()
	filename = engine.getCommandLine()[2] or "./my_at.lua"
	atedit = ATEdit{ filename = filename }
	atedit:exec()
end

ATEdit.helpText = [[
== Introduction ==

Archetypes hold the basic data used in maps
and throughout the game. To be more precise,
they provide the IDs and names used in the
[Map Format].

ATEdit is an application written in Lua and
executed within FIFE, that allows editing
of these archetypes.

It uses an own save format, which basically
is lua code that generates an archetype
representation in memory.

From this save format it can export to
the XML format that is used in FIFE.
The exported files can be edited by
hand if necessary, copied and pasted,
if you like.

== Startup ==

On startup ATEdit tries to load the file
"my_at.lua". You can change this in the file
"content/scripts/atedit/at_edit.lua" or
you can load another file.

Note that as of version 0.1 ATEdit, does
not have a NEW button yet :-/ so you'll
have to create an empty file.

== Features ==

* Prototypes can be edited, load and saved.

== Missing features ==

* Inventories
* Dependant Archetypes
* Dependant Prototypes
* Tilesets
* Animation editor
* Preview on a map.

== Bugs ==

Oh. Well. Some, I guess.


]]
