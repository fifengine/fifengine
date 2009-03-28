import fife
import undomanager
import editor

class MapView:
	def __init__(self, map):
		self._map = map
		
		if map is None:
			print "No map passed to MapView!"
		else:
			if not self._map.getLayers():
				raise AttributeError('Editor error: map ' + self._map.getId() + ' has no layers. Cannot edit.')

			self._selectDefaultCamera(self._map)
		
		self._undomanager = undomanager.UndoManager()
		
	# Copied from mapeditor.py
	def _selectDefaultCamera(self, map):
		_camera = None
		
		engine = editor.getEditor().getEngine()
		engine.getView().resetRenderers()
		for cam in engine.getView().getCameras():
			cam.setEnabled(False)

		for cam in engine.getView().getCameras():
			if cam.getLocationRef().getMap().getId() == map.getId():
				rb = engine.getRenderBackend()
				cam.setViewPort(fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight()))
				cam.setEnabled(True)
				_camera = cam
				break
		if not _camera:
			raise AttributeError('No cameras found associated with this map: ' + map.getId())

		
	def getMap(self):
		return self._map
		
	def getUndoManager(self):
		return self._undomanager
		
	def undo(self):
		self._undomanager.undo()
		
	def redo(self):
		self._undomanager.redo()
	
	def save(self):
		pass
		
	def saveAs(self):
		pass
		
	def close(self):
		pass
		
	