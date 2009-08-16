import fife
import editor
import loaders, savers
from events import events
from mapcontroller import MapController

class MapView:
	""" MapView represents a map in the editor. 
	
	It handles opening, saving and closing of a map,
	as well as displaying it on screen.
	"""
	def __init__(self, map):
		self._map = map
		self._editor = editor.getEditor()
		self._controller = MapController(self._map)
		
		if map is None:
			print "No map passed to MapView!"
		else:
			if not self._map.getLayers():
				raise AttributeError('Editor error: map ' + self._map.getId() + ' has no layers. Cannot edit.')

		self.importlist = []
		if hasattr(map, "importDirs"):
			self.importlist.extend(map.importDirs)
		
	# Copied from mapeditor.py
	def show(self):
		""" Sets up the camera to display the map. Size of the camera is equal to the
		screen size """
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
		
	def setCamera(self, camera):
		if not camera:
			print "Camera can not be None"
			return
		
		if camera.getLocation().getLayer().getMap() != self._map:
			print "Camera is not associated with this map"
			return
			
		self._camera = None
	
	def getController(self):
		return self._controller
		
	def getMap(self):
		return self._map
		
	def save(self):
		""" Saves the map using the previous filename.
		
		Emits preSave and postSave signals.
		"""
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
		""" Saves the map using a specified filename.
		
		Emits preSave and postSave signals.
		"""
		events.preSave.send(sender=self, mapview=self)
		savers.saveMapFile(str(filename), self._editor.getEngine(), self._map, importList=self.importlist)
		events.postSave.send(sender=self, mapview=self)
		
	def close(self):
		""" Closes the mapview """
		pass

	