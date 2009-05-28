from pychan import widgets
from pychan.tools import callbackWithArguments as cbwa

class FakeTabWidget(widgets.VBox):
	def __init__(self, *args, **kwargs):
		super(FakeTabWidget, self).__init__(*args, **kwargs)
		
		self.tabs = []
		
		self.buttonbox = widgets.HBox()
		self.widgetarea = widgets.VBox()
		self.buttonbox.hexpand = 1
		self.buttonbox.vexpand = 0
		self.widgetarea.hexpand = 1
		self.widgetarea.vexpand = 1
		
		self.addChild(self.buttonbox)
		self.addChild(self.widgetarea)
		
	def addTab(self, widget, title):
		for tab in self.tabs:
			if tab[1] == widget:
				return
	
		widget.max_size = (1000, 1000)
		widget.hexpand = 1
		widget.vexpand = 1
	
		self.widgetarea.removeAllChildren()
		self.widgetarea.addChild( widget )
		
		
		button = widgets.Button(text=title)
		self.buttonbox.addChild(button)
		
		tab = (title, widget, button)
		self.tabs.append( tab )
		print self.tabs[0][1].parent
		
		def callback(event):
			self.buttonClicked(tab, event)
		button.capture(callback, "mouseClicked")
		
		return tab
		
	def removeTab(self, widget):
		for i, tab in enumerate(self.tabs):
			if tab[1] == widget:
				if widget.parent == self.widgetarea:
					self.widgetarea.removeChild(widget)
				self.buttonbox.removeChild(tab[2])
				del self.tabs[i]
		
	def buttonClicked(self, tab, event):
		pass
		
		