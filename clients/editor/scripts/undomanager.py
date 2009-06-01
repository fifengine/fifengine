from events.signal import Signal
import collections
import pdb

actionAdded = Signal(providing_args=["action"])
preUndo = Signal()
postUndo = Signal()
preRedo = Signal()
postRedo = Signal()
cleared = Signal()
modeChanged = Signal(providing_args=["mode"])
changed = Signal()

# Fixme: Sometimes undoing/redoing many items at the same time ( undomanager.undo(n) )
#        causes too many tiles to be placed.
class UndoManager:
	# Methods
	def __init__(self, branchedMode = False, max_history=10000):
		self._max_history = max_history
		# Should we use deque, instead of lists?
		self._undostack = collections.deque()
		self._groups = []
		self._branched_mode = False
		
		def warn(msg):
			print "Warning: ",msg
		self.first_item = UndoStackItem(UndoObject(None, None))
		self.first_item.object.name = "First item"
		self.first_item.object.description = "First item in stack. Placeholder"
		self.first_item.object.undoCallback = lambda: warn("Tried to undo first item")
		self.first_item.object.redoCallback = lambda: warn("Tried to redo first item")
		
		self.current_item = self.first_item

	def getUndoStack(self):
		return self._undostack

	def startGroup(self, name="", description="", icon=""): 
		undogroup = UndoGroup(name, description, icon)
		self._groups.append(undogroup)
		return undogroup
	
	def endGroup(self):
		if len(self._groups) <= 0:
			print "Warning: UndoManager: No groups to end!"
			return
			
		group = self._groups.pop()
		self.addAction(group)

	def addAction(self, action):
		if len(self._groups) > 0:
			self._groups[len(self._groups)-1].addObject(action)
		else:
			stackitem = UndoStackItem(action)
			
			stackitem.previous = self.current_item
			stackitem.previous.addBranch(stackitem)
			
			self.current_item = stackitem
			
			actionAdded.send(sender=self, action=action)
			changed.send(sender=self)

	def clear(self):
		self._undostack.clear()
		self._groups = []
		cleared.send(sender=self)
		changed.send(sender=self)

	# Linear undo
	def undo(self, amount=1): 
		if amount <= 0:
			return
	
		preUndo.send(sender=self)
		for i in range(amount):
			if self.current_item == self.first_item:
				print "Warning: UndoManager: Tried to undo non-existing action."
				break
			
			actions = [self.current_item.object]
			if isinstance(self.current_item.object, UndoGroup):
				actions = (self.current_item.object.getObjects())
			for action in actions:
				action.undo()
			
			self.current_item = self.current_item.previous
			
		postUndo.send(sender=self)
		changed.send(sender=self)
			
			
	def redo(self, amount=1):
		if amount <= 0:
			return
		
		preRedo.send(sender=self)
		for i in range(amount):
			if self.current_item.next is None:
				print "Warning: UndoManager: Tried to redo non-existing action."
				break
				
			self.current_item = self.current_item.next
			
			actions = [self.current_item.object]
			if isinstance(self.current_item.object, UndoGroup):
				actions = reversed(self.current_item.object.getObjects())
			for action in actions:
				action.redo()
				
		postRedo.send(sender=self)
		changed.send(sender=self)

	def undoAction(self, action): 
		action.undo()
		
	def redoAction(self, action):
		action.redo()

	# Branched mode
	def setMaxHistoryItems(self, maxitems): 
		self._max_history = maxitems
		
	def getMaxHistoryItems(self): 
		return self._max_history
	maxHistoryItems = property(getMaxHistoryItems, setMaxHistoryItems)

	def getBranchMode(self): 
		return self._branched_mode
		
	def setBranchMode(self, enable):
		self._branched_mode = enable
		changed.send(sender=self)

	def getBranches(self): return []
	def nextBranch(self): 
		self.current_item.nextBranch()
		changed.send(sender=self)
		
	def previousBranch(self):
		self.current_item.previousBranch()
		changed.send(sender=self)

class UndoObject:
	def __init__(self, undoCallback, redoCallback, name="", description="", icon=""):
		self.name = name
		self.redoCallback = redoCallback
		self.undoCallback = undoCallback
		self.description = description
		self.icon = icon
		
		self.undone = False

	def undo(self): 
		if self.undone is True:
			print "Tried to undo already undone action!"
			return
			
		self.undone = True
		self.undoCallback()
		
	def redo(self): 
		if self.undone is False:
			print "Tried to redo already redone action!"
			return
			
		self.undone = False
		self.redoCallback()
		
class UndoGroup:
	def __init__(self, name="", description="", icon=""):
		self.name = name
		self.description = description
		self.icon = icon
		
		self.undoobjects = []
		
	def addObject(self, object):
		self.undoobjects.append(object)
	
	def getObjects(self):
		return self.undoobjects

class UndoStackItem:
	def __init__(self, object):
		self._branches = []
		self._currentbranch = -1
		
		self.parent = None
		self.object = object
		self.previous = None
		self.next = None
	
	def addBranch(self, item):
		self._branches.append(item)
		
		self._currentbranch += 1
		self.next = self._branches[self._currentbranch]
		self.next.parent = self
		
	def nextBranch(self):
		if len(self._branches) <= 0:
			return
		self._currentbranch += 1
		if self._currentbranch >= len(self._branches):
			self._currentbranch = 0
		changed.send(sender=self)
		self.next = self._branches[self._currentbranch]
			
	def previousBranch(self):
		if len(self._branches) <= 0:
			return
			
		self._currentbranch -= 1
		if self._currentbranch < 0:
			self._currentbranch = len(self._branches)-1
		changed.send(sender=self)
		self.next = self._branches[self._currentbranch]
		
	def setBranchIndex(self, index):
		if index < 0 or index >= len(self._branches):
			return
		self._currentbranch = index
		changed.send(sender=self)
		self.next = self._branches[self._currentbranch]
	
	def setBranch(self, branch):
		for b in range(len(self._branches)):
			if self._branches[b] == branch:
				self._currentbranch = b
				self.next = self._branches[self._currentbranch]
				changed.send(sender=self)
				return True
		else:
			print "Didn't find branch!"
			return False
	