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

	def keyPressed(self, event):
		keyval = event.getKey().getValue()
		if (keyval == fife.IKey.LEFT):
			print 'left pressed on map'
		elif (keyval == fife.IKey.RIGHT):
			print 'right pressed on map'
		elif (keyval == fife.IKey.UP):
			print 'up pressed on map'
		elif (keyval == fife.IKey.DOWN):
			print 'down pressed on map'

	def keyReleased(self, event):
		pass
	
	def mouseEntered(self, evt):
		print "mouse entered map area"
	
	def mouseExited(self, evt):
		print "mouse exited map area"
	
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
		print "mouse move %d, %d" % (evt.getX(), evt.getY())
	
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
		self.ctrl.turn()
		self.cam.render()

