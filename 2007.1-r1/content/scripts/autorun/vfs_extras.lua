-- VFS extras

function vfs.exportFile(filename,outfilename)
	local data = vfs.read(filename)
	local file = io.open(outfilename,"wb")
	file:write(data)
	file:close()
end
