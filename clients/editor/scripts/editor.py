# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

"""
Editor
======

This class serves as 
"""

import sys
import traceback

import fife
import loaders
import events
import plugin

from basicapplication import ApplicationBase

import pychan
from pychan.tools import callbackWithArguments as cbwa

from events import *
from gui import ToolBar, action
from gui.action import Action, ActionGroup
from gui.filemanager import FileManager
from gui.mainwindow import MainWindow
from gui.mapeditor import MapEditor
from gui.menubar import Menu, MenuBar
from gui.error import ErrorDialog
from gui.yesnodialog import YesNoDialog
from mapview import MapView
from settings import Settings

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
		self._open_files = []
	
		self._params = params
		self._eventlistener = None
		self._pluginmanager = None
		
		self._inited = False
		
		self._mapview = None
		self._mapviewlist = []
		self._mapgroup = None
		self._mapbar = None
		self._maparea = None
		self._mapeditor = None
		
		self._file_menu = None
		self._edit_menu = None
		self._view_menu = None
		self._tools_menu = None
		self._help_menu = None
		
		self._settings = None
		
		self._help_dialog = None
	
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
		
		logModules = self._settings.get("FIFE", "LogModules")
		loglevel = self._settings.get("FIFE", "LogLevel", "Warn");
			
		self.log = fifelog.LogManager(self.engine, self._settings.get("FIFE", "LogToPrompt"), self._settings.get("FIFE", "LogToFile"))
		if logModules:
			self.log.setVisibleModules(*logModules)
			
		if loglevel == "Debug":
			loglevel = fife.LogManager.LEVEL_DEBUG
		elif loglevel == "Log":
			loglevel = fife.LogManager.LEVEL_LOG
		elif loglevel == "Error":
			loglevel = fife.LogManager.LEVEL_ERROR
		else:
			loglevel = fife.LogManager.LEVEL_WARN
		self.log.lm.setLevelFilter(loglevel)
		
	def _initTools(self):
		""" Initializes tools """
		self._pluginmanager = plugin.PluginManager(self.getSettings())
		
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
		
		self._mapbar = ToolBar(name="MapBar", panel_size=20)
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
		
		self._file_menu = Menu(name=u"File")
		self._file_menu.addAction(exitAction)
		
		self._edit_menu = Menu(name=u"Edit")
		self._view_menu = Menu(name=u"View")
		self._tools_menu = Menu(name=u"Tools")
		self._window_menu = Menu(name=u"Window")
		self._help_menu = Menu(name=u"Help")
		
		self._action_show_statusbar = Action(u"Statusbar")
		self._action_show_statusbar.helptext = u"Toggle statusbar"
		action.activated.connect(self.toggleStatusbar, sender=self._action_show_statusbar)
		
		self._action_show_toolbar = Action(u"Toolbar")
		self._action_show_toolbar.helptext = u"Toggle toolbar"
		action.activated.connect(self.toggleToolbar, sender=self._action_show_toolbar)
		
		self._action_show_toolbox = Action(u"Tool box")
		self._action_show_toolbox.helptext = u"Toggle tool box"
		action.activated.connect(self.toggleToolbox, sender=self._action_show_toolbox)
		
		self._view_menu.addAction(self._action_show_statusbar)
		self._view_menu.addAction(self._action_show_toolbar)
		self._view_menu.addAction(self._action_show_toolbox)
		self._view_menu.addSeparator()
	
	
		test_action1 = Action(u"Cycle buttonstyles", "gui/icons/cycle_styles.png")
		test_action1.helptext = u"Cycles button styles. There are currently four button styles."
		action.activated.connect(self._actionActivated, sender=test_action1)
		self._view_menu.addAction(test_action1)
		
		self._mapgroup = ActionGroup(exclusive=True, name="Mapgroup")
		self._mapbar.addAction(self._mapgroup)
		self._window_menu.addAction(self._mapgroup)
		
		help_action = Action(u"Help", "gui/icons/help.png")
		help_action.helptext = u"Displays a window with some simple instructions"
		action.activated.connect(self._showHelpDialog, sender=help_action)
		self._help_menu.addAction(help_action)
		
		self._menubar.addMenu(self._file_menu)
		self._menubar.addMenu(self._edit_menu)
		self._menubar.addMenu(self._view_menu)
		self._menubar.addMenu(self._tools_menu)
		self._menubar.addMenu(self._window_menu)
		self._menubar.addMenu(self._help_menu)
	
	def _actionActivated(self, sender):
		self._toolbar.button_style += 1
		
	def _showHelpDialog(self, sender):
		""" Shows the help dialog """
		if self._help_dialog is not None:
			self._help_dialog.show()
			return
		
		self._help_dialog = pychan.loadXML("gui/help.xml")
		self._help_dialog.findChild(name="closeButton").capture(self._help_dialog.hide)
		
		f = open('lang/infotext.txt', 'r')
		self._help_dialog.findChild(name="helpText").text = unicode(f.read())
		f.close()
		
		self._help_dialog.show()
		
	def toggleStatusbar(self):
		""" Toggles status bar """
		statusbar = self.getStatusBar()
		if statusbar.max_size[1] > 0:
			statusbar.min_size=(statusbar.min_size[0], 0)
			statusbar.max_size=(statusbar.max_size[0], 0)
			self._action_show_statusbar.setChecked(False)
		else:
			statusbar.min_size=(statusbar.min_size[0], statusbar.min_size[0])
			statusbar.max_size=(statusbar.max_size[0], statusbar.max_size[0])
			self._action_show_statusbar.setChecked(True)
		statusbar.adaptLayout()
			
	def toggleToolbar(self):
		""" Toggles toolbar """
		toolbar = self.getToolBar()
		if toolbar.isVisible():
			toolbar.setDocked(False)
			toolbar.hide()
			self._action_show_toolbar.setChecked(False)
		else: 
			tx = toolbar.x
			ty = toolbar.y
			toolbar.show()
			toolbar.x = tx
			toolbar.y = ty
			self._action_show_toolbar.setChecked(True)
			
	def toggleToolbox(self):
		""" Toggles tool box """
		toolbox = self.getToolbox()
		if toolbox.isVisible():
			toolbox.setDocked(False)
			toolbox.hide()
			self._action_show_toolbox.setChecked(False)
		else:
			tx = toolbox.x
			ty = toolbox.y
			toolbox.show()
			toolbox.x = tx
			toolbox.y = ty
			self._action_show_toolbox.setChecked(True)
		toolbox.adaptLayout()
			
	def getToolbox(self): 
		return self._toolbox
	
	def getPluginManager(self): 
		return self._pluginmanager
		
	def getEngine(self): 
		return self.engine

	def getMapViews(self):
		return self._mapviewlist
		
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
		
		self._mapviewlist.append(mapview)
		
		map_action = Action(unicode(map.getId()))
		action.activated.connect(cbwa(self.showMapView, mapview), sender=map_action, weak=False)
		self._mapgroup.addAction(map_action)
		
		self.showMapView(mapview)
		
		events.preMapClosed.connect(self._preMapRemoved)
		events.postMapClosed.connect(self._postMapRemoved)
		events.mapAdded.send(sender=self, map=map)
		
		return mapview
		
	def _preMapRemoved(self, sender, mapview):
		events.preMapClosed.disconnect(self._preMapRemoved)
	
		# Remove from open files list
		try:
			path = mapview.getMap().getResourceLocation().getFilename()
			try:
				self._open_files.remove(path)
			except ValueError:
				pass
		except RuntimeError:
			# Mapview is not saved
			return
		
		# Remove tab
		for map_action in self._mapgroup.getActions():
			if map_action.text == unicode(mapview.getMap().getId()):
				self._mapgroup.removeAction(map_action)
				break
			
	def _postMapRemoved(self, mapview):
		events.postMapClosed.disconnect(self._postMapRemoved)
		
		# Remove from mapviewlist
		index = self._mapviewlist.index(mapview)-1
		self._mapviewlist.remove(mapview)
		
		# Change mapview
		if index >= 0:
			self.showMapView(self._mapviewlist[index])
		else:
			self._mapview = None
			self.getEngine().getView().clearCameras()
			
	def getMapviewByPath(self, path):
		mappath = ""
		for mv in self._mapviewlist:
			try:
				mappath = mv.getMap().getResourceLocation().getFilename()
			except RuntimeError:
				# Mapview is not saved yet
				continue
				
			if mappath == path:
				return mv

	def openFile(self, path):
		if path in self._open_files:
			# Map is already open, ask user if he wants to reload the map
			mapview = self.getMapviewByPath(path)
			YesNoDialog(u"Map is already open. Do you want to reload it?", cbwa(self.reloadMapview, mapview=mapview))
			return
	
		""" Opens a file """
		try:
			map = loaders.loadMapFile(path, self.engine)
			mapview = self.newMapView(map)
			self._open_files.append(path)
			return mapview
		except:
			traceback.print_exc(sys.exc_info()[1])
			errormsg = u"Opening map failed:\n"
			errormsg += u"File: "+unicode(path, sys.getfilesystemencoding())+u"\n"
			errormsg += u"Error: "+unicode(sys.exc_info()[1])
			ErrorDialog(errormsg)
			return None
			
	def reloadMapview(self, mapview=None):
		if mapview is None:
			mapview = self._mapview
			
		if mapview is None:
			print "Can't reload map: No maps are open."
			return
			
		path = mapview.getMap().getResourceLocation().getFilename()
		mapview.close()
		self.openFile(path)
			
	
	def saveAll(self):
		""" Saves all open maps """
		tmpview = self._mapview
		for mapview in self._mapviewlist:
			self._mapview = mapview
			self._filemanager.save()
		self._mapview = tmpview
		
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
			if self._params: self.openFile(self._params)
			self._inited = True
		
		events.onPump.send(sender=self)
		
	def __sendMouseEvent(self, event, **kwargs):
		""" Function used to capture mouse events for EventListener """
		ms_event = fife.MouseEvent
		type = event.getType()
		
		if type == ms_event.MOVED:
			mouseMoved.send(sender=self._maparea, event=event)
			
		elif type == ms_event.PRESSED:
			mousePressed.send(sender=self._maparea, event=event)
			
		elif type == ms_event.RELEASED:
			mouseReleased.send(sender=self._maparea, event=event)
			
		elif type == ms_event.WHEEL_MOVED_DOWN:
			mouseWheelMovedDown.send(sender=self._maparea, event=event)
			
		elif type == ms_event.WHEEL_MOVED_UP:
			mouseWheelMovedUp.send(sender=self._maparea, event=event)
			
		elif type == ms_event.CLICKED:
			mouseClicked.send(sender=self._maparea, event=event)
			
		elif type == ms_event.ENTERED:
			mouseEntered.send(sender=self._maparea, event=event)
			
		elif type == ms_event.EXITED:
			mouseExited.send(sender=self._maparea, event=event)
			
		elif type == ms_event.DRAGGED:
			mouseDragged.send(sender=self._maparea, event=event)
		
	def __sendKeyEvent(self, event, **kwargs):
		""" Function used to capture key events for EventListener """
		type = event.getType()
		
		if type == fife.KeyEvent.PRESSED:
			keyPressed.send(sender=self._maparea, event=event)
		
		elif type == fife.KeyEvent.RELEASED:
			keyReleased.send(sender=self._maparea, event=event)
			
		

		
