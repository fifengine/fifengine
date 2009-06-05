import shutil
import os
import pychan
try:
	import xml.etree.cElementTree as ET
except:
	import xml.etree.ElementTree as ET

class Settings(object):
	instance = None
	def __init__(self, *args, **kwargs):
		if Settings.instance is not None:
			raise RuntimeWarning("Settings instance has already been initialized! Use Editor.getSettings instead")
			
		Settings.instance = self
	
		if os.path.exists('settings.xml') is False:
			shutil.copyfile('settings-dist.xml', 'settings.xml')
			
		self.tree = ET.parse('settings.xml')
		self.root_element = self.tree.getroot()
		self.validateTree()
		
	def validateTree(self):
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
		if not isinstance(module, str) and not isinstance(module, unicode):
			raise AttributeError("Settings:getModuleTree: Invalid type for module argument.")
			
		for c in self.root_element.getchildren():
			if c.tag == "Module" and c.get("name", "") == module:
				return c
	
		# Create module
		return ET.SubElement(self.root_element, "Module", {"name":module})

	def get(self, module, name, defaultValue=None):
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
		self._indent(self.root_element)
		self.tree.write('settings.xml', 'UTF-8')
		
	def _indent(self, elem, level=0):
		""" 
		Clean XML-code
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
		return " ; ".join(list)
		
	def _deserializeList(self, string):
		return string.split(" ; ")

	def _serializeDict(self, dict):
		serial = ""
		for key in dict:
			value = dict[key]
			if serial != "": serial += " ; "
			serial += str(key)+" : "+str(value)
			
		return serial
	
	def _deserializeDict(self, serial):
		dict = {}
		items = serial.split(" ; ")
		for i in items:
			kv_pair = i.split(" : ")
			dict[kv_pair[0]] = kv_pair[1]
		return dict
