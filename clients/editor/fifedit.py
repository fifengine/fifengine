# Fifedit is the editor tool. It is designed to be embedded in clients, most notably the editor.
# Fifedit is based on a plugin system. See registerPlugin below.

import fife
import pychan
import pychan.widgets as widgets

class Fifedit(fife.IMouseListener, fife.IKeyListener):
	def __init__(self, engine):
		pychan.init(engine,debug=True)
		self.gui = pychan.loadXML('content/gui/rootpanel.xml')
		eventMap = {
			'quitButton'  : self.quit
		}
		self.gui.mapEvents(eventMap)
		self.gui.show()

		self.map = None
		self.camera = None
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

	def mousePressed(self, evt):
		self.selection = fife.ScreenPoint(evt.getX(), evt.getY())
		self.callback()

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
			self.make_inst = True
		elif keystr == 'x':
			self.delete_inst = True
		elif keystr == 'l':
			self.show_layers = True
		elif keystr == 'o':
			self.open_file = True

	def keyReleased(self, evt):
		pass

