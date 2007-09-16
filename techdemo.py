#!/usr/bin/python
import sys, os, re

_paths = ('engine/swigwrappers/python', 'engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(os.path.sep.join(p.split('/')))

import fife, fifelog

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
		
		glyphs = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" + \
		         ".,!?-+/:();%`'*#=[]"
		font = self.engine.getGuiManager().createFont('content/fonts/FreeMono.ttf', 12, glyphs)
		self.engine.getGuiManager().setGlobalFont(font)
		
	def __del__(self):
		self.engine.getView().removeCamera(self.camera)
		
	def create_world(self):
		_map = self.engine.getModel().addMap("map")
		elev = _map.addElevation("elevation1")
		self.squaregrid = fife.SquareGrid(True)
		self.hexgrid = fife.HexGrid()
		self.tilelayer = elev.addLayer("elevation1:ground", self.squaregrid)
		#self.instlayer = elev.addLayer("elevation1:move", self.hexgrid)
		self.instlayer = elev.addLayer("elevation1:move", self.squaregrid)
		self.target = fife.Location()
		self.target.setLayer(self.instlayer)
		self.pather = fife.LinearPather()
		
	def create_stage(self):
		self.ground = fife.Object("ground")
		imgid = self.engine.getImagePool().addResourceFromFile('content/gfx/tiles/ground/earth_1.png')
		self.ground.img = self.engine.getImagePool().getImage(imgid)
		self.ground.addStaticImage(0, imgid)
		for y in xrange(-2,3):
			for x in xrange(-2,3):
				self.tilelayer.addInstance(self.ground, fife.ModelCoordinate(x,y))
	
	def create_dummy(self):
		self.dummyObj = fife.Object("dummy")
		self.dummyObj.setPather(self.pather)
		a = self.dummyObj.addAction('dummy:walk')
		
		path = 'techdemo/animations/agents/dummy/walk/'
		angles = sorted([p for p in os.listdir(path) if re.search(r'\d+', p)])
		for angle in angles:
			animid = self.engine.getAnimationPool().addResourceFromFile(path + angle + '/animation.xml')
			a.addAnimation(int(angle), animid)
		self.dummy = self.instlayer.addInstance(self.dummyObj, fife.ModelCoordinate(0,0))
		self.dummy.addListener(self.reactor)
		
	def adjust_views(self):
		camloc = fife.Location()
		camloc.setLayer(self.tilelayer)
		camloc.setLayerCoordinates(fife.ModelCoordinate(0,0))
		
		self.camera = fife.Camera()
		self.camera.setCellImageDimensions(self.ground.img.getWidth(), self.ground.img.getHeight())
		self.camera.setRotation(45)
		self.camera.setTilt(40)
		self.camera.setLocation(camloc)
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		self.camera.setViewPort(viewport)
		self.engine.getView().addCamera(self.camera)
	
	def run(self):
		evtlistener = MyEventListener(self)
		self.engine.initializePumping()
		self.target.setLayerCoordinates(fife.ModelCoordinate(1,0))
		self.dummy.act('dummy:walk', self.target, 0.3)

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
				
				self.dummy.act('dummy:walk', self.target, 1.0)
				
				evtlistener.newTarget = None
			
			if (evtlistener.quitRequested):
				break

		self.engine.finalizePumping()


if __name__ == '__main__':
	w = World()
	w.create_world()
	w.create_stage()
	w.create_dummy()
	w.adjust_views()
	w.run()
	
