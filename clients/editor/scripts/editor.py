import basicapplication
from events import EventMapper
import pychan
from gui import MenuBar, ToolBar, Toolbox, StatusBar
import loaders
from mapview import MapView
from gui.action import Action, ActionGroup
import fife

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
	
		self.params = params
		self._eventmapper = None
		
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
		
	def _initGui(self):
		screen_width = self.engine.getSettings().getScreenWidth()
		screen_height = self.engine.getSettings().getScreenHeight()
		bar_height = 30
		self._statusbar = StatusBar(min_size=(screen_width, bar_height))
		self._toolbar = ToolBar(button_style=3)
		self._menubar = MenuBar(min_size=(screen_width, bar_height), position=(0, 0))
		self._toolbox = Toolbox(min_size=(50, 150), position=(150, 150))
		self._toolbox.position_technique = "explicit"
		
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
		self.getEventMapper().capture("Editor1", "activated", self._actionActivated, sender=testAction1)
		self._toolbar.addAction(testAction1)
		
		testAction2 = Action(u"Remove action", "gui/icons/zoom_out.png")
		self.getEventMapper().capture("Editor2", "activated", self._actionActivated2, sender=testAction2)
		self._toolbar.addAction(testAction2)
		self.testAction = testAction2
		
		testAction4 = Action(u"Separator")
		testAction4.setCheckable(True)
		testAction4.setSeparator(True)
		self._toolbar.addAction(testAction4)
		
		testAction3 = Action(u"CheckAble1", "gui/icons/eraser.png")
		testAction3.setCheckable(True)
		self._toolbar.addAction(testAction3)
		
		testAction5 = Action(u"CheckAble2", "gui/icons/eraser.png")
		testAction5.setCheckable(True)
		self._toolbar.insertAction(testAction5, 3)
		
		testAction6 = Action(u"CheckAble3", "gui/icons/eraser.png")
		testAction6.setCheckable(True)
		self._toolbar.insertAction(testAction6, 0, before=testAction5)
		
		self._toolbar.addSeparator()
		
		dockGroup = ActionGroup(exclusive=True)
		
		dockTop = Action(u"Top", "gui/icons/select_layer.png")
		dockTop.setCheckable(True)
		dockTop.setChecked(True)
		self._eventmapper.capture("docking1", "activated", self._actionDockTop, sender=dockTop)
		dockGroup.addAction(dockTop)
		
		dockRight = Action(u"Right", "gui/icons/select_layer.png")
		dockRight.setCheckable(True)
		self._eventmapper.capture("docking2", "activated", self._actionDockRight, sender=dockRight)
		dockGroup.addAction(dockRight)
		
		dockBottom = Action(u"Bottom", "gui/icons/select_layer.png")
		dockBottom.setCheckable(True)
		self._eventmapper.capture("docking3", "activated", self._actionDockBottom, sender=dockBottom)
		dockGroup.addAction(dockBottom)
		
		dockLeft = Action(u"Left", "gui/icons/select_layer.png")
		dockLeft.setCheckable(True)
		self._eventmapper.capture("docking4", "activated", self._actionDockLeft, sender=dockLeft)
		dockGroup.addAction(dockLeft)
		
		dockFloat = Action(u"Float", "gui/icons/delete_layer.png")
		self._eventmapper.capture("docking5", "activated", self._toolbar.unDock, sender=dockFloat)
		dockGroup.addAction(dockFloat)
		
		self._toolbar.addActionGroup(dockGroup)
	
	def _actionActivated(self, sender):
		print "Action activated - cycling buttonstyle:", sender
		self._toolbar.button_style += 1
		
	def _actionActivated2(self, sender):
		print "Action activated - removing item:", sender
		if self.testAction is not None:
			self._toolbar.removeAction(self.testAction)
			self.testAction = None
			
	def _actionDockLeft(self, sender):
		if sender.isChecked() is False: self._toolbar.unDock()
		self._toolbar.dockTo("left")
		
	def _actionDockRight(self, sender):
		if sender.isChecked() is False: self._toolbar.unDock()
		self._toolbar.dockTo("right")
		
	def _actionDockTop(self, sender):
		if sender.isChecked() is False: self._toolbar.unDock()
		self._toolbar.dockTo("top")
		
	def _actionDockBottom(self, sender):
		if sender.isChecked() is False: self._toolbar.unDock()
		self._toolbar.dockTo("bottom")
			
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

	def getEventMapper(self):
		return self._eventmapper

	def createListener(self):
		if self._eventmapper is None:
			self._eventmapper = EventMapper(self.engine)
			self._eventmapper.capture("Editor", "onQuit", self._onQuit)
		
		return self._eventmapper
		
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
		self._eventmapper.mapAdded(map)
		self._mapviewList.append(self._mapview)
		
		return self._mapview
	
	def saveAll(self):
		for mapView in self._mapviews:
			mapView.save()

	def _pump(self):
		if self._inited == False:
			self._initGui()
			self._inited = True
			
		self._eventmapper.pump()
		
	def _onQuit(self):
		print "Quitting already?"
		self.quit()
