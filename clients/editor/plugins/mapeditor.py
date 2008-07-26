# MapEditor is a plugin for Fifedit. It allows for selection and visual editing of maps.
# MapEditor must be pumped (see pump).

import math

import fife
import plugin
import pychan
import pychan.widgets as widgets
from pychan.tools import callbackWithArguments as cbwa
from selection import Selection, ClickSelection
from plugins.objectselector import ObjectSelector

from pychan.manager import DEFAULT_STYLE
DEFAULT_STYLE['default']['base_color'] = fife.Color(85,128,151)

states = ('NOTHING_LOADED', 'VIEWING', 'INSERTING', 'REMOVING', 'MOVING')
for s in states:
	globals()[s] = s
NOT_INITIALIZED = -9999999

class EditorLogicError(Exception):
	pass

class MapSelection(object):
	def __init__(self, onLayerSelect, onObjectSelect):
		self._mapedit = None
		self._onLayerSelect = onLayerSelect
		self._onObjectSelect = onObjectSelect

	def show(self, map):
		if not self._mapedit:
			self._mapedit = pychan.loadXML('content/gui/mapeditor.xml')
			self._mapedit.mapEvents({
				'layerButton'  : self._onLayerSelect,
				'objButton'   : self._onObjectSelect,
				'closeButton' : self.hide
			})
		fields = self._mapedit.findChild(name='Properties')
		# Clear previously added children
		fields.removeChildren(*fields.children)
		hbox = widgets.HBox()
		fields.addChild(hbox)
		label = widgets.Label(text='ID',min_size=(80,0))
		hbox.addChild(label)
		field = widgets.TextField(text=map.getId(),min_size=(100,0))
		hbox.addChild(field)
		self._mapedit.adaptLayout()
		self._mapedit.show()
		self._mapedit.x = 10
		self._mapedit.y = 580

	def hide(self):
		self._mapedit.hide()

class Toolbar(object):
	def __init__(self, onSelect, onMove, onInsert, onDelete, onBtnEnter, onBtnExit):
		self._onSelect, self._onMove, self._onInsert, self._onDelete = onSelect, onMove, onInsert, onDelete
		self.onBtnEnter, self.onBtnExit = onBtnEnter, onBtnExit
		self._toolbar = None
	
	def show(self):
		if not self._toolbar:
			self._toolbar = pychan.loadXML('content/gui/tools.xml')
			evtmap = {
				'btnSelect' : self._onSelect,
				'btnMove' : self._onMove,
				'btnInsert' : self._onInsert,
				'btnDelete' : self._onDelete
			}
			self._toolbar.mapEvents(evtmap)
			for k in evtmap.keys():
				btn = self._toolbar.findChild(name=k)
				btn.setEnterCallback(self.onBtnEnter)
				btn.setExitCallback(self.onBtnExit)
		
		#self._toolbar.adaptLayout()
		self._toolbar.show()
		self._toolbar.x = 10
		self._toolbar.y = 50
	
	def hide(self):
		self._toolbar.hide()
	
	def _enableBtn(self, enabled, btn):
		pass
	
	def enableInsert(self, enabled):
		self._enableBtn(enabled, self._toolbar.findChild(name='btnInsert'))
		
	def enableDelete(self, enabled):
		self._enableBtn(enabled, self._toolbar.findChild(name='btnDelete'))

	def enableSelect(self, enabled):
		self._enableBtn(enabled, self._toolbar.findChild(name='btnSelect'))
	
class StatusBar(object):
	def __init__(self, screenw, screenh):
		self._statusbar = pychan.loadXML('content/gui/statuspanel.xml')
		self._statusbar.show()
		height = 25
		self._statusbar.position = (0, screenh - height)
		self._statusbar.size = (screenw, height)
		self.statustxt = ''
		self.lbl = self._statusbar.findChild(name='lblStatus')

	def setStatus(self, msg):
		self.statustxt = msg
		self.lbl.text = '  ' + msg
		self.lbl.resizeToContent()
	
	def showTooltip(self, elem):
		self.lbl.text = elem.helptext
		self.lbl.resizeToContent()

	def hideTooltip(self, elem):
		self.lbl.text = self.statustxt
		self.lbl.resizeToContent()


