# Fifedit is the editor tool. It is designed to be embedded in clients, most notably the editor.
# Fifedit is based on a plugin system. See registerPlugin below.

import pychan
import pychan.widgets as widgets

class Fifedit:
	def __init__(self, engine):
		pychan.init(engine,debug=True)
		self.gui = pychan.loadXML('content/gui/rootpanel.xml')
		eventMap = {
			'quitButton'  : self.quit
		}
		self.gui.mapEvents(eventMap)
		self.gui.show()

		self.camera = None

	# To create a plugin, just define menu_items with string keys and function values.
	# The key will be displayed on the Editor menu, and the value will be called when the key is clicked.
	def registerPlugin(self, plugin):
		for key in plugin.menu_items:
			button = widgets.Button(parent=self.gui, text=key)
			button.name = key
			self.gui.add(button)

		self.gui.mapEvents(plugin.menu_items)
		self.gui.adaptLayout()

	def quit(self):
		self.gui.hide()
