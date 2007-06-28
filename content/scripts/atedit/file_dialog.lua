require("content/scripts/class")
require("content/scripts/atedit/dialog")

FileDialog = class(Dialog, function (self,optargs)
	-- Init instance values from optional arguments
	optargs = optargs or {}
	self.size = optargs.size or {screen_width/2,screen_height/2}
	self.path     = optargs.path or "./"
	self.lastpath = self:splitPath( self.path )
	self.callbacks = {}
	self.callbacks.on_select = optargs.on_select or console.print
	self.callbacks.on_cancel = optargs.on_cancel or bind(console.print,"FileDialog cancelled")

	-- Init Dialog
	Dialog.init(self,{ caption="Select File", size=self.size })
	self:addButton( "Select", bind(FileDialog.on_select, self) )
	self:addButton( "Cancel", bind(FileDialog.on_cancel, self) )

	-- Init own widgets
	self.text_font = TTFont("content/fonts/FreeMono.ttf",12)
	self.text_font:setColor(255,255,255)

	self.listmodel = ListModel()

	self.listbox = ListBox(self.listmodel)
	self.listbox:setBackgroundColor(Color(0,0,128))
	self.listbox:setBaseColor(Color(0,0,255))
	self.listbox:setForegroundColor(Color(255,255,255))
	self.listbox:setSize(self.size[1],self.size[1]-10)
	self.listbox:setFont( self.text_font )

	self.scrollarea = ScrollArea()
	self.scrollarea:setBackgroundColor(Color(0,0,128))
	self.scrollarea:setContent( self.listbox )
	self.scrollarea:setSize(unpack(self.size))

	self:add( self.scrollarea )

	-- Trigger rebuild
	Timer.singleshot( 500, bind( FileDialog.rebuildList, self) )
end)

function FileDialog:splitPath( path )
	local splitted = { "./" }
	local partial  = "./"
	for elem in string.gfind(path,"(%w+)/") do
		partial = partial .. elem .. "/"
		table.insert(splitted,partial)
-- 		console.print("SPLIT: " .. partial)
	end
	return splitted
end

function FileDialog:rebuildList()
	local prefix = function (pre,list) 
		return map(function(path) return pre .. path end, list)
	end
	local remove_hidden = bind(filter,function(path) return string.sub(path,1,1) ~= "." end)

	local listmodel = self.listmodel
	local dirs  = remove_hidden(vfs.listDirectories(self.path))
	local files = remove_hidden(vfs.listFiles(self.path))

	if table.getn(self.lastpath) > 0 then
		table.insert(dirs,1,"..")
	end

	listmodel:clear()
	
	local listmodel_add = bind(map,bind(listmodel.addElement,listmodel))
	listmodel_add(prefix("[DIR]  ", dirs))
	listmodel_add(prefix("[FILE] ", files))

	self.list = {}
	local function add(t,isdir)
		for _,name in ipairs(t) do
			table.insert(self.list,{name = name, isdir = isdir})
		end
	end
	add( dirs, 1 )
	add( files, 0 )
end

function FileDialog:on_select(event)
	local iselected = self.listbox:getSelected() + 1
	local selected  = self.list[iselected]

	if selected == nil then return end

	if selected.isdir == 1 then
		if selected.name == ".." then
			self.path = table.remove(self.lastpath)
		else
			table.insert(self.lastpath,self.path)
			self.path = self.path .. selected.name .. "/"
		end
		--console.print(self.path)
		self:rebuildList()
	else
		event.dialog:hide()
		self.callbacks.on_select( self.path .. selected.name )
	end
end

function FileDialog:on_cancel(event)
	event.dialog:hide()
	self.list = {}
	self.listmodel:clear()
	self.callbacks.on_cancel()
end

on_engine_start = function()
	local file_dialog = FileDialog{ path = "./art/critters/" }
	file_dialog:exec()
end
