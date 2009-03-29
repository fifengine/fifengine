from pychan import widgets
import scripts.editor
import copy

from action import Action, ActionGroup
from fife import Color

# Orientation isn't implemented yet
ORIENTATION = {
			"Horizontal"	: 0,
			"Vertical"		: 1
		}

BUTTON_STYLE = {
			"IconOnly"			: 0,
			"TextOnly"			: 1,
			"TextUnderIcon"		: 2,
			"TextBesideIcon"	: 3
			}

class ToolbarButton(widgets.VBox):
	def __init__(self, action, button_style=0, **kwargs):
		self._action = action
		self._widget = None

		super(ToolbarButton, self).__init__(**kwargs)
		
		self.setButtonStyle(button_style)
		self.update()
		
		# Register new eventlistener
		eventMapper = scripts.editor.getEditor().getEventMapper()
		eventMapper.capture("toolbutton_"+str(id(self)), "changed", self.update, sender=action)
		
	def setAction(self, action):
		# Remove eventlistener for old action
		eventMapper = scripts.editor.getEditor().getEventMapper()
		eventMapper.capture("toolbutton_"+str(id(self)), "changed", None, sender=self._action)
		
		self._action = action
		self.update()
		
		# Register new eventlistener
		eventMapper.capture("toolbutton_"+str(id(self)), "changed", self.update, sender=action)
	
	def getAction(self):
		return self._action
	action = property(getAction, setAction)
	
	def setButtonStyle(self, button_style):
		self._button_style = BUTTON_STYLE['IconOnly']
		for key, val in BUTTON_STYLE.iteritems():
			if val == button_style:
				self._button_style = button_style
				break
		self.update()
		
	def getButtonStyle(self):
		return self._button_style
	button_style = property(getButtonStyle, setButtonStyle)
		
	def update(self):
		if self._widget != None:
			self.removeChild(self._widget)
			self._widget = None
			
		if self._action is None:
			return
			
		widget = None
		icon = None
		text = None

		if self._action.isSeparator():
			widget = widgets.VBox()
			widget.base_color += Color(8, 8, 8)
			widget.min_size = (2, 2)
		else:
			if self._button_style != BUTTON_STYLE['TextOnly']:
				if self._action.isCheckable():
					icon = widgets.ToggleButton(up_image=self._action.icon,down_image=self._action.icon,hover_image=self._action.icon,offset=(1,1))
					icon.toggled = self._action.isChecked()
				else:
					icon = widgets.ImageButton(up_image=self._action.icon,down_image=self._action.icon,hover_image=self._action.icon,offset=(1,1))
				icon.capture(self._action.activate)
				
			if self._button_style != BUTTON_STYLE['IconOnly']:
				text = widgets.Label(text=self._action.text)
				text.capture(self._action.activate, "mouseClicked")
			
			if self._button_style == BUTTON_STYLE['TextOnly']:
				widget = text
				widget.capture(self._action.activate, "mouseClicked")
				
			elif self._button_style == BUTTON_STYLE['TextUnderIcon']:
				widget = widgets.VBox()
				icon.position_technique = "center:top"
				text.position_technique = "center:bottom"
				widget.addChild(icon)
				widget.addChild(text)
				
			elif self._button_style == BUTTON_STYLE['TextBesideIcon']:
				widget = widgets.HBox()
				widget.addChild(icon)
				widget.addChild(text)
					
			else:
				widget = icon
			
		widget.position_technique = "left:center"
		widget.hexpand = 0
		
		self._widget = widget
		self.addChild(self._widget)
		self.adaptLayout()
		

