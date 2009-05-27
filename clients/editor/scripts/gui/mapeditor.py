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

SCROLL_TOLERANCE = 10
SCROLL_SPEED = 1.0

states = (u'VIEWING', u'INSERTING', u'REMOVING', u'MOVING')
for s in states:
	globals()[s] = s
NOT_INITIALIZED = -9999999

class MapEditor:
	def __init__(self):	
		self._ignoreToggles = False # A hack to avoid infinite recursion when toggling a button
		self._controller = None
		self._mode = VIEWING
		
		self._editor = scripts.editor.getEditor()
		self._statusbar = self._editor.getStatusBar()
		self._toolbar = self._editor.getToolBar()
		
		self._toolbox = self._editor.getToolbox()
		
		self._initToolbuttons()
		
		self._toolbox.show()
		
		self._ctrldown = False
		self._shiftdown = False
		self._altdown = False
		
		events.postMapShown.connect(self._mapChanged)
		
	def _init(self):
		self._dragx = NOT_INITIALIZED
		self._dragy = NOT_INITIALIZED
		self._scrollx = 0
		self._scrolly = 0
		
		self._setMode(VIEWING)
		
		events.keyPressed.connect(self.keyPressed)
		events.keyReleased.connect(self.keyReleased)
		
		events.mousePressed.connect(self.mousePressed)
		events.mouseDragged.connect(self.mouseDragged)
		events.mouseReleased.connect(self.mouseReleased)
		events.mouseMoved.connect(self.mouseMoved)
		events.mouseWheelMovedUp.connect(self.mouseWheelMovedUp)
		events.mouseWheelMovedDown.connect(self.mouseWheelMovedDown)
		events.mouseExited.connect(self.mouseExited)
	
		events.onPump.connect(self.pump)
		
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
		
		self._toolbar.addAction(self._toolgroup)
		self._toolbar.adaptLayout()
		
		self._editor._editMenu.addAction(self._toolgroup)
		
	def _setMode(self, mode):
		if (mode == INSERTING) and (not self._controller._object):
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
	
		mode = VIEWING
		
		if toggled:
			if sender == self._selectAction:
				mode = VIEWING
			elif sender == self._moveAction:
				mode = MOVING
			elif sender == self._drawAction:
				mode = INSERTING
			elif sender == self._removeAction:
				mode = REMOVING

		self._setMode(mode)
		
	def mouseExited(self, sender, event):
		self._scrollx = 0
		self._scrolly = 0


	def mousePressed(self, sender, event):
		if event.isConsumedByWidgets():
			return

		realCoords = self._getRealCoords(sender, event)

		if self._ctrldown:
			if event.getButton() == fife.MouseEvent.LEFT:
				self._dragx = realCoords[0]
				self._dragy = realCoords[1]
		else:
			if self._controller._camera:
				self._controller.selectCell(realCoords[0], realCoords[1], self._shiftdown)
			if self._mode == VIEWING:
				self._controller.setSelectedInstances(self._controller.getInstancesFromPosition(self._controller._selection, top_only=True))
			elif self._mode == INSERTING:
				self._controller.placeInstance(self._controller._selection, self._controller._object)
			elif self._mode == REMOVING:
				self._controller.removeInstances(self._controller._selection)
			elif self._mode == MOVING:
				self._controller.setSelectedInstances(self._controller.getInstancesFromPosition(self._controller._selection, top_only=True))
			else:
				self._setMode(self._mode) # refresh status

	def mouseDragged(self, sender, event):
		if event.isConsumedByWidgets():
			return


		realCoords = self._getRealCoords(sender, event)
			
		if self._ctrldown:
			if (self._dragx != NOT_INITIALIZED) and (self._dragy != NOT_INITIALIZED):
				self._controller.moveCamera(realCoords[0] - self._dragx, realCoords[1] - self._dragy)
			self._dragx = realCoords[0]
			self._dragy = realCoords[1]
		else:
			if self._mode == INSERTING:
				self._controller.selectCell(realCoords[0], realCoords[1])
				self._controller.placeInstance(self._controller._selection, self._controller._object)
			elif self._mode == REMOVING:
				self._controller.selectCell(realCoords[0], realCoords[1])
				self._controller.removeInstances(self._controller._selection)
			elif self._mode == MOVING and self._controller._instances:
				self._controller.selectCell(realCoords[0], realCoords[1], self._shiftdown)
				self._controller.moveInstances(exact=self._shiftdown)

	def mouseReleased(self, sender, event):
		if event.isConsumedByWidgets():
			return

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

			self._scrollx = 0
			self._scrolly = 0

			if mouse_y <= SCROLL_TOLERANCE + sender_y:
				# up
				self._scrolly = SCROLL_SPEED * ratio
			if mouse_x >= screen_x - SCROLL_TOLERANCE:
				# right
				self._scrollx = -SCROLL_SPEED
			if mouse_y >= screen_y - SCROLL_TOLERANCE:
				# bottom
				self._scrolly = -SCROLL_SPEED * ratio
			if mouse_x <= SCROLL_TOLERANCE + sender_x:
				# left
				self._scrollx = SCROLL_SPEED
				
	def mouseWheelMovedUp(self, event):
		if self._ctrldown and self._controller._camera:
			self._controller._camera.setZoom(self._controller._camera.getZoom() * 1.05)

	def mouseWheelMovedDown(self, event):
		if self._ctrldown and self._controller._camera:
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
		elif keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self._ctrldown = True
		elif keyval in (fife.Key.LEFT_SHIFT, fife.Key.RIGHT_SHIFT):
			self._shiftdown = True
		elif keyval in (fife.Key.LEFT_ALT, fife.Key.RIGHT_ALT):
			self._altdown = True

		elif keyval == fife.Key.INSERT:
			if self._mode != INSERTING:
				self._setMode(INSERTING)
			else:
				self._setMode(VIEWING)

		elif keyval == fife.Key.DELETE:
			if self._mode != REMOVING:
				self._setMode(REMOVING)
			else:
				self._setMode(VIEWING)

		elif keystr == 'm':
			if self._mode != MOVING:
				self._setMode(MOVING)
			else:
				self._setMode(VIEWING)

		elif keystr == 't':
			gridrenderer = self._controller._camera.getRenderer('GridRenderer')
			gridrenderer.setEnabled(not gridrenderer.isEnabled())

		elif keystr == 'b':
			blockrenderer = self._controller._camera.getRenderer('BlockingInfoRenderer')
			blockrenderer.setEnabled(not blockrenderer.isEnabled())

		elif keystr == 'r':
			if self._controller._selection:
				self._controller.rotateInstances()

		elif keystr == 'o':
			self._controller.changeRotation()

		elif keyval == ord('z'):
			if self._ctrldown:
				if self._altdown:
					if self._shiftdown:
						self._controller.getUndoManager().previousBranch()
					else:
						self._controller.getUndoManager().nextBranch()
				else:
					if self._shiftdown:
						self._controller.redo()
					else:
						self._controller.undo()

	def keyReleased(self, event):
		keyval = event.getKey().getValue()
		if keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self._ctrldown = False
		elif keyval in (fife.Key.LEFT_SHIFT, fife.Key.RIGHT_SHIFT):
			self._shiftdown = False
		elif keyval in (fife.Key.LEFT_ALT, fife.Key.RIGHT_ALT):
			self._altdown = False
			
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

	def pump(self):
		if self._scrollx != 0 or self._scrolly != 0:
			engine = self._editor.getEngine()
			self._controller.moveCamera(self._scrollx * engine.getTimeManager().getTimeDelta(), self._scrolly * engine.getTimeManager().getTimeDelta())
