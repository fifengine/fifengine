# coding: utf-8

from pychan import widgets, tools, attrs, internal
import fife
from fife import Color
import pdb
#from internal import DEFAULT_STYLE

# TODO:
# - Better event handling in ObjectIcon and ObjectIconList
# - Label background color can't be set
# - Model-View design for ObjectIconList

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
	def __init__(self,**kwargs):
		super(ObjectIconList, self).__init__(max_size=(5000,500000), **kwargs)
		self.base_color = self.background_color

		# TODO: Pychan doesn't support keyevents for nonfocusable widgets, yet
		self.capture(self._keyPressed, "keyPressed")
		self.capture(self._keyPressed, "keyReleased")
		self._selectedItem = None

	def _keyPressed(self, event):
		print "KeyEvent", event

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
	
class ObjectSelector(object):
	"""The ObjectSelector class offers a gui Widget that let's you select the object you
	wish to use to in the editor.
	@param engine: fife instance
	@param map: fife.Map instance containing your map
	@param selectNotify: callback function used to tell the editor you selected an object.
	"""
	def __init__(self, engine, map, selectNotify):
		self.engine = engine
		self.map = map
		self.notify = selectNotify
		self.mode = 'list' # Other mode is 'preview'

		self.buildGui()

	def buildGui(self):
		# Create the main Window
		self.gui = widgets.Window(title="Object selector")
		vbox = widgets.VBox()
		self.gui.addChild(vbox)

		# Add the drop down with list of namespaces
		self.namespaces = widgets.DropDown()
		vbox.addChild(self.namespaces)
		
		self.namespaces.items = self.engine.getModel().getNamespaces()
		self.namespaces.selected = 0

		# Events for namespacelist
		# TODO: Replace with SelectionEvent, once pychan supports it
		self.namespaces.capture(self.update_namespace, "action")
		self.namespaces.capture(self.update_namespace, "mouseWheelMovedUp")
		self.namespaces.capture(self.update_namespace, "mouseWheelMovedDown")
		self.namespaces.capture(self.update_namespace, "keyReleased")

		# This scrollarea is used to display the preview images
		self.mainScrollArea = widgets.ScrollArea(size=(230,300))
		self.objects = None
		if self.mode == 'list':
			self.setTextList()
		else: # Assuming self.mode is 'preview'
			self.setImageList()
		vbox.addChild(self.mainScrollArea)

		# Add another Hbox to hold the close button
		hbox = widgets.HBox(parent=self.gui)
		vbox.addChild(hbox)
		hbox.addSpacer(widgets.Spacer())
		toggleButton = widgets.Button(text="Toggle Preview Mode")
		toggleButton.capture(self.toggleMode)
		hbox.addChild(toggleButton)
		closeButton = widgets.Button(text="Close")
		closeButton.capture(self.hide)
		hbox.addChild(closeButton)

		# This is the preview area
		scrollArea = widgets.ScrollArea(size=(230,1))
		vbox.addChild(scrollArea)
		self.preview = widgets.Icon()
		scrollArea.addChild(self.preview)
		scrollArea._setBackgroundColor(self.gui._getBaseColor())

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

	def fillTextList(self):
		if self.namespaces.selected_item:
			self.objects.items = [obj.getId() for obj in self.engine.getModel().getObjects(self.namespaces.selected_item)]
			if not self.objects.selected_item:
				self.objects.selected = 0
				self.listEntrySelected()

	def listEntrySelected(self):
		"""This function is used as callback for the TextList."""
		if self.objects.selected_item:
			obj = self.engine.getModel().getObject(self.objects.selected_item, self.namespaces.selected_item)
			self.objectSelected(obj)

	def fillPreviewList(self):
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
		self.notify(obj)

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
		""" Returs an image for the given object.
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

	def hide(self):
		self.gui.hide()
