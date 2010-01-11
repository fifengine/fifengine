# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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

from common import *
from widget import Widget
from layout import VBoxLayoutMixin, HBoxLayoutMixin

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

	ATTRIBUTES = Widget.ATTRIBUTES + [ IntAttr('padding'), Attr('background_image'), BoolAttr('opaque'),PointAttr('margins') ]

	def __init__(self,padding=5,margins=(5,5),_real_widget=None, **kwargs):
		self.real_widget = _real_widget or fife.Container()
		self.children = []
		self.margins = margins
		self.padding = padding
		self._background = []
		self._background_image = None
		super(Container,self).__init__(**kwargs)

	def addChild(self, widget):
		widget.parent = self
		self.children.append(widget)
		self.real_widget.add(widget.real_widget)

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
		self.removeAllChildren()
		for child in children:
			self.addChild(child)

	def insertChildBefore(self, widget, before):
		if before not in self.children:
			raise RuntimeError("Couldn't find widget %s as child of %s - in insertChildBefore" % (str(widget),str(before)))
		self.insertChild(widget, self.children.index(before))

	def removeChild(self,widget):
		if not widget in self.children:
			raise RuntimeError("%s does not have %s as direct child widget." % (str(self),str(widget)))
		self.children.remove(widget)
		self.real_widget.remove(widget.real_widget)
		widget.parent = None

	def add(self,*widgets):
		print "PyChan: Deprecation warning: Please use 'addChild' or 'addChildren' instead."
		self.addChildren(*widgets)

	def getMaxChildrenWidth(self):
		if not self.children: return 0
		return max(widget.width for widget in self.children)

	def getMaxChildrenHeight(self):
		if not self.children: return 0
		return max(widget.height for widget in self.children)

	def deepApply(self,visitorFunc, leaves_first = True):
		if leaves_first:
			for child in self.children:
				child.deepApply(visitorFunc, leaves_first = leaves_first)
		visitorFunc(self)
		if not leaves_first:
			for child in self.children:
				child.deepApply(visitorFunc, leaves_first = leaves_first)

	def beforeShow(self):
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
		icon = fife.Icon(image)
		x, w = 0, image_w
		while x < back_w:
			y, h = 0, image_h
			while y < self.height:
				icon = fife.Icon(image)
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
			self._background_image = image
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

	def __init__(self,padding=5,**kwargs):
		super(VBox,self).__init__(**kwargs)
		self.padding = padding


class HBox(HBoxLayoutMixin,Container):
	"""
	A horizontally aligned box - for containement of child widgets.

	Please see L{VBox} for details - just change the directions :-).
	"""
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 0

	def __init__(self,padding=5,**kwargs):
		super(HBox,self).__init__(**kwargs)
		self.padding = padding

class Window(VBoxLayoutMixin,Container):
	"""
	A L{VBox} with a draggable title bar aka a window

	New Attributes
	==============

	  - title: The Caption of the window
	  - titlebar_height: The height of the window title bar
	"""

	ATTRIBUTES = Container.ATTRIBUTES + [ UnicodeAttr('title'), IntAttr('titlebar_height') ]

	def __init__(self,title=u"title",titlebar_height=0,**kwargs):
		super(Window,self).__init__(_real_widget = fife.Window(), **kwargs)
		if titlebar_height == 0:
			titlebar_height = self.real_font.getHeight() + 4
		self.titlebar_height = titlebar_height
		self.title = title

		# Override explicit positioning
		self.position_technique = "automatic"


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

