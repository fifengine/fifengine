#!/usr/bin/env python

import sys, os, re

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife, fifelog
import settings as TDS
from loaders import loadMapFile
from savers import saveMapFile

from fifedit import *

class InstanceReactor(fife.InstanceListener):
	def OnActionFinished(self, instance, action):
		instance.act_here('idle', instance.getFacingLocation(), True)

SCROLL_MODIFIER = 0.1
class MyEventListener(fife.IKeyListener, fife.ICommandListener, fife.IMouseListener, 
	              fife.ConsoleExecuter, fife.IWidgetListener):
	def __init__(self, world):
		self.world = world
		engine = world.engine
		eventmanager = engine.getEventManager()
		eventmanager.setNonConsumableKeys([
			fife.IKey.ESCAPE,
			fife.IKey.F10,
			fife.IKey.F9,
			fife.IKey.F8,
			fife.IKey.TAB,
			fife.IKey.LEFT,
			fife.IKey.RIGHT,
			fife.IKey.UP,
			fife.IKey.DOWN])
		
		fife.IKeyListener.__init__(self)
		eventmanager.addKeyListener(self)
		fife.ICommandListener.__init__(self)
		eventmanager.addCommandListener(self)
		fife.IMouseListener.__init__(self)
		eventmanager.addMouseListener(self)
		fife.ConsoleExecuter.__init__(self)
		engine.getGuiManager().getConsole().setConsoleExecuter(self)
		fife.IWidgetListener.__init__(self)
		eventmanager.addWidgetListener(self)
	
		self.engine = engine		
		self.quitRequested = False
		self.newTarget = None
		self.showTileOutline = True
		self.showCoordinates = False
		self.scrollwheelvalue = 0
		self.ctrl_scrollwheelvalue = 0
		self.shift_scrollwheelvalue = 0
		self.selectview = False
		self.newMapView = False
				
		# scroll support
		self.horizscroll = 0
		self.vertscroll = 0
		
		# gui
		self.showInfo = False
		self._ctrldown = False
		self._shiftdown = False
		self._dragx = 0
		self._dragy = 0
		

	def mousePressed(self, evt):
		if self._ctrldown:
			self._dragx = evt.getX()
			self._dragy = evt.getY()
			self.horizscroll = 0
			self.vertscroll = 0
		else:
			self.newTarget = fife.ScreenPoint(evt.getX(), evt.getY())

	def mouseReleased(self, evt):
		self._dragx = 0
		self._dragy = 0
		self.horizscroll = 0
		self.vertscroll = 0
	
	def mouseEntered(self, evt):
		pass
	def mouseExited(self, evt):
		pass
	def mouseClicked(self, evt):
		pass
	def mouseWheelMovedUp(self, evt):
		if self._ctrldown:
			self.ctrl_scrollwheelvalue += 1
		elif self._shiftdown:
			self.shift_scrollwheelvalue += 0.01
		else:
			self.scrollwheelvalue += 0.1
			
		
	def mouseWheelMovedDown(self, evt):
		if self._ctrldown:
			self.ctrl_scrollwheelvalue -= 1
		elif self._shiftdown:
			self.shift_scrollwheelvalue -= 0.01
		else:
			self.scrollwheelvalue -= 0.1
	
	def mouseMoved(self, evt):
		pass
	def mouseDragged(self, evt):
		if self._ctrldown:
			self.horizscroll = (self._dragx - evt.getX()) / 100.0
			self.vertscroll = (self._dragy - evt.getY()) / 100.0
			self._dragx = evt.getX()
			self._dragy = evt.getY()

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keyval == fife.IKey.ESCAPE:
			self.quitRequested = True
		elif keyval == fife.IKey.F10:
			self.engine.getGuiManager().getConsole().toggleShowHide()
		elif keyval == fife.IKey.LEFT:
			self.horizscroll -= SCROLL_MODIFIER
		elif keyval == fife.IKey.RIGHT:
			self.horizscroll += SCROLL_MODIFIER
		elif keyval == fife.IKey.UP:
			self.vertscroll -= SCROLL_MODIFIER
		elif keyval == fife.IKey.DOWN:
			self.vertscroll += SCROLL_MODIFIER
		elif keystr == 'p':
			self.engine.getRenderBackend().captureScreen('editor.bmp')
		elif keystr == 't':
			self.showTileOutline = not self.showTileOutline
		elif keystr == 'c':
			self.showCoordinates = not self.showCoordinates
		elif keystr == 'v':
			self.selectview = True
		elif keyval in (fife.IKey.LEFT_CONTROL, fife.IKey.RIGHT_CONTROL):
			self._ctrldown = True
		elif keyval in (fife.IKey.LEFT_SHIFT, fife.IKey.RIGHT_SHIFT):
			self._shiftdown = True
	
	def keyReleased(self, evt):
		keyval = evt.getKey().getValue()
		if keyval in (fife.IKey.LEFT_CONTROL, fife.IKey.RIGHT_CONTROL):
			self._ctrldown = False
		elif keyval in (fife.IKey.LEFT_SHIFT, fife.IKey.RIGHT_SHIFT):
			self._shiftdown = False

	def onCommand(self, command):
		self.quitRequested = (command.getCommandType() == fife.CMD_QUIT_GAME)

	def onToolsClick(self):
		print "No tools set up yet"
	
	def onConsoleCommand(self, command):
		result = "no result"
		if command.lower() in ('quit', 'exit'):
			self.quitRequested = True
			return "quitting"
		
		try:
			result = str(eval(command))
		except:
			pass
		return result
	
	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'WidgetEvtQuit':
			self.quitRequested = True
		elif evtid == 'WidgetEvtAbout':
			if self.showInfo:
				self.showInfo = False
			else:
				self.showInfo = True
		elif evtid == 'SelectMap':
			self.newMapView = True

