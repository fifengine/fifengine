import scripts.editor
import pychan.internal

class Action:
	def __init__(self, text="", icon="", separator=False):
		self._separator = separator
		self._text = text
		self._icon = icon
		self._shortcut = ""
		self._helptext = ""
		self._enabled = True
		self._checked = False
		self._checkable = False
		
	def activate(self):
		scripts.editor.getEditor().getEventMapper().dispatchEvent("activated", self)
		
	def _changed(self):
		scripts.editor.getEditor().getEventMapper().dispatchEvent("changed", self)

	def _setSeparator(self, separator): 
		self._separator = separator
		self._changed()
	def _isSeparator(self): return self._separator
	separator = property(_isSeparator, _setSeparator)

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

	def _setEnabled(self, enabled): 
		self._enabled = enabled
		self._changed()
	def _isEnabled(self): return self._enabled
	enabled = property(_isEnabled, _setEnabled)

	def _setChecked(self, checked): 
		self._checked = checked
		scripts.editor.getEditor().getEventMapper().dispatchEvent("toggled", sender=self, checked=checked)	
	def _isChecked(self): return self._checked
	checked = property(_isChecked, _setChecked)

	def _setCheckable(self, checkable): 
		self._checkable = checkable
		self._changed()
	def _isCheckable(self): return self._checkable
	checkable = property(_isCheckable, _setCheckable)

	# Emits signals:
	# changed()
	# toggled(bool)
	# activated()

class ActionGroup:
	def __init__(self):
		self._exclusive = True
		self._enabled = True

	def setEnabled(self, enabled): self._enabled = enabled
	def isEnabled(self): return self._enabled
	enabled = property(isEnabled, setEnabled)

	def setExclusive(self, exclusive):
		self._exclusive = exclusive
	def isExclusive(self):
		return self._exclusive

	def addAction(self, action):
		pass
	
	def removeAction(self, action):
		pass
	
	def clear(self):
		pass

	# Emits signals:
	# triggered( Action action )
