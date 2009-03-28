from pychan import widgets
import scripts.editor

class ToolbarAction:
	def __init__(self, action, widget):
		self.action = action
		self.widget = widget

class ToolBar(widgets.HBox):
	def __init__(self, *args, **kwargs):
		super(ToolBar, self).__init__(*args, **kwargs)
		#self.addChild(widgets.Label(text=u"Toolbar"))
		self._actions = []

	def addSeparator(self, separator=None): 
		pass

	def addAction(self, action):
		if action is None:
			return
			
		widget = None
		if action.checkable:
			widget = widgets.ToggleButton(up_image=action.icon,down_image=action.icon,hover_image=action.icon,offset=(1,1),group="t")
		else:
			widget = widgets.ImageButton(up_image=action.icon,down_image=action.icon,hover_image=action.icon,offset=(1,1))
			
		widget.capture(action.activate)
		self.addChild(widget)
		toolbarAction = ToolbarAction(action, widget)
		self._actions.append(toolbarAction)
		eventMapper = scripts.editor.getEditor().getEventMapper()
		eventMapper.capture("toolbar_"+str(id(self)), "changed", self._actionChanged, sender=action)
		
	def _actionChanged(self, sender, event_name, *args, **kwargs):
		pass
		
	def _updateToolbar(self):
		actionlist = self._actions
		self.clear()
		for action in actionlist:
			self.addAction(action)
		
	def _updateAction(self, action):
		for a in self._actions:
			if a.action == action:
				pass
		
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
