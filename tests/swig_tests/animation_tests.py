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

		frame_delay = 100

		#create the animation... messy I know
		self.anim = fife.SharedAnimationPointer()
		anim = fife.Animation()
		anim.thisown = 0
		self.anim.reset(anim)		
		
		imgs = []
		imgs.append(self.imgMgr.load('../data/crate/full_s_000.png'))
		imgs.append(self.imgMgr.load('../data/crate/full_s_0001.png'))
		imgs.append(self.imgMgr.load('../data/crate/full_s_0002.png'))
		imgs.append(self.imgMgr.load('../data/crate/full_s_0003.png'))
		imgs.append(self.imgMgr.load('../data/crate/full_s_0004.png'))
		imgs.append(self.imgMgr.load('../data/crate/full_s_0005.png'))
		imgs.append(self.imgMgr.load('../data/crate/full_s_0006.png'))
		imgs.append(self.imgMgr.load('../data/crate/full_s_0007.png'))
		imgs.append(self.imgMgr.load('../data/crate/full_s_0008.png'))
		
		for img in imgs:
			self.anim.addFrame(img, frame_delay)		
		
		self.obj = self.model.createObject('0','test_nspace')
		fife.ObjectVisual.create(self.obj)
		img = self.imgMgr.get('../data/earth_1.png')
		self.obj.get2dGfxVisual().addStaticImage(0, img.getHandle())

		self.screen_cell_w = img.getWidth()
		self.screen_cell_h = img.getHeight()
		
		self.layer = self.map.createLayer("layer001", self.grid)

		self.camcoord = fife.ExactModelCoordinate(2,0)
		
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

		genericrenderer = fife.GenericRenderer.getInstance(cam)
		genericrenderer.addActiveLayer(self.layer)
		genericrenderer.setEnabled(True)
		
		self.engine.initializePumping()
		
		for y in xrange(4):
			for x in xrange(4):
				i = self.layer.createInstance(self.obj, fife.ModelCoordinate(x,y))
				fife.InstanceVisual.create(i)
				self.engine.pump()
				time.sleep(0.01)

		i = self.layer.createInstance(self.obj, fife.ModelCoordinate(0,0))
		fife.InstanceVisual.create(i)
		node = fife.RendererNode(i)
		genericrenderer.addAnimation("test", node, self.anim)

		i = self.layer.createInstance(self.obj, fife.ModelCoordinate(2,0))
		fife.InstanceVisual.create(i)
		node = fife.RendererNode(i)
		genericrenderer.addAnimation("test", node, self.anim)
		
		for i in xrange(900):
			self.engine.pump()
		self.engine.finalizePumping()
	
		

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

