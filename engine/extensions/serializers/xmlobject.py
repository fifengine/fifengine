import fife
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

class ObjectLocation(fife.ResourceLocation):
	def __init__(self, file, node=None):
		fife.ResourceLocation.__init__(self, file)
		self.node = node

class XMLObjectLoader(fife.ObjectLoader):
	def __init__(self, image_pool, anim_pool, model, vfs=None, dataset=None):
		self.image_pool = image_pool
		self.anim_pool = anim_pool
		self.model = model
		self.metamodel = model.getMetaModel()
		self.dataset=dataset
		self.vfs = vfs

	def _err(self, msg):
		raise SyntaxError(''.join(['File: ', self.source, '. ', msg]))

	def loadResource(self, location):
		node = None
		try:
			node = location.node
		except AttributeError:
			pass

		if node == None:
			try:
				f = self.vfs.open(self.source)
			except AttributeError:
				print 'XMLObjectLoader was asked to open a file, but no vfs was given.'
				raise
			tree = ET.parse(f)
			node = tree.getroot()
			if node.tag != 'object':
				self._err(''.join(['Expected <object> tag, but found <', node.tag, '>.']))

		self.parse_object(self.dataset, node)

	def parse_object(self, dataset, object):
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
				return
		else:
			try:
				obj = dataset.createObject(str(id))
				fife.ObjectVisual.create(obj)
			except fife.Exception, e:
				print e.getMessage()
				print 'The object ' + str(id) + ' already exists! Ignoring object definition.'
				return

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


