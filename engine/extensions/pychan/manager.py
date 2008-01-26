# coding: utf-8

import fife
import widgets
import fonts
from exceptions import *

class Manager(fife.IWidgetListener):
	manager = None

	def __init__(self, engine, debug = False):
		super(Manager,self).__init__()
		self.engine = engine
		self.debug = debug

		if not self.engine.getEventManager():
			raise InitializationError("No event manager installed.")
		if not self.engine.getGuiManager():
			raise InitializationError("No GUI manager installed.")
		
		self.guimanager = engine.getGuiManager()
		self.fonts = {}
		#glyphs = ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"'
		self.fonts['default'] = self.engine.getDefaultFont()
		
		self.styles = {}
		self.styles['default'] = DEFAULT_STYLE
		
		self.widgetEvents = {}
		self.engine.getEventManager().addWidgetListener(self)
                Manager.manager = self

	def show(self,widget):
		"""
		Shows a widget on screen. Used by L{Widget.show} - do not use directly.
		"""
		if widget.position_technique == "automatic":
			w,h = self.engine.getSettings().getScreenWidth(), self.engine.getSettings().getScreenHeight()
			widget.position = (w-widget.width)/2,(h-widget.height)/2
		self.guimanager.add( widget.real_widget )

	def hide(self,widget):
		"""
		Hides a widget again. Used by L{Widget.hide} - do not use directly.
		"""
		self.guimanager.remove( widget.real_widget )

	def getFont(self,name):
		"""
		Returns a GuiFont identified by its name.
		
		@param name: A string identifier from the font definitions in pychans config files.
		"""
		font = self.fonts.get(name)
		return getattr(font,"font",font)

	def addFont(self,font):
		"""
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
		
		for k,v in self.styles['default'].items():
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
		return fife.GuiImage(self.engine.imagePool.addResourceFromFile(filename),self.engine.imagePool)

	def defaultWidgetAction(self,event):
		if self.debug:
			print "Event(%s) received." % event.getId()

	def onWidgetAction(self, event):
		#print event.getId(),self.widgetEvents
		handler = self.widgetEvents.get( event.getId(), self.defaultWidgetAction )
		try:
			handler( event )
		except Exception, e:
			print "*** Error in event callback for ",event.getId()
			import traceback
			traceback.print_exc()

# Default Widget style.

DEFAULT_STYLE = {
	'default' : {
		'border_size': 1,
		'margins': (0,0),
		'base_color' : fife.Color(0,0,100),
		'foreground_color' : fife.Color(255,255,255),
		'background_color' : fife.Color(0,0,0),
	},
	widgets.Button : {
		'border_size': 0,
		'margins' : (10,5),
	},
	widgets.CheckBox : {
		'border_size': 0,
	},
	widgets.Label : {
		'border_size': 0,
	},
	widgets.ClickLabel : {
		'border_size': 0,
	},
	widgets.ListBox : {
		'border_size': 0,
	},
	widgets.Window : {
		'border_size': 1,
		'margins': (5,5),
	},
	(widgets.Container,widgets.HBox,widgets.VBox) : {
		'border_size': 0,
		'opaque' : False
	}
}