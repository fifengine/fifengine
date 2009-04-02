import pychan
from menubar import MenuBar
from toolbar import ToolBar
from statusbar import StatusBar

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
	
		self._rootwidget = None
		self._central_widget = None
		self._dockareas = {
				DOCKAREA['left']:None, 
				DOCKAREA['right']:None, 
				DOCKAREA['top']:None, 
				DOCKAREA['bottom']:None
			}
			
	def initGui(self, screen_width, screen_height):
		bar_height = 30
		
		self._statusbar = StatusBar(text=u"", panel_size=bar_height)
		self._toolbar = ToolBar(button_style=3)
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
		self.dockWidgetTo(self._toolbar, "top")
		
		self._rootwidget.show()
	
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
			widget.hide()
	
		if dockarea == DOCKAREA['left']:
			self._dockareas[DOCKAREA['left']].addChild(widget)
			self._dockareas[DOCKAREA['left']].adaptLayout()
			
		elif dockarea == DOCKAREA['right']:
			self._dockareas[DOCKAREA['right']].addChild(widget)
			self._dockareas[DOCKAREA['right']].adaptLayout()
			
		elif dockarea == DOCKAREA['top']:
			self._dockareas[DOCKAREA['top']].addChild(widget)
			self._dockareas[DOCKAREA['top']].adaptLayout()
			
		elif dockarea == DOCKAREA['bottom']:
			self._dockareas[DOCKAREA['bottom']].addChild(widget)
			self._dockareas[DOCKAREA['bottom']].adaptLayout()
			
		else:
			print "Invalid dockarea"
	