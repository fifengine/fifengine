# coding: utf-8

import pychan.widgets as widgets

class Plugin(object):
	"""
	Plugin base for the editor.
	Currently transitional code.
	"""
	def __init__(self):
		self.menu_items = {}

	def install(self,gui):
		for key in self.menu_items:
			button = widgets.Button(name=key,text=key)
			gui.addChild(button)
		gui.mapEvents(self.menu_items)
		gui.adaptLayout()

	def deinstall(self,fifedit):
		pass
