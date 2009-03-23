# -*- coding: utf-8 -*-
### Widget/Container Base Classes ###

"""
Widget wrappers.

Please look at the documentation of L{Widget} for details.
"""

import fife, pythonize
import tools
import events
from exceptions import *
from attrs import Attr,UnicodeAttr, PointAttr,ColorAttr,BoolAttr,IntAttr,FloatAttr

def get_manager():
	import pychan
	return pychan.manager

def _text2gui(text):
	"""
	This function is applied to all text set on widgets.
	It replaces tabs by four spaces.
	It assumes the text to be a unicode object.
	"""
	if not isinstance(text,unicode):
		print "Widget text needs to be set from an unicode object. Got: '%s'" % repr(text)
		text = unicode(text,"utf8")
	return text.encode("utf8",*get_manager().unicodePolicy).replace("\t"," "*4).replace("[br]","\n")

def _gui2text(text):
	"""
	This function is applied to all text get from widgets.
	Translates the encoded string into a unicode object.
	"""
	return unicode(text,"utf8",*get_manager().unicodePolicy)


def isLayouted(widget):
	return isinstance(widget,LayoutBase)

class _DummyImage(object):
	def getWidth(self): return 0
	def getHeight(self): return 0

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
	  - selection_color: Color
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

	ATTRIBUTES = [ Attr('name'), PointAttr('position'),
		PointAttr('min_size'), PointAttr('size'), PointAttr('max_size'),
		ColorAttr('base_color'),ColorAttr('background_color'),ColorAttr('foreground_color'),ColorAttr('selection_color'),
		Attr('style'), Attr('font'),IntAttr('border_size')
		]

	DEFAULT_NAME = '__unnamed__'

	HIDE_SHOW_ERROR = """\
		You can only show/hide the top widget of a hierachy.
		Use 'addChild' or 'removeChild' to add/remove labels for example.
		"""

	def __init__(self,parent = None, name = DEFAULT_NAME,
				 size = (-1,-1), min_size=(0,0), max_size=(5000,5000),
				 style = None, **kwargs):

		assert( hasattr(self,'real_widget') )
		self.event_mapper = events.EventMapper(self)
		self._visible = False

		# Data distribution & retrieval settings
		self.accepts_data = False
		self.accepts_initial_data = False

		self.parent = parent

		# This will also set the _event_id and call real_widget.setActionEventId
		self.name = name

		self.min_size = min_size
		self.max_size = max_size
		self.size = size
		self.position_technique = "explicit"
		self.font = 'default'

		# Inherit style
		if style is None and parent:
			style = parent.style
		self.style = style or "default"

		# Not needed as attrib assignment will trigger manager.stylize call
		#manager.stylize(self,self.style)

	def execute(self,bind):
		"""
		Execute a dialog synchronously.

		As argument a dictionary mapping widget names to return values
		is expected. Events from these widgets will cause this function
		to return with the associated return value.

		This function will not return until such an event occurs.
		The widget will be shown before execution and hidden afterwards.
		You can only execute root widgets.

		Note: This feature is not tested well, and the API will probably
		change. Otherwise have fun::
		  # Okay this a very condensed example :-)
		  return pychan.loadXML("contents/gui/dialog.xml").execute({ 'okButton' : True, 'closeButton' : False })

		"""
		if not get_manager().can_execute:
			raise RuntimeError("Synchronous execution is not set up!")
		if self._parent:
			raise RuntimeError("You can only 'execute' root widgets, not %s!" % str(self))

		for name,returnValue in bind.items():
			def _quitThisDialog(returnValue = returnValue ):
				get_manager().breakFromMainLoop( returnValue )
				self.hide()
			self.findChild(name=name).capture( _quitThisDialog )
		self.show()
		return get_manager().mainLoop()

	def match(self,**kwargs):
		"""
		Matches the widget against a list of key-value pairs.
		Only if all keys are attributes and their value is the same it returns True.
		"""
		for k,v in kwargs.items():
			if v != getattr(self,k,None):
				return False
		return True

	def capture(self, callback, event_name="action", group_name="default"):
		"""
		Add a callback to be executed when the widget event occurs on this widget.

		The callback must be either a callable or None.
		The old event handler (if any) will be overridden by the callback.
		If None is given, the event will be disabled. You can query L{isCaptured}
		wether this widgets events are currently captured.

		It might be useful to check out L{tools.callbackWithArguments}.

		@param callback: Event callback - may accept keyword arguments event and widget.
		@paran event_name: The event to capture - may be one of L{events.EVENTS} and defaults to "action"
		"""
		self.event_mapper.capture( event_name, callback, group_name )

	def isCaptured(self):
		"""
		Check whether this widgets events are captured
		(a callback is installed) or not.
		"""
		return bool(self.event_mapper.getCapturedEvents())

	def show(self):
		"""
		Show the widget and all contained widgets.
		"""
		if self._parent:
			raise RuntimeError(Widget.HIDE_SHOW_ERROR)
		if self._visible: return
		self.adaptLayout()
		self.beforeShow()
		get_manager().show(self)
		self._visible = True

	def hide(self):
		"""
		Hide the widget and all contained widgets.
		"""
		if self._parent:
			raise RuntimeError(Widget.HIDE_SHOW_ERROR)
		if not self._visible: return

		get_manager().hide(self)

		self.afterHide()
		self._visible = False

	def isVisible(self):
		"""
		Check whether the widget is currently shown,
		either directly or as part of a container widget.
		"""
		widget = self
		while widget._parent:
			widget = widget._parent
		return widget._visible

	def adaptLayout(self,recurse=True):
		"""
		Execute the Layout engine. Automatically called by L{show}.
		In case you want to relayout a visible widget.
		This function will automatically perform the layout adaption
		from the top-most layouted widget.

		To make this clear consider this arrangement::
		      VBox 1
		      - Container
			- VBox 2
			  - HBox
			    - Label

		If you call adaptLayout on the Label the layout from the VBox 2
		will get recalculated, while the VBox 1 stays untouched.

		@param recurse Pass False here to force the layout to start from
		this widget.
		"""
		widget = self
		while widget.parent and recurse:
			if not isLayouted(widget.parent):
				break
			widget = widget.parent
		widget._recursiveResizeToContent()
		widget._recursiveExpandContent()

	def beforeShow(self):
		"""
		This method is called just before the widget is shown.
		You can override this in derived widgets to add finalization
		behaviour.
		"""

	def afterHide(self):
		"""
		This method is called just before the widget is hidden.
		You can override this in derived widgets to add finalization
		behaviour.
		"""

	def findChildren(self,**kwargs):
		"""
		Find all contained child widgets by attribute values.

		Usage::
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

		Usage::
		  closeButton = root_widget.findChild(name='close')
		"""
		children = self.findChildren(**kwargs)
		if children:
			return children[0]
		return None

	def addChild(self,widget):
		"""
		This function adds a widget as child widget and is only implemented
		in container widgets.
		"""
		raise RuntimeError("Trying to add a widget to %s, which doesn't allow this." % repr(self))

	def addChildren(self,*widgets):
		for widget in widgets:
			self.addChild(widget)

	def removeChild(self,widget):
		"""
		This function removes a direct child widget and is only implemented
		in container widgets.
		"""
		raise RuntimeError("Trying to remove a widget from %s, which is not a container widget." % repr(self))

	def removeChildren(self,*widgets):
		for widget in widgets:
			self.removeChild(widget)

	def mapEvents(self,eventMap,ignoreMissing = False):
		"""
		Convenience function to map widget events to functions
		in a batch.

		Subsequent calls of mapEvents will merge events with different
		widget names and override the previously set callback.
		You can also pass C{None} instead of a callback, which will
		disable the event completely.

		@param eventMap: A dictionary with widget/event names as keys and callbacks as values.
		@param ignoreMissing: Normally this method raises an RuntimeError, when a widget
		can not be found - this behaviour can be overriden by passing True here.

		The keys in the dictionary are parsed as "widgetName/eventName" with the slash
		separating the two. If no slash is found the eventName is assumed to be "action".

		Example::
		    guiElement.mapEvents({
			"button" : guiElement.hide,
			"button/mouseEntered" : toggleButtonColorGreen,
			"button/mouseExited" :  toggleButtonColorBlue,
		    })

		"""
		for descr,func in eventMap.items():
			name, event_name, group_name = events.splitEventDescriptor(descr)
			#print name, event_name, group_name
			widget = self.findChild(name=name)
			if widget:
				widget.capture( func, event_name = event_name, group_name = group_name )
			elif not ignoreMissing:
				raise RuntimeError("No widget with the name: %s" % name)

	def setInitialData(self,data):
		"""
		Set the initial data on a widget, what this means depends on the Widget.
		In case the widget does not accept initial data, a L{RuntimeError} is thrown.
		"""
		if not self.accepts_initial_data:
			raise RuntimeError("Trying to set data on a widget that does not accept initial data. Widget: %s Data: %s " % (repr(self),repr(data)))
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
				if get_manager().debug:
					self.listNamedWidgets()
				raise RuntimeError("DistributeData can only handle widgets with unique names.")
			widgetList[0].setData(data)

	def collectDataAsDict(self,widgetNames):
		"""
		Collect data from a widget hierachy by names into a dictionary.
		This can only handle UNIQUE widget names (in the hierachy)
		and will raise a RuntimeError if the number of matching widgets
		is not equal to one.

		Usage::
		  data = guiElement.collectDataAsDict(['myTextField','myListBox'])
		  print "You entered:",data['myTextField']," and selected ",data['myListBox']

		"""
		dataMap = {}
		for name in widgetNames:
			widgetList = self.findChildren(name = name)
			if len(widgetList) != 1:
				if get_manager().debug:
					self.listNamedWidgets()
				raise RuntimeError("CollectData can only handle widgets with unique names.")

			dataMap[name] = widgetList[0].getData()
		return dataMap

	def collectData(self,*widgetNames):
		"""
		Collect data from a widget hierachy by names.
		This can only handle UNIQUE widget names (in the hierachy)
		and will raise a RuntimeError if the number of matching widgets
		is not equal to one.

		This function takes an arbitrary number of widget names and
		returns a list of the collected data in the same order.

		In case only one argument is given, it will return just the
		data, with out putting it into a list.

		Usage::
		  # Multiple element extraction:
		  text, selected = guiElement.collectData('myTextField','myListBox')
		  print "You entered:",text," and selected item nr",selected
		  # Single elements are handled gracefully, too:
		  test = guiElement.collectData('testElement')

		"""
		dataList = []
		for name in widgetNames:
			widgetList = self.findChildren(name = name)
			if len(widgetList) != 1:
				if get_manager().debug:
					self.listNamedWidgets()
				raise RuntimeError("CollectData can only handle widgets with unique names.")
			dataList.append( widgetList[0].getData() )
		if len(dataList) == 1:
			return dataList[0]
		return dataList

	def listNamedWidgets(self):
		"""
		This function will print a list of all currently named child-widgets
		to the standard output. This is useful for debugging purposes.
		"""
		def _printNamedWidget(widget):
			if widget.name != Widget.DEFAULT_NAME:
				print widget.name.ljust(20),repr(widget).ljust(50),repr(widget._parent)
		print "Named child widgets of ",repr(self)
		print "name".ljust(20),"widget".ljust(50),"parent"
		self.deepApply(_printNamedWidget)

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

	def expandContent(self,recurse = True):
		"""
		Try to expand any spacer in the widget within the current size.
		Do not call directly.
		"""


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

	def __str__(self):
		return "%s(name='%s')" % (self.__class__.__name__,self.name)

	def __repr__(self):
		return "<%s(name='%s') at %x>" % (self.__class__.__name__,self.name,id(self))

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

	def _getBorderSize(self): return self.real_widget.getFrameSize()
	def _setBorderSize(self,size): self.real_widget.setFrameSize(size)

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

	def _getSelectionColor(self): return self.real_widget.getSelectionColor()
	def _setSelectionColor(self,color):
		if isinstance(color,type(())):
			color = fife.Color(*color)
		self.real_widget.setSelectionColor(color)
	selection_color = property(_getSelectionColor,_setSelectionColor)

	def _getStyle(self): return self._style
	def _setStyle(self,style):
		self._style = style
		get_manager().stylize(self,style)
	style = property(_getStyle,_setStyle)

	def _getParent(self): return self._parent
	def _setParent(self,parent):
		self._parent = parent
	parent = property(_getParent,_setParent)

	def _setName(self,name): self._name = name
	def _getName(self): return self._name
	name = property(_getName,_setName)

	x = property(_getX,_setX)
	y = property(_getY,_setY)
	width = property(_getWidth,_setWidth)
	height = property(_getHeight,_setHeight)
	size = property(_getSize,_setSize)
	position = property(_getPosition,_setPosition)
	font = property(_getFont,_setFont)
	border_size = property(_getBorderSize,_setBorderSize)

	def setEnterCallback(self, cb):
		"""
		*DEPRECATED*

		Callback is called when mouse enters the area of Widget
		callback should have form of function(button)
		"""
		if cb is None:
			self.capture(None, event_name = "mouseEntered" )
			return

		def callback(widget=None):
			return cb(widget)
		print "PyChan: You are using the DEPRECATED functionality: setEnterCallback."
		self.capture(callback, event_name = "mouseEntered" )

	def setExitCallback(self, cb):
		"""
		*DEPRECATED*

		Callback is called when mouse exits the area of Widget
		callback should have form of function(button)
		"""
		if cb is None:
			self.capture(None, event_name = "mouseExited" )
			return

		def callback(widget=None):
			return cb(widget)
		print "PyChan: You are using the DEPRECATED functionality: setExitCallback."
		self.capture(callback, event_name = "mouseExited" )



