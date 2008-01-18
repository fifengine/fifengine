#!/usr/bin/env python
# coding: utf-8

import sys, os, re

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife
import fifelog
import settings

import pychan

class EventListener(fife.IKeyListener, fife.ICommandListener, fife.IMouseListener, fife.ConsoleExecuter, fife.IWidgetListener):
	def __init__(self, app):
		self.app = app
		self.engine = app.engine
		eventmanager = self.engine.getEventManager()
		eventmanager.setNonConsumableKeys([
			fife.IKey.ESCAPE,
			fife.IKey.F10,
			fife.IKey.F9,
			fife.IKey.F8,
			fife.IKey.TAB,
			fife.IKey.LEFT,
			fife.IKey.RIGHT,
			fife.IKey.UP,
			fife.IKey.DOWN])
		
		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		fife.ICommandListener.__init__(self)
		eventmanager.addCommandListener(self)
		fife.IMouseListener.__init__(self)
		eventmanager.addMouseListener(self)
		fife.ConsoleExecuter.__init__(self)
		self.engine.getGuiManager().getConsole().setConsoleExecuter(self)
		fife.IWidgetListener.__init__(self)
		eventmanager.addWidgetListener(self)
		self.quitRequested = False

	def mousePressed(self, evt):
		pass

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
		if keyval == fife.IKey.ESCAPE:
			self.app.quit()
	
	def keyReleased(self, evt):
		pass

	def onCommand(self, command):
		pass

	def onToolsClick(self):
		print "No tools set up yet"
	
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
	
	def onWidgetAction(self, evt):
		print "Widget action %s" % str(evt.getId())



class Application(object):
	def __init__(self):
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
		eventListener = EventListener(self)
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

if __name__ == '__main__':
	app = Application()
	
	# Pychan specific code is here!
	pychan.init(app.engine)
	gui = pychan.loadXML('content/gui/all_widgets.xml')
	gui.findChild(name='closeButton').capture( app.quit )
	gui.findChild(name='demoList').items += ['PyChan Demo']
	gui.show()
	
	app.run()