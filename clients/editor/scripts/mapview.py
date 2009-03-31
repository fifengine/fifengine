import fife
import undomanager
import editor
import loaders, savers

class MapView:
	def __init__(self, map):
		self._map = map
		self.editor = editor.getEditor()
		
		if map is None:
			print "No map passed to MapView!"
		else:
			if not self._map.getLayers():
				raise AttributeError('Editor error: map ' + self._map.getId() + ' has no layers. Cannot edit.')

		self._undomanager = undomanager.UndoManager()
		
	# Copied from mapeditor.py
	def show(self):
		_camera = None
		
		engine = self.editor.getEngine()
		engine.getView().resetRenderers()
		for cam in engine.getView().getCameras():
			cam.setEnabled(False)

		for cam in engine.getView().getCameras():
			if cam.getLocationRef().getMap().getId() == self._map.getId():
				rb = engine.getRenderBackend()
				cam.setViewPort(fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight()))
				cam.setEnabled(True)
				_camera = cam
				break
		if not _camera:
			raise AttributeError('No cameras found associated with this map: ' + self._map.getId())

		
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
		
		importlist = self.editor.importlist
		savers.saveMapFile(curname, self.editor.getEngine(), self._map, importList=importlist)
		
	def saveAs(self, filename):
		importlist = self.editor.importlist
		savers.saveMapFile(str(filename), self.editor.getEngine(), self._map, importList=importlist)
		
	def close(self):
		pass
		
	