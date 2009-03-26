import basicapplication
from events import EventMapper

def getEditor():
	if Editor.editor is None:
		Editor(None)
	return Editor.editor

class Editor(basicapplication.ApplicationBase):
	editor = None

	def __init__(self, params):
		super(Editor,self).__init__()

		self.params = params
		self._eventmapper = None
		
		self.editor = self
	
	def getStatusBar(self): 
		return None
		
	def getMenuBar(self):
		return None
	
	def getToolBar(self): 
		return None
	
	def getToolbox(self): 
		return None
	
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
