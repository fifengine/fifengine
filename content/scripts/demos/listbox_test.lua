gui.model = ListModel('Hello', 'World', 'Can you read this?', 'Or not at all?', 'How about this...')
gui.box = ListBox(gui.model)
gui.box:setSize(300, 100)

font = TTFont('content/fonts/FreeMono.ttf', 16)
font:setColor(20, 10, 15)

--gui.box:setBackgroundColor(gui.color_dark)
gui.box:setPosition(30, 30)
gui.box:setFont(font)
guimanager.add(gui.box)

gui.drop = DropDown(gui.model)
gui.drop:setPosition(30, 200)
gui.drop:setFont(font)
guimanager.add(gui.drop)
