import pychan
from menubar import MenuBar, Menu
from panel import Panel
from dockarea import DockArea
import toolbar
from toolbar import ToolBar
from statusbar import StatusBar
import fife

DOCKAREA = {
	'left'	: 'left',
	'right' : 'right',
	'top'	: 'top',
	'bottom': 'bottom'
}

class MainWindow(object):

	def __init__(self, *args, **kwargs):
		self._toolbar = None
		self._menubar = None
		self._statusbar = None
	
		self._rootwidget = None
		self._centralwidget = None
		self._dockareas = {
				DOCKAREA['left']:None, 
				DOCKAREA['right']:None, 
				DOCKAREA['top']:None, 
				DOCKAREA['bottom']:None
			}
			
		self._toolbarareas = {
				DOCKAREA['left']:None, 
				DOCKAREA['right']:None, 
				DOCKAREA['top']:None, 
				DOCKAREA['bottom']:None
			}
			
			
	def initGui(self, screen_width, screen_height):
		bar_height = 30
		
		self._statusbar = StatusBar(text=u"", panel_size=bar_height)
		self._toolbar = ToolBar(title=u"Toolbar", button_style=0)
		self._menubar = MenuBar(min_size=(screen_width, bar_height), position=(0, 0))
		
		# Set up root widget
		self._rootwidget = pychan.widgets.VBox(padding=0, vexpand=1, hexpand=1)
		self._rootwidget.min_size = \
		self._rootwidget.max_size = (screen_width, screen_height)
		self._rootwidget.opaque = False
		
		self._dockareas[DOCKAREA['left']] = DockArea("left")
		self._dockareas[DOCKAREA['right']] = DockArea("right")
		self._dockareas[DOCKAREA['top']] = DockArea("top")
		self._dockareas[DOCKAREA['bottom']] = DockArea("bottom")

		self._toolbarareas[DOCKAREA['left']] = pychan.widgets.VBox(margins=(0,0,0,0))
		self._toolbarareas[DOCKAREA['right']] = pychan.widgets.VBox(margins=(0,0,0,0))
		self._toolbarareas[DOCKAREA['top']] = pychan.widgets.HBox(margins=(0,0,0,0))
		self._toolbarareas[DOCKAREA['bottom']] = pychan.widgets.HBox(margins=(0,0,0,0))
		
		# This is where the map will be displayed
		self._centralwidget = pychan.widgets.VBox(vexpand=1, hexpand=1)
		self._centralwidget.opaque = False
		
		middle = pychan.widgets.HBox(padding=0, vexpand=1, hexpand=1)
		middle.opaque = False
		
		# Pychan bug? Adding a spacer instead of a container creates
		# a gap after the right dockarea
		middle.addChild(self._toolbarareas['left'])
		middle.addChild(self._dockareas['left'])
		middle.addChild(self._centralwidget)
		#middle.addSpacer(pychan.widgets.Spacer())
		middle.addChild(self._dockareas['right'])
		middle.addChild(self._toolbarareas['right'])
		
		self._rootwidget.addChild(self._menubar)
		#self._rootwidget.addChild(self._toolbar)
		self._rootwidget.addChild(self._toolbarareas['top'])
		self._rootwidget.addChild(self._dockareas['top'])
		self._rootwidget.addChild(middle)
		self._rootwidget.addChild(self._dockareas['bottom'])
		self._rootwidget.addChild(self._toolbarareas['bottom'])
		self._rootwidget.addChild(self._statusbar)

		self._toolbar.setDocked(True)
		self.dockWidgetTo(self._toolbar, "top")
		
		self._rootwidget.show()
		
	def getCentralWidget(self):
		return self._centralwidget
	
	def getStatusBar(self): 
		return self._statusbar
		
	def getMenuBar(self):
		return self._menubar
	
	def getToolBar(self): 
		return self._toolbar
	
	def dockWidgetTo(self, widget, dockarea):
		if isinstance(widget, pychan.widgets.Widget) is False:
			print "Argument is not a valid widget"
			return
			
		if widget.parent:
			widgetParent = widget.parent
			widgetParent.removeChild(widget)
			widgetParent.adaptLayout()
			
		# We must hide the widget before adding it to the dockarea,
		# or we will get a duplicate copy of the widget in the top left corner
		# of screen.
		widget.hide() 
		dockareas = self._dockareas
		if isinstance(widget, ToolBar):
			dockareas = self._toolbarareas
			if dockarea == DOCKAREA['left'] or dockarea == DOCKAREA['right']:
				widget.setOrientation(ToolBar.ORIENTATION["Vertical"])
			elif dockarea == DOCKAREA['top'] or dockarea == DOCKAREA['bottom']:
				widget.setOrientation(ToolBar.ORIENTATION["Horizontal"])
	
		if dockarea == DOCKAREA['left']:
			widget.resizable = False
			widget.setDocked(True)
			dockareas[DOCKAREA['left']].addChild(widget)
			dockareas[DOCKAREA['left']].adaptLayout()
			
		elif dockarea == DOCKAREA['right']:
			widget.resizable = False
			widget.setDocked(True)
			dockareas[DOCKAREA['right']].addChild(widget)
			dockareas[DOCKAREA['right']].adaptLayout()
			
		elif dockarea == DOCKAREA['top']:
			widget.setDocked(True)
			widget.resizable = False
			dockareas[DOCKAREA['top']].addChild(widget)
			dockareas[DOCKAREA['top']].adaptLayout()
			
		elif dockarea == DOCKAREA['bottom']:
			widget.setDocked(True)
			widget.resizable = False
			dockareas[DOCKAREA['bottom']].addChild(widget)
			dockareas[DOCKAREA['bottom']].adaptLayout()
			
		else:
			print "Invalid dockarea"

