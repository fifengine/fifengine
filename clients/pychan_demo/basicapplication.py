# coding: utf-8

"""
Basic Application
"""

import fife
import fifelog

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
		self.quitRequested = False

	def run(self):
		eventListener = ExitEventListener(self)
		self.engine.initializePumping()
		while not self.quitRequested:
			try:
				self.engine.pump()
			except fife.Exception, e:
				print e.getMessage()
				break
			try:
				self.engine.finalizePumping()
			except fife.Exception, e:
				print e.getMessage()
				break

	def quit(self):
		self.quitRequested = True
