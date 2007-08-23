import fife
from xml.sax import saxutils

class ModelLoader(saxutils.DefaultHandler):
	def __init__(self):
		self.SModel, self.SMap, self.SElevation, self.SLayer, self.SSparseData, self.SDenseData = range(6)

		self.model = fife.Model()	
		self.state = self.Model

	def startElement(self, name, attrs):
		if (name == 'map'):
			self.current = self.model.addMap()	
			self.state = self.SMap

		elif (name == 'metadata'):
			if (self.state == self.SMap) || (self.state == self.SElevation) || (self.state == self.SLayer):
				pass

		elif (name == 'dataset'):
			if self.state == self.SMap:
				pass

		elif (name == 'elevation'):
			self.current = self.current
			self.state = self.SElevation

		elif (name == 'layer'):
			self.state = self.SLayer

		elif (name == 'sparse_insts'):
			self.state = SSparseData

		elif (name == 'dense_insts'):
			self.state = SDenseData

		elif (name == 'inst'):
			if (self.state == self.SSparseData):
				pass
			elif (self.state == self.SDenseData):
				pass

	def endElement(self, name):
