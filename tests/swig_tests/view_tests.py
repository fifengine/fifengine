#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
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
import time

class TestView(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine()
		self.model = self.engine.getModel()
		self.map = self.model.createMap("map001")
		
		self.grid = self.model.getCellGrid("square")
		
		self.imgMgr = self.engine.getImageManager()

		self.obj1 = self.model.createObject('0','test_nspace')
		fife.ObjectVisual.create(self.obj1)
		img1 = self.imgMgr.load('../data/mushroom_007.png')
		self.obj1.get2dGfxVisual().addStaticImage(0, img1.getHandle())
		
		self.obj2 = self.model.createObject('1','test_nspace')
		fife.ObjectVisual.create(self.obj2)
		img2 = self.imgMgr.get('../data/earth_1.png')
		self.obj2.get2dGfxVisual().addStaticImage(0, img2.getHandle())

		self.screen_cell_w = img2.getWidth()
		self.screen_cell_h = img2.getHeight()
		
		self.layer = self.map.createLayer("layer001", self.grid)

		self.camcoord = fife.ExactModelCoordinate(2,0)
		
		print "Total Image Memory Used: " + str(self.imgMgr.getMemoryUsed())
		
	def tearDown(self):
		self.engine.destroy()

	def testCamera(self):
		rb = self.engine.getRenderBackend()
		viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())

		cam = self.map.addCamera("foo", self.layer, viewport )
		cam.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)
		cam.setRotation(45)
		cam.setTilt(40)
		
		cam.setViewPort(viewport)
		
		renderer = fife.InstanceRenderer.getInstance(cam)
		renderer.activateAllLayers(self.map)
		
		self.engine.initializePumping()
		
		for y in xrange(4):
			for x in xrange(4):
				i = self.layer.createInstance(self.obj2, fife.ModelCoordinate(x,y))
				fife.InstanceVisual.create(i)
				self.engine.pump()
				time.sleep(0.01)
		i = self.layer.createInstance(self.obj1, fife.ModelCoordinate(0,0))
		fife.InstanceVisual.create(i)
		i = self.layer.createInstance(self.obj1, fife.ModelCoordinate(2,1))
		fife.InstanceVisual.create(i)
		
		for i in xrange(120):
			if i > 20 and i < 30:
				cam.setRotation(cam.getRotation() + 1)
			elif i > 30 and i < 40:
				cam.setRotation(cam.getRotation() - 1)
			elif i > 40 and i < 50:
				if i % 2 == 0:
					c = cam.getLocation().getExactLayerCoordinates()
					c.x += 0.1
					c = cam.getLocation().setExactLayerCoordinates(c)
			elif i > 50 and i < 60:
				if i % 2 == 0:
					c = cam.getLocation().getExactLayerCoordinates()
					c.x -= 0.1
					c = cam.getLocation().setExactLayerCoordinates(c)
			elif i > 60 and i < 70:
				cam.setTilt(cam.getTilt() + 1)
			elif i > 70 and i < 80:
				cam.setTilt(cam.getTilt() - 1)
			elif i > 80 and i < 90:	
				cam.setZoom(cam.getZoom() + 0.010)
			elif i > 90 and i < 100:	
				cam.setZoom(cam.getZoom() - 0.010)
			self.engine.pump()
		self.engine.finalizePumping()
	
		

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