### Containers + Layout code ###

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

	Dynamic Layouting
	-----------------

	The layout is calculated in the L{Widget.show} method. Thus if you modify the layout,
	by adding or removing child widgets for example, you have to call L{Widget.adaptLayout}
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

	ATTRIBUTES = Container.ATTRIBUTES + [ UnicodeAttr('title'), IntAttr('titlebar_height') ]

	def __init__(self,title=u"title",titlebar_height=0,**kwargs):
		super(Window,self).__init__(_real_widget = fife.Window(), **kwargs)
		if titlebar_height == 0:
			titlebar_height = self.real_font.getHeight() + 4
		self.titlebar_height = titlebar_height
		self.title = title

		# Override explicit positioning
		self.position_technique = "automatic"


	def _getTitle(self): return _gui2text(self.real_widget.getCaption())
	def _setTitle(self,text): self.real_widget.setCaption(_text2gui(text))
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

	ATTRIBUTES = Widget.ATTRIBUTES + [UnicodeAttr('text')]

	def __init__(self, text = u"",**kwargs):
		self.margins = (5,5)
		self.text = text
		super(BasicTextWidget,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_initial_data = True
		self._realSetInitialData = self._setText

	def _getText(self): return _gui2text(self.real_widget.getCaption())
	def _setText(self,text): self.real_widget.setCaption(_text2gui(text))
	text = property(_getText,_setText)

	def resizeToContent(self, recurse = True):
		self.height = self.real_font.getHeight() + self.margins[1]*2
		self.width = self.real_font.getWidth(_text2gui(self.text)) + self.margins[0]*2

class Icon(Widget):
	"""
	An image icon.

	New Attributes
	==============

	  - image: String or GuiImage: The source location of the Image or a direct GuiImage
	"""
	ATTRIBUTES = Widget.ATTRIBUTES + [Attr('image')]

	def __init__(self,image="",**kwargs):
		self.real_widget = fife.Icon(None)
		super(Icon,self).__init__(**kwargs)
		self._source = self._image = None
		if image:
			self.image = image

	def _setImage(self,source):
		if isinstance(source,str):
			self._source = source
			self._image = get_manager().loadImage(source)
		elif isinstance(source,fife.GuiImage):
			self._source = None
			self._image = source
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))
		self.real_widget.setImage( self._image )

		# Set minimum size accoriding to image
		self.min_size = self.real_widget.getWidth(),self.real_widget.getHeight()
		self.size  = self.max_size = self.min_size

	def _getImage(self):
		if self._source is not None:
			return self._source
		return self._image
	image = property(_getImage,_setImage)

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

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [BoolAttr('wrap_text')]

	def __init__(self,wrap_text=False,**kwargs):
		self.real_widget = fife.Label("")
		self.wrap_text = wrap_text
		super(Label,self).__init__(**kwargs)

	def resizeToContent(self):
		self.real_widget.setWidth( self.max_size[0] )
		self.real_widget.adjustSize()
		self.height = self.real_widget.getHeight() + self.margins[1]*2
		self.width  = self.real_widget.getWidth()  + self.margins[0]*2
		#print self.width,self.max_size[0]

	def _setTextWrapping(self,wrapping): self.real_widget.setTextWrapping(wrapping)
	def _getTextWrapping(self): self.real_widget.isTextWrapping()
	wrap_text = property(_getTextWrapping,_setTextWrapping)

