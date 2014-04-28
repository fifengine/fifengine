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

from fife.extensions.pychan.attrs import Attr,UnicodeAttr, PointAttr,BoolAttr,IntAttr

from common import get_manager, gui2text, text2gui
from widget import Widget


class Tab(Widget):
	"""
	A basic tab.

	New Attributes
	==============


	Data
	====
	The marked status can be read and set via L{distributeData} and L{collectData}
	"""
	
	ATTRIBUTES = Widget.ATTRIBUTES + [ UnicodeAttr('text') ]
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 0
	DEFAULT_MARGINS = 0
	DEFAULT_TEXT = u""
	
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
				 margins = None,
				 text = None):
				 
		self.real_widget = fifechan.Tab()
		self.margins = self.DEFAULT_MARGINS
		self.text = self.DEFAULT_TEXT
		super(Tab,self).__init__(parent=parent, 
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
									  margins=margins,
									  text=text)

		# Prepare Data collection framework
		if margins is not None: self.margins = margins
		if text is not None: self.text = text

	def clone(self, prefix):
		tabClone = Tab(None, 
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
						self.margins,
						self.text)
		return tabClone

	def _getText(self): return gui2text(self.real_widget.getCaption())
	def _setText(self,text): self.real_widget.setCaption(text2gui(text))

	text = property(_getText,_setText)


class TabbedArea(Widget):
	"""
	This is the tabbed area class.

	New Attributes
	==============


	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [ IntAttr('padding'),
									   BoolAttr('opaque'),
									   PointAttr('margins') 
									 ]

	DEFAULT_OPAQUE = True
	DEFAULT_MARGINS = 5,5
	DEFAULT_PADDING = 5
	DEFAULT_POSITION_TECHNIQUE = "automatic"
	
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
				 opaque = None,
				 margins = None,
				 _real_widget = None):


		self.real_widget = _real_widget or fifechan.TabbedArea()
		self.tabs = []
		self.opaque = self.DEFAULT_OPAQUE

		super(TabbedArea,self).__init__(parent=parent, 
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
									   comment=comment)

		if margins is not None:
			self.margins = margins
		else:
			self.margins = self.DEFAULT_MARGINS
			
		if padding is not None:
			self.padding = padding
		else:
			self.DEFAULT_PADDING
		if opaque is not None: self.opaque = opaque

		
	def clone(self, prefix):
		tabbedareaClone = TabbedArea(None, 
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
						self.margins)
			
		#tabbedareaClone.addTabs(self._cloneChildren(prefix))
			
		return tabbedareaClone
		

	def addTab(self, widget, text=""):
		"""
		Adds a child widget to the container.
		
		"""
		
		widget.parent = self
			
		self.tabs.append(widget)
		self.real_widget.addTab(text2gui(text), widget.real_widget)


	def removeTab(self, widget):
		if not widget in self.tabs:
			raise RuntimeError("%s does not have %s as direct tab widget." % (str(self),str(widget)))


		i = self.tabs.index(widget)
		self.real_widget.removeTabWithIndex(i)
		self.tabs.remove(widget)
		widget.parent = None
			
	def add(self,*widgets):
		print "PyChan: Deprecation warning: Please use 'addTab' or 'addTabs' instead."
		self.addTabs(*widgets)

	def addTabs(self,*widgets):
		if len(widgets) == 1 and not isinstance(widgets[0],Widget):
			widgets = widgets[0]
		for widget in widgets:
			self.addTab(widget)


	def deepApply(self,visitorFunc, leaves_first = True, shown_only = False):
		if not shown_only:
			children = self.tabs
		else:
			children = filter(lambda w: w.real_widget.isVisible(), self.tabs)
		
		if leaves_first:
			for child in children:
				child.deepApply(visitorFunc, leaves_first = leaves_first, shown_only = shown_only)
		visitorFunc(self)
		if not leaves_first:
			for child in children:
				child.deepApply(visitorFunc, leaves_first = leaves_first, shown_only = shown_only)


	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)

	def _cloneChildren(self, prefix):
		"""
		Clones each child and return the clones in a list.
		"""
		cloneList = [ child.clone(prefix) for child in self.tabs ]
		
		return cloneList