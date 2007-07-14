
ListModel.add = function (self,t)
	for _,v in ipairs(t) do
		self:addElement( v )
	end
end

file_chooser = { path = "./", on_ok=console.print,lastpath={} }

function file_dialog( optargs )
	if optargs == nil then optargs = {} end

	if optargs.on_ok == nil then
		optargs.on_ok = console.print
	end

	if optargs.on_cancel == nil then
		optargs.on_cancel = function() end
	end

	-- This does NOT work somewhere near the way it should.
	if optargs.path ~= nil then
		file_chooser.lastpath = {}
		path = "./"
		for piece in optargs.path:gmatch("(%w)/") do
			path = path .. piece .. "/"
			table.insert(file_chooser.lastpath,path)
		end
		file_chooser.path = optargs.path
	end

	file_chooser.on_ok = optargs.on_ok
	file_chooser.on_cancel = optargs.on_cancel


	if file_chooser.window == nil then
		local font = TTFont("content/fonts/falloutd.ttf",12)
		local txtfont = TTFont("content/fonts/FreeMono.ttf",12)
		-- Bug? Why doesn't this set the color?
		txtfont:setColor(Color(255,255,255))
	
		local container = Container()
		local window = Window(container,"Choose Filename")
	
		window:setSize(screen_width/2,screen_height/2)
		window:setPosition(screen_width/4,screen_height/4)
		window:setBaseColor(Color(80,80,255))
		window:setFont( font )
		window:setTitleBarHeight(20)
	
		container:setSize(screen_width/2,screen_height/2)
		container:setPosition(0,0)
		container:setBaseColor(Color(80,80,255))
	
		local listmodel = ListModel()
	
		local listbox = ListBox(listmodel)
		listbox:setBackgroundColor(Color(0,0,128))
		listbox:setBaseColor(Color(0,0,255))
		listbox:setForegroundColor(Color(255,255,255))
		listbox:setSize(screen_width/2 - 40 - 10,screen_height/2 - 40)
		listbox:setFont( txtfont )
	
		local scrollarea = ScrollArea()
		scrollarea:setBackgroundColor(Color(0,0,128))
-- 		scrollarea:setBaseColor(Color(0,0,255))
		scrollarea:setContent( listbox )
		scrollarea:setPosition(10,10)
		scrollarea:setSize(screen_width/2 - 20,screen_height/2 - 80)
	
		local ok_button = Button("Select")
		ok_button:setPosition(screen_width/2 - 160,screen_height/2 - 60)
		ok_button:setFont( font )
		ok_button:setEventId("filechooser_select")	
		ok_button:setBaseColor( Color(0,0,128) )

		local cancel_button = Button("Cancel")
		cancel_button:setPosition(screen_width/2 - 100,screen_height/2 - 60)
		cancel_button:setFont( font )
		cancel_button:setEventId("filechooser_cancel")	
		cancel_button:setBaseColor( Color(0,0,128) )

		container:add( ok_button )
		container:add( cancel_button )
		container:add( scrollarea )

		file_chooser.listmodel = listmodel
		file_chooser.listbox = listbox
		file_chooser.window = window
	end

	if file_chooser.update_listmodel == nil  then
		file_chooser.update_listmodel = function()
			local function prefix(pre, t)
				local retval = {}
				for k,v in pairs(t) do
					retval[k] = pre .. v
				end
				return retval
			end
			local listmodel = file_chooser.listmodel
			local dirs  = vfs.listDirectories(file_chooser.path)
			local files = vfs.listFiles(file_chooser.path)

			if table.getn(file_chooser.lastpath) > 0 then
				table.insert(dirs,1,"..")
			end

			listmodel:clear()
			listmodel:add(prefix("[DIR]  ", dirs))
			listmodel:add(prefix("[FILE] ", files))

			file_chooser.list = {}
			local function add(t,isdir)
				for _,name in ipairs(t) do
					table.insert(file_chooser.list,{name = name, isdir = isdir})
				end
			end
			add( dirs, 1 )
			add( files, 0 )
		end
	end

	if file_chooser.select == nil then
		file_chooser.select = function()
			local iselected = file_chooser.listbox:getSelected() + 1
			local selected = file_chooser.list[iselected]

			if selected == nil then return end

			if selected.isdir == 1 then
				if selected.name == ".." then
					file_chooser.path = table.remove(file_chooser.lastpath)
				else
					table.insert(file_chooser.lastpath,file_chooser.path)
					file_chooser.path = file_chooser.path .. selected.name .. "/"
				end

				console.print(file_chooser.path)
				file_chooser.update_listmodel()
			else
				guimanager.remove( file_chooser.window )
				file_chooser.on_ok( file_chooser.path .. selected.name )
			end
		end
	end

	if file_chooser.cancel == nil then
		file_chooser.cancel = function()
			guimanager.remove( file_chooser.window )
			file_chooser.on_cancel()
		end
	end

	gui.actions.filechooser_select = file_chooser.select
	gui.actions.filechooser_cancel = file_chooser.cancel

	guimanager.add(file_chooser.window)
	Timer.singleshot( 20, file_chooser.update_listmodel )
	console.hide()
	file_chooser.window:requestModalFocus()
end

on_engine_start = file_dialog