class ToolBar(widgets.Window):
	def __init__(self, button_style=0, auto_expand=True, panel_size=30, orientation=1, *args, **kwargs):
		super(ToolBar, self).__init__(*args, **kwargs)
		
		self._actions = []
		self._button_style = 0
		self._panel_size = panel_size
		self.gui = None
		self._auto_expand = auto_expand 
		
		self._orientation = orientation
		self._button_style = button_style
		
		self._titlebarheight = 16

		self.unDock()
		self._updateToolbar()
		

	def addSeparator(self, separator=None): 
		self.insertSeparator(separator, len(self._actions))

	def addAction(self, action):
		self.insertAction(action, len(self._actions))
		
	def removeAction(self, action):
		if self.hasAction(action) is False:
			print "Tried to remove an action, which is not in the toolbar."
			return
		
		for a in self._actions:
			if a.action == action:
				self.gui.removeChild(a)
				self._actions.remove(a)
				break
		self.adaptLayout()
		
	def hasAction(self, action):
		for a in self._actions:
			if a.action == action:
				return True
		return False
		
	def addActionGroup(self, actiongroup): 
		self.insertActionGroup(actiongroup, len(self._actions))
		
	def insertAction(self, action, position=0, before=None):
		if self.hasAction(action):
			print "Action already added to toolbar"
			return
			
		button = ToolbarButton(action, button_style=self._button_style)
		self._actions.append(button)
		
		if before is not None:
			for a in self._actions:
				if a.action == before:
					before = a
					break
			self.gui.insertChildBefore(button, before)
		else:
			self.gui.insertChild(button, position)
		
		
	def insertSeparator(self, separator=None, position=0, before=None): 
		if separator==None:
			separator = Action(separator=True)
		self.insertAction(separator, position, before)
		
	def insertActionGroup(self, actiongroup, position=0, before=None): 
		actions = actiongroup.getActions()

		for action in actions:
			self.insertAction(action, position, before)
		
	def clear(self):
		self.removeAllChildren()
		self._actions = []
		
	def setButtonStyle(self, button_style):
		self._button_style = BUTTON_STYLE['IconOnly']
		for key, val in BUTTON_STYLE.iteritems():
			if val == button_style:
				self._button_style = button_style
				break
		self._updateToolbar()
		
	def getButtonStyle(self):
		return self._button_style
	button_style = property(getButtonStyle, setButtonStyle)
		
	def _updateToolbar(self):
		actionlist = self._actions
		self.clear()
		
		if self._orientation == ORIENTATION['Vertical']:
			self.gui = widgets.VBox(min_size=(self._panel_size, self._panel_size))
		else:
			self.gui = widgets.HBox(min_size=(self._panel_size, self._panel_size))
		self.addChild(self.gui)
		
		
		for action in actionlist:
			self.addAction(action.action)

		self.setAutoExpand(self._auto_expand)
		self.adaptLayout()
		
	def setOrientation(self, orientation):
		if orientation == ORIENTATION['Vertical']:
			self._orientation = ORIENTATION['Vertical']
			self._max_size = (self._panel_size, 5000)
		else:
			self._orientation = ORIENTATION['Horizontal']
			self._max_size = (5000, self._panel_size)
		self._orientation = orientation
		
		self._updateToolbar()
		
	def getOrientation(self):
		return self._orientation
	orientation = property(getOrientation, setOrientation)
	
	def setAutoExpand(self, auto_expand):
		self._auto_expand = auto_expand
		
		self.hexpand = self.gui.hexpand = 0
		self.vexpand = self.gui.vexpand = 0
		if auto_expand:
			if self._orientation == ORIENTATION['Vertical']:
				self.vexpand = 1
			else:
				self.hexpand = 1
		
	def getAutoExpand(self):
		return self._auto_expand
		
	def setPanelSize(self, panel_size):
		self._panel_size = panel_size
		self.min_size = self.gui.min_size = (self._panel_size, self._panel_size)
		self.setOrientation(self._orientation)
		
	def getPanelSize(self):
		return self._panel_size
	panel_size = property(getPanelSize, setPanelSize)
	
	def dockTo(self, dockarea):
		print "Dockto", dockarea
		self._floating = False
		self.real_widget.setTitleBarHeight(0)
		
		if dockarea == "left" or dockarea == "right":
			self.orientation = ORIENTATION['Vertical']
		else:
			self.orientation = ORIENTATION['Horizontal']
		
		mainwindow = scripts.editor.getEditor()
		mainwindow.dockWidgetTo(self, dockarea)
	
	def unDock(self):
		self._floating = True
		
		if self.parent is not None:
			widgetParent = self.parent
			widgetParent.removeChild(self)
			widgetParent.adaptLayout()
			self.hide()
			
		self.position = (150,150)
		self.position_technique = "explicit"
		self.real_widget.setTitleBarHeight(self._titlebarheight)
		self.show()
		#self._updateToolbar()
		
	def isFloating(self):
		return self._floating