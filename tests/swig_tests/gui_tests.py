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

class TestGui(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine()
		self.guimanager = self.engine.getGuiManager()
		self.renderbackend = self.engine.getRenderBackend()

	def tearDown(self):
		self.engine.destroy()
	
	def testFonts(self):
		ttffont = fife.TTFont('../data/FreeMono.ttf', 14)
		ttffont.thisown = 0
		subimagefont = fife.SubImageFont('../data/rpgfont.png', 
			       ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"')
		subimagefont.thisown = 0
		fonts = [fife.GuiFont(ttffont), fife.GuiFont(subimagefont)]
		for f in fonts:
			f.setColor(255,20,20)
		container = fife.Container()
		self.guimanager.add(container)
		container.setSize(self.renderbackend.getWidth(), 
		                  self.renderbackend.getHeight())
		container.setOpaque(False)
		label1 = fife.Label('This is a Truetype font')
		label1.setPosition(0, 70)
		label1.setFont(fonts[0])
		container.add(label1)
		label2 = fife.Label('This is an Image font')
		label2.setPosition(0, 100)
		label2.setFont(fonts[1])
		container.add(label2)
		labels = [label1, label2]

		self.engine.initializePumping()
		for i in xrange(100):
			self.engine.pump()
		self.engine.finalizePumping()


TEST_CLASSES = [TestGui]

if __name__ == '__main__':
    unittest.main()

