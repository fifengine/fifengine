#!/usr/bin/env python
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
		del self.engine
		del self.log
	
	def testLeftRight(self):
		sound = self.soundmanager.createEmitter()
		sound.load('tests/data/left_right_test.ogg')
		sound.setLooping(True)
		sound.play()
		time.sleep(30);
	
	def test2Streams(self):
		em = self.soundmanager.createEmitter()
		sound = self.soundmanager.createEmitter()
		sound.load('tests/data/left_right_test.ogg')
		sound.setLooping(True)
		sound.setCursor(fife.SD_TIME_POS, 5)
		em.load('tests/data/left_right_test.ogg')
		em.setGain(0.7)
		em.play()
		sound.play()
		time.sleep(30);

TEST_CLASSES = []

if __name__ == '__main__':
    unittest.main()
