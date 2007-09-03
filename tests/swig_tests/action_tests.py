#!/usr/bin/python
from swig_test_utils import *
import pythonize

class AnimationTests(unittest.TestCase):
	def setUp(self):
		template = 'content/animations/critters/animals/wolf_walk_%s.xml'
		dirnames = ['n','ne','e','se','s','sw','w','nw']
		files = map(lambda dirname: template % dirname, dirnames)

		self.engine = fife.Engine()
		self.grid = fife.SquareGrid(True)
		elev = fife.Elevation("Elevation001")
		self.layer = elev.addLayer("Layer001", self.grid)
		
		self.target = fife.Location()
		self.target.setLayer(self.layer)
		self.target.position = fife.Point(19,9)
		
		self.obj = fife.Object("object001")
		self.pather = fife.LinearPather()
		self.obj.setPather(self.pather)
		self.action = self.obj.addAction('action001', 'walk')
		self.action.thisown = 0
		addResource = self.engine.animationPool.addResourceFromFile
		for index, direction in enumerate(dirnames):
			degree = 45 * index
			self.action.addAnimation(degree, addResource(files[index]))

		self.inst = self.layer.addInstance(self.obj, fife.Point(4,4))

	def tearDown(self):
		del self.engine

	def testWalkingAction(self):
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

TEST_CLASSES = [AnimationTests]

if __name__ == '__main__':
    unittest.main()
