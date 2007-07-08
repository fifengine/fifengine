import os, time
os.environ['PYTHONPATH'] = '.'

import engine.fife as fife
import viewgamestate as vgs

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

STRESS_TEST_PERIOD = 500
class Controller(fife.IKeyListener, fife.ICommandListener, fife.ConsoleExecuter, fife.IWidgetListener, fife.TimeEvent):
	def __init__(self, gui, gamestate):
		eventmanager = engine.getEventManager()
		eventmanager.setNonConsumableKeys([fife.IKey.ESCAPE, fife.IKey.F10, fife.IKey.F9])
		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		fife.ICommandListener.__init__(self)
		eventmanager.addCommandListener(self)
		fife.IWidgetListener.__init__(self)
		engine.getEventManager().addWidgetListener(self)
		fife.ConsoleExecuter.__init__(self)
		engine.getGuiManager().getConsole().setConsoleExecuter(self)
		fife.ConsoleExecuter.__init__(self)
		fife.TimeEvent.__init__(self)
		engine.getTimeManager().registerEvent(self)
		
		self.quitRequested = False
		self.stressTestingOn = False
		self.gui = gui
		self.gamestate = gamestate
		self.stressTestingMapIndex = 0
		
		eventmanager.addMouseListener(gamestate)
		eventmanager.addKeyListener(gamestate)

	def keyPressed(self, event):
		keyval = event.getKey().getValue()
		if (keyval == fife.IKey.ESCAPE):
			self.quitRequested = true
		elif (keyval == fife.IKey.F10):
			engine.getGuiManager().getConsole().toggleShowHide()
		elif (keyval == fife.IKey.F9):
			self.gui.panel.setVisible(not self.gui.panel.isVisible())
			
	def keyReleased(self, event):
		pass

	def onCommand(self, command):
		if command.getCommandType() == fife.CMD_QUIT_GAME:
			self.quitRequested = True

	def onToolsClick(self):
		self.gui.panel.setVisible(True)

	def onConsoleCommand(self, command):
		result = "no result"
		if command.lower() in ('quit', 'exit'):
			self.quitRequested = True
			return "quitting"
		
		try:
			result = str(eval(command))
		except:
			pass
		return result

	def activateStressTesting(self, activate):
		if activate:
			self.stressTestingMapIndex = 0
			self.setPeriod(STRESS_TEST_PERIOD)
		else:
			self.setPeriod(-1)
		self.stressTestingOn = activate

	
	def onWidgetAction(self, event):
		if event.getId() == 'close_level_chooser':
			self.gui.panel.setVisible(False)
		elif event.getId() == 'on_loadmap':
			self.activateStressTesting(False)
			self.gamestate.deactivate()
			self.gamestate.setMap(self.gui.get_selected_map())
			self.gamestate.activate()
		elif event.getId() == 'on_stresstest':
			self.activateStressTesting(True)
	
	def updateEvent(self, time):
		if self.stressTestingOn:
			self.gamestate.deactivate()
			ind = self.stressTestingMapIndex % self.gui.level_list.getNumberOfElements()
			self.stressTestingMapIndex += 1
			self.gamestate.setMap(self.gui.level_list.getElementAt(ind))
			self.gamestate.activate()



class Gui(object):
	def __init__(self):
		self.level_list = GenericListmodel()
		self.level_drop = None
		self.widgets = []
		self.panel = None

	# used to keep widget references alive
	def newwidget(self, widget):
		self.widgets.append(widget)
		return widget
	
	def get_selected_map(self):
		return self.level_list.getElementAt(self.level_drop.getSelected())
	
	def create_mainmenu(self):
		guiman = engine.getGuiManager()
		nw = self.newwidget
		small_font = nw(fife.TTFont('content/fonts/FreeMono.ttf', 12))
		small_font.setColor(255, 255, 255)
		sx = 330
		sy = 400
		listitems = fife.VFSUtility().readLines(genpath('content/etc/maps_custom_players.txt'))
		self.level_list.items = [i.strip() for i in listitems]
		self.level_drop = nw(fife.DropDown(self.level_list))
		self.level_drop.setPosition(5, 45)
		self.level_drop.setSize(sx - 140, 16)
		dark_color = fife.Color(10, 20, 20)
		medium_color = fife.Color(60, 70, 60)
		self.level_drop.setBackgroundColor(dark_color)
		self.level_drop.setBaseColor(medium_color)
		self.level_drop.setFont(small_font)
		
		self.panel = nw(fife.Container())
		self.panel.setSize(sx-2, sy-2)
		self.panel.setPosition(2, 2)
		top = nw(fife.Window('Choose a custom Map'))
		self.panel.add(top)
		top.setFont(small_font)
		top.setBaseColor(dark_color)
		
		top.setSize(sx, sy)
		top.setPosition(0, 0)
		top.setTitleBarHeight(20)
		top.setEnabled(False)
		guiman.add(self.panel)
		self.lchooser_active = True
	
		self.panel.setOpaque(False)
		self.panel.add(self.level_drop)

		button = nw(fife.Button('Load Map'))
		button.setPosition(sx - 125, 45)
		button.setBaseColor(medium_color)
		button.setActionEventId('on_loadmap')
		button.addActionListener(guiman)
		button.setSize(100,16)
		button.setFont(small_font)
		button.adjustSize()
		self.panel.add(button)
		
		button2 = nw(fife.Button('Stress test'))
		button2.setPosition(sx - 125, 77)
		button2.setBaseColor(medium_color)
		button2.setActionEventId('on_stresstest')
		button2.addActionListener(guiman)
		button2.setSize(100,16)
		button2.setFont(small_font)
		button2.adjustSize()
		self.panel.add(button2)
		
		close_button = nw(fife.Button('close chooser'))
		close_button.setBaseColor(medium_color)
		close_button.setPosition(sx - 125, 109)
		close_button.setSize(100,16)
		close_button.setFont(small_font)
		close_button.adjustSize()
		close_button.setActionEventId('close_level_chooser')
		close_button.addActionListener(guiman)
		self.panel.add(close_button)
		

def main():
	global engine
	engine = fife.Engine()
	engine.getAudioManager().setAmbientSound(genpath('content/audio/music/maybe.ogg'))
	gui = Gui()
	gui.create_mainmenu()
	gamestate = vgs.ViewGameState()
	controller = Controller(gui, gamestate)

	engine.initializePumping()
	while True:
		engine.pump()
		gamestate.turn()
		if controller.quitRequested:
			gamestate.deactivate()
			break
	engine.finalizePumping()

if __name__ == '__main__':
	main()

