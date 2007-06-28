require('liblua_i18n')

os.setlocale("")
i18n.setenv("LANGUAGE", "de", 1) -- switch this!
i18n.bindtextdomain("messages", "content/locale")

function on_engine_start()
  
  gui.con = Container()
  gui.con:setSize(300, 170)
  gui.con:setPosition(40, 50)
  
  gui.top = Window(gui.con, i18n.gettext('Move me around'))
  
  dark = Color(20, 50, 10)
  gui.top:setBaseColor(dark)
  
  gui.top:setSize(310, 210)
  gui.top:setPosition(5, 5)
  gui.top:setOpaque(false)
  gui.top:setTitleBarHeight(30)
  guimanager.add(gui.top)


  gui.b1 = Button(i18n.gettext('Test inside window'))
  gui.b1:setPosition(30, 40)
  gui.b1:setEventId('FOO_BAR')

  gui.con:add(gui.b1)

  gui.l1 = Label(i18n.gettext('right'))
  gui.l1:setAlignment(2)
  gui.l1:setPosition(0, 70)
  gui.l1:setSize(100, 30)
  gui.con:add(gui.l1)

  gui.l2 = Label(i18n.gettext('left'))
  gui.l2:setAlignment(0)
  gui.l2:setPosition(0, 100)
  gui.l2:setSize(100, 30)
  gui.con:add(gui.l2)

  gui.l3 = Label(i18n.gettext('center'))
  gui.l3:setAlignment(1)
  gui.l3:setPosition(0, 130)
  gui.l3:setSize(100, 30)
  gui.con:add(gui.l3)



end
