### Widget/Container Base Classes ###

import fife, pythonize
import tools
from exceptions import *

def get_manager():
    import pychan
    return pychan.manager

def _mungeText(text):
	"""
	This function is applied to all text set on widgets, currently only replacing tabs with four spaces.
	"""
	return text.replace('\t'," "*4)

class Widget(object):
	"""
	This is the common widget base class, which provides most of the wrapping
	functionality.
	
	Attributes
	==========
	
	Widgets are manipulated (mostly) through attributes - and these can all be set by XML attributes.
	Derived widgets will have other attributes. Please see their B{New Attributes} sections. The types of the
	attributes are pretty straightforward, but note that Position and Color attribute types will also accept
	C{fife.Point} and C{fife.Color} values.
	
	  - name: String: The identification of the widget, most useful if it is unique within a given widget hiarachy.
	  This is used to find widgets by L{mapEvents},L{distributeInitialData},L{distributeData} and L{collectData}.
	  - position: Position: The position relative to the parent widget - or on screen, if this is the root widget.
	  - size: Position: The real size of the widget (including border and margins). Usually you do not need to set this.
	  A notable exception is the L{ScrollArea}.
	  - min_size: Position: The minimal size this widget is allowed to have. This is enforced through the accessor methods
	  of the actual size attribute.
	  - max_size: Position: The maximal size this widget is allowed to have. This is enforced through the accessor methods
	  of the actual size attribute.
	  - base_color: Color
	  - background_color: Color
	  - foreground_color: Color
	  - font: String: This should identify a font that was loaded via L{loadFonts} before.
	  - border_size: Integer: The size of the border in pixels.
	  - position_technique: This can be either "automatic" or "explicit" - only L{Window} has this set to "automatic" which
	  results in new windows being centered on screen (for now).
	  If it is set to "explicit" the position attribute will not be touched.

	Convenience Attributes
	======================
	
	These attributes are convenience/shorthand versions of above mentioned attributes and assignment will reflect
	the associated attributes values. E.g. the following is equivalent::
	   # Set X position, leave Y alone
	   widget.x = 10
	   # Same here
	   posi = widget.position
	   widget.position = (10, posi[1])

	Here they are.

	   - x: Integer: The horizontal part of the position attribute.
	   - y: Integer: The vertical part of the position attribute.
	   - width: Integer: The horizontal part of the size attribute.
	   - height: Integer: The vertical part of the size attribute.

	"""
	def __init__(self,parent = None, name = '_unnamed_',
			size = (-1,-1), min_size=(0,0), max_size=(5000,5000),
			style = None, **kwargs):
		
		assert( hasattr(self,'real_widget') )
		self._parent = parent
		self.name = name
		self.min_size = min_size
		self.max_size = max_size
		self.size = size
		self.position_technique = "explicit"
		self._visible = False
		self.font = 'default'
		
		self.accepts_data = False
		self.accepts_initial_data = False

		# Inherit style
		if style is None and parent:
			style = parent.style
		self.style = style or "default"
		
		# Not needed as attrib assignment will trigger manager.stylize call
		#manager.stylize(self,self.style)
	
	def match(self,**kwargs):
		"""
		Matches the widget against a list of key-value pairs.
		Only if all keys are attributes and their value is the same it returns True.
		"""
		for k,v in kwargs.items():
			if v != getattr(self,k,None):
				return False
		return True

	def capture(self, callback):
		"""
		Add a callback to be executed when the widget event occurs on this widget.
		"""
		def captured_f(event):
			tools.applyOnlySuitable(callback,event=event,widget=self)
		get_manager().widgetEvents.setdefault(self._event_id,[]).append(captured_f)

	def show(self):
		"""
		Show the widget and all contained widgets.
		"""
		if self._visible: return
		self.beforeShow()
		self.adaptLayout()
		get_manager().show(self)
		self._visible = True

	def hide(self):
		"""
		Hide the widget and all contained widgets.
		"""
		if not self._visible: return
		get_manager().hide(self)
		self.afterHide()
		self._visible = False
	
	def adaptLayout(self):
		"""
		Execute the Layout engine. Automatically called by L{show}.
		In case you want to relayout a visible widget, you have to call this function
		on the root widget.
		"""
		self._recursiveResizeToContent()
		self._recursiveExpandContent()
	
	def beforeShow(self):
		"""
		This method is called just before the widget is shown.
		You can override this in derived widgets to add finalization
		behaviour.
		"""
		pass

	def afterHide(self):
		"""
		This method is called just before the widget is hidden.
		You can override this in derived widgets to add finalization
		behaviour.
		"""
		pass

	def findChildren(self,**kwargs):
		"""
		Find all contained child widgets by attribute values.
		
		closeButtons = root_widget.findChildren(name='close')
		"""

		children = []
		def _childCollector(widget):
			if widget.match(**kwargs):
				children.append(widget)
		self.deepApply(_childCollector)
		return children

	def findChild(self,**kwargs):
		""" Find the first contained child widgets by attribute values.
		
		closeButton = root_widget.findChild(name='close')
		"""
		children = self.findChildren(**kwargs)
		if children:
			return children[0]
		return None

	def mapEvents(self,eventMap,ignoreMissing = False):
		"""
		Convenience function to map widget events to functions
		in a batch.
		
		@param eventMap: A dictionary with widget names as keys and callbacks as values.
		@param ignoreMissing: Normally this method raises an RuntimeError, when a widget
		can not be found - this behaviour can be overriden by passing True here.
		"""
		for name,func in eventMap.items():
			widget = self.findChild(name=name)
			if widget:
				widget.capture( func )
			elif not ignoreMissing:
				raise RuntimeError("No widget with the name: %s" % name)

	def setInitialData(self,data):
		"""
		Set the initial data on a widget, what this means depends on the Widget.
		In case the widget does not accept initial data, a L{RuntimeError} is thrown.
		"""
		if not self.accepts_initial_data:
			raise RuntimeError("Trying to set data on a widget that does not accept initial data.")
		self._realSetInitialData(data)
	
	def setData(self,data):
		"""
		Set the user-mutable data on a widget, what this means depends on the Widget.
		In case the widget does not accept data, a L{RuntimeError} is thrown.
		This is inverse to L{getData}.
		"""
		if not self.accepts_data:
			raise RuntimeError("Trying to set data on a widget that does not accept data.")
		self._realSetData(data)

	def getData(self):
		"""
		Get the user-mutable data of a widget, what this means depends on the Widget.
		In case the widget does not have user mutable data, a L{RuntimeError} is thrown.
		This is inverse to L{setData}.
		"""
		if not self.accepts_data:
			raise RuntimeError("Trying to retrieve data from a widget that does not accept data.")
		return self._realGetData()

	def distributeInitialData(self,initialDataMap):
		"""
		Distribute B{initial} (not mutable by the user) data from a dictionary over the widgets in the hierachy
		using the keys as names and the values as the data (which is set via L{setInitialData}).
		If more than one widget matches - the data is set on ALL matching widgets.
		By default a missing widget is just ignored.
		
		Use it like this::
		  guiElement.distributeInitialData({
		       'myTextField' : 'Hello World!',
		       'myListBox' : ["1","2","3"]
		  })
		
		"""
		for name,data in initialDataMap.items():
			widgetList = self.findChildren(name = name)
			for widget in widgetList:
				widget.setInitialData(data)
	
	def distributeData(self,dataMap):
		"""
		Distribute data from a dictionary over the widgets in the hierachy
		using the keys as names and the values as the data (which is set via L{setData}).
		This will only accept unique matches.
		
		Use it like this::
		  guiElement.distributeData({
		       'myTextField' : 'Hello World!',
		       'myListBox' : ["1","2","3"]
		  })
		
		"""
		for name,data in dataMap.items():
			widgetList = self.findChildren(name = name)
			if len(widgetList) != 1:
				raise RuntimeError("DistributeData can only handle widgets with unique names.")
			widgetList[0].setData(data)

	def collectData(self,widgetNames):
		"""
		Collect data from a widget hierachy by names.
		This can only handle UNIQUE widget names (in the hierachy)
		and will raise a RuntimeError if the number of matching widgets
		is not equal to one.
		
		Usage::
		  data = guiElement.collectData(['myTextField','myListBox'])
		  print "You entered:",data['myTextField']," and selected ",data['myListBox']
		
		"""
		dataMap = {}
		for name in widgetNames:
			widgetList = self.findChildren(name = name)
			if len(widgetList) != 1:
				raise RuntimeError("CollectData can only handle widgets with unique names.")
			
			dataMap[name] = widgetList[0].getData()
		return dataMap

	def stylize(self,style,**kwargs):
		"""
		Recursively apply a style to all widgets.
		"""
		def _restyle(widget):
			get_manager().stylize(widget,style,**kwargs)
		self.deepApply(_restyle)

	def resizeToContent(self,recurse = True):
		"""
		Try to shrink the widget, so that it fits closely around its content.
		Do not call directly.
		"""
		pass

	def expandContent(self,recurse = True):
		"""
		Try to expand any spacer in the widget within the current size.
		Do not call directly.
		"""
		pass

	def _recursiveResizeToContent(self):
		"""
		Recursively call L{resizeToContent}. Uses L{deepApply}.
		Do not call directly.
		"""
		def _callResizeToContent(widget):
			#print "RTC:",widget
			widget.resizeToContent()
		self.deepApply(_callResizeToContent)

	def _recursiveExpandContent(self):
		"""
		Recursively call L{expandContent}. Uses L{deepApply}.
		Do not call directly.
		"""
		def _callExpandContent(widget):
			#print "ETC:",widget
			widget.expandContent()
		self.deepApply(_callExpandContent)

	def deepApply(self,visitorFunc):
		"""
		Recursively apply a callable to all contained widgets and then the widget itself.
		"""
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

	def _setX(self,x):self.real_widget.setX(x)
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

	def _setFont(self, font):
		self._font = font
		self.real_font = get_manager().getFont(font)
		self.real_widget.setFont(self.real_font)
	def _getFont(self):
		return self._font

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
			self.real_widget.addActionListener(get_manager().guimanager)
		except AttributeError: pass
	name = property(_getName,_setName)

	def _getStyle(self): return self._style
	def _setStyle(self,style):
		self._style = style
		get_manager().stylize(self,style)
	style = property(_getStyle,_setStyle)

	x = property(_getX,_setX)
	y = property(_getY,_setY)
	width = property(_getWidth,_setWidth)
	height = property(_getHeight,_setHeight)
	size = property(_getSize,_setSize)
	position = property(_getPosition,_setPosition)
	font = property(_getFont,_setFont)
	border_size = property(_getBorderSize,_setBorderSize)

