import pychan
from pychan import widgets
import scripts.editor
import fife

class Panel(widgets.Window):
	def __init__(self, resizable=True, dockable=True, *args, **kwargs):
		super(Panel, self).__init__(*args, **kwargs)
		self.capture(self.mouseEntered, "mouseEntered", "panel")
		self.capture(self.mouseExited, "mouseExited", "panel")
		self.capture(self.mouseMoved, "mouseMoved", "panel")
		self.capture(self.mouseDragged, "mouseDragged", "panel")
		self.capture(self.mousePressed, "mousePressed", "panel")
		self.capture(self.mouseReleased, "mouseReleased", "panel")
		
		self.engine = scripts.editor.getEditor().getEngine()
		
		self.cursor_id = 0
		self.cursor_type = 0
		
		self.dockable = dockable
		self.resizable = resizable
		self.resize = False
		self._movable = self.real_widget.isMovable()
		self._resizable = resizable
		self._resizable_top = True
		self._resizable_left = True
		self._resizable_right = True
		self._resizable_bottom = True
		
		self.dragoffset = (0, 0)
		
		self._floating = True
		self._titlebarheight = 16
		
	def setDocked(self, docked):
		if self.dockable is False:
			return
		
		if docked is True and self._floating == True:
				self._floating = False
				self.real_widget.setTitleBarHeight(0)
				self.real_widget.setMovable(False)
				self._movable = False
				#self.resizable = False
				
		elif docked is False and self._floating is False:			
			self._floating = True
			self._movable = True
			self.real_widget.setMovable(True)
			self.resizable = self._resizable
			
			# Since x and y coordinates are reset if the widget gets hidden,
			# we need to store them
			absX = self.x
			absY = self.y
			# Get absolute pos
			parent = self.parent
			while parent is not None:
				absX += parent.x
				absY += parent.y
				parent = parent.parent
			
			if self.parent is not None:
				# Remove from parent widget
				widgetParent = self.parent
				widgetParent.removeChild(self)
				widgetParent.adaptLayout()
				self.hide()
				
			self.real_widget.setTitleBarHeight(self._titlebarheight)
			self.show()
			
			# Slighly offset toolbar when undocking
			mw = pychan.internal.screen_width() / 2
			mh = pychan.internal.screen_height() / 2
			if absX < mw:
				self.x = absX + self._titlebarheight
			else:
				self.x = absX - self._titlebarheight
			if absY < mh:
				self.y = absY + self._titlebarheight
			else:
				self.y = absY - self._titlebarheight

	def isDocked(self):
		return self._floating == False
		
		
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
		bottom	= event.getY() - self.titlebar_height > self.height-5 and self._resizable_bottom
		
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
			print "Resizable windows are a lot cooler with native cursors =)",
			print " Check the patch in the forums"
		
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
				self.height = diffY-self.titlebar_height
			
		
	def mousePressed(self, event):
		if self.resizable is False:
			return
			
		self.left	= event.getX() < 5 and self._resizable_left
		self.right	= event.getX() > self.width-5 and self._resizable_right
		self.top	= event.getY() < 5 and self._resizable_top
		self.bottom	= event.getY() - self.titlebar_height > self.height-5 and self._resizable_bottom
		
		if self.left or self.right or self.top or self.bottom:
			self._movable = self.real_widget.isMovable()
			self.real_widget.setMovable(False) # Don't let guichan move window while we resize
			self.resize = True
			self.min_size = (10, 10)
			self.max_size = (5000, 5000)
		
	def mouseReleased(self, event):
		# Resize/move done
		self.real_widget.setMovable(self._movable)
		
		if self.resize:
			self.min_size = (self.width, self.height)
			self.max_size = (self.width, self.height)
			self.adaptLayout()
			event.consume()
		
			self.resize = False
			if event.getX() <= 0 or event.getX() >= self.width \
					or event.getY() <= 0 or event.getY() >= self.height+self.titlebar_height:
				self.mouseExited(event)
		elif self._movable:
			editor = scripts.editor.getEditor()
			if self.x + event.getX() < 25:
				editor.dockWidgetTo(self, "left")
				
			elif self.x + event.getX() > pychan.internal.screen_width() - 25:
				editor.dockWidgetTo(self, "right")
				
			elif self.y + event.getY() < 50:
				editor.dockWidgetTo(self, "top")
				
			elif self.y + event.getY() > pychan.internal.screen_height() - 50:
				editor.dockWidgetTo(self, "bottom")
	
	
# Register widget to pychan
if 'Panel' not in widgets.WIDGETS:
	widgets.WIDGETS['Panel'] = Panel
		