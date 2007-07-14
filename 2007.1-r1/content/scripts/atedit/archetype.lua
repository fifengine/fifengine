require("content/scripts/class")
require("content/scripts/atedit/prototype")

Archetype = class(function(self,filename)
	self.filename = filename
	self.protoList  = {}
	self.protoNames = {}
end)

function Archetype:addPrototype(proto)
	if self.protoNames[ proto.name ] ~= nil then
		self.protoList[self.protoNames[ proto.name ]] = proto
	else
		local i = table.getn(self.protoList) + 1
		self.protoList[i] = proto
		self.protoNames[ proto.name ] = i
	end
end

function Archetype:delPrototype(name)
	if self.protoNames[ name ] == nil then return end
	table.remove(self.protoList,self.protoNames[ name ])
	self.protoNames = {}
	for i,proto in ipairs(self.protoList) do
		self.protoNames[ proto.name ] = i
	end
end

function Archetype:save()
	local file = io.open(self.filename,"w+")
	for i,proto in pairs(self.protoList) do
		file:write( proto:tostring() .. "\n" )
	end
	file:close()
end

function Archetype:load()
	local _Prototype = Prototype
	Prototype = function(t)
		print( "----###1---0")
		self:addPrototype( _Prototype(t) )
	end
	print("----###1")
	pcall(dofile,self.filename)
	print("----###2")
	Prototype = _Prototype
end

function Archetype:export(filename)
	local file = io.open(filename,"w+")
	file:write( "<archetype>\n<prototypes>\n")
	for i,proto in pairs(self.protoList) do
		file:write( proto:toXml() .. "\n" )
	end
	file:write( "</prototypes>\n</archetype>\n")
	file:close()
end


on_engine_start = function()
	archetype = Archetype("my_at.lua")
	archetype:load()
end


on_engine_stop = function()
	if archetype ~= nil then
		archetype:save()
		archetype:export("my_at.xml")
	end
end


