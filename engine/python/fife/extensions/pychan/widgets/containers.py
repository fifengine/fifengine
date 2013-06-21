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
from layout import VBoxLayoutMixin, HBoxLayoutMixin, Spacer
from widget import Widget


class Container(Widget):
	"""
	This is the basic container class. It provides space in which child widgets can
	be position via the position attribute. If you want to use the layout engine,
	you have to use derived containers with vertical or horizontal orientation
	(L{VBox} or L{HBox})

	New Attributes
	==============

	  - padding - Integer: Not used in the Container class istelf, distance between child widgets.
	  - background_image - Set this to a GuiImage or a resource location (simply a filename).
	    The image will be tiled over the background area.
	  - opaque - Boolean: Whether the background should be drawn at all. Set this to False
	    to make the widget transparent.
	  - children - Just contains the list of contained child widgets. Do NOT modify.
	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [ IntAttr('padding'), 
									   Attr('background_image'), 
									   BoolAttr('opaque'),
									   PointAttr('margins') 
									 ]

	DEFAULT_OPAQUE = True
	DEFAULT_MARGINS = 5,5
	DEFAULT_PADDING = 5
	DEFAULT_BACKGROUND = None
	
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
				 _real_widget = None):
				 
		self.real_widget = _real_widget or fifechan.Container()
		self.children = []
		self.children_position_cache = []
		self.hidden_children = []
		self._background = []
		self._background_image = None
		self.background_image = self.DEFAULT_BACKGROUND
		self.margins = self.DEFAULT_MARGINS
		self.padding = self.DEFAULT_PADDING
		self.opaque = self.DEFAULT_OPAQUE

		super(Container,self).__init__(parent=parent, 
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
									   
		if margins is not None: self.margins = margins
		if padding is not None: self.padding = padding
		if opaque is not None: self.opaque = opaque
		if background_image is not None: self.background_image = background_image
		
	def clone(self, prefix):
		containerClone = Container(None, 
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
		
		if isinstance(widget, Spacer):
			self.addSpacer(widget)
			return

		widget.parent = self
		widget._visible = self._visible
		self.children.append(widget)
		self.children_position_cache.append(widget)
		self.real_widget.add(widget.real_widget)
		
		#update the states of the child widgets.  This does not actually call
		#the show() or hide() functions of the widget.
		if self._visible:
			def _show(shown_widget):
				shown_widget._visible = True
				
			self.deepApply(_show, shown_only=True)
		else:
			def _hide(hidden_widget):
				hidden_widget._visible = False
				
			self.deepApply(_hide)
		
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
		if not widget in self.children and not widget in self.hidden_children:
			raise RuntimeError("%s does not have %s as direct child widget." % (str(self),str(widget)))

		if widget in self.children:
			self.children.remove(widget)
			self.real_widget.remove(widget.real_widget)

		if widget in self.children_position_cache:
			self.children_position_cache.remove(widget)

		if widget in self.hidden_children:
			self.hidden_children.remove(widget)

		widget.parent = None

	def hideChild(self, child):
		if not child in self.children:
			raise RuntimeError("%s does not have %s as direct child widget." % (str(self), str(child)))
		
		self.hidden_children.append(child)
		self.children.remove(child)

		self.real_widget.remove(child.real_widget)
		child._visible = False
		
	def showChild(self, child):
		if not child in self.hidden_children:
			return
	
		self.hidden_children.remove(child)
		
		children = self.children[:]
		children_position_cache = self.children_position_cache[:]
		hidden_children = self.hidden_children[:]

		for widget in children:
			self.removeChild(widget)
		
		for child_widget in children_position_cache:
			if not child_widget in hidden_children:
				self.addChild(child_widget)
		
		self.children_position_cache = children_position_cache[:]
		self.hidden_children = hidden_children[:]
			
	def add(self,*widgets):
		print "PyChan: Deprecation warning: Please use 'addChild' or 'addChildren' instead."
		self.addChildren(*widgets)

	def getMaxChildrenWidth(self):
		if not self.children: return 0
		return max(widget.width for widget in self.children)

	def getMaxChildrenHeight(self):
		if not self.children: return 0
		return max(widget.height for widget in self.children)

	def deepApply(self,visitorFunc, leaves_first = True, shown_only = False):
		if not shown_only:
			children = self.children + self.hidden_children
		else:
			children = self.children
		
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
		image_w, image_h = image.getWidth(), image.getHeight()

		map(self.real_widget.remove,self._background)

		# Now tile the background over the widget
		self._background = []
		icon = fifechan.Icon(image)
		x, w = 0, image_w
		while x < back_w:
			y, h = 0, image_h
			while y < self.height:
				icon = fifechan.Icon(image)
				icon.setPosition(x,y)
				self._background.append(icon)
				y += h
			x += w
		map(self.real_widget.add,self._background)
		for tile in self._background:
			tile.requestMoveToBottom()

	def setBackgroundImage(self,image):
		self._background = getattr(self,'_background',None)
		if image is None:
			self._background_image = None
			map(self.real_widget.remove,self._background)
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
	
	def _cloneChildren(self, prefix):
		"""
		Clones each child and return the clones in a list.
		"""
		cloneList = [ child.clone(prefix) for child in self.children ]
		
		return cloneList

class VBox(VBoxLayoutMixin,Container):
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
	DEFAULT_HEXPAND = 0
	DEFAULT_VEXPAND = 1

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
				 _real_widget = None):
				 
		super(VBox,self).__init__(parent=parent, 
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
								  _real_widget=_real_widget)
	def clone(self, prefix):
		vboxClone = VBox(None, 
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
					
		vboxClone.addChildren(self._cloneChildren(prefix))
					
		return vboxClone

class HBox(HBoxLayoutMixin,Container):
	"""
	A horizontally aligned box - for containement of child widgets.

	Please see L{VBox} for details - just change the directions :-).
	"""
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 0

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
				 _real_widget = None):
				 
		super(HBox,self).__init__(parent=parent, 
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
								  _real_widget=_real_widget)

	def clone(self, prefix):
		hboxClone = HBox(None,
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
					
		hboxClone.addChildren(self._cloneChildren(prefix))
		
		return hboxClone
		
								  
class Window(VBoxLayoutMixin,Container):
	"""
	A L{VBox} with a draggable title bar aka a window

	New Attributes
	==============

	  - title: The Caption of the window
	  - titlebar_height: The height of the window title bar
	"""

	ATTRIBUTES = Container.ATTRIBUTES + [ UnicodeAttr('title'), 
										  IntAttr('titlebar_height') 
										]

	DEFAULT_TITLE = u"title"
	DEFAULT_TITLE_HEIGHT = 0
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
				 background_image = None,
				 opaque = None,
				 margins = None,
				 _real_widget = None,
				 title = None,
				 titlebar_height = None):
		
		super(Window,self).__init__(parent=parent, 
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
								    _real_widget= fifechan.Window())

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

	def clone(self, prefix):
		
		windowClone = Window(None, 
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
					self.titlebar_height
				    )
				     
		windowClone.addChildren(self._cloneChildren(prefix))
				     
		return windowClone
			
	def _getTitle(self): return gui2text(self.real_widget.getCaption())
	def _setTitle(self,text): self.real_widget.setCaption(text2gui(text))
	title = property(_getTitle,_setTitle)

	def _getTitleBarHeight(self): return self.real_widget.getTitleBarHeight()
	def _setTitleBarHeight(self,h): self.real_widget.setTitleBarHeight(h)
	titlebar_height = property(_getTitleBarHeight,_setTitleBarHeight)

	# Hackish way of hiding that title bar height in the perceived height.
	# Fixes VBox calculation
	def _setHeight(self,h):
		h = max(self.min_size[1],h)
		h = min(self.max_size[1],h)
		self.real_widget.setHeight(h + self.titlebar_height)
	def _getHeight(self): return self.real_widget.getHeight() - self.titlebar_height
	height = property(_getHeight,_setHeight)

