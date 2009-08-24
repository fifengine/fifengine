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
from basictextwidget import BasicTextWidget

class Button(BasicTextWidget):
	"""
	A basic push button.
	"""
	def __init__(self,**kwargs):
		self.real_widget = fife.Button("")
		super(Button,self).__init__(**kwargs)

class ImageButton(BasicTextWidget):
	"""
	A basic push button with three different images for the up, down and hover state.

	B{Work in progress.}

	New Attributes
	==============

	  - up_image: String: The source location of the Image for the B{unpressed} state.
	  - down_image: String: The source location of the Image for the B{pressed} state.
	  - hover_image: String: The source location of the Image for the B{unpressed hovered} state.
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [Attr('up_image'),Attr('down_image'),PointAttr('offset'),Attr('hover_image')]

	def __init__(self,up_image="",down_image="",hover_image="",offset=(0,0),**kwargs):
		self.real_widget = fife.TwoButton()
		super(ImageButton,self).__init__(**kwargs)

		self.up_image = up_image
		self.down_image = down_image
		self.hover_image = hover_image
		self.offset = offset

	def _setUpImage(self, source):
		if isinstance(source,str):
			self._upimage_source = source
			try:
				self._upimage = get_manager().loadImage(source)
				self.real_widget.setUpImage( self._upimage )
			except:
				self._upimage = DummyImage()
		elif isinstance(source,fife.GuiImage):
			self._upimage_source = None
			self._upimage = source
			self.real_widget.setUpImage( self._upimage )
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))

		# Set minimum size accoriding to image
		self.min_size = self.real_widget.getWidth(),self.real_widget.getHeight()
		self.size  = self.max_size = self.min_size

	def _getUpImage(self): return self._upimage_source
	up_image = property(_getUpImage,_setUpImage)

	def _setDownImage(self, source):
		if isinstance(source,str):
			self._downimage_source = source
			try:
				self._downimage = get_manager().loadImage(source)
				self.real_widget.setDownImage( self._downimage )
			except:
				self._downimage = DummyImage()
		elif isinstance(source,fife.GuiImage):
			self._downimage_source = None
			self._downimage = source
			self.real_widget.setDownImage( self._downimage )
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))

	def _getDownImage(self): return self._downimage_source
	down_image = property(_getDownImage,_setDownImage)

	def _setHoverImage(self, source):
		if isinstance(source, str):
			self._hoverimage_source = source
			try:
				self._hoverimage = get_manager().loadImage(source)
				self.real_widget.setHoverImage( self._hoverimage )
			except:
				self._hoverimage = DummyImage()
		elif isinstance(source,fife.GuiImage):
			self._hoverimage_source = None
			self._hoverimage = source
			self.real_widget.setHoverImage( self._hoverimage )
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))

	def _getHoverImage(self): return self._hoverimage_source
	hover_image = property(_getHoverImage,_setHoverImage)

	def _setOffset(self, offset):
		self.real_widget.setDownOffset(offset[0], offset[1])
	def _getOffset(self):
		return (self.real_widget.getDownXOffset(), self.real_widget.getDownYOffset())
	offset = property(_getOffset,_setOffset)

	def resizeToContent(self, recurse=True):
		self.height = max(self._upimage.getHeight(),self._downimage.getHeight(),self._hoverimage.getHeight()) + self.margins[1]*2
		self.width = max(self._upimage.getWidth(),self._downimage.getWidth(),self._hoverimage.getWidth()) + self.margins[0]*2

class ToggleButton(BasicTextWidget):
	"""
	A basic push button that can be toggled.

	Unfortunately a bit of code duplication from ImageButton.

	New Attributes
	==============

	  - group: String: The group the button belongs to. Only one button in each group will be toggled at one time.
	  - toggled: Boolean: Whether the button is toggled or not.
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [
		Attr('up_image'),Attr('down_image'),Attr('hover_image'),
		PointAttr('offset'),Attr('group')
	]

	def __init__(self,up_image="",down_image="",hover_image="",offset=(0,0),group="",**kwargs):

		self.real_widget = fife.ToggleButton()
		super(ToggleButton,self).__init__(**kwargs)
		self.group = group
		self.up_image = up_image
		self.down_image = down_image
		self.hover_image = hover_image
		self.offset = offset

	def _setGroup(self,group):
		self.real_widget.setGroup( group )

	def _getGroup(self):
		return self.real_widget.getGroup()
	group = property(_getGroup,_setGroup)

	def _setToggled(self, toggled):
		self.real_widget.setToggled( toggled )

	def _isToggled(self):
		return self.real_widget.isToggled()
	toggled = property(_isToggled, _setToggled)

	###
	# I didn't want to do this, but this is just cut and paste from the ImageButton class:
	###

	def _setUpImage(self,image):
		self._upimage_source = image
		try:
			self._upimage = get_manager().loadImage(image)
			self.real_widget.setUpImage( self._upimage )
		except:
			self._upimage = DummyImage()
	def _getUpImage(self): return self._upimage_source
	up_image = property(_getUpImage,_setUpImage)

	def _setDownImage(self,image):
		self._downimage_source = image
		try:
			self._downimage = get_manager().loadImage(image)
			self.real_widget.setDownImage( self._downimage )
		except:
			self._downimage = DummyImage()
	def _getDownImage(self): return self._downimage_source
	down_image = property(_getDownImage,_setDownImage)

	def _setHoverImage(self,image):
		self._hoverimage_source = image
		try:
			self._hoverimage = get_manager().loadImage(image)
			self.real_widget.setHoverImage( self._hoverimage )
		except:
			self._hoverimage = DummyImage()
	def _getHoverImage(self): return self._hoverimage_source
	hover_image = property(_getHoverImage,_setHoverImage)

	def _setOffset(self, offset):
		self.real_widget.setDownOffset(offset[0], offset[1])
	def _getOffset(self):
		return (self.real_widget.getDownXOffset(), self.real_widget.getDownYOffset())
	offset = property(_getOffset,_setOffset)

	def resizeToContent(self, recurse=True):
		th = self.real_font.getHeight()+self.real_widget.getSpacing()
		tw = self.real_font.getWidth(text2gui(self.text))+self.real_widget.getSpacing()
		self.height = max(self._upimage.getHeight(),self._downimage.getHeight(),self._hoverimage.getHeight(),th) + self.margins[1]*2
		self.width = max(self._upimage.getWidth(),self._downimage.getWidth(),self._hoverimage.getWidth(),tw) + self.margins[0]*2
