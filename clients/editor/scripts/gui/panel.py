import pychan
from pychan import widgets
import scripts.editor
import fife

class Panel(widgets.Window):
	def __init__(self, *args, **kwargs):
		super(Panel, self).__init__(*args, **kwargs)
		self.capture(self.mouseEntered, "mouseEntered", "widget")
		self.capture(self.mouseExited, "mouseExited", "widget")
		self.capture(self.mouseMoved, "mouseMoved", "widget")
		self.capture(self.mouseDragged, "mouseDragged", "widget")
		self.capture(self.mousePressed, "mousePressed", "widget")
		self.capture(self.mouseReleased, "mouseReleased", "widget")
		
		self.engine = scripts.editor.getEditor().getEngine()
		
		self.cursor_id = 0
		self.cursor_type = 0
		
		self.resize = False
		self.real_widget.setMovable(False) # Don't let guichan move window while we resize
		
		self.dragoffset = (0, 0)
		
	def mouseEntered(self, event):
		# Save cursor id
		if self.resize is False:
			cursor = self.engine.getCursor()
			self.cursor_id = cursor.getId()
			self.cursor_type = cursor.getType()
		
	def mouseMoved(self, event):
		cursor = self.engine.getCursor()
		
		left	= event.getX() < 5
		right	= event.getX() > self.width-5
		top		= event.getY() < 5
		bottom	= event.getY() - self.titlebar_height > self.height-5
		
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
		if self.resize is False:
			cursor = self.engine.getCursor()
			cursor.set(self.cursor_type, self.cursor_id)
		
	def mouseDragged(self, event):
		if self.resize:
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
		self.left	= event.getX() < 5
		self.right	= event.getX() > self.width-5
		self.top	= event.getY() < 5
		self.bottom	= event.getY() - self.titlebar_height > self.height-5
		
		if self.left or self.right or self.top or self.bottom:
			self.resize = True
			self.min_size = (10, 10)
			self.max_size = (5000, 5000)
		else:
			# Let guichan take care of moving the window for us
			self.real_widget.setMovable(True)
			self.resize = False
		
	def mouseReleased(self, event):
		# Resize/move done
		self.real_widget.setMovable(False)
		
		if self.resize:
			self.min_size = (self.width, self.height)
			self.max_size = (self.width, self.height)
			self.adaptLayout()
		
		self.resize = False
		if event.getX() < 0 or event.getX() >= self.width \
				or event.getY() < 0 or event.getY() >= self.height+self.titlebar_height:
			self.mouseExited(event)
	
# Register widget to pychan
if 'Panel' not in widgets.WIDGETS:
	widgets.WIDGETS['Panel'] = Panel
		