# Defines the event listener that is used by the editor. See editor.py.

import fife

class EditorListener(fife.IKeyListener, fife.ICommandListener, fife.IMouseListener, 
	              fife.ConsoleExecuter, fife.IWidgetListener):
	def __init__(self, app):
		self.app = app
		engine = app.engine
		eventmanager = engine.getEventManager()
		eventmanager.setNonConsumableKeys([
			fife.Key.ESCAPE,
			fife.Key.F10,
			fife.Key.F9,
			fife.Key.F8,
			fife.Key.TAB,
			fife.Key.LEFT,
			fife.Key.RIGHT,
			fife.Key.UP,
			fife.Key.DOWN])
		
		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		fife.ICommandListener.__init__(self)
		eventmanager.addCommandListener(self)
		fife.IMouseListener.__init__(self)
		eventmanager.addMouseListener(self)
		fife.ConsoleExecuter.__init__(self)
		engine.getGuiManager().getConsole().setConsoleExecuter(self)
		fife.IWidgetListener.__init__(self)
		eventmanager.addWidgetListener(self)
	
		self.engine = engine		
		self.showTileOutline = True
		self.showEditor = False
		self.showCoordinates = False
		self.showSecondCamera = False
		self.reloadRequested = False

	def mousePressed(self, evt):
		if(evt.getButton() == fife.MouseEvent.RIGHT ):
			print 'right click'
	def mouseReleased(self, evt):
		pass	
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	def mouseWheelMovedUp(self, evt):
		pass	
	def mouseWheelMovedDown(self, evt):
		pass
	def mouseMoved(self, evt):
		pass
	def mouseDragged(self, evt):
		pass

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keyval == fife.Key.ESCAPE:
			self.app.quit()
		elif keyval == fife.Key.F10:
			self.engine.getGuiManager().getConsole().toggleShowHide()
		elif keystr == 'p':
			self.engine.getRenderBackend().captureScreen('screenshot.bmp')
		elif keystr == 't':
			self.showTileOutline = not self.showTileOutline
		elif keystr == 'c':
			self.showCoordinates = not self.showCoordinates
		elif keystr == 's':
			self.showSecondCamera = not self.showSecondCamera
		elif keystr == 'r':
			self.reloadRequested = True
		elif keystr == 'e':
			self.showEditor = True
	
	def keyReleased(self, evt):
		pass

	def onCommand(self, command):
		if command.getCommandType() == fife.CMD_QUIT_GAME:
			self.app.quit()

	def onToolsClick(self):
		print "No tools set up yet"
	
	def onConsoleCommand(self, command):
		result = "no result"
		if command.lower() in ('quit', 'exit'):
			self.app.quit()

		if command.lower() in ( 'help', 'help()' ):
			self.engine.getGuiManager().getConsole().println( open( 'content/infotext.txt', 'r' ).read() )
			return "-- End of help --"
		
		try:
			result = str(eval(command))
		except:
			pass
		return result
	
	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'WidgetEvtQuit':
			cmd = fife.Command()
			cmd.setSource(evt.getSource())
			cmd.setCommandType(fife.CMD_QUIT_GAME)
			self.engine.getEventManager().dispatchCommand( cmd );
		if evtid == 'WidgetEvtAbout':
			if self.showInfo:
				self.showInfo = False
			else:
				self.showInfo = True

