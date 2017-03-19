# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2013 by the FIFE team
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

from fife import fife
from fife import fifechan

from fife.extensions.pychan.attrs import BoolAttr, MixedListAttr
from common import get_manager

from containers import Window

class ResizableWindow(Window):
	"""
	This is a window class that can be resizable.

	New Attributes
	==============

	  - resizable: Allows resizing with the mouse. If disabled all the side parameters are ignored.
	  - top_resizable: If true, the window can be resized at the top side.
	  - right_resizable: If true, the window can be resized at the right side.
	  - bottom_resizable: If true, the window can be resized at the bottom side.
	  - left_resizable: If true, the window can be resized at the left side.
	  - shove: That allows pull and push in case the min/max size is reached.
	  - cursors: List that contains the cursor definitions. Left, Right, Top, Bottom, LeftTop, RightTop, LeftBottom, RightBottom
	"""

	DEFAULT_RESIZABLE = True
	DEFAULT_TOP_RESIZABLE = True
	DEFAULT_RIGHT_RESIZABLE = True
	DEFAULT_BOTTOM_RESIZABLE = True
	DEFAULT_LEFT_RESIZABLE = True
	DEFAULT_SHOVE = False
	
	
	ATTRIBUTES = Window.ATTRIBUTES + [ BoolAttr('resizable'),
									   BoolAttr('top_resizable'),
									   BoolAttr('right_resizable'),
									   BoolAttr('bottom_resizable'),
									   BoolAttr('left_resizable'),
									   BoolAttr('shove'),
									   MixedListAttr('cursors')
									 ]

	DIRECTION_LIST = [ fifechan.ResizableWindow.CURSOR_DIRECTION_L,
					   fifechan.ResizableWindow.CURSOR_DIRECTION_R,
					   fifechan.ResizableWindow.CURSOR_DIRECTION_T,
					   fifechan.ResizableWindow.CURSOR_DIRECTION_B,
					   fifechan.ResizableWindow.CURSOR_DIRECTION_LT,
					   fifechan.ResizableWindow.CURSOR_DIRECTION_RT,
					   fifechan.ResizableWindow.CURSOR_DIRECTION_LB,
					   fifechan.ResizableWindow.CURSOR_DIRECTION_RB
					 ]

	def __init__(self, 
				 parent = None,
				 name = None,
				 size = None,
				 min_size = None,
				 max_size = None,
				 fixed_size = None,
				 margins = None,
				 padding = None,
				 helptext = None,
				 position = None,
				 style = None,
				 hexpand = None,
				 vexpand = None,
				 font = None,
				 base_color = None,
				 background_color = None,
				 foreground_color = None,
				 selection_color = None,
				 border_color = None,
				 outline_color = None,
				 border_size = None,
				 outline_size = None,
				 position_technique = None,
				 is_focusable = None,
				 comment = None,
				 background_image = None,
				 opaque = None,
				 layout = None,
				 spacing = None,
				 uniform_size = None,
				 _real_widget = None,
				 title = None,
				 titlebar_height = None,
				 movable = None,
				 resizable = None,
				 top_resizable = None,
				 right_resizable = None,
				 bottom_resizable = None,
				 left_resizable = None,
				 shove = None,
				 cursors = None):
		
		if _real_widget is None: _real_widget = fifechan.ResizableWindow()
			
		super(ResizableWindow,self).__init__(parent=parent,
											 name=name,
											 size=size,
											 min_size=min_size,
											 max_size=max_size,
											 fixed_size=fixed_size,
											 margins=margins,
											 padding=padding,
											 helptext=helptext,
											 position=position,
											 style=style,
											 hexpand=hexpand,
											 vexpand=vexpand,
											 font=font,
											 base_color=base_color,
											 background_color=background_color,
											 foreground_color=foreground_color,
											 selection_color=selection_color,
											 border_color=border_color,
											 outline_color=outline_color,
											 border_size=border_size,
											 outline_size=outline_size,
											 position_technique=position_technique,
											 is_focusable=is_focusable,
											 comment=comment,
											 background_image=background_image,
											 opaque=opaque,
											 layout=layout,
											 spacing=spacing,
											 uniform_size=uniform_size,
											 _real_widget=_real_widget,
											 title=title,
											 titlebar_height=titlebar_height,
											 movable=movable)

		if resizable is not None: self.resizable = resizable
		else: self.resizable = self.DEFAULT_RESIZABLE

		if top_resizable is not None: self.top_resizable = top_resizable
		else: self.top_resizable = self.DEFAULT_TOP_RESIZABLE

		if right_resizable is not None: self.right_resizable = right_resizable
		else: self.right_resizable = self.DEFAULT_RIGHT_RESIZABLE

		if bottom_resizable is not None: self.bottom_resizable = bottom_resizable
		else: self.bottom_resizable = self.DEFAULT_BOTTOM_RESIZABLE

		if left_resizable is not None: self.left_resizable = left_resizable
		else: self.left_resizable = self.DEFAULT_LEFT_RESIZABLE

		if shove is not None: self.shove = shove
		else: self.shove = self.DEFAULT_SHOVE

		if cursors is not None: self.cursors = cursors

	def clone(self, prefix):
		windowClone = ResizableWindow(None, 
					self._createNameWithPrefix(prefix),
					self.size,
					self.min_size, 
					self.max_size,
					self.fixed_size,
					self.margins,
					self.padding,
					self.helptext, 
					self.position, 
					self.style, 
					self.hexpand,
					self.vexpand,
					self.font,
					self.base_color,
					self.background_color,
					self.foreground_color,
					self.selection_color,
					self.border_color,
					self.outline_color,
					self.border_size,
					self.outline_size,
					self.position_technique,
					self.is_focusable,
					self.comment,
					self.background_image,
					self.opaque,
					self.layout,
					self.spacing,
					self.uniform_size,
					None,
					self.title,
					self.titlebar_height,
					self.movable,
					self.resizable,
					self.top_resizable,
					self.right_resizable,
					self.bottom_resizable,
					self.left_resizable,
					self.shove,
					self.cursors)
		
		windowClone.addChildren(self._cloneChildren(prefix))		     
		return windowClone

	def _setResizable(self, resizable): self.real_widget.setResizable(resizable)
	def _getResizable(self): return self.real_widget.getResizable()
	resizable = property(_getResizable, _setResizable)

	def _setTopResizable(self, resizable): self.real_widget.setTopResizable(resizable)
	def _getTopResizable(self): return self.real_widget.getTopResizable()
	top_resizable = property(_getTopResizable, _setTopResizable)

	def _setRightResizable(self, resizable): self.real_widget.setRightResizable(resizable)
	def _getRightResizable(self): return self.real_widget.getRightResizable()
	right_resizable = property(_getRightResizable, _setRightResizable)

	def _setBottomResizable(self, resizable): self.real_widget.setBottomResizable(resizable)
	def _getBottomResizable(self): return self.real_widget.getBottomResizable()
	bottom_resizable = property(_getBottomResizable, _setBottomResizable)

	def _setLeftResizable(self, resizable): self.real_widget.setLeftResizable(resizable)
	def _getLeftResizable(self): return self.real_widget.getLeftResizable()
	left_resizable = property(_getLeftResizable, _setLeftResizable)

	def _setShove(self, shove): self.real_widget.setShove(shove)
	def _getShove(self): return self.real_widget.getShove()
	shove = property(_getShove, _setShove)

	def _setCursor(self, direction, cursor):
		self.real_widget.set(direction, cursor)
	def _getCursor(self, direction):
		type = self.real_widget.getType(direction)
		if type is fife.CURSOR_NATIVE:
			return self.real_widget.getId(direction)
		elif type is fife.CURSOR_IMAGE:
			return self.real_widget.getImage(direction)
		elif type is fife.CURSOR_Animation:
			return self.real_widget.getAnimation(direction)
	cursor = property(_getCursor, _setCursor)

	def _setCursors(self, cursors):
		if cursors is not None:
			i = 0
			for c in cursors:
				if isinstance(c, str):
					# we need a fife image
					c = get_manager().loadImage(c, gui=False)
				self.real_widget.set(self.DIRECTION_LIST[i], c)
				i += 1
	def _getCursors(self):
		cursors = []
		for d in self.DIRECTION_LIST:
			cursors.append(self.cursor(d))
		return cursors
	cursors = property(_getCursors, _setCursors)

