# coding: utf-8

import fife, pythonize

__doc__="""\
Simple Guichan Wrapper
Very unfinished.

Features:
* Simpler Interface
* Automagic background tiling (WIP)
* Very Basic Layout Engine
* Very Basic XML Format support
* Basic Styling support

TODO:
* Completion of above features
* Wrap missing widgets: DropDown, RadioButton, Slider, TextField
* Documentation
* Add support for 'Spacers' in layouts
* Easier Font handling.

BUGS:
* Padding is not applied.
* Fonts are just engine.getDefaultFont()

Problems:
* Reference counting problems again *sigh*
* ... and thus possible leaks.
* Fails sometimes without an error message.
* High amount of code reuse -> Complex code
* Needs at least new style classes and other goodies.
* Documentation!

"""

### Functools ###

def applyOnlySuitable(func,**kwargs):
	if hasattr(func,'im_func'):
		code = func.im_func.func_code
		varnames = code.co_varnames[1:code.co_argcount]#ditch bound instance
	else:
		code = func.func_code
		varnames = code.co_varnames[0:code.co_argcount]

	#http://docs.python.org/lib/inspect-types.html
	if code.co_flags & 8:
		return func(**kwargs)
	for name,value in kwargs.items():
		if name not in varnames:
			del kwargs[name]
	return func(**kwargs)
		

# Text munging befor adding it to TextBoxes

def _mungeText(text):
	return text.replace('\t',"    ")

### Initialisation ###

class InitializationError(Exception):
	pass

def _InitCheckOrFail(check,message):
	if not check: raise InitializationError(message)

class _Manager(fife.IWidgetListener, fife.TimeEvent):
	def __init__(self, engine, debug = False):
		super(_Manager,self).__init__()
		self.engine = engine
		self.debug = debug

		_InitCheckOrFail(self.engine.getEventManager(),"No event manager installed.")
		_InitCheckOrFail(self.engine.getGuiManager(),"No GUI manager installed.")
		
		self.guimanager = engine.getGuiManager()
		self.initFont()
		self.initStyles()
		self.widgetEvents = {}
		self.engine.getEventManager().addWidgetListener(self)

	def show(self,widget):
		self.guimanager.add( widget.real_widget )
	def hide(self,widget):
		self.guimanager.remove( widget.real_widget )

	def initFont(self):
		glyphs = ' abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/:();%`\'*#=[]"'
		self.imagefont = self.engine.getDefaultFont()
		self.font = self.engine.getDefaultFont()

	def initStyles(self):
		self.styles = {}
		self.styles['default'] = {
			'default' : {
				'border_size': 1,
				'base_color' : fife.Color(0,0,100),
				'foreground_color' : fife.Color(255,255,255),
				'background_color' : fife.Color(0,0,0),
				'font' : self.font
			},
			Button : {
				'border_size': 0,
				'margins' : (10,5),
				'base_color' : fife.Color(0,0,100),
				'foreground_color' : fife.Color(0,0,100),
			},
			CheckBox : {
				'border_size': 0,
				'background_color' : fife.Color(0,0,0,0),
			},
			Label : {
				'border_size': 0,
				'base_color' : fife.Color(0,0,100),
			},
			ClickLabel : {
				'border_size': 0,
				'base_color' : fife.Color(0,0,100),
			},
			ListBox : {
				'border_size': 0,
				'base_color' : fife.Color(0,0,100),
			},
			Window : {
				'border_size': 1,
#				'background_image' : 'lambda/gfx/alumox.jpg'
			},
			(Container,HBox,VBox) : {
				'border_size': 0,
				'opaque' : False
			}
		}

	def addStyle(self,name,style):
		for k,v in self.styles['default']:
			style[k] = style.get(k,v)
		self.styles[name] = style

	def stylize(self,widget, style, **kwargs):
		style = self.styles[style]
		for k,v in style.get('default',{}).items():
			v = kwargs.get(k,v)
			setattr(widget,k,v)
		
		cls = widget.__class__
		for applicable,specstyle in style.items():
			if not isinstance(applicable,type(())):
				applicable = (applicable,)
			if cls in applicable:
				for k,v in specstyle.items():
					v = kwargs.get(k,v)
					setattr(widget,k,v)

	def loadImage(self,filename):
		return fife.GuiImage(self.engine.imagePool.addResourceFromFile(filename),self.engine.imagePool)

	def defaultWidgetAction(self,event):
		print "Event(%s) received." % event.getId()

	def onWidgetAction(self, event):
		print event.getId(),self.widgetEvents
		for handler in self.widgetEvents.get( event.getId(), [self.defaultWidgetAction] ):
			try:
				handler( event )
			except Exception, e:
				print "*** Error in event callback for ",event.getId()
				import traceback
				traceback.print_exc()