class Gui(object):
	def __init__(self, engine):
		self.engine = engine
		self.font = engine.getDefaultFont()
		self.guimanager = self.engine.getGuiManager()
		self.widgets = []
		self.infoVisible = False
		self.renderbackend = self.engine.getRenderBackend()
		self.create_panel()
		self.create_infoscreen()

	# to prevent mem problems in current codebase and shorten the code length
	def register_widget(self, w, container):
		self.widgets.append(w)
		container.add(w)
	
	def create_panel(self):
		container = fife.Container()
		container.setOpaque(True)
		self.register_widget(container, self.guimanager)
		
		label1 = fife.Label('FIFE 2008.0 Standalone Editor')
		label1.setPosition(1, 0)
		label1.setFont(self.font)
		self.register_widget(label1, container)
		
		container.setSize(label1.getWidth() + 2, label1.getHeight() + 2)
		container.setPosition(2,2)
		
		container2 = fife.Container()
		container2.setOpaque(True)
		self.register_widget(container2, self.guimanager)

		button1 = fife.Button('Quit')
		button1.setActionEventId('WidgetEvtQuit')
		button1.addActionListener(self.engine.getGuiManager())
		button1.adjustSize()
		button1.setPosition(1, 0)
		button1.setFont(self.font)
		self.register_widget(button1, container2)
		button2 = fife.Button('?')
		button2.setActionEventId('WidgetEvtAbout')
		button2.addActionListener(self.engine.getGuiManager())
		button2.setPosition(button1.getWidth() + 10, 0)
		button2.setFont(self.font)
		self.register_widget(button2, container2)
		container2.setSize(button1.getWidth() + button2.getWidth() + 10, button1.getHeight())
		container2.setPosition(1,28)
		
		container.setVisible(True)
		container2.setVisible(True)
	
	def create_infoscreen(self):
		self.container_info = fife.Container()
		self.container_info.setOpaque(True)
		self.container_info.setSize(self.renderbackend.getScreenWidth() - 2 * 200, self.renderbackend.getScreenHeight() - 2 * 100)
		self.container_info.setPosition(200, 100)
		self.register_widget(self.container_info, self.guimanager)

		label_info = fife.Label('Information box')
		label_info.setPosition(10, 10)
		label_info.setSize(self.renderbackend.getScreenWidth() - 2 * 210, 20)
		label_info.setFont(self.font)
		self.register_widget(label_info, self.container_info)
		
		# text
		text_info = fife.TextBox()
		text_info.setPosition(10,40)
		text_info.setText(open(_jp('content/infotext.txt'), 'r').read())
		text_info.setOpaque(False)
		text_info.setBorderSize(0)
		self.register_widget(text_info, self.container_info)
		self.container_info.setVisible(False)
				
	def show_info(self, show):
		if show != self.infoVisible:
			self.container_info.setVisible(show)
			self.infoVisible = show

