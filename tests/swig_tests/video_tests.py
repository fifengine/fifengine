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
from serializers.xmlanimation import XMLAnimationLoader

class TestVideo(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine()
		
	def tearDown(self):
		self.engine.destroy()

	def testSimpleAnimation(self):
		pool = self.engine.getAnimationPool()
		animationloader = XMLAnimationLoader(self.engine.getImagePool(), self.engine.getVFS())
		pool.addResourceLoader(animationloader)
		
		id = pool.addResourceFromFile('tests/data/wolf_walk/wolf_walk_sw.xml')
		animation = pool.getAnimation(id)
		self.engine.initializePumping()
		backend = self.engine.getRenderBackend()
		gettime = self.engine.getTimeManager().getTime
		starttime = gettime()
		for i in xrange(50):
			image = animation.getFrameByTimestamp(gettime() - starttime)
			if not image:
				image = animation.getFrame(0)
				starttime = gettime()
			#print fife.Rect(0,0,image.getWidth(),image.getHeight())
			image.render(fife.Rect(0,0,image.getWidth(),image.getHeight()), 255)
			self.engine.pump()
		self.engine.finalizePumping()


	def testDrawLine(self):
		points = (
			(1,1), (50,20), (20,50), (200,500), (500,200), (100,200),
			(10,10), (70,40), (80,30), (300,520), (340,220), (170,600),
		)
		self.engine.initializePumping()
		
		be = self.engine.getRenderBackend()
		for i in xrange(20):
			prevPt = fife.Point(*points[1])
			for pt in points[1:]:
				curPt = fife.Point(*pt)
				be.drawLine(prevPt, curPt, 100, 100, 100)
				prevPt = curPt
			self.engine.pump()
		self.engine.finalizePumping()
	

TEST_CLASSES = [TestVideo]

if __name__ == '__main__':
    unittest.main()

