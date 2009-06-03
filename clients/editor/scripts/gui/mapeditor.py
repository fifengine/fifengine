# MapEditor is a plugin for Fifedit. It allows for selection and visual editing of maps.

import math

import fife
import pychan
import pychan.widgets as widgets
import scripts
import scripts.events as events
import action
from toolbar import ToolBar
from menubar import Menu, MenuBar
from action import Action, ActionGroup
from scripts.mapcontroller import MapController

states = (u'SELECTING', u'INSERTING', u'REMOVING', u'MOVING')
for s in states:
	globals()[s] = s
NOT_INITIALIZED = -9999999

class MapEditor:
	def __init__(self):	
		self._ignoreToggles = False # A hack to avoid infinite recursion when toggling a button
		self._controller = None
		self._mode = SELECTING
		
		self._editor = scripts.editor.getEditor()
		self._eventlistener = self._editor.getEventListener()
		self._statusbar = self._editor.getStatusBar()
		self._toolbar = self._editor.getToolBar()
		self._object = None
		self._startDragPos = None
		self._lastDragPos = None
		self._lastDragPosExact = None
		
		self._toolbox = self._editor.getToolbox()
		
		self._initToolbuttons()
		
		self._toolbox.show()
		self._instances = []
		
		events.postMapShown.connect(self._mapChanged)
		events.onObjectSelected.connect(self.setObject)
		self._undogroup = False
		
	def _init(self):
		self._dragx = NOT_INITIALIZED
		self._dragy = NOT_INITIALIZED
		
		self._setMode(SELECTING)
		
		events.keyPressed.connect(self.keyPressed)
		events.keyReleased.connect(self.keyReleased)
		
		events.mousePressed.connect(self.mousePressed)
		events.mouseDragged.connect(self.mouseDragged)
		events.mouseReleased.connect(self.mouseReleased)
		events.mouseMoved.connect(self.mouseMoved)
		events.mouseWheelMovedUp.connect(self.mouseWheelMovedUp)
		events.mouseWheelMovedDown.connect(self.mouseWheelMovedDown)
		events.mouseExited.connect(self.mouseExited)
		
	def _clear(self):
		events.keyPressed.disconnect(self.keyPressed)
		events.keyReleased.disconnect(self.keyReleased)
		
		events.mousePressed.disconnect(self.mousePressed)
		events.mouseDragged.disconnect(self.mouseDragged)
		events.mouseReleased.disconnect(self.mouseReleased)
		events.mouseMoved.disconnect(self.mouseMoved)
		events.mouseWheelMovedUp.disconnect(self.mouseWheelMovedUp)
		events.mouseWheelMovedDown.disconnect(self.mouseWheelMovedDown)
		events.mouseExited.disconnect(self.mouseExited)
	
		events.onPump.disconnect(self.pump)
		
	def _mapChanged(self, sender, mapview):
		self.setController(mapview.getController())
		
	def setObject(self, object):
		self._object = object
		
	def setController(self, controller):
		if self._controller is not None:
			self._clear()

		if controller is not None:
			self._init()
			
		self._controller = controller
		
	def _initToolbuttons(self):
		self._selectAction = Action(text=u"Select", icon="gui/icons/hand.png", checkable=True)
		self._drawAction = Action(text=u"Draw", icon="gui/icons/add_instance.png", checkable=True)
		self._removeAction = Action(text=u"Remove", icon="gui/icons/quit.png", checkable=True)
		self._moveAction = Action(text=u"Move", icon="gui/icons/select_layer.png", checkable=True)
		
		self._selectAction.helptext = u"Select cells on layer"
		self._moveAction.helptext = u"Moves instances"
		self._drawAction.helptext = u"Adds new instances based on currently selected object"
		self._removeAction.helptext = u"Deletes instances"
		
		action.toggled.connect(self._buttonToggled, sender=self._selectAction)
		action.toggled.connect(self._buttonToggled, sender=self._moveAction)
		action.toggled.connect(self._buttonToggled, sender=self._drawAction)
		action.toggled.connect(self._buttonToggled, sender=self._removeAction)
		
		self._toolgroup = ActionGroup(exclusive=True, name=u"Tool group")
		self._toolgroup.addAction(self._selectAction)
		self._toolgroup.addAction(self._moveAction)
		self._toolgroup.addAction(self._drawAction)
		self._toolgroup.addAction(self._removeAction)
		
		
		self._toolbox.addAction(self._toolgroup)
		self._toolbox.adaptLayout()
		
		self._editor._editMenu.addAction(self._toolgroup)
		
	def _setMode(self, mode):
		if (mode == INSERTING) and (not self._object):
			self._statusbar.setText(u'Please select object first')
			mode = self._mode

		self._ignoreToggles = True
		# Update toolbox buttons
		if (mode == INSERTING):
			self._drawAction.setChecked(True)
		elif mode == REMOVING:
			self._removeAction.setChecked(True)
		elif mode == MOVING:
			self._moveAction.setChecked(True)
		else:
			self._selectAction.setChecked(True)
		self._ignoreToggles = False

		self._mode = mode
		print "Entered mode " + mode
		self._statusbar.setText(mode.replace('_', ' ').capitalize())
		
	def _buttonToggled(self, sender, toggled):
		if self._controller is None: return
		if self._ignoreToggles is True: return
	
		mode = SELECTING
		
		if toggled:
			if sender == self._selectAction:
				mode = SELECTING
			elif sender == self._moveAction:
				mode = MOVING
			elif sender == self._drawAction:
				mode = INSERTING
			elif sender == self._removeAction:
				mode = REMOVING

		self._setMode(mode)
		
	def mouseExited(self, sender, event):
		pass


	def mousePressed(self, sender, event):
		if event.isConsumedByWidgets():
			return

		realCoords = self._getRealCoords(sender, event)

		if self._eventlistener.controlPressed:
			if event.getButton() == fife.MouseEvent.LEFT:
				self._dragx = realCoords[0]
				self._dragy = realCoords[1]
		else:
			if self._mode == SELECTING:
				self._controller.resetSelection()
			elif self._mode == INSERTING:
				self._controller.selectCell(realCoords[0], realCoords[1])
				self._controller.getUndoManager().startGroup("Inserted instances")
				self._undogroup = True
				
				position = self._controller._camera.toMapCoordinates(fife.ScreenPoint(realCoords[0], realCoords[1]), False)
				position = self._controller._layer.getCellGrid().toLayerCoordinates(position)
				
				self._controller.selectCell(realCoords[0], realCoords[1])
				self._controller.placeInstance(position, self._object)
			elif self._mode == REMOVING:
				self._controller.selectCell(realCoords[0], realCoords[1])
				self._controller.getUndoManager().startGroup("Removed instances")
				self._undogroup = True
				
				self._controller.removeInstances(self._controller.getInstancesFromSelection())
			elif self._mode == MOVING:
				# TODO: Check if mousepos is in a selected cell. If not, select the clicked cell
				
				position = self._controller._camera.toMapCoordinates(fife.ScreenPoint(realCoords[0], realCoords[1]), False)
				position = self._controller._layer.getCellGrid().toExactLayerCoordinates(position)

				self._lastDragPos = self._controller._layer.getCellGrid().toLayerCoordinates(position)
				self._lastDragPosExact = position
				
				for i in self._controller._selection:
					if i.getLayerCoordinates() == self._lastDragPos:
						break
				else:
					self._controller.resetSelection()
					self._controller.selectCell(realCoords[0], realCoords[1])
					
				self._instances = self._controller.getInstancesFromSelection()
				
				self._controller.getUndoManager().startGroup("Moved instances")
				self._undogroup = True

	def mouseDragged(self, sender, event):
		if event.isConsumedByWidgets():
			return
			
		realCoords = self._getRealCoords(sender, event)
			
		if self._eventlistener.controlPressed:
			if (self._dragx != NOT_INITIALIZED) and (self._dragy != NOT_INITIALIZED):
				self._controller.moveCamera(realCoords[0] - self._dragx, realCoords[1] - self._dragy)
			self._dragx = realCoords[0]
			self._dragy = realCoords[1]
		else:
			if self._mode != SELECTING:
				self._controller.resetSelection()
				
			if self._mode == SELECTING:
				self._controller.selectCell(realCoords[0], realCoords[1])
			elif self._mode == INSERTING:
				position = self._controller._camera.toMapCoordinates(fife.ScreenPoint(realCoords[0], realCoords[1]), False)
				position = self._controller._layer.getCellGrid().toLayerCoordinates(position)
				
				self._controller.selectCell(realCoords[0], realCoords[1])
				self._controller.placeInstance(position, self._object)
			elif self._mode == REMOVING:
				self._controller.selectCell(realCoords[0], realCoords[1])
				self._controller.removeInstances(self._controller.getInstancesFromSelection())
			elif self._mode == MOVING:
				position = self._controller._camera.toMapCoordinates(fife.ScreenPoint(realCoords[0], realCoords[1]), False)
				positionExact = self._controller._layer.getCellGrid().toExactLayerCoordinates(position)
				position = self._controller._layer.getCellGrid().toLayerCoordinates(position)
				
				if self._eventlistener.shiftPressed:
					self._controller.moveInstances(self._instances, positionExact-self._lastDragPosExact, True)
				else:
					self._controller.moveInstances(self._instances, position-self._lastDragPos, False)
				self._lastDragPos = position
				self._lastDragPosExact = positionExact
				
				# Update selection
				self._controller.resetSelection()
				
				for i in self._instances:
					pos = i.getLocation().getExactLayerCoordinates()
					pos = self._controller._camera.toScreenCoordinates(pos)
					self._controller.selectCell(pos.x, pos.y)

	def mouseReleased(self, sender, event):
		if event.isConsumedByWidgets():
			return
			
		realCoords = self._getRealCoords(sender, event)
		if self._mode == SELECTING and not self._eventlistener.controlPressed:
			self._controller.selectCell(realCoords[0], realCoords[1])
		
		if self._undogroup:
			self._controller.getUndoManager().endGroup()
			self._undogroup = False

		self._dragx = NOT_INITIALIZED
		self._dragy = NOT_INITIALIZED

	def mouseMoved(self, sender, event):
		realCoords = self._getRealCoords(sender, event)
		if self._controller._camera:
			mouse_x = realCoords[0]
			mouse_y = realCoords[1]
			
			sender_x = realCoords[0] - event.getX()
			sender_y = realCoords[1] - event.getY()
			
			screen_x = sender.width + sender_x
			screen_y = sender.height + sender_y
			ratio = float(screen_x) / screen_y
				
	def mouseWheelMovedUp(self, event):
		if self._eventlistener.controlPressed and self._controller._camera:
			self._controller._camera.setZoom(self._controller._camera.getZoom() * 1.05)

	def mouseWheelMovedDown(self, event):
		if self._eventlistener.controlPressed and self._controller._camera:
			self._controller._camera.setZoom(self._controller._camera.getZoom() / 1.05)


	def keyPressed(self, event):
		keyval = event.getKey().getValue()
		keystr = event.getKey().getAsString().lower()
		
		if keyval == fife.Key.LEFT:
			self._controller.moveCamera(50, 0)
		elif keyval == fife.Key.RIGHT:
			self._controller.moveCamera(-50, 0)
		elif keyval == fife.Key.UP:
			self._controller.moveCamera(0, 50)
		elif keyval == fife.Key.DOWN:
			self._controller.moveCamera(0, -50)
		
		elif keyval == fife.Key.INSERT:
			if self._mode != INSERTING:
				self._setMode(INSERTING)
			else:
				self._setMode(SELECTING)

		elif keyval == fife.Key.DELETE:
			if self._mode != REMOVING:
				self._setMode(REMOVING)
			else:
				self._setMode(SELECTING)

		elif keystr == 'm':
			if self._mode != MOVING:
				self._setMode(MOVING)
			else:
				self._setMode(SELECTING)

		elif keystr == 't':
			gridrenderer = self._controller._camera.getRenderer('GridRenderer')
			gridrenderer.setEnabled(not gridrenderer.isEnabled())

		elif keystr == 'b':
			blockrenderer = self._controller._camera.getRenderer('BlockingInfoRenderer')
			blockrenderer.setEnabled(not blockrenderer.isEnabled())

		elif keystr == 'r':
			self._controller.rotateInstances()

		elif keystr == 'o':
			self._controller.changeRotation()

		elif keyval == ord('z'):
			if self._eventlistener.controlPressed:
				if self._eventlistener.altPressed:
					if self._eventlistener.shiftPressed:
						self._controller.getUndoManager().previousBranch()
					else:
						self._controller.getUndoManager().nextBranch()
				else:
					if self._eventlistener.shiftPressed:
						self._controller.redo()
					else:
						self._controller.undo()

	def keyReleased(self, event):		
		pass
			
	def _getRealCoords(self, sender, event):
		cw = sender
		offsetX = event.getX()
		offsetY = event.getY()
		
		parent = cw
		while parent is not None:
			if isinstance(parent, widgets.Widget):
				offsetX += parent.x
				offsetY += parent.y
				parent = parent.parent
			else:
				break
			
		return (offsetX, offsetY)