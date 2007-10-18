import fife
from loaders import loadMapFile
from savers import saveMapFile

from pychan import *

class FIFEdit(fife.IWidgetListener, object):

	def __init__(self, engine):
		self.engine = engine
		self.eventmanager = engine.getEventManager()
		self.map = 0

		fife.IWidgetListener.__init__(self)
		self.eventmanager.addWidgetListener(self)

		self.guimanager = engine.getGuiManager()
		glyphs = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" + \
		         ".,!?-+/:();%`'*#=[]"
		self.font = self.guimanager.createFont('techdemo/fonts/samanata.ttf', 12, glyphs)
		self.font.setColor(0, 0, 0)
		self.guimanager.setGlobalFont(self.font)
		self.widgets = []

		self.screenheight = engine.getRenderBackend().getScreenHeight()
		self.screenwidth = engine.getRenderBackend().getScreenWidth()

		self.map_list = GenericListmodel()
		self.map_list.extend(['content/maps/new_official_map.xml', 'techdemo/maps/city1.xml']) # TODO: this shouldn't be hardcoded

		self.create_mainpanel()
		self.create_mapdialogs()

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
		else:
			print "Uncaught gui event."

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

		self.path_box = fife.TextBox('Replace this with the path/filename')
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

		self.mapid_box = fife.TextBox(self.map.Id())
		self.mapid_box.setPosition(1 + mapid_label.getWidth() + 1, 1)
		self.register_widget(self.mapid_box, self.mapedit)

		dy = 1 + self.mapid_box.getHeight() + 1

		list = self.map.listFields()
		self.mapmetaboxes = []
		for i in range(0, len(list)):
			label = fife.Label(list[i])
			label.setPosition(1, dy)
			self.register_widget(label, self.mapedit)

			textbox = 0

			type = self.map.getTypeName(list[i])
			if (type == 'id'):
				textbox = fife.TextBox(str(self.map.get_int(list[i])))

			elif (type == 'text'):
				textbox = fife.TextBox(self.map.get_string(list[i]))

			else:
				textbox = fife.TextBox("Error: unsupported type.")

			textbox.setPosition(1 + label.getWidth() + 1, dy)
			self.register_widget(textbox, self.mapedit)
			self.mapmetaboxes.append((list[i], type, textbox))

			dy += textbox.getHeight() + 1

	def save_mapmetadata(self):
		self.map.set_string('id', self.mapid_box.getText())
		
		for i in range(0, len(self.mapmetaboxes)):
			if (self.mapmetaboxes[i][1] == 'id'):
				self.map.set_int(self.mapmetaboxes[i][0], int(self.mapmetaboxes[i][2].getText()))
			elif (self.mapmetaboxes[i][1] == 'text'):
				self.map.set_string(self.mapmetaboxes[i][0], self.mapmetaboxes[i][2].getText())

