from xml.sax.saxutils import XMLGenerator
from xml.sax.xmlreader import AttributesNSImpl

import fife

MAPFORMAT = '1.0'

class ModelSaver:

	def __init__(self, filepath, engine, state = 0, datastate = 0):
		self.SModel, self.SDataset, self.SMetadata, self.SMap, self.SElevation, self.SLayer, self.SInstances, self.SObject, self.SAction = range(9)

		self.engine = engine
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()
		self.pool = self.engine.getImagePool()
		self.anim_pool = self.engine.getAnimationPool()

		if (state):
			self.state = state
			if (state == self.SMap):
				self.map = datastate
			elif (state == self.SDataset):
				self.dataset = datastate
			else:
				assert 0, "Invalid initialization state."
		else:
			self.state = self.SModel

		self.stack = [ self.SModel ] 
		self.datastack = [ ]

		self.file = open(filepath, 'w')
		self.xmlout = XMLGenerator(self.file, 'ascii')
		self.xmlout.startDocument()

		self.indent_level = ''

	def startElement(self, name, attrs):
		self.file.write(self.indent_level)
		self.xmlout.startElementNS((None, name), name, attrs)
		self.file.write('\n')
		self.indent_level = self.indent_level + '\t'

	def endElement(self, name):
		self.indent_level = self.indent_level[0:(len(self.indent_level) - 1)]
		self.file.write(self.indent_level)
		self.xmlout.endElementNS((None, name), name)
		self.file.write('\n')

	def write_map(self, map):
		assert self.state == self.SModel, "Declaration of <map> not at the top level."

		attr_vals = {
			(None, 'id'): map.Id(),
			(None, 'format'): MAPFORMAT,
		}
		attr_names = {
			(None, 'id'): 'id',
			(None, 'format'): 'format',
		}
		attrs = AttributesNSImpl(attr_vals, attr_names)
		self.startElement('map', attrs)
		self.state = self.SMap
		self.write_metadata(map)
		self.write_datasets(map)
		self.write_elevations(map)
		self.endElement('map')

	def write_metadata(self, parent):
		attrs = AttributesNSImpl({}, {})
		self.startElement('metadata', attrs)
		list = parent.listFields()

		for i in range(0, len(list)):
			type = parent.getTypeName(list[i])

			attr_vals = {
				(None, 'type'): type,
				(None, 'name'): list[i],
			}
			attr_names = {
				(None, 'type'): 'type',
				(None, 'name'): 'name',
			}
			attrs = AttributesNSImpl(attr_vals, attr_names)
			self.file.write(self.indent_level)
			self.xmlout.startElementNS((None, 'param'), 'param', attrs)

			if (type == 'id'):
				self.xmlout.characters(str(parent.get_int(list[i])))
			elif (type == 'text'):
				self.xmlout.characters(parent.get_string(list[i]))

			self.xmlout.endElementNS((None, 'param'), 'param')
			self.file.write('\n')

		self.endElement('metadata')

	def write_datasets(self, parent):
		pass

	def write_elevations(self, map):
		pass

	def write_layers(self, elevation):
		pass

	def flush(self):
		self.xmlout.endDocument()
		self.file.close()

def saveMapFile(path, engine, map):
	writer = ModelSaver(path, engine)
	writer.write_map(map)
