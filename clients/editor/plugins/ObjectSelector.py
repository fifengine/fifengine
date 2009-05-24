# coding: utf-8

import pychan
from pychan import widgets, tools, attrs, internal
from pychan.tools import callbackWithArguments
import scripts
import scripts.plugin as plugin
from scripts.events import *
from scripts.gui.action import Action
import fife
from fife import Color

# TODO:
# - Better event handling
# - Label background color can't be set

_DEFAULT_BASE_COLOR = internal.DEFAULT_STYLE['default']['base_color']
_DEFAULT_SELECTION_COLOR = internal.DEFAULT_STYLE['default']['selection_color']
_DEFAULT_COLOR_STEP = Color(10, 10, 10)

class ObjectIcon(widgets.VBox):
	""" The ObjectIcon is used to represent the object in the object selector.
	"""	
	ATTRIBUTES = widgets.VBox.ATTRIBUTES + [ attrs.Attr("text"), attrs.Attr("image"), attrs.BoolAttr("selected") ]
	
	def __init__(self,callback,**kwargs):
		super(ObjectIcon,self).__init__(**kwargs)

		self.callback = callback	

		self.capture(self._mouseEntered, "mouseEntered")
		self.capture(self._mouseExited, "mouseExited")
		self.capture(self._mouseClicked, "mouseClicked")

		vbox = widgets.VBox(padding=3)

		# Icon
		self.icon = widgets.Icon(**kwargs)
		self.addChild(self.icon)

		# Label
		hbox = widgets.HBox(padding=1)
		self.addChild(hbox)
		self.label = widgets.Label(**kwargs)
		hbox.addChild(self.label)

	def _setText(self, text):
		self.label.text = text
		
	def _getText(self):
		return self.label.text
	text = property(_getText, _setText)

	def _setImage(self, image):
		self.icon.image = image

	def _getImage(self):
		return self.icon.image
	image = property(_getImage, _setImage)

	def _setSelected(self, enabled):
		if isinstance(self.parent, ObjectIconList):
			if enabled == True:
				self.parent.selected_item = self
			else:
				if self.selected:
					self.parent.selected_item = None
		
		# + Color(0,0,0) to force variable copy
		if self.selected:
			self.base_color = _DEFAULT_SELECTION_COLOR + Color(0,0,0)
		else:
			self.base_color = _DEFAULT_BASE_COLOR + Color(0,0,0)

	def _isSelected(self):
		if isinstance(self.parent, ObjectIconList):
			return self == self.parent.selected_item
		return False
	selected = property(_isSelected, _setSelected)

	#--- Event handling ---#
	def _mouseEntered(self, event):
		self.base_color += _DEFAULT_COLOR_STEP

	def _mouseExited(self, event):
		self.base_color -= _DEFAULT_COLOR_STEP

	def _mouseClicked(self, event):
		self.selected = True
		self.callback()

class ObjectIconList(widgets.VBox):
	ATTRIBUTES = widgets.VBox.ATTRIBUTES
	
	def __init__(self,**kwargs):
		super(ObjectIconList, self).__init__(max_size=(5000,500000), **kwargs)
		self.base_color = self.background_color

		# TODO: Pychan doesn't support keyevents for nonfocusable widgets, yet
		#self.capture(self._keyPressed, "keyPressed")
		#self.capture(self._keyPressed, "keyReleased")
		self._selectedItem = None

	#def _keyPressed(self, event):
		#print "KeyEvent", event

	def clear(self):
		for c in reversed(self.children):
			self.removeChild(c)

	def _setSelectedItem(self, item):
		if isinstance(item, ObjectIcon) or item is None:
			if self._selectedItem is not None:
				tmp = self._selectedItem
				self._selectedItem = item
				tmp.selected = False
			else:
				self._selectedItem = item	

	def _getSelectedItem(self):
		return self._selectedItem
	selected_item = property(_getSelectedItem, _setSelectedItem)
	
