#!/usr/bin/python

# merged techdemo.py from jasoka / jwt with my own:
# 
# enables
#	* map loading
#	* agent "gunner" on loaded map
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

class MyEventListener(fife.IKeyListener, fife.ICommandListener, fife.IMouseListener, fife.ConsoleExecuter):
	def __init__(self, world):
		self.world = world
		engine = world.engine
		eventmanager = engine.getEventManager()
		eventmanager.setNonConsumableKeys([fife.IKey.ESCAPE, fife.IKey.F10, fife.IKey.F9, fife.IKey.F8, fife.IKey.TAB])
		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		fife.ICommandListener.__init__(self)
		eventmanager.addCommandListener(self)
		fife.IMouseListener.__init__(self)
		eventmanager.addMouseListener(self)
		fife.ConsoleExecuter.__init__(self)
		engine.getGuiManager().getConsole().setConsoleExecuter(self)
		
		self.engine = engine		
		self.quitRequested = False
		self.newTarget = None

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
		if (keyval == fife.IKey.ESCAPE):
			self.quitRequested = True		
		elif (keyval == fife.IKey.F10):
			self.engine.getGuiManager().getConsole().toggleShowHide()
	
	
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

class World(object):
	def __init__(self):
		self.engine = fife.Engine()
		self.reactor = InstanceReactor()
		logman = self.engine.getLogManager()
		self.log = fifelog.LogManager(self.engine)
		#self.log.setVisibleModules('hexgrid')
		self.eventmanager = self.engine.getEventManager()
		
		
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()
		
		glyphs = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" + \
		         ".,!?-+/:();%`'*#=[]"
		font = self.engine.getGuiManager().createFont('content/fonts/FreeMono.ttf', 12, glyphs)
		self.engine.getGuiManager().setGlobalFont(font)
		
	def __del__(self):
		self.engine.getView().removeCamera(self.camera)
		
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
		self.dummyObj = fife.Object("dummy")
		self.dummyObj.setPather(self.pather)
		a = self.dummyObj.addAction('dummy:walk')
		
		path = 'techdemo/animations/agents/gunner/walk/'
		angles = sorted([p for p in os.listdir(path) if re.search(r'\d+', p)])
		for angle in angles:
			animid = self.engine.getAnimationPool().addResourceFromFile(path + angle + '/animation.xml')
			a.addAnimation(int(angle), animid)
		self.dummy = self.layer.addInstance(self.dummyObj, fife.ModelCoordinate(5,0))
		self.dummy.addListener(self.reactor)
		
	def adjust_views(self):
		camloc = fife.Location()
		camloc.setLayer(self.layer)
		camloc.setLayerCoordinates(fife.ModelCoordinate(5,0))
		
		self.camera = fife.Camera()
		self.camera.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)

		self.camera.setRotation(35)
		self.camera.setTilt(60)

		self.camera.setLocation(camloc)
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		self.camera.setViewPort(viewport)
		self.engine.getView().addCamera(self.camera)
	
	def run(self):
		evtlistener = MyEventListener(self)
		self.engine.initializePumping()
		self.target.setLayerCoordinates(fife.ModelCoordinate(1,0))
		self.dummy.act('dummy:walk', self.target, 0.5)

		while True:
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

		self.engine.finalizePumping()


if __name__ == '__main__':
	w = World()
	w.create_world()
	w.create_dummy()
	w.adjust_views()
	w.run()
	

