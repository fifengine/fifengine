import basicapplication
from events import EventMapper
import pychan
from gui import MenuBar, ToolBar, Toolbox, StatusBar

def getEditor():
	if Editor.editor is None:
		Editor(None)
	return Editor.editor

class Editor(basicapplication.ApplicationBase):
	editor = None

	def __init__(self, params, *args, **kwargs):
		super(Editor,self).__init__(*args, **kwargs)

		self.params = params
		self._eventmapper = None
		
		self.editor = self
		
		pychan.init(self.engine, debug=False)
		
		self._initGui()
		
	def _initGui(self):
		bar_height = 30
		screen_width = self.engine.getSettings().getScreenWidth()
		screen_height = self.engine.getSettings().getScreenHeight()
		self._statusbar = StatusBar(min_size=(screen_width, bar_height), position=(0, screen_height-bar_height))
		self._toolbar = ToolBar(min_size=(screen_width, bar_height), position=(0, bar_height))
		self._menubar = MenuBar(min_size=(screen_width, bar_height), position=(0, 0))
		self._toolbox = Toolbox(min_size=(50, 150), position=(150, 300))
		
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
		
	def getUndoManagerList(self): 
		return []
	
	def getUndoManager(self, name): 
		return None
	
	def removeUndoManager(self, name): 
		pass

	def getEventListener(self):
		return self._eventmapper

	def createListener(self):
		if self._eventmapper is None:
			self._eventmapper = EventMapper(self.engine)
			self._eventmapper.capture("Editor", "onQuit", self._onQuit)
		return self._eventmapper

	def _pump(self):
		pass
		
	def _onQuit(self):
		print "Quitting already?"
		self.quit()
