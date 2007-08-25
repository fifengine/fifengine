#!/usr/bin/python
from swig_test_utils import *

class TestAttributedClass(unittest.TestCase):
	
	def setUp(self):
		self.c = fife.AttributedClass('Attributedclass for testing')

	def testBools(self):
		c = self.c
		c.set_bool('myvalue1', True)
		c.set_bool('myvalue2', False)
		
		self.assert_(c.get_bool('myvalue1'))
		self.assert_(not c.get_bool('myvalue2'))

	def testInts(self):
		c = self.c
		c.set_int('myvalue1', 3)
		c.set_int('myvalue2', 6)
		
		self.assertEquals(c.get_int('myvalue1') + c.get_int('myvalue2'), 9)


TEST_CLASSES = [TestAttributedClass]

if __name__ == '__main__':
    unittest.main()
