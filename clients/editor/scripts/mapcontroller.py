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
		self._selection = None  # currently selected coordinates
		self._instances = None  # currently selected instances
		self._map = None
		self._undo = False
		self._undomanager = undomanager.UndoManager()
		undomanager.preUndo.connect(self._startUndo, sender=self._undomanager)
		undomanager.preRedo.connect(self._startUndo, sender=self._undomanager)
		undomanager.postUndo.connect(self._endUndo, sender=self._undomanager)
		undomanager.postRedo.connect(self._endUndo, sender=self._undomanager)
		
		if map is not None:
			self.setMap(map.getId())
		
	def setMap(self, mapid):
		self._camera = None
		self._map = None
		self._layer = None
		self._object = None
		self._selection = None
		self._instances = None

		self._map = self._engine.getModel().getMap(mapid)
		if not self._map.getLayers():
			raise AttributeError('Editor error: map ' + self._map.getId() + ' has no layers. Cannot edit.')

		for cam in self._engine.getView().getCameras():
			if cam.getLocationRef().getMap().getId() == self._map.getId():
				self._camera = cam
				break

		self._layer = self._map.getLayers()[0]

	def selectLayer(self, layerid):
		self._layer = None
		layers = [l for l in self._map.getLayers() if l.getId() == layerid]
		if len(layers) == 1:
			self._layer = layers[0]

	def selectObject(self, object):
		self._object = object

	def selectCell(self, screenx, screeny, preciseCoords=False):
		if not self._camera: 
			print 'No camera bind yet, cannot select any cell'
			return
		if not self._layer:
			print 'No layer assigned in selectCell'
			return

		self._selection = self._camera.toMapCoordinates(fife.ScreenPoint(screenx, screeny), False)
		self._selection.z = 0
		loc = fife.Location(self._layer)
		if preciseCoords:
			self._selection = self._layer.getCellGrid().toExactLayerCoordinates(self._selection)
			loc.setExactLayerCoordinates(self._selection)
		else:
			self._selection = self._layer.getCellGrid().toLayerCoordinates(self._selection)
			loc.setLayerCoordinates(self._selection)
		fife.CellSelectionRenderer.getInstance(self._camera).selectLocation(loc)
		return loc
		
	def setSelectedInstances(self, instances):
		events.onInstancesSelected.send(sender=self, instances=instances)
		self._instances = instances

	def getInstancesFromPosition(self, position, top_only):
		if not self._layer:
			print 'No layer assigned in getInstancesFromPosition'
			return
		if not position:
			print 'No position assigned in getInstancesFromPosition'
			return

		loc = fife.Location(self._layer)
		if type(position) == fife.ExactModelCoordinate:
			loc.setExactLayerCoordinates(position)
		else:
			loc.setLayerCoordinates(position)
			
		instances = self._layer.getInstancesAt(loc)

		if top_only and (len(instances) > 0):
			instances = [instances[0]]
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

	def placeInstance(self, position, object, force=False):
		mname = '_placeInstance'
		if not object:
			print 'No object assigned in %s' % mname
			return
		if not position:
			print 'No position assigned in %s' % mname
			return
		if not self._layer:
			print 'No layer assigned in %s' % mname
			return

		print 'Placing instance of ' + object.getId() + ' at ' + str(position)

		# don't place repeat instances
		if force is False:
			for i in self.getInstancesFromPosition(position, False):
				if i.getObject().getId() == object.getId():
					print 'Warning: attempt to place duplicate instance of object %s. Ignoring request.' % object.getId()
					return

		inst = self._layer.createInstance(object, position)
		fife.InstanceVisual.create(inst)
		
		if not self._undo:
			redocall = lambda: self.placeInstance(position, object, force=True)
			undocall = lambda: self.removeInstances(position, force=True)
			undoobject = undomanager.UndoObject(undocall, redocall, "Placed instance")
			self._undomanager.addAction(undoobject)

	def removeInstances(self, position, force=False):
		mname = '_removeInstances'
		if not position:
			print 'No position assigned in %s' % mname
			return
		if not self._layer:
			print 'No layer assigned in %s' % mname
			return

		instances = self.getInstancesFromPosition(position, top_only=True)
		for i in instances:
			print 'Deleting instance ' + i.getObject().getId() + ' at ' + str(position)
			self._layer.deleteInstance(i)
			
			if not self._undo:
				object = i.getObject()
				
				undocall = lambda: self.placeInstance(position, object, force=True)
				redocall = lambda: self.removeInstances(position, force=True)
				undoobject = undomanager.UndoObject(undocall, redocall, "Removed instance")
				self._undomanager.addAction(undoobject)
		
	def moveInstances(self, exact=False):
		mname = '_moveInstances'
		if not self._selection:
			print 'No selection assigned in %s' % mname
			return
		if not self._layer:
			print 'No layer assigned in %s' % mname
			return

		loc = fife.Location(self._layer)
		if exact:
			loc.setExactLayerCoordinates(self._selection)
		else:
			loc.setLayerCoordinates(self._selection)
		for i in self._instances:
			f = fife.Location(self._layer)
			f.setExactLayerCoordinates(i.getFacingLocation().getExactLayerCoordinates() + fife.ExactModelCoordinate(float(self._selection.x), float(self._selection.y)) - i.getLocation().getExactLayerCoordinates())
			i.setLocation(loc)
			i.setFacingLocation(f)

	def rotateInstances(self):
		mname = '_rotateInstances'
		if not self._selection:
			print 'No selection assigned in %s' % mname
			return
		if not self._layer:
			print 'No layer assigned in %s' % mname
			return

		for i in self.getInstancesFromPosition(self._selection, top_only=True):
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
