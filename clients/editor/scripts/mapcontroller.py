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
		self._object = None     # currently selected object
		self._selection = set()	# currently selected coordinates
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
		self._object = None
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
		instances = getInstancesFromSelection()
		self.undomanager.startGroup("Cleared selection")
		self.removeInstances(instances)
		self.undomanager.endGroup()
		
	def fillSelection(self, object):
		self.undomanager.startGroup("Cleared selection")
		for pos in self._selection:
			self.placeInstance(pos, object)
		self.undomanager.endGroup()

	def selectCell(self, screenx, screeny, preciseCoords=False):
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot select any cell'
			return
		if not self._layer:
			if self.debug: print 'No layer assigned in selectCell'
			return

		mapCoords = self._camera.toMapCoordinates(fife.ScreenPoint(screenx, screeny), False)
		loc = fife.Location(self._layer)
		if preciseCoords:
			position = self._layer.getCellGrid().toExactLayerCoordinates(mapCoords)
			self._selection.add( position )
			loc.setExactLayerCoordinates(position)
		else:
			position = self._layer.getCellGrid().toLayerCoordinates(mapCoords)
			self._selection.add(position)
			loc.setLayerCoordinates(position)
			
		fife.CellSelectionRenderer.getInstance(self._camera).selectLocation(loc)
		return loc
		
	def deselectCell(self, screenx, screeny, preciseCoords=False):
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot select any cell'
			return
		if not self._layer:
			if self.debug: print 'No layer assigned in selectCell'
			return

		mapCoords = self._camera.toMapCoordinates(fife.ScreenPoint(screenx, screeny), False)
		loc = fife.Location(self._layer)
		if preciseCoords:
			position = self._layer.getCellGrid().toExactLayerCoordinates(mapCoords)
			self._selection.discard( position )
			loc.setExactLayerCoordinates(position)
		else:
			position = self._layer.getCellGrid().toLayerCoordinates(mapCoords)
			self._selection.discard(position)
			loc.setLayerCoordinates(position)
			
		fife.CellSelectionRenderer.getInstance(self._camera).deselectLocation(loc)
		return loc
		
	def getInstancesFromSelection(self):
		instances = []
		for pos in self._selection:
			instances.extend(self.getInstancesFromPosition(pos))
		return instances

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
			
	def placeInstances(self, instances):
		for i in instances:
			fife.InstanceVisual.create(i)
			
	def removeInstances(self, instances):
		mname = 'removeInstances'
		if not instances:
			if self.debug: print 'No instances assigned in %s' % mname
			return
			
		for i in instances:
			print 'Deleting instance ' + i.getObject().getId() + ' at ' + str(i.getLocation().getExactLayerCoordinates())
			
			if not self._undo:
				object = i.getObject()
				
				position = i.getLocation().getExactLayerCoordinates()
				undocall = lambda: self.placeInstances(position, object)
				redocall = lambda: self.removeInstances([i])
				undoobject = undomanager.UndoObject(undocall, redocall, "Removed instance")
				self._undomanager.addAction(undoobject)
				
			self._layer.deleteInstance(i)

	def moveInstances(self, instances, moveBy):
		mname = 'moveInstances'
		if not self._layer:
			if self.debug: print 'No layer assigned in %s' % mname
			return

		for i in instances:
			loc = i.getLocation()
			if type(moveBy) == fife.ExactModelCoordinate:
				oldCoords = loc.getExactLayerCoordinates()
				loc.setExactLayerCoordinates(oldCoords+moveBy)
			else:
				oldCoords = loc.getLayerCoordinates()
				exactCoords = oldCoords+moveBy
				exactCoords = fife.ExactModelCoordinate(float(exactCoords.x), float(exactCoords.y), float(exactCoords.z))
				loc.setMapCoordinates(exactCoords)
				
			i.setLocation(loc)

	def rotateInstances(self, instances):
		mname = 'rotateInstances'
		if not self._layer:
			if self.debug: print 'No layer assigned in %s' % mname
			return

		for i in instances:
# by c 09/11/08
# FIXME:
			# "hardcoded" rotation is bad for offset editing
			# instead we should use the angle list given from the object
			# animations are an issue, as a workaround settings.py provides
			# project specific animation angles
			try:
				if self._objectedit_rotations is not None:
#				print "available angles: ", self._objectedit_rotations
					rotation_prev = i.getRotation()
#				print "previous rotation: ", rotation_prev
					length = len(self._objectedit_rotations)
#				print "length: ", length
					index = self._objectedit_rotations.index( str(rotation_prev) )
#				print "index, old: ", index
					if index < length - 1:
						index += 1
					elif index == length:
						index = 0
					else:
						index = 0
#				print "index, new: ", index

					i.setRotation( int(self._objectedit_rotations[index]) )
#				print "new rotation: ", self._objectedit_rotations[index]
			except:
				# Fallback
				i.setRotation((i.getRotation() + 90) % 360)

# end FIXME
# end edit c

##    Surprisingly, the following "snap-to-rotation" code is actually incorrect. Object
##    rotation is independent of the camera, whereas the choice of an actual rotation image
##    depends very much on how the camera is situated. For example, suppose an object has
##    rotations defined for 45,135,225,315. And suppose the camera position results in an
##    effective 60 degree rotation. If the object is given a rotation of 0, then the (correct)
##    final rotation value of 45 (which is closest to 60 = 0 + 60) will be chosen. If we try
##    to snap to the closest value to 0 (45), then an incorrect final rotation value will be
##    chosen: 135, which is closest to 105 = 45 + 60. --jwt
#			ovis = i.getObject().get2dGfxVisual()
#			curUsedAngle = ovis.getClosestMatchingAngle(i.getRotation())
#			angles = ovis.getStaticImageAngles()
#			if angles:
#				ind = list(angles).index(curUsedAngle)
#				if ind == (len(angles) - 1):
#					ind = 0
#				else:
#					ind += 1
#				i.setRotation(angles[ind])
#			else:
#				print "rotation not supported for this instance"
	def changeRotation(self):
		currot = self._camera.getRotation()
		self._camera.setRotation((currot + 90) % 360)

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