class ClickLabel(Label):
	"""
	Deprecated - use L{Label} instead.
	"""
	__init__ = tools.this_is_deprecated(Label.__init__,message = "ClickLabel - Use Label instead")


class Button(BasicTextWidget):
	"""
	A basic push button.
	"""
	def __init__(self,**kwargs):
		self.real_widget = fife.Button("")
		super(Button,self).__init__(**kwargs)

class ImageButton(BasicTextWidget):
	"""
	A basic push button with three different images for the up, down and hover state.

	B{Work in progress.}

	New Attributes
	==============

	  - up_image: String: The source location of the Image for the B{unpressed} state.
	  - down_image: String: The source location of the Image for the B{pressed} state.
	  - hover_image: String: The source location of the Image for the B{unpressed hovered} state.
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [Attr('up_image'),Attr('down_image'),PointAttr('offset'),UnicodeAttr('helptext'),Attr('hover_image')]

	def __init__(self,up_image="",down_image="",hover_image="",offset=(0,0),**kwargs):
		self.real_widget = fife.TwoButton()
		super(ImageButton,self).__init__(**kwargs)

		self.up_image = up_image
		self.down_image = down_image
		self.hover_image = hover_image
		self.offset = offset

	def _setUpImage(self, source):
		if isinstance(source,str):
			self._upimage_source = source
			try:
				self._upimage = get_manager().loadImage(source)
				self.real_widget.setUpImage( self._upimage )
			except:
				self._upimage = _DummyImage()
		elif isinstance(source,fife.GuiImage):
			self._upimage_source = None
			self._upimage = source
			self.real_widget.setUpImage( self._upimage )
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))

		# Set minimum size accoriding to image
		self.min_size = self.real_widget.getWidth(),self.real_widget.getHeight()
		self.size  = self.max_size = self.min_size

	def _getUpImage(self): return self._upimage_source
	up_image = property(_getUpImage,_setUpImage)

	def _setDownImage(self, source):
		if isinstance(source,str):
			self._downimage_source = source
			try:
				self._downimage = get_manager().loadImage(source)
				self.real_widget.setDownImage( self._downimage )
			except:
				self._downimage = _DummyImage()
		elif isinstance(source,fife.GuiImage):
			self._downimage_source = None
			self._downimage = source
			self.real_widget.setDownImage( self._downimage )
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))

	def _getDownImage(self): return self._downimage_source
	down_image = property(_getDownImage,_setDownImage)

	def _setHoverImage(self, source):
		if isinstance(source, str):
			self._hoverimage_source = source
			try:
				self._hoverimage = get_manager().loadImage(source)
				self.real_widget.setHoverImage( self._hoverimage )
			except:
				self._hoverimage = _DummyImage()
		elif isinstance(source,fife.GuiImage):
			self._hoverimage_source = None
			self._hoverimage = source
			self.real_widget.setHoverImage( self._hoverimage )
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))

	def _getHoverImage(self): return self._hoverimage_source
	hover_image = property(_getHoverImage,_setHoverImage)

	def _setOffset(self, offset):
		self.real_widget.setDownOffset(offset[0], offset[1])
	def _getOffset(self):
		return (self.real_widget.getDownXOffset(), self.real_widget.getDownYOffset())
	offset = property(_getOffset,_setOffset)

	def _setHelpText(self, txt):
		self.real_widget.setHelpText(txt)
	def _getHelpText(self):
		return self.real_widget.getHelpText()
	helptext = property(_getHelpText,_setHelpText)

	def resizeToContent(self):
		self.height = max(self._upimage.getHeight(),self._downimage.getHeight(),self._hoverimage.getHeight()) + self.margins[1]*2
		self.width = max(self._upimage.getWidth(),self._downimage.getWidth(),self._hoverimage.getWidth()) + self.margins[1]*2

class ToggleButton(BasicTextWidget):
	"""
	A basic push button that can be toggled.

	Unfortunately a bit of code duplication from ImageButton.

	New Attributes
	==============

	  - group: String: The group the button belongs to. Only one button in each group will be toggled at one time.
	  - toggled: Boolean: Whether the button is toggled or not.
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [
		Attr('up_image'),Attr('down_image'),Attr('hover_image'),
		PointAttr('offset'),
		UnicodeAttr('helptext'),Attr('group')
	]

	def __init__(self,up_image="",down_image="",hover_image="",offset=(0,0),group="",**kwargs):

		self.real_widget = fife.ToggleButton()
		super(ToggleButton,self).__init__(**kwargs)
		self.group = group
		self.up_image = up_image
		self.down_image = down_image
		self.hover_image = hover_image
		self.offset = offset

	def _setGroup(self,group):
		self.real_widget.setGroup( group )

	def _getGroup(self):
		return self.real_widget.getGroup()
	group = property(_getGroup,_setGroup)

	def _setToggled(self, toggled):
		self.real_widget.setToggled( toggled )

	def _isToggled(self):
		return self.real_widget.isToggled()
	toggled = property(_isToggled, _setToggled)

	###
	# I didn't want to do this, but this is just cut and paste from the ImageButton class:
	###

	def _setUpImage(self,image):
		self._upimage_source = image
		try:
			self._upimage = get_manager().loadImage(image)
			self.real_widget.setUpImage( self._upimage )
		except:
			self._upimage = _DummyImage()
	def _getUpImage(self): return self._upimage_source
	up_image = property(_getUpImage,_setUpImage)

	def _setDownImage(self,image):
		self._downimage_source = image
		try:
			self._downimage = get_manager().loadImage(image)
			self.real_widget.setDownImage( self._downimage )
		except:
			self._downimage = _DummyImage()
	def _getDownImage(self): return self._downimage_source
	down_image = property(_getDownImage,_setDownImage)

	def _setHoverImage(self,image):
		self._hoverimage_source = image
		try:
			self._hoverimage = get_manager().loadImage(image)
			self.real_widget.setHoverImage( self._hoverimage )
		except:
			self._hoverimage = _DummyImage()
	def _getHoverImage(self): return self._hoverimage_source
	hover_image = property(_getHoverImage,_setHoverImage)

	def _setOffset(self, offset):
		self.real_widget.setDownOffset(offset[0], offset[1])
	def _getOffset(self):
		return (self.real_widget.getDownXOffset(), self.real_widget.getDownYOffset())
	offset = property(_getOffset,_setOffset)

	def _setHelpText(self, txt):
		self.real_widget.setHelpText(_text2gui(txt))
	def _getHelpText(self):
		return _gui2text(self.real_widget.getHelpText())
	helptext = property(_getHelpText,_setHelpText)

	def resizeToContent(self):
		self.height = max(self._upimage.getHeight(),self._downimage.getHeight(),self._hoverimage.getHeight()) + self.margins[1]*2
		self.width = max(self._upimage.getWidth(),self._downimage.getWidth(),self._hoverimage.getWidth()) + self.margins[1]*2

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

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [BoolAttr('marked')]

	def __init__(self,**kwargs):
		self.real_widget = fife.CheckBox()
		super(CheckBox,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_data = True
		self._realGetData = self._isMarked
		self._realSetData = self._setMarked

		# Initial data stuff inherited.

	def _isMarked(self): return self.real_widget.isSelected()
	def _setMarked(self,mark): self.real_widget.setSelected(mark)
	marked = property(_isMarked,_setMarked)

class RadioButton(BasicTextWidget):
	"""
	A basic radiobutton (an exclusive checkbox).

	New Attributes
	==============

	  - marked: Boolean: Whether the checkbox is checked or not.
	  - group: String: All RadioButtons with the same group name
	  can only be checked exclusively.

	Data
	====
	The marked status can be read and set via L{distributeData} and L{collectData}
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [BoolAttr('marked'),Attr('group')]

	def __init__(self,group="_no_group_",**kwargs):
		self.real_widget = fife.RadioButton()
		super(RadioButton,self).__init__(**kwargs)

		self.group = group

		# Prepare Data collection framework
		self.accepts_data = True
		self._realGetData = self._isMarked
		self._realSetData = self._setMarked

		# Initial data stuff inherited.

	def _isMarked(self): return self.real_widget.isSelected()
	def _setMarked(self,mark): self.real_widget.setSelected(mark)
	marked = property(_isMarked,_setMarked)

	def _setGroup(self,group): self.real_widget.setGroup(group)
	def _getGroup(self): return self.real_widget.getGroup()
	group = property(_getGroup,_setGroup)

	def resizeToContent(self,recurse=True):
		self.width = self.real_font.getWidth(_text2gui(self.text)) + 35# Size of the Checked box?
		self.height = self.real_font.getHeight()

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

	ATTRIBUTES = Widget.ATTRIBUTES + [UnicodeAttr('text'),Attr('filename')]

	def __init__(self,text=u"",filename = "", **kwargs):
		self.real_widget = fife.TextBox()
		self.text = text
		self.filename = filename
		super(TextBox,self).__init__(**kwargs)

		# Prepare Data collection framework
		self.accepts_data = True
		self.accepts_initial_data = True # Make sense in a way ...
		self._realSetInitialData = self._setText
		self._realSetData = self._setText
		self._realGetData = self._getText

	def _getFileName(self): return self._filename
	def _loadFromFile(self,filename):
		self._filename = filename
		if not filename: return
		try:
			# FIXME needs encoding detection.
			self.text = unicode(open(filename).read(),"utf8")
		except Exception, e:
			self.text = str(e)
	filename = property(_getFileName, _loadFromFile)

	def resizeToContent(self,recurse=True):
		rows = [self.real_widget.getTextRow(i) for i in range(self.real_widget.getNumberOfRows())]
		max_w = max(map(self.real_font.getWidth,rows))
		self.width = max_w
		self.height = (self.real_font.getHeight() + 2) * self.real_widget.getNumberOfRows()

	def _getText(self): return _gui2text(self.real_widget.getText())
	def _setText(self,text): self.real_widget.setText(_text2gui(text))
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

	ATTRIBUTES = Widget.ATTRIBUTES + [UnicodeAttr('text')]

	def __init__(self,text=u"", **kwargs):
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
	def _setText(self,text): self.real_widget.setText(_text2gui(text))
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
	  - vertical_scrollbar: Boolean: Set this to False to hide the Vertical scrollbar
	  - horizontal_scrollbar: Boolean: Set this to False to hide the Horizontal scrollbar

	"""

	ATTRIBUTES = Widget.ATTRIBUTES + [ BoolAttr("vertical_scrollbar"),BoolAttr("horizontal_scrollbar") ]

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
		# Set a Container here, as this can not be set to None
		# Might be possible to find a better solution, needs peer review
		self.content = Container()
		widget.parent = None

	def _setContent(self,content):
		self.real_widget.setContent(content.real_widget)
		self._content = content
	def _getContent(self): return self._content
	content = property(_getContent,_setContent)

	def deepApply(self,visitorFunc):
		if self._content: self._content.deepApply(visitorFunc)
		visitorFunc(self)

	def resizeToContent(self,recurse=True):
		if self._content is None: return
		if recurse:
			self.content.resizeToContent(recurse=True)
		self.content.width = max(self.content.width,self.width-5)
		self.content.height = max(self.content.height,self.height-5)

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

	vertical_scrollbar = property(_getVerticalScrollbar,_setVerticalScrollbar)
	horizontal_scrollbar = property(_getHorizontalScrollbar,_setHorizontalScrollbar)

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

	def __str__(self):
		return "Spacer(parent.name='%s')" % getattr(self._parent,'name','None')

	def __repr__(self):
		return "<Spacer(parent.name='%s') at %x>" % (getattr(self._parent,'name','None'),id(self))

class Slider(Widget):
	""" A slider widget

	Use a callback to read out the slider value every time the marker
	is moved.

	New Attributes
	==============

	  - orientation: 1 = horizontal, 0=vertical
	  - scale_start: float: default 0.0
	  - scale_end: float: default 1.0

	FIXME:
		- set new attributes for marker & step length, value
		- update docstrings
	"""

	HORIZONTAL = fife.Slider.HORIZONTAL
	VERTICAL = fife.Slider.VERTICAL

	ATTRIBUTES = Widget.ATTRIBUTES + [IntAttr('orientation'), FloatAttr('scale_start'), FloatAttr('scale_end')]

	def __init__(self, scaleStart=0.0, scaleEnd=1.0, orientation=HORIZONTAL, **kwargs):
		self.real_widget = fife.Slider(scaleStart, scaleEnd)
		self.orientation = orientation
		self.setOrientation(self.orientation)
		super(Slider, self).__init__(**kwargs)

	def _setScale(self, start, end):
		"""setScale(self, double scaleStart, double scaleEnd)"""
		if type(start) != float:
			raise RuntimeError("Slider expects float for start scale")
		if type(end) != float:
			raise RuntimeError("Slider expects float for end scale")
		self.real_widget.setScale(start, end)

	def getScaleStart(self):
		"""getScaleStart(self) -> double"""
		return self.real_widget.getScaleStart()

	def setScaleStart(self, start):
		"""setScaleStart(self, double scaleStart)"""
		if type(start) != float:
			raise RuntimeError("Slider expects float for start scale")
		self.real_widget.setScaleStart(start)
	scale_start = property(getScaleStart, setScaleStart)

	def getScaleEnd(self):
		"""getScaleEnd(self) -> double"""
		return self.real_widget.getScaleEnd()

	def setScaleEnd(self, end):
		"""setScaleEnd(self, double scaleEnd)"""
		if type(end) != float:
			raise RuntimeError("Slider expects float for end scale")
		self.real_widget.setScaleEnd(end)
	scale_end = property(getScaleEnd, setScaleEnd)

	def getValue(self):
		"""getValue(self) -> double"""
		return self.real_widget.getValue()

	def setValue(self, value):
		"""setValue(self, double value)"""
		if type(value) != float:
			raise RuntimeError("Slider only accepts float values")
		self.real_widget.setValue(value)

	def setMarkerLength(self, length):
		"""setMarkerLength(self, int length)"""
		if type(length) != int:
			raise RuntimeError("Slider only accepts int for Marker lenght")
		self.real_widget.setMarkerLength(length)

	def getMarkerLength(self):
		"""getMarkerLength(self) -> int"""
		return self.real_widget.getMarkerLength()

	def setOrientation(self, orientation):
		"""setOrientation(self, Orientation orientation)"""
		self.real_widget.setOrientation(orientation)

	def getOrientation(self):
		"""getOrientation(self) -> int"""
		return self.real_widget.getOrientation()
	orientation = property(getOrientation, setOrientation)

	def setStepLength(self, length):
		"""setStepLength(self, double length)"""
		if type(length) != float:
			raise RuntimeError("Slider only accepts floats for step length")
		self.real_widget.setStepLength(length)

	def getStepLength(self):
		"""getStepLength(self) -> double"""
		return self.real_widget.getStepLength()

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
	"RadioButton" : RadioButton,
	"ImageButton" : ImageButton,
	"ToggleButton" : ToggleButton,

	#Complexer Widgets / Text io
	"TextField" : TextField,
	"TextBox" : TextBox,
	"ListBox" : ListBox,
	"DropDown" : DropDown,
	"Slider" : Slider
}

def registerWidget(cls):
	"""
	Register a new Widget class for pychan.
	"""
	global WIDGETS
	name = cls.__name__
	if name in WIDGETS:
		raise InitializationError("Widget class name '%s' already registered." % name)
	WIDGETS[name] = cls
