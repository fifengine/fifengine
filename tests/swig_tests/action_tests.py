#!/usr/bin/env python
from swig_test_utils import *
import pythonize

class ActionTests(unittest.TestCase):
	def setUp(self):
		template = 'tests/data/wolf_walk/wolf_walk_%s.xml'
		dirnames = ['e', 'ne', 'n', 'nw', 'w', 'sw', 's', 'se']
		files = map(lambda dirname: template % dirname, dirnames)

		self.engine = getEngine()
		_map = self.engine.getModel().createMap("map001")
		elev = _map.createElevation("elevation001")
		
		self.grid = fife.SquareGrid(True)
		self.layer = elev.createLayer("Layer001", self.grid)
		
		self.target = fife.Location()
		self.target.setLayer(self.layer)
		
		self.obj = fife.Object("object001")
		fife.ObjectVisual.create(self.obj)
		self.pather = fife.LinearPather()
		self.obj.setPather(self.pather)
		self.action = self.obj.createAction('walk')
		fife.ActionVisual.create(self.action)
		addResource = self.engine.animationPool.addResourceFromFile
		for index, direction in enumerate(dirnames):
			degree = 45 * index
			self.action.get2dGfxVisual().addAnimation(degree, addResource(files[index]))

		self.ground = fife.Object("ground")
		imgid = self.engine.imagePool.addResourceFromFile('tests/data/earth_1.png')
		fife.ObjectVisual.create(self.ground)
		self.ground.get2dGfxVisual().addStaticImage(0, imgid)		
		self.ground.img = self.engine.getImagePool().getImage(imgid)
		for y in xrange(-2,3):
			for x in xrange(-2,3):
				inst = self.layer.createInstance(self.ground, fife.ModelCoordinate(x,y))
				fife.InstanceVisual.create(inst)
		self.inst = self.layer.createInstance(self.obj, fife.ModelCoordinate(-2,-2))
		fife.InstanceVisual.create(self.inst)
			
	def tearDown(self):
		del self.engine

	def _testWalkingAction(self):
		print 'test1'
		getAnimation = self.engine.animationPool.getAnimation
		print 'test2'
		self.inst.act('walk', self.target, 0.05)
		self.engine.initializePumping()
		backend = self.engine.renderBackend
		for i in xrange(360):
			self.inst.getLocation().getLayerCoordinates()
			self.target.getLayerCoordinates()
			if self.inst.getLocation().getLayerCoordinates() == self.target.getLayerCoordinates():
				break
			self.inst.update()
			action = self.inst.currentAction
			angle = 0 #self.inst.orientation
			animation = getAnimation( action.getAnimationIndexByAngle(angle) )
			timestamp = self.inst.actionRuntime % animation.duration
			image = animation.getFrameByTimestamp( timestamp )
			if image:
				image.render(fife.Rect(0,0,image.width,image.height),255)
			self.engine.pump()
		self.engine.finalizePumping()

	def testWalkAround(self):

		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight())
		
		cam = self.engine.getView().addCamera("foo", self.layer, viewport, fife.ExactModelCoordinate(0,0) )
		cam.setCellImageDimensions(self.ground.img.getWidth(), self.ground.img.getHeight())
		cam.setRotation(45)
		cam.setTilt(40)
		
		self.engine.getView().resetRenderers()
		
		self.engine.initializePumping()

		self.target.setLayerCoordinates(fife.ModelCoordinate(2,-2))	
		self.inst.act('walk', self.target, 0.9)
		targets = (
			(2,0), (2,-1), (2,-2), (1,-2),
			(0,-2), (-1,-2), (-2,-2), (-2,-1),
			(-2,0), (-2,1), (-2,2), (-1,2),
			(0,2), (1,2), (2,2), (2,1))
		for target in targets:
			print "============> in 0,0"
			l = self.inst.getLocation()
			l.setLayerCoordinates(fife.ModelCoordinate(0,0))
			self.inst.setLocation(l)
			self.target.setLayerCoordinates(fife.ModelCoordinate(*target))
			self.inst.act('walk', self.target, 0.9)
			for i in xrange(10):
				self.engine.pump()
		
		self.engine.finalizePumping()
		self.engine.getView().removeCamera(cam)

		

TEST_CLASSES = [ActionTests]

if __name__ == '__main__':
    unittest.main()
