import fife
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

class XMLDatasetLoader(fife.DatasetLoader):
	def __init__(self, image_pool, anim_pool, model, vfs):
#		fife.DatasetLoader.__init__(self)
		self.image_pool = image_pool
		self.anim_pool = anim_pool
		self.model = model
		self.metamodel = model.getMetaModel()
		self.vfs = vfs
#		self.thisown = 0

	def _err(self, msg):
		raise SyntaxError(''.join(['File: ', self.source, '. ', msg]))
	
	def loadResource(self, location):
		self.source = location.getFilename()
		f = self.vfs.open(self.source)
		tree = ET.parse(f)
		root = tree.getroot()

		return self.parse_dataset(root)

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

	def parse_dataset(self, datelt):
		if not datelt:
			self._err('No <dataset> element found at top level of dataset file definition.')

		type = datelt.get('type')
		id = datelt.get('id')

		if not type: self._err('Dataset declared with no type attribute.')
		if type != 'Embedded': self._err(''.join(['Invalid dataset type: ', type]))

		dataset = None
		try:
			dataset = self.metamodel.createDataset(str(id))
		except fife.Exception, e:
			print e.getMessage()
			print ''.join(['File: ', self.source, '. The dataset ', str(id), ' already exists! Returning dataset definition.'])
			return self.metamodel.getDatasets('id', str(id))[0]

		dataset.setSource(str(self.source))

		self.parse_metadata(datelt, dataset)
		self.parse_inclusions(datelt, dataset)
		self.parse_objects(datelt, dataset)

		return dataset

	def parse_inclusions(self, datelt, dataset):
		for include in datelt.findall('dataset'):
			type,source = include.get('type'),include.get('source')
			if not type: self._err('<dataset> inclusion declared with no type attribute.')

			dataset_loader = XMLDatasetLoader(self.image_pool, self.anim_pool, self.model, self.vfs)
			inc_obj = dataset_loader.loadResource(fife.ResourceLocation(source))
			if inc_obj:
				dataset.addDataset(inc_obj)

	def parse_objects(self, datelt, dataset):

		for object in datelt.findall('object'):
			id = object.get('id')
			parent = object.get('parent')
			blocking = object.get('blocking')
			static = object.get('static')
			pather = object.get('pather')

			if not pather: pather = self.model.getPather('RoutePather')

			if not id: self._err('<object> declared without an id attribute.')

			obj = None
			if parent:
				try:
					query = self.metamodel.getObjects('id', str(parent))
					if len(query) == 0: self._err(''.join(['No objects found with identifier', str(parent), '.']))
					if len(query) > 1: print ''.join(['Warning: ', str(len(query)), ' objects found with identifier ', str(parent), '.'])
					parent = query[0]

					obj = dataset.createObject(str(id), parent)
					fife.ObjectVisual.create(obj)
				except fife.Exception, e:
					print e.getMessage()
					print 'The object ' + str(id) + ' already exists! Ignoring object definition.'
					continue
			else:
				try:
					obj = dataset.createObject(str(id))
					fife.ObjectVisual.create(obj)
				except fife.Exception, e:
					print e.getMessage()
					print 'The object ' + str(id) + ' already exists! Ignoring object definition.'
					continue

			obj.setBlocking(bool(blocking))
			obj.setStatic(bool(static))
			obj.setPather(pather)

			self.parse_images(object, obj)
			self.parse_actions(object, obj)

	def parse_images(self, objelt, object):
		for image in objelt.findall('image'):	
			source = image.get('source')
			direction = image.get('direction')
			x_offset = image.get('x_offset')
			y_offset = image.get('y_offset')

			if not direction: direction = 0

			if not source: self._err('<image> declared without a source attribute.')

			try:
				id = self.image_pool.addResourceFromFile(str(source))	
				object.get2dGfxVisual().addStaticImage(int(direction), id)
				if (x_offset or y_offset):
					img = self.image_pool.getImage(id)
					img.setXShift(int(x_offset))
					img.setYShift(int(y_offset))
			except fife.Exception,e:
				print e.getMessage()
				raise

	def parse_actions(self, objelt, object):
		for action in objelt.findall('action'):
			id = action.get('id')
			if not id: self._err('<action> declared without an id attribute.')
	
			act_obj = None
			try:
				act_obj = object.createAction(str(id))
				fife.ActionVisual.create(act_obj)
			except fife.Exception, e:
				print e.getMessage()
				print 'The action ' + str(id) + ' already exists! Ignoring action definition.'
				continue

			self.parse_animations(action, act_obj)

	def parse_animations(self, actelt, action):
		for anim in actelt.findall('animation'):
			source = anim.get('source')
			direction = anim.get('direction')

			if not direction: direction = 0
	
			if not source: self._err('Animation declared with no source location.')

			try:
				anim_id = self.anim_pool.addResourceFromFile(str(source))
				animation = self.anim_pool.getAnimation(anim_id)
				action.get2dGfxVisual().addAnimation(int(direction), anim_id)
				action.setDuration(animation.getDuration())
			except fife.Exception,e:
				print e.getMessage()
				raise

