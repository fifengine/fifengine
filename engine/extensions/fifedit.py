import fife
from loaders import loadMapFile
from savers import saveMapFile

from pychan import *

import os

class InputListener(fife.IMouseListener, fife.IKeyListener):

	def __init__(self, eventmanager, callback):
		self.eventmanager = eventmanager

		fife.IMouseListener.__init__(self)
		self.eventmanager.addMouseListener(self)

		fife.IKeyListener.__init__(self)
		self.eventmanager.addKeyListener(self)

		self.callback = callback

		self.newTarget = None
		self.make_inst = False
		self.delete_inst = False
		self.show_layers = False
		self.open_file = False

		self.togglehide = False

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
		keystr = evt.getKey().getAsString().lower()
		if keyval == fife.IKey.F2:
			self.togglehide = True
			self.callback()
		elif keystr == 'm':
			self.make_inst = True
			self.callback()
		elif keystr == 'x':
			self.delete_inst = True
			self.callback()
		elif keystr == 'l':
			self.show_layers = True
			self.callback()
		elif keystr == 'o':
			self.open_file = True
			self.callback()

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

		self.guiroot = Container(self.guimanager)

		#bb = ButtonBox(self.eventmanager, self.guimanager, self.guiroot, 'test', 'select an option:', 'ok', 'quit')

		self.map_edit = 0
		self.map_datedit = 0

		self.camera = 0
		self.edit_layer = 0
		self.edit_object = 0
		self.datedit = 0

		self.selection = 0

		self.layerselect = 0
		self.filebrowser = 0

		self.inputlistener = InputListener(self.eventmanager, self.input)

	def show(self):
		self.create_mainpanel()
		self.create_mapdialogs()
	
	def hide(self):
		pass

	def input(self):
		if (not self.edit_layer):
			self.edit_layer = self.camera.getLocation().getLayer()

		if self.inputlistener.newTarget and self.camera:
			ec = self.camera.toElevationCoordinates(self.inputlistener.newTarget)
			self.selection = self.edit_layer.getCellGrid().toLayerCoordinates(ec)
			self.inputlistener.newTarget = None
			print 'Selection is: (' + str(self.selection.x) + ',' + str(self.selection.y) + ')'

		elif self.inputlistener.togglehide:
			self.guiroot.togglehide()
			self.inputlistener.togglehide = False

		elif self.inputlistener.make_inst:
			inst = self.edit_layer.createInstance(self.datedit.getSelectedObject(), self.selection)
			fife.InstanceVisual.create(inst)	
			self.inputlistener.make_inst = False

		elif self.inputlistener.delete_inst:
			instlist = self.edit_layer.getInstances('loc', str(self.selection.x) + ',' + str(self.selection.y))
			print [inst.getObject().Id() for inst in instlist]
			for inst in instlist:
				self.edit_layer.deleteInstance(inst)
			self.inputlistener.delete_inst = False

		elif self.inputlistener.show_layers:
			self.create_layerselect(self.camera.getLocation().getElevation())
			self.inputlistener.show_layers = False

		elif self.inputlistener.open_file:
			self.create_filebrowser('/home/jwt/fife/trunk/clients/techdemo')
			self.inputlistener.open_file = False
	
	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'LoadMapDialog':
			self.map_list.extend([map.Id() for map in self.engine.getModel().getMaps()])
			self.mapwnd.setVisible(not self.mapwnd.isVisible())

		elif evtid == 'SaveMapDialog':
			self.savewnd.setVisible(not self.savewnd.isVisible())

		elif evtid == 'LoadDatasetDialog':
			print 'Not implemented yet.'

		elif evtid == 'SaveDatasetDialog':
			print 'Not implemented yet.'

		elif evtid == 'LoadMapEvt':
			self.map = self.engine.getModel().getMaps('id', self.map_list[self.level_drop.getSelected()])[0]
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
			self.create_mapdataset_edit()

		elif evtid == 'EditDataset':
			if(self.map_datedit):
				self.create_datedit(self.map_datedit.dataset_list[self.map_datedit.dataset_drop.getSelected()])

		elif evtid == 'SelectLayer':
			if(self.layerselect):
				print 'hi'
				self.edit_layer = self.layerselect.getLayer()

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
		
		if self.map_edit:
			self.guiroot.remove_widget(self.map_edit)
		self.mapedit = MapEdit(self.eventmanager, self.guimanager, self.guiroot, self.engine.getImagePool(), self.map)

	def save_mapmetadata(self):
		self.map.set('id', self.mapedit.mapid_field.getText())
		for i in range(0, len(self.mapedit.map_metafields)):
			self.map.set(self.mapedit.map_metafields[i][0], self.mapedit.map_metafields[i][1].getText())

	def create_mapdataset_edit(self):
		assert self.map

		if self.map_datedit:
			self.guiroot.remove_widget(self.map_datedit)
		self.map_datedit = MapDatasetEditor(self.eventmanager, self.guimanager, self.guiroot, self.engine.getImagePool(), self.map)
		
	def create_datedit(self, dataset):
		query = self.engine.getModel().getMetaModel().getDatasets('id', dataset)
		assert len(query) > 0, 'Reference to non-existent dataset.'
		dat = query[0]
		if(self.datedit):
			self.guiroot.remove_widget(self.datedit)
		self.datedit = DatasetEditor(self.eventmanager, self.guimanager, self.guiroot, self.engine.getImagePool(), dat)

	def create_layerselect(self, elevation):
		if(self.layerselect):
			self.guiroot.remove_widget(self.layerselect)
		self.layerselect = LayerSelect(self.eventmanager, self.guimanager, self.guiroot, elevation)

	def create_filebrowser(self, path):
		if(self.filebrowser):
			self.guiroot.remove_widget(self.filebrowser)
		self.filebrowser = FileBrowser(self.eventmanager, self.guimanager, self.guiroot, path)

	def edit_camview(self, camera):
		self.camera = camera

