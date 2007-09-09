-- simple filter example over rgb bitmap
for y=0, height-1 do
	for x=0, width-1 do
    r,g,b = get_rgb(x,y)
    v = (r + g + b)/3
    if (v == 255) then
      set_rgb(x,y, 251, 251, 251)
    end
	end
end
