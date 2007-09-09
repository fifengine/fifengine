-- simple filter example over rgb bitmap
for y=0, height-1 do
	for x=0, width-1 do
    r,g,b = get_rgb(x,y)
    v = (r + g + b)/3
    v = v + math.random(0,3)
    set_rgb(x,y, v, v, v)
	end
end
