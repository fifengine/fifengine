from scripts.events.signal import Signal
import pychan.internal

changed = Signal(providing_args=[])
toggled = Signal(providing_args=["toggled"])
activated = Signal(providing_args=[])
#triggered = Signal(providing_args=["action"])

class Action:
	def __init__(self, text="", icon="", separator=False, checkable=False, checked=False):
		self._separator = separator
		self._text = text
		self._icon = icon
		self._shortcut = ""
		self._helptext = ""
		self._enabled = True
		self._checked = checked
		self._checkable = checkable
	
	def __str__(self):
		return "%s(name='%s')" % (self.__class__.__name__,self.text)

	def __repr__(self):
		return "<%s(name='%s') at %x>" % (self.__class__.__name__,self.text,id(self))

	
	def activate(self):
		if self.isCheckable():
			self.setChecked(not self.isChecked())
		activated.send(sender=self)
		
	def _changed(self):
		changed.send(sender=self)

	def setSeparator(self, separator):
		self._separator = separator
		self._changed()
	def isSeparator(self): return self._separator

	def _setText(self, text): 
		self._text = text
		self._changed(self)
	def _getText(self): return self._text
	text = property(_getText, _setText)

	def _setIcon(self, icon):
		self._icon = icon
		self._changed()
	def _getIcon(self): return self._icon
	icon = property(_getIcon, _setIcon)

	def _setShortcut(self, keysequence): 
		self._shortcut = keysequence
		self._changed()
	def _getShortcut(self): return self._shortcut
	shortcut = property(_getShortcut, _setShortcut)

	def _setHelpText(self, helptext): 
		self._helptext = helptext
		self._changed()
	def _getHelpText(self): return self._helptext
	helptext = property(_getHelpText, _setHelpText)

	def setEnabled(self, enabled): 
		self._enabled = enabled
		self._changed()
		
	def isEnabled(self): 
		return self._enabled

	def setChecked(self, checked):
		self._checked = checked
		self._changed()
		toggled.send(sender=self, toggled=checked)

	def isChecked(self): 
		return self._checked

	def setCheckable(self, checkable): 
		self._checkable = checkable
		if self._checkable is False and self._checked is True:
			self.checked = False
			
		self._changed()
		
	def isCheckable(self):
		return self._checkable

class ActionGroup:
	def __init__(self, exclusive=False, name="actiongroup"):
		self._exclusive = exclusive
		self._enabled = True
		self._actions = []
		self.name = name
		
	def __str__(self):
		return "%s(name='%s')" % (self.__class__.__name__,self.name)

	def __repr__(self):
		return "<%s(name='%s') at %x>" % (self.__class__.__name__,self.name,id(self))


	def setEnabled(self, enabled): 
		self._enabled = enabled
		self._changed()
		
	def isEnabled(self): 
		return self._enabled

	def setExclusive(self, exclusive):
		self._exclusive = exclusive
		self._changed()
		
	def isExclusive(self):
		return self._exclusive

	def addAction(self, action):
		if self.hasAction(action):
			print "Actiongroup already has this action"
			return
		self._actions.append(action)
		toggled.connect(self._actionToggled, sender=action)
		self._changed()

	def addSeparator(self):
		separator = Action(separator=True)
		self.addAction(separator)
		self._changed()
	
	def getActions(self):
		return self._actions
	
	def removeAction(self, action):
		self._actions.remove(action)
		toggled.disconnect(self._actionToggled, sender=action)
		self._changed()
	
	def clear(self):
		for action in self._actions:
			toggled.disconnect(self._actionToggled, sender=action)
		self._actions = []
		self._changed()
			
	def hasAction(self, action):
		for a in self._actions:
			if a == action:
				return True
		return False
	
	def _actionToggled(self, sender):
		if sender.isChecked() is False or self._exclusive is False:
			return
			
		for a in self._actions:
			if a != sender and a.isChecked():
				a.setChecked(False)
				
	def getChecked(self):
		for a in self._actions:
			if a.isChecked():
				return a
			
		return None
				
	def _changed(self):
		changed.send(sender=self)

