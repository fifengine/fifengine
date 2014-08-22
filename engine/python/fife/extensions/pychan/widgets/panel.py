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

from resizablewindow import ResizableWindow

class Panel(ResizableWindow):
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


	def __init__(self, 
				 parent = None, 
				 name = None,
				 size = None,
				 min_size = None, 
				 max_size = None, 
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
				 border_size = None,
				 position_technique = None,
				 is_focusable = None,
				 comment = None,
				 padding = None,
				 background_image = None,
				 opaque = None,
				 margins = None,
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
		
		if _real_widget is None: _real_widget = fifechan.Panel()
			
		super(Panel,self).__init__(parent=parent,
											 name=name,
											 size=size,
											 min_size=min_size,
											 max_size=max_size,
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
											 border_size=border_size,
											 position_technique=position_technique,
											 is_focusable=is_focusable,
											 comment=comment,
											 padding=padding,
											 background_image=background_image,
											 opaque=opaque,
											 margins=margins,
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

		self.capture(self.mouseReleased, "mouseReleased", "Panel")
		self._docked = False

	def clone(self, prefix):
		windowClone = Panel(None, 
					self._createNameWithPrefix(prefix),
					self.size,
					self.min_size, 
					self.max_size, 
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
					self.border_size,
					self.position_technique,
					self.is_focusable,
					self.comment,
					self.padding,
					self.background_image,
					self.opaque,
					self.margins,
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


	def mouseReleased(self, event):
		# needed for proper parent
		if self.real_widget.isDocked() != self._docked:
			self._docked = self.real_widget.isDocked()
			if self.parent:
				if self in self.parent.children:
					self.parent.children.remove(self)

				if self._docked:
					children = self.parent.children
					real_parent = self.real_widget.getParent()
				else:
					children = [self.parent.parent,]
					real_parent = self.parent.real_widget.getParent()
					
				self.parent = None
				# remove all from the manager
				def _remove(removed_widget):
					if removed_widget._added:
						get_manager().removeWidget(removed_widget)
					if removed_widget._top_added:
						get_manager().removeTopWidget(removed_widget)
				self.deepApply(_remove)
				
				for child in children:
					if child.real_widget.this == real_parent.this:
						self.parent = child
						if self.max_size[0] > child.max_size[0] or self.max_size[1] > child.max_size[1]:
							self.max_size = child.max_size
							
						child.children.append(self)
						# add all to the manager
						def _add(added_widget):
							if not added_widget._added:
								get_manager().addWidget(added_widget)
							if added_widget._top_added:
								get_manager().removeTopWidget(added_widget)
						self.deepApply(_add)
						break

