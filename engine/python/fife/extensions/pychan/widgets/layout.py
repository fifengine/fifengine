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

from fife.extensions.pychan.attrs import IntAttr

AlignTop, AlignBottom, AlignLeft, AlignRight, AlignCenter = range(5)
def isLayouted(widget):
	return isinstance(widget,LayoutBase)

class LayoutBase(object):
	"""
	This class is at the core of the layout engine. The two MixIn classes L{VBoxLayoutMixin}
	and L{HBoxLayoutMixin} specialise on this by reimplementing the C{resizeToContent} and
	the C{expandContent} methods.

	Dynamic Layouting
	-----------------

	The layout is calculated in the L{Widget.show} method. Thus if you modify the layout,
	by adding or removing child widgets for example, you have to call L{widgets.Widget.adaptLayout}
	so that the changes ripple through the widget hierachy.

	Internals
	---------

	At the core the layout engine works in two passes:

	Before a root widget loaded by the XML code is shown, its resizeToContent method
	is called recursively (walking the widget containment relation in post order).
	This shrinks all HBoxes and VBoxes to their minimum heigt and width.
	After that the expandContent method is called recursively in the same order,
	which will re-align the widgets if there is space left AND if a Spacer is contained.

	Inside bare Container instances (without a Layout MixIn) absolute positioning
	can be used.
	"""
	def __init__(self,align = (AlignLeft,AlignTop), **kwargs):
		self.align = align
		self.spacer = []
		super(LayoutBase,self).__init__(**kwargs)

	def addSpacer(self,spacer):
		self.spacer.append(spacer)
		spacer.index = len(self.children)

	def xdelta(self,widget):return 0
	def ydelta(self,widget):return 0

	def _applyHeight(self, spacers = []):
		y = self.border_size + self.margins[1]
		ydelta = map(self.ydelta,self.children)
		for index, child in enumerate(self.children):
			while spacers and spacers[0].index == index:
				y += spacers.pop(0).size
			child.y = y
			y += ydelta.pop(0)

	def _adjustHeightWithSpacer(self):
		pass

	def _applyWidth(self, spacers = []):
		x = self.border_size + self.margins[0]
		xdelta = map(self.xdelta,self.children)
		for index, child in enumerate(self.children):
			while spacers and spacers[0].index == index:
				x += spacers.pop(0).size
			child.x = x
			x += xdelta.pop(0)

	def _expandWidthSpacer(self):
		xdelta = map(self.xdelta,self.children)
		xdelta += [spacer.min_size for spacer in self.spacer]

		available_space = self.width - 2*self.margins[0] - 2*self.border_size - self._extra_border[0]

		used_space = sum(xdelta)
		if self.children:
			used_space -= self.padding
		if used_space >= available_space:
			return

		expandable_items = self._getExpanders(vertical=False)
		#print "AS/US - before",self,[o.width for o in expandable_items]
		#print "SPACERS",self.spacer

		index = 0
		n = len(expandable_items)
		while used_space < available_space and expandable_items:
			index = index % n
			delta = (available_space - used_space) / n
			if delta == 0:
				delta = 1

			expander = expandable_items[index]
			old_width = expander.width
			expander.width += delta
			delta = expander.width - old_width
			if delta == 0:
				expandable_items.pop(index)
				n -= 1
			else:
				used_space += delta
				index += 1
		#print "AS/US - after",self,[o.width for o in expandable_items]
		#print "SPACERS",self.spacer
		self._applyWidth(spacers = self.spacer[:])

	def _expandHeightSpacer(self):
		ydelta = map(self.ydelta,self.children)
		ydelta += [spacer.min_size for spacer in self.spacer]

		available_space = self.height - 2*self.margins[1] - 2*self.border_size - self._extra_border[1]

		used_space = sum(ydelta)
		if self.children:
			used_space -= self.padding

		if used_space >= available_space:
			return

		expandable_items = self._getExpanders(vertical=True)
		#print "AS/US - before",self,[o.height for o in expandable_items]

		index = 0
		n = len(expandable_items)
		while used_space < available_space and expandable_items:
			index = index % n
			delta = (available_space - used_space) / n
			if delta == 0:
				delta = 1

			expander = expandable_items[index]
			old_height = expander.height
			expander.height += delta
			delta = expander.height - old_height
			if delta == 0:
				expandable_items.pop(index)
				n -= 1
			else:
				used_space += delta
				index += 1

		#print "AS/US - after",self,[o.height for o in expandable_items]
		self._applyHeight(spacers = self.spacer[:])


	def _getExpanders(self,vertical=True):
		expanders = []
		spacers = self.spacer[:]
		for index, child in enumerate(self.children):
			if spacers and spacers[0].index == index:
				expanders.append( spacers.pop(0) )
			if child.vexpand and vertical:
				expanders += [child]*child.vexpand
			if child.hexpand and not vertical:
				expanders += [child]*child.hexpand
		return expanders + spacers

	def _resetSpacers(self):
		for spacer in self.spacer:
			spacer.size = 0