manager = None
def init(engine,debug=False):
	""" init( FifeEngine, debug = False )
	
	This has to be called before any other pychan methods can be used.
	It sets up a manager object which is available under pychan.manager.
	"""
	global manager
	manager = _Manager(engine,debug)

### Widget/Container Base Classes ###

class _widget(object):
	def __init__(self,parent = None, name = '_unnamed_',
			size = (-1,-1), min_size=(0,0), max_size=(5000,5000),**kwargs):
		
		assert( hasattr(self,'real_widget') )
		self._parent = parent
		self.name = name
		self.min_size = min_size
		self.max_size = max_size
		self.size = size
		self._visible = False
		manager.stylize(self,kwargs.get('style','default'),**kwargs)
		#self.resizeToContent()
	
	def match(self,**kwargs):
		""" Matches the widget against a list of key-value pairs.
		Only if all keys are attributes and their value is the same it returns True.
		"""
		for k,v in kwargs.items():
			if v != getattr(self,k,None):
				return False
		return True

	def capture(self, callback):
		def captured_f(event):
			applyOnlySuitable(callback,event=event,widget=self)
		manager.widgetEvents.setdefault(self._event_id,[]).append(captured_f)

	def show(self):
		self.beforeShow()
		self.adaptLayout()
		manager.show(self)
		self._visible = True

	def hide(self):
		manager.hide(self)
		self.afterHide()
		self._visible = False
	
	def adaptLayout(self):
		self._recursiveResizeToContent()
		self._recursiveExpandContent()
	
	def beforeShow(self):
		pass

	def afterHide(self):
		pass

	def findChildren(self,**kwargs):
		""" Find all contained child widgets by attribute values.
		
		closeButtons = root_widget.findChildren(name='close')
		"""

		children = []
		def _childCollector(widget):
			if widget.match(**kwargs):
				children.append(widget)
		self._deepApply(_childCollector)
		return children

	def findChild(self,**kwargs):
		""" Find the first contained child widgets by attribute values.
		
		closeButton = root_widget.findChild(name='close')
		"""
		children = self.findChildren(**kwargs)
		if children:
			return children[0]
		return None


	def resizeToContent(self,recurse = True):
		pass

	def expandContent(self,recurse = True):
		pass

	def _recursiveResizeToContent(self):
		def _callResizeToContent(widget):
			print "RTC:",widget
			widget.resizeToContent()
		self._deepApply(_callResizeToContent)

	def _recursiveExpandContent(self):
		def _callExpandContent(widget):
			print "ETC:",widget
			widget.expandContent()
		self._deepApply(_callExpandContent)

	def _deepApply(self,visitorFunc):
		visitorFunc(self)

	def sizeChanged(self):
		if self._parent:
			self._parent.sizeChanged()
		else:
			self.adaptLayout()

	def _setSize(self,size):
		if isinstance(size,fife.Point):
			self.width, self.height = size.x, size.y
		else:
			self.width, self.height = size
		#self.sizeChanged()

	def _getSize(self):
		return self.width, self.height

	def _setPosition(self,size):
		if isinstance(size,fife.Point):
			self.x, self.y = size.x, size.y
		else:
			self.x, self.y = size

	def _getPosition(self):
		return self.x, self.y

	def _setX(self,x): self.real_widget.setX(x)
	def _getX(self): return self.real_widget.getX()
	def _setY(self,y): self.real_widget.setY(y)
	def _getY(self): return self.real_widget.getY()

	def _setWidth(self,w):
		w = max(self.min_size[0],w)
		w = min(self.max_size[0],w)
		self.real_widget.setWidth(w)

	def _getWidth(self): return self.real_widget.getWidth()
	def _setHeight(self,h):
		h = max(self.min_size[1],h)
		h = min(self.max_size[1],h)
		self.real_widget.setHeight(h)

	def _getHeight(self): return self.real_widget.getHeight()

	def _setFont(self, font): self.real_widget.setFont(font)
	def _getFont(self): return self.real_widget.getFont()

	def _getBorderSize(self): return self.real_widget.getBorderSize()
	def _setBorderSize(self,size): self.real_widget.setBorderSize(size)

	def _getBaseColor(self): return self.real_widget.getBaseColor()
	def _setBaseColor(self,color):
		if isinstance(color,type(())):
			color = fife.Color(*color)
		self.real_widget.setBaseColor(color)
	base_color = property(_getBaseColor,_setBaseColor)
	
	def _getBackgroundColor(self): return self.real_widget.getBackgroundColor()
	def _setBackgroundColor(self,color):
		if isinstance(color,type(())):
			color = fife.Color(*color)
		self.real_widget.setBackgroundColor(color)
	background_color = property(_getBackgroundColor,_setBackgroundColor)

	def _getForegroundColor(self): return self.real_widget.getForegroundColor()
	def _setForegroundColor(self,color):
		if isinstance(color,type(())):
			color = fife.Color(*color)
		self.real_widget.setForegroundColor(color)
	foreground_color = property(_getForegroundColor,_setForegroundColor)
	
	def _getName(self): return self._name
	def _setName(self,name):
		self._name = name
		try:
			self._event_id = "%s(name=%s,id=%d)" % (str(self.__class__),name,id(self))
			self.real_widget.setActionEventId(self._event_id)
			self.real_widget.addActionListener(manager.guimanager)
		except AttributeError: pass
	name = property(_getName,_setName)

	x = property(_getX,_setX)
	y = property(_getY,_setY)
	width = property(_getWidth,_setWidth)
	height = property(_getHeight,_setHeight)
	size = property(_getSize,_setSize)
	position = property(_getPosition,_setPosition)
	font = property(_getFont,_setFont)
	border_size = property(_getBorderSize,_setBorderSize)

