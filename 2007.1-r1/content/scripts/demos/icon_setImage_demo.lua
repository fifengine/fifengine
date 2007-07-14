gui.actions.toggle = function()
  if active_img == 1 then
    icon:setImage(img2)
    active_img = 2
  else
    icon:setImage(img1)
    active_img = 1
  end
end

function on_engine_start()

con = Container()
con:setSize(300, 300)

img1 = Image('content/gfx/tiles/chess/plain_brown.png')
img2 = Image('content/gfx/tiles/chess/plain_white.png')

bt = Button('toggle')
bt:setEventId('toggle')

icon = Icon(img1)
icon:setPosition(0, 80)

active_img = 1

con:add(bt)
con:add(icon)

guimanager.add(con)
end
