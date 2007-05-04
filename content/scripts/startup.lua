--[[ 
  This is short startup script.
--]]

require "content/scripts/functional"

function test(_if,_then,_else)
	if _if == nil then return _else end
	if _if then return _then else return _else end
end

--[[
	Warm welcome to the console.
--]] 
console.print(unpack(vfs.readlines("content/etc/welcome.txt" or {})))

--[[
	Load custom VFS sources with nice output in console.
--]]
function load_vfs_sources(filename)
	local valid_line = function(line) 
		return line ~= "" and string.sub(line,1,1) ~= '#'
	end
	local load_source = function(src)
		local msg = "Loading VFS source: " .. src .. " "
		if vfs.addSource(src) then 
			msg = msg .. "OK"
		else
			msg = msg .. "FAILED"
		end
		console.print(msg)
	end
	map(load_source,filter(valid_line, vfs.readlines(filename)))
end


dofile('content/scripts/bindings.lua')

function run_dir(dirname)
	local function _run_dir()
		local files = vfs.listFiles(dirname)
		table.sort(files)
		print(unpack(files))
		for _,filename in ipairs(files) do
			local pos = string.find(filename,"\.lua$")
			if pos then
				filename = dirname  .. string.sub(filename,1,pos-1)
				if pcall(require,filename) then
					console.print("Loaded Auto-Run Script: '" .. filename .. ".lua'")
				else
					error("Errors in " .. filename)
				end
			end
		end
	end

	if not pcall(_run_dir) then
		console.print("Errors while loading Auto-Run scripts - aborted.")
	end
end

run_dir('./content/scripts/autorun/')

-- Load the VFS sources _after_ calling listFiles really speeds up
-- engine startup. 

load_vfs_sources("content/etc/vfs_sources.txt")
