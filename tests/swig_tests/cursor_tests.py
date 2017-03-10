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
		
		self.imgMgr = self.engine.getImageManager()

		frame_delay = 100

		#create the animation... messy I know
		self.cursor_anim = fife.SharedAnimationPointer()
		anim = fife.Animation()
		anim.thisown = 0
		self.cursor_anim.reset(anim)		
		
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
			self.cursor_anim.addFrame(img, frame_delay)


		#create the drag animation... messy I know
		self.cursor_drag_anim = fife.SharedAnimationPointer()
		anim = fife.Animation()
		anim.thisown = 0
		self.cursor_drag_anim.reset(anim)	
			
		imgs = []
		imgs.append(self.imgMgr.load('../data/wolf_walk/gfx/run_e_1.png'))
		imgs.append(self.imgMgr.load('../data/wolf_walk/gfx/run_e_2.png'))
		imgs.append(self.imgMgr.load('../data/wolf_walk/gfx/run_e_3.png'))
		imgs.append(self.imgMgr.load('../data/wolf_walk/gfx/run_e_4.png'))
		imgs.append(self.imgMgr.load('../data/wolf_walk/gfx/run_e_5.png'))
		
		for img in imgs:
			self.cursor_drag_anim.addFrame(img, frame_delay)
			
		self.cursor = self.engine.getCursor()
		self.cursor.set(self.cursor_anim)
		self.cursor.setDrag(self.cursor_drag_anim, 50, 50)

		self.engine.initializePumping()
		
	def tearDown(self):
		self.engine.finalizePumping()
		self.engine.destroy()

	def testAnimatedCursor(self):
		for i in xrange(500):
			self.engine.pump()
			
	
		

TEST_CLASSES = [TestView]

if __name__ == '__main__':
    unittest.main()

