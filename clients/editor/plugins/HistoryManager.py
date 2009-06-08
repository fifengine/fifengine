# coding: utf-8

import pychan
from pychan import widgets, tools, attrs, internal
import scripts
import scripts.plugin as plugin
from scripts.events import *
from scripts.gui.action import Action, ActionGroup
import fife
from fife import Color
from scripts import undomanager
import scripts.gui
from scripts.gui.panel import Panel
import pdb

class HistoryManager(plugin.Plugin):
	def __init__(self):
		self.editor = None
		self.engine = None
		
		self._enabled = False
		self.undomanager = None

	def enable(self):
		if self._enabled is True:
			return
			
		self.editor = scripts.editor.getEditor()
		self.engine = self.editor.getEngine()
			
		self._undoGroup = ActionGroup(name=u"UndoGroup")
		self._showAction = Action(u"History manager", checkable=True)
		self._undoAction = Action(u"Undo", "gui/icons/undo.png")
		self._redoAction = Action(u"Redo", "gui/icons/redo.png")
		self._nextAction = Action(u"Next branch", "gui/icons/next_branch.png")
		self._prevAction = Action(u"Previous branch", "gui/icons/previous_branch.png")
		scripts.gui.action.activated.connect(self.toggle, sender=self._showAction)
		scripts.gui.action.activated.connect(self._undo, sender=self._undoAction)
		scripts.gui.action.activated.connect(self._redo, sender=self._redoAction)
		scripts.gui.action.activated.connect(self._next, sender=self._nextAction)
		scripts.gui.action.activated.connect(self._prev, sender=self._prevAction)
		
		self._undoGroup.addAction(self._undoAction)
		self._undoGroup.addAction(self._redoAction)
		self._undoGroup.addAction(self._nextAction)
		self._undoGroup.addAction(self._prevAction)
		
		self.editor._toolsMenu.addAction(self._showAction)
		self.editor._editMenu.insertAction(self._undoGroup, 0)
		self.editor._editMenu.insertSeparator(position=1)
		
		events.postMapShown.connect(self.update)
		undomanager.changed.connect(self.update)
		
		self.buildGui()

	def disable(self):
		if self._enabled is False:
			return
			
		self.gui.hide()
		self.removeAllChildren()
		
		events.postMapShown.disconnect(self.update)
		undomanager.changed.disconnect(self.update)
		
		scripts.gui.action.activated.connect(self.toggle, sender=self._showAction)
		scripts.gui.action.activated.disconnect(self._undo, sender=self._undoAction)
		scripts.gui.action.activated.disconnect(self._redo, sender=self._redoAction)
		scripts.gui.action.activated.disconnect(self._next, sender=self._nextAction)
		scripts.gui.action.activated.disconnect(self._prev, sender=self._prevAction)
		
		self.editor._toolsMenu.removeAction(self._showAction)
		self.editor._toolsMenu.removeAction(self._undoGroup)


	def isEnabled(self):
		return self._enabled;

	def getName(self):
		return "History manager"
		

	def buildGui(self):
		self.gui = Panel(title=u"History")
		self.scrollarea = widgets.ScrollArea(min_size=(200,300))
		self.list = widgets.ListBox()
		self.list.capture(self._itemSelected)
		
		self.gui.addChild(self.scrollarea)
		self.scrollarea.addChild(self.list)
		
		self.gui.position_technique = "right:center"
		
	def _linearUndo(self, target):
		mapview = self.editor.getActiveMapView()
		if mapview is None:
			return
			
		undomanager = mapview.getController().getUndoManager()
		current_item = undomanager.current_item
		
		# Redo?
		item = current_item
		count = 0
		while item is not None:
			if item == target:
				undomanager.redo(count)
				break
			count += 1
			item = item.next
			
		else: 
			# Undo?
			count = 0
			item = current_item
			while item is not None:
				if item == target:
					undomanager.undo(count)
					break
				count += 1
				item = item.previous
				
			else:
				print "HistoryManager: Didn't find target item!"
		
		# Select the current item, important to see if the undo/redo didn't work as expected
		self.update()
		
		
	def _itemSelected(self):
		mapview = self.editor.getActiveMapView()
		if mapview is None:
			return
			
		undomanager = mapview.getController().getUndoManager()
		
		stackitem = self.list.selected_item.item
		if stackitem == undomanager.current_item:
			return
		
		if undomanager.getBranchMode() is False:
			self._linearUndo(stackitem)
			return
		
		searchlist = []
		searchlist2 = []
		parent = stackitem
		branch = parent.next
		while parent is not None:
			if parent is undomanager.first_item or len(parent._branches) > 1:
				searchlist.append( (parent, branch) )
			branch = parent
			parent = parent.parent
		
		current_item = undomanager.current_item
		
		parent = current_item
		branch = parent.next
		while parent is not None:
			if parent is undomanager.first_item or len(parent._branches) > 1:
				searchlist2.append( (parent, branch) )
			branch = parent
			parent = parent.parent
			
		searchlist.reverse()
		searchlist2.reverse()
		
		# Remove duplicate entries, except the last duplicate, so we don't undo
		# more items than necessary
		sl = len(searchlist);
		if len(searchlist2) < sl:
			sl = len(searchlist2)
		for s in range(sl):
			if searchlist[s][0] != searchlist[s][0]:
				searchlist = searchlist[s-1:]
		
		s_item = searchlist[0][0]

		# Undo until we reach the first shared parent
		i = 0
		item = current_item
		while item is not None:
			if item == s_item:
				undomanager.undo(i)
				current_item = item
				break
			i += 1
			item = item.previous
		else:
			print "Nada (undo)"
			return
				
		# Switch branches
		for s_item in searchlist:
			 if s_item[0].setBranch(s_item[1]) is False:
				print "Warning: HistoryManager: Switching branch failed for: ", s_item
			 
		# Redo to stackitem
		item = current_item
		i = 0
		while item is not None:
			if item == stackitem:
				undomanager.redo(i)
				break
			i += 1
			item = item.next
		else:
			print "Nada (redo)"
		
		# Select the current item, important to see if the undo/redo didn't work as expected
		self.update()
		
		
	def recursiveUpdate(self, item, indention, parent=None, branchstr="-"):
		items = []
		
		branchnr = 0
		
		class _ListItem:
			def __init__(self, str, item, parent):
				self.str = str
				self.item = item
				self.parent = parent
				
			def __str__(self):
				return self.str
		
		while item is not None:
			listitem = _ListItem(u" "*indention + branchstr + " " + item.object.name, item, parent)
			items.append(listitem)
			branchnr = -1
			
			for branch in item.getBranches():
				branchnr += 1
				if branchnr == 0:
					continue
					
				items.extend(self.recursiveUpdate(branch, indention+2, listitem, str(branchnr)))
			
			if self.undomanager.getBranchMode():
				if len(item._branches) > 0:
					item = item._branches[0]
				else:
					break
			else: item = item.next

		return items
		
	def update(self):
		mapview = self.editor.getActiveMapView()
		if mapview is None:
			self.list.items = []
			return
			
		self.undomanager = undomanager = mapview.getController().getUndoManager()
		items = []
		items = self.recursiveUpdate(undomanager.first_item, 0)

		self.list.items = items
		i = 0
		for it in items:
			if it.item == undomanager.current_item:
				self.list.selected = i
				break
			i += 1
		self.scrollarea.adaptLayout()

	def show(self):
		self.update()
		self.gui.show()
		self._showAction.setChecked(True)

	def hide(self):
		self.gui.setDocked(False)
		self.gui.hide()
		self._showAction.setChecked(False)
		
	def _undo(self):
		if self.undomanager:
			self.undomanager.undo()
	
	def _redo(self):
		if self.undomanager:
			self.undomanager.redo()
		
	def _next(self):
		if self.undomanager:
			self.undomanager.nextBranch()
		
	def _prev(self):
		if self.undomanager:
			self.undomanager.previousBranch()
		
	def toggle(self):
		if self.gui.isVisible() or self.gui.isDocked():
			self.hide()
		else:
			self.show()
