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
import time, fifelog

class TestAudio(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine(True)
		self.soundmanager = self.engine.getSoundManager()
		self.log = fifelog.LogManager(self.engine, promptlog=True, filelog=False)
		self.log.setVisibleModules('pool', 'audio')
		self.soundmanager.init()

	def tearDown(self):
		self.engine.destroy()
		del self.log
	
	def testLeftRight(self):
		sound = self.soundmanager.createEmitter()
		id = self.engine.getSoundClipPool().addResourceFromFile('tests/data/left_right_test.ogg')
		sound.setSoundClip(id)
		sound.setLooping(True)
		sound.play()
		time.sleep(3);
	
	def test2Streams(self):
		em = self.soundmanager.createEmitter()
		sound = self.soundmanager.createEmitter()
		id = self.engine.getSoundClipPool().addResourceFromFile('tests/data/left_right_test.ogg')
		sound.setSoundClip(id)
		sound.setLooping(True)
		sound.setCursor(fife.SD_TIME_POS, 5)
		em.setSoundClip(id)
		em.setGain(0.7)
		em.play()
		sound.play()
		time.sleep(3);

TEST_CLASSES = [TestAudio]

if __name__ == '__main__':
    unittest.main()
