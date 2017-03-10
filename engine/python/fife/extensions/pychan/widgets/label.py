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

from fife import fifechan

from fife.extensions.pychan.attrs import BoolAttr

from basictextwidget import BasicTextWidget


class Label(BasicTextWidget):
	"""
	A basic label - displaying a string.

	Also allows text wrapping and onMouse hover callbacks.

	New Attributes
	==============

	 - wrap_text: Boolean: Enable/Disable automatic text wrapping. Disabled by default.
	 Currently to actually see text wrapping you have to explicitly set a max_size with
	 the desired width of the text, as the layout engine is not capable of deriving
	 the maximum width from a parent container.

	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [ BoolAttr('wrap_text') ]

	DEFAULT_WRAP_TEXT = False

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
				 text = None,
				 wrap_text = None):
				 
		self.real_widget = fifechan.Label("")
		self.wrap_text = self.DEFAULT_WRAP_TEXT
		super(Label,self).__init__(parent=parent, 
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
								   text=text)
								   
		if wrap_text is not None:
			self.wrap_text = wrap_text
		
	def clone(self, prefix):
		lblClone = Label(None,
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
						 self.text,
						 self.wrap_text)
		
		return lblClone;

	def _setTextWrapping(self,wrapping): self.real_widget.setTextWrapping(wrapping)
	def _getTextWrapping(self): self.real_widget.isTextWrapping()
	wrap_text = property(_getTextWrapping,_setTextWrapping)
