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

class UnicodeAttr(Attr):
	def parse(self,value):
		"""
		Parses a value and checks for errors.
		Override with specialiced behaviour.
		"""
		return unicode(value)


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
			raise ParserError("Expected a color.")

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
