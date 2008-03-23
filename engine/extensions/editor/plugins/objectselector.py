# coding: utf-8

from pychan import widgets
import fife

class ObjectSelector(object):
	def __init__(self,engine,map,selectNotify):
		self.engine = engine
		self.map = map
		self.notify = selectNotify
		
		self.buildGui()
	
	def buildGui(self):
		self.gui = widgets.Window(title="Object selector")
		hbox = widgets.HBox(parent=self.gui)
		self.gui.addChild(hbox)
		scrollArea = widgets.ScrollArea(parent=hbox,size=(120,300))
		hbox.addChild(scrollArea)
		self.namespaces = widgets.ListBox(parent=scrollArea)
		scrollArea.content = self.namespaces
		scrollArea = widgets.ScrollArea(parent=hbox,size=(120,300))
		hbox.addChild(scrollArea)
		self.objects = widgets.ListBox(parent=scrollArea)
		scrollArea.content = self.objects
		scrollArea = widgets.ScrollArea(parent=hbox, size=(120,300))
		hbox.addChild(scrollArea)
		self.preview = widgets.Icon()
		scrollArea.content = self.preview

		hbox = widgets.HBox(parent=self.gui)
		self.gui.addChild(hbox)
		hbox.addSpacer( widgets.Spacer() )
		closeButton = widgets.Button(parent=hbox,text="Close")
		hbox.addChild( closeButton )
		closeButton.capture(self.hide)
		
		self.namespaces.capture(self.updateObjects)
		self.namespaces.items = self.engine.getModel().getNamespaces()
		self.namespaces.selected = 0
		self.updateObjects()
	
		self.objects.capture(self.objectSelected)

	
	def updateObjects(self):
		if not self.namespaces.selected_item: return

		selected_namespace = self.namespaces.items[self.namespaces.selected_item]
		self.objects.items = [obj.getId() for obj in self.engine.getModel().getObjects(selected_namespace)]
		if not self.objects.selected_item:
			self.objects.selected = 0
		self.objectSelected()

	def objectSelected(self):
		if not self.objects.selected_item: return
		self.selected_object = self.objects.items[self.objects.selected_item]
		object = self.engine.getModel().getObjects(self.selected_object, self.namespace.items[self.namespaces.selected_item])
		self.notify(object)
		self._refreshPreview(object)

	def _refreshPreview(self, object):
		visual = None
		
		try:
			visual = object.get2dGfxVisual()
		except:
			print 'Visual Selection created for type without a visual?'
			raise	

		index = visual.getStaticImageIndexByAngle(0)
		if index == -1:
			print 'Visual missing static image.'
			return

		image = fife.GuiImage(index, self.engine.getImagePool())
		self.preview.image = image
		self.gui.adaptLayout()
	
	def show(self):
		self.gui.show()
	def hide(self):
		self.gui.hide()
