# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

import editor
import pdb

import math

from fife import fife
import editor
import events
import undomanager

from fife.extensions.pychan.tools import callbackWithArguments as cbwa
from fife.extensions.fife_settings import Setting

TDS = Setting(app_name="editor")
		
class MapController(object):
	""" MapController provides an interface for editing maps """
	def __init__(self, map):
		
		self._editor = editor.getEditor()
		self._engine = self._editor.getEngine()

		self._camera = None     # currently selected camera
		self._layer = None      # currently selected layer
		self._selection = []	# currently selected cells
		self._single_instance = False # flag to force selection of one single instance
		self._instance = None	# current selected single instance
		self._map = None
		self._undo = False
		self._undomanager = undomanager.UndoManager()
		undomanager.preUndo.connect(self._startUndo, sender=self._undomanager)
		undomanager.preRedo.connect(self._startUndo, sender=self._undomanager)
		undomanager.postUndo.connect(self._endUndo, sender=self._undomanager)
		undomanager.postRedo.connect(self._endUndo, sender=self._undomanager)
		self.debug = False
		self._settings = TDS
		
		self.overwriteInstances = True # Remove instances on cell before placing new instance
		
		self.importList	= {} # used to keep track of current imports needed by the map
		
		if map is not None:
			self.setMap(map.getId())
			
	def cleanUp(self):
		undomanager.preUndo.disconnect(self._startUndo, sender=self._undomanager)
		undomanager.preRedo.disconnect(self._startUndo, sender=self._undomanager)
		undomanager.postUndo.disconnect(self._endUndo, sender=self._undomanager)
		undomanager.postRedo.disconnect(self._endUndo, sender=self._undomanager)
		self._undomanager.clear()
		
		self._editor = None
		self._engine = None
		self._map = None
		self._selection = []
		self._layer = None
		self._camera = None
		
	def setMap(self, mapid):
		""" Set the map to be edited """
		self._camera = None
		self._map = None
		self._layer = None
		self._selection = []

		self._map = self._engine.getModel().getMap(mapid)
		if not self._map.getLayers():
			raise AttributeError('Editor error: map ' + self._map.getId() + ' has no layers. Cannot edit.')

		for cam in self._map.getCameras():
			if cam.getLocationRef().getMap().getId() == self._map.getId():
				self._camera = cam
				break

		# remove imports from previous map
		self.importList.clear()
		
		# keep track of imports that were loaded with the map
		for layer in self._map.getLayers():
			for i in layer.getInstances():
				self.incrementReferenceCountForObject(i.getObject())
				
		self._layer = self._map.getLayers()[0]
		
		gridrenderer = fife.GridRenderer.getInstance(self._camera)
		gridrenderer.activateAllLayers(self._map)
		color = str(self._settings.get("Colors", "Grid", "0,255,0"))
		gridrenderer.setColor(*[int(c) for c in color.split(',')])

		blockrenderer = fife.BlockingInfoRenderer.getInstance(self._camera)
		blockrenderer.activateAllLayers(self._map)
		color = str(self._settings.get("Colors", "Blocking", "0,255,0"))
		blockrenderer.setColor(*[int(c) for c in color.split(',')])

		coordinaterenderer = fife.CoordinateRenderer.getInstance(self._camera)
		coordinaterenderer.activateAllLayers(self._map)
		color = str(self._settings.get("Colors", "Coordinate", "255,255,255"))
		coordinaterenderer.setColor(*[int(c) for c in color.split(',')])		

		cellrenderer = fife.CellSelectionRenderer.getInstance(self._camera)
		cellrenderer.activateAllLayers(self._map)
		color = str(self._settings.get("Colors", "CellSelection", "255,0,0"))
		cellrenderer.setColor(*[int(c) for c in color.split(',')])
		cellrenderer.setEnabled(True)

	def getMap(self):
		return self._map
	
	def selectLayer(self, layerid):
		""" Select layer to be edited """
		self.deselectSelection()
		self._layer = None
		layers = [l for l in self._map.getLayers() if l.getId() == layerid]
		if len(layers) == 1:
			self._layer = layers[0]

	def deselectSelection(self):
		""" Deselects all selected cells """
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot select any cell'
			return
		self._selection = []
		fife.CellSelectionRenderer.getInstance(self._camera).reset()
		
	def clearSelection(self):
		""" Removes all instances on selected cells """
		instances = self.getInstancesFromSelection()
		self._undomanager.startGroup("Cleared selection")
		self.removeInstances(instances)
		self._undomanager.endGroup()
		
	def fillSelection(self, object):
		""" Adds an instance of object on each selected cell """
		self._undomanager.startGroup("Filled selection")
		for loc in self._selection:
			self.placeInstance(loc.getLayerCoordinates(), object)
		self._undomanager.endGroup()

	def selectCell(self, screenx, screeny):
		""" Selects a cell at a position on screen """
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot select any cell'
			return
		if not self._layer:
			if self.debug: print 'No layer assigned in selectCell'
			return

		loc = fife.Location(self._layer)
		loc.setLayerCoordinates(self._layer.getCellGrid().toLayerCoordinates(self.screenToMapCoordinates(screenx, screeny)))

		for i in self._selection:
			if loc.getLayerCoordinates() == i.getLayerCoordinates(): return
			
		self._selection.append(loc)
		fife.CellSelectionRenderer.getInstance(self._camera).selectLocation(loc)
		
	def deselectCell(self, screenx, screeny):
		""" Deselects a cell at a position on screen """
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot select any cell'
			return
		if not self._layer:
			if self.debug: print 'No layer assigned in selectCell'
			return

		loc = fife.Location(self._layer)
		loc.setLayerCoordinates(self._layer.getCellGrid().toLayerCoordinates(self.screenToMapCoordinates(screenx, screeny)))
		
		for i in self._selection[:]:
			if loc.getLayerCoordinates() == i.getLayerCoordinates():
				self._selection.remove(i)
				fife.CellSelectionRenderer.getInstance(self._camera).deselectLocation(i)
				return

	def getLocationsFromSelection(self):
		""" Returns all locations in the selected cells """
		return self._selection
	
	def getInstance(self):
		""" Returns a single instance packed into a list (compat to API) """
		if self._instance:
			return [self._instance, ]
		return []
		
	def getInstancesFromSelection(self):
		""" Returns all instances in the selected cells """
		instances = []
		
		for loc in self._selection:
			instances.extend(self.getInstancesFromPosition(loc.getExactLayerCoordinates(), loc.getLayer()))

		return instances

	def getInstancesFromPosition(self, position, layer=None):
		""" Returns all instances on a specified position
			Interprets ivar _single_instance and returns only the 
			first instance if flag is set to True
		"""
		if not self._layer and not layer:
			if self.debug: print 'No layer assigned in getInstancesFromPosition'
			return
		if not position:
			if self.debug: print 'No position assigned in getInstancesFromPosition'
			return
			
		if layer:
			loc = fife.Location(layer)
		else:
			loc = fife.Location(self._layer)

		if isinstance(position, fife.ExactModelCoordinate):
			loc.setExactLayerCoordinates(position)
		else:
			loc.setLayerCoordinates(position)

		instances = self._camera.getMatchingInstances(loc)
			
		if self._single_instance and instances:
			return [instances[0], ]

		return instances

	def getAllInstances(self):
		""" Returns all instances """
		return self._layer.getInstances()

	def getUndoManager(self):
		""" Returns undo manager """
		return self._undomanager

	def undo(self):
		""" Undo one level """
		self._undomanager.undo()

	def redo(self):
		""" Redo one level """
		self._undomanager.redo()
		
	def _startUndo(self):
		""" Called before a undo operation is performed. Makes sure undo stack does not get corrupted """
		self._undo = True
		
	def _endUndo(self):
		""" Called when a undo operation is done """
		self._undo = False

	def placeInstance(self, position, object, layer=None):
		""" Places an instance of object at position. Any existing instances on position are removed. """
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
			instances = self.getInstancesFromPosition(position)
			if len(instances) == 1:
				# Check if the only instance at position has the same object
				objectId = instances[0].getObject().getId()
				objectNs = instances[0].getObject().getNamespace()
				if objectId == object.getId() and objectNs == object.getNamespace():
					if self.debug: print "Tried to place duplicate instance"
					return
					
			self._undomanager.startGroup("Placed instance")
			self.removeInstances(instances)

		if layer:
			inst = layer.createInstance(object, position)
		else:
			inst = self._layer.createInstance(object, position)
		fife.InstanceVisual.create(inst)
		
		# update reference count in import list for object
		self.incrementReferenceCountForObject(object)
		
		if not self._undo:
			redocall = cbwa(self.placeInstance, position, object, inst.getLocation().getLayer())
			undocall = cbwa(self.removeInstanceOfObjectAt, position, object, inst.getLocation().getLayer())
			undoobject = undomanager.UndoObject(undocall, redocall, "Placed instance")
			self._undomanager.addAction(undoobject)
			self._undomanager.endGroup()
			
	def removeInstanceOfObjectAt(self, position, object, layer=None):
		""" Removes the first instance of object it can find on position """
		instances = self.getInstancesFromPosition(position, layer)
		for i in instances:
			if i.getObject().getId() == object.getId() and i.getObject().getNamespace() == object.getNamespace():
				self.removeInstances([i],layer)
				return
					
	def removeInstances(self, instances, layer=None):
		""" Removes all provided instances """
		mname = 'removeInstances'
		if not instances:
			if self.debug: print 'No instances assigned in %s' % mname
			return
			
		for i in instances:
			if self.debug: print 'Deleting instance ' + i.getObject().getId() + ' at ' + str(i.getLocation().getExactLayerCoordinates())
			
			if not self._undo:
				object = i.getObject()
				position = i.getLocation().getExactLayerCoordinates()
				undocall = cbwa(self.placeInstance, position, object, i.getLocation().getLayer())
				redocall = cbwa(self.removeInstanceOfObjectAt, position, object, i.getLocation().getLayer())
				undoobject = undomanager.UndoObject(undocall, redocall, "Removed instance")
				self._undomanager.addAction(undoobject)
			
			if layer:
				layer.deleteInstance(i)
			else:
				self._layer.deleteInstance(i)
							
			self.decrementReferenceCountForObject(i.getObject())

	def moveInstances(self, instances, moveBy, exact=False, origLoc=None, origFacing=None):
		""" Moves provided instances by moveBy. If exact is false, the instances are
			snapped to closest cell. origLoc and origFacing are only set when an undo/redo
			operation takes place and will have no effect and should not be used under normal
			circumstances.
		
		@type	instances:	list
		@param	instances:	a bunch of selected fife.Instance objects
		@type	moveBy:	fife.Point3D
		@param	moveBy:	diff of last and current exact model coordinate relative to the cursor
		@type	exact:	bool
		@param	exact:	flag to either set exactLayerCoordinates or LayerCoordinates
		@rtype	result:	bool
		@return	result:	flag wether the instances were moved or not (always True if exact=True)
		"""
		result = True
		mname = 'moveInstances'
		if not self._layer:
			if self.debug: print 'No layer assigned in %s' % mname
			return not result

		moveBy = fife.ExactModelCoordinate(float(moveBy.x), float(moveBy.y), float(moveBy.z))

		for i in instances:
			loc = i.getLocation()
			f = i.getFacingLocation()
			
			nloc = fife.Location(self._layer)
			nloc.setMapCoordinates(loc.getMapCoordinates() + moveBy)
			
			if loc.getLayerCoordinates() == nloc.getLayerCoordinates() and not exact:
				result = False
				break
			
			if exact:
				loc.setMapCoordinates(nloc.getMapCoordinates())
				f.setMapCoordinates(loc.getMapCoordinates())
			else:	
				loc.setLayerCoordinates(nloc.getLayerCoordinates())
				f.setLayerCoordinates(loc.getLayerCoordinates())

			if not self._undo:
				undocall = cbwa(self.moveInstances, [i], moveBy, exact, i.getLocation(), i.getFacingLocation())
				redocall = cbwa(self.moveInstances, [i], moveBy, exact, i.getLocation(), i.getFacingLocation())
				undoobject = undomanager.UndoObject(undocall, redocall, "Moved instance")
				self._undomanager.addAction(undoobject)
				i.setLocation(loc)
				i.setFacingLocation(f)
				
			else:
				assert(origLoc)
				assert(origFacing)
				i.setLocation(origLoc)
				i.setFacingLocation(origFacing)
				
		return result


	def rotateCounterClockwise(self):
		""" Rotates map counterclockwise by 90 degrees """
		currot = self._camera.getRotation()
		self._camera.setRotation((currot + 270) % 360)
		
	def rotateClockwise(self):
		""" Rotates map clockwise by 90 degrees """
		currot = self._camera.getRotation()
		self._camera.setRotation((currot + 90) % 360)
		
	def getZoom(self):
		""" Returns camera zoom """
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot get zoom'
			return 0
		return self._camera.getZoom()
		
	def setZoom(self, zoom):
		""" Sets camera zoom """
		if not self._camera: 
			if self.debug: print 'No camera bind yet, cannot get zoom'
			return
		self._camera.setZoom(zoom)

	def moveCamera(self, screen_x, screen_y):
		""" Move camera (scroll) by screen_x, screen_y """
		if not self._camera: 
			return
			
		coords = self._camera.getLocationRef().getMapCoordinates()
		z = self._camera.getZoom()
		r = self._camera.getRotation()
		if screen_x:
			coords.x -= screen_x / z * math.cos(r / 180.0 * math.pi) / 100
			coords.y -= screen_x / z * math.sin(r / 180.0 * math.pi) / 100
		if screen_y:
			coords.x -= screen_y / z * math.sin(-r / 180.0 * math.pi) / 100
			coords.y -= screen_y / z * math.cos(-r / 180.0 * math.pi) / 100
			
		coords = self._camera.getLocationRef().setMapCoordinates(coords)
		self._camera.refresh()

	def screenToMapCoordinates(self, screenx, screeny):
		""" Convert screen coordinates to map coordinates, including z """
		if not self._camera: 
			if self.debug: print 'No camera bind yet in screenToMapCoordinates'
			return
		if not self._layer:
			if self.debug: print 'No layer assigned in screenToMapCoordinates'
			return
			
		screencoords = fife.ScreenPoint(screenx, screeny)
		z_offset = self._camera.getZOffset(self._layer)
		if self._layer == self._camera.getLocation().getLayer():
			instance_z = float(-self._camera.getLocation().getExactLayerCoordinates().z)
			layer_z = float(self._layer.getCellGrid().getZShift() - (self._camera.getLocation().getMapCoordinates().z - self._camera.getLocation().getExactLayerCoordinates().z))
			z_offset.x *= int(instance_z + layer_z * self._layer.getCellGrid().getXScale())
			z_offset.y *= int(instance_z + layer_z * self._layer.getCellGrid().getYScale())
		else:
			instance_z = float(-self._camera.getLocation().getExactLayerCoordinates(self._layer).z)
			layer_z = float(self._layer.getCellGrid().getZShift() - (self._camera.getLocation().getMapCoordinates().z - self._camera.getLocation().getExactLayerCoordinates(self._layer).z))
			z_offset.x *= int(instance_z + layer_z * self._layer.getCellGrid().getXScale())
			z_offset.y *= int(instance_z + layer_z * self._layer.getCellGrid().getYScale())

		if (z_offset.z <= 0):
			screencoords.y += int(z_offset.y)
		else:
			screencoords.y -= int(z_offset.y)

		mapCoords = self._camera.toMapCoordinates(screencoords, False)
		mapCoords.z = self._layer.getCellGrid().getZShift()
		
		return mapCoords
	
	def incrementReferenceCountForObject(self, object):
		""" updates the import count for the object passed in """
		objFilename = object.getFilename()
		if objFilename not in self.importList.keys():
			self.importList[objFilename] = 1
		else:
			self.importList[objFilename] += 1
		
	def decrementReferenceCountForObject(self, object):
		""" decrements the reference count for the object passed in
		and removes the object from the import list if the reference count hits 0 """
		objFilename = object.getFilename()
		if objFilename in self.importList.keys():
			self.importList[objFilename] -= 1
			if self.importList[objFilename] == 0:
				del self.importList[objFilename]
		
