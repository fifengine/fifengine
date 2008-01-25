# coding: utf-8
# ApplicationBase is an extendable class that provides a basic environment for a FIFE-based client.
#
#   The unextended application reads in and initializes engine settings, sets up a simple event
# listener, and pumps the engine while listening for a quit message. Specialized applications can
# modify settings.py to change initial engine settings. They can provide their own event listener
# by assigning to self.listenertype in their constructor. And they can override the _pump method
# to define runtime behavior of the application.

"""
Basic Application
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

	def onWidgetAction(self, evt):
		print "Widget action %s" % str(evt.getId())

class ApplicationBase(object):
	def __init__(self):

		# Initialize engine settings from settings.py

		import settings
		engine = fife.Engine()
		self.engine = engine
		log = fifelog.LogManager(engine, settings.LogToPrompt, settings.LogToFile)
		
		engineSetting = engine.getSettings()
		engineSetting.setDefaultFontGlyphs(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" +
				".,!?-+/:();%`'*#=[]")
		engineSetting.setDefaultFontPath('content/fonts/samanata.ttf')
		engineSetting.setDefaultFontSize(12)
		engineSetting.setBitsPerPixel(settings.BitsPerPixel)
		engineSetting.setFullScreen(settings.FullScreen)
		engineSetting.setInitialVolume(settings.InitialVolume)
		engineSetting.setRenderBackend(settings.RenderBackend)
		engineSetting.setSDLRemoveFakeAlpha(settings.SDLRemoveFakeAlpha)
		engineSetting.setScreenWidth(settings.ScreenWidth)
		engineSetting.setScreenHeight(settings.ScreenHeight)
	
		engine.init()

		# Set listener type. This may be overridden by derived classes for specialized listening
		self.listenertype = ExitEventListener
	
		self.quitRequested = False

	def run(self):
		eventlistener = self.listenertype(self)
		self.engine.initializePumping()
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

	# Application pump. Derived classes can specialize this for unique behavior.
	def _pump():
		pass

	def quit(self):
		self.quitRequested = True
