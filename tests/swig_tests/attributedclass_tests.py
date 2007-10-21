#!/usr/bin/env python
from swig_test_utils import *

class TestAttributedClass(unittest.TestCase):
	
	def setUp(self):
		self.c = fife.AttributedClass('Attributedclass for testing')

	def testMetadata(self):
		c = self.c
		c.set('myvalue1', '3')
		c.set('myvalue2', '6')
		
		self.assertEquals(int(c.get('myvalue1')) + int(c.get('myvalue2')), 9)


TEST_CLASSES = [TestAttributedClass]

if __name__ == '__main__':
    unittest.main()
