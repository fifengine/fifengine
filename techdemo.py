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

class MyMouseListener(fife.IMouseListener):
	def __init__(self):
		fife.IMouseListener.__init__(self)
		self.target = fife.Point(0,0)
		self.update = False	
		self.pressed = False

	def mousePressed(self, evt):
		self.update = False
		self.pressed = True
		self.target = fife.Point(evt.getX(), evt.getY())

	def mouseReleased(self, evt):
		self.pressed = False
		self.update = True

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

class MyKeyListener(fife.IKeyListener):
	def __init__(self):
		fife.IKeyListener.__init__(self)
		self.quitRequested = False
		
	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		if keyval == fife.IKey.ESCAPE:
			self.quitRequested = True
	
	def keyReleased(self, evt):
		pass


class World(object):
	def __init__(self):
		self.engine = fife.Engine()
		self.reactor = InstanceReactor()
		logman = self.engine.getLogManager()
		self.log = fifelog.LogManager(self.engine)
		#self.log.setVisibleModules('hexgrid')
		self.eventmanager = self.engine.getEventManager()
		
	def __del__(self):
		self.engine.getView().removeCamera(self.camera)
		
	def create_world(self):
		_map = self.engine.getModel().addMap("map")
		elev = _map.addElevation("elevation1")
		self.squaregrid = fife.SquareGrid(True)
		self.hexgrid = fife.HexGrid()
		self.tilelayer = elev.addLayer("elevation1:ground", self.squaregrid)
		self.instlayer = elev.addLayer("elevation1:move", self.hexgrid)
		#self.instlayer = elev.addLayer("elevation1:move", self.squaregrid)
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
				self.tilelayer.addInstance(self.ground, fife.Point(x,y))
	
	def create_dummy(self):
		self.dummyObj = fife.Object("dummy")
		self.dummyObj.setPather(self.pather)
		a = self.dummyObj.addAction('dummy:walk')
		
		path = 'techdemo/animations/agents/dummy/walk/'
		angles = sorted([p for p in os.listdir(path) if re.search(r'\d+', p)])
		for angle in angles:
			animid = self.engine.getAnimationPool().addResourceFromFile(path + angle + '/animation.xml')
			a.addAnimation(int(angle), animid)
		self.dummy = self.instlayer.addInstance(self.dummyObj, fife.Point(0,0))
		self.dummy.addListener(self.reactor)
		
	def adjust_views(self):
		camloc = fife.Location()
		camloc.setLayer(self.tilelayer)
		camloc.setLayerCoordinates(fife.Point(0,0))
		
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
		l = MyMouseListener()
		k = MyKeyListener()
		self.eventmanager.addMouseListener(l)
		self.eventmanager.addKeyListener(k)
		self.engine.initializePumping()
		self.target.setLayerCoordinates(fife.Point(1,0))
		self.dummy.act('dummy:walk', self.target, 0.3)

		while True:
			self.engine.pump()
			if (l.update == True):
				l.update = False
				self.target.setElevationCoordinates(self.camera.toElevationCoordinates(l.target))
				#print self.target.getLayerCoordinates()
				self.dummy.act('dummy:walk', self.target, 1.0)
			if (k.quitRequested):
				break

		self.engine.finalizePumping()
		self.eventmanager.removeMouseListener(l)


if __name__ == '__main__':
	w = World()
	w.create_world()
	w.create_stage()
	w.create_dummy()
	w.adjust_views()
	w.run()
	
