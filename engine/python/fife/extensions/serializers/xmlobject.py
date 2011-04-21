# -*- coding: utf-8 -*-
# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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

""" submodule for xml map parsing """

from fife import fife

from fife.extensions.serializers import ET
from fife.extensions.serializers import SerializerError, InvalidFormat 
from fife.extensions.serializers import NameClash, NotFound, WrongFileType
from fife.extensions.serializers.xmlanimation import loadXMLAnimation

class XMLObjectLoader(object):
	"""
	
	"""
	def __init__(self, engine):
		"""
		
		"""
		self.engine = engine
		self.imgMgr = engine.getImageManager()
		self.anim_pool = None
		self.model = engine.getModel()
		self.vfs = engine.getVFS()
		self.source = None
		self.filename = ''

	def loadResource(self, location):
		"""
		
		"""
		self.source = location
		self.filename = self.source
		self.node = None
		self.file = None
		if hasattr(location, 'node'):
			self.node = location.node
		else:
			isobjectfile = True
			f = self.vfs.open(self.filename)
			f.thisown = 1

			obj_identifier = '<?fife type="object"?>'
			try:
				s = f.readString(len(obj_identifier))
			except fife.IndexOverflow:
				isobjectfile = False

			if isobjectfile and s != obj_identifier:
				isobjectfile = False

			if not isobjectfile:
				return
				
				# this will never be hit currently, if this is put before the return it can provide useful debugging
				# but animation.xml files will raise this exception because apparently they come through here first
				raise WrongFileType('Tried to open non-object file %s with XMLObjectLoader.' % self.filename)

		self.do_load_resource(f)

	def do_load_resource(self, file):
		"""
		
		"""
		if file:
			tree = ET.parse(file)
			self.node = tree.getroot()
		self.parse_object(self.node)

	def parse_object(self, object):
		"""
		
		"""
		if self.node.tag != 'object':
			raise InvalidFormat('Expected <object> tag, but found <%s>.' % self.node.tag)

		_id = object.get('id')
		if not _id:
			raise InvalidFormat('<object> declared without an id attribute.')
		_id = str(_id)

		nspace = object.get('namespace')
		if not nspace:
			raise InvalidFormat('<object> %s declared without a namespace attribute.' % str(_id))
		nspace = str(nspace)

		obj = None
		parent = object.get('parent', None)
		if parent:
			query = self.metamodel.getObjects('id', str(parent))
			if len(query) == 0:
				raise NotFound('No objects found with identifier %s.' % str(parent))
			elif len(query) > 1:
				raise NameClash('%d objects found with identifier %s.' % (len(query), str(parent)))
			parent = query[0]

		# check if model already has this object
		if not bool(self.model.getObject(_id, nspace)):
			obj = self.model.createObject(_id, nspace, parent)
		else:
			print NameClash('Tried to create already existing object \n\t...ignoring: %s, %s' % (_id, nspace))
			return

		obj.setFilename(self.source)
		fife.ObjectVisual.create(obj)
		obj.setBlocking(bool( int(object.get('blocking', False)) ))
		obj.setStatic(bool( int(object.get('static', False)) ))

		pather = object.get('pather', 'RoutePather')
		obj.setPather( self.model.getPather(pather) )

		self.parse_images(object, obj)
		self.parse_actions(object, obj)

	def parse_images(self, objelt, object):
		"""
		
		"""
		for image in objelt.findall('image'):
			source = image.get('source')
			if not source:
				raise InvalidFormat('<image> declared without a source attribute.')

			# paths are relative to this resource's path
			path = self.filename.split('/')
			path.pop()
			path.append(str(source))

			img = self.imgMgr.create('/'.join(path))
			img.setXShift(int( image.get('x_offset', 0) ))
			img.setYShift(int( image.get('y_offset', 0) ))
			
			object.get2dGfxVisual().addStaticImage(int( image.get('direction', 0) ), img.getHandle())

	def parse_actions(self, objelt, object):
		"""
		
		"""
		for action in objelt.findall('action'):
			id = action.get('id')
			if not id:
				raise InvalidFormat('<action> declared without an id attribute.')

			act_obj = object.createAction(str(id))
			fife.ActionVisual.create(act_obj)
			self.parse_animations(action, act_obj)

	def parse_animations(self, actelt, action):
		"""
		
		"""
		pass
		for anim in actelt.findall('animation'):
			source = anim.get('source')
			if not source:
				raise InvalidFormat('Animation declared with no source location.')

			# animation paths are relative to this resource's path
			path = self.filename.split('/')
			path.pop()
			path.append(str(source))

			animation = loadXMLAnimation(self.engine, '/'.join(path))
			action.get2dGfxVisual().addAnimation(int( anim.get('direction', 0) ), animation)
			action.setDuration(animation.getDuration())
