# Maploader for FIFE's native xml format.
# See the documentation for loadMapFile at the bottom of this file for use. 

from xml.sax import make_parser 
from xml.sax import saxutils, handler

import fife

FORMAT = '1.0'

class ModelLoader(handler.ContentHandler):

	def __init__(self, engine, source, content, state = 0, datastate = 0):
		self.SModel, self.SDataset, self.SMetadata, self.SMap, self.SElevation, self.SLayer, self.SInstances, self.SInstance, self.SObject, self.SAction = range(10)

		self.engine = engine
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()
		self.pool = self.engine.getImagePool()
		self.anim_pool = self.engine.getAnimationPool()

		self.source = source
		self.content_path = content

		if (state):
			self.state = state
			if (state == self.SMap):
				self.map = datastate
			elif (state == self.SDataset):
				self.dataset = datastate
			else:
				raise SyntaxError(''.join(['Invalid initialization state in file: ', self.source]))
		else:
			self.state = self.SModel

		self.stack = [ self.SModel ] 
		self.datastack = [ ]

		self.chars = ""
		self.construct = False 

	def _err(self, msg):
		raise SyntaxError(''.join(['File: ', self.source, ' . ', msg]))

	def startElement(self, name, attrs):
		try:
			getattr(self, "parse_"+name)(attrs)
		except AttributeError:
			print ''.join(['Invalid xml tag: ' , name , '. Ignoring...'])
		except SyntaxError, e:
			print e
			print 'Attempting to continue.'
		except fife.Exception, e:
			print 'Unexpected error: ', e.getMessage()
			raise

	def parse_map(self, attrs):
		if self.construct:
			self._err('Multiple maps/datasets declared.')
		self.construct = True 
		if not self.state == self.SModel: self.err('<map> tag declared in invalid section.')
		id = 0
		for attrName in attrs.keys():
			if(attrName == "format"):
				if not attrs.get(attrName) == FORMAT:
					self._err(''.join(['This file has format ', attrs.get(attrName), ' but these loaders are for format ', FORMAT]))
			if(attrName == "id"):
				id = attrs.get(attrName)

		if not id: self._err('Map declared without an identifier.')

		try:
			self.map = self.model.createMap(str(id))
		except fife.Exception, e: # NameClash appears as general fife.Exception; any ideas?
			print e.getMessage()
			print 'The map ' + str(id) + ' already exists! Using existing map.'
			self.map = self.model.getMaps('id', str(id))[0]

		self.state = self.SMap

	def parse_metadata(self, attrs):
		if not self.state in (self.SMap, self.SElevation, self.SLayer, self.SDataset,
			self.SObject, self.SAction, self.SInstance): self._err('<metadata> declared in an invalid section.')

		if (self.state == self.SMap):
			self.datastack.append(self.map)
		elif (self.state == self.SElevation):
			self.datastack.append(self.elevation)
		elif (self.state == self.SLayer):
			self.datastack.append(self.layer)
		elif (self.state == self.SDataset):
			self.datastack.append(self.dataset)
		elif (self.state == self.SObject):
			self.datastack.append(self.object)
		elif (self.state == self.SAction):
			self.datastack.append(self.action)
		elif (self.state == self.SInstance):
			self.datastack.append(self.instance)

		self.stack.append(self.state)
		self.state = self.SMetadata

	def parse_param(self, attrs):
		if not self.state == self.SMetadata: self._err('<param> tag found outside <metadata> section.')

		self.param_name = ''
		self.value = ''
		for attrName in attrs.keys():
			if attrName == "name":
				self.param_name = str(attrs.get(attrName))
			if attrName == "value":
				self.value = str(attrs.get(attrName))
			
		if not self.param_name: self._err('<param> declared without a name')

	def parse_dataset(self, attrs):
		if not self.state in (self.SMap, self.SModel, self.SDataset): self._err('<dataset> tag found in an invalid section.')

		type = 0
		source = 0
		id = 0
		for attrName in attrs.keys():
			if (attrName == "type"):
				type = attrs.get(attrName)
			elif (attrName == "source"):
				source = attrs.get(attrName)
			elif (attrName == "id"):
				id = attrs.get(attrName)

		if not type: self._err('Dataset declared with no type attribute.')

		if type == 'XML':
			if not source: self._err('External dataset declared with no source.')
			parser = make_parser()

			if self.content_path:
				source = '/'.join([self.content_path, source])

			handler = 0
			if (self.state == self.SMap):
				handler = ModelLoader(self.engine, source, self.content_path, self.SMap, self.map)
			elif (self.state == self.SDataset):
				handler = ModelLoader(self.model, source, self.content_path, self.SDataset, self.dataset)
			else:
				handler = ModelLoader(self.model, source, self.content_path)

			parser.setContentHandler(handler)
			parser.parse(open(source))

		elif (type == 'Embedded'):
			if self.construct: self._err('Multiple maps/datasets declared in the same file.')
			self.construct = True

			if not id: self._err('Dataset declared without an identifier.')

			try:
				dataset = self.metamodel.createDataset(str(id))
			except fife.Exception, e:
				print e.getMessage()
				print ''.join(['File: ', self.source, '. The dataset ', str(id), ' already exists! Using existing dataset.'])
				dataset = self.metamodel.getDatasets('id', str(id))[0]

			if (self.state == self.SModel):
				self.dataset = dataset

			elif (self.state == self.SMap):
				self.dataset = dataset
				self.map.addDataset(self.dataset)
				
			elif (self.state == self.SDataset):
				self.datastack.append(self.dataset)
				self.dataset = dataset

			self.dataset.setSource(str(self.source))

		else:
			self._err('Invalid dataset type attribute. Datasets must be of type XML or Embedded.')

		self.stack.append(self.state)
		self.state = self.SDataset

	def parse_object(self, attrs):
		if not self.state == self.SDataset: self._err('<object> tag found outside a <dataset> section.')

		self.state = self.SObject

		id = 0
		parent = 0
		blocking = 0
		static = 0
		pather = None
		for attrName in attrs.keys():
			if (attrName == "id"):
				id = attrs.get(attrName)
			elif (attrName == "parent"):
				query = self.metamodel.getObjects("id", str(attrs.get(attrName)))
				if len(query) != 1: self._err(''.join([str(len(query)), 'objects found with identifier ', str(id), '.']))
				parent = query[0]
			elif (attrName == "blocking"):
				blocking = int(attrs.get(attrName))
			elif (attrName == "static"):
				static = int(attrs.get(attrName))
			elif (attrName == "pather"):
				pather = self.model.getPather( str(attrs.get(attrName))  )
			if not pather:
				pather = self.model.getPather("RoutePather")

		if not id: self._err('Object declared without an identifier (id).')

		if (parent):
			try:
				self.object = self.dataset.createObject(str(id), parent)
				fife.ObjectVisual.create(self.object)
			except fife.Exception, e:
				print e.getMessage()
				print 'The object ' + str(id) + ' already exists! Using existing object definition.'
				self.object = self.dataset.getObjects('id', str(id))[0]
		else:
			try:
				self.object = self.dataset.createObject(str(id))
				fife.ObjectVisual.create(self.object)
			except fife.Exception, e:
				print e.getMessage()
				print 'The object ' + str(id) + ' already exists! Using existing object definition.'
				self.object = self.dataset.getObjects('id', str(id))[0]

		self.object.setBlocking(blocking)
		self.object.setStatic(static)
		self.object.setPather(pather)

	def parse_image(self, attrs):
		if self.state != self.SObject: self._err('<image> tag found outside an <object> section.')

		source = 0
		direction = 0
		x_offset = 0
		y_offset = 0
		for attrName in attrs.keys():
			if (attrName == "source"):
				source = attrs.get(attrName)
			elif (attrName == "direction"):
				direction = attrs.get(attrName)
			elif (attrName == "x_offset"):
				x_offset = int(attrs.get(attrName))
			elif (attrName == "y_offset"):
				y_offset = int(attrs.get(attrName))

		if not source: self._err('Image declared with no source.')

		try:
			id = self.pool.addResourceFromFile(str(source))	
			self.object.get2dGfxVisual().addStaticImage(int(direction), id)
			if (x_offset or y_offset):
				img = self.pool.getImage(id)
				img.setXShift(x_offset)
				img.setYShift(y_offset)
		except fife.Exception,e:
			print e.getMessage()

	def parse_action(self, attrs):
		if self.state != self.SObject: self._err('<action> tag found outside an <object> section.')

		self.state = self.SAction

		id = 0
		for attrName in attrs.keys():
			if (attrName == "id"):
				id = attrs.get(attrName)

		if not id: self._err('Action declared without an identifier (id).')

		try:
			self.action = self.object.createAction(str(id))
			fife.ActionVisual.create(self.action)
		except fife.Exception, e:
			print e.getMessage()
			print 'The action ' + str(id) + ' already exists! Using existing action definition.'
			self.action = self.object.getAction(str(id))

	def parse_animation(self, attrs):
		if self.state != self.SAction: self._err('<animation> tag found outside an <action> section.')

		source = 0
		direction = 0
		for attrName in attrs.keys():
			if (attrName == "direction"):
				direction = attrs.get(attrName)
			elif (attrName == "source"):
				source = attrs.get(attrName)
	
		if not source: self._err('Animation declared with no source location.')

		try:
			anim_id = self.anim_pool.addResourceFromFile(str(source))
			animation = self.anim_pool.getAnimation(anim_id)
			self.action.get2dGfxVisual().addAnimation(int(direction), anim_id)
			self.action.setDuration(animation.getDuration())
		except fife.Exception,e:
			print e.getMessage()

	def parse_elevation(self, attrs):
		if self.state != self.SMap: self._err('<elevation> tag found outside a <map> section.')

		id = 0
		for attrName in attrs.keys():
			if (attrName == "id"):
				id = attrs.get(attrName)

		if not id: self._err('Elevation declared without an identifier.')

		try:
			self.elevation = self.map.createElevation(str(id))
		except fife.Exception, e:
			print e.getMessage()
			print 'The elevation ' + str(id) + ' already exists! Using existing elevation.'
			self.elevation = self.map.getElevations('id', str(id))[0]

		self.state = self.SElevation

	def parse_camera(self, attrs):
		if self.state != self.SElevation: self._err('<camera> tag found outside an <elevation> section.')

		id = 0
		zoom = 1
		tilt = 0
		rotation = 0
		ref_layer_id = 0
		ref_cell_width = 0
		ref_cell_height = 0
		viewport = 0
		for attrName in attrs.keys():
			if (attrName == 'id'):
				id = attrs.get(attrName)
			elif (attrName == 'zoom'):
				zoom = attrs.get(attrName)
			elif (attrName == 'tilt'):
				tilt = attrs.get(attrName)
			elif (attrName == 'rotation'):
				rotation = attrs.get(attrName)
			elif (attrName == 'ref_layer_id'):
				ref_layer_id = attrs.get(attrName)
			elif (attrName == 'ref_cell_width'):
				ref_cell_width = attrs.get(attrName)
			elif (attrName == 'ref_cell_height'):
				ref_cell_height = attrs.get(attrName)
			elif (attrName == 'viewport'):
				viewport = attrs.get(attrName)

		if not id: self._err('Camera declared without an id.')
		if not viewport: self._err(''.join(['Camera ', str(id), ' declared without a viewport.']))
		if not ref_layer_id: self._err(''.join(['Camera ', str(id), ' declared with no reference layer.']))
		if not (ref_cell_width and ref_cell_height): self._err(''.join(['Camera ', str(id), ' declared without reference cell dimensions.']))

		try:
			camera = self.engine.getView().addCamera(str(id), self.elevation.getLayers('id', str(ref_layer_id))[0],fife.Rect(*[int(c) for c in viewport.split(',')]),fife.ExactModelCoordinate(0,0,0))

			camera.setCellImageDimensions(int(ref_cell_width), int(ref_cell_height))
			camera.setRotation(float(rotation))
			camera.setTilt(float(tilt))
			camera.setZoom(float(zoom))
		except fife.Exception, e:
			print e.getMessage()

	def parse_layer(self, attrs):
		if self.state != self.SElevation: self._err('<layer> tag found outside an <elevation> section.')

		id = 0
		cellgrid = 0
		x_scale = 1.0
		y_scale = 1.0
		rotation = 0.0
		x_offset = 0.0
		y_offset = 0.0
		pathing = "cell_edges_only"
		grid_type = ""

		for attrName in attrs.keys():
			if (attrName == "grid_type"):
				t = attrs.get(attrName)
				if t in ("square", "hexagonal"):
					grid_type = t
			
			elif (attrName == "pathing"):
				t = attrs.get(attrName)
				if t in ("cell_edges_only", "cell_edges_and_diagonals", "freeform"):
					pathing = t

			elif (attrName == "x_scale"):
				x_scale = eval(attrs.get(attrName))

			elif (attrName == "y_scale"):
				y_scale = eval(attrs.get(attrName))
			
			elif (attrName == "rotation"):
				rotation = eval(attrs.get(attrName))

			elif (attrName == "x_offset"):
				x_offset = eval(attrs.get(attrName))

			elif (attrName == "y_offset"):
				y_offset = eval(attrs.get(attrName))

			elif (attrName == "id"):
				id = attrs.get(attrName)
					
		if not id: self._err('Layer declared with no identifier.')
		if not grid_type: self._err(''.join(['Layer ', str(id), ' has no grid type defined.']))
		
		allow_diagonals = pathing == "cell_edges_and_diagonals"
		if grid_type == "square":
			cellgrid = fife.SquareGrid(allow_diagonals)
		else:
			cellgrid = fife.HexGrid(allow_diagonals)
		cellgrid.thisown = 0

		cellgrid.setRotation(rotation)
		cellgrid.setXScale(x_scale)
		cellgrid.setYScale(y_scale)
		cellgrid.setXShift(x_offset)
		cellgrid.setYShift(y_offset)

		try:
			self.layer = self.elevation.createLayer(str(id), cellgrid)
		except fife.Exception, e:
			print e.getMessage()
			print 'The layer ' + str(id) + ' already exists! Using existing layer.'
			self.layer = self.elevation.getLayers('id', str(id))[0]
		
		strgy = fife.CELL_EDGES_ONLY
		if pathing == "cell_edges_and_diagonals":
			strgy = fife.CELL_EDGES_AND_DIAGONALS
		if pathing == "freeform":
			strgy = fife.FREEFORM
		self.layer.setPathingStrategy(strgy)
		
		self.state = self.SLayer

	def parse_instances(self, attrs):
		if self.state != self.SLayer: self._err('<instances> tag found outside a <layer> section.')

		self.state = self.SInstances
		self.x = self.y = 0

	def parse_i(self, attrs):
		self.parse_instance(attrs)

	def parse_inst(self, attrs):
		self.parse_instance(attrs)

	def parse_instance(self, attrs):
		if self.state != self.SInstances: self._err('<instance> section found outside a <instances> section.')

		self.oldstate = self.state
		self.state = self.SInstance

		objectID = attrs.get("object")
		if (not objectID):
			objectID = attrs.get("obj")
		if (not objectID):
			objectID = attrs.get("o")

		if not objectID: self._err('<instance> does not specify an object.')

		query = self.metamodel.getObjects("id", str(objectID))
		if len(query) != 1: self._err(''.join([str(len(query)), 'objects found with identifier ', str(id), '.']))
		object = query[0]

		x = attrs.get("x")
		y = attrs.get("y")
		z = attrs.get("z")
		stackpos = attrs.get("stackpos")
		id = attrs.get("id")

		if (x):
			x = float(x)
			self.x = x
		else:
			self.x = self.x + 1
			x = self.x

		if (y):
			y = float(y)
			self.y = y
		else:
			y = self.y
		
		if (z):
			z = float(z)
		else:
			z = 0.0
		
		if not (id):
			id = ''
		else:
			id = str(id)
		
		inst = self.layer.createInstance(object, fife.ExactModelCoordinate(x,y,z), id)
		fife.InstanceVisual.create(inst)
		if (stackpos):
			inst.get2dGfxVisual().setStackPosition(int(stackpos))
			
		if (object.getAction("default")):
			target = fife.Location()
			target.setLayer(self.layer)
			inst.act("default", target, True)
		self.instance = inst

	def characters(self, ch):
		self.chars += ch

	def endElement(self, name):
		try:
			getattr(self, "finish_"+name)()
		except AttributeError:
			print ''.join(['Invalid xml close tag: ' , name , '. Ignoring...'])
		except fife.Exception, e:
			print 'Unexpected error: ', e.getMessage()
			raise

	def finish_metadata(self):
		self.state = self.stack.pop()

	def finish_param(self):
		assert len(self.datastack) > 0
		if self.value:
			self.datastack[-1].set(self.param_name, self.value)
		else:
			self.datastack[-1].set(self.param_name, str(self.chars).strip())

		self.chars = ""

	def finish_dataset(self):
		self.state = self.stack.pop()
		if (self.state == self.SDataset):
			assert len(self.datastack) > 0, "Corrupted dataset stack."
			self.dataset = self.datastack.pop()

	def finish_object(self):
		self.state = self.SDataset

	def finish_image(self):
		pass

	def finish_action(self):
		self.state = self.SObject

	def finish_animation(self):
		pass

	def finish_map(self):
		self.state = self.SModel

	def finish_elevation(self):
		self.state = self.SMap

	def finish_camera(self):
		pass

	def finish_layer(self):
		self.state = self.SElevation

	def finish_instances(self):
		self.state = self.SLayer

	def finish_i(self):
		self.finish_instance()

	def finish_inst(self):
		self.finish_instance()

	def finish_instance(self):
		self.state = self.oldstate


# This is the entire of the loading API. Just call this function to load a map.
#		path - the path of the map to load
#		engine - a reference to the engine
#		content - (optional) reference to the content path.
def loadMapFile(path, engine, content = ''):
	parser = make_parser()
	handler = ModelLoader(engine, path, content)
	parser.setContentHandler(handler)

	if content != '':
		engine.getVFS().addNewSource(content)

	parser.parse(open(path))

	return handler.map
