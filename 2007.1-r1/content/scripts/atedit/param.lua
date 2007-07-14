require("content/scripts/class")

Param = class(function(self,optargs)
	if optargs == nil then optargs = {} end 
	self.name  = optargs.name or "unnamed"
	self.type  = optargs.type or "text"
	self.value = optargs.value or ""
end)

function Param:isValid()
	if Param.types[ self.type ] == nil then return false, "Unknown type: " .. self.type end
	local validator = Param.types[ self.type ].validator
	if validator then
		return validator(self.value)
	end
	return true,"Ok"
end

function Param:tostring()
	local param_fmt = "Param{ name = [[$name]], type = \"$type\", value = [[$value]] }"
	return (string.gsub(param_fmt,"$(%w+)",self))
end

function Param:toXml()
	local param_fmt = "<param name=\"%s\" type=\"%s\">%s</param>"
	return (string.format(param_fmt,self.name, self.type, self.value))
end


-- Validation table (incomplete)
Param.types = {
	text = {
		id = 1
	},
	int  = {
		id = 2,
		validator = function(v) return string.match(v,"^-?[0-9]+$") end
	},
	bool = {
		id = 3,
	},
	Point = {
		id = 4,
	}
}

Param.typesList = {"text","int","bool","Point"}
