-- FIXME:
-- * the following functions should actually do anything!
-- * a lot is still missing ; more code needs to be written and exported to lua

-- details:
-- * a text-scrollbox on the left (FIXME: need scrolling)
-- * the dotted-bar
--   ** does it only trigger one menu? which one?
--   ** a class derived from gcn::Button-child should do it ?
-- * displaying the actual numbers (HP, AC)
-- * the combat-turn-buttons (right)

-- for each ~window~ that can be hidden/active we
-- want a variable to store the state
gui.inven_active = 0
gui.menu_active = 0
gui.skill_active = 0
gui.cmbt_btn_active = 0

gui.actions.show_inventory = function ()
	gui.cmb_anim:setAnimActive(false)
	if gui.inven_active == 0 then
		gui.inven_active = 1
		gui.invbox:setVisible(true)	
	else
		gui.inven_active = 0
	      gui.invbox:setVisible(false)
	end

end

gui.actions.combat_start = function ()
	gui.cmb_anim:setAnimActive(true)
	gui.combat_bt_holder:setVisible(false)
end

gui.actions.show_game_options = function ()
	gui.cmb_anim:setAnimActive(false)
	if gui.menu_active == 0 then
		gui.menu_active = 1
		gui.menu:setVisible(true)	
	else
		gui.menu_active = 0
	      gui.menu:setVisible(false)
	end
end

gui.actions.show_map = function ()
end

gui.actions.show_character_screen = function ()
end

gui.actions.show_pipboy = function ()
end

gui.actions.change_weapon = function ()
	
end

gui.actions.skill = function ()
	gui.cmb_anim:setAnimActive(false)
	if gui.skill_active == 0 then
		gui.skill_active = 1
		gui.skldx:setVisible(true)	
	else
		gui.skill_active = 0
	      gui.skldx:setVisible(false)
	end
end

gui.actions.exit = function ()
	guimanager.remove(gui.menu)
	guimanager.remove(gui.invbox)
	guimanager.remove(gui.skldx)
	state_manager.deactivate("MapView")
	guimanager.remove(gui.bottom)
	dofile('content/scripts/mainmenu.lua')
end

gui.anim_end.cmb_btn_anim = function ()
	
	     	gui.cmb_anim:setAnimActive(false)
		if gui.cmbt_btn_active == 0 then
			gui.cmbt_btn_active = 1
			gui.combat_bt_holder:setVisible(true)
		else
			gui.cmbt_btn_active = 0
		end
	
end

--################################################
--
-- the 'top' container at the bottom of the screen
--
--################################################
gui.bottom = Container()
gui.bottom:setSize(640, 99)
-- center position
gui.bottom:setPosition((screen_width/2 - 320), (screen_height - 99))
guimanager.add(gui.bottom)


--################################################
--
-- the 'menu' container at the center of the screen
--
--################################################
gui.menu = Container()
gui.menu:setSize(164, 217)
-- center position
gui.menu:setPosition((screen_width/2 - 82), (screen_height/2 - 138))
guimanager.add(gui.menu)
gui.menu:setVisible(false)


--################################################
--
-- the 'Inventory' container at the center of the screen
--
--################################################
gui.invbox = Container()
gui.invbox:setSize(499, 377)
-- center position
gui.invbox:setPosition((screen_width/2 - 250), (screen_height/2 - 238))
guimanager.add(gui.invbox)
gui.invbox:setVisible(false)


--################################################
--
-- the 'Skill' container at the center of the screen
--
--################################################
gui.skldx = Container()
gui.skldx:setSize(185, 368)
-- center position
gui.skldx:setPosition((screen_width/2 + 135), (screen_height - 467))
guimanager.add(gui.skldx)
gui.skldx:setVisible(false)



