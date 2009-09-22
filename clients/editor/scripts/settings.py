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

import shutil
import os
from fife_utils import getUserDataDirectory
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

class Settings(object):
	""" This class provides an interface for retrieving and putting settings
	to an XML-file. 
	
	Only one instance of this class may exist, or a RuntimeWarning exception
	will be raised. Use Settings.instance or Editor.getSettings() to retrieve
	an instance of this class.
	"""
	instance = None # Points to the first initialized instance of this class
	def __init__(self, *args, **kwargs):
		if Settings.instance is not None:
			raise RuntimeWarning("Settings instance has already been initialized! Use Editor.getSettings instead")
			
		Settings.instance = self
		
		self._appdata = getUserDataDirectory("fife", "editor")
	
		if os.path.exists(self._appdata+'/settings.xml') is False:
			shutil.copyfile('settings-dist.xml', self._appdata+'/settings.xml')
			
		self.tree = ET.parse(self._appdata+'/settings.xml')
		self.root_element = self.tree.getroot()
		self.validateTree()
		
	def validateTree(self):
		""" Iterates the settings tree and prints warning when an invalid tag is found """
		for c in self.root_element.getchildren():
			if c.tag != "Module":
				print "Invalid tag in settings.xml. Expected Module, got: ", c.tag
			elif c.get("name", "") == "":
				print "Invalid tag in settings.xml. Module name is empty."
			else:
				for e in c.getchildren():
					if e.tag != "Setting":
						print "Invalid tag in settings.xml in module: ",c.tag,
						print ". Expected Setting, got: ", e.tag
					elif c.get("name", "") == "":
						print "Invalid tag in settings.xml in module: ",c.tag,
						print ". Setting name is empty", e.tag
		
	def getModuleTree(self, module):
		""" Returns a module element from the settings tree. If no module with the specified
		name exists, a new element will be created. """
		if not isinstance(module, str) and not isinstance(module, unicode):
			raise AttributeError("Settings:getModuleTree: Invalid type for module argument.")
			
		for c in self.root_element.getchildren():
			if c.tag == "Module" and c.get("name", "") == module:
				return c
	
		# Create module
		return ET.SubElement(self.root_element, "Module", {"name":module})

	def get(self, module, name, defaultValue=None):
		""" Gets the value of a specified setting
		
		defaultValue is returned if the setting does not exist
		"""
		if not isinstance(name, str) and not isinstance(name, unicode):
			raise AttributeError("Settings:get: Invalid type for name argument.")
		
		moduleTree = self.getModuleTree(module)
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
		if e_strip == "" or e_strip == "false" or e_strip == "no" or e_strip == "0":
			e_strip = False
		else: e_strip = True
		
		if e_type == "str" or e_type == "unicode":
			if e_strip: e_value = e_value.strip()
		else:
			e_value = e_value.strip()
		
		# Return value
		if e_type == 'int':
			return int(e_value)
		elif e_type == 'float':
			return float(e_value)
		elif e_type == 'bool':
			e_value = e_value.lower()
			if e_value == "" or e_value == "false" or e_value == "no" or e_value == "0":
				return False
			else:
				return True
		elif e_type == 'str':
			return str(e_value)
		elif e_type == 'unicode':
			return unicode(e_value)
		elif e_type == 'list':
			return self._deserializeList(e_value)
		elif e_type == 'dict':
			return self._deserializeDict(e_value)

	def set(self, module, name, value, extra_attrs={}):
		"""
		Sets a setting to specified value.
		
		Parameters:
		module (str|unicode): Module where the setting should be set
		name (str|unicode): Name of setting
		value: Value to assign to setting
		extra_attrs (dict): Extra attributes to be stored in the XML-file
		"""
		if not isinstance(name, str) and not isinstance(name, unicode):
			raise AttributeError("Settings:set: Invalid type for name argument.")
			
		moduleTree = self.getModuleTree(module)
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
					attrs[k] = extra_args[k]
			elm = ET.SubElement(moduleTree, "Setting", attrs)
			elm.text = value

	def saveSettings(self):
		""" Save settings into settings.xml """
		self._indent(self.root_element)
		self.tree.write(self._appdata+'/settings.xml', 'UTF-8')
		
	def _indent(self, elem, level=0):
		""" 
		Adds whitespace, so the resulting XML-file is properly indented.
		Shamelessly stolen from http://effbot.org/zone/element-lib.htm 
		"""
		i = "\n" + level*"  "
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
		dict = {}
		items = serial.split(" ; ")
		for i in items:
			kv_pair = i.split(" : ")
			dict[kv_pair[0]] = kv_pair[1]
		return dict