### Containers + Layout code ###

class Container(_widget,fife.Container):
	""" Basic Container Class """
	def __init__(self,padding=5,margins=(5,5),_real_widget=None, **kwargs):
		self.real_widget = _real_widget or fife.Container()
		self.children = []
		self.margins = margins
		self.padding = padding
		self._background = []
		self._background_image = None
		super(Container,self).__init__(**kwargs)

	def add(self, *widgets):
		for widget in widgets:
			self.children.append(widget)
			self.real_widget.add(widget.real_widget)

	def getMaxChildrenWidth(self):
		if not self.children: return 0
		return max(widget.width for widget in self.children)

	def getMaxChildrenHeight(self):
		if not self.children: return 0
		return max(widget.height for widget in self.children)

	def _deepApply(self,visitorFunc):
		for child in self.children:
			child._deepApply(visitorFunc)
		visitorFunc(self)

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

		# Background generation is done in _resetTiling

		if not isinstance(image, fife.GuiImage):
			image = manager.loadImage(image)
		self._background_image = image
	
	def getBackgroundImage(self): return self._background_image
	background_image = property(getBackgroundImage,setBackgroundImage)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)

	#def _setPadding(self,padding): self.real_widget.setPadding(padding)
	#def _getPadding(self): return self.real_widget.isPadding()
	#padding = property(_getPadding,_setPadding)

AlignTop, AlignBottom, AlignLeft, AlignRight, AlignCenter = range(5)