class MapViewSelect(Form):
  def __init__(self, event_manager, gui_manager, parent, model):
    self.size = (400,100)
    self.position = (400,100)

    Form.__init__(self, event_manager, gui_manager, parent, 'Select a map:' , self.position, self.size)

    self.model = model

    self.map_list = GenericListmodel()
    self.map_list.extend([map.Id() for map in self.model.getMaps()])
    self.map_drop = fife.DropDown(self.map_list)
    self.map_drop.setSelected(0)
    self.map_drop.setPosition(5, 10)
    self.map_drop.setSize(self.size[0] - 150, 16)
    self.map_drop.setActionEventId('MapListEvt')
    self.map_drop.addActionListener(self.guimanager)
    self.add_widget(self.map_drop)

    button = fife.Button('Select')
    button.setActionEventId('SelectMap')
    button.addActionListener(self.guimanager)
    button.adjustSize()
    button.setPosition(self.size[0] - button.getWidth() - 10, 10)
    self.add_widget(button)

  def getMap(self):
    return self.model.getMaps('id', self.map_list[self.map_drop.getSelected()])[0]


class World(object):
	def __init__(self, engine, gui, editor):
		self.engine = engine
		self.renderbackend = self.engine.getRenderBackend()
		self.reactor = InstanceReactor()

		self.editor = editor

		self.eventmanager = self.engine.getEventManager()
		self.model = self.engine.getModel()
		self.metamodel = self.model.getMetaModel()
		self.gui = gui
		self.view = self.engine.getView()
		
		self.ctrl_scrollwheelvalue = 0
		self.shift_scrollwheelvalue = 0
		self.scrollwheelvalue = 0

		self.mapselect = 0
		
		self.camera = 0

	def _set_camera(self, coordinate, viewport):
		# TODO: this is completely FUBAR. Fix it.
		img = self.engine.getImagePool().getImage(self.layer.getInstances()[0].getObject().get2dGfxVisual().getStaticImageIndexByAngle(0))
		self.screen_cell_w = img.getWidth()
		self.screen_cell_h = img.getHeight()
		self.camera.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)
		self.camera.setRotation(35)
		self.camera.setTilt(60)

		camloc = fife.Location()
		camloc.setLayer(self.layer)
		camloc.setLayerCoordinates(fife.ModelCoordinate(*coordinate))
		self.camera.setViewPort(fife.Rect(*[int(c) for c in viewport]))
		self.camera.setLocation(camloc)		

	def adjust_views(self):
		W = self.renderbackend.getScreenWidth()
		H = self.renderbackend.getScreenHeight()
		maincoords = (1, 1)

		if (not self.camera):
			self.camera = self.view.addCamera()
		self._set_camera(maincoords, (0, 0, W, H))
		self.view.resetRenderers()
		self.ctrl_scrollwheelvalue = self.camera.getRotation()
		self.shift_scrollwheelvalue = self.camera.getZoom()
	
	def run(self):
		self.guiroot = Container(self.engine.getGuiManager())

		camloc = fife.Location()
		evtlistener = MyEventListener(self)
		evtlistener.scrollwheelvalue = self.scrollwheelvalue
		evtlistener.ctrl_scrollwheelvalue = self.ctrl_scrollwheelvalue
		evtlistener.shift_scrollwheelvalue = self.shift_scrollwheelvalue
		self.engine.initializePumping()
	
		showTileOutline = not evtlistener.showTileOutline
		showCoordinates = not evtlistener.showCoordinates
	
		while True:
			if evtlistener.selectview:
				if self.mapselect:
					self.guiroot.remove_widget(self.mapselect)
				self.mapselect = MapViewSelect(self.eventmanager, self.engine.getGuiManager(), self.guiroot, self.model)
				evtlistener.selectview = False

			if evtlistener.newMapView:
				self.map = self.mapselect.getMap()
				self.elevation = self.map.getElevations()[0]
				self.layer = self.elevation.getLayers()[0]
				self.adjust_views()
				self.editor.edit_camview(self.camera)
				evtlistener.newMapView = False

			if showTileOutline != evtlistener.showTileOutline:
				self.view.getRenderer('GridRenderer').setEnabled(evtlistener.showTileOutline)
				showTileOutline = evtlistener.showTileOutline
		
			if showCoordinates != evtlistener.showCoordinates:
				renderer = self.view.getRenderer('CoordinateRenderer')
				showCoordinates = evtlistener.showCoordinates
				renderer.setEnabled(showCoordinates)
