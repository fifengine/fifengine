import filebrowser
import loaders, savers
import action
from action import Action, ActionGroup
import scripts.editor

class FileManager(object):
	def __init__(self):
		self.editor = scripts.editor.getEditor()
		
		self._filebrowser = None

		newAction = Action(u"New map", "gui/icons/new_map.png")
		loadAction = Action(u"Load", "gui/icons/load_map.png")
		saveAction = Action(u"Save", "gui/icons/save_map.png")
		saveAllAction = Action(u"Save all", "gui/icons/select_layer.png")
		
		newAction.helptext = u"(Not implemented) Create new map"
		loadAction.helptext = u"Open map"
		saveAction.helptext = u"(Not implemented) Save map"
		saveAllAction.helptext = u"(Not implemented) Save all maps"
		
		#action.activated.connect(self._actionActivated2, sender=testAction2)
		action.activated.connect(self.showLoadDialog, sender=loadAction)
		#action.activated.connect(self._actionActivated2, sender=testAction2)
		#action.activated.connect(self._actionActivated2, sender=testAction2)
		#action.activated.connect(self._actionActivated2, sender=testAction2)
		
		fileGroup = ActionGroup()
		fileGroup.addAction(newAction)
		fileGroup.addAction(loadAction)
		fileGroup.addAction(saveAction)
		fileGroup.addAction(saveAllAction)
		
		self.editor.getToolBar().insertActionGroup(fileGroup, 0)

		self.newMap = None
		self.saveRequested = False
		self._location = None
		self.path = '.'

	def showLoadDialog(self):
		filebrowser.FileBrowser(self.editor.getEngine(), self.loadFile, extensions = loaders.fileExtensions).showBrowser()
		
	def showSaveDialog(self):
		pass

	def loadFile(self, path, filename):
		self.editor.openFile('/'.join([path, filename]))
"""
	def save(self):
		self.saveRequested = True
	
	def saveMap(self, map, importList):
		pass
		curname = None
		try:
			curname = map.getResourceLocation().getFilename()
		except RuntimeError:
			pass # no name set for map yet
			
		if self._location:
			fname = '/'.join([self.path, self._location])
			saveMapFile(fname, self.engine, map, importList)
			print "map saved as " + fname
			self._location = None
		elif curname:
			saveMapFile(curname, self.engine, map, importList)
			print "map saved with old name " + curname
		else:
			print 'MapSaver: error, no file location specified.'

	def _selectFile(self,path,filename):
		self._location = filename
		self.path = path
		self.saveRequested = True
"""