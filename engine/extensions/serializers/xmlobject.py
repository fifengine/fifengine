import fife, sys
from traceback import print_exc

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
		self.source = None

	def loadResource(self, location):
		self.source = location
		node = None
		try:
			node = location.node
		except AttributeError:
			pass

		if node == None:
			try:
				f = self.vfs.open(self.source)
			except AttributeError:
				print 'XMLObjectLoader was asked to open a file %s, but no vfs was given.' % self.source.getFilename()
				raise
			tree = ET.parse(f)
			node = tree.getroot()
			if node.tag != 'object':
				self._err(''.join(['Expected <object> tag, but found <', node.tag, '>.']))

		try:
			self.parse_object(self.dataset, node)
		except:
			print "\n\n=================== LOADER EXCEPTION INFO BEGIN ======================"
			print "Exception : ", sys.exc_info()[0]
			print "Error while processing file " + self.source.getFilename()
			print "----------------------------------------------------------------"
			print_exc()
			print "----------------------------------------------------------------"
			print "XML dump:"
			ET.dump(node)
			print "==================== LOADER EXCEPTION INFO END =======================\n\n"
			raise

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
				if len(query) > 1: 
					self._print_err('Warning: %d objects found with identifier %s.' % (str(len(query)), str(parent)))
				parent = query[0]

				obj = dataset.createObject(str(id), parent)
				fife.ObjectVisual.create(obj)
			except fife.Exception, e:
				self._print_err(e.getMessage() + '\n\tThe object ' + str(id) + ' already exists! Ignoring object definition.')
				return
		else:
			try:
				obj = dataset.createObject(str(id))
				fife.ObjectVisual.create(obj)
			except fife.Exception, e:
				self._print_err(e.getMessage() + '\n\tThe object ' + str(id) + ' already exists! Ignoring object definition.')
				return

		obj.setBlocking(bool(blocking))
		obj.setStatic(bool(static))
		obj.setPather(pather)

		self.parse_images(object, obj)
		self.parse_actions(object, obj)

	def parse_images(self, objelt, object):
		for image in objelt.findall('image'):
			source = image.get('source')
			if not source:
				self._err('<image> declared without a source attribute.')
			
			direction = image.get('direction', 0)
			x_offset = image.get('x_offset', 0)
			y_offset = image.get('y_offset', 0)

			id = self.image_pool.addResourceFromFile(str(source))	
			object.get2dGfxVisual().addStaticImage(int(direction), id)
			img = self.image_pool.getImage(id)
			img.setXShift(int(x_offset))
			img.setYShift(int(y_offset))

	def parse_actions(self, objelt, object):
		for action in objelt.findall('action'):
			id = action.get('id')
			if not id: self._err('<action> declared without an id attribute.')
	
			act_obj = None
			act_obj = object.createAction(str(id))
			fife.ActionVisual.create(act_obj)
			self.parse_animations(action, act_obj)

	def parse_animations(self, actelt, action):
		for anim in actelt.findall('animation'):
			source = anim.get('source')
			if not source: 
				self._err('Animation declared with no source location.')
			
			direction = anim.get('direction', 0)
			anim_id = self.anim_pool.addResourceFromFile(str(source))
			animation = self.anim_pool.getAnimation(anim_id)
			action.get2dGfxVisual().addAnimation(int(direction), anim_id)
			action.setDuration(animation.getDuration())