class Container:
	def __init__(self, guimanager):
		self.guimanager = guimanager
		self.widgets = []

	def togglehide(self):
		for child in self.widgets:
			child.setVisible(not child.isVisible())

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
		self.clbutton.setPosition(self.getWidth() - self.clbutton.getWidth() - 5, self.getHeight() - self.clbutton.getHeight() - 20)
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

class MapEdit(Form):
	def __init__(self, event_manager, gui_manager, parent, imagepool, map):
		self.size = (500, 300)
		self.position = (100,300)

		Form.__init__(self, event_manager, gui_manager, parent, map.Id() + ' Property Editor', self.position, self.size)

		self.map = map

		dy = 0

		mapid_label = fife.Label('Map Identifier: ')
		mapid_label.setPosition(1, 1)
		self.add_widget(mapid_label)

		self.mapid_field = fife.TextField(self.map.Id())
		self.mapid_field.setPosition(1 + mapid_label.getWidth() + 1, 1)
		self.mapid_field.setWidth(300)
		self.add_widget(self.mapid_field)

		dy = 1 + self.mapid_field.getHeight() + 1

		list = self.map.listFields()
		self.map_metafields = []
		for i in range(0, len(list)):
			label = fife.Label(list[i])
			label.setPosition(1, dy)
			self.add_widget(label)

			textfield = fife.TextField(self.map.get(list[i]))

			textfield.setWidth(300)
			textfield.setPosition(1 + label.getWidth() + 1, dy)
			self.add_widget(textfield)
			self.map_metafields.append((list[i], textfield))

			dy += textfield.getHeight() + 1

		datbutton = fife.Button('Dataset Inclusions')
		datbutton.setActionEventId('EditMapDatasets')
		datbutton.addActionListener(self.guimanager)
		datbutton.adjustSize()
		datbutton.setPosition(1, self.size[1] - 55)
		self.add_widget(datbutton)



class MapDatasetEditor(Form):
	def __init__(self, event_manager, gui_manager, parent, imagepool, map):
		self.size = (500,200)
		self.position = (400,100)
		
		Form.__init__(self, event_manager, gui_manager, parent, 'Edit ' + map.Id() + ' dataset inclusions:' , self.position, self.size)

		self.map = map

		self.dataset_list = GenericListmodel()
		self.dataset_list.extend([dat.Id() for dat in self.map.getDatasets()])
		self.dataset_drop = fife.DropDown(self.dataset_list)
		self.dataset_drop.setSelected(0)
		self.dataset_drop.setPosition(5, 10)
		self.dataset_drop.setSize(self.size[0] - 150, 16)
		self.dataset_drop.setActionEventId('DatasetListEvt')
		self.dataset_drop.addActionListener(self.guimanager)
		self.add_widget(self.dataset_drop)

		editbutton = fife.Button('Edit')
		editbutton.setActionEventId('EditDataset')
		editbutton.addActionListener(self.guimanager)
		editbutton.adjustSize()
		editbutton.setPosition(self.size[0] - editbutton.getWidth() - 10, 10)
		self.add_widget(editbutton)

		rmbutton = fife.Button('Remove')
		rmbutton.setActionEventId('RemoveMapDataset')
		rmbutton.addActionListener(self.guimanager)
		rmbutton.adjustSize()
		rmbutton.setPosition(self.size[0] - rmbutton.getWidth() - 10, 10 + editbutton.getHeight())
		self.add_widget(rmbutton)

		add_label = fife.Label('Add new dataset: ')
		add_label.setPosition(5, 10 + editbutton.getHeight() + rmbutton.getHeight())
		add_label.adjustSize()
		self.add_widget(add_label)

		self.datasetadd_field = fife.TextField()
		self.datasetadd_field.setPosition(5 + add_label.getWidth() + 1, 10 + editbutton.getHeight() + rmbutton.getHeight())
		self.datasetadd_field.setWidth(250)
		self.add_widget(self.datasetadd_field)

		addbutton = fife.Button('Add')
		addbutton.setActionEventId('AddMapDataset')
		addbutton.addActionListener(self.guimanager)
		addbutton.adjustSize()
		addbutton.setPosition(self.size[0] - addbutton.getWidth() - 10, 10 + editbutton.getHeight() + rmbutton.getHeight())
		self.add_widget(addbutton)

