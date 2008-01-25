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
			attr_vals = {
				(None, 'name'): list[i],
			}
			attr_names = {
				(None, 'name'): 'name',
			}
			attrs = AttributesNSImpl(attr_vals, attr_names)
			self.file.write(self.indent_level)
			self.xmlout.startElementNS((None, 'param'), 'param', attrs)

			self.xmlout.characters(parent.get(list[i]))

			self.xmlout.endElementNS((None, 'param'), 'param')
			self.file.write('\n')

		self.endElement('metadata')

	def write_datasets(self, parent):
		for set in parent.getDatasets():
			attr_vals = {
				(None, 'type'): 'XML',
				(None, 'source'): set.getSource(),
			}
			attr_names = {
				(None, 'type'): 'type',
				(None, 'name'): 'name',
			}
			attrs = AttributesNSImpl(attr_vals, attr_names)
			self.file.write(self.indent_level)
			self.xmlout.startElementNS((None, 'dataset'), 'dataset', attrs)
			self.xmlout.endElementNS((None, 'dataset'), 'dataset')
			self.file.write('\n')

	def write_elevations(self, map):
		for elev in map.getElevations():
			attr_vals = {
				(None, 'id'): elev.Id(),
			}
			attr_names = {
				(None, 'id'): 'id',
			}
			attrs = AttributesNSImpl(attr_vals, attr_names)
			self.startElement('elevation', attrs)
			self.write_metadata(elev)
			self.write_layers(elev)
			self.write_camera(elev)
			self.endElement('elevation')

	def pathing_val_to_str(self, val):
		if val == fife.CELL_EDGES_AND_DIAGONALS:
			return "cell_edges_and_diagonals"
		if val == fife.FREEFORM:
			return "freeform"
		return "cell_edges_only"
	
	def write_layers(self, elevation):
		for layer in elevation.getLayers():
			cellgrid = layer.getCellGrid()
			attr_vals = {
				(None, 'id'): layer.Id(),
				(None, 'grid_type'): cellgrid.getType(),
				(None, 'x_scale'): str(cellgrid.getXScale()),
				(None, 'y_scale'): str(cellgrid.getYScale()),
				(None, 'rotation'): str(cellgrid.getRotation()),
				(None, 'x_offset'): str(cellgrid.getXShift()),
				(None, 'y_offset'): str(cellgrid.getYShift()),
				(None, 'pathing'): self.pathing_val_to_str(layer.getPathingStrategy()),
			}
			attr_names = {
				(None, 'id'): 'id',
				(None, 'grid_type'): 'grid_type',
				(None, 'scaling'): 'scaling',
				(None, 'rotation'): 'rotation',
				(None, 'x_offset'): 'x_offset',
				(None, 'y_offset'): 'y_offset',
				(None, 'pathing'): 'pathing',
			}
			attrs = AttributesNSImpl(attr_vals, attr_names)
			self.startElement('layer', attrs)
			self.write_metadata(layer)
			self.write_instances(layer)
			self.endElement('layer')

	def write_instances(self, layer):
		attrs = AttributesNSImpl({}, {})
		self.startElement('instances',  attrs)
		for inst in layer.getInstances():
			position = inst.getLocation().getLayerCoordinates()
			attr_vals = {
				(None, 'o'): inst.getObject().Id(),
				(None, 'x'): str(position.x),
				(None, 'y'): str(position.y),
				(None, 'z'): str(position.z),
			}
			attr_names = {
				(None, 'o'): 'o',
				(None, 'x'): 'x',
				(None, 'y'): 'y',
				(None, 'z'): 'z',
			}
			instId = inst.Id()
			if instId:
				attr_vals[(None, 'id')] = inst.Id()
				attr_names[(None, 'id')] = 'id'
			attrs = AttributesNSImpl(attr_vals, attr_names)
			if not inst.listFields():
				self.file.write(self.indent_level)
				self.xmlout.startElementNS((None, 'i'), 'i', attrs)
				self.xmlout.endElementNS((None, 'i'), 'i')
				self.file.write('\n')
			else:
				self.startElement('i', attrs)
				self.write_metadata(inst)
				self.endElement('i')

		self.endElement('instances')

	# Save the linked camera of an elevation.
	def write_camera( self, elev ):
		cameralist = self.engine.getView().getCameras()

		for cam in cameralist:
			if cam.getLocation().getElevation().Id() == elev.Id():
				celldimensions = cam.getCellImageDimensions()
				viewport = cam.getViewPort();

				attr_names = {
						(None, 'id'): 'id',
						(None, 'zoom'): 'zoom',
						(None, 'tilt'): 'tile',
						(None, 'rotation'): 'rotation',
						(None, 'ref_layer_id'): 'ref_layer_id',
						(None, 'ref_cell_width'): 'ref_cell_width',
						(None, 'ref_cell_height'): 'ref_cell_height',
						(None, 'viewport'): 'viewport',
				}

				attr_vals = {
					(None, 'id'): cam.getId(),
					(None, 'zoom'): str( cam.getZoom()),
					(None, 'tilt'): str( cam.getTilt()),
					(None, 'rotation'): str( cam.getRotation()),
					(None, 'ref_layer_id'): cam.getLocation().getLayer().Id(),
					(None, 'ref_cell_width'): str( celldimensions.x ),
					(None, 'ref_cell_height'): str( celldimensions.y ),
					(None, 'viewport'): '%d,%d,%d,%d' % (viewport.x, viewport.y, viewport.w, viewport.h),
				}

				attrs = AttributesNSImpl( attr_vals, attr_names )
				self.startElement( 'camera', attrs );
				self.endElement( 'camera' );

				return

	def flush(self):
		self.xmlout.endDocument()
		self.file.close()

def saveMapFile(path, engine, map):
	writer = ModelSaver(path, engine)
	writer.write_map(map)
