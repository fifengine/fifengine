require("content/scripts/class")
require("content/scripts/atedit/dialog")
require("content/scripts/atedit/param")

Param.Dialog = class(Dialog,function(self,optargs)
	self.param     = optargs[1]
	self.on_accept = optargs.on_accept or function(param) console.print(param:tostring()) end
	self.on_cancel = optargs.on_cancel or function() end
	optargs.caption = optargs.caption or "Edit Parameter"
	optargs.size    = optargs.size or {300,100}
	Dialog.init(self,optargs)

	self:addButton("Accept", function (event)
		event.dialog:hide()
		self.on_accept(Param(self:evaluate()))
	end)
	self:addButton("Close", function (event) event.dialog:hide() self.on_cancel() end )

	local name_label = Label("Name")
	name_label:setPosition(0,0)
	self:add(name_label)

	local type_label = Label("Type")
	type_label:setPosition(140,0)
	self:add( type_label )

	local value_label = Label("Value")
	value_label:setPosition(0,50)
	self:add( value_label )


	self.apply_style = false
	self.text_font = TTFont("content/fonts/FreeMono.ttf",12)
	self.text_font:setColor(0,0,0)
	
	self.name_field = TextField("Param Name")
	self.name_field:setPosition(0,20)
	self.name_field:setFont( self.text_font )
	self:add("name",self.name_field)

	self.listmodel = ListModel()
	map(bind(ListModel.addElement, self.listmodel),Param.typesList )

	self.type_field = DropDown(self.listmodel)
	self.type_field:setPosition(140,20)
	self.type_field:setFont( self.text_font )
	self:add("type",self.type_field)

	self.value_field = TextField("Param value")
	self.value_field:setPosition(0,70)
	self.value_field:setFont( self.text_font )
	self.value_field:setSize(300,20)
	self:add( "value", self.value_field )

	self:setParam(self.param)

	self.validation_timer = Timer()
	self.validation_timer:setInterval( 500 )
	self.validation_timer:setCallback( bind(Param.Dialog.validate,self) )
end)

function Param.Dialog:setParam(param)
	self.param = Param(param)
	self.name_field:setText( param.name )
	self.value_field:setText( param.value )
	self.type_field:setSelected( Param.types[self.param.type].id - 1 )

end

function Param.Dialog:show()
	self.validation_timer:start()
	Dialog.show(self)
end

function Param.Dialog:hide()
	self.validation_timer:stop()
	Dialog.hide(self)
end

function Param.Dialog:evaluate()
	Dialog.evaluate(self)
	self.values.type = Param.typesList[ self.values.type + 1 ]
	return self.values
end

function Param.Dialog:validate()
	self.param = Param(self:evaluate())
	local valid, errmsg = self.param:isValid()
	if valid then
		self.value_field:setBackgroundColor(Color(255,255,255))
	else
		self.value_field:setBackgroundColor(Color(255,0,0))
		--console.print(errmsg)
	end
end

on_engine_start = function()
	param = Param{type="int"}
	param_dlg = Param.Dialog{param}
	param_dlg:exec()

end