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
		self.namespaces.capture(self.fillList)
		self.namespaces.items = self.engine.getModel().getNamespaces()
		self.namespaces.selected = 0
		scrollArea.addChild(self.namespaces)

		# This Vbox is used to display the preview images
		scrollArea = widgets.ScrollArea(size=(200,300))
		self.objects = widgets.VBox(size=(200,300))
		scrollArea.addChild(self.objects)
		hbox.addChild(scrollArea)

		# Add another Hbox to hold the close button
		hbox = widgets.HBox(parent=self.gui)
		self.gui.addChild(hbox)
		hbox.addSpacer(widgets.Spacer())
		closeButton = widgets.Button(text="Close")
		closeButton.capture(self.hide)
		hbox.addChild(closeButton)


	def update(self):
		self.namespaces.items = self.engine.getModel().getNamespaces()
		self.namespaces.selected = 0
		self.fillList()

	def fillList(self):
		for obj in self.engine.getModel().getObjects(self.namespaces.selected_item):
			# Find visual for the object
			visual = None
			try:
				visual = obj.get2dGfxVisual()
			except:
				print 'Visual Selection created for type without a visual?'
				raise

			# Try to find a usable image
			index = visual.getStaticImageIndexByAngle(0)

			# if no static image available, try default action
			if index == -1:
				action = obj.getDefaultAction()
				if action:
					animation_id = action.get2dGfxVisual().getAnimationIndexByAngle(0)
					animation = self.engine.getAnimationPool().getAnimation(animation_id)
					image = animation.getFrameByTimestamp(0)
					index = image.getPoolId()

			# Use the hopefully found image to set up the imagebutton
			if index != -1:
				image = fife.GuiImage(index, self.engine.getImagePool())
				imagebutton = widgets.ImageButton(up_image=image, down_image=image, hover_image=image)
				imagebutton.capture(tools.callbackWithArguments(self.objectSelected, obj))
				self.objects.addChild(imagebutton)
				self.objects._recursiveResizeToContent()
				self.gui.adaptLayout()
			else:
				print 'No image available for selected object'

	def objectSelected(self, obj):
		"""This is used as callback function to notify the editor that a new object has
		been selected.
		@param obj: fife.Object instance"""
		self.notify(obj)

	def show(self):
		self.update()
		self.gui.show()

	def hide(self):
		self.gui.hide()
