#from editor import getEditor # Needed to quit application

import fife
from fife import IKeyListener, ICommandListener, IMouseListener, LayerChangeListener, MapChangeListener, ConsoleExecuter
import pychan

CALLBACK_NONE_MESSAGE = """\
You passed None as parameter to EventMapper, which would normally remove a mapped event.
But there was no event mapped. Did you accidently call a function instead of passing it?
Group name = %s
"""

class EventCallback:
	def __init__(self, callback, sender):
		self.callback = callback
		self.sender = sender

class EventMapper(IKeyListener, ICommandListener, IMouseListener, LayerChangeListener, MapChangeListener, ConsoleExecuter):
	def __init__(self, engine):
		self.engine = engine
		
		self.callbacks = {}
		
		eventmanager = self.engine.getEventManager()

		IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		
		ICommandListener.__init__(self)
		eventmanager.addCommandListener(self)
		
		IMouseListener.__init__(self)
		eventmanager.addMouseListener(self)
		
		ConsoleExecuter.__init__(self)
		self.engine.getGuiManager().getConsole().setConsoleExecuter(self)
		
		MapChangeListener.__init__(self)
		LayerChangeListener.__init__(self)
		

	# Since this EventMapper is shared by all modules in the editor, 
	# a groupname parameter must be supplied so that the event can be
	# removed by the object that created it later.
	#
	# By setting callback to None, the event will be removed.
	def capture(self, group_name, event_name, callback=None, sender=None ):
		if callback is None:
			if self.isCaptured(group_name, event_name):
				self._removeEvent(group_name, event_name)
			else:
				print CALLBACK_NONE_MESSAGE % str(group_name)
			return
		self._addEvent(group_name, event_name, callback, sender)
	
	def isCaptured(self, group_name, event_name):
		if self.callbacks.has_key(group_name):
			if self.callbacks[group_name].has_key(event_name):
				return True
		return False

	def _addEvent(self, group_name, event_name, callback, sender):
		# Set up callback dictionary. This should fix some GC issues
		if not self.callbacks.has_key(group_name):
			self.callbacks[group_name] = {}
			
		if not self.callbacks[group_name].has_key(event_name):
			self.callbacks[group_name][event_name] = {}
			
		self.callbacks[group_name][event_name] = EventCallback(callback, sender)

	def _removeEvent(self, groupname, event_name):
		del self.callbacks[group_name][event_name]
		if len(self.callbacks[group_name]) <= 0:
			del self.callbacks[group_name]

	def dispatchEvent(self, event_name, sender=None, *args, **kwargs):
		for group in self.callbacks:
			for event in self.callbacks[group]:
				if event == event_name:
					if self.callbacks[group][event].sender is None \
							or self.callbacks[group][event].sender == sender:
						pychan.tools.applyOnlySuitable(self.callbacks[group][event].callback, \
									event_name=event_name, group_name=group, sender=sender, *args, **kwargs)

	# addHotkey is similar to capture, except that it accepts a key 
	# sequence (e.g. "CTRL+X") instead of an event.
	def addHotkey(self, groupname, keysequence, callback=None): 
		pass
	
	#--- Listener methods ---#
	def onSave(self, map):
		self.dispatchEvent("onSave", map)
		
	def mapAdded(self, map):
		self.dispatchEvent("mapAdded", map)
		
	def mapRemoved(self):
		self.dispatchEvent("mapRemoved")
		
	def onQuit(self):
		self.dispatchEvent("onQuit")
	
	# IKeyListener
	def keyPressed(self, evt): 
		keyval = evt.getKey().getValue()
		if keyval == fife.Key.ESCAPE:
			self.onQuit()
		elif keyval == fife.Key.F10:
			self.engine.getGuiManager().getConsole().toggleShowHide()
		
		self.dispatchEvent("keyPressed", evt)
		
		evt.consume()

	def keyReleased(self, evt): self.dispatchEvent("keyReleased", evt)

	# ICommandListener
	def onCommand(self, command):
		if command.getCommandType() == fife.CMD_QUIT_GAME:
			self.onQuit()
		else:
			self.dispatchEvent("onCommand", command)
		

	# IMouseListener
	def mouseEntered(self, evt): self.dispatchEvent("mouseEntered", evt)
	def mouseExited(self, evt): self.dispatchEvent("mouseExited", evt)
	def mousePressed(self, evt): self.dispatchEvent("mousePressed", evt)
	def mouseReleased(self, evt): self.dispatchEvent("mouseReleased", evt)
	def mouseClicked(self, evt): self.dispatchEvent("mouseClicked", evt)
	def mouseWheelMovedUp(self, evt): self.dispatchEvent("mouseWheelMovedUp", evt)
	def mouseWheelMovedDown(self, evt): self.dispatchEvent("mouseWheelMovedDown", evt)
	def mouseMoved(self, evt): self.dispatchEvent("mouseMoved", evt)
	def mouseDragged(self, evt): self.dispatchEvent("mouseDragged", evt)

	# LayerChangeListener
	def onLayerChanged(self, layer, changedInstances): self.dispatchEvent("onLayerChanged", layer, changedInstances)
	def onInstanceCreate(self, layer, instance): self.dispatchEvent("onInstanceCreate", layer, instance)
	def onInstanceDelete(self, layer, instance): self.dispatchEvent("onInstanceDelete", layer, instance)

	# MapChangeListener
	def onMapChanged(self, map, changedLayers): self.dispatchEvent("onMapChanged", map, changedLayers)
	def onLayerCreate(self, map, layer): self.dispatchEvent("onLayerCreate", map, layer)
	def onLayerDelete(self, map, layer): self.dispatchEvent("onLayerDelete", map, layer)

	# ConsoleExecuter
	def onToolsClick(self): self.dispatchEvent("onConsoleCommand")
	def onConsoleCommand(self, command): self.dispatchEvent("onConsoleCommand", command)
