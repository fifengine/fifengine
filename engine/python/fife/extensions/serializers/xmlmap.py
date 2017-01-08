# -*- coding: utf-8 -*-
# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
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

""" main xml parser class for xml map loading """

import time

from fife import fife

from fife.extensions.serializers import ET
from fife.extensions.serializers import SerializerError, InvalidFormat 
from fife.extensions.serializers import NameClash, NotFound, WrongFileType

from fife.extensions.serializers.xmlobject import XMLObjectLoader
from fife.extensions.serializers.xmlanimation import loadXMLAnimation
from fife.extensions.serializers.xml_loader_tools import loadImportFile, loadImportDir
from fife.extensions.serializers.xml_loader_tools import loadImportDirRec
from fife.extensions.serializers.xml_loader_tools import root_subfile, reverse_root_subfile	
	

FORMAT = '1.0'

class XMLMapLoader(object):
	""" The B{XMLMapLoader} parses the xml map using several section. 
	Each section fires a callback (if given) which can e. g. be
	used to show a progress bar.
	
	The callback sends two values, a string and a float (which shows
	the overall process): callback(string, float)
	"""
	def __init__(self, engine, callback, debug, extensions):
		"""
		@type	engine:		object
		@param	engine:		a pointer to fife.engine
		@type	callback:	function
		@param	callback:	a callback with two arguments, optional
		@type	debug:		bool
		@param	debug:		flag to activate / deactivate print statements
		@type	extensions:	dict
		@param	extensions:	information package which extension should be activated (lights, sounds)
		"""
