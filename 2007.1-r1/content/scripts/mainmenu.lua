-- notice: you don't have to define gui = {} and gui.actions = {} any
--         more; the engine now does that on startup


gui.actions.intro = function ()
	print('Play it one more time, Sam')
end
gui.actions.new_game = function ()
	guimanager.remove(gui.top)
	f2_new_game()
end

gui.top = Container()
guimanager.add(gui.top)
gui.top:setSize(640, 480)
gui.top:setPosition((screen_width - 640 ) / 2, (screen_height - 480) / 2)
gui.top:setOpaque(false)

gui.bg_image = Image('art/intrface/mainmenu.frm')
gui.button_up = Image('art/intrface/menuup.frm')
gui.button_down = Image('art/intrface/menudown.frm')

gui.background = Icon(gui.bg_image)
gui.top:add(gui.background)
gui.top:moveToBottom(gui.background)

gui.b1 = TwoButton(gui.button_up, gui.button_down)
gui.b1:setPosition(30, 20)
gui.b1:setEventId('intro');
gui.top:add(gui.b1)

gui.b2 = TwoButton(gui.button_up, gui.button_down)
gui.b2:setPosition(30, 60)
gui.b2:setEventId('new_game')
gui.top:add(gui.b2)

gui.b3 = TwoButton(gui.button_up, gui.button_down)
gui.b3:setPosition(30, 101)
gui.b3:setEventId('xxx')
gui.top:add(gui.b3)

gui.b4 = TwoButton(gui.button_up, gui.button_down)
gui.b4:setPosition(30, 142)
gui.b4:setEventId('yyy')
gui.top:add(gui.b4)

gui.b5 = TwoButton(gui.button_up, gui.button_down)
gui.b5:setPosition(30, 183)
gui.b5:setEventId('xxx')
gui.top:add(gui.b5)

gui.b6 = TwoButton(gui.button_up, gui.button_down)
gui.b6:setPosition(30, 224)
gui.b6:setEventId('yyy')
gui.top:add(gui.b6)

gui.fallout_font_text_mmanu = TTFont('content/fonts/falloutd.ttf', 22)
gui.fallout_font_text_mmanu:setColor(172,148,34)
gui.main_menu_label_text = {'INTRO','NEW GAME','LOAD GAME','OPTIONS','CREDITS','EXIT'}


for i = 1, 6 do
    local lable_menu_name = "label" .. i
    gui.lable_menu_name = Label(gui.main_menu_label_text[i])
    gui.lable_menu_name:setPosition(70, (-23) + i * 41)
    gui.lable_menu_name:setFont(gui.fallout_font_text_mmanu)
    gui.top:add(gui.lable_menu_name)
  end




-- vim: set noexpandtab: set shiftwidth=2: set tabstop=2:
