import pychan
from pychan import widgets

import scripts.events
import action
import scripts.editor
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

class ToolBar(widgets.Window):
	def __init__(self, button_style=0, auto_expand=True, panel_size=30, orientation=0, *args, **kwargs):
		super(ToolBar, self).__init__(*args, **kwargs)
		
		self._actions = []
		self._actionbuttons = []
		self._button_style = 0
		self._panel_size = panel_size
		self.gui = None
		self._auto_expand = auto_expand 
		self._floating = True
		
		self._orientation = orientation
		self._button_style = button_style

		self._titlebarheight = 16
		
		self._updateToolbar()
		
		self.capture(self.mouseReleased, "mouseReleased", "widget")
		self.capture(self.mouseClicked, "mouseClicked", "widget")

	def addSeparator(self, separator=None): 
		self.insertSeparator(separator, len(self._actions))

	def addAction(self, action):
		self.insertAction(action, len(self._actions))
		
	def removeAction(self, action):
		self._actions.remove(action)
		
		actions = [action]
		if isinstance(action, ActionGroup):
			actions = action.getActions()
			scripts.gui.action.changed.disconnect(self._updateActionGroup, sender=action)

		for a in actions:
			for b in self._actionbuttons[:]:
				if a == b.action:
					self.gui.removeChild(b)
					self._actionbuttons.remove(b)
			
		self.adaptLayout()
		
	def hasAction(self, action):
		for a in self._actions:
			if a == action: return True
		return False
		
	def insertAction(self, action, position=0, before=None):
		if self.hasAction(action):
			print "Action already added to toolbar"
			return

		if before is not None:
			position = self._actions.index(before)

		self._actions.insert(position, action)
		self._insertButton(action, position)
		
	def _updateActionGroup(self, sender):
		position = self._actions.index(sender)
		self.removeAction(sender)
		self.insertAction(sender, position)
		self.adaptLayout()
		
	def _insertButton(self, action, position):
		actions = [action]
		if isinstance(action, ActionGroup):
			actions = action.getActions()
			scripts.gui.action.changed.connect(self._updateActionGroup, sender=action)

		if position >= 0:
			actions = reversed(actions)
		
		# Action groups are counted as one action, add the hidde number of actions to position
		for i in range(position):
			if isinstance(self._actions[i], ActionGroup):
				position += len(self._actions[i].getActions()) - 1

		for a in actions:
			button = ToolbarButton(a, button_style=self._button_style, name=a.text)
			self.gui.insertChild(button, position)
			self._actionbuttons.insert(position, button)
		
	def insertSeparator(self, separator=None, position=0, before=None): 
		if separator==None:
			separator = Action(separator=True)
		self.insertAction(separator, position, before)
		
	def clear(self):
		self.removeAllChildren()
		self._actions = []
		
		for i in reversed(range(len(self._actionbuttons))):
			self._actionbuttons[i].removeEvents()
		self._actionbuttons = []
		
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
		actions = self._actions
		
		self.clear()
		
		if self._orientation == ORIENTATION['Vertical']:
			self.gui = widgets.VBox(min_size=(self._panel_size, self._panel_size))
		else:
			self.gui = widgets.HBox(min_size=(self._panel_size, self._panel_size))
		self.addChild(self.gui)
		
		for action in actions:
			self.addAction(action)

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
	
	def setDocked(self, docked):
		if docked is True and self._floating == True:
				self._floating = False
				self.real_widget.setTitleBarHeight(0)
				self.real_widget.setMovable(False)
				
		elif docked is False and self._floating is False:			
			self._floating = True
			self.real_widget.setMovable(True)
			
			if self.parent is not None:
				widgetParent = self.parent
				widgetParent.removeChild(self)
				widgetParent.adaptLayout()
				self.hide()
				
			self.real_widget.setTitleBarHeight(self._titlebarheight)
			self.show()
		
	def isFloating(self):
		return self._floating
		
	# Drag and drop docking
	def mouseReleased(self, event):
		if self.isFloating() is False:
			return
			
		editor = scripts.editor.getEditor()
		if self.x + event.getX() < 25:
			self.setDocked(True)
			self.setOrientation(ORIENTATION["Vertical"])
			editor.dockWidgetTo(self, "left")
			
		elif self.x + event.getX() > pychan.internal.screen_width() - 25:
			self.setDocked(True)
			self.setOrientation(ORIENTATION["Vertical"])
			editor.dockWidgetTo(self, "right")
		elif self.y + event.getY() < 50:
			self.setDocked(True)
			self.setOrientation(ORIENTATION["Horizontal"])
			editor.dockWidgetTo(self, "top")
		elif self.y + event.getY() > pychan.internal.screen_height() - 50:
			self.setDocked(True)
			self.setOrientation(ORIENTATION["Horizontal"])
			editor.dockWidgetTo(self, "bottom")
			
	def mouseClicked(self, event):
		if event.getButton() == 2: # Right click
			self.setDocked(False)
			
			
