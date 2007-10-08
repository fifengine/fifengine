#!/usr/bin/env python

# merged techdemo.py from jasoka / jwt with my own:
# 
# enables
#	* map loading
#	* agent "gunner" on loaded map
#	* map scrolling
#
# changes
#	* agent speed is now 0.5
#	* agent & cam start at 5/0
#
# breaks
#	* nothing (so far)
#
# chewie
import sys, os, re

_paths = ('engine/swigwrappers/python', 'engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(os.path.sep.join(p.split('/')))

import fife, fifelog

sys.path.append("./engine/extensions")
from loaders import *

class InstanceReactor(fife.InstanceListener):
	def OnActionFinished(self, instance, action):
		pass

class MyEventListener(fife.IKeyListener, fife.ICommandListener, fife.IMouseListener, 
	              fife.ConsoleExecuter, fife.IWidgetListener):
	def __init__(self, world):
		self.world = world
		engine = world.engine
		eventmanager = engine.getEventManager()
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
		engine.getGuiManager().getConsole().setConsoleExecuter(self)
		fife.IWidgetListener.__init__(self)
		eventmanager.addWidgetListener(self)
		
		self.engine = engine		
		self.quitRequested = False
		self.newTarget = None
		
		# scroll support
		self.ScrollLeft = False
		self.ScrollRight = False
		self.ScrollUp = False
		self.ScrollDown = False

	def mousePressed(self, evt):
		self.newTarget = fife.ScreenPoint(evt.getX(), evt.getY())

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
		if (keyval == fife.IKey.ESCAPE):
			self.quitRequested = True		
		elif (keyval == fife.IKey.F10):
			self.engine.getGuiManager().getConsole().toggleShowHide()
		elif (keyval == fife.IKey.LEFT):
			#print "camera: left"
			self.ScrollLeft = True
		elif (keyval == fife.IKey.RIGHT):
			#print "camera: right"
			self.ScrollRight = True
		elif (keyval == fife.IKey.UP):
			#print "camera: up"
			self.ScrollUp = True
		elif (keyval == fife.IKey.DOWN):
			#print "camera: down"
			self.ScrollDown = True
		elif (keystr == 'p'):
			self.engine.getRenderBackend().captureScreen('techdemo.bmp')
	
	def keyReleased(self, evt):
		pass

	def onCommand(self, command):
		self.quitRequested = (command.getCommandType() == fife.CMD_QUIT_GAME)

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
		evtid = evt.getId()
		if evtid == 'WidgetEvtQuit':
			self.quitRequested = True
		if evtid == 'WidgetEvtAbout':
			print "This is the zero & fife 2007.2 techdemo"

class World(object):
	def __init__(self):
		self.engine = fife.Engine()
		self.reactor = InstanceReactor()
		logman = self.engine.getLogManager()
		self.log = fifelog.LogManager(self.engine, promptlog=True, filelog=False)
		#self.log.setVisibleModules('hexgrid')

		self.eventmanager = self.engine.getEventManager()
		self.renderbackend = self.engine.getRenderBackend()	
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()
		self.camera = None
		

		
	def __del__(self):
		self.engine.getView().removeCamera(self.camera)

	def gui(self):
		self.guimanager = self.engine.getGuiManager()
		glyphs = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" + \
		         ".,!?-+/:();%`'*#=[]"
		font = self.guimanager.createFont('techdemo/fonts/samanata.ttf', 12, glyphs)

		self.guimanager.setGlobalFont(font)
		#font.setColor(255,20,20)
		
		self.container = fife.Container()
		self.container.setOpaque(True)
		self.guimanager.add(self.container)
		self.label1 = fife.Label('FIFE 2007.2 techdemo')
		self.label1.setPosition(1, 0)
		self.label1.setFont(font)
		self.container.add(self.label1)
		self.container.setSize(self.label1.getWidth() + 2, self.label1.getHeight() + 2)
		self.container.setPosition(2,2)
		
		self.container2 = fife.Container()
		self.container2.setOpaque(True)
		self.guimanager.add(self.container2)
		self.button1 = fife.Button('Quit')
		self.button1.setActionEventId('WidgetEvtQuit')
		self.button1.addActionListener(self.engine.getGuiManager())
		self.button1.adjustSize()
		self.button1.setPosition(1, 0)
		self.button1.setFont(font)
		self.button2 = fife.Button('?')
		self.button2.setActionEventId('WidgetEvtAbout')
		self.button2.addActionListener(self.engine.getGuiManager())
		self.button2.setPosition(self.button1.getWidth() + 10, 0)
		self.button2.setFont(font)
		self.container2.add(self.button1)
		self.container2.add(self.button2)		
		self.container2.setSize(self.button1.getWidth() + self.button2.getWidth() + 10, self.button1.getHeight())
		self.container2.setPosition(1,28)


	def create_world(self):
		loadMapFile("techdemo/maps/city1.xml", self.engine)
	
		self.map = self.model.getMapsByString("id", "TechdemoMap")[0]
		self.elevation = self.map.getElevationsByString("id", "TechdemoMapElevation")[0]
		self.layer = self.elevation.getLayersByString("id", "TechdemoMapTileLayer")[0]
		
		img = self.engine.getImagePool().getImage(self.layer.getInstances()[0].getObject().getStaticImageIndexByAngle(0))
		self.screen_cell_w = img.getWidth()
		self.screen_cell_h = img.getHeight()
		
		self.target = fife.Location()
		self.target.setLayer(self.layer)
		self.pather = fife.LinearPather()
		
	def create_dummy(self):
		# replace this method with something like: metamodel.getObejctsByString('id', 'agent_gunner')
		
		self.dummyObj = fife.Object("dummy")
		self.dummyObj.setPather(self.pather)
		a = self.dummyObj.addAction('dummy:walk')
		
		path = 'techdemo/animations/agents/gunner/walk/'
		angles = sorted([p for p in os.listdir(path) if re.search(r'\d+', p)])
		for angle in angles:
			animid = self.engine.getAnimationPool().addResourceFromFile(path + angle + '/animation.xml')
			a.addAnimation(int(angle), animid)
		self.dummy = self.layer.addInstance(self.dummyObj, fife.ModelCoordinate(2,8))
		self.dummy.addListener(self.reactor)
		
	def adjust_views(self):
		self.camloc = fife.Location()
		self.camloc.setLayer(self.layer)
		self.camloc.setLayerCoordinates(fife.ModelCoordinate(6,0))
		
		self.camera = fife.Camera()
		self.camera.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)

		self.camera.setRotation(35)
		self.camera.setTilt(60)

		self.camera.setLocation(self.camloc)
		viewport = fife.Rect(0, 0, self.renderbackend.getScreenWidth(), self.renderbackend.getScreenHeight())
		self.camera.setViewPort(viewport)
		self.engine.getView().addCamera(self.camera)
			
	def run(self):
		evtlistener = MyEventListener(self)
		self.engine.initializePumping()
		
		# no movement at start
		self.target.setLayerCoordinates(fife.ModelCoordinate(2,8))
		self.dummy.act('dummy:walk', self.target, 0.5)
		
		# map scrolling
		scroll_modifier = 0.1

		while True:
			cam_scroll = self.camloc.getExactLayerCoordinates()
			
			self.engine.pump()
			if (evtlistener.newTarget):
				
				print "===================================================="
				print "screen coordinates = " + str(evtlistener.newTarget)
				
				ec = self.camera.toElevationCoordinates(evtlistener.newTarget)
				print "elevation coordinates = " + str(ec)
				
				print "back to screen = " + str(self.camera.toScreenCoordinates(ec))
				
				self.target.setElevationCoordinates(ec)
				print "layer coordinates = " + str(self.target.getLayerCoordinates())
				
				self.dummy.act('dummy:walk', self.target, 0.5)
				
				evtlistener.newTarget = None
			
			if (evtlistener.quitRequested):
				break

			# scroll the map with cursor keys
			if (evtlistener.ScrollLeft):
				cam_scroll.x -= scroll_modifier
				evtlistener.ScrollLeft = False
			elif (evtlistener.ScrollRight):
				cam_scroll.x += scroll_modifier
				evtlistener.ScrollRight = False
			elif (evtlistener.ScrollUp):
				cam_scroll.y -= scroll_modifier
				evtlistener.ScrollUp = False
			elif (evtlistener.ScrollDown):
				cam_scroll.y += scroll_modifier
				evtlistener.ScrollDown = False				
				
			cam_scroll = self.camloc.setExactLayerCoordinates(cam_scroll)			
			self.camera.setLocation(self.camloc)

		self.engine.finalizePumping()


if __name__ == '__main__':
	w = World()
	w.gui()
	w.create_world()
	w.create_dummy()
	w.adjust_views()
	w.run()
	

