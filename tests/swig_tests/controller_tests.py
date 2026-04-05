#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


from .swig_test_utils import getEngine, unittest


class TestController(unittest.TestCase):
    def setUp(self):
        self.engine = getEngine(True)

    def tearDown(self):
        self.engine.destroy()

    def testInstances(self):
        print("\nTest SoundManager")
        self.assertTrue(self.engine.getSoundManager())
        print("Test EventManager")
        self.assertTrue(self.engine.getEventManager())
        print("Test TimeManager")
        self.assertTrue(self.engine.getTimeManager())
        print("Test ImageManager")
        self.assertTrue(self.engine.getImageManager())
        print("Test AnimationManager")
        self.assertTrue(self.engine.getAnimationManager())
        print("Test SoundClipManager")
        self.assertTrue(self.engine.getSoundClipManager())
        print("Test LogManager")
        self.assertTrue(self.engine.getLogManager())
        pass

    def testPumping(self):
        self.engine.initializePumping()
        for i in range(10):
            self.engine.pump()
        self.engine.finalizePumping()


TEST_CLASSES = [TestController]

if __name__ == "__main__":
    unittest.main()
