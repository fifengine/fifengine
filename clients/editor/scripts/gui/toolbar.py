from pychan import widgets
import scripts.editor
import copy

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

class ToolbarAction:
	def __init__(self, action, widget):
		self.action = action
		self.widget = widget

class ToolBar(widgets.HBox):
	def __init__(self, button_style=0, orientation=0, *args, **kwargs):
		super(ToolBar, self).__init__(*args, **kwargs)
		#self.addChild(widgets.Label(text=u"Toolbar"))
		self._actions = []
		self._button_style = 0
		
		self.button_style = button_style
		

	def addSeparator(self, separator=None): 
		pass

	def addAction(self, action):
		if action is None:
			return
			
		widget = None
		icon = None
		text = None
		
		if self._button_style != BUTTON_STYLE['TextOnly']:
			if action.checkable:
				icon = widgets.ToggleButton(up_image=action.icon,down_image=action.icon,hover_image=action.icon,offset=(1,1),group="t")
			else:
				icon = widgets.ImageButton(up_image=action.icon,down_image=action.icon,hover_image=action.icon,offset=(1,1))
			icon.capture(action.activate)
			
		if self._button_style != BUTTON_STYLE['IconOnly']:
			text = widgets.Label(text=action.text)
			text.capture(action.activate, "mouseClicked")
		
		if self._button_style == BUTTON_STYLE['TextOnly']:
			widget = text
			widget.capture(action.activate, "mouseClicked")
			
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
		self.addChild(widget)
		toolbarAction = ToolbarAction(action, widget)
		self._actions.append(toolbarAction)
		
		eventMapper = scripts.editor.getEditor().getEventMapper()
		eventMapper.capture("toolbar_"+str(id(self)), "changed", self._actionChanged, sender=action)
		
	def removeAction(self, action):
		i = 0
		for a in self._actions:
			if a.action == action:
				self.removeWidget(action.widget)
				del self._actions[i]
				break
			i += 1
		
	def hasAction(self, action):
		for a in self._actions:
			if a.action == action:
				return True
		return False
		
	def _actionChanged(self, sender, event_name, *args, **kwargs):
		pass
		
	def _updateToolbar(self):
		actionlist = copy.copy(self._actions)
		self.clear()
		for action in actionlist:
			self.addAction(action.action)

		self.adaptLayout()
		
		
	def _updateAction(self, action):
		for a in self._actions:
			if a.action == action:
				pass
		
	def _setButtonStyle(self, button_style):
		self._button_style = BUTTON_STYLE['IconOnly']
		for key, val in BUTTON_STYLE.iteritems():
			if val == button_style:
				self._button_style = button_style
				break
		self._updateToolbar()
		
	def _getButtonStyle(self):
		return self._button_style
	button_style = property(_getButtonStyle, _setButtonStyle)
		
	def addActionGroup(self, actiongroup): 
		pass
		
	def insertAction(self, action, before): 
		pass
		
	def insertSeparator(self, separator, before): 
		pass
		
	def insertActionGroup(self, actiongroup, before): 
		pass

	def removeItem(self, item):
		pass
		
	def clear(self):
		self.removeAllChildren()
		self._actions = []
		pass
