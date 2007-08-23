import fife
from xml.sax import saxutils

class ModelLoader(saxutils.DefaultHandler):
	def __init__(self):
		self.SModel, self.SMetadata, self.SMap, self.SElevation, self.SLayer, self.SInstances = range(6)

		self.model = fife.Model()	
		self.state = self.SModel

	def startElement(self, name, attrs):
		if (name == 'map'):
			if (self.state == self.SModel):
				self.map = self.model.addMap()	
				self.state = self.SMap
				for attrName in attrs.keys():
					if(attrName == "format"):
						assert attrs.get(attrName) == 1, "Bad map format."
			else assert 0, "A <map> can only be declared at the top level"

		elif (name == 'metadata'):
			if (self.state == self.SMap) || (self.state == self.SElevation) || (self.state == self.SLayer):
				self.state = self.SMetadata

		 else assert 0, "Metadata can only be declared in a <map>, <elevation>, or <layer> section."

		elif (name == 'param'):
			if (self.state == self.SMetadata):
				pass

			else assert 0, "Parameters found outside the <metadata> section."

		elif (name == 'dataset'):
			if self.state == self.SMap:
				pass

			else assert 0, "Datasets can only be declared in a <map> section."

		elif (name == 'elevation'):
			if (self.state == SMap):
				self.elevation = self.map.addElevation(self.map)
				self.state = self.SElevation

			else assert 0, "Elevations can only be declared in a <map> section."

		elif (name == 'layer'):
			if (self.state == SElevation):
				geometry = 0
				for attrName in attrs.keys():
					if(attrName == "geometry"):
						pass # TODO: look up the geometry, and load it into self.layer	
				self.layer = self.elevation.addLayer(self.elevation, self.geometry)
				self.state = self.SLayer

			else assert 0, "Layers can only be declared in an <elevation> section."

		elif (name == 'instances'):
			if (self.state == self.SLayer):
				self.state = SInstances
				self.x = self.y = 0

			else assert 0, "An <instances> section can only be declared in a <layer> section."

		elif (name == 'i' || name == 'inst' || name == 'instance'):
			if (self.state == self.SInstances):
				objectID = attrs.get("object")
				if (not objectID):
					objectID = attrs.get("obj")
				if (not object):
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

			else assert 0, "Instances can only be declared in an <instances> section."

	def endElement(self, name):
		if (name == 'map'):
			self.state = self.SModel

		elif (name == 'elevation'):
			self.state = self.SMap

		elif (name == 'layer'):
			self.state = self.SElevation

		elif (name == 'instances'):
			self.state = self.SLayer