class LayoutBase(object):
	def __init__(self,align = (AlignLeft,AlignTop), **kwargs):
		self.align = align
		self.spacer = None
		super(LayoutBase,self).__init__(**kwargs)

	def addSpacer(self,spacer):
		self.spacer = spacer
		spacer.index = len(self.children)

	def xdelta(self,widget):return 0
	def ydelta(self,widget):return 0

	def _adjustHeight(self):
		
		if self.align[1] == AlignTop:return #dy = 0
		if self.align[1] == AlignBottom:
			y = self.height - self.childarea[1] - self.border_size - self.margins[1]
		else:
			y = (self.height - self.childarea[1] - self.border_size - self.margins[1])/2
		for widget in self.children:
			widget.y = y
			y += self.ydelta(widget)

	def _adjustHeightWithSpacer(self):
		pass

	def _adjustWidth(self):

		if self.align[0] == AlignLeft:return #dx = 0
		if self.align[0] == AlignRight:
			x = self.width - self.childarea[0] - self.border_size - self.margins[0]
		else:
			x = (self.width - self.childarea[0] - self.border_size - self.margins[0])/2
		for widget in self.children:
			widget.x = x
			x += self.xdelta(widget)

	def _expandWidthSpacer(self):
		x = self.border_size + self.margins[0]
		xdelta = map(self.xdelta,self.children)
		
		for widget in self.children[:self.spacer.index]:
			widget.x = x
			x += xdelta.pop(0)
		
		x = self.width - sum(xdelta) - self.border_size - self.margins[0]
		for widget in self.children[self.spacer.index:]:
			widget.x = x
			x += xdelta.pop(0)

	def _expandHeightSpacer(self):
		y = self.border_size + self.margins[1]
		ydelta = map(self.ydelta,self.children)
		
		for widget in self.children[:self.spacer.index]:
			widget.y = y
			y += ydelta.pop(0)
		
		y = self.height - sum(ydelta) - self.border_size - self.margins[1]
		for widget in self.children[self.spacer.index:]:
			widget.y = y
			y += ydelta.pop(0)


class VBoxLayoutMixin(LayoutBase):
	def __init__(self,**kwargs):
		super(VBoxLayoutMixin,self).__init__(**kwargs)

	def resizeToContent(self, recurse = True):
		max_w = self.getMaxChildrenWidth()
		x = self.margins[0] + self.border_size
		y = self.margins[1] + self.border_size
		for widget in self.children:
			widget.x = x
			widget.y = y
			widget.width = max_w
			y += widget.height + self.padding
		self.height = y + self.margins[1] - self.padding
		self.width = max_w + 2*x
		self.childarea = max_w, y - self.padding - self.margins[1]

		self._adjustHeight()
		self._adjustWidth()

	def expandContent(self):
		if self.spacer:
			self._expandHeightSpacer()

	def ydelta(self,widget):return widget.height + self.padding

class HBoxLayoutMixin(LayoutBase):
	def __init__(self,**kwargs):
		super(HBoxLayoutMixin,self).__init__(**kwargs)

	def resizeToContent(self, recurse = True):
		max_h = self.getMaxChildrenHeight()
		x = self.margins[0] + self.border_size
		y = self.margins[1] + self.border_size
		for widget in self.children:
			widget.x = x
			widget.y = y
			widget.height = max_h
			x += widget.width + self.padding
		self.width = x + self.margins[0] - self.padding
		self.height = max_h + 2*y
		self.childarea = x - self.margins[0] - self.padding, max_h
		
		self._adjustHeight()
		self._adjustWidth()

	def expandContent(self):
		if self.spacer:
			self._expandWidthSpacer()

	def xdelta(self,widget):return widget.width + self.padding


class VBox(VBoxLayoutMixin,Container):
	def __init__(self,padding=5,**kwargs):
		super(VBox,self).__init__(**kwargs)
		self.padding = padding


