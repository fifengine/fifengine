#!/usr/bin/env python
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

from common import *
from widget import Widget

class Icon(Widget):
	"""
	An image icon.

	New Attributes
	==============

	  - image: String or GuiImage: The source location of the Image or a direct GuiImage
	"""
	ATTRIBUTES = Widget.ATTRIBUTES + [Attr('image')]

	def __init__(self,image="",**kwargs):
		self.real_widget = fife.Icon(None)
		super(Icon,self).__init__(**kwargs)
		self._source = self._image = None
		if image:
			self.image = image

	def _setImage(self,source):
		if isinstance(source,str):
			self._source = source
			self._image = get_manager().loadImage(source)
		elif isinstance(source,fife.GuiImage):
			self._source = None
			self._image = source
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))
		self.real_widget.setImage( self._image )

		# Set minimum size accoriding to image
		self.min_size = self.real_widget.getWidth(),self.real_widget.getHeight()
		self.size  = self.max_size = self.min_size

	def _getImage(self):
		if self._source is not None:
			return self._source
		return self._image
	image = property(_getImage,_setImage)