-- used images
gui.bottom_image = Image('art/intrface/iface.frm')
gui.menu_image = Image('art/intrface/opbase.frm')
gui.big_red_up_image = Image('art/intrface/bigredup.frm')
gui.big_red_dn_image = Image('art/intrface/bigreddn.frm')
gui.satur_up_image = Image('art/intrface/saturbup.frm')
gui.satur_dn_image = Image('art/intrface/saturbdn.frm')
gui.big_red_dn_image = Image('art/intrface/bigreddn.frm')
gui.big_menu_up_image = Image('art/intrface/opbtnoff.frm')
gui.big_menu_dn_image = Image('art/intrface/opbtnon.frm')
gui.inv_up_image = Image('art/intrface/invbutup.frm')
gui.inv_dn_image = Image('art/intrface/invbutdn.frm')
gui.options_up_image = Image('art/intrface/optiup.frm')
gui.options_dn_image = Image('art/intrface/optidn.frm')
gui.pip_up_image = Image('art/intrface/pipup.frm')
gui.pip_dn_image = Image('art/intrface/pipdn.frm')
gui.map_up_image = Image('art/intrface/mapup.frm')
gui.map_dn_image = Image('art/intrface/mapdn.frm')
gui.char_up_image = Image('art/intrface/chaup.frm')
gui.char_dn_image = Image('art/intrface/chadn.frm')
gui.invbox_image = Image('art/intrface/invbox.frm')
gui.skill_image = Image('art/intrface/skldxbox.frm')


gui.cmb_green_image = Image('art/intrface/endltgrn.frm')
gui.cmb_red_image = Image('art/intrface/endltred.frm')

gui.endcmbt_up_image = Image('art/intrface/endcmbtu.frm')
gui.endcmbt_dn_image = Image('art/intrface/endcmbtd.frm')
gui.endturn_up_image = Image('art/intrface/endturnu.frm')
gui.endturn_dn_image = Image('art/intrface/endturnd.frm')


gui.cmb_anim = Image('art/intrface/endanim.frm')
gui.cmb_anim:setAnimActive(false)
gui.cmb_anim:setAnimDirection(true)
gui.cmb_anim:setAnimEndCallback('cmb_btn_anim')


-- background
gui.bottom_bg = Icon(gui.bottom_image)
gui.bottom:add(gui.bottom_bg)
gui.bottom:moveToBottom(gui.bottom_bg)

gui.menu_bg = Icon(gui.menu_image)
gui.menu:add(gui.menu_bg)
gui.menu:moveToBottom(gui.menu_bg)

gui.invbox_bg = Icon(gui.invbox_image)
gui.invbox:add(gui.invbox_bg)
gui.invbox:moveToBottom(gui.invbox_bg)

gui.skill_bg = Icon(gui.skill_image)
gui.skldx:add(gui.skill_bg)
gui.skldx:moveToBottom(gui.skill_bg)

-- the buttons

gui.menu_button_1 = TwoButton(gui.big_menu_up_image, gui.big_menu_dn_image)
gui.menu_button_2 = TwoButton(gui.big_menu_up_image, gui.big_menu_dn_image)
gui.menu_button_3 = TwoButton(gui.big_menu_up_image, gui.big_menu_dn_image)
gui.menu_button_4 = TwoButton(gui.big_menu_up_image, gui.big_menu_dn_image)
gui.menu_button_5 = TwoButton(gui.big_menu_up_image, gui.big_menu_dn_image)
gui.cmbt_button = TwoButton(gui.satur_up_image, gui.satur_dn_image)
gui.red_button_1 = TwoButton(gui.big_red_up_image, gui.big_red_dn_image)
gui.red_button_2 = TwoButton(gui.big_red_up_image, gui.big_red_dn_image)
gui.inventory_button = TwoButton(gui.inv_up_image, gui.inv_dn_image)
gui.options_button = TwoButton(gui.options_up_image, gui.options_dn_image)
gui.pip_button = TwoButton(gui.pip_up_image, gui.pip_dn_image)
gui.map_button = TwoButton(gui.map_up_image, gui.map_dn_image)
gui.character_button = TwoButton(gui.char_up_image, gui.char_dn_image)
gui.endturn_button = TwoButton(gui.endturn_up_image, gui.endturn_dn_image)
gui.endcmbt_button = TwoButton(gui.endcmbt_up_image, gui.endcmbt_dn_image)

