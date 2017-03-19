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

from fife.extensions.pychan.attrs import BoolAttr, ColorAttr, UnicodeAttr
from fife.extensions.pychan.properties import ColorProperty
from common import get_manager

from resizablewindow import ResizableWindow

class DockArea(ResizableWindow):
	"""
	This is a window class that can be resizable and it's possible to dock Panels.

	New Attributes
	==============

	  - active: If true, the DockArea is active and Panels can be docked to it.
	  - side: Allows to specify the side for expanding.
	  - highlight_color: Color that is used if a Panel enters the DockArea.
	"""

	ATTRIBUTES = ResizableWindow.ATTRIBUTES + [ BoolAttr('active'),
												UnicodeAttr('side'),
												ColorAttr('highlight_color'),
												]

	DEFAULT_MARGINS = 0
	DEFAULT_PADDING = 0
	DEFAULT_SPACING = 0
	
	DEFAULT_ACTIVE = True
	DEFAULT_HIGHLIGHT_COLOR = (200, 0, 0)

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
				 cursors = None,
				 active = None,
				 side = None,
				 highlight_color = None):
		
		if _real_widget is None: _real_widget = fifechan.DockArea()
			
		super(DockArea,self).__init__(parent=parent,
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
									  movable=movable,
									  resizable=resizable,
									  top_resizable=top_resizable,
									  right_resizable=right_resizable,
									  bottom_resizable=bottom_resizable,
									  left_resizable=left_resizable,
									  shove=shove,
									  cursors=cursors)

		if active is not None: self.active = active
		else: self.active = self.DEFAULT_ACTIVE
		
		if side is not None: self.side = side

		if highlight_color is not None: self.highlight_color = highlight_color
		else: self.highlight_color = self.DEFAULT_HIGHLIGHT_COLOR

	def clone(self, prefix):
		dockAreaClone = DockArea(None, 
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
					self.cursors,
					self.active,
					self.side,
					self.highlight_color)
		
		dockAreaClone.addChildren(self._cloneChildren(prefix))		     
		return dockAreaClone

	def addChild(self, widget):
		super(DockArea,self).addChild(widget)
		widget.docked = True

	def _getActiveDockArea(self): return self.real_widget.isActiveDockArea()
	def _setActiveDockArea(self, active): self.real_widget.setActiveDockArea(active)
	active = property(_getActiveDockArea, _setActiveDockArea)
	
	def isTopSide(self):
		return self.real_widget.isTopSide()
	
	def setTopSide(self, side):
		self.real_widget.setTopSide(side)
		
	def isRightSide(self):
		return self.real_widget.isRightSide()
	
	def setRightSide(self, side):
		self.real_widget.setRightSide(side)

	def isBottomSide(self):
		return self.real_widget.isBottomSide()
	
	def setBottomSide(self, side):
		self.real_widget.setBottomSide(side)

	def isLeftSide(self):
		return self.real_widget.isLeftSide()
	
	def setLeftSide(self, side):
		self.real_widget.setLeftSide(side)
		
	def _getSide(self):
		if self.real_widget.isTopSide():
			return 'top'
		elif self.real_widget.isRightSide():
			return 'right'
		elif self.real_widget.isBottomSide():
			return 'bottom'
		elif self.real_widget.isLeftSide():
			return 'left'
		return ''
	def _setSide(self, side):
		self.setTopSide(False)
		self.setRightSide(False)
		self.setBottomSide(False)
		self.setLeftSide(False)
		if side == 'top':
			self.setTopSide(True)
		elif side == 'right':
			self.setRightSide(True)
		elif side == 'bottom':
			self.setBottomSide(True)
		elif side == 'left':
			self.setLeftSide(True)
	side = property(_getSide, _setSide)

	highlight_color = ColorProperty("HighlightColor")
