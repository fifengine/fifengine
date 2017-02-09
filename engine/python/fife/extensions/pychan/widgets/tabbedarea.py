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
from containers import Container
from widget import Widget


class Tab(Container):
	"""
	A tab widget.
	It can be used as a container.
	"""

	ATTRIBUTES = Container.ATTRIBUTES + [ UnicodeAttr('content_name'),
										]
	DEFAULT_HEXPAND = True
	DEFAULT_VEXPAND = False
	DEFAULT_MARGINS = 0, 0
	DEFAULT_PADDING = 0
	
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
				 _real_widget = None):

		if _real_widget is None: _real_widget = fifechan.Tab()
		# name of the widget that is associated with this tab
		self.content_name = None
		
		super(Tab,self).__init__(parent=parent,
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
								 _real_widget=_real_widget)
		
	def addTabToWidget(self, widget):
		if widget.tab is self: return
		if widget.tab is not None:
			self.removeTabFromWidget(widget)
		widget.tab = self

	def removeTabFromWidget(self, widget):
		if widget.tab == self:
			widget.tab = None

	def clone(self, prefix):
		tabClone = Tab(None, 
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
					self.opaque)
					
		tabClone.addChildren(self._cloneChildren(prefix))
					
		return tabClone


class TabbedArea(Container):
	"""
	This is the tabbed area class.

	New Attributes
	==============


	"""
	ATTRIBUTES = Container.ATTRIBUTES + [ IntAttr('select_tab_index'),
										  UnicodeAttr('select_tab'),
										]

	DEFAULT_OPAQUE = False
	DEFAULT_MARGINS = 0, 0
	DEFAULT_PADDING = 0
	DEFAULT_LAYOUT = 'Horizontal'
	DEFAULT_POSITION_TECHNIQUE = "automatic"
	
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
				 select_tab_index = None,
				 select_tab = None):

		if _real_widget is None: _real_widget = fifechan.TabbedArea()
		
		super(TabbedArea,self).__init__(parent=parent,
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
										_real_widget=_real_widget)

		self.tab_definitions = {}
		# only use one
		if select_tab_index is not None: self.select_tab_index = select_tab_index
		elif select_tab is not None: self.select_tab = select_tab
		
	def clone(self, prefix):
		tabbedareaClone = TabbedArea(None, 
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
						select_tab_index)
			
		tabbedareaClone.addChilds(self._cloneChildren(prefix))
			
		return tabbedareaClone
		

	def addChild(self, widget):
		"""
		Adds a child widget to the container.
		
		"""

		# if the widget have no tabwidget, we try to find one
		if widget.tab is None:
			tab = self.getTabDefinition(widget)
			tab.addTabToWidget(widget)
			
		widget.parent = self

		if widget.max_size[0] > self.max_size[0] or widget.max_size[1] > self.max_size[1]:
			widget.max_size = self.max_size

		self.children.append(widget)

		# add real tab and real widget
		self.real_widget.addTab(widget.tab.real_widget, widget.real_widget)
		# add all to the manager
		def _add(added_widget):
			if not added_widget._added:
				get_manager().addWidget(added_widget)
			if added_widget._top_added:
				get_manager().removeTopWidget(added_widget)
		widget.deepApply(_add)

	def removeChild(self, widget):
		if not widget in self.children:
			raise RuntimeError("%s does not have %s as direct tab widget." % (str(self),str(widget)))

		# remove all from the manager
		def _remove(removed_widget):
			if removed_widget._added:
				get_manager().removeWidget(removed_widget)
			if removed_widget._top_added:
				get_manager().removeTopWidget(removed_widget)
		widget.deepApply(_remove)

		i = self.children.index(widget)
		self.real_widget.removeTabWithIndex(i)
		self.children.remove(widget)
		widget.parent = None

	def addTabDefinition(self, widget):
		# Only needed because of XML loading
		# The tab have the content_name that should be equal to
		# the name of the contained widget
		self.tab_definitions[widget.content_name] = widget

	def getTabDefinition(self, widget):
		# Only needed because of XML loading
		# Check if we have a tab that is associated with the widget
		name = widget.name
		if name in self.tab_definitions:
			widget = self.tab_definitions[name]
			del self.tab_definitions[name]
			return widget
		else:
			raise RuntimeError("%s does not have %s in the tab definitions." % (str(self),str(name)))
		
	def getNumberOfTabs(self):
		return self.real_widget.getNumberOfTabs()

	def isTabSelected(self, widget):
		i = self.children.index(widget)
		return self.real_widget.isTabSelected(i)
	
	def _setSelectedTab(self, widget):
		if isinstance(widget, str):
			widget = self.findChild(name=widget)
		if widget is not None:
			i = self.children.index(widget)
			self.real_widget.setSelectedTab(i)
	def _getSelectedTab(self):
		i = self.real_widget.getSelectedTabIndex()
		return self.children[i]
	select_tab = property(_getSelectedTab, _setSelectedTab)
	
	def _setSelectedTabIndex(self, index):
		self.real_widget.setSelectedTab(index)
	def _getSelectedTabIndex(self):
		return self.real_widget.getSelectedTabIndex()
	select_tab_index = property(_getSelectedTabIndex, _setSelectedTabIndex)
