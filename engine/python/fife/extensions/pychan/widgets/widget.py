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

import weakref

from fife import fife
from fife import fifechan

from fife.extensions.pychan import events
from fife.extensions.pychan.attrs import (Attr, UnicodeAttr, PointAttr,
                                          ColorAttr, BoolAttr, IntAttr, IntListAttr)
from fife.extensions.pychan.exceptions import StopTreeWalking
from fife.extensions.pychan.properties import ColorProperty

from common import get_manager
from layout import isLayouted


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
	  - margins: Integer list: The margin clears an area around an element (outside the border) and is completely transparent.
	  - padding: Integer list: The padding clears an area around the content (inside the border) of the element.
	  - base_color: Color
	  - background_color: Color
	  - foreground_color: Color
	  - selection_color: Color
	  - border_color: Color
	  - outline_color: Color
	  - font: String: This should identify a font that was loaded via L{loadFonts} before.
	  - helptext: Unicode: Text which can be used for e.g. tooltips.
	  - comment: Unicode: Additional text stored by the widget.  Not used by PyChan directly. Can be used by the client for additional info about the widget.
	  - border_size: Integer: The size of the border in pixels.
	  - position_technique: This can be either "automatic" or "explicit" - only L{Window} has this set to "automatic" which
	  results in new windows being centered on screen (for now).
	  If it is set to "explicit" the position attribute will not be touched.
	  - vexpand: Bool: True. Proportion to expand this widget vertically.
	  - hexpand: Bool: True. Proportion to expand this widget horizontally.

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

	ATTRIBUTES = [ Attr('name'), 
				   PointAttr('position'),
				   PointAttr('min_size'), 
				   PointAttr('size'), 
				   PointAttr('max_size'),
				   PointAttr('fixed_size'),
				   IntListAttr('margins'),
				   IntListAttr('padding'),
				   ColorAttr('base_color'),
				   ColorAttr('background_color'),
				   ColorAttr('foreground_color'),
				   ColorAttr('selection_color'),
				   ColorAttr('border_color'),
				   ColorAttr('outline_color'),
				   Attr('style'), 
				   Attr('font'),
				   IntAttr('border_size'),
				   IntAttr('outline_size'),
				   Attr('position_technique'),
				   BoolAttr('vexpand'),
				   BoolAttr('hexpand'), 
				   UnicodeAttr('helptext'),
				   BoolAttr('is_focusable'), 
				   UnicodeAttr('comment')
		]

	DEFAULT_NAME = '__unnamed__'
	DEFAULT_HEXPAND = False
	DEFAULT_VEXPAND = False
	DEFAULT_MAX_SIZE = 500000, 500000
	DEFAULT_SIZE = -1, -1
	DEFAULT_MIN_SIZE = 0, 0
	DEFAULT_MARGINS = 0, 0
	DEFAULT_PADDING = 0
	DEFAULT_HELPTEXT = u""
	DEFAULT_POSITION = 0, 0
	DEFAULT_FONT = "default"
	DEFAULT_BORDER_SIZE = 0
	DEFAULT_OUTLINE_SIZE = 0
	DEFAULT_POSITION_TECHNIQUE = "explicit"
	DEFAULT_COMMENT = u""

	HIDE_SHOW_ERROR = """\
		You can only show/hide the top widget of a hierachy.
		Use 'addChild' or 'removeChild' to add/remove labels for example.
		"""

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
				 comment = None):
		
		# Make sure the real_widget has been created
		assert( hasattr(self,'real_widget') )

		self.event_mapper = events.EventMapper(self)

		# Flag to indicate if the Widget is added to the Manager
		self._added = False
		# Flag to indicate if the Widget is added to
		# the top Widget list of the Manager
		self._top_added = False
		# Only needed for tabs
		self.tab = None
		
		# Data distribution & retrieval settings
		self.accepts_data = False
		self.accepts_initial_data = False
		
		#set all defaults
		if get_manager().compat_layout:
			self.hexpand, self.vexpand = 0,0
		else:
			self.hexpand = self.DEFAULT_HEXPAND
			self.vexpand = self.DEFAULT_VEXPAND 		

		self.name = self.DEFAULT_NAME
		self.has_name = False
		self.position = self.DEFAULT_POSITION
		self.position_technique = self.DEFAULT_POSITION_TECHNIQUE
		self.font = self.DEFAULT_FONT
		self.min_size = self.DEFAULT_MIN_SIZE
		self.max_size = self.DEFAULT_MAX_SIZE
		self.size = self.DEFAULT_SIZE
		self.margins = self.DEFAULT_MARGINS
		self.padding = self.DEFAULT_PADDING
		self.border_size = self.DEFAULT_BORDER_SIZE
		self.outline_size = self.DEFAULT_OUTLINE_SIZE
		self.helptext = self.DEFAULT_HELPTEXT
		self.comment = self.DEFAULT_COMMENT
		self._usedPrefixes = []
	
		# Parent attribute makes sure we only have one parent,
		# that tests self.__parent - so make sure we have the attr here.
		self.__parent = None
		self.parent = parent

		# Inherit and apply style
		if style is None and parent:
			style = parent.style
		self.style = style or "default"
		
		# override everything style has set
		if vexpand is not None:	self.vexpand = vexpand
		if hexpand is not None: self.hexpand = hexpand
		if name is not None: 
			self.name = name
			self.has_name = True

		if position is not None: self.position = position
		if position_technique is not None: self.position_technique = position_technique
		if font is not None: self.font = font
		
		# only set this if it's provided
		if is_focusable is not None: self.is_focusable = is_focusable
		
		if min_size is not None: self.min_size = min_size
		if max_size is not None: self.max_size = max_size
		if size is not None: self.size = size
		if fixed_size is not None: self.fixed_size = fixed_size
		if margins is not None: self.margins = margins
		if padding is not None: self.padding = padding
		if border_size is not None: self.border_size = border_size
		if outline_size is not None: self.outline_size = outline_size
		
		if helptext is not None: self.helptext = helptext
		if comment is not None: self.comment = comment

		# these are set in the default style
		if base_color is not None: self.base_color = base_color
		if background_color is not None: self.background_color = background_color
		if foreground_color is not None: self.foreground_color = foreground_color
		if selection_color is not None: self.selection_color = selection_color
		if border_color is not None: self.border_color = border_color
		if outline_color is not None: self.outline_color = outline_color
		# add this widget to the manager
		get_manager().addWidget(self)
	
	def clone(self, prefix):
		"""
		Clones this widget.
		
		Concrete widgets should implement this one, if not, an exception should
		be raised.
		
		Prefix is used to create the name of the cloned widget.
		"""
		raise RuntimeError("No implementation of clone method for %s" % self.__class__)
			      

	def execute(self, bind, focus=None):
		"""
		Execute a dialog synchronously.

		As argument a dictionary mapping widget names to return values
		is expected. Events from these widgets will cause this function
		to return with the associated return value.

		This function will not return until such an event occurs.
		The widget will be shown before execution and hidden afterwards.
		You can only execute root widgets.

		@param focus: name of child widget which should have focus. Defaults to main widget.

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
		if focus and self.findChild(name=focus):
			self.findChild(name=focus).is_focusable = True
			self.findChild(name=focus).requestFocus()
		else:
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

	def isModalFocusable(self):
		"""
		Checks if a widget is modal focusable.
		True if no other widget has modal focus, false otherwise.
		"""
		return self.real_widget.isModalFocusable()

	def isModalFocused(self):
		"""
		Checks if the widget or it's parent has modal focus.
		"""
		return self.real_widget.isModalFocused()
	
	def requestModalFocus(self):
		"""
		Requests modal focus.
		When a widget has modal focus, only that widget and it's children may recieve input.
		
		The widget must be modal focusable in order for this to work. Therefore,
		no other widget should has modal focus.
		
		"""
		if self.isVisible():
			if self.isModalFocusable():
				self.real_widget.requestModalFocus()

	def releaseModalFocus(self):
		"""
		Releases modal focus.
		"""
		if self.isModalFocused():
			self.real_widget.releaseModalFocus()

	def isModalMouseInputFocusable(self):
		"""
		Checks if a widget is modal mouse input focusable.
		True if no other widget has modal mouse input focus, false otherwise.
		"""
		return self.real_widget.isModalMouseInputFocusable()

	def isModalMouseInputFocused(self):
		"""
		Checks if the widget or it's parent has modal mouse input focus.
		"""
		return self.real_widget.isModalMouseInputFocused()
	
	def requestModalMouseInputFocus(self):
		"""
		Requests modal mouse input focus.
		When a widget has modal input focus that widget will be the only widget
		receiving input even if the input occurs outside of the widget and
		no matter what the input is.
		
		The widget must be modal mouse input focusable in order for this to work.
		Therefore, no other widget should has modal input focus.
		
		"""
		if self.isVisible():
			if self.isModalMouseInputFocusable():
				self.real_widget.requestModalMouseInputFocus()

	def releaseModalMouseInputFocus(self):
		"""
		Releases modal mouse input focus.
		"""
		if self.isModalMouseInputFocused():
			self.real_widget.releaseModalMouseInputFocus()
			
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
		@param event_name: The event to capture - may be one of L{events.EVENTS} and defaults to "action"
		@param group_name: Event group.

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
		# add this widget to the manager
		if not self._added:
			get_manager().addWidget(self)
		if self.parent is None and not self._top_added:
			get_manager().addTopWidget(self)
		# add childs of this widget to the manager
		def _show(shown_widget):
			get_manager().addWidget(shown_widget)
		self.deepApply(_show)
		
		if self.isVisible() and self.isSetVisible():
			self.beforeShow()
			self.adaptLayout()
			if self.parent is None:
				get_manager().placeWidget(self, self.position_technique)
			return

		self.beforeShow()
		# Show real widget to distribute a widgetShown event.
		self.real_widget.setVisible(True)
		self.adaptLayout()
		if self.parent is None:
			get_manager().placeWidget(self, self.position_technique)

	def hide(self, free=False):
		"""
		Hide the widget and all contained widgets.
		"""
		# remove this widget from the manager
		if self._added:
			get_manager().removeWidget(self)
		if self.parent is None and self._top_added:
			get_manager().removeTopWidget(self)
		# remove childs of this widget from the manager
		def _hide(hidden_widget):
			get_manager().removeWidget(hidden_widget)
		self.deepApply(_hide)
		
		if not self.isVisible() and not self.isSetVisible():
			self.adaptLayout()
			self.afterHide()
			return

		# Hide real widget to distribute a widgetHidden event.
		self.real_widget.setVisible(False)

		if free:
			if self.parent:
				self.parent.removeChild(self)
			self.removeAllChildren()

		self.adaptLayout()
		self.afterHide()

	def isVisible(self):
		"""
		Check whether the widget is currently shown,
		either directly or as part of a container widget.
		"""
		return self.real_widget.isVisible()

	def isSetVisible(self):
		"""
		Check the real widget visible flag.
		It checks not if the widget is currently shown!
		This is needed e.g. if the parent is already hidden
		but we want to hide the child too.
		"""
		return self.real_widget.isSetVisible()
	
	def adaptLayout(self,recurse=True):
		"""
		Execute the Layout engine. Automatically called by L{show}.
		In case you want to relayout a visible widget.
		This function will automatically perform the layout adaption
		from the top-most layouted widget.

		To make this clear consider this arrangement::
			VBox 1
				- Container
			VBox 2
				- HBox
					- Label

		If you call adaptLayout on the Label the layout from the VBox 2
		will get recalculated, while the VBox 1 stays untouched.

		@param recurse: Pass False here to force the layout to start from
		this widget.
		"""
		self.real_widget.adaptLayout(recurse)

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
		
		@param include_unnamed: Defaults to false. If this is true unnamed widgets are added, too.
		
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
		@param ignoreMissing: Normally this method raises a RuntimeError, when a widget
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
		self.real_widget.resizeToContent(recurse)

	def expandContent(self,recurse = True):
		"""
		Try to expand any spacer in the widget within the current size.
		Do not call directly.
		"""
		self.real_widget.expandContent(recurse)

	def _recursiveResizeToContent(self):
		"""
		Recursively call L{resizeToContent}. Uses L{deepApply}.
		Do not call directly.
		"""
		self.real_widget.resizeToContent(True)

	def _recursiveExpandContent(self):
		"""
		Recursively call L{expandContent}. Uses L{deepApply}.
		Do not call directly.
		"""
		self.real_widget.expandContent(True)

	def deepApply(self,visitorFunc, leaves_first = True, shown_only = False):
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
		self.real_widget.setWidth(w)
		w = self.real_widget.getWidth()
		if w != old_width:
			self.sizeChanged()

	def _getWidth(self): return self.real_widget.getWidth()
	def _setHeight(self,h):
		old_height = self.height
		self.real_widget.setHeight(h)
		h = self.real_widget.getHeight()
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

	def _setMinSize(self, size):
		self.real_widget.setMinSize(fifechan.Size(size[0], size[1]))
	def _getMinSize(self):
		size = self.real_widget.getMinSize()
		return (size.getWidth(), size.getHeight())
	def _setMaxSize(self, size):
		self.real_widget.setMaxSize(fifechan.Size(size[0], size[1]))
	def _getMaxSize(self):
		size = self.real_widget.getMaxSize()
		return (size.getWidth(), size.getHeight())
	def _setFixedSize(self, size):
		self.real_widget.setFixedSize(fifechan.Size(size[0], size[1]))
	def _getFixedSize(self):
		size = self.real_widget.getFixedSize()
		return (size.getWidth(), size.getHeight())
	def isFixedSize(self):
		return self.real_widget.isFixedSize()
	
	def _setFont(self, font):
		self._font = font
		self.real_font = get_manager().getFont(font)
		self.real_widget.setFont(self.real_font)
	def _getFont(self):
		return self._font

	def _getOutlineSize(self):
		return self.real_widget.getOutlineSize()
	def _setOutlineSize(self, size):
		self.real_widget.setOutlineSize(size)
		
	def _getBorderSize(self):
		return self.real_widget.getBorderSize()
	def _setBorderSize(self, size):
		self.real_widget.setBorderSize(size)

	def _setMargins(self, margin):
		# Shorthand property
		if isinstance(margin, tuple) or isinstance(margin, list):
			if len(margin) is 4:
				# 0=top, 1=right, 2=bottom, 3=left
				self.real_widget.setMarginTop(margin[0])
				self.real_widget.setMarginRight(margin[1])
				self.real_widget.setMarginBottom(margin[2])
				self.real_widget.setMarginLeft(margin[3])
			elif len(margin) is 3:
				# 0=top, 1=right, 2=bottom, 1=left
				self.real_widget.setMarginTop(margin[0])
				self.real_widget.setMarginRight(margin[1])
				self.real_widget.setMarginBottom(margin[2])
				self.real_widget.setMarginLeft(margin[1])
			elif len(margin) is 2:
				# 0=top, 1=right, 0=bottom, 1=left
				self.real_widget.setMarginTop(margin[0])
				self.real_widget.setMarginRight(margin[1])
				self.real_widget.setMarginBottom(margin[0])
				self.real_widget.setMarginLeft(margin[1])
			elif len(margin) is 1:
				# 0=top, 0=right, 0=bottom, 0=left
				self.real_widget.setMargin(margin[0])
		else:
			self.real_widget.setMargin(margin)
	def _getMargins(self):
		return (self.real_widget.getMarginTop(), self.real_widget.getMarginRight(),
				self.real_widget.getMarginBottom(), self.real_widget.getMarginLeft())
	
	def _setPadding(self, padding):
		# Shorthand property
		if isinstance(padding, tuple) or isinstance(padding, list):
			if len(padding) is 4:
				# 0=top, 1=right, 2=bottom, 3=left
				self.real_widget.setPaddingTop(padding[0])
				self.real_widget.setPaddingRight(padding[1])
				self.real_widget.setPaddingBottom(padding[2])
				self.real_widget.setPaddingLeft(padding[3])
			elif len(padding) is 3:
				# 0=top, 1=right, 2=bottom, 1=left
				self.real_widget.setPaddingTop(padding[0])
				self.real_widget.setPaddingRight(padding[1])
				self.real_widget.setPaddingBottom(padding[2])
				self.real_widget.setPaddingLeft(padding[1])
			elif len(padding) is 2:
				# 0=top, 1=right, 0=bottom, 1=left
				self.real_widget.setPaddingTop(padding[0])
				self.real_widget.setPaddingRight(padding[1])
				self.real_widget.setPaddingBottom(padding[0])
				self.real_widget.setPaddingLeft(padding[1])
			elif len(padding) is 1:
				# 0=top, 0=right, 0=bottom, 0=left
				self.real_widget.setPadding(padding[0])
		else:
			self.real_widget.setPadding(padding)
	def _getPadding(self):
		return (self.real_widget.getPaddingTop(), self.real_widget.getPaddingRight(),
				self.real_widget.getPaddingBottom(), self.real_widget.getPaddingLeft())
	
	base_color = ColorProperty("BaseColor")
	background_color = ColorProperty("BackgroundColor")
	foreground_color = ColorProperty("ForegroundColor")
	selection_color = ColorProperty("SelectionColor")
	outline_color = ColorProperty("OutlineColor")
	border_color = ColorProperty("BorderColor")
	
	def _getStyle(self): return self._style
	def _setStyle(self,style):
		self._style = style
		get_manager().stylize(self,style)
	style = property(_getStyle,_setStyle)

	def _getParent(self):
		if self.__parent is not None:
			return self.__parent()
		return None
	def _setParent(self,parent):
		if parent and not issubclass(type(parent), Widget):
			raise RuntimeError("Parent must be subclass of the Widget type.")

		if self.__parent is not None and self.__parent() is not parent:
			if self.__parent() is not None and parent is not None:
				print "Widget containment fumble:", self, self.__parent, parent
				self.__parent().removeChild(self)
		if parent is not None:
			self.__parent = weakref.ref(parent)
		else:
			self.__parent = None
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

	def _setHExpand(self, expand):
		self.real_widget.setHorizontalExpand(expand)
	def _isHExpand(self):
		return self.real_widget.isHorizontalExpand()
	def _setVExpand(self, expand):
		self.real_widget.setVerticalExpand(expand)
	def _isVExpand(self):
		return self.real_widget.isVerticalExpand()
	
	def _createNameWithPrefix(self, prefix):
		
		if not isinstance(prefix, str):
			raise RuntimeError("Widget names should be prefixed with a string")
		
		if prefix in self._usedPrefixes:
			raise RuntimeError("Widget %s already cloned with prefix %s" % (self.name, prefix))
		
		if len(prefix) == 0:
			raise RuntimeError("New widget name cannot be created with an empty prefix")
		
		self._usedPrefixes.append(prefix)
		
		return prefix + self.name

	x = property(_getX,_setX)
	y = property(_getY,_setY)
	width = property(_getWidth,_setWidth)
	height = property(_getHeight,_setHeight)
	min_width = property(_getMinWidth,_setMinWidth)
	min_height = property(_getMinHeight,_setMinHeight)
	max_width = property(_getMaxWidth,_setMaxWidth)
	max_height = property(_getMaxHeight,_setMaxHeight)
	size = property(_getSize,_setSize)
	min_size = property(_getMinSize, _setMinSize)
	max_size = property(_getMaxSize, _setMaxSize)
	fixed_size = property(_getFixedSize, _setFixedSize)
	position = property(_getPosition,_setPosition)
	font = property(_getFont,_setFont)
	outline_size = property(_getOutlineSize,_setOutlineSize)
	border_size = property(_getBorderSize,_setBorderSize)
	is_focusable = property(_isFocusable,_setFocusable)
	margins = property(_getMargins, _setMargins)
	padding = property(_getPadding, _setPadding)
	is_focusable = property(_isFocusable,_setFocusable)
	hexpand = property(_isHExpand,_setHExpand)
	vexpand = property(_isVExpand,_setVExpand)
