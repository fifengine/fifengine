import basicapplication
from events import EventMapper
import pychan
from gui import MenuBar, ToolBar, Toolbox, StatusBar
import loaders
from mapview import MapView
from gui.action import Action, ActionGroup

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
		
		super(Editor,self).__init__(*args, **kwargs)
		pychan.init(self.engine, debug=False)
		
	def _initGui(self):
		screen_width = self.engine.getSettings().getScreenWidth()
		bar_height = 30
		self._statusbar = StatusBar(min_size=(screen_width, bar_height))
		self._toolbar = ToolBar(min_size=(screen_width, bar_height), position=(0, bar_height))
		self._menubar = MenuBar(min_size=(screen_width, bar_height), position=(0, 0))
		self._toolbox = Toolbox(min_size=(50, 150), position=(50, 150))
		self._toolbox.position_technique = "explicit"
		self._statusbar.position_technique = "left:bottom"
		
		testAction1 = Action(u"TestAction1", "gui/icons/hand.png")
		self.getEventMapper().capture("Editor1", "activated", self._actionActivated, sender=testAction1)
		self._toolbar.addAction(testAction1)
		
		testAction2 = Action(u"TestAction2", "gui/icons/quit.png")
		self.getEventMapper().capture("Editor2", "activated", self._actionActivated2, sender=testAction2)
		self._toolbar.addAction(testAction2)
		self.testAction = testAction2
		
		testAction4 = Action(u"Separator")
		testAction4.setCheckable(True)
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
		
		actionGroup = ActionGroup()
		toggle1 = Action(u"ToggleGroup1", "gui/icons/add_instance.png")
		toggle1.setCheckable(True)
		actionGroup.addAction(toggle1)
		
		toggle2 = Action(u"ToggleGroup2", "gui/icons/add_instance.png")
		toggle2.setCheckable(True)
		actionGroup.addAction(toggle2)
		
		toggle3 = Action(u"ToggleGroup3", "gui/icons/add_instance.png")
		toggle3.setCheckable(True)
		actionGroup.addAction(toggle3)
		
		self._toolbar.addActionGroup(actionGroup)
		
		self._menubar.show()
		self._statusbar.show()
		self._toolbar.show()
		self._toolbox.show()
	
	def _actionActivated(self, sender):
		print "Action activated - cycling buttonstyle:", sender
		self._toolbar.button_style += 1
		
	def _actionActivated2(self, sender):
		print "Action activated - removing item:", sender
		if self.testAction is not None:
			self._toolbar.removeAction(self.testAction)
			self.testAction = None
	
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
		pass
		
	def _onQuit(self):
		print "Quitting already?"
		self.quit()
