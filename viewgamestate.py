import engine.fife as fife

class ViewGameState(fife.IKeyListener, fife.IMouseListener):
	def __init__(self):
		fife.IKeyListener.__init__(self)
		fife.IMouseListener.__init__(self)
		self.ctrl = fife.Control()
		self.cam = fife.Camera(self.ctrl)
		self.active = False

		self.xml_loader = fife.XML()
		print "Adding map loader " + self.xml_loader.getName()
		self.ctrl.addMapLoader(self.xml_loader)
		
		self.fallout_loader = fife.Fallout()
		print "Adding map loader " + self.fallout_loader.getName()
		self.ctrl.addMapLoader(self.fallout_loader)

		self.dx = 0
		self.dy = 0
		self.prevDragX = 0
		self.prevDragY = 0

	def keyPressed(self, event):
		keyval = event.getKey().getValue()
		if (keyval == fife.IKey.LEFT):
			self.dx = -30
		elif (keyval == fife.IKey.RIGHT):
			self.dx = 30
		elif (keyval == fife.IKey.UP):
			self.dy = -30
		elif (keyval == fife.IKey.DOWN):
			self.dy = 30

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
		self.prevDragX = 0
		self.prevDragY = 0
	
	def mouseReleased(self, evt):
		pass
	
	def mouseClicked(self, evt):
		pass
	
	def mouseWheelMovedUp(self, evt):
		print "mouse wheel up"
	
	def mouseWheelMovedDown(self, evt):
		print "mouse wheel down"
	
	def mouseMoved(self, evt):
		pass
	
	def mouseDragged(self, evt):
		if evt.getButton() == fife.IMouseEvent.MIDDLE:
			if self.prevDragX and self.prevDragY:
				self.dx = self.prevDragX - evt.getX()
				self.dy = self.prevDragY - evt.getY()
			self.prevDragX = evt.getX();
			self.prevDragY = evt.getY();
	
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
		if self.dx or self.dy:
			self.cam.moveBy(fife.Point(self.dx, self.dy))
			self.dx, self.dy = 0, 0
		self.ctrl.turn()
		self.cam.render()

