--[[
	Some functional extensions that come in quite handy.
--]]

function bind1(f,arg1)
	return function (...) return f(arg1,unpack(arg)) end 
end

function bind(f,...) 
	local _arg = arg 
	if type(f) ~= "function" then error("not a function",2) end
	local bound = bind1(f,arg[1])
	for i =2,#arg do
		bound = bind1(bound,arg[i])
	end
	return bound
end

function map(func,t)
	local retval = {}
	for _,v in ipairs(t) do
		table.insert(retval,(func(v)))
	end
	return retval
end

function filter(func,t)
	local retval = {}
	if t == nil then 
		t = func
		func = function (x) return x end
	end

	for _,v in ipairs(t) do
		if func(v) then table.insert(retval,v) end
	end
	return retval
end
