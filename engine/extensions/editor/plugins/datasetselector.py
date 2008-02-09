# coding: utf-8

from pychan import widgets

class DatasetSelector(object):
	def __init__(self,engine,map,selectNotify):
		self.engine = engine
		self.map = map
		self.notify = selectNotify
		
		self.buildGui()
	
	def buildGui(self):
		self.gui = widgets.Window(title="Dataset selector")
		hbox = widgets.HBox(parent=self.gui)
		self.gui.add(hbox)
		scrollArea = widgets.ScrollArea(parent=hbox,size=(120,300))
		hbox.add(scrollArea)
		self.datasets = widgets.ListBox(parent=scrollArea)
		scrollArea.content = self.datasets
		scrollArea = widgets.ScrollArea(parent=hbox,size=(120,300))
		hbox.add(scrollArea)
		self.instances = widgets.ListBox(parent=scrollArea)
		scrollArea.content = self.instances

		hbox = widgets.HBox(parent=self.gui)
		self.gui.add(hbox)
		hbox.addSpacer( widgets.Spacer() )
		closeButton = widgets.Button(parent=hbox,text="Close")
		hbox.add( closeButton )
		closeButton.capture(self.hide)
		
		self.datasets.capture(self.updateInstances)
		self.datasets.items = [dat.Id() for dat in self.map.getDatasets()]
		self.datasets.selected = 0
		self.updateInstances()
	
		self.instances.capture(self.instanceSelected)

	
	def updateInstances(self):
		datid = self.datasets.selected_item
		self.selected_dataset = self.engine.getModel().getMetaModel().getDatasets('id', datid)[0]
		self.instances.items  = [obj.Id() for obj in self.selected_dataset.getObjects()]
		if not self.instances.selected_item:
			self.instances.selected = 0
		self.instanceSelected()

	def instanceSelected(self):
		self.selectedInstance = self.instances.selected_item
		object = self.selected_dataset.getObjects('id', self.selectedInstance)[0]
		self.notify(object)
	
	def show(self):
		self.gui.show()
	def hide(self):
		self.gui.hide()