class ToolbarButton(widgets.VBox):
	def __init__(self, action, button_style=0, **kwargs):
		self._action = action
		self._widget = None
		
		super(ToolbarButton, self).__init__(**kwargs)
		
		self.setButtonStyle(button_style)
		self.update()

		self.initEvents()
	
	def initEvents(self):
		# Register eventlisteners
		self.capture(self._showTooltip, "mouseEntered")
		self.capture(self._hideTooltip, "mouseExited")
		
		scripts.gui.action.changed.connect(self._actionChanged, sender=self._action)
	
	def removeEvents(self):
		# Remove eventlisteners
		self.capture(None, "mouseEntered")
		self.capture(None, "mouseExited")
		
		scripts.gui.action.changed.disconnect(self.update, sender=self._action)
	
	def setAction(self, action):
		self.removeEvents()
		
		self._action = action
		self.update()
		self.adaptLayout()
		
		self.initEvents()
	
	def getAction(self):
		return self._action
	action = property(getAction, setAction)
	
	def setButtonStyle(self, button_style):
		self._button_style = BUTTON_STYLE['IconOnly']
		for key, val in BUTTON_STYLE.iteritems():
			if val == button_style:
				self._button_style = button_style
				break
		
	def getButtonStyle(self):
		return self._button_style
	button_style = property(getButtonStyle, setButtonStyle)
	
	def _showTooltip(self):
		if self._action is not None and self._action.helptext != "":
			scripts.editor.getEditor().getStatusBar().showTooltip(self._action.helptext)
			
	def _hideTooltip(self):
		scripts.editor.getEditor().getStatusBar().hideTooltip()
		
	def _actionChanged(self):
		self.update()
		self.adaptLayout()
		
	def update(self):
		""" Sets up the button widget """
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
			if self._button_style != BUTTON_STYLE['TextOnly'] and len(self._action.icon) > 0:
				if self._action.isCheckable():
					icon = widgets.ToggleButton(hexpand=0, up_image=self._action.icon,down_image=self._action.icon,hover_image=self._action.icon,offset=(1,1))
					icon.toggled = self._action.isChecked()
				else:
					icon = widgets.ImageButton(hexpand=0, up_image=self._action.icon,down_image=self._action.icon,hover_image=self._action.icon,offset=(1,1))
				icon.capture(self._action.activate)
				
			if self._button_style != BUTTON_STYLE['IconOnly'] or len(self._action.icon) <= 0:
				if self._action.isCheckable():
					text = widgets.ToggleButton(hexpand=0, text=self._action.text,offset=(1,1))
					text.toggled = self._action.isChecked()
				else:
					text = widgets.Button(text=self._action.text)
				text.capture(self._action.activate)
			
			if self._button_style == BUTTON_STYLE['TextOnly'] or len(self._action.icon) <= 0:
				widget = text
				
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
		