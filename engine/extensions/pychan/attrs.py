# coding: utf-8
from exceptions import GuiXMLError

class Attr(object):
	def __init__(self,name):
		self.name = name
	
	def set(self,obj,value):
		value = self.parse(value)
		setattr(obj,self.name,value)

	def parse(self,value):
		return str(value)

class PointAttr(Attr):
	def parse(self,value):
		try:
			return tuple(map(int,str(value).split(',')))
		except:
			raise GuiXMLError("Expected an comma separated list of integers.")

class ColorAttr(Attr):
	def parse(self,value):
		try:
			return tuple(map(int,str(value).split(',')))
		except:
			raise GuiXMLError("Expected an color.")

class IntAttr(Attr):
	def parse(self,value):
		try:
			return int(value)
		except:
			raise GuiXMLError("Expected an single integer.")

class BoolAttr(Attr):
	def parse(self,value):
		try:
			value = int(value)
			if value not in (0,1):
				raise GuiXMLError("Expected a 0 or 1.")
			return value
		except:
			raise GuiXMLError("Expected a 0 or 1.")