class MapEditor(plugin.Plugin,fife.IMouseListener, fife.IKeyListener):
	def __init__(self, engine):
		self._engine = engine
		eventmanager = self._engine.getEventManager()
		eventmanager.setNonConsumableKeys([
			fife.Key.LEFT,
			fife.Key.RIGHT,
			fife.Key.UP,
			fife.Key.DOWN])
		fife.IMouseListener.__init__(self)
		eventmanager.addMouseListener(self)
		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)

		# Fifedit plugin data
		self.menu_items = { 'Select Map' : self._selectMap }

		self._camera = None     # currently selected camera
		self._map = None        # currently selected map
		self._layer = None      # currently selected layer
		self._object = None     # currently selected object
		self._selection = None  # currently selected coordinates
		self._instances = None  # currently selected instances

		self._ctrldown = False
		self._shiftdown = False
		self._altdown = False
		self._dragx = NOT_INITIALIZED
		self._dragy = NOT_INITIALIZED
		
		self._mapselector = MapSelection(self._selectLayer, self._selectObject)
		self._objectselector = None
		rb = self._engine.getRenderBackend()
		self._statusbar = StatusBar(rb.getWidth(), rb.getHeight())
		self._toolbar = Toolbar(cbwa(self._setMode, VIEWING), cbwa(self._setMode, MOVING),
		                        cbwa(self._setMode, INSERTING), cbwa(self._setMode, REMOVING),
					self._statusbar.showTooltip, self._statusbar.hideTooltip)
		self._toolbar.show()
		self._setMode(NOTHING_LOADED)

		self._undoStack = []
		self._undo = False # tracks whether current action is an undo

	def _assert(self, statement, msg):
		if not statement:
			print msg
			raise EditorLogicError(msg)
	
	def _setMode(self, mode):
		if (mode != NOTHING_LOADED) and (not self._camera):
			self._statusbar.setStatus('Please load map first')
			return
		if (mode == INSERTING) and (not self._object):
			self._statusbar.setStatus('Please select object first')
			return
		self._mode = mode
		print "Entered mode " + mode
		self._statusbar.setStatus(mode.replace('_', ' ').capitalize())
	
	# gui for selecting a map
	def _selectMap(self):
		Selection([map.getId() for map in self._engine.getModel().getMaps()], self.editMap)

	def _selectDefaultCamera(self, map):
		self._camera = None
		
		self._engine.getView().resetRenderers()
		for cam in self._engine.getView().getCameras():
			cam.setEnabled(False)
			
		for cam in self._engine.getView().getCameras():
			if cam.getLocationRef().getMap().getId() == map.getId():
				rb = self._engine.getRenderBackend()
				cam.setViewPort(fife.Rect(0, 0, rb.getScreenWidth(), rb.getScreenHeight()))
				cam.setEnabled(True)
				self._camera = cam
				break
		if not self._camera:
			raise AttributeError('No cameras found associated with this map: ' + map.getId())

	def editMap(self, mapid):
		self._camera = None
		self._map = None
		self._layer = None
		self._object = None
		self._selection = None
		self._instances = None
		self._setMode(NOTHING_LOADED)
		
		self._map = self._engine.getModel().getMap(mapid)
		if not self._map.getLayers():
			raise AttributeError('Editor error: map ' + self._map.getId() + ' has no layers. Cannot edit.')
		
		self._layer = self._map.getLayers()[0]
		self._selectDefaultCamera(self._map)
		self._setMode(VIEWING)
		
		self._mapselector.show(self._map)
	
	def _selectLayer(self):
		Selection([layer.getId() for layer in self._map.getLayers()], self._editLayer)

	def _editLayer(self, layerid):
		self._layer = None
		layers = [l for l in self._map.getLayers() if l.getId() == layerid]
		self._assert(len(layers) == 1, 'Layer amount != 1')
		self._layer = layers[0]

	def _selectObject(self):
		if not self._objectselector:
			self._objectselector = ObjectSelector(self._engine, self._map, self._editObject)
		self._objectselector.show()

	def _editObject(self, object):
		self._object = object

	def _selectCell(self, screenx, screeny, preciseCoords=False):
		self._assert(self._camera, 'No camera bind yet, cannot select any cell')
		
		self._selection = self._camera.toMapCoordinates(fife.ScreenPoint(screenx, screeny), False)
		self._selection.z = 0
		loc = fife.Location(self._layer)
		if preciseCoords:
			self._selection = self._layer.getCellGrid().toExactLayerCoordinates(self._selection)
			loc.setExactLayerCoordinates(self._selection)
		else:
			self._selection = self._layer.getCellGrid().toLayerCoordinates(self._selection)
			loc.setLayerCoordinates(self._selection)
		fife.CellSelectionRenderer.getInstance(self._camera).selectLocation(loc)
		return loc

	def _getInstancesFromPosition(self, position, top_only):
		self._assert(self._layer, 'No layer assigned in _getInstancesFromPosition')
		self._assert(position, 'No position assigned in _getInstancesFromPosition')
		self._assert(self._camera, 'No camera assigned in _getInstancesFromPosition')
		
		loc = fife.Location(self._layer)
		if type(position) == fife.ExactModelCoordinate:
			loc.setExactLayerCoordinates(position)
		else:
			loc.setLayerCoordinates(position)
		instances = self._camera.getMatchingInstances(loc)
		if top_only and (len(instances) > 0):
			instances = [instances[0]]
		return instances

	def undo(self):
		if self._undoStack != []:
			# execute inverse of last action
			self._undo = True
			self._undoStack.pop()()
			self._undo = False
	
	def _placeInstance(self,position,object):
		mname = '_placeInstance'
		self._assert(object, 'No object assigned in %s' % mname)
		self._assert(position, 'No position assigned in %s' % mname)
		self._assert(self._layer, 'No layer assigned in %s' % mname)

		print 'Placing instance of ' + object.getId() + ' at ' + str(position)
		print object
		
		# don't place repeat instances
		for i in self._getInstancesFromPosition(position, False):
			if i.getObject().getId() == object.getId():
				print 'Warning: attempt to place duplicate instance of object %s. Ignoring request.' % object.getId()
				return

		inst = self._layer.createInstance(object, position)
		fife.InstanceVisual.create(inst)
		if not self._undo:
			self._undoStack.append(lambda: self._removeInstances(position))

	def _removeInstances(self,position):
		mname = '_removeInstances'
		self._assert(position, 'No position assigned in %s' % mname)
		self._assert(self._layer, 'No layer assigned in %s' % mname)

		for i in self._getInstancesFromPosition(position, top_only=True):
			print 'Deleting instance ' + str(i) + ' at ' + str(position)
			if not self._undo:
				print '>>> ' + i.getObject().getId()
				print '>>> ' + str(i.getObject())
				object = i.getObject()
				self._undoStack.append(lambda: self._placeInstance(position,object))
			self._layer.deleteInstance(i)
				
	def _moveInstances(self):
		mname = '_moveInstances'
		self._assert(self._selection, 'No selection assigned in %s' % mname)
		self._assert(self._layer, 'No layer assigned in %s' % mname)
		self._assert(self._mode == MOVING, 'Mode is not MOVING in %s (is instead %s)' % (mname, str(self._mode)))
		
		loc = fife.Location(self._layer)
		if self._shiftdown:
			loc.setExactLayerCoordinates(self._selection)
		else:
			loc.setLayerCoordinates(self._selection)
		for i in self._instances:
			f = fife.Location(self._layer)
			f.setExactLayerCoordinates(i.getFacingLocation().getExactLayerCoordinates() + fife.ExactModelCoordinate(float(self._selection.x), float(self._selection.y)) - i.getLocation().getExactLayerCoordinates())
			i.setLocation(loc)
			i.setFacingLocation(f)
	
	def _rotateInstances(self):
		mname = '_rotateInstances'
		self._assert(self._selection, 'No selection assigned in %s' % mname)
		self._assert(self._layer, 'No layer assigned in %s' % mname)
		
		for i in self._getInstancesFromPosition(self._selection, top_only=True):
			i.setRotation((i.getRotation() + 90) % 360)
