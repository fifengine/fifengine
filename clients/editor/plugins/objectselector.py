# coding: utf-8

from pychan import widgets, tools, attrs
import fife

# TODO:
# - Scrollarea in previewmode does not display the last item properly
# - Better event handling in ObjectIcon

class ObjectIcon(widgets.VBox):
	""" The ObjectIcon is used to represent the object in the object selector.
	Unlike 
	"""	
	ATTRIBUTES = widgets.VBox.ATTRIBUTES + [ attrs.Attr("text"), attrs.Attr("image") ]
	
	def __init__(self,**kwargs):
		self.real_widget = fife.Container()
		super(ObjectIcon,self).__init__(_real_widget=self.real_widget,**kwargs)

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
		self.objects = widgets.VBox(name='list', size=(200,300))
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
			
			icon = ObjectIcon(image=image, text=obj.getId())
			icon.capture(tools.callbackWithArguments(self.objectSelected, obj), "mouseClicked")
			self.objects.addChild(icon)

		self.objects._recursiveResizeToContent()
		self.gui.adaptLayout()
			
		if len(objects)>0:
			self.objectSelected(objects[0])


	def objectSelected(self, obj):
		"""This is used as callback function to notify the editor that a new object has
		been selected.
		@param obj: fife.Object instance"""
		self.preview.image = self._getImage(obj)
		height = self.preview.image.getHeight();
		if height > 200: height = 200
		self.preview._getParent()._setHeight(height)
		self.gui.adaptLayout()
		self.notify(obj)

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
