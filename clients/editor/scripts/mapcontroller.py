import editor
import pdb

import math

import fife
import editor
import events
import undomanager

class MapController(object):
	def __init__(self, map):
		
		self._editor = editor.getEditor()
		self._engine = self._editor.getEngine()

		self._camera = None     # currently selected camera
		self._layer = None      # currently selected layer
		self._selection = set()	# currently selected cells
		self._map = None
		self._undo = False
		self._undomanager = undomanager.UndoManager()
		undomanager.preUndo.connect(self._startUndo, sender=self._undomanager)
		undomanager.preRedo.connect(self._startUndo, sender=self._undomanager)
		undomanager.postUndo.connect(self._endUndo, sender=self._undomanager)
		undomanager.postRedo.connect(self._endUndo, sender=self._undomanager)
		self.debug = False
		
		self.overwriteInstances = True # Remove instances on cell before placing new instance
		
		if map is not None:
			self.setMap(map.getId())
		
	def setMap(self, mapid):
		self._camera = None
		self._map = None
		self._layer = None
		self._selection.clear()

		self._map = self._engine.getModel().getMap(mapid)
		if not self._map.getLayers():
			raise AttributeError('Editor error: map ' + self._map.getId() + ' has no layers. Cannot edit.')

		for cam in self._engine.getView().getCameras():
			if cam.getLocationRef().getMap().getId() == self._map.getId():
				self._camera = cam
				break

		self._layer = self._map.getLayers()[0]

	def selectLayer(self, layerid):
		self.resetSelection()
		self._layer = None
		layers = [l for l in self._map.getLayers() if l.getId() == layerid]
		if len(layers) == 1:
			self._layer = layers[0]

	def resetSelection(self):
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot select any cell'
			return
		self._selection.clear()
		fife.CellSelectionRenderer.getInstance(self._camera).reset()
		
	def clearSelection(self):
		instances = self.getInstancesFromSelection()
		self._undomanager.startGroup("Cleared selection")
		self.removeInstances(instances)
		self._undomanager.endGroup()
		
	def fillSelection(self, object):
		self._undomanager.startGroup("Filled selection")
		for loc in self._selection:
			self.placeInstance(loc.getLayerCoordinates(), object)
		self._undomanager.endGroup()

	def selectCell(self, screenx, screeny):
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot select any cell'
			return
		if not self._layer:
			if self.debug: print 'No layer assigned in selectCell'
			return

		mapCoords = self._camera.toMapCoordinates(fife.ScreenPoint(screenx, screeny), False)
		position = self._layer.getCellGrid().toLayerCoordinates(mapCoords)
		
		loc = fife.Location(self._layer)
		loc.setLayerCoordinates(position)
		
		for i in self._selection:
			if loc == i: return
			
		self._selection.add( loc )
		fife.CellSelectionRenderer.getInstance(self._camera).selectLocation(loc)
		
	def getInstancesFromSelection(self):
		instances = []
		uniqueInstances = []
		
		for loc in self._selection:
			instances.extend(self.getInstancesFromPosition(loc.getLayerCoordinates()))
		
		for i in instances:
			for u in uniqueInstances:
				# We must use the location reference to check for equality,
				# as i == u always returns false
				# However, this will not select all tiles if there are multiple tiles on a location
				if i.getLocationRef() == u.getLocationRef(): 
					break
			else: uniqueInstances.append(i)
		return uniqueInstances

	def getInstancesFromPosition(self, position):
		if not self._layer:
			if self.debug: print 'No layer assigned in getInstancesFromPosition'
			return
		if not position:
			if self.debug: print 'No position assigned in getInstancesFromPosition'
			return

		loc = fife.Location(self._layer)
		if type(position) == fife.ExactModelCoordinate:
			loc.setExactLayerCoordinates(position)
		else:
			loc.setLayerCoordinates(position)
			
		instances = self._layer.getInstancesAt(loc)

		return instances

	def getUndoManager(self):
		return self._undomanager

	def undo(self):
		self._undomanager.undo()

	def redo(self):
		self._undomanager.redo()
		
	def _startUndo(self):
		self._undo = True
		
	def _endUndo(self):
		self._undo = False

	def placeInstance(self, position, object):
		mname = 'placeInstance'
		if not object:
			if self.debug: print 'No object assigned in %s' % mname
			return
		if not position:
			if self.debug: print 'No position assigned in %s' % mname
			return
		if not self._layer:
			if self.debug: print 'No layer assigned in %s' % mname
			return

		if self.debug: print 'Placing instance of ' + object.getId() + ' at ' + str(position)

		# Remove instances from target position
		if not self._undo:
			self._undomanager.startGroup("Placed instance")
			self.removeInstances(self.getInstancesFromPosition(position))

		inst = self._layer.createInstance(object, position)
		fife.InstanceVisual.create(inst)
		
		if not self._undo:
			redocall = lambda: self.placeInstance(position, object)
			undocall = lambda: self.removeInstances([inst])
			undoobject = undomanager.UndoObject(undocall, redocall, "Placed instance")
			self._undomanager.addAction(undoobject)
			self._undomanager.endGroup()
			
	def removeInstances(self, instances):
		mname = 'removeInstances'
		if not instances:
			if self.debug: print 'No instances assigned in %s' % mname
			return
			
		for i in instances:
			if self.debug: print 'Deleting instance ' + i.getObject().getId() + ' at ' + str(i.getLocation().getExactLayerCoordinates())
			
			if not self._undo:
				object = i.getObject()
				
				position = i.getLocation().getExactLayerCoordinates()
				undocall = lambda: self.placeInstance(position, object)
				redocall = lambda: self.removeInstances([i])
				undoobject = undomanager.UndoObject(undocall, redocall, "Removed instance")
				self._undomanager.addAction(undoobject)
				
			self._layer.deleteInstance(i)

	def moveInstances(self, instances, moveBy, exact=False):
		mname = 'moveInstances'
		if not self._layer:
			if self.debug: print 'No layer assigned in %s' % mname
			return
			
		if exact and type(moveBy) != fife.ExactModelCoordinate:
			moveBy = fife.ExactModelCoordinate(float(moveBy.x), float(moveBy.y), float(moveBy.z))
		elif exact is False and type(moveBy) != fife.ModelCoordinate:
			moveBy = fife.ModelCoordinate(int(round(moveBy.x)), int(round(moveBy.y)), int(round(moveBy.z)))
			
		for i in instances:
			loc = i.getLocation()
			f = i.getFacingLocation()
			if exact:
				newCoords = loc.getExactLayerCoordinates() + moveBy
				loc.setExactLayerCoordinates(newCoords)
				f.setExactLayerCoordinates(f.getExactLayerCoordinates() + moveBy)
			else:
				# Move instance and snap to closest cell
				newCoords = loc.getLayerCoordinates() + moveBy
				loc.setLayerCoordinates(newCoords)
				f.setLayerCoordinates(f.getLayerCoordinates() + moveBy)
			i.setLocation(loc)
			i.setFacingLocation(f)

	def rotateCounterClockwise(self):
		currot = self._camera.getRotation()
		self._camera.setRotation((currot + 270) % 360)
		
	def rotateClockwise(self):
		currot = self._camera.getRotation()
		self._camera.setRotation((currot + 90) % 360)
		
	def getZoom(self):
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot get zoom'
			return 0
		return self._camera.getZoom()
		
	def setZoom(self, zoom):
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot get zoom'
			return
		self._camera.setZoom(zoom)

	def moveCamera(self, screen_x, screen_y):
		coords = self._camera.getLocationRef().getMapCoordinates()
		z = self._camera.getZoom()
		r = self._camera.getRotation()
		if screen_x:
			coords.x -= screen_x / z * math.cos(r / 180.0 * math.pi) / 100;
			coords.y -= screen_x / z * math.sin(r / 180.0 * math.pi) / 100;
		if screen_y:
			coords.x -= screen_y / z * math.sin(-r / 180.0 * math.pi) / 100;
			coords.y -= screen_y / z * math.cos(-r / 180.0 * math.pi) / 100;
		coords = self._camera.getLocationRef().setMapCoordinates(coords)
		self._camera.refresh()
