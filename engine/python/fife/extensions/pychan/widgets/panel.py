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

from fife.extensions.pychan.attrs import BoolAttr
from common import get_manager

from resizablewindow import ResizableWindow
from dockarea import DockArea
from containers import Container

class Panel(ResizableWindow):
	"""
	The Panel class can be docked or undocked from Dock Areas.
	If the Panel is added to a DockArea e.g. by XML loading, then it will automatically docked.
	By default undock will add the Panel to the parent of the DockArea, that can also be the top widget.

	New Attributes
	==============

	  - dockable: If true, the Panel can be docked/undocked to DockAreas.
	"""
	
	ATTRIBUTES = ResizableWindow.ATTRIBUTES + [ BoolAttr('dockable'),
												]


	DEFAULT_DOCKABLE = True


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
				 dockable = None):
		
		if _real_widget is None: _real_widget = fifechan.Panel()
			
		super(Panel,self).__init__(parent=parent,
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

		if dockable is not None: self.dockable = dockable
		else: self.dockable = self.DEFAULT_DOCKABLE
		
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
					self.dockable)
		
		panelClone.addChildren(self._cloneChildren(prefix))		     
		return panelClone

	def _getDocked(self): return self.real_widget.isDocked()
	def _setDocked(self, docked): self.real_widget.setDocked(docked)
	docked = property(_getDocked, _setDocked)

	def _getDockable(self): return self.real_widget.isDockable()
	def _setDockable(self, dockable): self.real_widget.setDockable(dockable)
	dockable = property(_getDockable, _setDockable)
	
	def getDockArea(self):
		if not self.docked:
			dockAreas = []
			# all top widgets are used for the search
			if not self.parent:
				topWidgets = get_manager().allTopHierachyWidgets
				for t in topWidgets:
					dockAreas.extend(t.findChildren(__class__=DockArea))
			else:
				# only all childs are used for the search
				dockAreas = self.parent.findChildren(__class__=DockArea)
			# reverse order so inner/deeper Areas are preferred
			dockAreas.reverse()
			# try to find an intersecting and active DockArea
			dim = fife.Rect(0, 0, self.width, self.height)
			dim.x, dim.y = self.getAbsolutePos()
			for d in dockAreas:
				if d.real_widget.isActiveDockArea():
					ddim = fife.Rect(0, 0, d.width, d.height)
					ddim.x, ddim.y = d.getAbsolutePos()
					if dim.intersects(ddim):
						return d
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
		# Dock the Panel to the given widget.
		if not self.docked and widget is not self.parent and self.dockable:
			widget.real_widget.setHighlighted(False)
			# map coordinates to new parent and remove it from old parent
			if self.parent:
				self.x = (self.x // (self.parent.width // 100)) * (widget.width // 100)
				self.y = (self.y // (self.parent.height // 100)) * (widget.height // 100)
				self.parent.removeChild(self)
			else:
				self.x = (self.x // (get_manager().hook.screen_width // 100)) * (widget.width // 100)
				self.y = (self.y // (get_manager().hook.screen_height // 100)) * (widget.height // 100)
				get_manager().removeTopWidget(self)
			# dock it to new parent
			widget.addChild(self)
			self.docked = True
			self.afterDock()

	def undockTo(self, widget):
		# Undock the Panel to the given widget.
		if self.docked and widget is not self.parent and self.dockable:
			self.parent.removeChild(self)
			# undock to main gui
			if widget is None:
				get_manager().addTopWidget(self)
			else:
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
			# by default it undocks to the parent of the DockArea
			if self.parent.parent:
				newParent = self.parent.parent
				self.undockTo(newParent)
			else:
				self.undockTo(None)

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
