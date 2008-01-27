# coding: utf-8
"""
ApplicationBase is an extendable class that provides a basic environment for a FIFE-based client.

 The unextended application reads in and initializes engine settings, sets up a simple event
listener, and pumps the engine while listening for a quit message. Specialized applications can
modify settings.py to change initial engine settings. They can provide their own event listener
by assigning to self.listenertype in their constructor. And they can override the _pump method
to define runtime behavior of the application.

"""

import fife
import fifelog

# Default, rudimentary event listener.
class ExitEventListener(fife.IKeyListener):
	def __init__(self, app):
		self.app = app
		self.engine = app.engine
		eventmanager = self.engine.getEventManager()
		eventmanager.setNonConsumableKeys([fife.IKey.ESCAPE])
		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		self.quitRequested = False

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		if keyval == fife.IKey.ESCAPE:
			self.app.quit()
	
	def keyReleased(self, evt):
		pass

class ApplicationBase(object):
	def __init__(self):
		self.engine = fife.Engine()

		self.loadSettings()
		self.initLogging()
		
		self.engine.init()

		self.quitRequested = False
		self.breakRequested = False
		self.returnValues = []

	def loadSettings(self):
		import settings
		self.settings = settings
		
		engineSetting = self.engine.getSettings()
		engineSetting.setDefaultFontGlyphs(
			" abcdefghijklmnopqrstuvwxyz"
			+ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
			+ ".,!?-+/:();%`'*#=[]")
		engineSetting.setDefaultFontPath('content/fonts/samanata.ttf')
		engineSetting.setDefaultFontSize(12)
		engineSetting.setBitsPerPixel(settings.BitsPerPixel)
		engineSetting.setFullScreen(settings.FullScreen)
		engineSetting.setInitialVolume(settings.InitialVolume)
		engineSetting.setRenderBackend(settings.RenderBackend)
		engineSetting.setSDLRemoveFakeAlpha(settings.SDLRemoveFakeAlpha)
		engineSetting.setScreenWidth(settings.ScreenWidth)
		engineSetting.setScreenHeight(settings.ScreenHeight)
	
	def initLogging(self):
		self.log = fifelog.LogManager(self.engine, self.settings.LogToPrompt, self.settings.LogToFile)
		if self.settings.LogModules:
			self.log.setVisibleModules(*self.settings.LogModules)

	def createListener(self):
		return ExitEventListener(self)

	def run(self):
		eventlistener = self.createListener()
		self.engine.initializePumping()
		return self.mainLoop()
	
	def mainLoop(self):
		self.returnValues.append(None)
		while not self.quitRequested:
			try:
				self.engine.pump()
			except fife.Exception, e:
				print e.getMessage()
				break

			self._pump()

			try:
				self.engine.finalizePumping()
			except fife.Exception, e:
				print e.getMessage()
				break

			if self.breakRequested:
				self.breakRequested = False
				break
		
		return self.returnValues.pop()

	def breakFromMainLoop(self,returnValue):
		self.returnValues[-1] = returnValue
		self.breakRequested = True

	# Application pump. Derived classes can specialize this for unique behavior.
	def _pump(self):
		pass

	def quit(self):
		self.quitRequested = True