class LayerSelect(Form):
	def __init__(self, event_manager, gui_manager, parent, elevation):
		self.size = (400,100)
		self.position = (400,100)
		
		Form.__init__(self, event_manager, gui_manager, parent, 'Select ' + elevation.Id() + ' layer:' , self.position, self.size)

		self.elevation = elevation

		self.layer_list = GenericListmodel()
		self.layer_list.extend([layer.Id() for layer in self.elevation.getLayers()])
		self.layer_drop = fife.DropDown(self.layer_list)
		self.layer_drop.setSelected(0)
		self.layer_drop.setPosition(5, 10)
		self.layer_drop.setSize(self.size[0] - 150, 16)
		self.layer_drop.setActionEventId('LayerListEvt')
		self.layer_drop.addActionListener(self.guimanager)
		self.add_widget(self.layer_drop)

		button = fife.Button('Select')
		button.setActionEventId('SelectLayer')
		button.addActionListener(self.guimanager)
		button.adjustSize()
		button.setPosition(self.size[0] - button.getWidth() - 10, 10)
		self.add_widget(button)

	def getLayer(self):
		return self.elevation.getLayers('id', self.layer_list[self.layer_drop.getSelected()])[0]

class FileBrowser(Form):

	def __init__(self, event_manager, gui_manager, parent, path):
		self.size = (400,400)
		self.position = (300,100)

		Form.__init__(self, event_manager, gui_manager, parent, 'File Browser', self.position, self.size)

		self.path = path

		self.dir_list = GenericListmodel('..')
		self.dir_list.extend([dir for dir in os.listdir(self.path) if not os.path.isfile(os.path.join(self.path,dir))])

		self.dir_box = fife.ListBox(self.dir_list)
		self.dir_box.setSelected(0)
		self.dir_box.setPosition(5,5)
		self.dir_box.setSize(150,400)
		self.dir_box.setActionEventId('ChangeDirectory')
		self.dir_box.addActionListener(self.guimanager)
		self.add_widget(self.dir_box)

		self.file_list = GenericListmodel()
		self.file_list.extend([file for file in os.listdir(self.path) if os.path.isfile(os.path.join(self.path,file))])

		self.file_box = fife.ListBox(self.file_list)
		self.file_box.setSelected(0)
		self.file_box.setPosition(5 + self.dir_box.getWidth() + 5,5)
		self.file_box.setSize(150,400)
		self.file_box.setActionEventId('SelectFile')
		self.file_box.addActionListener(self.guimanager)
		self.add_widget(self.file_box)

	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'ChangeDirectory' and (evt.getSourceWidget().this == self.dir_box.this):
			if(self.dir_list[self.dir_box.getSelected()] == '..'):
				lst = self.path.split('/')
				if(len(lst) > 2):
					lst.pop()
					self.path = '/'.join(lst)	
				else:
					self.path = '/'
			else:
				lst = self.path.split('/')
				lst.append(self.dir_list[self.dir_box.getSelected()])
				self.path = '/'.join(lst)
				
			self.dir_list = GenericListmodel('..')
			self.dir_list.extend([dir for dir in os.listdir(self.path) if not os.path.isfile(os.path.join(self.path,dir))])
			self.dir_box.setListModel(self.dir_list)
			self.dir_box.setSelected(0)

			self.file_list = GenericListmodel()
			self.file_list.extend([file for file in os.listdir(self.path) if os.path.isfile(os.path.join(self.path,file))])
			self.file_box.setListModel(self.file_list)
			self.file_box.setSelected(0)
		elif evtid == 'SelectFile' and (evt.getSourceWidget().this == self.file_box.this):
			print 'Selected file: ' + self.file_list[self.file_box.getSelected()]
		else:
			Form.onWidgetAction(self, evt)

