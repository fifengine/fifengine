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

from fife.extensions.pychan.widgets.common import *

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
	  - helptext: Unicode: Text which can be used for e.g. tooltips.
	  - border_size: Integer: The size of the border in pixels.
	  - position_technique: This can be either "automatic" or "explicit" - only L{Window} has this set to "automatic" which
	  results in new windows being centered on screen (for now).
	  If it is set to "explicit" the position attribute will not be touched.
	  - vexpand: Integer: >= 0. Proportion to expand this widget vertically.
	  - hexpand: Integer: >= 0. Proportion to expand this widget horizontally.

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
		Attr('style'), Attr('font'),IntAttr('border_size'),Attr('position_technique'),
		IntAttr('vexpand'),IntAttr('hexpand'),
		UnicodeAttr('helptext'), BoolAttr('is_focusable')
		]

	DEFAULT_NAME = '__unnamed__'
	DEFAULT_HEXPAND = 0
	DEFAULT_VEXPAND = 0
	DEFAULT_MAX_SIZE = 500000, 500000

	HIDE_SHOW_ERROR = """\
		You can only show/hide the top widget of a hierachy.
		Use 'addChild' or 'removeChild' to add/remove labels for example.
		"""


	def __init__(self,parent = None, name = DEFAULT_NAME,
				 size = (-1,-1), min_size=(0,0), max_size=DEFAULT_MAX_SIZE,
				 helptext=u"",
				 position = (0,0),
				 style = None, **kwargs):

		assert( hasattr(self,'real_widget') )
		self.event_mapper = events.EventMapper(self)
		self._visible = False
		self._extra_border = (0,0)
		self.hexpand = kwargs.get("hexpand",self.DEFAULT_HEXPAND)
		self.vexpand = kwargs.get("vexpand",self.DEFAULT_VEXPAND)
		# Simple way to get at least some compat layout:
		if get_manager().compat_layout:
			self.hexpand, self.vexpand = 0,0

		# Data distribution & retrieval settings
		self.accepts_data = False
		self.accepts_initial_data = False

		# Parent attribute makes sure we only have one parent,
		# that tests self.__parent - so make sure we have the attr here.
		self.__parent = None
		self.parent = parent

		self.has_name = False
		self.name = name

		self.position = position
		self.min_size = min_size
		self.max_size = max_size
		self.size = size
		self.position_technique = "explicit"
		self.font = 'default'

		# Inherit style
		if style is None and parent:
			style = parent.style
		self.style = style or "default"

		self.helptext = helptext
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
		if self.__parent:
			raise RuntimeError("You can only 'execute' root widgets, not %s!" % str(self))

		for name,returnValue in bind.items():
			def _quitThisDialog(returnValue = returnValue ):
				get_manager().breakFromMainLoop( returnValue )
				self.hide()
			self.findChild(name=name).capture( _quitThisDialog , group_name = "__execute__" )
		self.show()
		self.is_focusable = True
		self.requestFocus()
		return get_manager().mainLoop()

	def requestFocus(self):
		"""
		Requests focus.  
		
		The widget must be focusable in order for this to work.  See 
		the is_focusable property.
		
		"""
		if self.isVisible():
			self.real_widget.requestFocus()

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
		@paran group_name: Event group.

		Event groups are used to have different B{channels} which don't interfere with each other.
		For derived widgets that need to capture events it's advised to use the group_name 'widget'.
		The 'default' group is used by default, and should be reserved for the application programmers.
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
		if self.parent:
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
		if self.parent:
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
		while widget.parent:
			widget = widget.parent
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
		
		NOTE:
			- if your widget is a container, you have to call
			  _resetTiling(), as you will loose this call by using
			  your override method
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
		  buttons = root_widget.findChildren(__class__=pychan.widgets.Button)
		"""

		children = []
		def _childCollector(widget):
			if widget.match(**kwargs):
				children.append(widget)
		self.deepApply(_childCollector)
		return children

	def getNamedChildren(self, include_unnamed = False):
		"""
		Create a dictionary of child widgets with the keys being
		their name. This will contain only Widgets which have
		a name different from "__unnamed__" (which is the default).
		
		@param include_unnamed Defaults to false. If this is true unnamed widgets are added, too.
		
		The values are lists of widgets, so not only unique names
		are handled correctly.

		Usage::
			children = widget.getNamedChildren()
			for widget in children.get("info",[])
				print widget.name , " == info"
		"""
		children = {}
		if include_unnamed:
			def _childCollector(widget):
				children.setdefault(widget._name,[]).append(widget)
		else:
			def _childCollector(widget):
				if widget.has_name:
					children.setdefault(widget._name,[]).append(widget)
		self.deepApply(_childCollector)
		return children

	def findChild(self,**kwargs):
		""" Find the first contained child widgets by attribute values.

		Usage::
		  closeButton = root_widget.findChild(name='close')
		"""
		if kwargs.keys() == ["name"]:
			return self.findChildByName(kwargs["name"])

		children = self.findChildren(**kwargs)
		if children:
			return children[0]
		return None

	def findChildByName(self,name):
		"""
		Find first contained child widget by its name.

		Note that this is the fast version of findChild(name="...")
		and that you don't have to call this explicitly, it is used
		if possible.
		"""
		result = []
		def _childCollector(widget):
			if widget._name == name:
				result.append(widget)
				raise StopTreeWalking
		try:
			self.deepApply(_childCollector)
		except StopTreeWalking:
			return result[0]
		return None

	def addChild(self,widget):
		"""
		This function adds a widget as child widget and is only implemented
		in container widgets.

		You'll need to call L{adaptLayout} if the container is already shown,
		to adapt the layout to the new widget. This doesn't happen
		automatically.
		"""
		raise RuntimeError("Trying to add a widget to %s, which doesn't allow this." % repr(self))

	def insertChild(self, widget, position):
		"""
		This function inserts a widget a given index in the child list.
		
		See L{addChild} and L{insertChildBefore} 
		"""
		raise RuntimeError("Trying to insert a widget to %s, which doesn't allow this." % repr(self))
		
	def insertChildBefore(self, widget, before):
		"""
		Inserts a child widget before a given widget. If the widget isn't found,
		the widget is appended to the children list.
		
		See L{addChild} and L{insertChild} 
		"""
		raise RuntimeError("Trying to insert a widget to %s, which doesn't allow this." % repr(self))

	def addChildren(self,*widgets):
		"""
		Add multiple widgets as children.
		Only implemented for container widgets. See also L{addChild}

		Usage::
			container.addChildren( widget1, widget2, ... )
			# or you can use this on a list
			container.addChildren( [widget1,widget2,...] )
		"""
		if len(widgets) == 1 and not isinstance(widgets[0],Widget):
			widgets = widgets[0]
		for widget in widgets:
			self.addChild(widget)

	def removeChild(self,widget):
		"""
		This function removes a direct child widget and is only implemented
		in container widgets.

		You'll need to call L{adaptLayout} if the container is already shown,
		to adapt the layout to the removed widget. This doesn't happen
		automatically.
		"""
		raise RuntimeError("Trying to remove a widget from %s, which is not a container widget." % repr(self))

	def removeChildren(self,*widgets):
		"""
		Remove a list of direct child widgets.
		All widgets have to be direct child widgets.
		To 'clear' a container take a look at L{removeAllChildren}.
		See also L{removeChild}.

		Usage::
			container.removeChildren( widget1, widget2, ... )
			# or you can use this on a list
			container.removeChildren( [widget1,widget2,...] )
		"""
		if len(widgets) == 1 and not isinstance(widgets[0],Widget):
			widgets = widgets[0]
		for widget in widgets:
			self.removeChild(widget)

	def removeAllChildren(self):
		"""
		This function will remove all direct child widgets.
		This will work even for non-container widgets.
		"""
		children = self.findChildren(parent=self)
		for widget in children:
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

		The keys in the dictionary are parsed as C{"widgetName/eventName"} with the slash
		separating the two. If no slash is found the eventName is assumed to be "action".

		Additionally you can supply a group name or channel C{"widgetName/eventName/groupName"}.
		Event handlers from one group are not overridden by handlers from another group.
		The default group name is C{"default"}.

		Example::
			guiElement.mapEvents({
				"button" : guiElement.hide,
				"button/mouseEntered" : toggleButtonColorGreen,
				"button/mouseExited" :  toggleButtonColorBlue,
			})

		"""
		children = self.getNamedChildren(include_unnamed=True)
		for descr,func in eventMap.items():
			name, event_name, group_name = events.splitEventDescriptor(descr)
			#print name, event_name, group_name
			widgets = children.get(name,[])
			if widgets:
				for widget in widgets:
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
		children = self.getNamedChildren(include_unnamed=True)
		for name,data in initialDataMap.items():
			widgetList = children.get(name,[])
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
		children = self.getNamedChildren(include_unnamed=True)
		for name,data in dataMap.items():
			widgetList = children.get(name,[])
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
		children = self.getNamedChildren(include_unnamed=True)
		dataMap = {}
		for name in widgetNames:
			widgetList = children.get(name,[])
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
		children = self.getNamedChildren(include_unnamed=True)
		dataList = []
		for name in widgetNames:
			widgetList = children.get(name,[])
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
				print widget.name.ljust(20),repr(widget).ljust(50),repr(widget.__parent)
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
		self.deepApply(_callExpandContent, leaves_first=False)

	def deepApply(self,visitorFunc, leaves_first = True):
		"""
		Recursively apply a callable to all contained widgets and then the widget itself.
		"""
		visitorFunc(self)
		
	def getAbsolutePos(self):
		"""
		Get absolute position on screen
		"""
		absX = self.x
		absY = self.y
		parent = self.parent
		while parent is not None:
			absX += parent.x
			absY += parent.y
			parent = parent.parent
		return (absX, absY)

	def sizeChanged(self):
		pass

	def __str__(self):
		return "%s(name='%s')" % (self.__class__.__name__,self.name)

	def __repr__(self):
		return "<%s(name='%s') at %x>" % (self.__class__.__name__,self.name,id(self))

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

	def _setX(self,x):self.real_widget.setX(x)
	def _getX(self): return self.real_widget.getX()
	def _setY(self,y): self.real_widget.setY(y)
	def _getY(self): return self.real_widget.getY()

	def _setWidth(self,w):
		old_width = self.width
		w = max(self.min_size[0],w)
		w = min(self.max_size[0],w)
		self.real_widget.setWidth(w)
		if w != old_width:
			self.sizeChanged()

	def _getWidth(self): return self.real_widget.getWidth()
	def _setHeight(self,h):
		old_height = self.height
		h = max(self.min_size[1],h)
		h = min(self.max_size[1],h)
		self.real_widget.setHeight(h)
		if h != old_height:
			self.sizeChanged()

	def _getHeight(self): return self.real_widget.getHeight()

	def _getMinWidth(self): return self.min_size[0]
	def _getMaxWidth(self): return self.max_size[0]
	def _getMinHeight(self): return self.min_size[1]
	def _getMaxHeight(self): return self.max_size[1]
	def _setMinWidth(self,w):
		self.min_size = w, self.min_size[1]
	def _setMaxWidth(self,w):
		self.max_size = w, self.max_size[1]
	def _setMinHeight(self,h):
		self.min_size = self.min_size[0],h
	def _setMaxHeight(self,h):
		self.max_size = self.max_size[0],h

	def _setFont(self, font):
		self._font = font
		self.real_font = get_manager().getFont(font)
		self.real_widget.setFont(self.real_font)
	def _getFont(self):
		return self._font

	def _getBorderSize(self): return self.real_widget.getFrameSize()
	def _setBorderSize(self,size): self.real_widget.setFrameSize(size)

	base_color = ColorProperty("BaseColor")
	background_color = ColorProperty("BackgroundColor")
	foreground_color = ColorProperty("ForegroundColor")
	selection_color = ColorProperty("SelectionColor")

	def _getStyle(self): return self._style
	def _setStyle(self,style):
		self._style = style
		get_manager().stylize(self,style)
	style = property(_getStyle,_setStyle)

	def _getParent(self): return self.__parent
	def _setParent(self,parent):
		if self.__parent is not parent:
			if self.__parent and parent is not None:
				print "Widget containment fumble:", self, self.__parent, parent
				self.__parent.removeChild(self)
		self.__parent = parent
	parent = property(_getParent,_setParent)

	def _setName(self,name):
		self._name = name
		if name != Widget.DEFAULT_NAME:
			self.has_name = True
	def _getName(self):
		# __str__ relies on self.name
		return getattr(self,'_name','__no_name_yet__')
	name = property(_getName,_setName)

	def _setFocusable(self, b): self.real_widget.setFocusable(b)
	def _isFocusable(self):
		return self.real_widget.isFocusable()

	x = property(_getX,_setX)
	y = property(_getY,_setY)
	width = property(_getWidth,_setWidth)
	height = property(_getHeight,_setHeight)
	min_width = property(_getMinWidth,_setMinWidth)
	min_height = property(_getMinHeight,_setMinHeight)
	max_width = property(_getMaxWidth,_setMaxWidth)
	max_height = property(_getMaxHeight,_setMaxHeight)
	size = property(_getSize,_setSize)
	position = property(_getPosition,_setPosition)
	font = property(_getFont,_setFont)
	border_size = property(_getBorderSize,_setBorderSize)
	is_focusable = property(_isFocusable,_setFocusable) 

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

