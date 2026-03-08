#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import

import time

from fife.extensions import fifelog

from .swig_test_utils import fife, getEngine, unittest


class TestAudio(unittest.TestCase):

    def setUp(self):
        self.engine = getEngine(True)
        self.soundmanager = self.engine.getSoundManager()
        self.soundclipmanager = self.engine.getSoundClipManager()
        self.log = fifelog.LogManager(self.engine, promptlog=True, filelog=False)
        self.log.setVisibleModules("resource manager", "audio")
        self.soundmanager.init()

    def tearDown(self):
        self.engine.destroy()
        del self.log

    def testLeftRight(self):
        sound = self.soundmanager.createEmitter("tests/data/left_right_test.ogg")
        sound.setLooping(True)
        sound.play()
        time.sleep(3)

    def test2Streams(self):
        em = self.soundmanager.createEmitter()
        sound = self.soundmanager.createEmitter()
        clip = self.soundclipmanager.load("tests/data/left_right_test.ogg")
        sound.setSoundClip(clip)
        sound.setLooping(True)
        sound.setCursor(fife.SD_TIME_POS, 5)
        em.setSoundClip(clip)
        em.setGain(0.7)
        em.play()
        sound.play()
        time.sleep(3)


TEST_CLASSES = [TestAudio]

if __name__ == "__main__":
    unittest.main()