
class UndoManager:
	# Methods
	def __init__(branchedMode = False, max_history=10000):
		pass

	def getUndoStack(): return []
	def getRedoStack(): return []

	def startGroup(name=None): pass
	def endGroup(): pass

	def addAction(action): pass

	def clear(): pass

	# Linear undo
	def undo(amount=1): pass
	def redo(amount=1): pass

	# Non-linear undo
	def undoAction(action): pass
	def redoAction(action): pass

	# Branched mode
	def _setMaxHistoryItems(maxitems): pass
	def _getMaxHistoryItems(): return 0
	maxHistoryItems = property(_getMaxHistoryItems, _setMaxHistoryItems)

	def _getBranchMode(): return False
	def _setBranchMode(enable): pass
	branchmode = property(_getBranchMode, _setBranchMode)

	def getBranches(): return []
	def nextBranch(): pass
	def previousBranch(): pass

class UndoObject:
	def __init__(undoCallback, redoCallback, name="", description="", icon=""):
		pass

	def getUndoAction(): return None
	def getRedoAction(): return None
