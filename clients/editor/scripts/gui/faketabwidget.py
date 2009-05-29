from pychan import widgets
from pychan.tools import callbackWithArguments as cbwa

import scripts

class FakeTabWidget(widgets.VBox):
	def __init__(self, resizable=False, *args, **kwargs):
		super(FakeTabWidget, self).__init__(*args, **kwargs)
		
		self.tabs = []
		
		self.buttonbox = widgets.HBox()
		self.widgetarea = widgets.VBox()
		self.buttonbox.hexpand = 1
		self.buttonbox.vexpand = 0
		self.widgetarea.hexpand = 1
		self.widgetarea.vexpand = 1
		
		self.capture(self.mouseEntered, "mouseEntered", "dockarea")
		self.capture(self.mouseExited, "mouseExited", "dockarea")
		self.capture(self.mouseMoved, "mouseMoved", "dockarea")
		self.capture(self.mouseDragged, "mouseDragged", "dockarea")
		self.capture(self.mousePressed, "mousePressed", "dockarea")
		self.capture(self.mouseReleased, "mouseReleased", "dockarea")
		
		self.engine = scripts.editor.getEditor().getEngine()
		
		self.addChild(self.buttonbox)
		self.addChild(self.widgetarea)
				
		self.resizable = resizable
		self.resize = False
		self._resizable = resizable
		self._resizable_top = False
		self._resizable_left = False
		self._resizable_right = False
		self._resizable_bottom = False
		
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
		
	
	def mouseEntered(self, event):
		# Save cursor id
		if self.resizable and self.resize is False:
			cursor = self.engine.getCursor()
			self.cursor_id = cursor.getId()
			self.cursor_type = cursor.getType()
		
	def mouseMoved(self, event):
		if self.resizable is False: 
			return
		
		cursor = self.engine.getCursor()
		
		left	= event.getX() < 5 and self._resizable_left
		right	= event.getX() > self.width-5 and self._resizable_right
		top		= event.getY() < 5 and self._resizable_top
		bottom	= event.getY() > self.height-5 and self._resizable_bottom

		try:
			if left and top:
				cursor.set(fife.CURSOR_NATIVE, fife.NC_RESIZENW)
			elif right and top:
				cursor.set(fife.CURSOR_NATIVE, fife.NC_RESIZENE)
			elif left and bottom:
				cursor.set(fife.CURSOR_NATIVE, fife.NC_RESIZESW)
			elif right and bottom:
				cursor.set(fife.CURSOR_NATIVE, fife.NC_RESIZESE)
			elif left:
				cursor.set(fife.CURSOR_NATIVE, fife.NC_RESIZEW)
			elif right:
				cursor.set(fife.CURSOR_NATIVE, fife.NC_RESIZEE)
			elif top:
				cursor.set(fife.CURSOR_NATIVE, fife.NC_RESIZEN)
			elif bottom:
				cursor.set(fife.CURSOR_NATIVE, fife.NC_RESIZES)
			else:
				cursor.set(self.cursor_type, self.cursor_id)
		except:
			pass
		
	def mouseExited(self, event):
		# Reset cursor to whatever it was before it entered this window
		if self.resizable and self.resize is False:
			cursor = self.engine.getCursor()
			cursor.set(self.cursor_type, self.cursor_id)
		
	def mouseDragged(self, event):
		if self.resizable and self.resize:
			diffX = event.getX()
			diffY = event.getY()
			
			# Resize horizontally
			if self.left:
				self.x += diffX
				self.width -= diffX
			elif self.right:
				self.width = diffX

			# Resize vertically
			if self.top:
				self.y += diffY
				self.height -= diffY
			elif self.bottom:
				self.height = diffY

	def mousePressed(self, event):
		if self.resizable is False:
			return
			
		self.left	= event.getX() < 5 and self._resizable_left
		self.right	= event.getX() > self.width-5 and self._resizable_right
		self.top	= event.getY() < 5 and self._resizable_top
		self.bottom	= event.getY() > self.height-5 and self._resizable_bottom
		
		if self.left or self.right or self.top or self.bottom:
			self.resize = True
			self.min_size = (30, 30)
		
	def mouseReleased(self, event):
		if self.resize:
			self.min_size = (self.width, self.height)
			self.adaptLayout()
			event.consume()
		
			self.resize = False
			if event.getX() <= 0 or event.getX() >= self.width \
					or event.getY() <= 0 or event.getY() >= self.height:
				self.mouseExited(event)
				