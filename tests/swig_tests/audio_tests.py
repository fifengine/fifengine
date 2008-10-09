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
