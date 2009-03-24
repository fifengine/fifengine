# -*- coding: utf-8 -*-
import fife

"""
Property bindings
=================

This module contains a set of property bindings for
the widgets, factored out to de-clutter the Widget.

"""
class WrappedProperty(object):
	def __init__(self, name):
		self.name = name
	def _getSetter(self,obj):
		setter_name = 'set' + self.name
		return getattr(obj.real_widget,setter_name)
	def _getGetter(self,obj):
		getter_name = 'get' + self.name
		return getattr(obj.real_widget,getter_name)


class ColorProperty(WrappedProperty):
	"""
	A color property. Fakes a color attribute of a guichan widget.
	This accepts either tuples of the colors (r,g,b)
	or L{fife.Color} objects.

	Color objects have value semantics in this case.
	"""
	def __init__(self, name):
		super(ColorProperty, self).__init__(name)
	def __set__(self, obj, color):
		if isinstance(color, tuple):
			color = fife.Color(*color)
		else:
			# Force a copy to get value semantics
			color = fife.Color(color.r,color.g,color.b,color.a)
		self._getSetter(obj)(color)
	def __get__(self, obj, objtype = None):
		color = self._getGetter(obj)()
		return fife.Color(color.r,color.g,color.b,color.a)

