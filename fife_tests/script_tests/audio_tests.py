#!/usr/bin/python#!/usr/bin/python
from __init__ import *
import time

class TestAudio(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine()
		self.audiomanager = self.engine.getAudioManager()

	def tearDown(self):
		del self.engine	
	
	def testPlayback(self):
		self.audiomanager.setAmbientSound('content/audio/music/maybe.ogg')
		time.sleep(1)

	def testVolume(self):
		self.audiomanager.setAmbientSound('content/audio/music/maybe.ogg')
		for i in xrange(20):
			self.audiomanager.setVolume(float(i % 10) / 10)
			time.sleep(0.1)

TEST_CLASSES = [TestAudio]

if __name__ == '__main__':
    unittest.main()
