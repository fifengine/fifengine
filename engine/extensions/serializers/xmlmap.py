# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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

import fife
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

import loaders
from serializers import *
from fife_utils import *
import time

FORMAT = '1.0'

class XMLMapLoader(fife.ResourceLoader):
	def __init__(self, engine, callback):
		""" The XMLMapLoader parses the xml map using several section. 
		Each section fires a callback (if given) which can e. g. be
		used to show a progress bar.
		
		The callback sends two values, a string and a float (which shows
		the overall process): callback(string, float)
			
		@param	engine	:	a pointer to fife.engine
		@param	callback:	a callback with two arguments, optional
		"""
		fife.ResourceLoader.__init__(self)
		self.thisown = 0
		
		self.callback = callback

		self.engine = engine
		self.vfs = self.engine.getVFS()
		self.model = self.engine.getModel()
		self.pool = self.engine.getImagePool()
		self.anim_pool = self.engine.getAnimationPool()
		self.map = None
		self.source = None
		self.time_to_load = 0

		self.nspace = None

	def _err(self, msg):
		raise SyntaxError(''.join(['File: ', self.source, ' . ', msg]))

	def loadResource(self, location):
		start_time = time.time()
		self.source = location.getFilename()
		f = self.vfs.open(self.source)
		f.thisown = 1
		tree = ET.parse(f)
		root = tree.getroot()
			
		map = self.parse_map(root)
		self.time_to_load = time.time() - start_time
		return map

	def parse_map(self, mapelt):
		if not mapelt:
			self._err('No <map> element found at top level of map file definition.')
		id,format = mapelt.get('id'),mapelt.get('format')

		if not format == FORMAT: self._err(''.join(['This file has format ', format, ' but this loader has format ', FORMAT]))
		if not id: self._err('Map declared without an identifier.')

		map = None
		try:
			self.map = self.model.createMap(str(id))
			self.map.setResourceFile(self.source)
		except fife.Exception, e:
			if is_fife_exc(fife.NameClash, e):
				msg = e.getMessage()+" "
				msg = msg + X+''.join(['File: ', self.source, '. The map ', str(id), ' already exists! Ignoring map definition.'])
				logger.log(fife.LogManager.LEVEL_WARN, msg)
				return None
			raise

		# xml-specific directory imports. This is used by xml savers.
		self.map.importDirs = []

		if self.callback is not None:
			self.callback('created map', float(0.25) )

		self.parse_imports(mapelt, self.map)
		
		self.parse_layers(mapelt, self.map)	
		
		self.parse_cameras(mapelt, self.map)

		return self.map

	def parse_imports(self, mapelt, map):
		parsedImports = {}

		if self.callback:		
			tmplist = mapelt.findall('import')
			i = float(0)
		
		for item in mapelt.findall('import'):
			file = item.get('file')
			if file:
				file = reverse_root_subfile(self.source, file)
			dir = item.get('dir')
			if dir:
				dir = reverse_root_subfile(self.source, dir)

			# Don't parse duplicate imports
			if (dir,file) in parsedImports:
				logger.log(fife.LogManager.LEVEL_WARN, "Duplicate import:" + str((dir, file)))
				continue
			parsedImports[(dir,file)] = 1

			if file and dir:
				loaders.loadImportFile('/'.join(dir, file), self.engine)
			elif file:
				loaders.loadImportFile(file, self.engine)
			elif dir:
				loaders.loadImportDirRec(dir, self.engine)
				map.importDirs.append(dir)
			else:
				logger.log(fife.LogManager.LEVEL_WARN, 'Empty import statement?')
				
			if self.callback:
				i += 1				
				self.callback('loaded imports', float( i / float(len(tmplist)) * 0.25 + 0.25 ) )


	def parse_layers(self, mapelt, map):
		if self.callback is not None:		
			tmplist = mapelt.findall('layer')
			i = float(0)

		for layer in mapelt.findall('layer'):
			id = layer.get('id')
			grid_type = layer.get('grid_type')
			x_scale = layer.get('x_scale')
			y_scale = layer.get('y_scale')
			rotation = layer.get('rotation')
			x_offset = layer.get('x_offset')
			y_offset = layer.get('y_offset')
			pathing = layer.get('pathing')
			transparency = layer.get('transparency')
			
			if not x_scale: x_scale = 1.0
			if not y_scale: y_scale = 1.0
			if not rotation: rotation = 0.0
			if not x_offset: x_offset = 0.0
			if not y_offset: y_offset = 0.0
			if not pathing: pathing = "cell_edges_only"
			if not transparency: 
				transparency = 0
			else:
				transparency = int(transparency)

			if not id: self._err('<layer> declared with no id attribute.')
			if not grid_type: self._err(''.join(['Layer ', str(id), ' has no grid_type attribute.']))

			cellgrid = self.model.getCellGrid(grid_type)
			if not cellgrid: self._err('<layer> declared with invalid cellgrid type. (%s)' % grid_type)

			cellgrid.setRotation(float(rotation))
			cellgrid.setXScale(float(x_scale))
			cellgrid.setYScale(float(y_scale))
			cellgrid.setXShift(float(x_offset))
			cellgrid.setYShift(float(y_offset))

			layer_obj = None
			try:
				layer_obj = map.createLayer(str(id), cellgrid)
			except fife.Exception, e:
				print e.getMessage()
				logger.log(fife.LogManager.LEVEL_WARN, 'The layer ' + str(id) + ' already exists! Ignoring this layer.')
				continue

			strgy = fife.CELL_EDGES_ONLY
			if pathing == "cell_edges_and_diagonals":
				strgy = fife.CELL_EDGES_AND_DIAGONALS
			if pathing == "freeform":
				strgy = fife.FREEFORM
			layer_obj.setPathingStrategy(strgy)

			layer_obj.setLayerTransparency(transparency)

			self.parse_instances(layer, layer_obj)

			if self.callback is not None:
				i += 1
				self.callback('loaded layer :' + str(id), float( i / float(len(tmplist)) * 0.25 + 0.5 ) )

		# cleanup
		if self.callback is not None:
			del tmplist
			del i

	def parse_instances(self, layerelt, layer):
		instelt = layerelt.find('instances')

		instances = instelt.findall('i')
		instances.extend(instelt.findall('inst'))
		instances.extend(instelt.findall('instance'))
		for instance in instances:

			objectID = instance.get('object')
			if not objectID:
				objectID = instance.get('obj')
			if not objectID:
				objectID = instance.get('o')

			if not objectID: self._err('<instance> does not specify an object attribute.')

			nspace = instance.get('namespace')
			if not nspace:
				nspace = instance.get('ns')
			if not nspace:
				nspace = self.nspace

			if not nspace: self._err('<instance> %s does not specify an object namespace, and no default is available.' % str(objectID))

			self.nspace = nspace

			object = self.model.getObject(str(objectID), str(nspace))
			if not object:
				msg = 'Object with id=' + str(objectID) + ' ns=' + str(nspace) + ' could not be found. Omitting...'
				logger.log(fife.LogManager.LEVEL_WARN, msg)
				continue

			x = instance.get('x')
			y = instance.get('y')
			z = instance.get('z')
			stackpos = instance.get('stackpos')
			id = instance.get('id')

			if x:
				x = float(x)
				self.x = x
			else:
				self.x = self.x + 1
				x = self.x

			if y:
				y = float(y)
				self.y = y
			else:
				y = self.y

			if z:
				z = float(z)
			else:
				z = 0.0

			if not id:
				id = ''
			else:
				id = str(id)

			inst = layer.createInstance(object, fife.ExactModelCoordinate(x,y,z), str(id))

			rotation = instance.get('r')
			if not rotation:
				rotation = instance.get('rotation')
			if not rotation:
				angles = object.get2dGfxVisual().getStaticImageAngles()
				if angles:
					rotation = angles[0]
				else:
					rotation = 0
			else:
				rotation = int(rotation)
			inst.setRotation(rotation)

			fife.InstanceVisual.create(inst)
			if (stackpos):
				inst.get2dGfxVisual().setStackPosition(int(stackpos))

			if (object.getAction('default')):
				target = fife.Location(layer)
				inst.act('default', target, True)

	def parse_cameras(self, mapelt, map):
		if self.callback:		
			tmplist = mapelt.findall('camera')
			i = float(0)

		for camera in mapelt.findall('camera'):
			id = camera.get('id')
			zoom = camera.get('zoom')
			tilt = camera.get('tilt')
			rotation = camera.get('rotation')
			ref_layer_id = camera.get('ref_layer_id')
			ref_cell_width = camera.get('ref_cell_width')
			ref_cell_height = camera.get('ref_cell_height')
			viewport = camera.get('viewport')

			if not zoom: zoom = 1
			if not tilt: tilt = 0
			if not rotation: rotation = 0

			if not id: self._err('Camera declared without an id.')
			if not ref_layer_id: self._err(''.join(['Camera ', str(id), ' declared with no reference layer.']))
			if not (ref_cell_width and ref_cell_height): self._err(''.join(['Camera ', str(id), ' declared without reference cell dimensions.']))

			try:
				if viewport:
					cam = self.engine.getView().addCamera(str(id), map.getLayer(str(ref_layer_id)),fife.Rect(*[int(c) for c in viewport.split(',')]),fife.ExactModelCoordinate(0,0,0))
				else:
					screen = self.engine.getRenderBackend()
					cam = self.engine.getView().addCamera(str(id), map.getLayer(str(ref_layer_id)),fife.Rect(0,0,screen.getScreenWidth(),screen.getScreenHeight()),fife.ExactModelCoordinate(0,0,0))

				cam.setCellImageDimensions(int(ref_cell_width), int(ref_cell_height))
				cam.setRotation(float(rotation))
				cam.setTilt(float(tilt))
				cam.setZoom(float(zoom))
			except fife.Exception, e:
				logger.log(fife.LogManager.LEVEL_WARN, "Error when parsing cameras: "+e.getMessage())
				
			if self.callback:
				i += 1
				self.callback('loaded camera: ' +  str(id), float( i / len(tmplist) * 0.25 + 0.75 ) )	
			
