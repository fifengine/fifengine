import fife

MOVE_SMOOTHING_DECREASE = 2
MAX_MOVE_SMOOTHING = 50
class ViewGameState(fife.IKeyListener, fife.IMouseListener):
	def __init__(self):
		fife.IKeyListener.__init__(self)
		fife.IMouseListener.__init__(self)
		self.ctrl = fife.Control()
		self.cam = self.ctrl.createCamera()
		self.active = False

		xml_loader = fife.XML()
		xml_loader.thisown = 0
		print "Adding map loader " + xml_loader.getName()
		self.ctrl.addMapLoader(xml_loader)
		
		fallout_loader = fife.Fallout()
		fallout_loader.thisown = 0
		print "Adding map loader " + fallout_loader.getName()
		self.ctrl.addMapLoader(fallout_loader)

		self.dx = 0
		self.dy = 0
		self.prevDragX = 0
		self.prevDragY = 0
		self.moveSpeedX = 0
		self.moveSpeedY = 0
		self.dragging = False

	def keyPressed(self, event):
		keyval = event.getKey().getValue()
		if (keyval == fife.IKey.LEFT):
			self.dx = -20
		elif (keyval == fife.IKey.RIGHT):
			self.dx = 20
		elif (keyval == fife.IKey.UP):
			self.dy = -20
		elif (keyval == fife.IKey.DOWN):
			self.dy = 20
		elif (keyval == fife.IKey.TAB):
			elevation = self.ctrl.getCurrentElevation() + 1
			if (elevation >= self.ctrl.getNumElevations()):
				elevation = 0
			self.ctrl.setElevation(elevation)

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
		if(evt.getButton() == 1):	
			p = self.ctrl.select(fife.Point(evt.getX(), evt.getY()), 0)
			print "tile selected on layer 0 at location: %d %d" % (p.x, p.y)
		elif(evt.getButton() == 2):
			p = self.ctrl.select(fife.Point(evt.getX(), evt.getY()), 1)
			print "tile selected on layer 1 at location: %d %d" % (p.x, p.y)
		self.prevDragX = 0
		self.prevDragY = 0
	
	def mouseReleased(self, evt):
		if evt.getButton() == fife.IMouseEvent.MIDDLE:
			self.dragging = False
	
	def mouseClicked(self, evt):
		pass
	
	def mouseWheelMovedUp(self, evt):
		print "mouse wheel up"
	
	def mouseWheelMovedDown(self, evt):
		print "mouse wheel down"
	
	def mouseMoved(self, evt):
		pass
	
	def mouseDragged(self, evt):
		self.dragging = True
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
#			self.walker = self.ctrl.createDynamicObject(1,fife.Point(5,5),"Critter:Beekeeper:Talk:S")
			self.ctrl.start()

	def deactivate(self):
		if self.active:
			self.ctrl.stop()
			self.active = False

	def calc_move_smoothing(self, val):
		if val < 0:
			if val < -MAX_MOVE_SMOOTHING:
				val = -MAX_MOVE_SMOOTHING
			val += MOVE_SMOOTHING_DECREASE
			if val > 0: 
				val = 0
		else:
			if val > MAX_MOVE_SMOOTHING:
				val = MAX_MOVE_SMOOTHING
			val -= MOVE_SMOOTHING_DECREASE
			if val < 0: 
				val = 0
		return val
	
	def turn(self):
		if self.dx or self.dy:
			self.moveSpeedX, self.moveSpeedY = self.dx, self.dy
			self.cam.moveBy(fife.Point(self.dx, self.dy))
			self.dx, self.dy = 0, 0
		else:
			if not self.dragging and (self.moveSpeedX or self.moveSpeedY):
				self.moveSpeedX = self.calc_move_smoothing(self.moveSpeedX)
				self.moveSpeedY = self.calc_move_smoothing(self.moveSpeedY)
				self.cam.moveBy(fife.Point(self.moveSpeedX, self.moveSpeedY))
		self.ctrl.update()

