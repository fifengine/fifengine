import os
os.environ['PYTHONPATH'] = '.'

import engine.fife as fife
import time, os

engine = None

def genpath(path):
	return os.path.sep.join(path.split('/'))

class GenericListmodel(fife.ListModel):
	def __init__(self):
		fife.ListModel.__init__(self)
		self.items = []
		
	def getNumberOfElements(self):
		return len(self.items)
		
	def getElementAt(self, i):
		return self.items[i]

class ConsoleExecuter(fife.ConsoleExecuter):
	def __init__(self):
		fife.ConsoleExecuter.__init__(self)
		self.quitRequested = False
		
	def onToolsClick(self):
		print "In python, tools clicked"

	def onCommand(self, command):
		result = "no result"
		if command.lower() in ('quit', 'exit'):
			self.quitRequested = True
			return "quitting"
		
		try:
			result = str(eval(command))
		except:
			pass
		return result

class Gui(fife.IWidgetListener):
	def __init__(self):
		fife.IWidgetListener.__init__(self)
		engine.getEventManager().addWidgetListener(self)
		self.level_list = GenericListmodel()

	def onWidgetAction(self, event):
		print event.getId()
	
	def create_mainmenu(self):
		small_font = fife.TTFont('content/fonts/FreeMono.ttf', 12)
		small_font.thisown = 0
		small_font.setColor(255, 255, 255)
		sx = 330
		sy = 400
		listitems = fife.VFSUtility().readLines(genpath('content/etc/maps_custom_developers.txt'))
		self.level_list.items = [i.strip() for i in listitems]
		level_drop = fife.DropDown(self.level_list)
		level_drop.setPosition(5, 45)
		level_drop.setSize(sx - 140, 16)
		level_drop.thisown = 0
		dark_color = fife.Color(10, 20, 20)
		medium_color = fife.Color(60, 70, 60)
		level_drop.setBackgroundColor(dark_color)
		level_drop.setBaseColor(medium_color)
		level_drop.setFont(small_font)
		
		con = fife.Container()
		con.thisown = 0
		con.setSize(sx-2, sy-2)
		con.setPosition(2, 2)
		top = fife.Window('Choose a custom Map')
		top.thisown = 0
		con.add(top)
		top.setFont(small_font)
		top.setBaseColor(dark_color)
		
		top.setSize(sx, sy)
		top.setPosition(0, 0)
		top.setTitleBarHeight(20)
		engine.getGuiManager().add(con)
		self.lchooser_active = True
	
		con.setOpaque(False)
		con.add(level_drop)

		button = fife.Button('Load Map')
		button.thisown = 0
		button.setPosition(sx - 125, 45)
		button.setBaseColor(medium_color)
		button.setActionEventId('on_loadmap')
		button.setSize(100,16)
		button.setFont(small_font)
		button.adjustSize()
		con.add(button)
		
		button2 = fife.Button('Stress test')
		button2.thisown = 0
		button2.setPosition(sx - 125, 77)
		button2.setBaseColor(medium_color)
		button2.setActionEventId('on_loadmap')
		button2.setSize(100,16)
		button2.setFont(small_font)
		button2.adjustSize()
		con.add(button2)
		
		close_button = fife.Button('close chooser')
		close_button.thisown = 0
		close_button.setBaseColor(medium_color)
		close_button.setPosition(sx - 125, 109)
		close_button.setSize(100,16)
		close_button.setFont(small_font)
		close_button.adjustSize()
		close_button.setActionEventId('close_level_chooser')
		con.add(close_button)

def main():
	global engine
	engine = fife.Engine()
	engine.getAudioManager().setAmbientSound(genpath('content/audio/music/maybe.ogg'))
	consoleexec = ConsoleExecuter()
	engine.getGuiManager().getConsole().setConsoleExecuter(consoleexec)
	gui = Gui()
	gui.create_mainmenu()
	ctrl = fife.Control()
	ctrl.leaseMapLoader(fife.XML())
	while True:
		engine.pump()
		if consoleexec.quitRequested:
			break

if __name__ == '__main__':
	main()

