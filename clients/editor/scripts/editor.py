from basicapplication import ApplicationBase
import pychan
import fife
import loaders
import events
import plugin
from mapview import MapView
from events import EventListener
from events import *
from gui import ToolBar, action
from gui.action import Action, ActionGroup
from gui.filemanager import FileManager
from gui.mainwindow import MainWindow
from gui.mapeditor import MapEditor
from gui.menubar import Menu, MenuBar
from gui.error import ErrorDialog
from settings import Settings
from pychan.tools import callbackWithArguments as cbwa
import sys, traceback

def getEditor():
	""" Returns the Global editor instance """
	if Editor.editor is None:
		Editor(None)
	return Editor.editor

class Editor(ApplicationBase, MainWindow):
	""" Editor sets up all subsystems and provides access to them """
	editor = None

	def __init__(self, params, *args, **kwargs):
		Editor.editor = self
	
		self._filemanager = None
	
		self.params = params
		self._eventlistener = None
		self._pluginmanager = None
		
		self._inited = False
		
		self._mapview = None
		self._mapviewList = []
		self._mapgroup = None
		self._mapbar = None
		self._maparea = None
		self._mapeditor = None
		
		self._fileMenu = None
		self._editMenu = None
		self._viewMenu = None
		self._toolsMenu = None
		self._helpMenu = None
		
		self._settings = None
		
		self._helpDialog = None
	
		ApplicationBase.__init__(self, *args, **kwargs)
		MainWindow.__init__(self, *args, **kwargs)
		pychan.init(self.engine, debug=False)
		

	def loadSettings(self):
		"""
		Load the settings from a python file and load them into the engine.
		Called in the ApplicationBase constructor.
		"""
		self._settings = Settings()
		TDS = self._settings
		
		glyphDft = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&amp;`'*#=[]\\\""
		engineSetting = self.engine.getSettings()
		engineSetting.setDefaultFontGlyphs(TDS.get("FIFE", "FontGlyphs", glyphDft))
		engineSetting.setDefaultFontPath(TDS.get("FIFE", "Font", "fonts/FreeSans.ttf"))
		engineSetting.setDefaultFontSize(12)
		engineSetting.setBitsPerPixel(TDS.get("FIFE", "BitsPerPixel", 0))
		engineSetting.setInitialVolume(TDS.get("FIFE", "InitialVolume", 5.0))
		engineSetting.setSDLRemoveFakeAlpha(TDS.get("FIFE", "SDLRemoveFakeAlpha", 1))
		engineSetting.setScreenWidth(TDS.get("FIFE", "ScreenWidth", 1024))
		engineSetting.setScreenHeight(TDS.get("FIFE", "ScreenHeight", 768))
		engineSetting.setRenderBackend(TDS.get("FIFE", "RenderBackend", "OpenGL"))
		engineSetting.setFullScreen(TDS.get("FIFE", "FullScreen", 0))

		engineSetting.setWindowTitle(TDS.get("FIFE", "WindowTitle", "No window title set"))
		engineSetting.setWindowIcon(TDS.get("FIFE", "WindowIcon", ""))
		engineSetting.setImageChunkingSize(TDS.get("FIFE", "ImageChunkSize", 256))

	def initLogging(self):
		"""
		Initialize the LogManager.
		"""
		import fifelog
		
		LogModules = self._settings.get("FIFE", "LogModules")
		self.log = fifelog.LogManager(self.engine, self._settings.get("FIFE", "LogToPrompt"), self._settings.get("FIFE", "LogToFile"))
		if LogModules:
			self.log.setVisibleModules(*LogModules)
		
	def _initTools(self):
		""" Initializes tools """
		self._pluginmanager = plugin.PluginManager()
		
		self._filemanager = FileManager()
		self._toolbar.adaptLayout()
		self._mapeditor = MapEditor()
		
	def _initGui(self):
		""" Sets up the GUI """
		screen_width = self.engine.getSettings().getScreenWidth()
		screen_height = self.engine.getSettings().getScreenHeight()
		MainWindow.initGui(self, screen_width, screen_height)

		self._toolbox = ToolBar(title=u"", orientation=1)
		self._toolbox.position_technique = "explicit"
		self._toolbox.position = (150, 150)
		
		self._mapbar = ToolBar(panel_size=20)
		self._mapbar.setDocked(True)
		
		self._maparea = pychan.widgets.VBox()
		self._maparea.opaque = False
		self._maparea.is_focusable = True
		
		# Capture mouse and key events for EventListener
		cw = self._maparea
		cw.capture(self.__sendMouseEvent, "mouseEntered")
		cw.capture(self.__sendMouseEvent, "mouseExited")
		cw.capture(self.__sendMouseEvent, "mousePressed")
		cw.capture(self.__sendMouseEvent, "mouseReleased")
		cw.capture(self.__sendMouseEvent, "mouseClicked")
		cw.capture(self.__sendMouseEvent, "mouseMoved")
		cw.capture(self.__sendMouseEvent, "mouseWheelMovedUp")
		cw.capture(self.__sendMouseEvent, "mouseWheelMovedDown")
		cw.capture(self.__sendMouseEvent, "mouseDragged")
		cw.capture(self.__sendKeyEvent, "keyPressed")
		cw.capture(self.__sendKeyEvent, "keyReleased")
		
		self._centralwidget.addChild(self._mapbar)
		self._centralwidget.addChild(self._maparea)
		
		self._initActions()
		
		self._toolbox.show()

	def _initActions(self):
		""" Initializes toolbar and menubar buttons """
		exitAction = Action(u"Exit", "gui/icons/quit.png")
		exitAction.helptext = u"Exit program"
		action.activated.connect(self.quit, sender=exitAction)
		
		self._fileMenu = Menu(name=u"File")
		self._fileMenu.addAction(exitAction)
		
		self._editMenu = Menu(name=u"Edit")
		self._viewMenu = Menu(name=u"View")
		self._toolsMenu = Menu(name=u"Tools")
		self._windowMenu = Menu(name=u"Window")
		self._helpMenu = Menu(name=u"Help")
		
		self._actionShowStatusbar = Action(u"Statusbar")
		self._actionShowStatusbar.helptext = u"Toggle statusbar"
		action.activated.connect(self.toggleStatusbar, sender=self._actionShowStatusbar)
		
		self._actionShowToolbar = Action(u"Toolbar")
		self._actionShowToolbar.helptext = u"Toggle toolbar"
		action.activated.connect(self.toggleToolbar, sender=self._actionShowToolbar)
		
		self._actionShowToolbox = Action(u"Tool box")
		self._actionShowToolbox.helptext = u"Toggle tool box"
		action.activated.connect(self.toggleToolbox, sender=self._actionShowToolbox)
		
		self._viewMenu.addAction(self._actionShowStatusbar)
		self._viewMenu.addAction(self._actionShowToolbar)
		self._viewMenu.addAction(self._actionShowToolbox)
		self._viewMenu.addSeparator()
	
	
		testAction1 = Action(u"Cycle buttonstyles", "gui/icons/cycle_styles.png")
		testAction1.helptext = u"Cycles button styles. There are currently four button styles."
		action.activated.connect(self._actionActivated, sender=testAction1)
		self._viewMenu.addAction(testAction1)
		
		self._mapgroup = ActionGroup(exclusive=True, name="Mapgroup")
		self._mapbar.addAction(self._mapgroup)
		self._mapbar.addAction(ActionGroup(exclusive=True, name="Mapgroup2"))
		self._windowMenu.addAction(self._mapgroup)
		
		helpAction = Action(u"Help", "gui/icons/help.png")
		helpAction.helptext = u"Displays a window with some simple instructions"
		action.activated.connect(self._showHelpDialog, sender=helpAction)
		self._helpMenu.addAction(helpAction)
		
		self._menubar.addMenu(self._fileMenu)
		self._menubar.addMenu(self._editMenu)
		self._menubar.addMenu(self._viewMenu)
		self._menubar.addMenu(self._toolsMenu)
		self._menubar.addMenu(self._windowMenu)
		self._menubar.addMenu(self._helpMenu)
	
	def _actionActivated(self, sender):
		self._toolbar.button_style += 1
		
	def _showHelpDialog(self, sender):
		""" Shows the help dialog """
		if self._helpDialog is not None:
			self._helpDialog.show()
			return
		
		self._helpDialog = pychan.loadXML("gui/help.xml")
		self._helpDialog.findChild(name="closeButton").capture(self._helpDialog.hide)
		
		f = open('lang/infotext.txt', 'r')
		self._helpDialog.findChild(name="helpText").text = unicode(f.read())
		f.close()
		
		self._helpDialog.show()
		
	def toggleStatusbar(self):
		""" Toggles status bar """
		statusbar = self.getStatusBar()
		if statusbar.max_size[1] > 0:
			statusbar.min_size=(statusbar.min_size[0], 0)
			statusbar.max_size=(statusbar.max_size[0], 0)
			self._actionShowStatusbar.setChecked(False)
		else:
			statusbar.min_size=(statusbar.min_size[0], statusbar.min_size[0])
			statusbar.max_size=(statusbar.max_size[0], statusbar.max_size[0])
			self._actionShowStatusbar.setChecked(True)
		statusbar.adaptLayout()
			
	def toggleToolbar(self):
		""" Toggles toolbar """
		toolbar = self.getToolBar()
		if toolbar.isVisible():
			toolbar.setDocked(False)
			toolbar.hide()
			self._actionShowToolbar.setChecked(False)
		else: 
			tx = toolbar.x
			ty = toolbar.y
			toolbar.show()
			toolbar.x = tx
			toolbar.y = ty
			self._actionShowToolbar.setChecked(True)
			
	def toggleToolbox(self):
		""" Toggles tool box """
		toolbox = self.getToolbox()
		if toolbox.isVisible():
			toolbox.setDocked(False)
			toolbox.hide()
			self._actionShowToolbox.setChecked(False)
		else:
			tx = toolbox.x
			ty = toolbox.y
			toolbox.show()
			toolbox.x = tx
			toolbox.y = ty
			self._actionShowToolbox.setChecked(True)
		toolbox.adaptLayout()
			
	def getToolbox(self): 
		return self._toolbox
	
	def getPluginManager(self): 
		return self._pluginmanager
		
	def getEngine(self): 
		return self.engine

	def getMapViews(self):
		return self._mapviewList
		
	def getActiveMapView(self):
		return self._mapview
		
	def getSettings(self):
		return self._settings;
		
	def showMapView(self, mapview):
		""" Switches to mapview. """
		if mapview is None or mapview == self._mapview:
			return
			
		events.preMapShown.send(sender=self, mapview=mapview)
		self._mapview = mapview
		self._mapview.show()
		events.postMapShown.send(sender=self, mapview=mapview)

	def createListener(self):
		""" Creates the event listener. This is called by ApplicationBase """
		if self._eventlistener is None:
			self._eventlistener = EventListener(self.engine)
		
		return self._eventlistener
		
	def getEventListener(self):
		""" Returns the event listener """
		return self._eventlistener
	
	def newMapView(self, map):
		""" Creates a new map view """
		mapview = MapView(map)
		
		self._mapviewList.append(mapview)
		
		mapAction = Action(unicode(map.getId()))
		action.activated.connect(cbwa(self.showMapView, mapview), sender=mapAction, weak=False)
		self._mapgroup.addAction(mapAction)
		
		self.showMapView(mapview)
		
		events.mapAdded.send(sender=self, map=map)
		
		return mapview
	
	def openFile(self, path):
		""" Opens a file """
		try:
			map = loaders.loadMapFile(path, self.engine)
			return self.newMapView(map)
		except:
			traceback.print_exc(sys.exc_info()[1])
			errormsg = u"Opening map failed:\n"
			errormsg += u"File: "+unicode(path, sys.getfilesystemencoding())+u"\n"
			errormsg += u"Error: "+unicode(sys.exc_info()[1])
			ErrorDialog(errormsg)
			return None
			
	
	def saveAll(self):
		""" Saves all open maps """
		tmpView = self._mapview
		for mapView in self._mapviewList:
			self._mapview = mapView
			self._filemanager.save()
		self._mapview = tmpView
		
	def quit(self):
		""" Quits the editor. An onQuit signal is sent before the application closes """
		events.onQuit.send(sender=self)
		
		self._settings.saveSettings()
		ApplicationBase.quit(self)

	def _pump(self):
		""" Called once per frame """
		# ApplicationBase and Engine should be done initializing at this point
		if self._inited == False:
			self._initGui()
			self._initTools()
			self._inited = True
		
		events.onPump.send(sender=self)
		
	def __sendMouseEvent(self, event, **kwargs):
		""" Function used to capture mouse events for EventListener """
		msEvent = fife.MouseEvent
		type = event.getType()
		
		if type == msEvent.MOVED:
			mouseMoved.send(sender=self._maparea, event=event)
			
		elif type == msEvent.PRESSED:
			mousePressed.send(sender=self._maparea, event=event)
			
		elif type == msEvent.RELEASED:
			mouseReleased.send(sender=self._maparea, event=event)
			
		elif type == msEvent.WHEEL_MOVED_DOWN:
			mouseWheelMovedDown.send(sender=self._maparea, event=event)
			
		elif type == msEvent.WHEEL_MOVED_UP:
			mouseWheelMovedUp.send(sender=self._maparea, event=event)
			
		elif type == msEvent.CLICKED:
			mouseClicked.send(sender=self._maparea, event=event)
			
		elif type == msEvent.ENTERED:
			mouseEntered.send(sender=self._maparea, event=event)
			
		elif type == msEvent.EXITED:
			mouseExited.send(sender=self._maparea, event=event)
			
		elif type == msEvent.DRAGGED:
			mouseDragged.send(sender=self._maparea, event=event)
		
	def __sendKeyEvent(self, event, **kwargs):
		""" Function used to capture key events for EventListener """
		type = event.getType()
		
		if type == fife.KeyEvent.PRESSED:
			keyPressed.send(sender=self._maparea, event=event)
		
		elif type == fife.KeyEvent.RELEASED:
			keyReleased.send(sender=self._maparea, event=event)
			
		

		