
class UndoManager:
	# Methods
	def __init__(self, branchedMode = False, max_history=10000):
		self._max_history = max_history
		# Should we use deque, instead of lists?
		self._undostack = []
		self._redostack = []
		self._groups = []
		self._branched_mode = False

	def getStack(self):
		return self._stack

	def startGroup(self, name="", description="", icon=""): 
		self.groups.append(UndoGroup(name, description, icon))
	
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
			self._undostack.append(stackitem)
			self._redostack = []

	def clear(self):
		self._undostack = []
		self._redostack = []
		self._groups = []

	# Linear undo
	def undo(self, amount=1): 
		if amount <= 0:
			return
			
		for i in range(amount):
			if len(self._undostack) <= 0:
				print "Warning: UndoManager: Tried to undo non-existing action."
				return
				
			stackitem = self._undostack.pop()
			self._redostack.append(stackitem)
			
			actions = [stackitem._object]
			if isinstance(stackitem._object, UndoGroup):
				actions = stackitem._object._undoobjects
			for action in actions:
				stackitem._object.getUndoAction()()
			
			
	def redo(self, amount=1):
		if amount <= 0:
			return
			
		for i in range(amount):
			if len(self._redostack) <= 0:
				print "Warning: UndoManager: Tried to redo non-existing action."
				return
				
			stackitem = self._redostack.pop()
			self._undostack.append(stackitem)
			
			actions = [stackitem._object]
			if isinstance(stackitem._object, UndoGroup):
				actions = stackitem._object._undoobjects
			for action in actions:
				stackitem._object.getRedoAction()()

	# Non-linear undo. This needs some work so that an action can't be undone twice
	def undoAction(self, action): 
		action.getUndoAction()()
		
	def redoAction(self, action):
		action.getRedoAction()()

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

	def getBranches(self): return []
	def nextBranch(self): pass
	def previousBranch(self): pass

class UndoObject:
	def __init__(self, undoCallback, redoCallback, name="", description="", icon=""):
		self.name = name
		self.redoCallback = redoCallback
		self.undoCallback = undoCallback
		self.description = description
		self.icon = icon

	def getUndoAction(self): 
		return self.undoCallback
		
	def getRedoAction(self): 
		return self.redoCallback
		
class UndoGroup:
	def __init__(self, name="", description="", icon=""):
		self._name = name
		self._description = description
		self._icon = icon
		
		self._undoobjects = []
		
	def addObject(self, object):
		self._undoobjects.append(object)
	
	def getObjects(self):
		return self._undoobjects

class UndoStackItem:
	def __init__(self, object):
		self._branches = []
		self._currentbranch = -1
		self._object = object
	
	def addBranch(self, item):
		self._branches.append(item)
		
	def nextBranch():
		pass