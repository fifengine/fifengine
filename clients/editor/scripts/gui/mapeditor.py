# MapEditor is a plugin for Fifedit. It allows for selection and visual editing of maps.

import math

import fife
import pychan
import pychan.widgets as widgets
import scripts.events as events
import action
from toolbar import ToolBar
from action import Action, ActionGroup
from scripts.mapcontroller import MapController

SCROLL_TOLERANCE = 10
SCROLL_SPEED = 1.0

states = (u'VIEWING', u'INSERTING', u'REMOVING', u'MOVING')
for s in states:
	globals()[s] = s
NOT_INITIALIZED = -9999999

class MapEditor(MapController, fife.IKeyListener):
	def __init__(self, map):
		MapController.__init__(self, map)

		eventmanager = self._engine.getEventManager()

		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)

		self._ignoreToggles = False # A hack to avoid infinite recursion when toggling a button

		self._ctrldown = False
		self._shiftdown = False
		self._altdown = False
		self._dragx = NOT_INITIALIZED
		self._dragy = NOT_INITIALIZED
		self._scrollx = 0
		self._scrolly = 0
		self._mode = VIEWING
		
		self._statusbar = self._editor.getStatusBar()
		self._toolbar = self._editor.getToolBar()
		self._toolbox = ToolBar(title=u"", orientation=1, panel_size=26)
		self._toolbox.position_technique = "explicit"
		self._toolbox.position = (200, 150)
		self._toolbox.setDocked(False)
		self._toolbox.show()
		
		self._initToolbuttons()
		self._setMode(VIEWING)
		
		events.mousePressed.connect(self.mousePressed)
		events.mouseDragged.connect(self.mouseDragged)
		events.mouseReleased.connect(self.mouseReleased)
		events.mouseMoved.connect(self.mouseMoved)
		events.mouseWheelMovedUp.connect(self.mouseWheelMovedUp)
		events.mouseWheelMovedDown.connect(self.mouseWheelMovedDown)
	
		events.onPump.connect(self.pump)
		
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

	def mousePressed(self, sender, event):
		if event.isConsumedByWidgets():
			return

		realCoords = self._getRealCoords(sender, event)

		if self._ctrldown:
			if event.getButton() == fife.MouseEvent.LEFT:
				self._dragx = realCoords[0]
				self._dragy = realCoords[1]
		else:
			if self._camera:
				self.selectCell(realCoords[0], realCoords[1], self._shiftdown)
			if self._mode == VIEWING:
				self._instances = self.getInstancesFromPosition(self._selection, top_only=True)
			elif self._mode == INSERTING:
				self.placeInstance(self._selection,self._object)
			elif self._mode == REMOVING:
				self.removeInstances(self._selection)
			elif self._mode == MOVING:
				self._instances = self.getInstancesFromPosition(self._selection, top_only=True)
			else:
				self._setMode(self._mode) # refresh status

	def mouseDragged(self, sender, event):
		if event.isConsumedByWidgets():
			return


		realCoords = self._getRealCoords(sender, event)
			
		if self._ctrldown:
			if (self._dragx != NOT_INITIALIZED) and (self._dragy != NOT_INITIALIZED):
				self.moveCamera(realCoords[0] - self._dragx, realCoords[1] - self._dragy)
			self._dragx = realCoords[0]
			self._dragy = realCoords[1]
		else:
			if self._mode == INSERTING:
				self.selectCell(realCoords[0], realCoords[1])
				self.placeInstance(self._selection,self._object)
			elif self._mode == REMOVING:
				self.selectCell(realCoords[0], realCoords[1])
				self.removeInstances(self._selection)
			elif self._mode == MOVING and self._instances:
				self.selectCell(realCoords[0], realCoords[1], self._shiftdown)
				self.moveInstances()

	def mouseReleased(self, sender, event):
		if event.isConsumedByWidgets():
			return

		self._dragx = NOT_INITIALIZED
		self._dragy = NOT_INITIALIZED

	def mouseMoved(self, sender, event):
		realCoords = self._getRealCoords(sender, event)
		if self._camera:
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
		if self._ctrldown and self._camera:
			self._camera.setZoom(self._camera.getZoom() * 1.05)

	def mouseWheelMovedDown(self, event):
		if self._ctrldown and self._camera:
			self._camera.setZoom(self._camera.getZoom() / 1.05)


	def keyPressed(self, event):
		keyval = event.getKey().getValue()
		keystr = event.getKey().getAsString().lower()

		if keyval == fife.Key.LEFT:
			self.moveCamera(50, 0)
		elif keyval == fife.Key.RIGHT:
			self.moveCamera(-50, 0)
		elif keyval == fife.Key.UP:
			self.moveCamera(0, 50)
		elif keyval == fife.Key.DOWN:
			self.moveCamera(0, -50)
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
			gridrenderer = self._camera.getRenderer('GridRenderer')
			gridrenderer.setEnabled(not gridrenderer.isEnabled())

		elif keystr == 'b':
			blockrenderer = self._camera.getRenderer('BlockingInfoRenderer')
			blockrenderer.setEnabled(not blockrenderer.isEnabled())

		elif keystr == 'r':
			if self._selection:
				self._rotateInstances()

		elif keystr == 'o':
			self.changeRotation()

		elif keystr == 'u':
			self.undo()

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
			if hasattr(parent, "parent") is False:
				break
			
			parent = parent.parent
			
		return (offsetX, offsetY)

	def pump(self):
		if self._scrollx != 0 or self._scrolly != 0:
			self.moveCamera(self._scrollx * self._engine.getTimeManager().getTimeDelta(), self._scrolly * self._engine.getTimeManager().getTimeDelta())
