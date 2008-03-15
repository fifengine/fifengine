import fife
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

from xmlobject import *

class XMLDatasetLoader(fife.DatasetLoader):
	def __init__(self, image_pool, anim_pool, model, vfs):
		self.image_pool = image_pool
		self.anim_pool = anim_pool
		self.model = model
		self.metamodel = model.getMetaModel()
		self.vfs = vfs

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
			print ''.join(['File: ', self.source, '. The dataset ', str(id), ' already exists! Ignoring dataset definition.'])
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

	def parse_objects(self, datelt, set):
		loader = XMLObjectLoader(self.image_pool, self.anim_pool, self.model, dataset=set)
		for object in datelt.findall('object'):
			loader.loadResource(ObjectLocation(self.source, object))
