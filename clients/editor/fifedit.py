# Fifedit is the editor tool. It is designed to be embedded in clients, most notably the editor.
# Fifedit is based on a plugin system. See registerPlugin below.

import math

import fife
import pychan
import pychan.widgets as widgets

class Fifedit(fife.IMouseListener, fife.IKeyListener):
	def __init__(self, engine):
		self.eventmanager = engine.getEventManager()

		fife.IMouseListener.__init__(self)
		self.eventmanager.addMouseListener(self)
		fife.IKeyListener.__init__(self)
		self.eventmanager.addKeyListener(self)

		pychan.init(engine,debug=True)
		self.gui = pychan.loadXML('content/gui/rootpanel.xml')
		eventMap = {
			'quitButton'  : self.quit
		}
		self.gui.mapEvents(eventMap)
		self.gui.show()

		self.camera = None
		self.elevation = None
		self.layer = None
		self.selection = None

	# To create a plugin, just define menu_items with string keys and function values.
	# The key will be displayed on the Editor menu, and the value will be called when the key is clicked.
	def registerPlugin(self, plugin):
		for key in plugin.menu_items:
			button = widgets.Button(parent=self.gui,name=key,text=key)
			#button.name = key
			self.gui.add(button)

		self.gui.mapEvents(plugin.menu_items)
		self.gui.adaptLayout()

	def quit(self):
		self.gui.hide()

	def editWith(self, camera):
		self.camera = camera
		self.elevation = camera.getLocation().getElevation()
		self.layer = camera.getLocation().getLayer()

	def mousePressed(self, evt):
		if self.camera:
			# TODO: make Sleek fix this ugly mess
			tmp = fife.ScreenPoint(evt.getX(), evt.getY())
			dy = -(tmp.y - self.camera.toScreenCoordinates(self.camera.getLocation().getElevationCoordinates()).y)
			tmp.z = (int)(math.tan(self.camera.getTilt()* (math.pi / 180.0)) * dy)
			self.selection = self.camera.toElevationCoordinates(tmp)
			self.selection.z = 0
			self.selection = self.layer.getCellGrid().toLayerCoordinates(self.selection)

	def mouseReleased(self, evt):
		pass
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	def mouseWheelMovedUp(self, evt):
		pass
	def mouseWheelMovedDown(self, evt):
		pass
	def mouseMoved(self, evt):
		pass
	def mouseDragged(self, evt):
		pass

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keystr == 'm':
			pass # TODO: make an instance
		elif keystr == 'x':
			if self.selection:
				for inst in self.layer.getInstances('loc', str(self.selection.x) + ',' + str(self.selection.y)):
					self.layer.deleteInstance(inst)

	def keyReleased(self, evt):
		pass

