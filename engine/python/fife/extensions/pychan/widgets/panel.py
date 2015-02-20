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

import weakref
from fife import fife
from fife import fifechan

from fife.extensions.pychan.attrs import BoolAttr, MixedListAttr
from common import get_manager

from resizablewindow import ResizableWindow
from dockarea import DockArea
from containers import Container

class Panel(ResizableWindow):
	"""
	The Panel class can be docked or undocked from Dock Areas.

	"""

	def __init__(self, 
				 parent = None, 
				 name = None,
				 size = None,
				 min_size = None, 
				 max_size = None,
				 fixed_size = None,
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
								   fixed_size=fixed_size,
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

		self._foundDockArea = None
		
		self._barPressedLeft = False
		self._barPressedRight = False
		self._barReleasedLeft = False
		self._barReleasedRight = False
		
		self.capture(self.mousePressed, "mousePressed", "Panel")
		self.capture(self.mouseReleased, "mouseReleased", "Panel")
		self.capture(self.mouseDragged, "mouseDragged", "Panel")

	def clone(self, prefix):
		panelClone = Panel(None, 
					self._createNameWithPrefix(prefix),
					self.size,
					self.min_size, 
					self.max_size,
					self.fixed_size,
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
		
		panelClone.addChildren(self._cloneChildren(prefix))		     
		return panelClone

	def _getDocked(self): return self.real_widget.isDocked()
	def _setDocked(self, docked): self.real_widget.setDocked(docked)
	docked = property(_getDocked, _setDocked)

	def getDockArea(self):
		# try to find dock area
		# search only includes parent container
		if not self.docked and self.parent:
			dim = fife.Rect(0, 0, self.width, self.height)
			dim.x, dim.y = self.getAbsolutePos()
			childs = self.parent.children
			for c in childs:
				if isinstance(c, DockArea) and c.real_widget.isActiveDockArea():
					cdim = fife.Rect(0, 0, c.width, c.height)
					cdim.x, cdim.y = c.getAbsolutePos()
					if dim.intersects(cdim):
						return c
			return None
		else:
			return self.parent

	def afterDock(self):
		""" called after dockTo() process if the panel is docked
		
			overwrite this to e.g. keep a record on where the widget
			was last docked
		"""
		pass
		
	def afterUndock(self):
		""" called after undockTo() process if the panel is undocked 
		
			overwrite and implement this in your Panel instance
			to e.g. restore a particular default position
			(otherwise the panel would re-appear on the center)
		"""
		pass
	
	def dockTo(self, widget):
		if not self.docked and widget is not self.parent:
			widget.real_widget.setHighlighted(False)
			# map coordinates to new parent
			self.x = (self.x / (self.parent.width / 100)) * (widget.width / 100)
			self.y = (self.y / (self.parent.height / 100)) * (widget.height / 100)
			self.parent.removeChild(self)
			widget.addChild(self)
			self.docked = True
			self.afterDock()

	def undockTo(self, widget):
		if self.docked and widget is not self.parent:
			self.parent.removeChild(self)
			widget.addChild(self)
			self.docked = False
			self.afterUndock()
		
	def mousePressed(self, event):
		h = self.real_widget.getBorderSize() + self.real_widget.getPaddingTop() + self.real_widget.getTitleBarHeight()
		self._barPressedLeft = event.getButton() == 1 and event.getY() <= h and event.getY() > self.real_widget.getResizableBorderDistance()
		self._barPressedRight = event.getButton() == 2 and event.getY() <= h and event.getY() > self.real_widget.getResizableBorderDistance()
		self._barReleasedLeft = False
		self._barReleasedRight = False

	def mouseReleased(self, event):
		h = self.real_widget.getBorderSize() + self.real_widget.getPaddingTop() + self.real_widget.getTitleBarHeight()
		self._barReleasedLeft = event.getButton() == 1 and event.getY() <= h and event.getY() > self.real_widget.getResizableBorderDistance()
		self._barReleasedRight = event.getButton() == 2 and event.getY() <= h and event.getY() > self.real_widget.getResizableBorderDistance()
		releasedLeft = self._barPressedLeft and self._barReleasedLeft
		releasedRight = self._barPressedRight and self._barReleasedRight
		self._barPressedLeft = False
		self._barPressedRight = False
		if releasedLeft and self._foundDockArea and not self.docked:
			self.dockTo(self._foundDockArea())
		elif releasedRight and self.docked:
			if self.parent.parent:
				newParent = self.parent.parent
				self.undockTo(newParent)
				#get_manager().addTopWidget(self)
				#self._top_added = True

	def mouseDragged(self, event):
		# disable highlighting
		if self._foundDockArea is not None:
			self._foundDockArea().real_widget.setHighlighted(False)
			self._foundDockArea = None

		if not self.docked and self._barPressedLeft:
			dock = self.getDockArea()
			# enable highlighting for dock area
			if dock is not None and dock.real_widget.isActiveDockArea():
				self._foundDockArea = weakref.ref(dock)
				self._foundDockArea().real_widget.setHighlighted(True)
				


