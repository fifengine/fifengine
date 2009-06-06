import scripts.undomanager as undomanager

print "Testing undomanager:"
umanager = undomanager.UndoManager(branchedMode=False)

print "--- Testing basic undo/redo ---"

print "Adding 3 actions:"
def undoPrint(msg):
	print msg
def newAction(nr):
	undo = lambda: undoPrint("Undo: "+str(nr))
	redo = lambda: undoPrint("Redo: "+str(nr))
	return undomanager.UndoObject(undo, redo, "Name: "+str(nr), "Description: "+str(nr))


umanager.addAction(newAction("1"))
umanager.addAction(newAction("2"))
umanager.addAction(newAction("3"))

print "Undoing 3 actions:"
umanager.undo(3);

print "Redoing 3 actions, individually:"
umanager.redo()
umanager.redo()
umanager.redo()

print "--- Testing undogroups ---"
print "10 actions, 2 undogroups nested within 1 undogroup"
umanager.startGroup("ActionGroup 1", "Desc: AG1")
umanager.addAction(newAction("1"))
umanager.addAction(newAction("2"))
umanager.addAction(newAction("3"))
umanager.startGroup("ActionGroup 2", "AG2")
umanager.addAction(newAction("4"))
umanager.addAction(newAction("5"))
umanager.addAction(newAction("6"))
umanager.endGroup()
umanager.startGroup("ActionGroup 2", "AG2")
umanager.addAction(newAction("7"))
umanager.addAction(newAction("8"))
umanager.addAction(newAction("9"))
umanager.addAction(newAction("10"))
umanager.endGroup()
umanager.endGroup()

print "Undoing group:"
umanager.undo()

print "Redo"
umanager.redo()

print "--- Testing branches (without branch mode) ---"
print "Branch mode:", umanager.getBranchMode()

print "Setting up actions"
umanager.addAction(newAction("1-1"))
umanager.addAction(newAction("1-2"))
umanager.addAction(newAction("1-3"))
umanager.undo(3)
umanager.addAction(newAction("2-1"))
umanager.addAction(newAction("2-2"))
umanager.addAction(newAction("2-3"))
umanager.undo(3)
umanager.addAction(newAction("3-1"))
umanager.addAction(newAction("3-2"))
umanager.addAction(newAction("3-3"))
umanager.undo(3)
print "Branches", umanager.getBranches()
print "Next branch"
umanager.nextBranch()
umanager.redo(3)
umanager.undo(3)
print "Prev branch"
umanager.previousBranch()
umanager.redo(3)

print "--- Testing branches (with branch mode) ---"
print "Activating branch mode:"
umanager.setBranchMode(True)
print "Branch mode:", umanager.getBranchMode()

print "Setting up actions"
umanager.addAction(newAction("1-1"))
umanager.addAction(newAction("1-2"))
umanager.addAction(newAction("1-3"))
umanager.undo(3)
umanager.addAction(newAction("2-1"))
umanager.addAction(newAction("2-2"))
umanager.addAction(newAction("2-3"))
umanager.undo(3)
umanager.addAction(newAction("3-1"))
umanager.addAction(newAction("3-2"))
umanager.addAction(newAction("3-3"))
umanager.undo(3)
print "Branches", umanager.getBranches()
print "Next branch"
umanager.nextBranch()
umanager.redo(3)
umanager.undo(3)
print "Prev branch"
umanager.previousBranch()
umanager.redo(3)

import gc
print "--- Testing clear() function ---"
print "Checking garbage:", gc.collect(), "Uncollectable:", len(gc.garbage)
print "Clearing history"
umanager.clear()
print "Checking garbage:", gc.collect(), "Uncollectable:", len(gc.garbage)

print "--- Done ---"