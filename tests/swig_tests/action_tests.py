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
		elev = fife.Elevation()
		self.layer = elev.addLayer(self.grid)
		
		self.target = fife.Location()
		self.target.layer = self.layer
		self.target.elevation = elev
		self.target.position = fife.Point(19,9)
		
		self.obj = fife.Object("0")
		self.pather = fife.LinearPather()
		self.obj.setPather(self.pather)
		self.action = self.obj.addAction('walk')
		self.action.thisown = 0
		addResource = self.engine.animationPool.addResourceFromFile
		for index, direction in enumerate(dirnames):
			degree = 45 * index
			self.action.addAnimation(degree, addResource(files[index]))

		self.inst = self.layer.addInstance(self.obj, fife.Point(4,4))

	def tearDown(self):
		del self.engine

	def testWalkingAction(self):
		getAnimation = self.engine.animationPool.getAnimation
		self.inst.act('walk', self.target, 0.05)
		self.engine.initializePumping()
		backend = self.engine.renderBackend
		for i in xrange(360):
			if self.inst.position == self.target.position:
				break
			self.inst.update()
			action = self.inst.currentAction
			angle = self.inst.orientation
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
