# coding: utf-8

"""
Simple error checking attributes.

This module defines a set of Attribute classes
which you can use to define possible values
an attribute of an object accepts.

Usage::
  class SomeObject:
      nameAttr, posAttr = [ Attr("name"), PointAttr("pos") ]
  
  obj =  SomeObject()
  obj.nameAttr.set(obj,"newName")
  obj.posAttr.set(obj,"89,89")

This is most useful for error checking parsing and defining
accepted attributes in classes and is used by pychan internally.

"""

from exceptions import ParserError

class Attr(object):
	"""
	A simple text attribute.
	"""
	def __init__(self,name):
		self.name = name

	def set(self,obj,value):
		"""
		Parses the given value with the L{parse} method
		and sets it on the given instance with C{setattr}.
		"""
		value = self.parse(value)
		setattr(obj,self.name,value)

	def parse(self,value):
		"""
		Parses a value and checks for errors.
		Override with specialiced behaviour.
		"""
		return str(value)

class PointAttr(Attr):
	def parse(self,value):
		try:
			x,y = tuple(map(int,str(value).split(',')))
			return x,y
		except:
			raise ParserError("Expected a comma separated list of two integers.")

class ColorAttr(Attr):
	def parse(self,value):
		a = 255
		try:
			try:
				r,g,b,a = tuple(map(int,str(value).split(',')))
				for c in (r,g,b,a):
					if not 0 <= c < 256: raise ParserError("Expected a color (Failed: 0 <= %d <= 255)" %c)
			except:
				r,g,b = tuple(map(int,str(value).split(',')))
				for c in (r,g,b):
					if not 0 <= c < 256: raise ParserError("Expected a color (Failed: 0 <= %d <= 255)" %c)
		except:
			raise ParserError("Expected an color.")

		return r,g,b,a

class IntAttr(Attr):
	def parse(self,value):
		try:
			return int(value)
		except:
			raise ParserError("Expected a single integer.")

class BoolAttr(Attr):
	def parse(self,value):
		try:
			value = int(value)
			if value not in (0,1):
				raise ParserError("Expected a 0 or 1.")
			return value
		except:
			raise ParserError("Expected a 0 or 1.")
			
class FloatAttr(Attr):
	def parse(self, value):
		try:
			return float(value)
		except:
			raise ParseError("Expected a float.")
