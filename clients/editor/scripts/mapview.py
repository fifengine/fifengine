import fife
import undomanager
import editor
import loaders, savers
from events import events
from mapcontroller import MapController

class MapView:
	def __init__(self, map):
		self._map = map
		self._editor = editor.getEditor()
		self._controller = MapController(self._map)
		
		if map is None:
			print "No map passed to MapView!"
		else:
			if not self._map.getLayers():
				raise AttributeError('Editor error: map ' + self._map.getId() + ' has no layers. Cannot edit.')

		self._undomanager = undomanager.UndoManager()
		self.importlist = []
		if hasattr(map, "importDirs"):
			self.importlist.extend(map.importDirs)
		
	# Copied from mapeditor.py
	def show(self):
		_camera = None
		
		engine = self._editor.getEngine()
		engine.getView().resetRenderers()
		for cam in engine.getView().getCameras():
			cam.setEnabled(False)

		for cam in engine.getView().getCameras():
			if cam.getLocationRef().getMap().getId() == self._map.getId():
				rb = engine.getRenderBackend()
				cam.setViewPort(fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight()))
				cam.setEnabled(True)
				self._camera = cam
				break
		else:
			raise AttributeError('No cameras found associated with this map: ' + self._map.getId())

	def getCamera(self):
		return self._camera
	
	def getController(self):
		return self._controller
		
	def getMap(self):
		return self._map
		
	def getUndoManager(self):
		return self._undomanager
		
	def undo(self):
		self._undomanager.undo()
		
	def redo(self):
		self._undomanager.redo()
	
	def save(self):
		curname = ""
		try:
			curname = self._map.getResourceLocation().getFilename()
		except RuntimeError:
			print "Map has no filename yet, can't save."
			return

		events.preSave.send(sender=self, mapview=self)
		savers.saveMapFile(curname, self._editor.getEngine(), self._map, importList=self.importlist)
		events.postSave.send(sender=self, mapview=self)
		
	def saveAs(self, filename):
		events.preSave.send(sender=self, mapview=self)
		savers.saveMapFile(str(filename), self._editor.getEngine(), self._map, importList=self.importlist)
		events.postSave.send(sender=self, mapview=self)
		
	def close(self):
		pass
		
		
	def importFile(self, path):
		loaders.loadImportFile(path, self._editor.getEngine())
		
	def importDir(self, path, recursive=True):
		self.importlist.append(path)
		if recursive is True:
			loaders.loadImportDirRec(path, self._editor.getEngine())
		else:
			loaders.loadImportDir(path, self._editor.getEngine())
	