class VBoxLayoutMixin(LayoutBase):
	"""
	A mixin class for a vertical layout. Do not use directly.
	"""
	def __init__(self,**kwargs):
		super(VBoxLayoutMixin,self).__init__(**kwargs)

	def resizeToContent(self, recurse = True):
		self._resetSpacers()

		max_w = self.getMaxChildrenWidth()
		x = self.margins[0] + self.border_size
		y = self.margins[1] + self.border_size
		for widget in self.children:
			widget.width = max_w
			y += widget.height + self.padding

		if self.children:
			y -= self.padding

		y += sum([spacer.min_size for spacer in self.spacer])

		self.height = y + self.margins[1] + self.border_size + self._extra_border[1]
		self.width = max_w + 2*x + self._extra_border[0]

		self._applyHeight(spacers = self.spacer[:])
		self._applyWidth()

	def expandContent(self):
		self._expandHeightSpacer()
		if not self.hexpand and self.parent:return
		for widget in self.children:
			widget.width = self.width - 2*self.margins[0] - 2*self.border_size - self._extra_border[0]


	def ydelta(self,widget):return widget.height + self.padding

class HBoxLayoutMixin(LayoutBase):
	"""
	A mixin class for a horizontal layout. Do not use directly.
	"""
	def __init__(self,**kwargs):
		super(HBoxLayoutMixin,self).__init__(**kwargs)

	def resizeToContent(self, recurse = True):
		self._resetSpacers()

		max_h = self.getMaxChildrenHeight()
		x = self.margins[0] + self.border_size
		y = self.margins[1] + self.border_size
		for widget in self.children:
			widget.height = max_h
			x += widget.width + self.padding
		if self.children:
			x -= self.padding
		x += sum([spacer.min_size for spacer in self.spacer])

		self.width = x + self.margins[0] + self._extra_border[0]
		self.height = max_h + 2*y + self._extra_border[1]

		self._applyHeight()
		self._applyWidth(spacers = self.spacer[:])

	def expandContent(self):
		self._expandWidthSpacer()
		if not self.vexpand and self.parent:return
		for widget in self.children:
			widget.height = self.height - 2*self.margins[1] - 2*self.border_size - self._extra_border[1]

	def xdelta(self,widget):return widget.width + self.padding

class Spacer(object):
	""" A spacer represents expandable or fixed 'whitespace' in the GUI.

	In a XML file you can get this by adding a <Spacer /> inside a VBox or
	HBox element (Windows implicitly are VBox elements).

	Attributes
	----------

	As with widgets a number of attributes can be set on a spacer (inside the XML definition).
	
	  - min_size: Int: The minimal size this Spacer is allowed to have.
	  - max_size: Int: The maximal size this Spacer is allowed to have.
	  - fixed_size: Int: Set min_size and max_size to the same vale - effectively a Fixed size spacer. 

	"""

	ATTRIBUTES = [
		IntAttr('min_size'), IntAttr('size'), IntAttr('max_size'),
		IntAttr('fixed_size'),
	]

	def __init__(self,parent=None,**kwargs):
		self.parent = parent
		self.min_size = 0
		self.max_size = 1000
		self.size = 0

	def __str__(self):
		return "Spacer(parent.name='%s')" % getattr(self.parent,'name','None')

	def __repr__(self):
		return "<Spacer(parent.name='%s') at %x>" % (getattr(self.parent,'name','None'),id(self))

	def _getSize(self):
		self.size = self._size
		return self._size
	def _setSize(self,size):
		self._size = max(self.min_size, min(self.max_size,size))
	size = property(_getSize,_setSize)

	# Alias for size
	width = property(_getSize,_setSize)
	height = property(_getSize,_setSize)

	def _setFixedSize(self,size):
		self.min_size = self.max_size = size
		self.size = size
	fixed_size = property(fset=_setFixedSize)

	def _isExpanding(self):
		if self.min_size < self.max_size:
			return 1
		return 0
	vexpand = property(_isExpanding)
	hexpand = property(_isExpanding)
