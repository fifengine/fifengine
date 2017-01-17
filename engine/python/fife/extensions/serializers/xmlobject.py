# -*- coding: utf-8 -*-
# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
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

class XMLObjectSaver(object):
	""" The B{XMLObjectSaver} serializes a fife.Object instance by saving
	it back to its XML file
		
	@note:
		- this code does NOT allow the creation of a new xml file
		- this code does NOT touch atlas or animation definitions
		- this code does NOT allow saving to non-well-formed xml files
		- this code DOES save blocking & static flag, as well as
		  image offsets
		
	@type	engine:	fife
	@ivar	engine:	pointer to initialized fife engine instance
	@type	img_manager:	fife.ImageManager
	@ivar	img_manager:	pointer to fife image manager
	@type	compat:	bool
	@ivar	compat:	flag to either use outdated xml definitions or new approach
	@type	debug:	bool
	@ivar	debug:	flag to activate/deactivate debug output
	@type	vfs:	fife.VFS
	@ivar	vfs:	pointer to fife vfs
	@type	change:	bool
	@ivar	change:	flag if object data differs from file data
	"""
	PROCESSING_INSTRUCTION = '<?fife type="object"?>'
	def __init__(self, engine, debug=False, compat=True):
		"""
		
		@type	engine:	fife
		@param	engine:	intialized fife engine
		"""
		self.compat = compat
		self.debug = debug
		self.engine = engine
		self.img_manager = engine.getImageManager()
		self.vfs = self.engine.getVFS()
		self.change = False
		
	def save(self, object):
		""" saves the data of a fife.Object to its xml file
		
		@type	object:	fife.Object
		@param	object:	the object which should be saved
		@rtype	bool
		@return	flag whether the saving was successful or not
		"""
		self.change = False
		result = False
		
		file = object.getFilename()
		if not file:
			raise SerializerError("Object cannot be saved, no file found %s" % object)
			return result

		if not self.vfs.exists(file):
			raise NotFound("File not within vfs: %s" % file)
			return result

		file_handle = self.vfs.open(file)
		file_handle.thisown = 1
		tree = ET.parse(file_handle)
		root = tree.getroot()
		
		object_id = object.getId()
		blocking = object.isBlocking()
		static = object.isStatic()
		
		cost_id = object.getCostId()
		cost = object.getCost()
		cellstack_pos = object.getCellStackPosition()

		if self.debug:
			print "XML tree dump: (pre-save)"
			ET.dump(root)
			print "Object data: "
			print "\tid", object_id
			print "\tblocking", blocking
			print "\tstatic", static
			print "\tcost id", cost_id
			print "\tcost", cost
		
		# check for compat mode
		if root.tag != 'assets':
			self.compat = True

		# in compat mode tree is <object>
		if self.compat:
			objects = [root,]
		# new XML structure has tree root <assets> which groups multiple objects
		else:		
			objects = root.findall("object")
		
		for obj in objects:
			_id = obj.get("id")
			if _id != object_id: 
				if self.debug:
					print "...ommitting object %s " % _id			
				continue
			
			if int(obj.attrib['blocking']) != int(blocking):
				self.change = True
			if int(obj.attrib['static']) != int(static):
				self.change = True
			if str(obj.attrib['cost_id']) != str(cost_id):
				self.change = True
			if float(obj.attrib['cost']) != float(cost):
				self.change = True
			if int(obj.attrib['cellstack_position']) != int(cellstack_pos):
				self.change = True

			obj.attrib['blocking'] = str(int(blocking))
			obj.attrib['static'] = str(int(static))
			obj.attrib['cost_id'] = str(cost_id)
			obj.attrib['cost'] = str(cost)
			obj.attrib['cellstack_position'] = str(cellstack_pos)
			
			if self.debug and self.change:
				print "\tSet new data in xml tree: "
				print "\t\tblocking: ", obj.attrib['blocking']
				print "\t\tstatic: ", obj.attrib['static']
			
			images = obj.findall("image")
			actions = obj.findall("action")
			
			if self.debug:
				print "\tAttempting to save image data: "
				print "\t...found these image elements: "
				print "\t", images
				print "object dump: "
				print ET.dump(obj)
			
			self.save_images(images, object)
			self.save_actions(actions, object)
			
		if not self.change:
			return result			
			
		xmlcontent = ET.tostring(root)
		
		if self.debug:
			print "XML tree dump: (post-manipulation)"
			ET.dump(root)			

		# save xml data beneath the <?fife type="object"?> definition into the object file
		file = open(file, 'w')		
		file.write(XMLObjectSaver.PROCESSING_INSTRUCTION+'\n')
		file.write(xmlcontent + "\n")
		file.close()
		result = True		
		return result
		
	def save_actions(self, actions, object):
		""" save action definitions 
		
		@type	actions:	list
		@param	actions:	list of <action> elements
		@type	object:	fife.Object
		@param	object:	the object which should be saved
		"""
		for element in actions:
			# new xml format uses this, we only save the new format
			if 'animation_id' not in element.attrib:
				break
				
			animation_id = element.attrib['animation_id']
			self.save_animation(animation_id, object)
			
	def save_animation(self, animation_id, object):
		""" save animation definitions for the given id 
		
		@type	animation_id:	str
		@param	animation_id:	id of the animation data structure
		@type	object:	fife.Object
		@param	object:	the object which should be saved
		"""
		pass
			
	def save_images(self, images, object):
		"""	save image definitions
		
		@type	images:	list
		@param	images:	list of <image> elements
		@type	object:	fife.Object
		@param	object:	the object which should be saved
		"""
		visual = object.get2dGfxVisual()			
		angles = visual.getStaticImageAngles()
		if self.debug:
			print "\t\tobject angles: ", angles

		for element in images:
			angle = int(element.get("direction"))
			if angle not in angles: continue
			
			index = visual.getStaticImageIndexByAngle(angle)
			image = self.img_manager.get(index)
			x_offset = image.getXShift()
			y_offset = image.getYShift()
			
			if 'x_offset' not in element.attrib or int(element.attrib['x_offset']) != x_offset:
				self.change = True
			if 'y_offset' not in element.attrib or int(element.attrib['y_offset']) != y_offset:
				self.change = True
			
			element.attrib['x_offset'] = str(x_offset)
			element.attrib['y_offset'] = str(y_offset)

			if self.debug and self.change:
				print "\tSet new data in xml tree: (<image>) "
				print "\t\tx offset: ", element.attrib['x_offset']
				print "\t\ty offset: ", element.attrib['y_offset']

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
				
			if isobjectfile and not s.startswith(obj_identifier):
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
