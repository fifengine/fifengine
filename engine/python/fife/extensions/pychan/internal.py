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

from compat import guichan, in_fife
import widgets
from fife.extensions import fife_timer as timer
import fonts
from exceptions import *
from traceback import print_exc

def get_manager():
	"""
	Get the manager from inside pychan.

	To avoid cyclic imports write::
	   from internal import get_manager
	"""
	return Manager.manager

def screen_width():
	return get_manager().hook.screen_width

def screen_height():
	return get_manager().hook.screen_height

class Manager(object):
	manager = None

	def __init__(self, hook, debug = False, compat_layout = False):
		super(Manager,self).__init__()
		self.hook = hook
		self.debug = debug
		self.compat_layout = compat_layout
		self.unicodePolicy = ('ignore',)

		if in_fife:
			if not hook.engine.getEventManager():
				raise InitializationError("No event manager installed.")
			if not hook.engine.getGuiManager():
				raise InitializationError("No GUI manager installed.")
		timer.init(hook.engine.getTimeManager())

		self.fonts = {}
		#glyphs = ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"'
		self.fonts['default'] = hook.default_font

		self.styles = {}
		self.addStyle('default',DEFAULT_STYLE)

		Manager.manager = self

		# Setup synchronous dialogs
		self.mainLoop = None
		self.breakFromMainLoop = None
		self.can_execute = False

		import weakref
		self.allWidgets = weakref.WeakKeyDictionary()

		# Autopos
		from autoposition import placeWidget
		self.placeWidget = placeWidget

	def setupModalExecution(self,mainLoop,breakFromMainLoop):
		"""
		Setup synchronous execution of dialogs.
		"""
		self.mainLoop = mainLoop
		self.breakFromMainLoop = breakFromMainLoop
		self.can_execute = True

	def show(self,widget):
		"""
		Shows a widget on screen. Used by L{Widget.show} - do not use directly.
		"""
		self.placeWidget(widget, widget.position_technique)
		assert widget not in self.allWidgets
		self.allWidgets[ widget ] = 1
		self.hook.add_widget( widget.real_widget )

	def hide(self,widget):
		"""
		Hides a widget again. Used by L{Widget.hide} - do not use directly.
		"""
		self.hook.remove_widget( widget.real_widget )
		del self.allWidgets[ widget ]

	def setDefaultFont(self,name):
		self.fonts['default'] = self.getFont(name)

	def getFont(self,name):
		"""
		B{pending deprecation}

		Returns a GuiFont identified by its name.

		@param name: A string identifier from the font definitions in pychans config files.
		"""
		if in_fife:
			font = self.fonts.get(name)
			if isinstance(font,guichan.GuiFont):
				return font
			if hasattr(font,"font") and isinstance(getattr(font,"font"),guichan.GuiFont):
				return font.font
			raise InitializationError("Couldn't find the font '%s' - did you forget loading a .fontdef?" % str(name))
		else:
			return self.hook.get_font(name)

	def addFont(self,font):
		"""
		B{deprecated}

		Add a font to the font registry. It's not necessary to call this directly.
		But it expects a L{Font} instance and throws an L{InitializationError}
		otherwise.

		@param font: A L{Font} instance.
		"""
		if not isinstance(font,fonts.Font):
			raise InitializationError("PyChan Manager expected a fonts.Font instance, not %s." % repr(font))
		self.fonts[font.name] = font

	def addStyle(self,name,style):
		style = self._remapStyleKeys(style)

		for k,v in self.styles.get('default',{}).items():
			style[k] = style.get(k,v)
		self.styles[name] = style

	def stylize(self,widget, style, **kwargs):
		style = self.styles[style]
		for k,v in style.get('default',{}).items():
			v = kwargs.get(k,v)
			setattr(widget,k,v)

		cls = widget.__class__
		for applicable,specstyle in style.items():
			if not isinstance(applicable,tuple):
				applicable = (applicable,)
			if cls in applicable:
				for k,v in specstyle.items():
					v = kwargs.get(k,v)
					setattr(widget,k,v)

	def _remapStyleKeys(self,style):
		"""
		Translate style selectors to tuples of widget classes. (internal)
		"""
		# Remap class names, create copy:
		def _toClass(class_):
			if class_ == "default":
				return class_

			if type(class_) == type(widgets.Widget) and issubclass(class_,widgets.Widget):
				return class_
			if not widgets.WIDGETS.has_key(str(class_)):
				raise InitializationError("Can't resolve %s to a widget class." % repr(class_))
			return widgets.WIDGETS[str(class_)]

		style_copy = {}
		for k,v in style.items():
			if isinstance(k,tuple):
				new_k = tuple(map(_toClass,k))
			else:
				new_k = _toClass(k)
			style_copy[new_k] = v
		return style_copy

	def loadImage(self,filename):
		if not filename:
			  raise InitializationError("Empty Image file.")
		return self.hook.load_image(filename)

# Default Widget style.

DEFAULT_STYLE = {
	'default' : {
		'border_size': 0,
		'margins': (0,0),
		'base_color' : guichan.Color(28,28,28),
		'foreground_color' : guichan.Color(255,255,255),
		'background_color' : guichan.Color(50,50,50),
		'selection_color' : guichan.Color(80,80,80),
		'font' : 'default'
	},
	'Button' : {
		'border_size': 2,
		'margins' : (5,2),
		'min_size' : (15,10),
	},
	'CheckBox' : {
		'border_size': 0,
	},
	'RadioButton' : {
		'border_size': 0,
		'background_color' : guichan.Color(0,0,0),
	},
	'Label' : {
		'border_size': 0,
		'background_color' : guichan.Color(50,50,50,0)
	},
	'ClickLabel' : {
		'border_size': 0,
	},
	'ListBox' : {
		'border_size': 0,
	},
	'Window' : {
		'border_size': 0,
		'margins': (5,5),
		'opaque' : 1,
		'padding':2,
		'titlebar_height' : 12,
		'background_image' : None,
	},
	'TextBox' : {
	},
	('Container','HBox','VBox') : {
		'border_size': 0,
		'margins': (0,0),
		'padding':2,
		'opaque' : 1,
		'background_image' : None,
	}
}