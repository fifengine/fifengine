#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from swig_test_utils import *
import pythonize
from serializers.xmlanimation import XMLAnimationLoader

class ActionTests(unittest.TestCase):
	def setUp(self):
		template = 'tests/data/wolf_walk/wolf_walk_%s.xml'
		dirnames = ['e', 'ne', 'n', 'nw', 'w', 'sw', 's', 'se']
		files = map(lambda dirname: template % dirname, dirnames)

		self.engine = getEngine()
		pool = self.engine.getAnimationPool()
		self.animationloader = XMLAnimationLoader(self.engine.getImagePool(), self.engine.getVFS())
		pool.addResourceLoader(self.animationloader)
		
		_map = self.engine.getModel().createMap("map001")
		
		self.grid = fife.SquareGrid(True)
		self.layer = _map.createLayer("Layer001", self.grid)
		
		self.target = fife.Location(self.layer)
	
		self.obj = fife.Object("object001", 'plaa')
		fife.ObjectVisual.create(self.obj)
#		self.pather = fife.LinearPather()
                self.pather = fife.RoutePather()
		self.obj.setPather(self.pather)
		self.action = self.obj.createAction('walk')
		fife.ActionVisual.create(self.action)
		addResource = self.engine.animationPool.addResourceFromFile
		for index, direction in enumerate(dirnames):
			degree = 45 * index
			self.action.get2dGfxVisual().addAnimation(degree, addResource(files[index]))

		self.ground = fife.Object("ground", 'plaa')
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
		self.engine.destroy()

	def _testWalkingAction(self):
		print 'test1'
		getAnimation = self.engine.animationPool.getAnimation
		print 'test2'
		self.inst.move('walk', self.target, 0.05)
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
		viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())
		
		cam = self.engine.getView().addCamera("foo", self.layer, viewport, fife.ExactModelCoordinate(0,0) )
		cam.setCellImageDimensions(self.ground.img.getWidth(), self.ground.img.getHeight())
		cam.setRotation(45)
		cam.setTilt(40)

		self.engine.getView().resetRenderers()
		self.engine.initializePumping()
		self.target.setLayerCoordinates(fife.ModelCoordinate(2,-2))	
		self.inst.move('walk', self.target, 0.9)
		targets = (
			(2,0), (2,-1), (2,-2), (1,-2),
			(0,-2), (-1,-2), (-2,-2), (-2,-1),
			(-2,0), (-2,1), (-2,2), (-1,2),
			(0,2), (1,2), (2,2), (2,1))
		for target in targets:
			l = self.inst.getLocation()
			l.setLayerCoordinates(fife.ModelCoordinate(0,0))
			self.inst.setLocation(l)
			self.target.setLayerCoordinates(fife.ModelCoordinate(*target))
			self.inst.move('walk', self.target, 0.9)
			for i in xrange(10):
				self.engine.pump()

		self.engine.finalizePumping()
		self.engine.getView().removeCamera(cam)

		

TEST_CLASSES = [ActionTests]

if __name__ == '__main__':
    unittest.main()
