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

from fife import fife, fifechan
from exceptions import RuntimeError

def get_manager():
	from fife.extensions import pychan
	return pychan.manager

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
	A color property. Fakes a color attribute of a fifechan widget.
	This accepts either tuples of the colors (r,g,b)
	or L{fife.Color} objects.

	Color objects have value semantics in this case.
	"""
	def __init__(self, name):
		super(ColorProperty, self).__init__(name)
	def __set__(self, obj, color):
		if isinstance(color, tuple):
			color = fifechan.Color(*color)
		else:
			# Force a copy to get value semantics
			color = fifechan.Color(color.r,color.g,color.b,color.a)
		self._getSetter(obj)(color)
	def __get__(self, obj, objtype = None):
		color = self._getGetter(obj)()
		return fifechan.Color(color.r,color.g,color.b,color.a)

class ImageProperty(WrappedProperty):
	"""
	This unifies the way Images and Image attributes are handled
	inside PyChan.
	"""
	def __init__(self, name):
		super(ImageProperty, self).__init__(name)
		self.prop_name = "_prop_" + self.name.lower()
	def __set__(self, obj, image):
		image_info = getattr(obj, self.prop_name, {})
		if not image:
			image_info["source"] = ""
			image_info["image"] = fife.GuiImage()
			image_info["image"]._source = ""
			self._getSetter(obj)(None)

		elif isinstance(image, str):
			image_info["source"] = image
			# to catch or not to catch ...
			# we just let the NotFound exception trickle here.
			# depedning on complains we can catch and print a warning.
			image_info["image"] = get_manager().loadImage(image)
			image_info["image"].source = image
			self._getSetter(obj)(image_info["image"])

		elif isinstance(image,fife.GuiImage):
			# FIXME - this trickery with the hidden annotation
			# with an _source attribute isn't really clean.
			# Is it even necessary
			image_info["source"] = getattr(image,"source","")
			image_info["image"] = image
			if image_info["source"]:
				image_info["image"] = get_manager().loadImage(image_info["source"])
			self._getSetter(obj)(image_info["image"])
		else:
			attribute_name = "%s.%s" % (obj.__class__.__name__,self.name)
			error_message = "%s only accepts GuiImage and python strings, not '%s'"
			raise RuntimeError(error_message % (attribute_name, repr(image)))
		
		setattr(obj, self.prop_name, image_info)

	def __get__(self, obj, objtype = None):
		d = getattr(obj, self.prop_name, {})
		image = d.get("image",None)
		if not image:
			image = fife.GuiImage()
			image.source = ""
		return image

