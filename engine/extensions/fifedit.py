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

		elif evtid == 'SaveMapEvt':
			if(self.map):
				mapfilename = self.path_box.getText()
				print 'saving: ' + mapfilename
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
		self.savewnd.setPosition(100,100)
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

