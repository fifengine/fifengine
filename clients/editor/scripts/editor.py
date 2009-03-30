import basicapplication
import pychan
import fife
import loaders
import events
from events import EventListener
from gui import MenuBar, ToolBar, StatusBar
from mapview import MapView
from gui import action
from gui.action import Action, ActionGroup

from gui.filemanager import FileManager

DOCK_AREA = {
		'left'	: 'left',
		'right' : 'right',
		'top'	: 'top',
		'bottom': 'bottom'
	}

def getEditor():
	if Editor.editor is None:
		Editor(None)
	return Editor.editor

class Editor(basicapplication.ApplicationBase):
	editor = None

	def __init__(self, params, *args, **kwargs):
		Editor.editor = self
	
		self._filemanager = None
	
		self.params = params
		self._eventlistener = None
		
		self._inited = False
		
		self._mapview = None
		self._mapviewList = []
		
		self._rootwidget = None
		self._central_widget = None
		self._dockareas = {
				DOCK_AREA['left']:None, 
				DOCK_AREA['right']:None, 
				DOCK_AREA['top']:None, 
				DOCK_AREA['bottom']:None
			}
		
		super(Editor,self).__init__(*args, **kwargs)
		pychan.init(self.engine, debug=False)
		
	def _initTools(self):
		self._filemanager = FileManager()
		
	def _initGui(self):
		screen_width = self.engine.getSettings().getScreenWidth()
		screen_height = self.engine.getSettings().getScreenHeight()
		bar_height = 30
		self._statusbar = StatusBar(text=u"", panel_size=bar_height)
		self._toolbar = ToolBar(button_style=3)
		self._menubar = MenuBar(min_size=(screen_width, bar_height), position=(0, 0))
		self._toolbox = ToolBar(title=u"", orientation=1, panel_size=bar_height)
		self._toolbox.position_technique = "explicit"
		self._toolbox.position = (150, 150)
		
		# Set up root widget. This
		self._rootwidget = pychan.widgets.VBox(padding=0, vexpand=1, hexpand=1)
		self._rootwidget.min_size = \
		self._rootwidget.max_size = (screen_width, screen_height)
		self._rootwidget.base_color = fife.Color(0,0,0,0) # Transparent
		
		# These VBoxes should be replaced with panels, once that widget has been written
		self._dockareas[DOCK_AREA['left']] = pychan.widgets.VBox(margins=(0,0,0,0))
		self._dockareas[DOCK_AREA['right']] = pychan.widgets.VBox(margins=(0,0,0,0))
		self._dockareas[DOCK_AREA['top']] = pychan.widgets.HBox(margins=(0,0,0,0))
		self._dockareas[DOCK_AREA['bottom']] = pychan.widgets.HBox(margins=(0,0,0,0))
		
		# This is where the map will be displayed
		self._centralwidget = pychan.widgets.VBox(vexpand=1, hexpand=1)
		self._centralwidget.addChild(pychan.widgets.Label(text=u"Central widget: Map"))
		self._centralwidget.base_color = fife.Color(0,0,100,50) # Transparent
		
		self._initActions()
		
		middle = pychan.widgets.HBox(padding=0, vexpand=1, hexpand=1)
		middle.base_color = fife.Color(0,0,0,0) # Transparent
		
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

		self._toolbar.dockTo(DOCK_AREA['top'])
		
		self._rootwidget.show()
		
		self._toolbox.show()

	def _initActions(self):
		testAction1 = Action(u"Cycle buttonstyles", "gui/icons/zoom_in.png")
		testAction1.helptext = u"Cycles button styles. There are currently four button styles."
		action.activated.connect(self._actionActivated, sender=testAction1)
		self._toolbar.addAction(testAction1)
		
		testAction2 = Action(u"Remove action", "gui/icons/zoom_out.png")
		testAction2.helptext = u"Removes the action from the active toolbar/menubar"
		action.activated.connect(self._actionActivated2, sender=testAction2)
		self._toolbar.addAction(testAction2)
		self.testAction = testAction2
		
		testAction4 = Action(u"Separator")
		testAction4.setCheckable(True)
		testAction4.setSeparator(True)
		self._toolbar.addAction(testAction4)
		
		testAction3 = Action(u"CheckAble1", "gui/icons/eraser.png")
		testAction3.helptext = u"A simple action to showcase toggleable actions."
		testAction3.setCheckable(True)
		self._toolbar.addAction(testAction3)
		
		testAction5 = Action(u"CheckAble2", "gui/icons/eraser.png")
		testAction5.helptext = u"A simple action to showcase toggleable actions."
		testAction5.setCheckable(True)
		self._toolbar.insertAction(testAction5, 3)
		
		testAction6 = Action(u"CheckAble3", "gui/icons/eraser.png")
		testAction6.helptext = u"A simple action to showcase toggleable actions."
		testAction6.setCheckable(True)
		self._toolbar.insertAction(testAction6, 0, before=testAction5)
		
		self._toolbar.addSeparator()
		
		dockGroup = ActionGroup(exclusive=True)
		
		dockTop = Action(u"Top", "gui/icons/select_layer.png")
		dockTop.helptext = u"Dock the toolbar to the top dock area."
		dockTop.setCheckable(True)
		dockTop.setChecked(True)
		action.activated.connect(self._actionDockTop, sender=dockTop)
		dockGroup.addAction(dockTop)
		
		dockRight = Action(u"Right", "gui/icons/select_layer.png")
		dockRight.helptext = u"Dock the toolbar to the right dock area."
		dockRight.setCheckable(True)
		action.activated.connect(self._actionDockRight, sender=dockRight)
		dockGroup.addAction(dockRight)
		
		dockBottom = Action(u"Bottom", "gui/icons/select_layer.png")
		dockBottom.helptext = u"Dock the toolbar to the bottom dock area."
		dockBottom.setCheckable(True)
		action.activated.connect(self._actionDockBottom, sender=dockBottom)
		dockGroup.addAction(dockBottom)
		
		dockLeft = Action(u"Left", "gui/icons/select_layer.png")
		dockLeft.helptext = u"Dock the toolbar to the left dock area."
		dockLeft.setCheckable(True)
		action.activated.connect(self._actionDockLeft, sender=dockLeft)
		dockGroup.addAction(dockLeft)
		
		dockFloat = Action(u"Float", "gui/icons/delete_layer.png")
		dockFloat.helptext = u"Undock the toolbar. Currently a little buggy."
		action.activated.connect(self._toolbar.unDock, sender=dockFloat)
		dockGroup.addAction(dockFloat)
		
		self._toolbar.addActionGroup(dockGroup)
		self._toolbox.addActionGroup(dockGroup)
	
	def _actionActivated(self, sender):
		self._toolbar.button_style += 1
		
	def _actionActivated2(self, sender):
		if self.testAction is not None:
			self._toolbar.removeAction(self.testAction)
			self.testAction = None
			
	def _actionDockLeft(self, sender):
		if sender.isChecked() is False: self._toolbar.unDock()
		else: self._toolbar.dockTo("left")
		
	def _actionDockRight(self, sender):
		if sender.isChecked() is False: self._toolbar.unDock()
		else: self._toolbar.dockTo("right")
		
	def _actionDockTop(self, sender):
		if sender.isChecked() is False: self._toolbar.unDock()
		else: self._toolbar.dockTo("top")
		
	def _actionDockBottom(self, sender):
		if sender.isChecked() is False: self._toolbar.unDock()
		else: self._toolbar.dockTo("bottom")
			
	def dockWidgetTo(self, widget, dockarea):
		if isinstance(widget, pychan.widgets.Widget) is False:
			print "Argument is not a valid widget"
			return
			
		if widget.parent:
			widgetParent = widget.parent
			widgetParent.removeChild(widget)
			widgetParent.adaptLayout()
			widget.hide()
	
		if dockarea == DOCK_AREA['left']:
			self._dockareas[DOCK_AREA['left']].addChild(widget)
			self._dockareas[DOCK_AREA['left']].adaptLayout()
			
		elif dockarea == DOCK_AREA['right']:
			self._dockareas[DOCK_AREA['right']].addChild(widget)
			self._dockareas[DOCK_AREA['right']].adaptLayout()
			
		elif dockarea == DOCK_AREA['top']:
			self._dockareas[DOCK_AREA['top']].addChild(widget)
			self._dockareas[DOCK_AREA['top']].adaptLayout()
			
		elif dockarea == DOCK_AREA['bottom']:
			self._dockareas[DOCK_AREA['bottom']].addChild(widget)
			self._dockareas[DOCK_AREA['bottom']].adaptLayout()
			
		else:
			print "Invalid dockarea"
	
	def getStatusBar(self): 
		return self._statusbar
		
	def getMenuBar(self):
		return self._menubar
	
	def getToolBar(self): 
		return self._toolbar
	
	def getToolbox(self): 
		return self._toolbox
	
	def getPluginManager(self): 
		return None
		
	def getEngine(self): 
		return self.engine

	def getMapViews():
		return self._mapviewList
		
	def getActiveMapView():
		return self._mapview

	def createListener(self):
		if self._eventlistener is None:
			self._eventlistener = EventListener(self.engine)
			events.onQuit.connect(self.quit)
		
		return self._eventlistener
		
	def importFile(self, path):
		loaders.loadImportFile(path, self.engine)
		
	def importDir(self, path, recursive=True):
		if recursive is True:
			loaders.loadImportDirRec(path, self.engine)
		else:
			loaders.loadImportDir(path, self.engine)
	
	def openFile(self, path):
		map = loaders.loadMapFile(path, self.engine)
		self._mapview = MapView(map)
		events.mapAdded.send(sender=self, map=map)
		self._mapviewList.append(self._mapview)
		
		return self._mapview
	
	def saveAll(self):
		for mapView in self._mapviews:
			mapView.save()

	def _pump(self):
		# ApplicationBase and Engine should be done initializing at this point
		if self._inited == False:
			self._initGui()
			self._initTools()
			self._inited = True

		