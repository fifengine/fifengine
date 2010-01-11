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
from fife.extensions.pychan.properties import ImageProperty

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
		self.real_widget = kwargs.get("real_widget", fife.TwoButton())
		super(ImageButton,self).__init__(**kwargs)

		self.up_image = up_image
		self.down_image = down_image
		self.hover_image = hover_image
		self.offset = offset

	up_image = ImageProperty("UpImage")
	down_image = ImageProperty("DownImage")
	hover_image = ImageProperty("HoverImage")

	def _setOffset(self, offset):
		self.real_widget.setDownOffset(offset[0], offset[1])
	def _getOffset(self):
		return (self.real_widget.getDownXOffset(), self.real_widget.getDownYOffset())
	offset = property(_getOffset,_setOffset)

	def resizeToContent(self, recurse=True):
		th, tw = 0, 0
		if self.text:
			th = self.real_font.getHeight()#+self.real_font.getSpacing()
			tw = self.real_font.getWidth(text2gui(self.text))#+self.real_font.getSpacing()
		self.height = max(
			self._prop_upimage["image"].getHeight(),
			self._prop_downimage["image"].getHeight(),
			self._prop_hoverimage["image"].getHeight(),
			th) + self.margins[1]*2
		self.width = max(
			self._prop_upimage["image"].getWidth(),
			self._prop_downimage["image"].getWidth(),
			self._prop_hoverimage["image"].getWidth(),
			tw) + self.margins[0]*2

class ToggleButton(ImageButton):
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

	def __init__(self,group="",**kwargs):

		super(ToggleButton,self).__init__(real_widget = fife.ToggleButton(), **kwargs)
		self.group = group

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

	def resizeToContent(self, recurse=True):
		# NOTE: Figure out how the spacing comes into play
		tw, th = 0, 0
		if self.text:
			th = self.real_font.getHeight() + self.real_widget.getSpacing()
			tw = self.real_font.getWidth(text2gui(self.text)) + self.real_widget.getSpacing()
		self.height = max(
			self._prop_upimage["image"].getHeight(),
			self._prop_downimage["image"].getHeight(),
			self._prop_hoverimage["image"].getHeight(),
			th) + self.margins[1]*2
		self.width = max(
			self._prop_upimage["image"].getWidth(),
			self._prop_downimage["image"].getWidth(),
			self._prop_hoverimage["image"].getWidth(),
			tw) + self.margins[0]*2