class ObjectSelector(plugin.Plugin):
	"""The ObjectSelector class offers a gui Widget that let's you select the object you
	wish to use to in the editor.
	@param engine: fife instance
	@param map: fife.Map instance containing your map
	@param selectNotify: callback function used to tell the editor you selected an object.
	"""
	def __init__(self):
		self.editor = None
		self.engine = None
		self.mode = 'list' # Other mode is 'preview'
		
		self._enabled = False

	def enable(self):
		if self._enabled is True:
			return
			
		self.editor = scripts.editor.getEditor()
		self.engine = self.editor.getEngine()
			
		self._showAction = Action(u"Object selector", checkable=True)
		scripts.gui.action.activated.connect(self.toggle, sender=self._showAction)
		
		self.editor.getToolBar().addAction(self._showAction)
		self.editor._toolsMenu.addAction(self._showAction)
		
		events.postMapShown.connect(self.update)
		
		self.buildGui()

	def disable(self):
		if self._enabled is False:
			return
			
		self.gui.hide()
		self.removeAllChildren()
		
		events.postMapShown.disconnect(self.update)
		
		self.editor.getToolBar().removeAction(self._showAction)
		self.editor._toolsMenu.removeAction(self._showAction)

	def isEnabled(self):
		return self._enabled;

	def getName(self):
		return "Object selector"
		

	def buildGui(self):
		self.gui = pychan.loadXML('gui/objectselector.xml')

		# Add search field
		self._searchfield = self.gui.findChild(name="searchField")
		self._searchfield.capture(self._search)
		self._searchfield.capture(self._search, "keyPressed")
		self.gui.findChild(name="searchButton").capture(self._search)
		
		# Add the drop down with list of namespaces
		self.namespaces = self.gui.findChild(name="namespaceDropdown")
		self.namespaces.items = self.engine.getModel().getNamespaces()
		self.namespaces.selected = 0

		# TODO: Replace with SelectionEvent, once pychan supports it
		self.namespaces.capture(self.update_namespace, "action")
		self.namespaces.capture(self.update_namespace, "mouseWheelMovedUp")
		self.namespaces.capture(self.update_namespace, "mouseWheelMovedDown")
		self.namespaces.capture(self.update_namespace, "keyReleased")

		# Object list
		self.mainScrollArea = self.gui.findChild(name="mainScrollArea")
		self.objects = None
		if self.mode == 'list':
			self.setTextList()
		else: # Assuming self.mode is 'preview'
			self.setImageList()

		# Action buttons
		self.gui.findChild(name="toggleModeButton").capture(self.toggleMode)
		self.gui.findChild(name="closeButton").capture(self.hide)

		# Preview area
		self.gui.findChild(name="previewScrollArea").background_color = self.gui.base_color
		self.preview = self.gui.findChild(name="previewIcon")
		

	def toggleMode(self):
		if self.mode == 'list':
			self.setImageList()
			self.mode = 'preview'
		elif self.mode == 'preview':
			self.setTextList()
			self.mode = 'list'
		self.update()


	def setImageList(self):
		"""Sets the mainScrollArea to contain a Vbox that can be used to fill in
		preview Images"""
		if self.objects is not None:
			self.mainScrollArea.removeChild(self.objects)
		self.objects = ObjectIconList(name='list', size=(200,1000))
		self.objects.base_color = self.mainScrollArea.background_color
		self.mainScrollArea.addChild(self.objects)

	def setTextList(self):
		"""Sets the mainScrollArea to contain a List that can be used to fill in
		Object names/paths"""
		if self.objects is not None:
			self.mainScrollArea.removeChild(self.objects)
		self.objects = widgets.ListBox(name='list')
		self.objects.capture(self.listEntrySelected)
		self.mainScrollArea.addChild(self.objects)

	def _search(self):
		self.search(self._searchfield.text)

	def search(self, str):
		results = []	
			
		# Format search terms
		terms = [term.lower() for term in str.split()]
		
		# Search
		if len(terms) > 0:
			namespaces = self.engine.getModel().getNamespaces()
			for namesp in namespaces:
				objects = self.engine.getModel().getObjects(namesp)
				for obj in objects:
					doAppend = True
					for term in terms:
						if obj.getId().lower().find(term) < 0:
							doAppend = False
							break
					if doAppend:
						results.append(obj)
		else:
			results = None
		
		if self.mode == 'list':
			self.fillTextList(results)
		elif self.mode == 'preview':
			self.fillPreviewList(results)

	def fillTextList(self, objects=None):
		if objects is None:
			if self.namespaces.selected_item is None:
				return
			objects = self.engine.getModel().getObjects(self.namespaces.selected_item)
		
		class _ListItem:
			def __init__( self, name, namespace ):
				self.name = name
				self.namespace = namespace
			def __str__( self ):
				return self.name
			
		if self.namespaces.selected_item:
			self.objects.items = [_ListItem(obj.getId(), obj.getNamespace()) for obj in objects]
			if not self.objects.selected_item:
				self.objects.selected = 0
				self.listEntrySelected()

	def listEntrySelected(self):
		"""This function is used as callback for the TextList."""
		if self.objects.selected_item:
			object_id = self.objects.selected_item.name
			namespace = self.objects.selected_item.namespace
			obj = self.engine.getModel().getObject(object_id, namespace)
			self.objectSelected(obj)

	def fillPreviewList(self, objects=None):
		self.objects.clear()
		
		if objects is None:
			if self.namespaces.selected_item is None:
				return
			objects = self.engine.getModel().getObjects(self.namespaces.selected_item)
		
		for obj in objects:
			image = self._getImage(obj)
			if image is None:
				print 'No image available for selected object'
				image = ""

			callback = tools.callbackWithArguments(self.objectSelected, obj)	
			icon = ObjectIcon(callback=callback, image=image, text=obj.getId())
			self.objects.addChild(icon)
			
		if len(objects)>0:
			objects[0].selected = True
			self.objectSelected(objects[0])


	def objectSelected(self, obj):
		"""This is used as callback function to notify the editor that a new object has
		been selected.
		@param obj: fife.Object instance"""

		# Set preview image
		self.preview.image = self._getImage(obj)
		height = self.preview.image.getHeight();
		if height > 200: height = 200
		self.preview._getParent()._setHeight(height)
		
		self.gui.adaptLayout()
		self.editor.getActiveMapView().getController().selectObject(obj)

		self.objects.adaptLayout()
		self.gui.adaptLayout()		

	def update_namespace(self):
		
		self.namespaces.items = self.engine.getModel().getNamespaces()
		if not self.namespaces.selected_item:
			self.namespaces.selected = 0
		if self.mode == 'list':
			self.setTextList()
		elif self.mode == 'preview':
			self.setImageList()
		self.update()

	def update(self):
		if self.mode == 'list':
			self.fillTextList()
		elif self.mode == 'preview':
			self.fillPreviewList()

		self.mainScrollArea.resizeToContent()

	def _getImage(self, obj):
		""" Returns an image for the given object.
		@param: fife.Object for which an image is to be returned
		@return: fife.GuiImage"""
		visual = None
		try:
			visual = obj.get2dGfxVisual()
		except:
			print 'Visual Selection created for type without a visual?'
			raise

		# Try to find a usable image
		index = visual.getStaticImageIndexByAngle(0)
		image = None
		# if no static image available, try default action
		if index == -1:
			action = obj.getDefaultAction()
			if action:
				animation_id = action.get2dGfxVisual().getAnimationIndexByAngle(0)
				animation = self.engine.getAnimationPool().getAnimation(animation_id)
				image = animation.getFrameByTimestamp(0)
				index = image.getPoolId()

		# Construct the new GuiImage that is to be returned
		if index != -1:
			image = fife.GuiImage(index, self.engine.getImagePool())

		return image


	def show(self):
		self.update_namespace()
		self.gui.show()
		self._showAction.setChecked(True)

	def hide(self):
		self.gui.hide()
		self._showAction.setChecked(False)
		
	def toggle(self):
		if self.gui.isVisible():
			self.hide()
		else:
			self.show()
