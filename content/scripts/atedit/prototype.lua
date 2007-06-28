require("content/scripts/class")
require("content/scripts/atedit/param")
require("content/scripts/atedit/visual")

Prototype = class(function(self, optargs)
	self.name = optargs.name or optargs[1] or "__unnamed__"
	-- Init from table
	for _,v in ipairs{ "position", "zvalue", "dynamic", "visual" } do
		self[v] = optargs[v] or nil
	end
	self.paramList = map(Param,optargs.params or {})
	self.params = {}
	for i,param in ipairs(self.paramList) do
		self.params[param.name] = i
	end
end)

function Prototype:addParam(param)
	local i = self.params[param.name] or (#(self.paramList) + 1)
	self.paramList[i] = param
	self.params[param.name] = i
end

function Prototype:delParam(name)
	local i = self.params[name]
	if not i then return end
	self.params[name] = nil
	table.remove(self.paramList,i)

	self.params = {}
	for i,param in ipairs(self.paramList) do
		self.params[param.name] = i
	end
end

function Prototype:toXml()
	local fmt = string.format("<prototype name=\"%s\">\n%%s</prototype>\n", self.name)
	local position = ""
	if self.position then
		position = string.format("   <position x=\"%s\" y=\"%s\"/>\n",unpack(self.position))
	end

	local zvalue = ""
	if self.zvalue then
		zvalue = string.format("   <zvalue>%s<\zvalue>\n",self.zvalue)
	end

	local dynamic = ""
	if self.dynamic then
		dynamic = string.format("  <dynamic/>\n")
	end

	local visual = ""
	if self.visual then
		visual = "   " .. self.visual:toXml() .. "\n"
	end
	

	local params = ""
	for i,param in ipairs(self.paramList) do
		params = params .. "   " .. param:toXml() .. "\n"
	end
	return string.format(fmt,position .. zvalue .. dynamic .. visual .. params)
end

function Prototype:tostring()
	local s = "Prototype{\n"
	s = s .. string.format("   name = [[%s]],\n",self.name)

	if self.position then
		s = s .. string.format("   position = { %s, %s},\n",unpack(self.position))
	end
	if self.zvalue then
		s = s .. string.format("   zvalue = %s,\n",self.zvalue)
	end
	if self.dynamic then
		s = s .. string.format("   dynamic = true,\n")
	end
	if self.visual then
		s = s .. "   visual = " .. self.visual:tostring() .. ",\n"
	end
	s = s .. "   params = {\n"

	for _,param in ipairs(self.paramList) do
		s = s .. "      " .. param:tostring() .. ",\n"
	end
	s = s .. "   }\n}\n"

	return s
end

on_engine_start = function()
	require("content/scripts/help_dialog")
	proto = Prototype{"Animal",params = {{ name="species", type="text", value = "canine"}} }
	proto.visual = Visual{"./content/gfx/critters/greywolf/wolf.xml"}
	text = proto:tostring()
	proto.position = { 100, 100 }
	proto.visual = Visual{"./art/critters/hanpwraa.frm"}
	text = text .. proto:tostring()
	text = text .. proto:toXml()
	help_dlg = HelpDialog{ text = text,size={3*screen_width/4,screen_height/2}  }
	help_dlg:exec()
end