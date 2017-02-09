# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
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

from fife.extensions.pychan.attrs import Attr, UnicodeAttr, PointAttr, BoolAttr, IntAttr

from common import get_manager, gui2text, text2gui
from widget import Widget


class Container(Widget):
	"""
	This is the basic container class. It provides space in which child widgets can
	be position via the position attribute. If you want to use the layout engine,
	you have to use derived containers with vertical or horizontal orientation
	(L{VBox} or L{HBox})

	New Attributes
	==============

	  - background_image - Set this to a GuiImage or a resource location (simply a filename).
	    The image will be tiled over the background area.
	  - opaque - Boolean: Whether the background should be drawn at all. Set this to False
	    to make the widget transparent.
	  - spacing - Set the vertical and horizontal spacing between the childs.
	  - uniform_size - Boolean: If true, the free space is distributed in a way that the size of the
	    childrens will be equal (if possible), otherwise the free space will evenly distributed.
	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [ Attr('background_image'), 
									   BoolAttr('opaque'),
									   UnicodeAttr('layout'),
									   PointAttr('spacing'),
									   BoolAttr('uniform_size')
									 ]

	DEFAULT_BACKGROUND = None
	DEFAULT_OPAQUE = True
	#DEFAULT_MARGINS = 0,0
	#DEFAULT_PADDING = 5
	DEFAULT_SPACING = 0,0
	DEFAULT_UNIFORM_SIZE = False
	DEFAULT_LAYOUT = 'Absolute'
	
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
				 _real_widget = None):
				 
		self.real_widget = _real_widget or fifechan.Container()
		self.children = []
		self._background = []
		self._background_image = None
		self.background_image = self.DEFAULT_BACKGROUND
		
		self.opaque = self.DEFAULT_OPAQUE
		#self.layout = self.DEFAULT_LAYOUT

		super(Container,self).__init__(parent=parent, 
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
									   comment=comment)
									   
		if opaque is not None: self.opaque = opaque
		if background_image is not None: self.background_image = background_image
		
		if layout is not None: self.layout = layout
		else: self.layout = self.DEFAULT_LAYOUT
		
		if spacing is not None: self.spacing = spacing
		else: self.spacing = self.DEFAULT_SPACING

		if uniform_size is not None: self.uniform_size = uniform_size
		else: self.uniform_size = self.DEFAULT_UNIFORM_SIZE
		
	def clone(self, prefix):
		containerClone = Container(None, 
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
						None)
			
		containerClone.addChildren(self._cloneChildren(prefix))
			
		return containerClone


	def addChild(self, widget):
		"""
		Adds a child widget to the container.
		
		This makes the childs widgets visible state the same as the containers.
		i.e. if the containter is visible the child will be as well and if the
		container widget is hidden so will the child.  The child however WILL
		be shown when you show the container widget.  If you want the child to 
		be hidden when you show the container widget you must call child.hide().
		"""
		
		widget.parent = self

		if widget.max_size[0] > self.max_size[0] or widget.max_size[1] > self.max_size[1]:
			widget.max_size = self.max_size
		
		self.children.append(widget)
		self.real_widget.add(widget.real_widget)
		# add all to the manager
		def _add(added_widget):
			if not added_widget._added:
				get_manager().addWidget(added_widget)
			if added_widget._top_added:
				get_manager().removeTopWidget(added_widget)
		widget.deepApply(_add)
		
	def insertChild(self, widget, position):
		if position > len(self.children) or 0-position > len(self.children):
			print "insertChild: Warning: Index overflow.",
			if position >= 0:
				self.addChild(widget)
			else:
				self.insertChild(widget, 0)
			return
		
		children = self.children[0:position]+[widget]+self.children[position:]
		#assert len(children) == len(self.children) + 1

		for child_to_remove in self.children[:]:
			self.removeChild(child_to_remove)
		
		for child in children:
			self.addChild(child)

	def insertChildBefore(self, widget, before):
		if before not in self.children:
			raise RuntimeError("Couldn't find widget %s as child of %s - in insertChildBefore" % (str(widget),str(before)))
		self.insertChild(widget, self.children.index(before))

	def removeChild(self,widget):
		if not widget in self.children:
			raise RuntimeError("%s does not have %s as direct child widget." % (str(self),str(widget)))

		if widget in self.children:
			self.children.remove(widget)
			self.real_widget.remove(widget.real_widget)

		widget.parent = None
		# remove all from the manager
		def _remove(removed_widget):
			if removed_widget._added:
				get_manager().removeWidget(removed_widget)
			if removed_widget._top_added:
				get_manager().removeTopWidget(removed_widget)
		widget.deepApply(_remove)

	def hideChild(self, child, free=False):
		# remove child from the manager
		if child._added:
			get_manager().removeWidget(child)
		if child._top_added:
			get_manager().removeTopWidget(child)
		# remove childs of the child from the manager
		def _hide(hidden_widget):
			get_manager().removeWidget(hidden_widget)
		child.deepApply(_hide)
		
		if child.isVisible() or child.isSetVisible():
			# Hide real widget to distribute a widgetHidden event.
			child.real_widget.setVisible(False)
			
		if free:
			self.removeChild(child)
		self.adaptLayout()
		self.afterHide()
		
	def showChild(self, child):
		# add child to the manager
		if not child._added:
			get_manager().addWidget(child)
		# add childs of child to the manager
		def _show(shown_widget):
			get_manager().addWidget(shown_widget)
		child.deepApply(_show)

		child.beforeShow()
		if not child.isVisible() or not child.isSetVisible():
			# Show real widget to distribute a widgetShown event.
			child.real_widget.setVisible(True)

		self.adaptLayout()
			
	def add(self,*widgets):
		print "PyChan: Deprecation warning: Please use 'addChild' or 'addChildren' instead."
		self.addChildren(*widgets)

	def getMaxChildrenWidth(self):
		if not self.children: return 0
		w = 0
		for widget in self.children:
			if not widget.real_widget.isVisible(): continue
			w = max(widget.width, w)
		return w

	def getMaxChildrenHeight(self):
		if not self.children: return 0
		h = 0
		for widget in self.children:
			if not widget.real_widget.isVisible(): continue
			h = max(widget.height, h)
		return h

	def deepApply(self,visitorFunc, leaves_first = True, shown_only = False):
		if not shown_only:
			children = self.children
		else:
			children = filter(lambda w: w.real_widget.isVisible(), self.children)
		
		if leaves_first:
			for child in children:
				child.deepApply(visitorFunc, leaves_first = leaves_first, shown_only = shown_only)
		visitorFunc(self)
		if not leaves_first:
			for child in children:
				child.deepApply(visitorFunc, leaves_first = leaves_first, shown_only = shown_only)

	def beforeShow(self):
		# This is required because beforeShow() is NOT called on nested
		# containers or child widgets.  This ensures that background tiled 
		# images are shown properly
		def _resetTilingChildren(widget):
			tilingMethod = getattr(widget, "_resetTiling", None)
			if callable(tilingMethod):
				tilingMethod()
		self.deepApply(_resetTilingChildren)

		self._resetTiling()

	def _resetTiling(self):
		image = self._background_image
		if image is None:
			return

		back_w,back_h = self.width, self.height

		# Now tile the background over the widget
		self._background = []
		icon = fifechan.Icon(image)
		icon.setTiling(True)
		icon.setSize(back_w,back_h)
		self._background.append(icon)
		self.real_widget.setBackgroundWidget(icon)

	def setBackgroundImage(self,image):
		#self._background = getattr(self,'_background',None)
		if image is None:
			self._background_image = None
			if len(self._background) > 0:
				self.real_widget.setBackgroundWidget(None)
			self._background = []
			return
		# Background generation is done in _resetTiling

		if not isinstance(image, fife.GuiImage):
			image = get_manager().loadImage(image)
		self._background_image = image

	def getBackgroundImage(self): return self._background_image
	background_image = property(getBackgroundImage,setBackgroundImage)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)

	def _setVerticalSpacing(self, space):
		self.real_widget.setVerticalSpacing(space)
	def _getVerticalSpacing(self):
		return self.real_widget.getVerticalSpacing()
	vspacing = property(_getVerticalSpacing, _setVerticalSpacing)

	def _setHorizontalSpacing(self, space):
		self.real_widget.setHorizontalSpacing(space)
	def _getHorizontalSpacing(self):
		return self.real_widget.getHorizontalSpacing()
	hspacing = property(_getHorizontalSpacing, _setHorizontalSpacing)

	def _setSpacing(self, space):
		# Shorthand property
		if isinstance(space, tuple):
			self.vspacing = space[0]
			self.hspacing = space[1]
		else:
			self.vspacing = space
			self.hspacing = space
	def _getSpacing(self):
		return (self.vspacing, self.hspacing)
	spacing = property(_getSpacing, _setSpacing)

	def _setLayout(self, layout):
		if layout == 'vertical' or layout == 'Vertical' or layout == 'VERTICAL':
			self.real_widget.setLayout(fifechan.Container.Vertical)
		elif layout == 'horizontal' or layout == 'Horizontal' or layout == 'HORIZONTAL':
			self.real_widget.setLayout(fifechan.Container.Horizontal)
		elif layout == 'circular' or layout == 'Circular' or layout == 'CIRCULAR':
			self.real_widget.setLayout(fifechan.Container.Circular)
		else:
			self.real_widget.setLayout(fifechan.Container.Absolute)
	def _getLayout(self):
		if self.real_widget.getLayout() == fifechan.Container.Circular:
			return 'Circular'
		elif self.real_widget.getLayout() == fifechan.Container.Horizontal:
			return 'Horizontal'
		elif self.real_widget.getLayout() == fifechan.Container.Vertical:
			return 'Vertical'
		return 'Absolute'
	layout = property(_getLayout, _setLayout)

	def _setUniformSize(self, uniform): self.real_widget.setUniformSize(uniform)
	def _getUniformSize(self): return self.real_widget.isUniformSize()
	uniform_size = property(_getUniformSize, _setUniformSize)
	
	def _cloneChildren(self, prefix):
		"""
		Clones each child and return the clones in a list.
		"""
		cloneList = [ child.clone(prefix) for child in self.children ]
		
		return cloneList

class VBox(Container):
	"""
	A vertically aligned box - for containement of child widgets.

	Widgets added to this container widget, will layout on top of each other.
	Also the minimal width of the container will be the maximum of the minimal
	widths of the contained widgets.

	The default alignment is to the top. This can be changed by adding a Spacer
	to the widget at any point (but only one!). The spacer will expand, so that
	widgets above the spacer are aligned to the top, while widgets below the spacer
	are aligned to the bottom.
	"""
	DEFAULT_LAYOUT = 'Vertical'
	DEFAULT_HEXPAND = False
	DEFAULT_VEXPAND = True

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
				 _real_widget = None):
				 
		super(VBox,self).__init__(parent=parent, 
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
								  _real_widget=_real_widget)
	def clone(self, prefix):
		vboxClone = VBox(None, 
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
					None)
					
		vboxClone.addChildren(self._cloneChildren(prefix))
					
		return vboxClone

class HBox(Container):
	"""
	A horizontally aligned box - for containement of child widgets.

	Please see L{VBox} for details - just change the directions :-).
	"""
	DEFAULT_LAYOUT = 'Horizontal'
	DEFAULT_HEXPAND = True
	DEFAULT_VEXPAND = False

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
				 _real_widget = None):
				 
		super(HBox,self).__init__(parent=parent,
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
								  _real_widget=_real_widget)

	def clone(self, prefix):
		hboxClone = HBox(None,
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
					None)
					
		hboxClone.addChildren(self._cloneChildren(prefix))
		
		return hboxClone
		
class CBox(Container):
	"""
	A circular box - for containement of child widgets.

	Please see L{VBox} for details - just change the directions :-).
	"""
	DEFAULT_LAYOUT = 'Circular'
	DEFAULT_HEXPAND = True
	DEFAULT_VEXPAND = True

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
				 _real_widget = None):
				 
		super(CBox,self).__init__(parent=parent, 
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
								  _real_widget=_real_widget)

	def clone(self, prefix):
		cboxClone = CBox(None,
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
					None)
					
		cboxClone.addChildren(self._cloneChildren(prefix))
		
		return cboxClone


class Window(Container):
	"""
	A L{VBox} with a draggable title bar aka a window

	New Attributes
	==============

	  - title: The Caption of the window
	  - titlebar_height: The height of the window title bar
	  - movable: Can the Window be moved with the mouse
	"""

	ATTRIBUTES = Container.ATTRIBUTES + [ UnicodeAttr('title'), 
										  IntAttr('titlebar_height'),
										  BoolAttr('movable')
										]
	DEFAULT_LAYOUT = 'Vertical'
	DEFAULT_TITLE = u"title"
	DEFAULT_TITLE_HEIGHT = 16
	DEFAULT_MOVABLE = True
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
				 layout = None,
				 spacing = None,
				 uniform_size = None,
				 _real_widget = None,
				 title = None,
				 titlebar_height = None,
				 movable = None):

		if _real_widget is None: _real_widget = fifechan.Window()
		
		super(Window,self).__init__(parent=parent,
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
									_real_widget=_real_widget)

		if titlebar_height is not None:
			if titlebar_height == 0:
				titlebar_height = self.real_font.getHeight() + 4
			self.titlebar_height = titlebar_height
		else:
			self.titlebar_height = self.real_font.getHeight() + 4
		
		if title is not None: 
			self.title = title
		else:
			self.title = self.DEFAULT_TITLE

		if movable is not None: 
			self.movable = movable
		else:
			self.movable = self.DEFAULT_MOVABLE

	def clone(self, prefix):
		
		windowClone = Window(None, 
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
					self.movable)
		
		windowClone.addChildren(self._cloneChildren(prefix))
				     
		return windowClone
			
	def _getTitle(self): return gui2text(self.real_widget.getCaption())
	def _setTitle(self,text): self.real_widget.setCaption(text2gui(text))
	title = property(_getTitle,_setTitle)

	def _getTitleBarHeight(self): return self.real_widget.getTitleBarHeight()
	def _setTitleBarHeight(self,h): self.real_widget.setTitleBarHeight(h)
	titlebar_height = property(_getTitleBarHeight,_setTitleBarHeight)

	def _getMovable(self): return self.real_widget.isMovable()
	def _setMovable(self, move): self.real_widget.setMovable(move)
	movable = property(_getMovable, _setMovable)
	
	# Hackish way of hiding that title bar height in the perceived height.
	# Fixes VBox calculation
	#def _setHeight(self,h):
	#	h = max(self.min_size[1],h)
	#	h = min(self.max_size[1],h)
	#	self.real_widget.setHeight(h + self.titlebar_height)
	#def _getHeight(self): return self.real_widget.getHeight() - self.titlebar_height
	#height = property(_getHeight,_setHeight)

