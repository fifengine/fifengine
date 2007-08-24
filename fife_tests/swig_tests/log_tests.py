#!/usr/bin/python
from swig_test_utils import *

class TestLog(unittest.TestCase):
	
	def testLog(self):
		log = fife.Log.initialize(fife.Log.LEVEL_MAX, True, True)
		fife.Log("       Testing log...")

TEST_CLASSES = [TestLog]

if __name__ == '__main__':
    unittest.main()