gui.combat_anim_holder = Icon(gui.cmb_anim)
gui.combat_bt_holder = Container()
gui.combat_bt_holder:setSize(57, 58)
gui.combat_bt_holder:setBorderSize(0)
gui.combat_bt_holder:setOpaque(false)
gui.combat_bt_holder:setVisible(false)

gui.endturn_button:setPosition(10, 5)
gui.endcmbt_button:setPosition(10, 27)
gui.combat_status = Icon(gui.cmb_red_image)
gui.combat_bt_holder:add(gui.combat_status)
gui.combat_bt_holder:add(gui.endturn_button)
gui.combat_bt_holder:add(gui.endcmbt_button)

gui.red_button_1:setEventId('change_weapon')
gui.red_button_2:setEventId('skill')
gui.inventory_button:setEventId('show_inventory')
gui.options_button:setEventId('show_game_options')
gui.character_button:setEventId('show_character_screen')
gui.map_button:setEventId('show_map')
gui.pip_button:setEventId('show_pipboy')
gui.cmbt_button:setEventId('combat_start')
gui.menu_button_4:setEventId('exit')
gui.menu_button_5:setEventId('show_game_options')

-- positions ; found by trial-and-error
gui.menu_button_1:setPosition(14, 16)
gui.menu_button_2:setPosition(14, 53)
gui.menu_button_3:setPosition(14, 90)
gui.menu_button_4:setPosition(14, 127)
gui.menu_button_5:setPosition(14, 164)
gui.cmbt_button:setPosition(266, 27)
gui.red_button_1:setPosition(218, 6)
gui.red_button_2:setPosition(523, 6)
gui.inventory_button:setPosition(210, 40)
gui.options_button:setPosition(210, 61)
gui.map_button:setPosition(525, 39)
gui.character_button:setPosition(525, 58)
gui.pip_button:setPosition(525, 77)
gui.combat_anim_holder:setPosition(580, 39)
gui.combat_bt_holder:setPosition(580, 39)

-- Labels

gui.fallout_font_text_gamemenu = TTFont('content/fonts/falloutd.ttf', 18)
gui.fallout_font_text_gamemenu:setColor(172,148,34)
gui.game_menu_label_text = {'SAVE GAME','LOAD GAME','OPTIONS','EXIT','DONE'}


gui.menu_button_1:setFont(gui.fallout_font_text_gamemenu)
gui.menu_button_1:setCaption('SAVE GAME')
gui.menu_button_1:setSize(140, 35)
gui.menu_button_2:setFont(gui.fallout_font_text_gamemenu)
gui.menu_button_2:setCaption('LOAD GAME')
gui.menu_button_2:setSize(140, 35)
gui.menu_button_3:setFont(gui.fallout_font_text_gamemenu)
gui.menu_button_3:setCaption('OPTIONS')
gui.menu_button_3:setSize(140, 35)
gui.menu_button_4:setFont(gui.fallout_font_text_gamemenu)
gui.menu_button_4:setCaption('EXIT')
gui.menu_button_4:setSize(140, 35)
gui.menu_button_5:setFont(gui.fallout_font_text_gamemenu)
gui.menu_button_5:setCaption('DONE')
gui.menu_button_5:setSize(140, 35)

-- add declared widgets to the container
gui.menu:add(gui.menu_button_1)
gui.menu:add(gui.menu_button_2)
gui.menu:add(gui.menu_button_3)
gui.menu:add(gui.menu_button_4)
gui.menu:add(gui.menu_button_5)
gui.bottom:add(gui.cmbt_button)
gui.bottom:add(gui.red_button_1)
gui.bottom:add(gui.red_button_2)
gui.bottom:add(gui.inventory_button)
gui.bottom:add(gui.options_button)
gui.bottom:add(gui.pip_button)
gui.bottom:add(gui.map_button)
gui.bottom:add(gui.character_button)
gui.bottom:add(gui.combat_anim_holder)
gui.bottom:add(gui.combat_bt_holder)

-- vim: set noexpandtab: set shiftwidth=2: set tabstop=2: