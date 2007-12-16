import fife
from loaders import loadMapFile
from savers import saveMapFile

from pychan import *

class InputListener(fife.IMouseListener, fife.IKeyListener):

	def __init__(self, eventmanager, callback):
		self.eventmanager = eventmanager

		fife.IMouseListener.__init__(self)
		self.eventmanager.addMouseListener(self)

		fife.IKeyListener.__init__(self)
		self.eventmanager.addKeyListener(self)

		self.callback = callback

		self.newTarget = None
		self.editmode = False

	def mousePressed(self, evt):
		self.newTarget = fife.ScreenPoint(evt.getX(), evt.getY())
		self.callback()

	def mouseReleased(self, evt):
		pass
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	def mouseWheelMovedUp(self, evt):
		pass
	def mouseWheelMovedDown(self, evt):
		pass
	def mouseMoved(self, evt):
		pass
	def mouseDragged(self, evt):
		pass

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		if keyval == fife.IKey.F1:
			self.editmode = not self.editmode
			if(self.editmode):
				print 'FIFEdit in edit mode'
			else:
				print 'FIFEdit leaving edit mode'

	def keyReleased(self, evt):
		pass


class FIFEdit(fife.IWidgetListener, object):

	def __init__(self, engine, maplist):
		self.engine = engine
		self.font = engine.getDefaultFont()
		self.font.setColor(0,0,0)
		self.eventmanager = engine.getEventManager()
		self.map = 0

		fife.IWidgetListener.__init__(self)
		self.eventmanager.addWidgetListener(self)

		self.guimanager = engine.getGuiManager()
		self.widgets = []
		self.images = []

		self.screenheight = engine.getRenderBackend().getScreenHeight()
		self.screenwidth = engine.getRenderBackend().getScreenWidth()

		self.map_list = GenericListmodel()
		self.map_list.extend(maplist)

		self.guiroot = Container(self.guimanager)

		#bb = ButtonBox(self.eventmanager, self.guimanager, self.guiroot, 'test', 'select an option:', 'ok', 'quit')

		self.camera = 0
		self.edit_layer = 0
		self.edit_object = 0
		self.datedit = 0

		self.inputlistener = InputListener(self.eventmanager, self.input)

	def show(self):
		self.create_mainpanel()
		self.create_mapdialogs()
	
	def hide(self):
		pass

	def input(self):
		if self.inputlistener.editmode and self.inputlistener.newTarget and self.camera:
			ec = self.camera.toElevationCoordinates(self.inputlistener.newTarget)
			self.inputlistener.newTarget = None
			if (not self.edit_layer):
				self.edit_layer = self.camera.getLocation().getLayer()
			lc = self.edit_layer.getCellGrid().toLayerCoordinates(ec)
			if self.datedit:
				inst = self.edit_layer.createInstance(self.datedit.getSelectedObject(), lc)
				fife.InstanceVisual.create(inst)
	
	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'LoadMapDialog':
			self.mapwnd.setVisible(not self.mapwnd.isVisible())

		elif evtid == 'SaveMapDialog':
			self.savewnd.setVisible(not self.savewnd.isVisible())

		elif evtid == 'LoadDatasetDialog':
			print 'Not implemented yet.'

		elif evtid == 'SaveDatasetDialog':
			print 'Not implemented yet.'

		elif evtid == 'LoadMapEvt':
			mapfilename = self.map_list[self.level_drop.getSelected()]
			print 'loading: ' + mapfilename

			self.map = loadMapFile(mapfilename, self.engine)
			self.mapwnd.setVisible(False)
			self.create_mapedit()

		elif evtid == 'SaveMapEvt':
			if(self.map):
				mapfilename = self.path_box.getText()
				print 'saving: ' + mapfilename
				self.save_mapmetadata()
				saveMapFile(mapfilename, self.engine, self.map)
				self.savewnd.setVisible(False)
			else:
				print 'No map available to save.'

		elif evtid == 'EditMapDatasets':
			self.map_datwnd.setVisible(not self.map_datwnd.isVisible())

		elif evtid == 'EditDataset':
			self.create_datedit(self.dataset_list[self.dataset_drop.getSelected()])

	def register_widget(self, w, container):
		self.widgets.append(w)
		container.add(w)

	def create_mainpanel(self):
		panel = fife.Container()
		panel.setOpaque(True)
		self.register_widget(panel, self.guimanager)

		panel_width = 0
		
		label1 = fife.Label('FIFEdit 2007.3')
		label1.setPosition(1, 0)
		label1.setFont(self.font)
		self.register_widget(label1, panel)
		panel_width += 1 + label1.getWidth() + 1

		button1 = fife.Button('Load Map')
		button1.setActionEventId('LoadMapDialog')
		button1.addActionListener(self.guimanager)
		button1.adjustSize()
		button1.setPosition(1 + panel_width, 0)
		self.register_widget(button1, panel)
		panel_width += 1 + button1.getWidth() + 1

		button2 = fife.Button('Save Map')
		button2.setActionEventId('SaveMapDialog')
		button2.addActionListener(self.guimanager)
		button2.adjustSize()
		button2.setPosition(1 + panel_width, 0)
		self.register_widget(button2, panel)
		panel_width += 1 + button2.getWidth() + 1

		button3 = fife.Button('Load Dataset')
		button3.setActionEventId('LoadDatasetDialog')
		button3.addActionListener(self.guimanager)
		button3.adjustSize()
		button3.setPosition(1 + panel_width, 0)
		self.register_widget(button3, panel)
		panel_width += 1 + button3.getWidth() + 1

		button4 = fife.Button('Save Dataset')
		button4.setActionEventId('SaveDatasetDialog')
		button4.addActionListener(self.guimanager)
		button4.adjustSize()
		button4.setPosition(1 + panel_width, 0)
		self.register_widget(button4, panel)
		panel_width += 1 + button4.getWidth() + 1

		panel.setSize(panel_width, 30) 
		panel.setPosition(2, self.screenheight - panel.getHeight() - 2)

	def create_mapdialogs(self):
		sx,sy = 400, 80

		self.mapwnd = fife.Window('Choose a Map')
		self.mapwnd.setTitleBarHeight(20)
		self.mapwnd.setPosition(100,100)
		self.mapwnd.setSize(sx, sy)
		self.mapwnd.setVisible(False)
		self.register_widget(self.mapwnd, self.guimanager) 

		self.level_drop = fife.DropDown(self.map_list)
		self.level_drop.setSelected(0)
		print 'Default level selection: ' + str(self.level_drop.getSelected())
		self.level_drop.setPosition(5, 10)
		self.level_drop.setSize(sx - 150, 16)
		self.level_drop.setActionEventId('LoaderListEvt')
		self.level_drop.addActionListener(self.guimanager)
		self.register_widget(self.level_drop, self.mapwnd)
		
		loadbtn = fife.Button("Load Map")
		loadbtn.setSize(100,16)
 		loadbtn.setPosition(sx - loadbtn.getWidth(), 10)
		loadbtn.adjustSize()
		loadbtn.setActionEventId('LoadMapEvt')
		loadbtn.addActionListener(self.guimanager)
		self.register_widget(loadbtn, self.mapwnd)

		self.savewnd = fife.Window('Choose file to save to')
		self.savewnd.setTitleBarHeight(20)
		self.savewnd.setPosition(100,200)
		self.savewnd.setSize(sx, sy)
		self.savewnd.setVisible(False)
		self.register_widget(self.savewnd, self.guimanager) 

		self.path_box = fife.TextField('Replace this with the path/filename')
		self.path_box.setPosition(5, 10)
		self.path_box.setSize(sx - 150, 16)
		self.register_widget(self.path_box, self.savewnd)

		savebtn = fife.Button("Save Map")
		savebtn.setSize(100,16)
 		savebtn.setPosition(sx - savebtn.getWidth(), 10)
		savebtn.adjustSize()
		savebtn.setActionEventId('SaveMapEvt')
		savebtn.addActionListener(self.guimanager)
		self.register_widget(savebtn, self.savewnd)
	
	def create_mapedit(self):
		assert self.map

		self.create_mapdataset_edit()

		sx,sy = 500,300

		self.mapedit = fife.Window('Map Property Editor:')
		self.mapedit.setTitleBarHeight(20)
		self.mapedit.setPosition(100,300)
		self.mapedit.setSize(sx,sy)
		self.mapedit.setVisible(True)
		self.register_widget(self.mapedit, self.guimanager)

		dy = 0

		mapid_label = fife.Label('Map Identifier: ')
		mapid_label.setPosition(1, 1)
		mapid_label.setFont(self.font)
		self.register_widget(mapid_label, self.mapedit)

		self.mapid_field = fife.TextField(self.map.Id())
		self.mapid_field.setPosition(1 + mapid_label.getWidth() + 1, 1)
		self.mapid_field.setWidth(300)
		self.register_widget(self.mapid_field, self.mapedit)

		dy = 1 + self.mapid_field.getHeight() + 1

		list = self.map.listFields()
		self.map_metafields = []
		for i in range(0, len(list)):
			label = fife.Label(list[i])
			label.setPosition(1, dy)
			self.register_widget(label, self.mapedit)

			textfield = fife.TextField(self.map.get(list[i]))

			textfield.setWidth(300)
			textfield.setPosition(1 + label.getWidth() + 1, dy)
			self.register_widget(textfield, self.mapedit)
			self.map_metafields.append((list[i], textfield))

			dy += textfield.getHeight() + 1

		datbutton = fife.Button('Dataset Inclusions')
		datbutton.setActionEventId('EditMapDatasets')
		datbutton.addActionListener(self.guimanager)
		datbutton.adjustSize()
		datbutton.setPosition(1, sy - 55)
		self.register_widget(datbutton, self.mapedit)

	def save_mapmetadata(self):
		self.map.set('id', self.mapid_field.getText())
		for i in range(0, len(self.map_metafields)):
			self.map.set(self.map_metafields[i][0], self.map_metafields[i][1].getText())

	def create_mapdataset_edit(self):
		sx,sy = 500, 200

		self.map_datwnd = fife.Window('Edit map dataset inclusions:')
		self.map_datwnd.setTitleBarHeight(20)
		self.map_datwnd.setPosition(400,100)
		self.map_datwnd.setSize(sx, sy)
		self.map_datwnd.setVisible(False)
		self.register_widget(self.map_datwnd, self.guimanager) 

		self.dataset_list = GenericListmodel()
		self.dataset_list.extend([dat.Id() for dat in self.map.getDatasets()])
		self.dataset_drop = fife.DropDown(self.dataset_list)
		self.dataset_drop.setSelected(0)
		self.dataset_drop.setPosition(5, 10)
		self.dataset_drop.setSize(sx - 150, 16)
		self.dataset_drop.setActionEventId('DatasetListEvt')
		self.dataset_drop.addActionListener(self.guimanager)
		self.register_widget(self.dataset_drop, self.map_datwnd)

		editbutton = fife.Button('Edit')
		editbutton.setActionEventId('EditDataset')
		editbutton.addActionListener(self.guimanager)
		editbutton.adjustSize()
		editbutton.setPosition(sx - editbutton.getWidth() - 10, 10)
		self.register_widget(editbutton, self.map_datwnd)

		rmbutton = fife.Button('Remove')
		rmbutton.setActionEventId('RemoveMapDataset')
		rmbutton.addActionListener(self.guimanager)
		rmbutton.adjustSize()
		rmbutton.setPosition(sx - rmbutton.getWidth() - 10, 10 + editbutton.getHeight())
		self.register_widget(rmbutton, self.map_datwnd)

		add_label = fife.Label('Add new dataset: ')
		add_label.setPosition(5, 10 + editbutton.getHeight() + rmbutton.getHeight())
		add_label.setFont(self.font)
		add_label.adjustSize()
		self.register_widget(add_label, self.map_datwnd)

		self.datasetadd_field = fife.TextField()
		self.datasetadd_field.setPosition(5 + add_label.getWidth() + 1, 10 + editbutton.getHeight() + rmbutton.getHeight())
		self.datasetadd_field.setWidth(250)
		self.register_widget(self.datasetadd_field, self.map_datwnd)

		addbutton = fife.Button('Add')
		addbutton.setActionEventId('AddMapDataset')
		addbutton.addActionListener(self.guimanager)
		addbutton.adjustSize()
		addbutton.setPosition(sx - addbutton.getWidth() - 10, 10 + editbutton.getHeight() + rmbutton.getHeight())
		self.register_widget(addbutton, self.map_datwnd)
		
	def create_datedit(self, dataset):
		query = self.engine.getModel().getMetaModel().getDatasets('id', dataset)
		assert len(query) > 0, 'Reference to non-existent dataset.'
		dat = query[0]
		if(self.datedit):
			self.guiroot.remove_widget(self.datedit)
		self.datedit = DatasetEditor(self.eventmanager, self.guimanager, self.guiroot, self.engine.getImagePool(), dat)

	def edit_camview(self, camera):
		self.camera = camera

class Container:
	def __init__(self, guimanager):
		self.guimanager = guimanager
		self.widgets = []

	def add_widget(self, w):
		self.widgets.append(w)
		self.guimanager.add(w)

	def remove_widget(self, w):
		self.guimanager.remove(w)
#TODO: removing final reference (destroying) the widget results in segfault. So for now, old gui
# elements just hang around in the sytem.
#		self.widgets.remove(w)

class Dialog(fife.IWidgetListener, fife.Window, Container):
	def __init__(self, event_manager, guimanager, parent, caption, position, size):
		self.eventmanager = event_manager
		fife.IWidgetListener.__init__(self)
		self.eventmanager.addWidgetListener(self)

		fife.Window.__init__(self, caption)
		self.setTitleBarHeight(20)
		self.setPosition(position[0], position[1])
		self.setSize(size[0], size[1])
		self.setVisible(True)

		Container.__init__(self, guimanager)
		self.parent = parent
		self.parent.add_widget(self)

	def __del__(self):
		for child in self.widgets:
			self.remove_widget(child)
		self.eventmanager.removeWidgetListener(self)
		print 'deleting'

	def add_widget(self, w):
		self.widgets.append(w)
		self.add(w)

	def remove_widget(self, w):
		self.remove(w)
		self.widgets.remove(w)

	def onWidgetAction(self, evt):
		pass

class Form(Dialog):
	def __init__(self, event_manager, gui_manager, parent, caption, position, size):
		Dialog.__init__(self, event_manager, gui_manager, parent, caption, position, size)

		self.clbutton = fife.Button('Close')
		self.clbutton.setActionEventId('CloseEvt')
		self.clbutton.addActionListener(self.guimanager)
		self.clbutton.adjustSize()
		self.clbutton.setPosition(self.getWidth() - self.clbutton.getWidth() - 5, 5)
		self.add_widget(self.clbutton)

	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'CloseEvt':
			if(evt.getSourceWidget().this == self.clbutton.this):
				self.parent.remove_widget(self)

class ButtonBox(Dialog):
	def __init__(self, event_manager, gui_manager, parent, caption, prompt, optionA, optionB):
		self.size = (200, 100)
		self.position = (300,300)

		Dialog.__init__(self, event_manager, gui_manager, parent, caption, self.position, self.size)

		label = fife.Label(prompt + ':')
		label.setPosition(5, 5)
		#label.setFont(self.font)
		label.adjustSize()
		self.add_widget(label)

		self.buttonA = fife.Button(optionA)
		self.buttonA.setActionEventId('optionA')
		self.buttonA.addActionListener(self.guimanager)
		self.buttonA.setSize(50,25)
		self.buttonA.setPosition(45, 5 + label.getHeight() + 5)
		self.add_widget(self.buttonA)

		self.buttonB = fife.Button(optionB)
		self.buttonB.setActionEventId('optionB')
		self.buttonB.addActionListener(self.guimanager)
		self.buttonB.setSize(50, 25)
		self.buttonB.setPosition(100, self.buttonA.getY())
		self.add_widget(self.buttonB)

	def onWidgetAction(self, evt):
		evtid = evt.getId()
		src = evt.getSourceWidget()
		if src.this == self.buttonA.this or src.this == self.buttonB.this:
			if evtid == 'optionA':
				print 'First option selected'
			elif evtid == 'optionB':
				print 'Second option selected'
			self.parent.remove_widget(self)

