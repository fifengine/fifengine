import pychan
from menubar import MenuBar, Menu
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
			
	def initGui(self, screen_width, screen_height):
		bar_height = 30
		
		self._statusbar = StatusBar(text=u"", panel_size=bar_height)
		self._toolbar = ToolBar(title=u"Toolbar", button_style=0)
		self._menubar = MenuBar(min_size=(screen_width, bar_height), position=(0, 0))
		
		# Set up root widget. This
		self._rootwidget = pychan.widgets.VBox(padding=0, vexpand=1, hexpand=1)
		self._rootwidget.min_size = \
		self._rootwidget.max_size = (screen_width, screen_height)
		self._rootwidget.opaque = False
		
		# These VBoxes should be replaced with panels, once that widget has been written
		self._dockareas[DOCKAREA['left']] = pychan.widgets.VBox(margins=(0,0,0,0))
		self._dockareas[DOCKAREA['right']] = pychan.widgets.VBox(margins=(0,0,0,0))
		self._dockareas[DOCKAREA['top']] = pychan.widgets.HBox(margins=(0,0,0,0))
		self._dockareas[DOCKAREA['bottom']] = pychan.widgets.HBox(margins=(0,0,0,0))
		
		# This is where the map will be displayed
		self._centralwidget = pychan.widgets.VBox(vexpand=1, hexpand=1)
		self._centralwidget.opaque = False
		
		middle = pychan.widgets.HBox(padding=0, vexpand=1, hexpand=1)
		middle.opaque = False
		
		# Pychan bug? Adding a spacer instead of a container creates
		# a gap after the right dockarea
		middle.addChild(self._dockareas['left'])
		middle.addChild(self._centralwidget)
		#middle.addSpacer(pychan.widgets.Spacer())
		middle.addChild(self._dockareas['right'])
		
		self._rootwidget.addChild(self._menubar)
		#self._rootwidget.addChild(self._toolbar)
		self._rootwidget.addChild(self._dockareas['top'])
		self._rootwidget.addChild(middle)
		self._rootwidget.addChild(self._dockareas['bottom'])
		self._rootwidget.addChild(self._statusbar)

		self._toolbar.setDocked(True)
		self.dockToolbarTo(self._toolbar, "top")
		
		self._rootwidget.show()
		
	def getCentralWidget(self):
		return self._centralwidget
	
	def getStatusBar(self): 
		return self._statusbar
		
	def getMenuBar(self):
		return self._menubar
	
	def getToolBar(self): 
		return self._toolbar
	
	def dockToolbarTo(self, toolbar, dockarea):
		if isinstance(toolbar, ToolBar) is False:
			print "Argument is not a valid toolbar"
			return
			
		if toolbar.parent:
			widgetParent = toolbar.parent
			widgetParent.removeChild(toolbar)
			widgetParent.adaptLayout()
			
		# We must hide the widget before adding it to the dockarea,
		# or we will get a duplicate copy of the widget in the top left corner
		# of screen.
		toolbar.hide() 
	
		if dockarea == DOCKAREA['left']:
			toolbar.setDocked(True)
			toolbar.setOrientation(ToolBar.ORIENTATION["Vertical"])
			self._dockareas[DOCKAREA['left']].addChild(toolbar)
			self._dockareas[DOCKAREA['left']].adaptLayout()
			
		elif dockarea == DOCKAREA['right']:
			toolbar.setDocked(True)
			toolbar.setOrientation(ToolBar.ORIENTATION["Vertical"])
			self._dockareas[DOCKAREA['right']].addChild(toolbar)
			self._dockareas[DOCKAREA['right']].adaptLayout()
			
		elif dockarea == DOCKAREA['top']:
			toolbar.setDocked(True)
			toolbar.setOrientation(ToolBar.ORIENTATION["Horizontal"])
			self._dockareas[DOCKAREA['top']].addChild(toolbar)
			self._dockareas[DOCKAREA['top']].adaptLayout()
			
		elif dockarea == DOCKAREA['bottom']:
			toolbar.setDocked(True)
			toolbar.setOrientation(ToolBar.ORIENTATION["Horizontal"])
			self._dockareas[DOCKAREA['bottom']].addChild(toolbar)
			self._dockareas[DOCKAREA['bottom']].adaptLayout()
			
		else:
			print "Invalid dockarea"
