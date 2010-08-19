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

class ScrollArea(Widget):
	"""
	A wrapper around another (content) widget.

	New Attributes
	==============

	  - content: The wrapped widget.
	  - vertical_scrollbar: Boolean: Set this to False to hide the Vertical scrollbar
	  - horizontal_scrollbar: Boolean: Set this to False to hide the Horizontal scrollbar

	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [ BoolAttr("vertical_scrollbar"),BoolAttr("horizontal_scrollbar") ]
	DEFAULT_HEXPAND = 1
	DEFAULT_VEXPAND = 1

	def __init__(self,**kwargs):
		self.real_widget = fife.ScrollArea()
		self._content = None
		super(ScrollArea,self).__init__(**kwargs)

	def addChild(self,widget):
		self.content = widget
		widget.parent = self

	def removeChild(self,widget):
		if self._content != widget:
			raise RuntimeError("%s does not have %s as direct child widget." % (str(self),str(widget)))
		self.content = None
		widget.parent = None

	def _setContent(self,content):
		if content is None:
			self.real_widget.setContent(content)
		else:
			self.real_widget.setContent(content.real_widget)
		self._content = content
	def _getContent(self): return self._content
	content = property(_getContent,_setContent)

	def deepApply(self,visitorFunc, leaves_first = True):
		if leaves_first:
			if self._content: self._content.deepApply(visitorFunc, leaves_first = leaves_first)
		visitorFunc(self)
		if not leaves_first:
			if self._content: self._content.deepApply(visitorFunc, leaves_first = leaves_first)

	def resizeToContent(self,recurse=True):
		if self._content is None: return
		if recurse:
			self.content.resizeToContent(recurse=recurse)
		self.size = self.min_size

	def _visibilityToScrollPolicy(self,visibility):
		if visibility:
			return fife.ScrollArea.SHOW_AUTO
		return fife.ScrollArea.SHOW_NEVER

	def _scrollPolicyToVisibility(self,policy):
		if policy == fife.ScrollArea.SHOW_NEVER:
			return False
		return True

	def _setHorizontalScrollbar(self,visibility):
		self.real_widget.setHorizontalScrollPolicy( self._visibilityToScrollPolicy(visibility) )

	def _setVerticalScrollbar(self,visibility):
		self.real_widget.setVerticalScrollPolicy( self._visibilityToScrollPolicy(visibility) )

	def _getHorizontalScrollbar(self):
		return self._scrollPolicyToVisibility( self.real_widget.getHorizontalScrollPolicy() )

	def _getVerticalScrollbar(self):
		return self._scrollPolicyToVisibility( self.real_widget.getVerticalScrollPolicy() )
		
	def sizeChanged(self):
		if self.content:
			self.content.width = max(self.content.width,self.width-5)
			self.content.height = max(self.content.height,self.height-5)

	def getVerticalMaxScroll(self):
		return self.real_widget.getVerticalMaxScroll()
	def getHorizontalMaxScroll(self):
		return self.real_widget.getHorizontalMaxScroll()
	def _getHorizontalScrollAmount(self):
		return self.real_widget.getHorizontalScrollAmount()
	def _setHorizontalScrollAmount(self, scroll_amount):
		return self.real_widget.setHorizontalScrollAmount(scroll_amount)
	def _getVerticalScrollAmount(self):
		return self.real_widget.getVerticalScrollAmount()
	def _setVerticalScrollAmount(self, scroll_amount):
		return self.real_widget.setVerticalScrollAmount(scroll_amount)

	vertical_scrollbar = property(_getVerticalScrollbar,_setVerticalScrollbar)
	horizontal_scrollbar = property(_getHorizontalScrollbar,_setHorizontalScrollbar)

	horizontal_scroll_amount = property(_getHorizontalScrollAmount, _setHorizontalScrollAmount)
	vertical_scroll_amount = property(_getVerticalScrollAmount, _setVerticalScrollAmount)	