### Containers + Layout code ###

class Container(Widget,fife.Container):
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
	def __init__(self,padding=5,margins=(5,5),_real_widget=None, **kwargs):
		self.real_widget = _real_widget or fife.Container()
		self.children = []
		self.margins = margins
		self.padding = padding
		self._background = []
		self._background_image = None
		super(Container,self).__init__(**kwargs)

	def add(self, *widgets):
		"""
		Adds a child widget to the container.
		"""
		for widget in widgets:
			self.children.append(widget)
			self.real_widget.add(widget.real_widget)

	def getMaxChildrenWidth(self):
		if not self.children: return 0
		return max(widget.width for widget in self.children)

	def getMaxChildrenHeight(self):
		if not self.children: return 0
		return max(widget.height for widget in self.children)

	def deepApply(self,visitorFunc):
		for child in self.children:
			child.deepApply(visitorFunc)
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
			image = get_manager().loadImage(image)
		self._background_image = image
	
	def getBackgroundImage(self): return self._background_image
	background_image = property(getBackgroundImage,setBackgroundImage)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)

AlignTop, AlignBottom, AlignLeft, AlignRight, AlignCenter = range(5)

class LayoutBase(object):
	"""
	This class is at the core of the layout engine. The two MixIn classes L{VBoxLayoutMixin}
	and L{HBoxLayoutMixin} specialise on this by reimplementing the C{resizeToContent} and
	the C{expandContent} methods.
	
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
		self.spacer = None
		super(LayoutBase,self).__init__(**kwargs)

	def addSpacer(self,spacer):
		if self.spacer:
			raise RuntimeException("Already a Spacer in %s!" % str(self))
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
	"""
	A mixin class for a vertical layout. Do not use directly.
	"""
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
		
		#Add the padding for the spacer.
		if self.spacer:
			y += self.padding

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
	"""
	A mixin class for a horizontal layout. Do not use directly.
	"""
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
		
		#Add the padding for the spacer.
		if self.spacer:
			x += self.padding

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
	def __init__(self,padding=5,**kwargs):
		super(VBox,self).__init__(**kwargs)
		self.padding = padding


class HBox(HBoxLayoutMixin,Container):
	"""
	A horizontally aligned box - for containement of child widgets.
	
	Please see L{VBox} for details - just change the directions :-).
	"""	
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
	def __init__(self,title="title",titlebar_height=0,**kwargs):
		super(Window,self).__init__(_real_widget = fife.Window(), **kwargs)
		if titlebar_height == 0:
			titlebar_height = self.real_font.getHeight() + 4
		self.titlebar_height = titlebar_height
		self.title = title

		# Override explicit positioning
		self.position_technique = "automatic"


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

class BasicTextWidget(Widget):
	"""
	The base class for widgets which display a string - L{Label},L{ClickLabel},L{Button}, etc.
	Do not use directly.
	
	New Attributes
	==============
	
	  - text: The text (depends on actual widget)
	
	Data
	====
	
	The text can be set via the L{distributeInitialData} method.
	"""
	def __init__(self, text = "",**kwargs):
		self.margins = (5,5)
		self.text = text
		super(BasicTextWidget,self).__init__(**kwargs)
		
		# Prepare Data collection framework
		self.accepts_initial_data = True
		self._realSetInitialData = self._setText

	def _getText(self): return self.real_widget.getCaption()
	def _setText(self,text): self.real_widget.setCaption(text)
	text = property(_getText,_setText)

	def resizeToContent(self, recurse = True):
		self.height = self.real_font.getHeight() + self.margins[1]*2
		self.width = self.real_font.getWidth(self.text) + self.margins[0]*2

class Label(BasicTextWidget):
	"""
	A basic label - displaying a string.
	"""
	def __init__(self,**kwargs):
		self.real_widget = fife.Label("")
		super(Label,self).__init__(**kwargs)

class Icon(Widget):
	"""
	An image icon.
	
	New Attributes
	==============
	
	  - source: The source location of the Image.
	"""
	def __init__(self,source="",**kwargs):
		self.real_widget = fife.Icon(None)
		super(Icon,self).__init__(**kwargs)

	def _setSource(self,source):
		self._source = source
		self._image = get_manager().loadImage(source)
		self.real_widget.setImage( self._image )
                print self._image,self.real_widget.getWidth(),self.real_widget.getWidth()
	def _getSource(self): return self._source
	source = property(_getSource,_setSource)

class ClickLabel(BasicTextWidget):
	"""
	A basic label - displaying a string.
	
	Only difference to L{Label} is that this will generate an event,
	if clicked - just like a HTML link.
	"""
	def __init__(self,**kwargs):
		self.real_widget = fife.ClickLabel("")
		super(ClickLabel,self).__init__(**kwargs)

class Button(BasicTextWidget):
	"""
	A basic push button.
	"""
	def __init__(self,**kwargs):
		self.real_widget = fife.Button("")
		super(Button,self).__init__(**kwargs)


class CheckBox(BasicTextWidget):
	"""
	A basic checkbox.
	
	New Attributes
	==============
	
	  - marked: Boolean value, whether the checkbox is checked or not.
	
	Data
	====
	The marked status can be read and set via L{distributeData} and L{collectData}
	"""
	def __init__(self,**kwargs):
		self.real_widget = fife.CheckBox()
		super(CheckBox,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_data = True
		self._realGetData = self._isMarked
		self._realSetData = self._setMarked
		
		# Initial data stuff inherited.
	
	def _isMarked(self): return self.real_widget.isMarked()
	def _setMarked(self,mark): self.real_widget.setMarked(mark)
	marked = property(_isMarked,_setMarked)

class GenericListmodel(fife.ListModel,list):
	"""
	A wrapper for the exported list model to behave more like a Python list.
	Don't use directly.
	"""
	def __init__(self,*args):
		super(GenericListmodel,self).__init__()
		map(self.append,args)
	def clear(self):
		while len(self):
			self.pop()
	def getNumberOfElements(self):
		return len(self)
		
	def getElementAt(self, i):
		i = max(0,min(i,len(self) - 1))
		return str(self[i])

class ListBox(Widget):
	"""
	A basic list box widget for displaying lists of strings. It makes most sense to wrap
	this into a L{ScrollArea}.
	
	New Attributes
	==============
	
	  - items: A List of strings. This can be treated like an ordinary python list.
	    but only strings are allowed.
	  - selected: The index of the selected item in the list. Starting from C{0} to C{len(items)-1}.
	    A negative value indicates, that no item is selected.
	  - selected_item: The selected string itself, or C{None} - if no string is selected.
	
	Data
	====
	The selected attribute can be read and set via L{distributeData} and L{collectData}.
	The list items can be set via L{distributeInitialData}.
	"""
	def __init__(self,items=[],**kwargs):
		self._items = GenericListmodel(*items)
		self.real_widget = fife.ListBox(self._items)
		super(ListBox,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_initial_data = True
		self._realSetInitialData = self._setItems
		
		self.accepts_data = True
		self._realSetData = self._setSelected
		self._realGetData = self._getSelected

	def resizeToContent(self,recurse=True):
		# We append a minimum value, so max() does not bail out,
		# if no items are in the list
		_item_widths = map(self.real_font.getWidth,map(str,self._items)) + [0]
		max_w = max(_item_widths)
		self.width = max_w
		self.height = (self.real_font.getHeight() + 2) * len(self._items)

	def _getItems(self): return self._items
	def _setItems(self,items):
		# Note we cannot use real_widget.setListModel
		# for some reason ???
		
		# Also self assignment can kill you
		if id(items) != id(self._items):
			self._items.clear()
			self._items.extend(items)

	items = property(_getItems,_setItems)

	def _getSelected(self): return self.real_widget.getSelected()
	def _setSelected(self,index): self.real_widget.setSelected(index)
	selected = property(_getSelected,_setSelected)
	def _getSelectedItem(self):
		if 0 <= self.selected < len(self._items):
			return self._items[self.selected]
		return None
	selected_item = property(_getSelectedItem)

class DropDown(Widget):
	"""
	A dropdown or combo box widget for selecting lists of strings.
	
	New Attributes
	==============
	
	  - items: A List of strings. This can be treated like an ordinary python list.
	    but only strings are allowed.
	  - selected: The index of the selected item in the list. Starting from C{0} to C{len(items)-1}.
	    A negative value indicates, that no item is selected.
	  - selected_item: The selected string itself, or C{None} - if no string is selected.
	
	Data
	====
	The selected attribute can be read and set via L{distributeData} and L{collectData}.
	The list items can be set via L{distributeInitialData}.
	"""
	def __init__(self,items=[],**kwargs):
		self._items = GenericListmodel(*items)
		self.real_widget = fife.DropDown(self._items)
		super(DropDown,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_initial_data = True
		self._realSetInitialData = self._setItems
		
		self.accepts_data = True
		self._realSetData = self._setSelected
		self._realGetData = self._getSelected

	def resizeToContent(self,recurse=True):
		# We append a minimum value, so max() does not bail out,
		# if no items are in the list
		_item_widths = map(self.real_font.getWidth,map(str,self._items)) + [self.real_font.getHeight()]
		max_w = max(_item_widths)
		self.width = max_w
		self.height = (self.real_font.getHeight() + 2)

	def _getItems(self): return self._items
	def _setItems(self,items):
		# Note we cannot use real_widget.setListModel
		# for some reason ???
		
		# Also self assignment can kill you
		if id(items) != id(self._items):
			self._items.clear()
			self._items.extend(items)
	items = property(_getItems,_setItems)
	
	def _getSelected(self): return self.real_widget.getSelected()
	def _setSelected(self,index): self.real_widget.setSelected(index)
	selected = property(_getSelected,_setSelected)
	def _getSelectedItem(self):
		if 0 <= self.selected < len(self._items):
			return self._items[self.selected]
		return None
	selected_item = property(_getSelectedItem)

class TextBox(Widget):
	"""
	An editable B{multiline} text edit widget.
	
	New Attributes
	==============
	
	  - text: The text in the TextBox.
	  - filename: A write-only attribute - assigning a filename will cause the widget to load it's text from it.
	
	Data
	====
	The text can be read and set via L{distributeData} and L{collectData}.
	"""

	def __init__(self,text="",filename = "", **kwargs):
		self.real_widget = fife.TextBox()
		self.text = text
		self.filename = filename
		super(TextBox,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_data = True
		self.accepts_inital_data = False # Would make sense in a way ...
		self._realSetInitialData = self._setText
		self._realSetData = self._setText
		self._realGetData = self._getText

	def _getFileName(self): return self._filename
	def _loadFromFile(self,filename):
		self._filename = filename
		if not filename: return
		try:
			self.text = open(filename).read()
		except Exception, e:
			self.text = str(e)
	filename = property(_getFileName, _loadFromFile)

	def resizeToContent(self,recurse=True):
		rows = [self.real_widget.getTextRow(i) for i in range(self.real_widget.getNumberOfRows())]
		max_w = max(map(self.real_font.getWidth,rows))
		self.width = max_w
		self.height = (self.real_font.getHeight() + 2) * self.real_widget.getNumberOfRows()

	def _getText(self): return self.real_widget.getText()
	def _setText(self,text): self.real_widget.setText(_mungeText(text))
	text = property(_getText,_setText)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)

class TextField(Widget):
	"""
	An editable B{single line} text edit widget.
	
	New Attributes
	==============
	
	  - text: The text in the TextBox.
	
	Data
	====
	The text can be read and set via L{distributeData} and L{collectData}.
	"""
	def __init__(self,text="", **kwargs):
		self.real_widget = fife.TextField()
		self.text = text
		super(TextField,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_data = True
		self.accepts_inital_data = True
		self._realSetInitialData = self._setText
		self._realSetData = self._setText
		self._realGetData = self._getText

	def resizeToContent(self,recurse=True):
		max_w = self.real_font.getWidth(self.text)
		self.width = max_w
		self.height = (self.real_font.getHeight() + 2)
	def _getText(self): return self.real_widget.getText()
	def _setText(self,text): self.real_widget.setText(text)
	text = property(_getText,_setText)

	def _setOpaque(self,opaque): self.real_widget.setOpaque(opaque)
	def _getOpaque(self): return self.real_widget.isOpaque()
	opaque = property(_getOpaque,_setOpaque)


# coding: utf-8

class ScrollArea(Widget):
	"""
	A wrapper around another (content) widget.
	
	New Attributes
	==============
	
	  - content: The wrapped widget.
	
	"""
	def __init__(self,**kwargs):
		self.real_widget = fife.ScrollArea()
		self._content = None
		super(ScrollArea,self).__init__(**kwargs)

	def _setContent(self,content):
		self.real_widget.setContent(content.real_widget)
		self._content = content
	def _getContent(self): return self._content
	content = property(_getContent,_setContent)

	def deepApply(self,visitorFunc):
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
	""" A spacer represents expandable 'whitespace' in the GUI.
	
	In a XML file you can get this by adding a <Spacer /> inside a VBox or
	HBox element (Windows implicitly are VBox elements).
	
	The effect is, that elements before the spacer will be left (top)
	and elements after the spacer will be right (bottom) aligned.
	
	There can only be one spacer in VBox (HBox).
	"""
	def __init__(self,parent=None,**kwargs):
		self._parent = parent

# Global Widget Class registry

WIDGETS = {
	# Containers
	"Container" : Container,
	"Window" : Window,
	"VBox" : VBox,
	"HBox" : HBox,
	"ScrollArea" :ScrollArea,
	
	# Simple Widgets
	"Icon" : Icon,
	"Label" : Label,
	"ClickLabel" : ClickLabel,
	
	# Button Widgets
	"Button" : Button,
	"CheckBox" : CheckBox,
	
	#Complexer Widgets / Text io
	"TextField" : TextField,
	"TextBox" : TextBox,
	"ListBox" : ListBox,
	"DropDown" : DropDown
}
