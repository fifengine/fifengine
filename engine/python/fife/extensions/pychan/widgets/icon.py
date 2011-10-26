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
from fife.extensions.pychan.properties import ImageProperty

class Icon(Widget):
	"""
	An image icon.

	New Attributes
	==============

	  - image: String or GuiImage: The source location of the Image or a direct GuiImage
	"""
	ATTRIBUTES = Widget.ATTRIBUTES + [ Attr('image') ]

	def __init__(self,
				 parent = None, 
				 name = Widget.DEFAULT_NAME,
				 size = Widget.DEFAULT_SIZE, 
				 min_size = Widget.DEFAULT_MIN_SIZE, 
				 max_size = Widget.DEFAULT_MAX_SIZE,
				 helptext = Widget.DEFAULT_HELPTEXT, 
				 position = Widget.DEFAULT_POSITION,
				 style = None, 
				 hexpand = None, 
				 vexpand = None,
				 image = None):
				 
		self.real_widget = fife.Icon(None)
		super(Icon,self).__init__(parent=parent, 
								  name=name, 
								  size=size, 
								  min_size=min_size, 
								  max_size=max_size,
								  helptext=helptext, 
								  position=position,
								  style=style, 
								  hexpand=hexpand, 
								  vexpand=vexpand)
		self.image = image

	_image = ImageProperty("Image")

	def _setImage(self,source):
		self._image = source
		# This is a bit odd.
		# ... not sure whether to leave the sizes alone, but that might
		# break layouts. yikes.
		self.min_size = self.real_widget.getWidth(),self.real_widget.getHeight()
		self.max_size = self.min_size
		self.size  = self.min_size
		#print self._image, self.min_size, self.max_size

	def _getImage(self):
		return self._image
	image = property(_getImage,_setImage)
