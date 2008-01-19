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

# Pychan specific code is here!
class PyChanExample(object):
	def __init__(self,xmlFile):
		self.xmlFile = xmlFile
		self.widget = None
	
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		eventMap = {
			'closeButton':self.stop,
			'okButton'   :self.stop
		}
		self.widget.mapEvents(eventMap, ignoreMissing = True)
		self.widget.show()

	def stop(self):
		if self.widget:
			self.widget.hide()
		self.widget = None

class DemoApplication(Application):
	def __init__(self):
		super(DemoApplication,self).__init__()
		
		pychan.init(self.engine,debug=True)
		self.gui = pychan.loadXML('content/gui/all_widgets.xml')
		
		eventMap = {
			'creditsLink'  : self.showCredits,
			'closeButton'  : self.quit,
			'selectButton' : self.selectExample
		}
		self.gui.mapEvents(eventMap)

		from mapwidgets import MapProperties
		from styling import StylingExample
		
		self.examples = {
			'Load Map' : PyChanExample('content/gui/loadmap.xml'),
			'Map Properties' : MapProperties(),
			'Absolute' : PyChanExample('content/gui/absolute.xml'),
			'Styling' : StylingExample()
		}
		self.demoList = self.gui.findChild(name='demoList')
		self.demoList.items += self.examples.keys()
		self.gui.show()
		
		self.currentExample = None
		self.creditsWidget = None

	def selectExample(self):
		if self.demoList.selected_item is None: return
		print "selected",self.demoList.selected_item
		if self.currentExample: self.currentExample.stop()
		self.currentExample = self.examples[self.demoList.selected_item]
		self.gui.findChild(name="xmlSource").text = open(self.currentExample.xmlFile).read()
		self.currentExample.start()
	def showCredits(self):
		if self.creditsWidget:
			self.creditsWidget.show()
			return
		self.creditsWidget = pychan.loadXML('content/gui/credits.xml')
		self.creditsWidget.mapEvents({ 'okButton' : self.creditsWidget.hide })
		self.creditsWidget.distributeData({ 'creditText' : open("../../doc/AUTHORS").read() })
		self.creditsWidget.show()

if __name__ == '__main__':
	app = DemoApplication()
	app.run()