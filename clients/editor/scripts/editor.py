from basicapplication import ApplicationBase
import pychan
import fife
import loaders
import events
import plugin
from events import EventListener
from gui import ToolBar, action
from mapview import MapView
from gui.action import Action, ActionGroup
from gui.filemanager import FileManager
from gui.mainwindow import MainWindow

def getEditor():
	if Editor.editor is None:
		Editor(None)
	return Editor.editor

class Editor(ApplicationBase, MainWindow):
	editor = None

	def __init__(self, params, *args, **kwargs):
		Editor.editor = self
	
		self._filemanager = None
	
		self.params = params
		self._eventlistener = None
		self._pluginmanager = None
		
		self._inited = False
		
		self._mapview = None
		self._mapviewList = []
		self._mapgroup = None
		self._mapbar = None
	
		ApplicationBase.__init__(self, *args, **kwargs)
		MainWindow.__init__(self, *args, **kwargs)
		pychan.init(self.engine, debug=False)
		
	def _initTools(self):
		self._pluginmanager = plugin.PluginManager()
		
		self._filemanager = FileManager()
		self._toolbar.adaptLayout()
		
	def _initGui(self):
		screen_width = self.engine.getSettings().getScreenWidth()
		screen_height = self.engine.getSettings().getScreenHeight()
		bar_height = 30
		MainWindow.initGui(self, screen_width, screen_height)

		self._toolbox = ToolBar(title=u"", orientation=1, panel_size=bar_height)
		self._toolbox.position_technique = "explicit"
		self._toolbox.position = (150, 150)
		
		self._mapbar = ToolBar(panel_size=20)
		self._mapbar.setDocked(True)
		
		self._centralwidget.addChild(self._mapbar)
		
		self._initActions()
		
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
		action.activated.connect(self._actionUndock, sender=dockFloat)
		dockGroup.addAction(dockFloat)
		
		self._toolbar.addAction(dockGroup)
		self._toolbox.addAction(dockGroup)
		
		self._mapgroup = ActionGroup(exclusive=True, name="Mapgroup")
		self._mapbar.addAction(self._mapgroup)
		self._mapbar.addAction(ActionGroup(exclusive=True, name="Mapgroup2"))
	
	def _actionActivated(self, sender):
		self._toolbar.button_style += 1
		
	def _actionActivated2(self, sender):
		if self.testAction is not None:
			self._toolbar.removeAction(self.testAction)
			self.testAction = None
			self._toolbar.adaptLayout()
			
	def _actionDockLeft(self, sender):
		self._toolbar.setDocked(sender.isChecked())
		if sender.isChecked():
			self._toolbar.orientation = 1
			self.dockWidgetTo(self._toolbar, "left")
		
	def _actionDockRight(self, sender):
		self._toolbar.setDocked(sender.isChecked())
		if sender.isChecked():
			self._toolbar.orientation = 1
			self.dockWidgetTo(self._toolbar, "right")
		
	def _actionDockTop(self, sender):
		self._toolbar.setDocked(sender.isChecked())
		if sender.isChecked():
			self._toolbar.orientation = 0
			self.dockWidgetTo(self._toolbar, "top")
		
	def _actionDockBottom(self, sender):
		self._toolbar.setDocked(sender.isChecked())
		if sender.isChecked():
			self._toolbar.orientation = 0
			self.dockWidgetTo(self._toolbar, "bottom")
	
	def _actionUndock(self, sender):
		self._toolbar.setDocked(False)
			
	def getToolbox(self): 
		return self._toolbox
	
	def getPluginManager(self): 
		return self._pluginmanager
		
	def getEngine(self): 
		return self.engine

	def getMapViews(self):
		return self._mapviewList
		
	def getActiveMapView(self):
		return self._mapview
		
	def showMapView(self, mapview):
		if mapview is None:
			return
		self._mapview = mapview

	def createListener(self):
		if self._eventlistener is None:
			self._eventlistener = EventListener(self.engine)
			events.onQuit.connect(self.quit)
		
		return self._eventlistener
	
	def newMapView(self, map):
		self._mapview = MapView(map)
		
		self._mapviewList.append(self._mapview)
		
		mapAction = Action(unicode(map.getId()))
		action.activated.connect(self._mapview.show, sender=mapAction)
		self._mapgroup.addAction(mapAction)
		
		self._mapview.show()
		
		events.mapAdded.send(sender=self, map=map)
		
		return self._mapview
	
	def openFile(self, path):
		map = loaders.loadMapFile(path, self.engine)

		return self.newMapView(map)
	
	def saveAll(self):
		tmpView = self._mapview
		for mapView in self._mapviewList:
			self._mapview = mapView
			self._filemanager.save()
			print self._mapview
		self._mapview = tmpView

	def _pump(self):
		# ApplicationBase and Engine should be done initializing at this point
		if self._inited == False:
			self._initGui()
			self._initTools()
			self._inited = True
		
		events.onPump.send(sender=self)

		