class HBox(HBoxLayoutMixin,Container):
	def __init__(self,padding=5,**kwargs):
		super(HBox,self).__init__(**kwargs)
		self.padding = padding

class Window(VBoxLayoutMixin,Container):
	def __init__(self,title="title",titlebar_height=0,**kwargs):
		super(Window,self).__init__(_real_widget = fife.Window(), **kwargs)
		if titlebar_height == 0:
			titlebar_height = self.font.getHeight() + 4
		self.titlebar_height = titlebar_height
		self.title = title

	def _getTitle(self): return self.real_widget.getCaption()
	def _setTitle(self,text): self.real_widget.setCaption(text)
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

### Basic Widgets ###

class _basicTextWidget(_widget):
	def __init__(self, text = "",**kwargs):
		self.margins = (5,5)
		self.text = text
		super(_basicTextWidget,self).__init__(**kwargs)

	def _getText(self): return self.real_widget.getCaption()
	def _setText(self,text): self.real_widget.setCaption(text)
	text = property(_getText,_setText)

	def resizeToContent(self, recurse = True):
		self.height = self.font.getHeight() + self.margins[1]*2
		self.width = self.font.getWidth(self.text) + self.margins[0]*2

class Label(_basicTextWidget):
	def __init__(self,**kwargs):
		self.real_widget = fife.Label("")
		super(Label,self).__init__(**kwargs)

class ClickLabel(_basicTextWidget):
	def __init__(self,**kwargs):
		self.real_widget = fife.ClickLabel("")
		super(ClickLabel,self).__init__(**kwargs)

class Button(_basicTextWidget):
	def __init__(self,**kwargs):
		self.real_widget = fife.Button("")
		super(Button,self).__init__(**kwargs)

class CheckBox(_basicTextWidget):
	def __init__(self,**kwargs):
		self.real_widget = fife.CheckBox()
		super(CheckBox,self).__init__(**kwargs)

class GenericListmodel(fife.ListModel,list):
	def __init__(self,*args):
		super(GenericListmodel,self).__init__()
		map(self.append,args)
		
	def getNumberOfElements(self):
		return len(self)
		
	def getElementAt(self, i):
		i = max(0,min(i,len(self) - 1))
		return str(self[i])

class ListBox(_widget):
	def __init__(self,items=[],**kwargs):
		self.items = GenericListmodel(*items)
		self.real_widget = fife.ListBox(self.items)
		super(ListBox,self).__init__(**kwargs)

	def resizeToContent(self,recurse=True):
		# We append a minimum value, so max() does not bail out,
		# if no items are in the list
		_item_widths = map(self.font.getWidth,map(str,self.items)) + [0]
		max_w = max(_item_widths)
		self.width = max_w
		self.height = (self.font.getHeight() + 2) * len(self.items)

	def _getSelected(self): return self.real_widget.getSelected()
	def _setSelected(self,index): self.real_widget.setSelected(index)
	selected = property(_getSelected,_setSelected)
	def _getSelectedItem(self):
		if 0 <= self.selected < len(self.items):
			return self.items[self.selected]
		return None
	selected_item = property(_getSelectedItem)
	
class TextBox(_widget):
	def __init__(self,text="",filename = "", **kwargs):
		self.real_widget = fife.TextBox()
		self.text = text
		self.filename = filename
		super(TextBox,self).__init__(**kwargs)

	def _getFileName(self): return self._filename
	def _loadFromFile(self,filename):
		self._filename = filename
		if not filename: return
		try:
			self.text = _mungeText(open(filename).read())
		except Exception, e:
			self.text = str(e)
	filename = property(_getFileName, _loadFromFile)

	def resizeToContent(self,recurse=True):
		rows = [self.real_widget.getTextRow(i) for i in range(self.real_widget.getNumberOfRows())]
		max_w = max(map(self.font.getWidth,rows))
		self.width = max_w
		self.height = (self.font.getHeight() + 2) * self.real_widget.getNumberOfRows()
	def _getText(self): return self.real_widget.getText()
	def _setText(self,text): self.real_widget.setText(text)
	text = property(_getText,_setText)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)



