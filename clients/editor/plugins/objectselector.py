# coding: utf-8

from pychan import widgets, tools
import fife

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
		hbox = widgets.HBox()
		self.gui.addChild(hbox)

		# Add the Scrollarea with list of namespaces
		scrollArea = widgets.ScrollArea(size=(200,300))
		hbox.addChild(scrollArea)
		self.namespaces = widgets.ListBox()
		self.namespaces.capture(self.update_namespace)
		self.namespaces.items = self.engine.getModel().getNamespaces()
		self.namespaces.selected = 0
		scrollArea.addChild(self.namespaces)

		# This Vbox is used to display the preview images
		self.mainScrollArea = widgets.ScrollArea(size=(200,300))
		self.objects = None
		if self.mode == 'list':
			self.setTextList()
		else: # Assuming self.mode is 'preview'
			self.setImageList()
		hbox.addChild(self.mainScrollArea)

		# This is the preview area
		scrollArea = widgets.ScrollArea(size=(200,300))
		hbox.addChild(scrollArea)
		self.preview = widgets.Icon()
		scrollArea.addChild(self.preview)

		# Add another Hbox to hold the close button
		hbox = widgets.HBox(parent=self.gui)
		self.gui.addChild(hbox)
		hbox.addSpacer(widgets.Spacer())
		toggleButton = widgets.Button(text="Toggle Preview Mode")
		toggleButton.capture(self.toggleMode)
		hbox.addChild(toggleButton)
		closeButton = widgets.Button(text="Close")
		closeButton.capture(self.hide)
		hbox.addChild(closeButton)

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
			if image is not None:
				imagebutton = widgets.ImageButton(up_image=image, down_image=image, hover_image=image)
				imagebutton.capture(tools.callbackWithArguments(self.objectSelected, obj))
				self.objects.addChild(imagebutton)
				self.objects._recursiveResizeToContent()
				self.gui.adaptLayout()
			else:
				print 'No image available for selected object'
		if len(objects)>0:
			self.objectSelected(objects[0])


	def objectSelected(self, obj):
		"""This is used as callback function to notify the editor that a new object has
		been selected.
		@param obj: fife.Object instance"""
		self.preview.image = self._getImage(obj)
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
