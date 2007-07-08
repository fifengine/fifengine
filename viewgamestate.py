import engine.fife as fife

class ViewGameState(fife.IKeyListener, fife.IMouseListener):
	def __init__(self):
		fife.IKeyListener.__init__(self)
		fife.IMouseListener.__init__(self)
		self.ctrl = fife.Control()
		self.cam = fife.Camera(self.ctrl)
		self.active = False

		xml_loader = fife.XML()
		xml_loader.thisown = 0
		self.ctrl.addMapLoader(xml_loader)

		self.dx = 0
		self.dy = 0

	def keyPressed(self, event):
		keyval = event.getKey().getValue()
		if (keyval == fife.IKey.LEFT):
			self.dx = -10
		elif (keyval == fife.IKey.RIGHT):
			self.dx = 10
		elif (keyval == fife.IKey.UP):
			self.dy = -10
		elif (keyval == fife.IKey.DOWN):
			self.dy = 10

	def keyReleased(self, event):
		keyval = event.getKey().getValue()
		if (keyval == fife.IKey.LEFT):
			self.dx = 0
		elif (keyval == fife.IKey.RIGHT):
			self.dx = 0
		elif (keyval == fife.IKey.UP):
			self.dy = 0
		elif (keyval == fife.IKey.DOWN):
			self.dy = 0
	
	def mouseEntered(self, evt):
		print "mouse entered map area"
	
	def mouseExited(self, evt):
		print "mouse exited map area"
		self.dx = 0
		self.dy = 0
	
	def mousePressed(self, evt):
		print "mouse pressed, button = %d" % evt.getButton()
	
	def mouseReleased(self, evt):
		pass
	
	def mouseClicked(self, evt):
		pass
	
	def mouseWheelMovedUp(self, evt):
		print "mouse wheel up"
	
	def mouseWheelMovedDown(self, evt):
		print "mouse wheel down"
	
	def mouseMoved(self, evt):
		x = evt.getX()
		y = evt.getY()

# NOTE: screen_width and screen_height aren't currently defined
# in scripts. Also, this logic interacts oddly with the arrow
# keys.
#
#		self.dx = 0
#		self.dy = 0

#		if (x < 20 and x > 0):
#			self.dx = - 20
#		elif (y < 20 and y > 0):
#			self.dy = - 20
#		elif (x > screen_width - 20 and x < screen_width):
#			self.dx = 20
#		elif (y > screen_height - 20 and y < screen_height):
#			self.dy = 20
	
	def mouseDragged(self, evt):
		print "mouse drag %d, %d" % (evt.getX(), evt.getY())
	
	def setMap(self, path):
		self.map = path

	def activate(self):
		if not self.active:
			self.active = True
			self.ctrl.load(self.map)
			self.ctrl.start()

	def deactivate(self):
		if self.active:
			self.ctrl.stop()
			self.active = False

	def turn(self):
		self.cam.moveBy(fife.Point(self.dx, self.dy))
		self.ctrl.turn()
		self.cam.render()