class ScrollArea(_widget):
	def __init__(self,**kwargs):
		self.real_widget = fife.ScrollArea()
		self._content = None
		super(ScrollArea,self).__init__(**kwargs)

	def _setContent(self,content):
		self.real_widget.setContent(content.real_widget)
		self._content = content
	def _getContent(self): return self._content
	content = property(_getContent,_setContent)

	def _deepApply(self,visitorFunc):
		if self._content: visitorFunc(self._content)
		visitorFunc(self)

	def resizeToContent(self,recurse=True):
		if self._content is None: return
		if recurse:
			self.content.resizeToContent(recurse=True)
		self.content.width = max(self.content.width,self.width-5)
		self.content.height = max(self.content.height,self.height-5)


# Spacer

class Spacer(object):
	def __init__(self,parent=None,**kwargs):
		self._parent = parent

	def sizeChanged(self):
		pass


# XML Loader

from xml.sax import saxutils, handler

class GuiXMLError(Exception):
	pass

class _GuiLoader(object, handler.ContentHandler):
	def __init__(self):
		super(_GuiLoader,self).__init__()
		self.root = None
		self.indent = ""
		self.stack = []
		
	def _toIntList(self,s):
		try:
			return tuple(map(int,str(s).split(',')))
		except:
			raise GuiXMLError("Expected an comma separated list of integers.")

	def _parseAttr(self,attr):
		name, value = attr
		name = str(name)
		if name in ('position','size','min_size','max_size','margins'):
			value = self._toIntList(value)
		elif name in ('foreground_color','base_color','background_color'):
			value = self._toIntList(value)
		elif name in ('opaque','border_size','padding'):
			value = int(value)
		else:
			value = str(value)
		return (name,value)

	def _printTag(self,name,attrs):
		if not manager.debug: return
		attrstrings = map(lambda t: '%s="%s"' % tuple(map(str,t)),attrs.items())
		tag = "<%s " % name + " ".join(attrstrings) + ">"
		print self.indent + tag

	def _resolveTag(self,name):
		""" Resolve a XML Tag to a PyChan GUI class. """
		#FIXME Using globals is a __QUICK HACK__
		cls = globals().get(name,None)
		if cls is None:
			raise GuiXMLError("Unknown GUI Element: %s" % name)
		return cls

	def startElement(self, name, attrs):
		self._printTag(name,attrs)
		cls = self._resolveTag(name)
		if issubclass(cls,_widget):
			self.stack.append('gui_element')
			self._createInstance(cls,name,attrs)
		elif cls == Spacer:
			self.stack.append('spacer')
			self._createSpacer(cls,name,attrs)
		else:
			self.stack.append('unknown')
		self.indent += " "*4

	def _createInstance(self,cls,name,attrs):
		attrs = map(self._parseAttr,attrs.items())
		obj = cls(parent=self.root)
		for k,v in attrs:
			setattr(obj,k,v)
			
		if hasattr(self.root,'add'):
			self.root.add(obj)
		elif hasattr(self.root,'content'):
			self.root.content = obj
		else:
			if self.root is not None:
				raise GuiXMLError("*** unknown containment relation :-(")
		self.root = obj
	
	def _createSpacer(self,cls,name,attrs):
		attrs = map(self._parseAttr,attrs.items())
		obj = cls(parent=self.root)
		for k,v in attrs:
			setattr(obj,k,v)
			
		if hasattr(self.root,'add'):
			self.root.addSpacer(obj)
		else:
			raise GuiXMLError("A spacer needs to be added to a container widget!")
		self.root = obj

	def endElement(self, name):
		self.indent = self.indent[:-4]
		if manager.debug: print self.indent + "</%s>" % name
		if self.stack.pop() in ('gui_element','spacer'):
			self.root = self.root._parent or self.root

def loadXML(file):
	from xml.sax import parse
	loader = _GuiLoader()
	parse(file,loader)
	return loader.root