class DatasetEditor(Form):
	def __init__(self, event_manager, gui_manager, parent, imagepool, dataset):
		self.size = (400, 200) 
		self.position = (0, 100)

		Form.__init__(self, event_manager, gui_manager, parent, 'Dataset Editor:' , self.position, self.size)
		self.dataset = dataset
		self.imagepool = imagepool

		#scrollarea = fife.ScrollArea()
		#scrollarea.setContent(self.datedit)

		self.objects = self.dataset.getObjects()

		self.object_list = GenericListmodel()
		self.object_list.extend([object.Id() for object in self.objects])

		self.object_drop = fife.DropDown(self.object_list)
		self.object_drop.setSelected(0)
		print 'Default object selection: ' + str(self.object_drop.getSelected())
		self.object_drop.setPosition(5, 5)
		self.object_drop.setSize(250, 16)
		self.object_drop.setActionEventId('ObjectSelect')
		self.object_drop.addActionListener(self.guimanager)
		self.add_widget(self.object_drop)

		self.icon = 0

		self.refresh_preview()

	def getSelectedObject(self):
		return self.objects[self.object_drop.getSelected()]

	def refresh_preview(self):
		visual = self.objects[self.object_drop.getSelected()].get2dGfxVisual()
		index = visual.getStaticImageIndexByAngle(0)
		if (index == -1):
			print 'object missing static image'
		else:
			self.preview = fife.GuiImage(index, self.imagepool)
			if (not self.icon):
				self.icon = fife.Icon(self.preview)
				self.icon.setPosition(10, 5 + self.object_drop.getHeight() + 5)
				self.add_widget(self.icon)
			else:
				self.icon.setImage(self.preview)

			self.icon.setSize(self.preview.getWidth(),self.preview.getHeight())

	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'ObjectSelect':
			self.refresh_preview()
		else:
			Form.onWidgetAction(self, evt)
