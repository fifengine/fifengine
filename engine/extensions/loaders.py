from xml.sax import make_parser 
from xml.sax import saxutils, handler

import fife

class ModelLoader(handler.ContentHandler):

	def __init__(self, engine, state = 0, datastate = 0):
		self.SModel, self.SDataset, self.SMetadata, self.SMap, self.SElevation, self.SLayer, self.SInstances, self.SObject, self.SAction = range(9)

		self.engine = engine
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()
		self.pool = self.engine.getImagePool()

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

	def startElement(self, name, attrs):
		if (name == 'map'):
			if (self.state == self.SModel):
				id = 0
				for attrName in attrs.keys():
					if(attrName == "format"):
						assert attrs.get(attrName) == "1.0", "Bad map format."
					if(attrName == "id"):
						id = attrs.get(attrName)

				assert id, "Map declared without an identifier."

				self.map = self.model.addMap(str(id))
				self.state = self.SMap

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

				assert type, "Dataset declared with no type attribute."

				if (type == "XML"):
					assert source, "External dataset declared with no source."
					parser = make_parser()

					handler = 0
					if (self.state == self.SMap):
						handler = ModelLoader(self.engine, self.SMap, self.map)
					elif (self.state == self.SDataset):
						handler = ModelLoader(self.model, self.SDataset, self.dataset)
					else:
						handler = ModelLoader(self.model)

					assert handler, "Corrupt XML state."

					parser.setContentHandler(handler)
					parser.parse(open(source))

				elif (type == "Embedded"):
					assert id, "Dataset declared without an identifier."
					if (self.state == self.SModel):
						self.dataset = self.metamodel.addDataset(str(id))

					elif (self.state == self.SMap):
						self.dataset = self.metamodel.addDataset(str(id))
						self.map.useDataset(self.dataset)
						
					elif (self.state == self.SDataset):
						self.datastack.append(self.dataset)
						self.dataset = self.dataset.addDataset(str(id))

				else:
					assert 0, "Invalid dataset type attribute. Datasets must be of type XML or Embedded."

				self.stack.append(self.state)
				self.state = self.SDataset

			else:
				assert 0, "Datasets can only be declared at the top level, or in a <map> or <dataset> section."

		elif (name == 'object'):
			if (self.state == self.SDataset):
				self.state = self.SObject

				id = 0
				parent = 0
				for attrName in attrs.keys():
					if (attrName == "id"):
						id = attrs.get(attrName)
					elif (attrName == "parent"):
						query = self.metamodel.getObjectsByString("id", str(attrs.get(attrName)))
						assert len(query) != 0, "0 objects objects with this identifier found."
						assert len(query) == 1, "Multiple objects with this identifier found."
						parent = query[0]

				assert id, "Objects must be given an identifier (id) field."

				if (parent):
					self.object = self.dataset.addObject(str(id), parent)
				else:
					self.object = self.dataset.addObject(str(id))

			else:
				assert 0, "Objects can only be declared in a <dataset> section."

		elif (name == 'image'):
			if (self.state == self.SObject):
				source = 0
				direction = 0
				for attrName in attrs.keys():
					if (attrName == "source"):
						source = attrs.get(attrName)
					elif (attrName == "direction"):
						direction = attrs.get(attrName)

				id = self.pool.addResourceFromFile(str(source))	
				self.object.addStaticImage(0, id)

				assert source, "Image declared with no source location."	

			else:
				assert 0, "<image> tags can only be declared in an <object> section."

		elif (name == 'action'):
			if (self.state == self.SDataset):
				self.state = self.SAction

				# TODO: load actions

			else:
				assert 0, "Actions can only be declared in a <dataset> section."

		elif (name == 'elevation'):
			if (self.state == self.SMap):

				id = 0
				for attrName in attrs.keys():
					if (attrName == "id"):
						id = attrs.get(attrName)

				assert id, "Elevation declared without an identifier."

				self.elevation = self.map.addElevation(str(id))
				self.state = self.SElevation

			else:
				assert 0, "Elevations can only be declared in a <map> section."

		elif (name == 'layer'):
			if (self.state == self.SElevation):

				id = 0
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

					elif (attrName == "id"):
						id = attrs.get(attrName)

				assert cellgrid, "No grid type defined for this layer."
				assert id, "Layer declared with no identifier."

				cellgrid.setRotation(rotation)
				cellgrid.setScale(scaling)
				cellgrid.setXShift(x_offset)
				cellgrid.setYShift(y_offset)

				self.layer = self.elevation.addLayer(str(id), cellgrid)
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

				# We seem to need a unicode -> string conversion here...
				query = self.metamodel.getObjectsByString("id", str(objectID))
				assert len(query) != 0, "0 objects objects with this identifier found."
				assert len(query) == 1, "Multiple objects with this identifier found."
				object = query[0]

				x = attrs.get("x")
				y = attrs.get("y")

				if (x):
					x = int(x)
					self.x = x
				else:
					self.x = self.x + 1
					x = self.x

				if (y):
					y = int(y)
					self.y = y
				else:
					y = self.y

				self.layer.addInstance(object, fife.Point(x,y))

			else:
				assert 0, "Instances can only be declared in an <instances> section."

	def endElement(self, name):
		if (name == 'metadata'):
			self.state = self.stack.pop()

		if (name == 'dataset'):
			self.state = self.stack.pop()
			if (self.state == self.SDataset):
				assert len(self.datastack) > 0, "Corrupted dataset stack."
				self.dataset = self.datastack.pop()

		if (name == 'object' or name == 'action'):
			self.state = self.SDataset

		if (name == 'map'):
			self.state = self.SModel

		elif (name == 'elevation'):
			self.state = self.SMap

		elif (name == 'layer'):
			self.state = self.SElevation

		elif (name == 'instances'):
			self.state = self.SLayer

def loadMapFile(path, engine):
	parser = make_parser()
	handler = ModelLoader(engine)
	parser.setContentHandler(handler)

	parser.parse(open(path))
	del handler

