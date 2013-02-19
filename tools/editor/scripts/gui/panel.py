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

from fife.extensions import pychan
from fife.extensions.pychan import widgets
from fife.extensions.pychan.internal import get_manager
import scripts.editor
from fife import fife
from resizablebase import ResizableBase

class Panel(widgets.Window, ResizableBase):
	""" Panel is a window which can be resized and docked. 
	"""
	def __init__(self, dockable=True, *args, **kwargs):
		
		argscopy = kwargs.copy()
		if "resizable" in argscopy:
			del argscopy["resizable"]
		widgets.Window.__init__(self, *args, **argscopy)
		ResizableBase.__init__(self, *args, **kwargs)
		
		self.dockable = dockable
		self._movable = self.real_widget.isMovable()
		self._resizable = self.resizable

		self._floating = True
		self._titlebarheight = 16
		
		self.dockarea = None
		
		self.dockareaname = ""
		
		self._editor = scripts.editor.getEditor()
		
		self._panel_startPos = (0, 0)
		self.abs_pos_backup = ()
		
	def afterDock(self):
		""" called after setDocked() process if the panel is docked
		
			overwrite this to e.g. keep a record on where the widget
			was last docked
		"""
		pass
		
	def afterUndock(self):
		""" called after setDocked() process if the panel is undocked 
		
			overwrite and implement this in your Panel instance
			to e.g. restore a particular default position
			(otherwise the panel would re-appear on the top-left)
			
		@todo:
			- this implementation is focused on the ToolBar (probably a
			  leftover from previous versions as the Panel widget only
			  was designed for the ToolBar)
			- this should be more generalized to work better for all Panels
		"""
		if not self.abs_pos_backup: return 
		absX, absY = self.abs_pos_backup

		# Slighly offset toolbar when undocking
		mw = pychan.internal.screen_width() / 2
		mh = pychan.internal.screen_height() / 2
		if absX < mw:
			self.x = absX + self._titlebarheight
		else:
			self.x = absX - self._titlebarheight
		if absY < mh:
			self.y = absY + self._titlebarheight
		else:
			self.y = absY - self._titlebarheight

		self.abs_pos_backup = ()
		
	def setDocked(self, docked):
		""" 
		Dock or undock the panel
		
		setDocked(True) will disable resizing and moving
		of this panel, but will not dock it in a dockarea.
		
		setDocked(False) will enable resizing and moving.
		If this panel is docked in a dockarea or widget,
		it will undock itself. The new position will be 
		offset by panelSize.
		
		@type	docked:	bool
		@param	docked:	flag to either set the Panel to docked status or not
		"""
		if not self.dockable:
			return
		
		if docked and self._floating:
			self._floating = False
			self.real_widget.setTitleBarHeight(0)
			self.real_widget.setMovable(False)
			self._movable = False
			self.resizable = False
			self.afterDock()
				
		elif not docked and not self._floating:
			self._floating = True
			self._movable = True
			self.real_widget.setMovable(True)
			self.resizable = self._resizable
			self.dockareaname = ""
			
			# Since x and y coordinates are reset if the widget gets hidden,
			# we need to store them
			self.abs_pos_backup = self.getAbsolutePos()
			
			# Remove from parent widget
			if self.dockarea is not None:
				# @note: this step is necessary, otherwise the widget.isVisible()
				# call returns True although the widget is not visible
				# @note: self.parent is not None if the ToolBar calls this method
				# that's why cheesesucker added the elif branch
				if self.parent is None:
					self.hide()
				# Use dockareas undock method
				self.dockarea.undockChild(self, True)
				self.dockarea = None
			elif self.parent is not None:
				# Force removal
				widgetParent = self.parent
				widgetParent.removeChild(self)
				widgetParent.adaptLayout()
				get_manager().show(self)

			self.real_widget.setTitleBarHeight(self._titlebarheight)
			self.show()
			self.adaptLayout()
			
			self.afterUndock()		

	def isDocked(self):
		""" Returns true if the panel is docked """
		return self._floating == False
		
	def mousePressed(self, event):
		if not self.resizable:
			return
			
		self._panel_startPos = (self.x, self.y)
		
		ResizableBase.mousePressed(self, event)
		if self._rLeft or self._rRight or self._rTop or self._rBottom:
			self._movable = self.real_widget.isMovable()
			self.real_widget.setMovable(False) # Don't let fifechan move window while we resize
			
	def mouseDragged(self, event):
		self._dragging = True
		if not self._resize and not self.isDocked():
			if (self.x, self.y) == self._panel_startPos:
				return
			mouseX = self.x+event.getX()
			mouseY = self.y+event.getY()
			self._editor.getDockAreaAt(mouseX, mouseY, True)
		else:
			ResizableBase.mouseDragged(self, event)
	
	def mouseReleased(self, event):	
		didMove = False
		if (self.x, self.y) != self._panel_startPos:
			didMove = True
		
		# Resize/move done
		self.real_widget.setMovable(self._movable)
		
		if self._resize:
			ResizableBase.mouseReleased(self, event)
		elif self._movable and didMove:
			mouseX = self.x+event.getX()
			mouseY = self.y+event.getY()
			
			dockArea = self._editor.getDockAreaAt(mouseX, mouseY)
			if dockArea is not None:
				self._editor.dockWidgetTo(self, dockArea, mouseX, mouseY)
		
	def hide(self):
		""" Hides the panel. If the widget is docked, it is first undocked """
		if self.isDocked():
			self.setDocked(False)
		widgets.Window.hide(self)
		
	def show(self):
		""" Show the panel. """
		if self.isDocked():
			return
		widgets.Window.show(self)
	
# Register widget to pychan
if 'Panel' not in widgets.WIDGETS:
	widgets.WIDGETS['Panel'] = Panel
		
