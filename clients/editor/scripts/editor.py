import basicapplication
from events import EventMapper
import pychan
from gui import MenuBar, ToolBar, Toolbox, StatusBar
import loaders
from mapview import MapView

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
		
		self._initGui()
		
	def _initGui(self):
		bar_height = 30
		screen_width = self.engine.getSettings().getScreenWidth()
		screen_height = self.engine.getSettings().getScreenHeight()
		self._statusbar = StatusBar(min_size=(screen_width, bar_height), position=(0, screen_height-bar_height))
		self._toolbar = ToolBar(min_size=(screen_width, bar_height), position=(0, bar_height))
		self._menubar = MenuBar(min_size=(screen_width, bar_height), position=(0, 0))
		self._toolbox = Toolbox(min_size=(50, 150), position=(50, 150))
		self._toolbox.position_technique = "explicit"
		
		self._menubar.show()
		self._statusbar.show()
		self._toolbar.show()
		self._toolbox.show()
	
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

	def getEventListener(self):
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
		if self._inited is False:
			# Load a map for testing
			self.openFile("../rio_de_hola/maps/shrine.xml")
			self._inited = True
		pass
		
	def _onQuit(self):
		print "Quitting already?"
		self.quit()
