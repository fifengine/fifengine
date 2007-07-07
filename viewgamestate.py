import engine.fife as fife

class ViewGameState:
	def __init__(self):
		self.ctrl = fife.Control()
		self.cam = fife.Camera(self.ctrl)

		xml_loader = fife.XML()
		xml_loader.thisown = 0
		self.ctrl.addMapLoader(xml_loader)

	def setMap(self, path):
		self.map = path

	def activate(self):
		self.ctrl.load(self.map)
		self.ctrl.start()

	def deactivate(self):
		self.ctrl.stop()

	def turn(self):
		self.ctrl.turn()
		self.cam.render()

