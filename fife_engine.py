#!/usr/bin/python
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
		maxind = (len(self.items) - 1)
		if i > maxind:
			i = maxind
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
			self.quitRequested = True
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
			self.change_map(self.gui.get_selected_level())
		elif event.getId() == 'on_stresstest':
			self.activateStressTesting(True)
		elif event.getId() == 'on_loaderlist_change':
			self.gui.load_level_dropdown()
	
	def updateEvent(self, time):
		if self.stressTestingOn:
			ind = self.stressTestingMapIndex % self.gui.level_list.getNumberOfElements()
			self.stressTestingMapIndex += 1
			self.change_map(self.gui.level_list.getElementAt(ind))

	def change_map(self, map):
		try:
			self.gamestate.deactivate()
			self.gamestate.setMap(map)
			self.gamestate.activate()
			self.gui.show_message("Map %s loaded" % map)
		except fife.CannotOpenFile:
			self.gui.show_message("Selected map file not available")


class Gui(object):
	def __init__(self):
		self.loader_list = GenericListmodel()
		self.level_list = GenericListmodel()
		self.level_drop = None
		self.widgets = []
		self.panel = None

	def configure_widget(self, widget, pos, size, bgcol=None, fgcol=None, basecol=None, font=None, 
		                 evtid=None, adjustsize=False):
		self.widgets.append(widget)
		widget.setPosition(*pos)
		widget.setSize(*size)
		if bgcol: widget.setBackgroundColor(bgcol)
		if fgcol: widget.setForegroundColor(bgcol)
		if basecol: widget.setBaseColor(basecol)
		if font: widget.setFont(font)
		if evtid: 
			widget.setActionEventId(evtid)
			widget.addActionListener(engine.getGuiManager())
		if adjustsize: widget.adjustSize()
	
	def get_selected_level(self):
		return self.level_list.getElementAt(self.level_drop.getSelected())
	
	def load_level_dropdown(self):
		loaderfile = self.loader_list.getElementAt(self.loader_drop.getSelected())
		levelitems = fife.VFSUtility().readLines(genpath('content/etc/%s.txt' % loaderfile))
		self.level_list.items = [i.strip() for i in levelitems]
		
	def show_message(self, msg):
		self.msgarea.setText(msg)	
	
	def create_mainmenu(self):
		small_font = fife.TTFont('content/fonts/FreeMono.ttf', 12)
		small_font.setColor(255, 255, 255)
		self.widgets.append(small_font) # just to keep reference alive
		dark_color = fife.Color(10, 20, 20)
		medium_color = fife.Color(60, 70, 60)
		white = fife.Color(255,255,255,255)
		sx, sy = 360, 400
		
		self.panel = fife.Container()
		self.panel.setOpaque(False)		
		self.configure_widget(self.panel, pos=(2, 2), size=(sx-2, sy-2))
		
		title = fife.Window('Choose a Map')
		title.setTitleBarHeight(20)
		self.configure_widget(title, pos=(0, 0), size=(sx, sy), 
		                      basecol=dark_color, font=small_font)
		title.setEnabled(False)
		
		self.loader_list.items.extend(['maps_custom_players', 'maps_custom_developers', 'maps_fo1', 'maps_fo2'])
		self.loader_drop = fife.DropDown(self.loader_list)
		self.loader_drop.setSelected(0)
		self.configure_widget(self.loader_drop, pos=(5, 45), size=(sx - 20, 16), evtid='on_loaderlist_change',
		                      bgcol=dark_color, basecol=medium_color, font=small_font)
		
		self.level_drop = fife.DropDown(self.level_list)
		self.configure_widget(self.level_drop, pos=(5, 85), size=(sx - 170, 16), 
		                      bgcol=dark_color, basecol=medium_color, font=small_font)
		self.load_level_dropdown()
		self.level_drop.setSelected(0)
		
		loadbtn = fife.Button('Load Map')
		self.configure_widget(loadbtn, pos=(sx - 155, 85), size=(100,16), basecol=medium_color, 
		                      font=small_font, evtid='on_loadmap', adjustsize=True)

		stressbtn = fife.Button('Stress Test')
		self.configure_widget(stressbtn, pos=(sx - 155, 117), size=(100,16), basecol=medium_color, 
		                      font=small_font, evtid='on_stresstest', adjustsize=True)
		
		closebtn = fife.Button('Toggle chooser (F9)')
		self.configure_widget(closebtn, pos=(sx - 155, 149), size=(100,16), basecol=medium_color, 
		                      font=small_font, evtid='close_level_chooser', adjustsize=True)

		self.msgarea = fife.TextBox("")
		self.configure_widget(self.msgarea, pos=(5, sy - 200), size=(sx-10,200), font=small_font,
		                      bgcol=dark_color, fgcol=white)
		
		engine.getGuiManager().add(self.panel)
		for w in (title, self.level_drop, self.loader_drop, loadbtn, stressbtn, closebtn, self.msgarea):
			self.panel.add(w)
		

def main():
	global engine
	engine = fife.Engine()
	for src in ('content/critter.dat', 'content/master.dat', 'content/archives/testmap.zip'):
		fife.VFSUtility().addSource(genpath(src))
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
	
	del controller
	del gamestate
	del gui
	del engine

if __name__ == '__main__':
	main()

