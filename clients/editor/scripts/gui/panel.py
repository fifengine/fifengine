import pychan
from pychan import widgets
import scripts.editor
import fife
from resizablebase import ResizableBase

class Panel(widgets.Window, ResizableBase):
	def __init__(self, dockable=True, *args, **kwargs):
		widgets.Window.__init__(self, *args, **kwargs)
		ResizableBase.__init__(self, *args, **kwargs)
	
		self.cursor_id = 0
		self.cursor_type = 0
		
		self.dockable = dockable
		self._movable = self.real_widget.isMovable()
		self._resizable = self.resizable

		self._floating = True
		self._titlebarheight = 16
		
		self.dockarea = None
		
		self._editor = scripts.editor.getEditor()
		
	def setDocked(self, docked):
		if self.dockable is False:
			return
		
		if docked is True and self._floating == True:
			self._floating = False
			self.real_widget.setTitleBarHeight(0)
			self.real_widget.setMovable(False)
			self._movable = False
			self.resizable = False
				
		elif docked is False and self._floating is False:			
			self._floating = True
			self._movable = True
			self.real_widget.setMovable(True)
			self.resizable = self._resizable
			
			# Since x and y coordinates are reset if the widget gets hidden,
			# we need to store them
			absX, absY = self.getAbsolutePos()
			
			# Remove from parent widget
			if self.dockarea is not None:
				# Use dockareas undock method
				self.dockarea.undockChild(self, True)
				self.dockarea = None

			elif self.parent is not None:
				# Force removal
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
		
	def mousePressed(self, event):
		if self.resizable is False:
			return
		
		ResizableBase.mousePressed(self, event)
		if self._rLeft or self._rRight or self._rTop or self._rBottom:
			self._movable = self.real_widget.isMovable()
			self.real_widget.setMovable(False) # Don't let guichan move window while we resize
			
	def mouseDragged(self, event):
		if self._resize is False and self.isDocked() is False:
			mouseX = self.x+event.getX()
			mouseY = self.y+event.getY()
			self._editor.getDockAreaAt(mouseX, mouseY, True)
		else:
			ResizableBase.mouseDragged(self, event)
	
	def mouseReleased(self, event):
		# Resize/move done
		self.real_widget.setMovable(self._movable)
		
		if self._resize:
			ResizableBase.mouseReleased(self, event)
		elif self._movable:
			mouseX = self.x+event.getX()
			mouseY = self.y+event.getY()
		
			dockArea = self._editor.getDockAreaAt(mouseX, mouseY)
			if dockArea is not None:
				self._editor.dockWidgetTo(self, dockArea, mouseX, mouseY)
		
	def hide(self):
		if self.isDocked():
			self.setDocked(False)
		widgets.Window.hide(self)
		
	def show(self):
		if self.isDocked():
			return
		widgets.Window.show(self)
				
	
# Register widget to pychan
if 'Panel' not in widgets.WIDGETS:
	widgets.WIDGETS['Panel'] = Panel
		