#		self.thisown = 0
		
		self.callback = callback
		self.debug = debug

		self.engine = engine
		self.vfs = self.engine.getVFS()
		self.model = self.engine.getModel()
		self.image_manager = self.engine.getImageManager()
		self.anim_pool = None
		
		self.obj_loader = XMLObjectLoader(engine)
	
		self.map = None
		self.source = None
		self.time_to_load = 0

		self.nspace = None

		self.msg = {}
		self.msg['map'] = 'created map'
		self.msg['imports'] = 'loaded imports'		
		self.msg['layer'] = 'loaded layer: %s'
		self.msg['camera'] = 'loaded camera: %s'

		if 'sound' not in extensions:
			extensions['sound'] = False
		if 'lights' not in extensions:
			extensions['lights'] = False
			
		self.light_data = {}
		self.extensions = extensions
		
	def _err(self, msg):
		raise SyntaxError(''.join(['File: ', self.source, ' . ', msg]))

	def loadResource(self, location):
		""" overwrite of B{fife.ResourceLoader}
		
		@type	location:	object
		@param	location:	path to a map file as a fife.ResourceLocation
		@return	FIFE map object
		@rtype	object
		"""
		start_time = time.time()
		self.source = location
		f = self.vfs.open(self.source)
		f.thisown = 1
		tree = ET.parse(f)
		root = tree.getroot()
			
		map = self.parse_map(root)
		self.time_to_load = time.time() - start_time
		return map

	def parse_map(self, mapelt):
		""" start parsing the xml structure and
		call submethods for turning found tags
		into FIFE objects and create the map
		
		@type	mapelt:	object
		@param	mapelt:	ElementTree root
		@return	FIFE map object
		@rtype	object
		"""
		if not mapelt:
			self._err('No <map> element found at top level of map file definition.')
		_id, format = mapelt.get('id'), mapelt.get('format')

		if not format == FORMAT: self._err(''.join(['This file has format ', format, ' but this loader has format ', FORMAT]))
		if not _id: self._err('Map declared without an identifier.')

		map = None
		try:
			self.map = self.model.createMap(str(_id))
			self.map.setFilename(self.source)
		except fife.Exception, e: # NameClash appears as general fife.Exception; any ideas?
			print e.getMessage()
			print ''.join(['File: ', self.source, '. The map ', str(_id), ' already exists! Ignoring map definition.'])
			return map

		# xml-specific directory imports. This is used by xml savers.
		self.map.importDirs = []

		if self.callback is not None:
			self.callback(self.msg['map'], float(0.25) )

		self.parse_imports(mapelt, self.map)
		self.parse_layers(mapelt, self.map)	
		self.parse_cameras(mapelt, self.map)
		
		# create light nodes
		if self.light_data:
			self.create_light_nodes(self.map)
		
		return self.map

	def parse_imports(self, mapelt, map):
		""" load all objects defined as import into memory
		
		@type	mapelt:	object
		@param	mapelt:	ElementTree root
		@return	FIFE map object			
		@rtype	object
		"""
		parsedImports = {}

		if self.callback:		
			tmplist = mapelt.findall('import')
			i = float(0)
		
		for item in mapelt.findall('import'):
			_file = item.get('file')
			if _file:
				_file = reverse_root_subfile(self.source, _file)
			_dir = item.get('dir')
			if _dir:
				_dir = reverse_root_subfile(self.source, _dir)
				
			# Don't parse duplicate imports
			if (_dir,_file) in parsedImports:
				if self.debug: print "Duplicate import:" ,(_dir, _file)
				continue
			parsedImports[(_dir,_file)] = 1

			if _file and _dir:
				loadImportFile(self.obj_loader, '/'.join(_dir, _file), self.engine, self.debug)
			elif _file:
				loadImportFile(self.obj_loader, _file, self.engine, self.debug)
			elif _dir:
				loadImportDirRec(self.obj_loader, _dir, self.engine, self.debug)
				map.importDirs.append(_dir)
			else:
				if self.debug: print 'Empty import statement?'
				
			if self.callback:
				i += 1				
				self.callback(self.msg['imports'], float( i / float(len(tmplist)) * 0.25 + 0.25 ) )

	def parse_layers(self, mapelt, map):
		""" create all layers and their instances
		
		@type	mapelt:	object
		@param	mapelt:	ElementTree root
		@type	map:	object
		@param	map:	FIFE map object			
		"""		
		if self.callback is not None:		
			tmplist = mapelt.findall('layer')
			i = float(0)

		for layer in mapelt.findall('layer'):
			_id = layer.get('id')
			grid_type = layer.get('grid_type')

			if not _id: self._err('<layer> declared with no id attribute.')
			if not grid_type: self._err(''.join(['Layer ', str(_id), ' has no grid_type attribute.']))

			x_scale = layer.get('x_scale')
			y_scale = layer.get('y_scale')
			rotation = layer.get('rotation')
			x_offset = layer.get('x_offset')
			y_offset = layer.get('y_offset')
			z_offset = layer.get('z_offset')
			pathing = layer.get('pathing')
			transparency = layer.get('transparency')
			
			layer_type = layer.get('layer_type')
			layer_type_id = layer.get('layer_type_id')
			
			if not x_scale: x_scale = 1.0
			if not y_scale: y_scale = 1.0
			if not rotation: rotation = 0.0
			if not x_offset: x_offset = 0.0
			if not y_offset: y_offset = 0.0
			if not z_offset: z_offset = 0.0
			if not pathing: pathing = "cell_edges_only"
			if not transparency: 
				transparency = 0
			else:
				transparency = int(transparency)

			cellgrid = self.model.getCellGrid(grid_type)
			if not cellgrid: self._err('<layer> declared with invalid cellgrid type. (%s)' % grid_type)

			cellgrid.setRotation(float(rotation))
			cellgrid.setXScale(float(x_scale))
			cellgrid.setYScale(float(y_scale))
			cellgrid.setXShift(float(x_offset))
			cellgrid.setYShift(float(y_offset))
			cellgrid.setZShift(float(z_offset))

			layer_obj = None
			try:
				layer_obj = map.createLayer(str(_id), cellgrid)
			except fife.Exception, e:
				print e.getMessage()
				print 'The layer ' + str(_id) + ' already exists! Ignoring this layer.'

				continue

			strgy = fife.CELL_EDGES_ONLY
			if pathing == "cell_edges_and_diagonals":
				strgy = fife.CELL_EDGES_AND_DIAGONALS

			layer_obj.setPathingStrategy(strgy)
			layer_obj.setLayerTransparency(transparency)

			if layer_type:
				if layer_type == 'walkable':
					layer_obj.setWalkable(True)
				elif layer_type == 'interact' and layer_type_id:
					layer_obj.setInteract(True, layer_type_id)

			self.parse_instances(layer, layer_obj)
			
			if self.extensions['lights']:
				self.parse_lights(layer, layer_obj)
			if self.extensions['sound']:
				self.parse_sounds(layer, layer_obj)

			if self.callback is not None:
				i += 1
				self.callback(self.msg['layer'] % str(_id), float( i / float(len(tmplist)) * 0.25 + 0.5 ) )


		layers = map.getLayers()
		for l in layers:
			if l.isInteract():
				walk_layer = map.getLayer(l.getWalkableId())
				if walk_layer:
					walk_layer.addInteractLayer(l);
				
		for l in layers:
			if l.isWalkable():
				l.createCellCache()

		# cleanup
		if self.callback is not None:
			del tmplist
			del i

	def parse_lights(self, layerelt, layer):
		""" create light nodes
		
		@type	layerelt:	object
		@param	layerelt:	ElementTree layer branch
		@type	layer:	object
		@param	layer:	FIFE layer object
		"""
		_LIGHT_DEFAULT_BLENDING_SRC = -1
		_LIGHT_DEFAULT_BLENDING_DST = -1
		_LIGHT_DEFAULT_SUBDIVISIONS = 32
		_LIGHT_DEFAULT_CAM_ID = 'default'
		_LIGHT_DEFAULT_INTENSITY = 128
		_LIGHT_DEFAULT_RADIUS = 10.0
		
		print "Processing lights ... "		
		lightelt = layerelt.find('lights')
		if not lightelt: 
			print "\tno lights found on layer %s" % layer.getId()
			return		
		
		lights = []
		for attr in ('l', 'light', 'lgt'):
			lights.extend(lightelt.findall(attr))
		
		for light in lights:
			group = light.get('group')
			if not group:
				print "Light has no group. Omitting..."
				continue
				
			blending_src = light.get('src')
			if not blending_src:
				blending_src = _LIGHT_DEFAULT_BLENDING_SRC
			blending_dst = light.get('dst')
			if not blending_dst:
				blending_dst = _LIGHT_DEFAULT_BLENDING_DST
				
			_x = light.get('x')
			if not _x: _x = 0
			_y = light.get('y')
			if not _y: _y = 0
			_z = light.get('y')
			if not _z: _z = 0

			node = {}				
			node['blending_src'] = int(blending_src)
			node['blending_dst'] = int(blending_dst)
			node['layer'] = layer.getId()
			node['position'] = int(_x), int(_y), int(_z)

			# where is the light? *sing*
			instance_id = light.get('instance')
			node['instance'] = None
			if instance_id and layer.getInstance(instance_id):
				node['instance'] = instance_id
			
			type = light.get('type')
			if type:
				s_ref = light.get('s_ref')
				if not s_ref: s_ref = -1
				node['s_ref'] = int(s_ref)
				a_ref = light.get('a_ref')
				if not a_ref: a_ref = 0.0
				node['a_ref'] = float(a_ref)
				
				if type == 'image':
					image = light.get('image')					
					if not image:
						print "Light has no image. Omitting..."
						continue
					node['type'] = 'image'
					image = reverse_root_subfile(self.source, image)
					img = self.image_manager.create(image)
					node['image'] = img
				elif type == 'animation':
					animation = light.get('animation')					
					if not animation:
						print "Light has no animation. Omitting..."
						continue
					node['type'] = 'animation'
					animation = reverse_root_subfile(self.source, animation)
					anim = loadXMLAnimation(self.engine, animation)
					node['animation'] = anim					
				elif type == 'simple':
					node['type'] = type
					radius = light.get('radius')
					if not radius: radius = _LIGHT_DEFAULT_RADIUS
					node['radius'] = float(radius)

					subdivisions = light.get('subdivisions')
					if not subdivisions: 
						subdivisions = _LIGHT_DEFAULT_SUBDIVISIONS
					node['subdivisions'] = int(subdivisions)
					
					intensity = light.get('intensity')
					if not intensity:
						intensity = _LIGHT_DEFAULT_INTENSITY
					node['intensity'] = int(intensity)

					xstretch = light.get('xstretch')
					if not xstretch: xstretch = 1.0
					ystretch = light.get('ystretch')
					if not ystretch: ystretch = 1.0
					node['stretch'] = float(xstretch), float(ystretch)

					color = light.get('color')
					if not color: color = '%d,%d,%d' % (255, 255, 255)
					node['color'] = ([int(c) for c in color.split(',')])

			else:
				continue
			
			cam_id = light.get('camera_id')
			if not cam_id: cam_id = _LIGHT_DEFAULT_CAM_ID
	
			if not cam_id in self.light_data:
				self.light_data[cam_id] = {}
			if group not in self.light_data[cam_id]:
				self.light_data[cam_id][group] = []
			
			self.light_data[cam_id][group].append(node)
			
		for camera, groups in self.light_data.iteritems():
			print "Lights for camera %s" % camera
			for group, lights in groups.iteritems():
				print group, lights
			
	def parse_sounds(self, layerelt, layer):
		""" create sound emitter
		
		FIXME:
			- FIFE has a hard limit of sound emitters
			  how should we load emitters here?
			- my first thought: collect a list of sound
			  files & data for emitter creation,
			  then let the client decide what to do with it		  
		
		@type	layerelt:	object
		@param	layerelt:	ElementTree layer branch
		@type	layer:	object
		@param	layer:	FIFE layer object
		"""	
		# to be continued		
		pass
		
	def parse_instances(self, layerelt, layer):
		""" create all layers and their instances
		
		@type	layerelt:	object
		@param	layerelt:	ElementTree layer branch
		@type	layer:	object
		@param	layer:	FIFE layer object
		"""			
		instelt = layerelt.find('instances')

		instances = []
		for attr in ('i', 'inst', 'instance'):
			instances.extend(instelt.findall(attr))
		
		for instance in instances:
			_id = instance.get('id')
			if not _id:
				_id = ''
			
			objectID = ''
			for attr in ('o', 'object', 'obj'):
				objectID = instance.get(attr)
				if objectID: break
			if not objectID: self._err('<instance> %s does not specify an object attribute.' % str(objectID))
			objectID = str(objectID)
			
			nspace = ''
			for attr in ('namespace', 'ns'):
				nspace = instance.get(attr)
				if nspace: break
			# try to reuse the previous namespace
			if not nspace and self.nspace:
				nspace = self.nspace
			if not nspace and not self.nspace: self._err('<instance> %s does not specify an object namespace, and no default is available.' % str(objectID))
			nspace = str(nspace)
			self.nspace = nspace

			# check if there is an object for this instance available, if not -> skip this one
			object = self.model.getObject(objectID, nspace)
			if not object:
				print "Object with id=%s, ns=%s could not be found. Omitting..." % (objectID, nspace)
				continue

			x = instance.get('x')
			if x: self.x = x = float(x)
			else: x = self.x

			y = instance.get('y')
			if y: self.y = y = float(y)
			else: y = self.y

			z = instance.get('z')
			if z: z = float(z)
			else: z = 0.0

			inst = layer.createInstance(object, fife.ExactModelCoordinate(x,y,z), _id)

			rotation = 0
			for attr in ('r', 'rotation'):
				rotation = instance.get(attr)
				if rotation: break
			if not rotation:
				angles = object.get2dGfxVisual().getStaticImageAngles()
				if angles:
					rotation = angles[0]
				else:
					rotation = 0
			else:
				rotation = int(rotation)
			inst.setRotation(rotation)

			over_block = instance.get('override_blocking')
			if over_block is not None:
				inst.setOverrideBlocking(bool(over_block))
				blocking = instance.get('blocking')
				if blocking is not None:
					inst.setBlocking(bool(int(blocking)))

			fife.InstanceVisual.create(inst)
			
			stackpos = instance.get('stackpos')
			if stackpos:
				inst.get2dGfxVisual().setStackPosition(int(stackpos))

			if (object.getAction('default')):
				target = fife.Location(layer)
				inst.actRepeat('default', target)

	def parse_cameras(self, mapelt, map):
		""" create all cameras and activate them
		
		FIXME:
			- should the cameras really be enabled here?
			  IMO that's part of the setup within a client
			  (we just _load_ things here)
		
		@type	mapelt:	object
		@param	mapelt:	ElementTree root
		@type	map:	object
		@param	map:	FIFE map object			
		"""				
		if self.callback:		
			tmplist = mapelt.findall('camera')
			i = float(0)

		for camera in mapelt.findall('camera'):
			_id = camera.get('id')
			zoom = camera.get('zoom')
			tilt = camera.get('tilt')
			rotation = camera.get('rotation')
			ref_layer_id = camera.get('ref_layer_id')
			ref_cell_width = camera.get('ref_cell_width')
			ref_cell_height = camera.get('ref_cell_height')
			viewport = camera.get('viewport')
			light_color = camera.get('light_color')

			if not zoom: zoom = 1
			if not tilt: tilt = 0
			if not rotation: rotation = 0

			if not _id: self._err('Camera declared without an id.')
			if not ref_layer_id: self._err(''.join(['Camera ', str(_id), ' declared with no reference layer.']))
			if not (ref_cell_width and ref_cell_height): self._err(''.join(['Camera ', str(_id), ' declared without reference cell dimensions.']))

			try:
				if viewport:
					cam = map.addCamera(str(_id), map.getLayer(str(ref_layer_id)),fife.Rect(*[int(c) for c in viewport.split(',')]))

				else:
					screen = self.engine.getRenderBackend()
					cam = map.addCamera(str(_id), map.getLayer(str(ref_layer_id)),fife.Rect(0,0,screen.getScreenWidth(),screen.getScreenHeight()))
				
				renderer = fife.InstanceRenderer.getInstance(cam)
				renderer.activateAllLayers(map)
				
			except fife.Exception, e:
				print e.getMessage()

			if light_color: cam.setLightingColor(*[float(c) for c in light_color.split(',')])			
			cam.setCellImageDimensions(int(ref_cell_width), int(ref_cell_height))
			cam.setRotation(float(rotation))
			cam.setTilt(float(tilt))
			cam.setZoom(float(zoom))
			
			renderer = fife.InstanceRenderer.getInstance(cam)
			renderer.activateAllLayers(map)
				
			if self.callback:
				i += 1
				self.callback(self.msg['camera'] % str(_id), float( i / len(tmplist) * 0.25 + 0.75 ) )
				
	def create_light_nodes(self, map):
		""" loop through all preloaded lights and create them
		according to their data
		
		@type	map:	object
		@param	map:	FIFE map object
		"""
		cameras = [i.getId() for i in map.getCameras()]
		renderers = {}
		default_cam = map.getCameras()[0].getId()
		
		def add_simple_light(group, renderer, node, data):
			""" add a node as simple light to the renderer 
			
			@type	group:	string
			@param	group:	name of the light group
			@type	renderer:	object
			@param	renderer:	fife.LightRenderer instance
			@type	node:		object
			@param	node:		fife.RendererNode instance
			@type	data:		dict
			@param	data:		all data for the light type creation
			"""
			if not node: return
			if not group: return
			renderer.addSimpleLight(
				group,
				node,
				data['intensity'],
				data['radius'],
				data['subdivisions'],
				data['stretch'][0],
				data['stretch'][1],
				data['color'][0],
				data['color'][1],
				data['color'][2],
				data['blending_src'],
				data['blending_dst'],												
			)
			if data['s_ref'] is not -1:
				add_stencil_test(group, renderer, data)
		def add_animated_lightmap(group, renderer, node, data):
			""" add a node as animated lightmap to the renderer 
			
			@type	group:	string
			@param	group:	name of the light group
			@type	renderer:	object
			@param	renderer:	fife.LightRenderer instance
			@type	node:		object
			@param	node:		fife.RendererNode instance
			@type	data:		dict
			@param	data:		all data for the light type creation
			"""
			if not node: return
			if not group: return
			renderer.addAnimation(
				group,
				node,
				data['animation'],
				data['blending_src'],
				data['blending_dst'],
			)
			if data['s_ref'] is not -1:
				add_stencil_test(group, renderer, data)
		def add_lightmap(group, renderer, node, data):
			""" add a node as lightmap to the renderer 
			
			@type	group:	string
			@param	group:	name of the light group
			@type	renderer:	object
			@param	renderer:	fife.LightRenderer instance
			@type	node:		object
			@param	node:		fife.RendererNode instance
			@type	data:		dict
			@param	data:		all data for the light type creation
			"""
			if not node: return
			if not group: return
			renderer.addImage(
				group,
				node,
				data['image'],
				data['blending_src'],
				data['blending_dst'],
			)
			if data['s_ref'] is not -1:
				add_stencil_test(group, renderer, data)
		def add_stencil_test(group, renderer, data):
			""" add a stencil test to a group 
			
			@type	group:	string
			@param	group:	name of the light group
			@type	renderer:	object
			@param	renderer:	fife.LightRenderer instance
			@type	data:		dict
			@param	data:		all data for the light type creation
			"""
			if not group: return
			renderer.addStencilTest(
				group,
				data['s_ref'],
				data['a_ref'],
			)
			
		def create_node(instance=None, point=None, layer=None):
			""" creates a node of one of these types:
			
				- attached to an instance
				- attached to an instance with offset
				- attached to a point
			
				FIXME:
					- add location node
			
			@type:	instance:	object
			@param	instance:	fife instance object
			@type	point:		tuple
			@param	point:		x,y,z tuple
			@type	layer:		object
			@param	layer:		fife layer object
			"""
			node = None
			if not layer: return node
			
			# node at layer coordinates
			if point and not instance:
				point = fife.Point(point[0], point[1])
				node = fife.RendererNode(point);
			# node with offset
			if instance and point:
				node = fife.RendererNode(instance, layer, fife.Point(point[0], point[1]))
			# node attached to instance
			if instance and not point:
				node = fife.RendererNode(instance, layer)

			if node:
				node.thisown = 0
				
			return node			
		
		def dump_data():
			""" dump all loaded data """
			for camera, groups in self.light_data.iteritems():
				print "Lights for camera %s" % camera
				for group, lights in groups.iteritems():
					print group, lights		
		
		# fetch all renderer instances for available cameras
		for _id in cameras:
			camera = map.getCamera(_id)
			renderers[_id] = fife.LightRenderer.getInstance(camera)
		
		# parse data and create the lights	
		for camera, groups in self.light_data.iteritems():
			for group, lights in groups.iteritems():
				for light in lights:
					instance = None
					layer = map.getLayer(light['layer'])
					if light['instance']:
						instance = layer.getInstance(light['instance'])
					position = light['position']
					node = create_node(instance, position, layer)
					
					# some guards
					if not node: continue
					
					if camera == 'default':
						renderer = renderers[default_cam]
					else:
						renderer = renderers[camera]
						
					if light['type'] == 'simple':
						add_simple_light(group, renderer, node, light)
					elif light['type'] == 'image':
						add_lightmap(group, renderer, node, light)
					elif light['type'] == 'animation':
						add_animated_lightmap(group, renderer, node, light)
				
#		dump_data()
		

