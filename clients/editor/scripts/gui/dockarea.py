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
		self.tabwidgets = []
		
	def dockChild(self, child, x, y):
		child.setDocked(True)
		areaX, areaY = self.getAbsolutePos()
		
		placeAfter = None
		placeBefore = None
		placeIn = None
		
		if x >= 0 and y >= 0:
			# See if widget was dropped on a tabwidget
			for tabwidget in self.tabwidgets:
				absX, absY = tabwidget.getAbsolutePos()
					
				if absX <= x and absY <= y \
						and absX+tabwidget.width >= x and absX+tabwidget.height >= y:
					# Check if the user tried to place it in front, or after this widget
					if self.side == "left" or self.side == "right":
						if y < absY+30:
							placeBefore = tabwidget
						elif y > absY+tabwidget.height-30:
							placeAfter = tabwidget
						else:
							placeIn = tabwidget
					else:
						if x < absX+30:
							placeBefore = tabwidget
						elif x > absX+tabwidget.width-30:
							placeAfter = tabwidget
						else:
							placeIn = tabwidget
					break
	
		if placeIn is None:
			tabwidget = FakeTabWidget(resizable=True)
			tabwidget.hexpand=1
			tabwidget.vexpand=1
		
			if self.side == "left" or self.side == "right":
				tabwidget._resizable_bottom = True
			else:
				tabwidget._resizable_right = True
			self.tabwidgets.append(tabwidget)
			
			if placeBefore:
				self.gui.insertChildBefore(tabwidget, placeBefore)
			elif placeAfter:
				self.gui.insertChild(tabwidget, self.gui.children.index(placeAfter)+1)
			else:
				self.gui.addChild(tabwidget)
		else:
			tabwidget = placeIn

		tab = tabwidget.addTab(child, child.title)
		
		def undock(event):
			if event.getButton() != 2: # Right click
				return
				
			tabwidget.removeTab(child)
			child.setDocked(False)
			
			if len(tabwidget.tabs) <= 0:
				self.gui.removeChild(tabwidget)
				self.tabwidgets.remove(tabwidget)
			self.adaptLayout()
			
		tab[2].capture(undock, "mouseClicked")
		
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
				for child in self.gui.findChildren(parent=self.gui):
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
				for child in self.gui.findChildren(parent=self.gui):
					child.min_size = (child.min_size[0], self.height)
					child.max_size = (child.max_size[0], self.height)
					
			self.gui.max_size = (self.width, self.height)
					

	def mousePressed(self, event):
		if self.resizable is False:
			return
			
		self.left	= event.getX() < 5 and self._resizable_left
		self.right	= event.getX() > self.width-5 and self._resizable_right
		self.top	= event.getY() < 5 and self._resizable_top
		self.bottom	= event.getY() > self.height-5 and self._resizable_bottom
		
		if self.left or self.right or self.top or self.bottom:
			self.resize = True
		
	def mouseReleased(self, event):
		if self.resize:
			self.adaptLayout()
			event.consume()
		
			self.resize = False
			if event.getX() <= 0 or event.getX() >= self.width \
					or event.getY() <= 0 or event.getY() >= self.height:
				self.mouseExited(event)
				