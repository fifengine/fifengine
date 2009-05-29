from pychan import widgets
from pychan.tools import callbackWithArguments as cbwa
from resizablebase import ResizableBase

import scripts

class FakeTabWidget(widgets.VBox, ResizableBase):
	def __init__(self, resizable=None, *args, **kwargs):
		if resizable == None:
			resizable = False

		widgets.VBox.__init__(self, *args, **kwargs)
		ResizableBase.__init__(self, resizable, *args, **kwargs)
		
		self.tabs = []
		
		self.buttonbox = widgets.HBox()
		self.widgetarea = widgets.VBox()
		self.buttonbox.hexpand = 1
		self.buttonbox.vexpand = 0
		self.widgetarea.hexpand = 1
		self.widgetarea.vexpand = 1
		
		self.addChild(self.buttonbox)
		self.addChild(self.widgetarea)
	
		self.resizable_top = False
		self.resizable_left = False
		self.resizable_right = False
		self.resizable_bottom = False
		
	def addTab(self, widget, title):
		for tab in self.tabs:
			if tab[1] == widget:
				return
	
		widget.max_size = (5000, 5000)
		widget.hexpand = 1
		widget.vexpand = 1
	
		button = widgets.ToggleButton(text=title, group="faketab_"+str(id(self)))
		self.buttonbox.addChild(button)
		
		tab = (title, widget, button)
		self.tabs.append( tab )

		button.capture(cbwa(self.showTab, tab))
		self.showTab(tab)
		
		return tab
		
	def removeTab(self, widget):
		for i, tab in enumerate(self.tabs):
			if tab[1] == widget:
				if widget.parent == self.widgetarea:
					self.widgetarea.removeChild(widget)
				self.buttonbox.removeChild(tab[2])
				del self.tabs[i]
				break
		else: return
			
		if len(self.tabs) > 0:
			self.showTab(self.tabs[0])
		
	def showTab(self, tab):
		tab[2].toggled = True
		self.widgetarea.removeAllChildren()
		self.widgetarea.addChild(tab[1])
		self.widgetarea.adaptLayout()
		
	