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

import os
from StringIO import StringIO
	
from fife.extensions.serializers import ET, SerializerError, InvalidFormat, \
										NotFound


EMPTY_XML_FILE="""\
<?xml version='1.0' encoding='UTF-8'?>
<Settings>

</Settings>
"""

class SimpleSerializer(object):
	"""
	Use this as a base class for custom setting loaders/savers to use with the 
	Setting class.
	"""
	
	def __init__(self, filename=None):
		pass
		
	def get(self, module, name, defaultValue=None):
		pass
		
	def set(self, module, name, value, extra_attrs={}):
		pass

	def set(self, module, name, value, extra_attrs={}):
		pass

	def load(self, filename=None):
		"""
		@note: If the filename specified is empty this function MUST 
		initialize an empty settings file in whatever format you need.
		"""
		pass
		
	def save(self, filename=None):
		pass

	def getModuleNameList(self):
		"""
		@note: Returns all the module names that are present in the 
		settings.xml file as a list of strings
		"""
		pass
	
	def getAllSettings(self,module):
		"""
		@note: Returns all the setting names and values under the Module name
		module as a	dictionary structure
		"""
		pass

class SimpleXMLSerializer(SimpleSerializer):
	"""
	This class is a simple interface to get and store data in XML files.

	Usage::
		from fife.extensions.serializers.simplexml import SimpleXMLSerializer
		serializer = SimpleXMLSerializer(filename="somefile.xml")
		serializer.set("module_name", "variable_name", "value")
		somevariable = serializer.get("module_name", "variable_name", \
									  "default_value")
	"""
	def __init__(self, filename=None):
		self._file = filename
		self._tree = None
		self._root_element = None
		
		self._initialized = False
		
	def load(self, filename=None):
		"""
		Loads the XML file into memory and validates it.
		
		Raises a SerializerError exception if the file is not specified.
		
		@param filename: The file to load
		@type filename: C{str}
		
		@note: If the file does not exist it will automatically create a blank
		file for you.
		"""
		if filename:
			self._file = filename
		
		if not self._file:
			raise SerializerError("Cannot load file or create file.  No "
								  "filename specified!")
		
		if not os.path.exists(self._file):
			self._tree = ET.parse(StringIO(EMPTY_XML_FILE))
			self._tree.write(self._file, 'UTF-8')			
		else:
			self._tree = ET.parse(self._file)

		self._root_element = self._tree.getroot()
		self._validateTree()

	def save(self, filename=None):
		"""
		Saves the XML file.
		
		@param filename: The file to save
		@type filename: C{str}
		
		@note: This Overwrites the file if it exists.
		"""
		if not self._initialized:
			self.load()	
			self._initialized = True
		
		if filename:
			savefile = filename
		else:
			savefile = self._file
				
		if not savefile:
			raise SerializerError("Cannot save file.  No filename specified!")
	
		""" Writes the settings to file """
		self._indent(self._root_element)
		self._tree.write(savefile, 'UTF-8')


	def getValue(self, e_type, e_value):
		if e_type == 'int':
			return int(e_value)
		elif e_type == 'float':
			return float(e_value)
		elif e_type == 'bool':
			e_value = e_value.lower()
			if e_value == "" or e_value == "false" or e_value == "no" \
			   or e_value == "0":
				return False
			else:
				return True
		elif e_type == 'str' or e_type == 'string':
			return str(e_value)
		elif e_type == 'unicode':
			return unicode(e_value)
		elif e_type == 'list':
			return self._deserializeList(e_value)
		elif e_type == 'dict':
			return self._deserializeDict(e_value)
		

	def get(self, module, name, defaultValue=None):
		""" Gets the value of a specified variable

		@param module: Name of the module to get the variable from
		@param name: Variable name
		@param defaultValue: Specifies the default value to return if the 
		variable is not found
		@type defaultValue: C{str} or C{unicode} or C{int} or C{float} or 
		C{bool} or C{list} or C{dict}
		"""
		if not self._initialized:
			self.load()
			self._initialized = True
		
		if not isinstance(name, str) and not isinstance(name, unicode):
			raise AttributeError("SimpleXMLSerializer.get(): Invalid type for "
								 "name argument.")

		#get the module tree: for example find tree under module FIFE
		moduleTree = self._getModuleTree(module)
		element = None
		for e in moduleTree.getchildren():
			if e.tag == "Setting" and e.get("name", "") == name:
				element = e
				break
		else:
			return defaultValue

		e_value = element.text
		e_strip = element.get("strip", "1").strip().lower()
		e_type	= str(element.get("type", "str")).strip()

		if e_value is None:
			return defaultValue

		# Strip value
		if e_strip == "" or e_strip == "false" or e_strip == "no" \
		   or e_strip == "0":
			e_strip = False
		else: e_strip = True

		if e_type == "str" or e_type == "unicode":
			if e_strip: e_value = e_value.strip()
		else:
			e_value = e_value.strip()

		# Return value
		e_value = self.getValue(e_type,e_value)
		
		return e_value

	def set(self, module, name, value, extra_attrs={}):
		"""
		Sets a variable to specified value.

		@param module: Module where the variable should be set
		@param name: Name of the variable
		@param value: Value to assign to the variable
		@type value: C{str} or C{unicode} or C{int} or C{float} or C{bool} or 
		C{list} or C{dict}
		@param extra_attrs: Extra attributes to be stored in the XML-file
		@type extra_attrs: C{dict}
		"""
		if not self._initialized:
			self.load()
			self._initialized = True
		
		if not isinstance(name, str) and not isinstance(name, unicode):
			raise AttributeError("SimpleXMLSerializer.set(): Invalid type for "
								 "name argument.")

		moduleTree = self._getModuleTree(module)
		e_type = "str"

		if isinstance(value, bool): # This must be before int
			e_type = "bool"
			value = str(value)
		elif isinstance(value, int):
			e_type = "int"
			value = str(value)
		elif isinstance(value, float):
			e_type = "float"
			value = str(value)
		elif isinstance(value, unicode):
			e_type = "unicode"
			value = unicode(value)
		elif isinstance(value, list):
			e_type = "list"
			value = self._serializeList(value)
		elif isinstance(value, dict):
			e_type = "dict"
			value = self._serializeDict(value)
		else:
			e_type = "str"
			value = str(value)

		for e in moduleTree.getchildren():
			if e.tag != "Setting": continue
			if e.get("name", "") == name:
				e.text = value
				break
		else:
			attrs = {"name":name, "type":e_type}
			for k in extra_attrs:
				if k not in attrs:
					attrs[k] = extra_attrs[k]
			elm = ET.SubElement(moduleTree, "Setting", attrs)
			elm.text = value

	def remove(self, module, name):
		"""
		Removes a variable

		@param module: Module where the variable should be set
		@param name: Name of the variable
		"""
		if not self._initialized:
			self.load()
			self._initialized = True

		if not isinstance(name, str) and not isinstance(name, unicode):
			raise AttributeError("SimpleXMLSerializer.set(): Invalid type for "
								 "name argument.")

		moduleTree = self._getModuleTree(module)

		for e in moduleTree.getchildren():
			if e.tag != "Setting": continue
			if e.get("name", "") == name:
				moduleTree.remove(e)

	"""
	returns a list of string, where each string is a module name
	"""
	def getModuleNameList(self):
		self._moduleNames = []
		for c in self._root_element.getchildren():
			if c.tag == "Module":
				name = c.get("name","")
				if not isinstance(name, str) and not isinstance(name, unicode):
					raise AttributeError("SimpleXMLSerializer.get(): Invalid "
										 "type for name argument.")
				
				self._moduleNames.append(name)
		return self._moduleNames
		
	
	def getAllSettings(self,module):
		self._settingsFromFile = {}
		
		# if file has not been loaded, load the file
		if not self._initialized:
			self.load()
			self._initialized = True
			
		# get the module tree, as we want to get values for module FIFE only
		moduleTree = self._getModuleTree(module)
		
		# now from the tree read every value, and put the necessary values
		# to the list
		for e in moduleTree.getchildren():
			if e.tag == "Setting":
				name = e.get("name", "")
	
				# check the name
				if not isinstance(name, str) and not isinstance(name, unicode):
					raise AttributeError("SimpleXMLSerializer.get(): Invalid "
										 "type for name argument.")
				element = e
				
				e_value = element.text
				e_strip = element.get("strip", "1").strip().lower()
				e_type	= str(element.get("type", "str")).strip()
					
				# Strip value
				if e_strip == "" or e_strip == "false" \
				   or e_strip == "no" or e_strip == "0":
					e_strip = False
				else: e_strip = True
			
				if e_type == "str" or e_type == "unicode":
					if e_strip and e_value:
						e_value = e_value.strip()
				else:
					if e_value:
						e_value = e_value.strip()
					
				# get the value
				e_value = self.getValue(e_type,e_value)
				self._settingsFromFile[name] = e_value;
		
		return self._settingsFromFile				



	def _validateTree(self):
		""" 
		Iterates the XML tree and prints warning when an invalid tag is found.
		
		Raises an InvalidFormat exception if there is a format error.
		"""
		for c in self._root_element.getchildren():
			if c.tag != "Module":
				raise InvalidFormat("Invalid tag in " + self._file + \
									". Expected Module, got: " + c.tag)
			elif c.get("name", "") == "":
				raise InvalidFormat("Invalid tag in " + self._file + \
									". Module name is empty.")
			else:
				for e in c.getchildren():
					if e.tag != "Setting":
						raise InvalidFormat("Invalid tag in " + self._file + \
											" in module: " + c.tag + \
											". Expected Setting, got: " + \
											e.tag)
					elif c.get("name", "") == "":
						raise InvalidFormat("Invalid tag in " + self._file + \
											" in module: " + c.tag + \
											". Setting name is empty" + e.tag)
						
	def _getModuleTree(self, module):
		"""
		Returns a module element from the XML tree. If no module with the 
		specified name exists, a new element will be created.

		@param module: The module to get from the settings tree
		@type module: C{string}
		"""
		if not isinstance(module, str) and not isinstance(module, unicode):
			raise AttributeError("Settings:_getModuleTree: Invalid type for "
								 "module argument.")

		for c in self._root_element.getchildren():
			if c.tag == "Module" and c.get("name", "") == module:
				return c

		# Create module
		return ET.SubElement(self._root_element, "Module", {"name":module})

	def _indent(self, elem, level=0):
		"""
		Adds whitespace, so the resulting XML-file is properly indented.
		Shamelessly stolen from http://effbot.org/zone/element-lib.htm
		"""
		i = os.linesep + level*"  "
		if len(elem):
			if not elem.text or not elem.text.strip():
				elem.text = i + "  "
			if not elem.tail or not elem.tail.strip():
				elem.tail = i
			for elem in elem:
				self._indent(elem, level+1)
			if not elem.tail or not elem.tail.strip():
				elem.tail = i
		else:
			if level and (not elem.tail or not elem.tail.strip()):
				elem.tail = i

	# FIXME:
	# These serialization functions are not reliable at all
	# This will only serialize the first level of a dict or list
	# It will not check the types nor the content for conflicts.
	# Perhaps we should add a small serialization library?
	def _serializeList(self, list):
		""" Serializes a list, so it can be stored in a text file """
		return " ; ".join(list)

	def _deserializeList(self, string):
		""" Deserializes a list back into a list object """
		if not string:
			return  list()
		return string.split(" ; ")

	def _serializeDict(self, dict):
		""" Serializes a list, so it can be stored in a text file """
		serial = ""
		for key in dict:
			value = dict[key]
			if serial != "": serial += " ; "
			serial += str(key)+" : "+str(value)

		return serial

	def _deserializeDict(self, serial):
		""" Deserializes a list back into a dict object """
		if not serial:
			return dict()
		dict = {}
		items = serial.split(" ; ")
		for i in items:
			kv_pair = i.split(" : ")
			dict[kv_pair[0]] = kv_pair[1]
		return dict
		
