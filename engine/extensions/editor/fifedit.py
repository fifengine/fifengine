# Fifedit is the editor tool. It is designed to be embedded in clients, most notably the editor.
# Fifedit is a plugin system for editing tools. See registerPlugin below.

import fife
import pychan
import pychan.widgets as widgets

class Fifedit():
	def __init__(self, engine):
		pychan.init(engine,debug=True)
		self.gui = pychan.loadXML('content/gui/rootpanel.xml')
		eventMap = {
			'quitButton'  : self.quit
		}
		self.gui.mapEvents(eventMap)
		self.gui.show()

		self.active = True

	# To create a plugin, just define menu_items with string keys and function values.
	# The key will be displayed on the Editor menu, and the value will be called when the key is clicked.
	def registerPlugin(self, plugin):
		for key in plugin.menu_items:
			button = widgets.Button(name=key,text=key)
			self.gui.addChild(button)
		self.gui.mapEvents(plugin.menu_items)
		self.gui.adaptLayout()

	def quit(self):
		self.gui.hide()
		self.active = False