##		
##		if self.ctrl_scrollwheelvalue != evtlistener.ctrl_scrollwheelvalue:
##			self.ctrl_scrollwheelvalue = evtlistener.ctrl_scrollwheelvalue
##			self.camera.setRotation(self.ctrl_scrollwheelvalue)
##			print "camera rotation " + str(self.ctrl_scrollwheelvalue)
##	
##		if self.shift_scrollwheelvalue != evtlistener.shift_scrollwheelvalue:
##			self.shift_scrollwheelvalue = evtlistener.shift_scrollwheelvalue
##			self.camera.setZoom(self.shift_scrollwheelvalue)
##			print "camera zoom " + str(self.shift_scrollwheelvalue)
##
##		if self.scrollwheelvalue != evtlistener.scrollwheelvalue:
##			self.scrollwheelvalue = evtlistener.scrollwheelvalue
##			l = self.elevation.getLayers("id", TDS.TestRotationLayerName)[0]
##			l.getCellGrid().setRotation(self.scrollwheelvalue)
##			print "cell grid rotation " + str(self.scrollwheelvalue)
	
			self.engine.pump()
		
			if evtlistener.quitRequested:
				break
		
			# scroll the map with cursor keys
			if (evtlistener.horizscroll or evtlistener.vertscroll):
				loc = self.camera.getLocation()
				cam_scroll = loc.getExactLayerCoordinates()
				cam_scroll.x += evtlistener.horizscroll
				cam_scroll.y += evtlistener.vertscroll
				loc.setExactLayerCoordinates(cam_scroll)
				self.camera.setLocation(loc)
				evtlistener.horizscroll = evtlistener.vertscroll = 0

			self.gui.show_info(evtlistener.showInfo)
		
			self.engine.finalizePumping()


if __name__ == '__main__':
	engine = fife.Engine()
	log = fifelog.LogManager(engine, TDS.LogToPrompt, TDS.LogToFile)
	if TDS.LogModules:
		log.setVisibleModules(*TDS.LogModules)

	s = engine.getSettings()
	s.setDefaultFontGlyphs(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" +
			".,!?-+/:();%`'*#=[]")
	s.setDefaultFontPath('content/fonts/samanata.ttf')
	s.setDefaultFontSize(12)
	s.setBitsPerPixel(TDS.BitsPerPixel)
	s.setFullScreen(TDS.FullScreen)
	s.setInitialVolume(TDS.InitialVolume)
	s.setRenderBackend(TDS.RenderBackend)
	s.setSDLRemoveFakeAlpha(TDS.SDLRemoveFakeAlpha)
	s.setScreenWidth(TDS.ScreenWidth)
	s.setScreenHeight(TDS.ScreenHeight)
	engine.init()
	
	gui = Gui(engine)
	e = FIFEdit(engine, [])
	e.show()

	w = World(engine, gui, e)

	w.run()

