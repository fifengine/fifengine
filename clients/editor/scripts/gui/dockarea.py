import pychan
from pychan import widgets
import scripts.editor
import fife
from panel import Panel
from faketabwidget import FakeTabWidget

class DockArea(widgets.VBox):
	def __init__(self, side, resizable=True, *args, **kwargs):
		super(DockArea, self).__init__(margins=(0,0,0,0), *args, **kwargs)
		self.capture(self.mouseEntered, "mouseEntered", "dockarea")
		self.capture(self.mouseExited, "mouseExited", "dockarea")
		self.capture(self.mouseMoved, "mouseMoved", "dockarea")
		self.capture(self.mouseDragged, "mouseDragged", "dockarea")
		self.capture(self.mousePressed, "mousePressed", "dockarea")
		self.capture(self.mouseReleased, "mouseReleased", "dockarea")
		
		self.engine = scripts.editor.getEditor().getEngine()
		
		self.cursor_id = 0
		self.cursor_type = 0
		
		self.vexpand=0
		self.hexpand=0
		
		self.resizable = resizable
		self.resize = False
		self._resizable = resizable
		self.side = side
		self._resizable_top = (side == "bottom")
		self._resizable_left = (side == "right")
		self._resizable_right = (side == "left")
		self._resizable_bottom = (side == "top")
		
		self.gui = None
		
		self.buildGui()
		
	def addChild(self, child):
		if isinstance(child, Panel):
			tabwidget = FakeTabWidget()
			tab = tabwidget.addTab(child, child.title)
			self.gui.addChild(tabwidget)
			def undock():
				tabwidget.removeTab(child)
				self.gui.removeChild(tabwidget)
				child.setDocked(False)
			tab[2].capture(undock)
			tabwidget.hexpand=1
			tabwidget.vexpand=1
		else:
			super(DockArea, self).addChild(child)
		
	def buildGui(self):
		if self.gui:
			self.removeChild(self.gui)
		
		if self.side == "left" or self.side == "right":
			self.gui = widgets.VBox()
		else:
			self.gui = widgets.HBox()
			
		self.gui.vexpand = 1
		self.gui.hexpand = 1
		
		self.addChild(self.gui)
			
		
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
				
			if self.left or self.right:
				# Resize children
				for child in self.findChildren(parent=self):
					child.min_size = (self.width, child.min_size[1])
					child.max_size = (self.width, child.max_size[1])
					
				
			# Resize vertically
			if self.top:
				self.y += diffY
				self.height -= diffY
			elif self.bottom:
				self.height = diffY
				
			if self.top or self.bottom:
				# Resize children
				for child in self.findChildren(parent=self):
					child.min_size = (child.min_size[0], self.height)
					child.max_size = (child.max_size[0], self.height)
					

	def mousePressed(self, event):
		if self.resizable is False:
			return
			
		self.left	= event.getX() < 5 and self._resizable_left
		self.right	= event.getX() > self.width-5 and self._resizable_right
		self.top	= event.getY() < 5 and self._resizable_top
		self.bottom	= event.getY() > self.height-5 and self._resizable_bottom
		
		if self.left or self.right or self.top or self.bottom:
			self.resize = True
			self.min_size = (5, 5)
			self.max_size = (5000, 5000)
		
	def mouseReleased(self, event):
		if self.resize:
			if self.side == "left" or self.side == "right":
				self.min_size = (self.width, self.min_size[1])
				self.max_size = (self.width, self.max_size[1])
			else:
				self.min_size = (self.min_size[0], self.height)
				self.max_size = (self.max_size[0], self.height)
				
			self.adaptLayout()
			event.consume()
		
			self.resize = False
			if event.getX() <= 0 or event.getX() >= self.width \
					or event.getY() <= 0 or event.getY() >= self.height:
				self.mouseExited(event)
	
		