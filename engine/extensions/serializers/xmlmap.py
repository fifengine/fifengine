import fife
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

from xmldataset import XMLDatasetLoader

FORMAT = '1.0'

class XMLMapLoader(fife.MapLoader):
	def __init__(self, engine):
		fife.MapLoader.__init__(self)
		self.thisown = 0

		self.engine = engine
		self.vfs = self.engine.getVFS()
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()
		self.pool = self.engine.getImagePool()
		self.anim_pool = self.engine.getAnimationPool()

		self.dataset_loader = XMLDatasetLoader(self.pool, self.anim_pool, self.model, self.vfs)

	def _err(self, msg):
		raise SyntaxError(''.join(['File: ', self.source, ' . ', msg]))

	def loadResource(self, location):
		self.source = location.getFilename()
		f = self.vfs.open(self.source)
		tree = ET.parse(f)
		root = tree.getroot()

		return self.parse_map(root)

	def parse_map(self, mapelt):
		if not mapelt:
			self._err('No <map> element found at top level of map file definition.')
		id,format = mapelt.get('id'),mapelt.get('format')

		if not format == FORMAT: self._err(''.join(['This file has format ', format, ' but this loader has format ', FORMAT]))
		if not id: self._err('Map declared without an identifier.')
		
		map = None
		try:
			map = self.model.createMap(str(id))
		except fife.Exception, e: # NameClash appears as general fife.Exception; any ideas?
			print e.getMessage()
			print ''.join(['File: ', self.source, '. The map ', str(id), ' already exists! Ignoring map definition.'])
			return None

		self.parse_metadata(mapelt, map)
		self.parse_datasets(mapelt, map)
		self.parse_layers(mapelt, map)	
		self.parse_cameras(mapelt, map)

		return map

	def parse_metadata(self, element, attr_object):
		metelt = element.find('metadata')
		if not metelt: return

		for param in metelt.findall('param'):
			name,value = param.get('name'),param.get('value')
			if not name: self._err('<param> declared without a name attribute')
			if value:
				attr_object.set(name,value)
			else:
				attr_object.set(name,param.text)	

	def parse_datasets(self, mapelt, map):
		datasets = mapelt.findall('dataset')

		for dataset in datasets:
			type,source = dataset.get('type'),dataset.get('source')
			if not type: self._err('<dataset> inclusion declared with no type attribute.')

			dat_obj = self.dataset_loader.loadResource(fife.ResourceLocation(source))
			map.addDataset(dat_obj)

	def parse_layers(self, mapelt, map):
		for layer in mapelt.findall('layer'):
			id = layer.get('id')
			grid_type = layer.get('grid_type')
			x_scale = layer.get('x_scale')
			y_scale = layer.get('y_scale')
			rotation = layer.get('rotation')
			x_offset = layer.get('x_offset')
			y_offset = layer.get('y_offset')
			pathing = "cell_edges_only"

			if not x_scale: x_scale = 1.0
			if not y_scale: y_scale = 1.0
			if not rotation: rotation = 0.0
			if not x_offset: x_offset = 0.0
			if not y_offset: y_offset = 0.0
			if not pathing: pathing = "cell_edges_only"

			if not id: self._err('<layer> declared with no id attribute.')
			if not grid_type: self._err(''.join(['Layer ', str(id), ' has no grid_type attribute.']))

			allow_diagonals = pathing == "cell_edges_and_diagonals"
			if grid_type == "square":
				cellgrid = fife.SquareGrid(allow_diagonals)
			else:
				cellgrid = fife.HexGrid(allow_diagonals)
			cellgrid.thisown = 0

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
				print 'The layer ' + str(id) + ' already exists! Ignoring this layer.'
				continue
		
			strgy = fife.CELL_EDGES_ONLY
			if pathing == "cell_edges_and_diagonals":
				strgy = fife.CELL_EDGES_AND_DIAGONALS
			if pathing == "freeform":
				strgy = fife.FREEFORM
			layer_obj.setPathingStrategy(strgy)

			self.parse_metadata(layer, layer_obj)
			self.parse_instances(layer, layer_obj)
		
	def parse_instances(self, layerelt, layer):
		instelt = layerelt.find('instances')

		instances = instelt.findall('i')
		instances.extend(instelt.findall('inst'))
		instances.extend(instelt.findall('instance'))
		for instance in instances:

			objectID = instance.get('object')
			if (not objectID):
				objectID = instance.get('obj')
			if (not objectID):
				objectID = instance.get('o')

			if not objectID: self._err('<instance> does not specify an object attribute.')

			query = self.metamodel.getObjects('id', str(objectID))
			if len(query) != 1: self._err(''.join([str(len(query)), 'objects found with identifier ', str(id), '.']))
			object = query[0]

			x = instance.get('x')
			y = instance.get('y')
			z = instance.get('z')
			stackpos = instance.get('stackpos')
			id = instance.get('id')

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
		
			inst = layer.createInstance(object, fife.ExactModelCoordinate(x,y,z), str(id))
			fife.InstanceVisual.create(inst)
			if (stackpos):
				inst.get2dGfxVisual().setStackPosition(int(stackpos))

			if (object.getAction('default')):
				target = fife.Location(layer)
				inst.act('default', target, True)

			self.parse_metadata(instance, inst)

	def parse_cameras(self, mapelt, map):
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
			if not viewport: self._err(''.join(['Camera ', str(id), ' declared without a viewport.']))
			if not ref_layer_id: self._err(''.join(['Camera ', str(id), ' declared with no reference layer.']))
			if not (ref_cell_width and ref_cell_height): self._err(''.join(['Camera ', str(id), ' declared without reference cell dimensions.']))

			try:
				cam = self.engine.getView().addCamera(str(id), map.getLayers('id', str(ref_layer_id))[0],fife.Rect(*[int(c) for c in viewport.split(',')]),fife.ExactModelCoordinate(0,0,0))

				cam.setCellImageDimensions(int(ref_cell_width), int(ref_cell_height))
				cam.setRotation(float(rotation))
				cam.setTilt(float(tilt))
				cam.setZoom(float(zoom))
			except fife.Exception, e:
				print e.getMessage()
