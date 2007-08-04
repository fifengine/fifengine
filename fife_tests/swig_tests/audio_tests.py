#!/usr/bin/python
from __init__ import *
import time

class TestAudio(unittest.TestCase):
	
	def setUp(self):
		self.engine = fife.Engine(True)
		self.audiomanager = self.engine.getAudioManager()

	def tearDown(self):
		del self.engine	
	
	def testOggStart(self):
		self.audiomanager.setAmbientSound('fife_tests/data/audiotest1.ogg')
		time.sleep(1)

	def testAcmStart(self):
		self.audiomanager.setAmbientSound('fife_tests/data/audiotest2.acm')
		time.sleep(1)

	def testVolume(self):
		self.audiomanager.setAmbientSound('fife_tests/data/audiotest1.ogg')
		for i in xrange(20):
			self.audiomanager.setVolume(float(i % 10) / 10)
			time.sleep(0.05)

TEST_CLASSES = [TestAudio]

if __name__ == '__main__':
    unittest.main()
