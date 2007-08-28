# coding: utf-8

import fife, pythonize

__doc__="""\
Simple Guichan Wrapper
Very unfinished.

Features:
* Simpler Interface
* Very Basic Layout Engine
* Very Basic XML Format support
* Basic Styling support

TODO:
* Completion of above features
* Wrap missing widgets
* Documentation
* Add support for 'Spacers' in layouts
* Easier Font handling.

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
	

### Initialisation ###

class _Manager(fife.IWidgetListener, fife.TimeEvent):
	def __init__(self, engine):
		super(_Manager,self).__init__()
		
		self.guimanager = engine.guiManager
		self.engine = engine
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
		self.imagefont = fife.SubImageFont('content/fonts/rpgfont.png',glyphs,self.engine.imagePool)
		self.font = fife.TTFont("content/fonts/FreeMono.ttf",20)

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
			Checkbox : {
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
def init(engine):
	global manager
	manager = _Manager(engine)

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
		
		try:
			self._event_id = "%s(name=%s,id=%d)" % (str(self.__class__),name,id(self))
			self.real_widget.setActionEventId(self._event_id)
			self.real_widget.addActionListener(manager.guimanager)
		except AttributeError: pass
		self.resizeToContent()

	def capture(self, callback):
		def captured_f(event):
			applyOnlySuitable(callback,event=event,widget=self)
		manager.widgetEvents.setdefault(self._event_id,[]).append(captured_f)

	def show(self):
		self.beforeShow()
		manager.show(self)
		self._visible = True

	def hide(self):
		manager.hide(self)
		self.afterHide()
		self._visible = False
	
	def beforeShow(self): pass
	def afterHide(self): pass
	
	def findChildren(self,**kwargs):
		return []

	def resizeToContent(self,recurse = True):
		pass

	def sizeChanged(self):
		if self._parent:
			self._parent.sizeChanged()
		else:
			self.resizeToContent()

	def _setSize(self,size):
		if isinstance(size,fife.Point):
			self.width, self.height = size.x, size.y
		else:
			self.width, self.height = size

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
	def __init__(self,padding=5,margins=(5,5),_real_widget=None, **kwargs):
		self.real_widget = _real_widget or fife.Container()
		self.children = []
		self.margins = margins
		self.padding = padding
		super(Container,self).__init__(**kwargs)

	def add(self, *widgets):
		for widget in widgets:
			self.children.append(widget)
			self.real_widget.add(widget.real_widget)

	def _recursiveResizeToContent(self):
		for widget in self.children:
			getattr(widget,'resizeToContent',lambda *args: None)()

	def getMaxChildrenWidth(self):
		if not self.children: return 0
		return max(widget.width for widget in self.children)

	def getMaxChildrenHeight(self):
		if not self.children: return 0
		return max(widget.height for widget in self.children)

	def findChildren(self,**kwargs):
		children = []
		for widget in self.children:
			add = True
			for k,v in kwargs.items():
				if v != getattr(widget,k,None):
					add = False
					break
			if add: children.append(widget)
		for widget in self.children:
			children += widget.findChildren(**kwargs)
		return children

	def beforeShow(self):
		self.resizeToContent()
		background = getattr(self,'_background',None)
		if background:
			background.requestMoveToBottom()

	def setBackgroundImage(self,image):
		self._background = getattr(self,'_background',None)
		if image is None:
			self._background_image = image
			if self._background is not None:
				self.real_widget.remove(self._background)
			return

		if not isinstance(image, fife.GuiImage):
			image = manager.loadImage(image)
		self._background_image = image
		if self._background is not None:
			self.real_widget.remove(self._background)
		self._background = fife.Icon(image)
		self._background.setPosition(0,0)
		self.real_widget.add(self._background)
	def getBackgroundImage(self): return self._background_image
	background_image = property(getBackgroundImage,setBackgroundImage)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)

AlignTop, AlignBottom, AlignLeft, AlignRight, AlignCenter = range(5)

class LayoutBase(object):
	def __init__(self,align = (AlignLeft,AlignTop), **kwargs):
		self.align = align
		super(LayoutBase,self).__init__(**kwargs)

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

	def _adjustWidth(self):
		if self.align[0] == AlignLeft:return #dx = 0
		if self.align[0] == AlignRight:
			x = self.width - self.childarea[0] - self.border_size - self.margins[0]
		else:
			x = (self.width - self.childarea[0] - self.border_size - self.margins[0])/2
		for widget in self.children:
			widget.x = x
			x += self.xdelta(widget)

class VBoxLayoutMixin(LayoutBase):
	def __init__(self,**kwargs):
		super(VBoxLayoutMixin,self).__init__(**kwargs)

	def resizeToContent(self, recurse = True):
		if recurse: self._recursiveResizeToContent()
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

	def ydelta(self,widget):return widget.height + self.padding

class HBoxLayoutMixin(LayoutBase):
	def __init__(self,**kwargs):
		super(HBoxLayoutMixin,self).__init__(**kwargs)

	def resizeToContent(self, recurse = True):
		if recurse: self._recursiveResizeToContent()
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

class Checkbox(_basicTextWidget):
	def __init__(self,**kwargs):
		self.real_widget = fife.CheckBox()
		super(Checkbox,self).__init__(**kwargs)

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
		max_w = max(map(self.font.getWidth,map(str,self.items)))
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
	def __init__(self,text="",**kwargs):
		self.real_widget = fife.TextBox()
		self.text = text
		super(TextBox,self).__init__(**kwargs)

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

	def resizeToContent(self,recurse=True):
		if self._content is None: return
		if recurse:
			self.content.resizeToContent(recurse=True)
		self.content.width = max(self.content.width,self.width-5)
		self.content.height = max(self.content.height,self.height-5)

from xml.sax import saxutils

class _GuiLoader(object,saxutils.DefaultHandler):
	def __init__(self):
		super(_GuiLoader,self).__init__()
		self.root = None
		self.indent = ""
		self.stack = []
		
	def _parseAttr(self,attr):
		def _toIntList(s): return tuple(map(int,str(s).split(',')))
		name, value = attr
		name = str(name)
		if name in ('position','size','min_size','max_size','margins'):
			value = _toIntList(value)
		elif name in ('foreground_color','base_color','background_color'):
			value = _toIntList(value)
		elif name in ('opaque','border_size','padding'):
			value = int(value)
		else:
			value = str(value)
		return (name,value)

	def _printTag(self,name,attrs):
		attrstrings = map(lambda t: '%s="%s"' % tuple(map(str,t)),attrs.items())
		tag = "<%s " % name + " ".join(attrstrings) + ">"
		print self.indent + tag

	def startElement(self, name, attrs):
		self._printTag(name,attrs)
		cls = globals().get(name,None)
		if issubclass(cls,_widget):
			self.stack.append('gui_element')
			self._createInstance(cls,name,attrs)
		else:
			self.stack.append('unknown')
		self.indent += " "*4

	def _createInstance(self,cls,name,attrs):
		attrs = map(self._parseAttr,attrs.items())
		obj = cls(parent=self.root)
		for k,v in attrs:
			setattr(obj,k,v)
		try:
			if hasattr(self.root,'add'):
				self.root.add(obj)
			elif hasattr(self.root,'content'):
				self.root.content = obj
			else:
				print "*** unknown containment relation :-("
		except:
			pass
		self.root = obj

	def endElement(self, name):
		self.indent = self.indent[:-4]
		print self.indent + "</%s>" % name
		if self.stack.pop() == 'gui_element':
			self.root = self.root._parent or self.root

def loadXML(file):
	from xml.sax import parse
	loader = _GuiLoader()
	parse(file,loader)
	return loader.root
