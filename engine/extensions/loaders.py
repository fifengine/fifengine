from xml.sax import make_parser 
from xml.sax import saxutils

import fife

class ModelLoader(saxutils.DefaultHandler):
	def __init__(self):
		self.SModel, self.SDataset, self.SMetadata, self.SMap, self.SElevation, self.SLayer, self.SInstances, self.SObject, self.SAction = range(9)

		self.model = fife.Model()	
		self.metamodel = fife.MetaModel()
		self.state = self.SModel
		self.stack = [ self.SModel ] 

	def startElement(self, name, attrs):
		if (name == 'map'):
			if (self.state == self.SModel):
				self.map = self.model.addMap()	
				self.state = self.SMap
				for attrName in attrs.keys():
					if(attrName == "format"):
						assert attrs.get(attrName) == "1.0", "Bad map format."

			else:
				assert 0, "A <map> can only be declared at the top level"

		elif (name == 'metadata'):
			if (self.state == self.SMap) or (self.state == self.SElevation) or (self.state == self.SLayer) or (self.state == self.SDataset)\
				or (self.state == self.SObject) or (self.state == self.SAction):
				self.stack.append(self.state)
				self.state = self.SMetadata

			else:
				assert 0, "Metadata can only be declared in a <map>, <elevation>, or <layer> section."

		elif (name == 'param'):
			if (self.state == self.SMetadata):
				pass

			else:
				assert 0, "Parameters found outside the <metadata> section."

		elif (name == 'dataset'):
			if (self.state == self.SMap) or (self.state == self.SModel) or (self.state == self.SDataset):
				self.dataset = self.metamodel.addDataset()
				if (self.state == self.SMap):
					self.map.useDataset(self.dataset)
				self.stack.append(self.state)
				self.state = self.SDataset

			else:
				assert 0, "Datasets can only be declared at the top level, or in a <map> or <dataset> section."

		elif (name == 'object'):
			pass

		elif (name == 'action'):
			pass

		elif (name == 'elevation'):
			if (self.state == self.SMap):
				self.elevation = self.map.addElevation()
				self.state = self.SElevation

			else:
				print self.state
				assert 0, "Elevations can only be declared in a <map> section."

		elif (name == 'layer'):
			if (self.state == self.SElevation):
				cellgrid = 0
				scaling = 1.0
				rotation = 0.0
				x_offset = 0.0
				y_offset = 0.0

				for attrName in attrs.keys():
					if (attrName == "grid_type"):
						if (attrs.get(attrName) == "square"):
							cellgrid = fife.SquareGrid()
							cellgrid.thisown = 0
						elif (attrs.get(attrName) == "hexagonal"):
							cellgrid = fife.HexGrid()
							cellgrid.thisown = 0

					elif (attrName == "scaling"):
						scaling = eval(attrs.get(attrName))

					elif (attrName == "rotation"):
						rotation = eval(attrs.get(attrName))

					elif (attrName == "x_offset"):
						x_offset = eval(attrs.get(attrName))

					elif (attrName == "y_offset"):
						y_offset = eval(attrs.get(attrName))

				assert cellgrid, "No grid type defined for this layer."

				cellgrid.setRotation(rotation)
				cellgrid.setScale(scaling)
				cellgrid.setXShift(x_offset)
				cellgrid.setYShift(y_offset)

				self.layer = self.elevation.addLayer(cellgrid)
				self.state = self.SLayer

			else:
				assert 0, "Layers can only be declared in an <elevation> section."

		elif (name == 'instances'):
			if (self.state == self.SLayer):
				self.state = self.SInstances
				self.x = self.y = 0

			else:
				assert 0, "An <instances> section can only be declared in a <layer> section."

		elif (name == 'i' or name == 'inst' or name == 'instance'):
			if (self.state == self.SInstances):
				objectID = attrs.get("object")
				if (not objectID):
					objectID = attrs.get("obj")
				if (not objectID):
					objectID = attrs.get("o")
				assert objectID, "No object specified."

				# TODO: look up the correct object based on thisobjectID
				#in available datasets, accounting for namespaces.
				# object = lookup(objectID)

				x = attrs.get("x")
				y = attrs.get("y")

				x = x if x else self.x + 1
				y = y if y else self.y

				# self.layer.addInstance(object, fife.Point(x,y))

			else:
				assert 0, "Instances can only be declared in an <instances> section."

	def endElement(self, name):
		if (name == 'metadata'):
			self.state = self.stack.pop()

		if (name == 'dataset'):
			self.state = self.stack.pop()

		if (name == 'map'):
			self.state = self.SModel

		elif (name == 'elevation'):
			self.state = self.SMap

		elif (name == 'layer'):
			self.state = self.SElevation

		elif (name == 'instances'):
			self.state = self.SLayer

parser = make_parser()
handler = ModelLoader()
parser.setContentHandler(handler)

def loadModelFile(path):
	parser.parse(open(path))

parser.parse(open("/home/jwt/fife/engine/engine/branches/active/metamodel/content/maps/new_official_map.xml"))
