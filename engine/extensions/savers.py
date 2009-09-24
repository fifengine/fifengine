# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

import os
from xml.sax.saxutils import XMLGenerator
from xml.sax.xmlreader import AttributesNSImpl
from serializers import *

import fife

MAPFORMAT = '1.0'

fileExtensions = ('xml',)
class ModelSaver:

	def __init__(self, filepath, engine, state = 0, datastate = 0):
		self.SModel, self.SMap, self.SLayer, self.SInstances, self.SObject, self.SAction = range(6)

		self.engine = engine
		self.model = self.engine.getModel()
		self.pool = self.engine.getImagePool()
		self.anim_pool = self.engine.getAnimationPool()

		if (state):
			self.state = state
			if (state == self.SMap):
				self.map = datastate
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

		self.nspace = None

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

	def write_map(self, map, importList):
		assert self.state == self.SModel, "Declaration of <map> not at the top level."

		attr_vals = {
			(None, 'id'): map.getId(),
			(None, 'format'): MAPFORMAT,
		}
		attr_names = {
			(None, 'id'): 'id',
			(None, 'format'): 'format',
		}
		attrs = AttributesNSImpl(attr_vals, attr_names)
		self.startElement('map', attrs)
		self.state = self.SMap
		self.write_imports(map, importList)
		self.write_layers(map)
		self.write_camera(map)
		self.endElement('map')

	def write_imports(self, map, importList):
		for importdir in importList:
			self.write_importdir(root_subfile(map.getResourceFile(), importdir))

		imports = []
		for layer in map.getLayers():
			for instance in layer.getInstances():
				file = instance.getObject().getResourceFile()
				if not (file in imports):
					if not self.have_superdir(file, importList):
						imports.append(file)	
						self.write_import(root_subfile(map.getResourceFile(), file))

	def have_superdir(self, file, importList):
		'''returns true, if file is in directories given in importList'''
		for dir in importList:
			have = True
			for test in zip(dir.split(os.path.sep), file.split(os.path.sep)):
				if test[0] != test[1]: have = False
			if have: return True

		return False

	def write_import(self, file):
		attr_vals = {
			(None, 'file'): file,
		}
		attr_names = {
			(None, 'file'): 'file',
		}
		attrs = AttributesNSImpl(attr_vals, attr_names)
		self.file.write(self.indent_level)
		self.xmlout.startElementNS((None, 'import'), 'import', attrs)
		self.xmlout.endElementNS((None, 'import'), 'import')
		self.file.write('\n')

	def write_importdir(self, dir):
		attr_vals = {
			(None, 'dir'): dir,
		}
		attr_names = {
			(None, 'dir'): 'dir',
		}
		attrs = AttributesNSImpl(attr_vals, attr_names)
		self.file.write(self.indent_level)
		self.xmlout.startElementNS((None, 'import'), 'import', attrs)
		self.xmlout.endElementNS((None, 'import'), 'import')
		self.file.write('\n')

	def pathing_val_to_str(self, val):
		if val == fife.CELL_EDGES_AND_DIAGONALS:
			return "cell_edges_and_diagonals"
		if val == fife.FREEFORM:
			return "freeform"
		return "cell_edges_only"

	def write_layers(self, map):
		for layer in map.getLayers():
			cellgrid = layer.getCellGrid()
			attr_vals = {
				(None, 'id'): layer.getId(),
				(None, 'grid_type'): cellgrid.getType(),
				(None, 'x_scale'): str(cellgrid.getXScale()),
				(None, 'y_scale'): str(cellgrid.getYScale()),
				(None, 'rotation'): str(cellgrid.getRotation()),
				(None, 'x_offset'): str(cellgrid.getXShift()),
				(None, 'y_offset'): str(cellgrid.getYShift()),
				(None, 'pathing'): self.pathing_val_to_str(layer.getPathingStrategy()),
				(None, 'transparency'): str(layer.getLayerTransparency()),
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
			self.write_instances(layer)
			self.endElement('layer')

	def write_instances(self, layer):
		attrs = AttributesNSImpl({}, {})
		self.startElement('instances',  attrs)

		for inst in layer.getInstances():
			position = inst.getLocationRef().getExactLayerCoordinates()
			attr_vals = {
				(None, 'o'): inst.getObject().getId(),
				(None, 'x'): str(position.x),
				(None, 'y'): str(position.y),
				(None, 'z'): str(position.z),
				(None, 'r'): str(inst.getRotation()),
			}
			attr_names = {
				(None, 'o'): 'o',
				(None, 'x'): 'x',
				(None, 'y'): 'y',
				(None, 'z'): 'z',
				(None, 'r'): 'r',
			}

			nspace = inst.getObject().getNamespace()
			if nspace != self.nspace:
				attr_vals[(None, 'ns')] = inst.getObject().getNamespace()
				attr_names[(None, 'ns')] = 'ns'
				self.nspace = nspace

			instId = inst.getId()
			if instId:
				attr_vals[(None, 'id')] = inst.getId()
				attr_names[(None, 'id')] = 'id'

			attrs = AttributesNSImpl(attr_vals, attr_names)
			self.file.write(self.indent_level)
			self.xmlout.startElementNS((None, 'i'), 'i', attrs)
			self.xmlout.endElementNS((None, 'i'), 'i')
			self.file.write('\n')

		self.endElement('instances')

	# Save the linked camera of a map.
	def write_camera( self, map ):
		cameralist = self.engine.getView().getCameras()

		for cam in cameralist:
			if cam.getLocationRef().getMap().getId() == map.getId():
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
				}

				attr_vals = {
					(None, 'id'): cam.getId(),
					(None, 'zoom'): str( cam.getZoom()),
					(None, 'tilt'): str( cam.getTilt()),
					(None, 'rotation'): str( cam.getRotation()),
					(None, 'ref_layer_id'): cam.getLocation().getLayer().getId(),
					(None, 'ref_cell_width'): str( celldimensions.x ),
					(None, 'ref_cell_height'): str( celldimensions.y ),
				}

				# add a viewport entry if the cam isn't full sized
				if not (viewport == self.engine.getRenderBackend().getArea()):
					attr_names[(None,'viewport')] = 'viewport'
					attr_vals[(None,'viewport')] = '%d,%d,%d,%d' % (viewport.x, viewport.y, viewport.w, viewport.h)

				attrs = AttributesNSImpl( attr_vals, attr_names )
				self.startElement( 'camera', attrs );
				self.endElement( 'camera' );

	def flush(self):
		self.xmlout.endDocument()
		self.file.close()

def saveMapFile(path, engine, map, importList=[]):
	map.setResourceFile(path)
	writer = ModelSaver(path, engine)
	writer.write_map(map, importList)
