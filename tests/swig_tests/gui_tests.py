#!/usr/bin/env python
from swig_test_utils import *

class TestGui(unittest.TestCase):
	
	def setUp(self):
		self.engine = getEngine()
		self.guimanager = self.engine.getGuiManager()
		self.renderbackend = self.engine.getRenderBackend()

	def tearDown(self):
		self.engine.destroy()
	
	def testFonts(self):
		ttffont = fife.TTFont('tests/data/FreeMono.ttf', 14)
		ttffont.thisown = 0
		subimagefont = fife.SubImageFont('tests/data/rpgfont.png', 
			       ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"',
				self.engine.getImagePool())
		subimagefont.thisown = 0
		fonts = [fife.GuiFont(ttffont), fife.GuiFont(subimagefont)]
		for f in fonts:
			f.setColor(255,20,20)
		container = fife.Container()
		self.guimanager.add(container)
		container.setSize(self.renderbackend.getWidth(), 
		                  self.renderbackend.getHeight())
		container.setOpaque(False)
		label1 = fife.Label('This is a Truetype font')
		label1.setPosition(0, 70)
		label1.setFont(fonts[0])
		container.add(label1)
		label2 = fife.Label('This is a Image font')
		label2.setPosition(0, 100)
		label2.setFont(fonts[1])
		container.add(label2)
		labels = [label1, label2]

		self.engine.initializePumping()
		for i in xrange(100):
			self.engine.pump()
		self.engine.finalizePumping()


TEST_CLASSES = [TestGui]

if __name__ == '__main__':
    unittest.main()