##    Surprisingly, the following "snap-to-rotation" code is actually incorrect. Object
##    rotation is independent of the camera, whereas the choice of an actual rotation image
##    depends very much on how the camera is situated. For example, suppose an object has
##    rotations defined for 45,135,225,315. And suppose the camera position results in an
##    effective 60 degree rotation. If the object is given a rotation of 0, then the (correct)
##    final rotation value of 45 (which is closest to 60 = 0 + 60) will be chosen. If we try
##    to snap to the closest value to 0 (45), then an incorrect final rotation value will be
##    chosen: 135, which is closest to 105 = 45 + 60. --jwt
#			ovis = i.getObject().get2dGfxVisual()
#			curUsedAngle = ovis.getClosestMatchingAngle(i.getRotation())
#			angles = ovis.getStaticImageAngles()
#			if angles:
#				ind = list(angles).index(curUsedAngle)
#				if ind == (len(angles) - 1):
#					ind = 0
#				else:
#					ind += 1
#				i.setRotation(angles[ind])
#			else:
#				print "rotation not supported for this instance"

	def changeRotation(self):
		currot = self._camera.getRotation()
		self._camera.setRotation((currot + 90) % 360)
	
	def _moveCamera(self, screen_x, screen_y):
		coords = self._camera.getLocationRef().getMapCoordinates()
		z = self._camera.getZoom()
		r = self._camera.getRotation()
		if screen_x:
			coords.x -= screen_x / z * math.cos(r / 180.0 * math.pi) / 100;
			coords.y -= screen_x / z * math.sin(r / 180.0 * math.pi) / 100;
		if screen_y:
			coords.x -= screen_y / z * math.sin(-r / 180.0 * math.pi) / 100;
			coords.y -= screen_y / z * math.cos(-r / 180.0 * math.pi) / 100;
		coords = self._camera.getLocationRef().setMapCoordinates(coords)
		self._camera.refresh()
	
	def mousePressed(self, evt):
		if evt.isConsumedByWidgets():
			return
		
		if self._ctrldown:
			if evt.getButton() == fife.MouseEvent.LEFT:
				self._dragx = evt.getX()
				self._dragy = evt.getY()
		else:
			if self._camera:
				self._selectCell(evt.getX(), evt.getY(), self._shiftdown)
			if self._mode == VIEWING:
				self._instances = self._getInstancesFromPosition(self._selection, top_only=True)
			elif self._mode == INSERTING:
				self._placeInstance(self._selection,self._object)
			elif self._mode == REMOVING:
				self._removeInstances(self._selection)
			elif self._mode == MOVING:
				self._instances = self._getInstancesFromPosition(self._selection, top_only=True)
			else:
				self._setMode(self._mode) # refresh status
	
	def mouseDragged(self, evt):
		if evt.isConsumedByWidgets():
			return
		
		if self._ctrldown:
			if (self._dragx != NOT_INITIALIZED) and (self._dragy != NOT_INITIALIZED):
				self._moveCamera(evt.getX() - self._dragx, evt.getY() - self._dragy)
			self._dragx = evt.getX()
			self._dragy = evt.getY()
		else:
			if self._mode == INSERTING:
				self._selectCell(evt.getX(), evt.getY())
				self._placeInstance(self._selection,self._object)
			elif self._mode == REMOVING:
				self._selectCell(evt.getX(), evt.getY())
				self._removeInstances(self._selection)
			elif self._mode == MOVING and self._instances:
				self._selectCell(evt.getX(), evt.getY(), self._shiftdown)
				self._moveInstances()

	def mouseReleased(self, evt):
		if evt.isConsumedByWidgets():
			return
		
		self._dragx = NOT_INITIALIZED
		self._dragy = NOT_INITIALIZED
	
	def mouseMoved(self, evt):
		pass
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	
	def mouseWheelMovedUp(self, evt):
		if self._ctrldown and self._camera:
			self._camera.setZoom(self._camera.getZoom() * 1.05)
			
	def mouseWheelMovedDown(self, evt):
		if self._ctrldown and self._camera:
			self._camera.setZoom(self._camera.getZoom() / 1.05)
			

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		
		if keyval == fife.Key.LEFT:
			self._moveCamera(50, 0)
		elif keyval == fife.Key.RIGHT:
			self._moveCamera(-50, 0)
		elif keyval == fife.Key.UP:
			self._moveCamera(0, 50)
		elif keyval == fife.Key.DOWN:
			self._moveCamera(0, -50)
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

		elif keyval == fife.Key.DELETE_KEY:
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
	
	def keyReleased(self, evt):
		keyval = evt.getKey().getValue()
		if keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self._ctrldown = False
		elif keyval in (fife.Key.LEFT_SHIFT, fife.Key.RIGHT_SHIFT):
			self._shiftdown = False
		elif keyval in (fife.Key.LEFT_ALT, fife.Key.RIGHT_ALT):
			self._altdown = False


