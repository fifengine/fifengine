gui.actions.toggle = function()
  gui.anim:setAnimActive(true)
  bt:setCaption('wait')
end

gui.anim_end.anim = function ()
  gui.anim:setAnimActive(false)
  bt:setCaption('click')
end

function on_engine_start()

con = Container()
con:setSize(600, 400)

img1 = Image('content/gfx/tiles/chess/plain_brown.png')
img2 = Image('content/gfx/tiles/chess/plain_white.png')

bt = ClickLabel('click')
bt:setEventId('toggle')
bt:setPosition(2, 88)


gui.anim = Image('art/intrface/endanim.frm')
gui.anim:setAnimActive(false)
gui.anim:setAnimEndCallback('anim')
gui.anim:setAnimDirection(true)
icon = Icon(gui.anim)
icon:setPosition(0, 80)

con:add(bt)
con:add(icon)
con:moveToTop(bt)

guimanager.add(con)

end
