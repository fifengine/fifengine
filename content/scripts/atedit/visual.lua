require("content/scripts/class")

Visual = class(function(self,optargs)
	optargs = optargs or {}
	if optargs[1] then
		optargs.source = optargs[1]
	end

	if optargs.source == nil then
		optargs.source = ""
	end

	if optargs.offset == nil then
		optargs.offset = {0,0}
	end

	if optargs.type == nil then
		local image = Image(optargs.source)
		if image:isAnimation() then
			optargs.type = "animation"
		else
			optargs.type = "image"
		end
	end

	self.source = optargs.source
	self.type   = optargs.type
	self.offset = {optargs.offset[1],optargs.offset[2]}
end)

function Visual:tostring()
	local fmt = "Visual{[[%s]], type=\"%s\", offset={%s,%s} }"
	return string.format(fmt,self.source,self.type,unpack(self.offset))
end

function Visual:toXml()
	local s = string.gsub("<visual><$type source=\"$source\" %s/></visual>","$(%w+)",self)
	local offset_str = string.format("y_offset=\"%s\" x_offset=\"%s\"",unpack(self.offset))
	return string.format(s,offset_str)
end

function Visual:toImage()
	return Image(self.source)
end

function Visual:toIcon()
	return Icon(Image(self.source))
end
