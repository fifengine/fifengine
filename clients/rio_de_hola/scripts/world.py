# -*- coding: utf-8 -*-
import fife, math, random
import pychan
import pychan.widgets as widgets

from scripts.common.eventlistenerbase import EventListenerBase
from loaders import loadMapFile
from savers import saveMapFile
from agents.hero import Hero
from agents.girl import Girl
from agents.cloud import Cloud
from agents.beekeeper import Beekeeper
from agents.agent import create_anonymous_agents
from settings import Setting

TDS = Setting()

class MapListener(fife.MapChangeListener):
	def __init__(self, map):
		fife.MapChangeListener.__init__(self)
		map.addChangeListener(self)

	def onMapChanged(self, map, changedLayers):
		return
		print "Changes on map ", map.getId()
		for layer in map.getLayers():
			print layer.getId()
			print "    ", ["%s, %x" % (i.getObject().getId(), i.getChangeInfo()) for i in layer.getChangedInstances()]

	def onLayerCreate(self, map, layer):
		pass

	def onLayerDelete(self, map, layer):
		pass


class World(EventListenerBase):
	def __init__(self, engine):
		super(World, self).__init__(engine, regMouse=True, regKeys=True)
		self.engine = engine
		self.eventmanager = engine.getEventManager()
		self.model = engine.getModel()
		self.view = self.engine.getView()
		self.filename = ''
		self.pump_ctr = 0 # for testing purposis
		self.ctrldown = False
		self.instancemenu = None
		self.instance_to_agent = {}
		self.dynamic_widgets = {}

	def show_instancemenu(self, clickpoint, instance):
		if instance.getFifeId() == self.hero.agent.getFifeId():
			return

		dynamicbuttons = ('moveButton', 'talkButton', 'kickButton', 'inspectButton')
		if not self.instancemenu:
			self.instancemenu = pychan.loadXML('gui/instancemenu.xml')
			self.instancemenu.mapEvents({
				'moveButton' : self.onMoveButtonPress,
				'talkButton' : self.onTalkButtonPress,
				'kickButton' : self.onKickButtonPress,
				'inspectButton' : self.onInspectButtonPress,
			})
			for btn in dynamicbuttons:
				self.dynamic_widgets[btn] = self.instancemenu.findChild(name=btn)
		for btn in dynamicbuttons:
			try:
				self.instancemenu.removeChild(self.dynamic_widgets[btn])
			except pychan.exceptions.RuntimeError:
				pass

		self.instancemenu.clickpoint = clickpoint
		self.instancemenu.instance = instance

		self.instancemenu.addChild(self.dynamic_widgets['inspectButton'])
		target_distance = self.hero.agent.getLocationRef().getLayerDistanceTo(instance.getLocationRef())
		if target_distance > 3.0:
			self.instancemenu.addChild(self.dynamic_widgets['moveButton'])
		else:
			if self.instance_to_agent.has_key(instance.getFifeId()):
				self.instancemenu.addChild(self.dynamic_widgets['talkButton'])
				self.instancemenu.addChild(self.dynamic_widgets['kickButton'])
		self.instancemenu.position = (clickpoint.x, clickpoint.y)
		self.instancemenu.show()

	def hide_instancemenu(self):
		if self.instancemenu:
			self.instancemenu.hide()

	def reset(self):
		self.map, self.agentlayer = None, None
		self.cameras = {}
		self.hero, self.girl, self.clouds, self.beekeepers = None, None, [], []
		self.cur_cam2_x, self.initial_cam2_x, self.cam2_scrolling_right = 0, 0, True
		self.target_rotation = 0
		self.instance_to_agent = {}

	def load(self, filename):
		self.filename = filename
		self.reset()
		self.map = loadMapFile(filename, self.engine)
		self.maplistener = MapListener(self.map)

		self.agentlayer = self.map.getLayer('TechdemoMapGroundObjectLayer')
		self.hero = Hero(self.model, 'PC', self.agentlayer)
		self.instance_to_agent[self.hero.agent.getFifeId()] = self.hero
		self.hero.start()

		self.girl = Girl(self.model, 'NPC:girl', self.agentlayer)
		self.instance_to_agent[self.girl.agent.getFifeId()] = self.girl
		self.girl.start()

		self.beekeepers = create_anonymous_agents(self.model, 'beekeeper', self.agentlayer, Beekeeper)
		for beekeeper in self.beekeepers:
			self.instance_to_agent[beekeeper.agent.getFifeId()] = beekeeper
			beekeeper.start()

		cloudlayer = self.map.getLayer('TechdemoMapTileLayer')
		self.clouds = create_anonymous_agents(self.model, 'Cloud', cloudlayer, Cloud)
		for cloud in self.clouds:
			cloud.start(0.1, 0.05)

		for cam in self.view.getCameras():
			self.cameras[cam.getId()] = cam
		self.cameras['main'].attach(self.hero.agent)

		self.view.resetRenderers()
		renderer = fife.FloatingTextRenderer.getInstance(self.cameras['main'])
		textfont = self.engine.getGuiManager().createFont('fonts/rpgfont.png', 0, str(TDS.readSetting("FontGlyphs", strip=False)));
		renderer.changeDefaultFont(textfont)

		renderer = fife.FloatingTextRenderer.getInstance(self.cameras['small'])
		renderer.changeDefaultFont(None)

		renderer = self.cameras['main'].getRenderer('CoordinateRenderer')
		renderer.clearActiveLayers()
		renderer.addActiveLayer(self.map.getLayer(str(TDS.readSetting("CoordinateLayerName"))))

		renderer = self.cameras['main'].getRenderer('QuadTreeRenderer')
		renderer.setEnabled(True)
		renderer.clearActiveLayers()
		if str(TDS.readSetting("QuadTreeLayerName")):
			renderer.addActiveLayer(self.map.getLayer(str(TDS.readSetting("QuadTreeLayerName"))))

		self.cameras['small'].getLocationRef().setExactLayerCoordinates( fife.ExactModelCoordinate( 40.0, 40.0, 0.0 ))
		self.initial_cam2_x = self.cameras['small'].getLocation().getExactLayerCoordinates().x
		self.cur_cam2_x = self.initial_cam2_x
		self.cam2_scrolling_right = True
		# We need to set the second cameras location
		self.cameras['small'].setLocation(self.hero.agent.getLocation())
		self.target_rotation = self.cameras['main'].getRotation()

	def save(self, filename):
		saveMapFile(filename, self.engine, self.map)

	def keyPressed(self, evt):
		keyval = evt.getKey().getValue()
		keystr = evt.getKey().getAsString().lower()
		if keystr == 't':
			r = self.cameras['main'].getRenderer('GridRenderer')
			r.setEnabled(not r.isEnabled())
		elif keystr == 'c':
			r = self.cameras['main'].getRenderer('CoordinateRenderer')
			r.setEnabled(not r.isEnabled())
		elif keystr == 's':
			c = self.cameras['small']
			c.setEnabled(not c.isEnabled())
		elif keystr == 'r':
			pass
#			self.model.deleteMaps()
#			self.metamodel.deleteDatasets()
#			self.view.clearCameras()
#			self.load(self.filename)
		elif keystr == 'o':
			self.target_rotation = (self.target_rotation + 90) % 360
		elif keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self.ctrldown = True

	def keyReleased(self, evt):
		keyval = evt.getKey().getValue()
		if keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
			self.ctrldown = False

	def mouseWheelMovedUp(self, evt):
		if self.ctrldown:
			self.cameras['main'].setZoom(self.cameras['main'].getZoom() * 1.05)

	def mouseWheelMovedDown(self, evt):
		if self.ctrldown:
			self.cameras['main'].setZoom(self.cameras['main'].getZoom() / 1.05)

	def changeRotation(self):
		currot = self.cameras['main'].getRotation()
		if self.target_rotation != currot:
			self.cameras['main'].setRotation((currot + 5) % 360)

	def mousePressed(self, evt):
		if evt.isConsumedByWidgets():
			return

		clickpoint = fife.ScreenPoint(evt.getX(), evt.getY())
		if (evt.getButton() == fife.MouseEvent.LEFT):
			self.hide_instancemenu()
			target_mapcoord = self.cameras['main'].toMapCoordinates(clickpoint, False)
			target_mapcoord.z = 0
			l = fife.Location(self.agentlayer)
			l.setMapCoordinates(target_mapcoord)
			self.hero.run(l)

		if (evt.getButton() == fife.MouseEvent.RIGHT):
			instances = self.cameras['main'].getMatchingInstances(clickpoint, self.agentlayer)
			print "selected instances on agent layer: ", [i.getObject().getId() for i in instances]
			if instances:
				self.show_instancemenu(clickpoint, instances[0])

	def mouseMoved(self, evt):
		renderer = fife.InstanceRenderer.getInstance(self.cameras['main'])
		renderer.removeAllOutlines()

		pt = fife.ScreenPoint(evt.getX(), evt.getY())
		instances = self.cameras['main'].getMatchingInstances(pt, self.agentlayer);
		for i in instances:
			if i.getObject().getId() in ('girl', 'beekeeper'):
				renderer.addOutlined(i, 173, 255, 47, 2)

	def onConsoleCommand(self, command):
		result = ''
		try:
			result = str(eval(command))
		except:
			pass
		return result

	def onMoveButtonPress(self):
		self.hide_instancemenu()
		self.hero.run(self.instancemenu.instance.getLocationRef())

	def onTalkButtonPress(self):
		self.hide_instancemenu()
		instance = self.instancemenu.instance
		self.hero.talk(instance.getLocationRef())
		if instance.getObject().getId() == 'beekeeper':
			beekeeperTexts = TDS.readSetting("beekeeperTexts", type='list', text=True)
			txtindex = random.randint(0, len(beekeeperTexts) - 1)
			instance.say(beekeeperTexts[txtindex], 5000)
		if instance.getObject().getId() == 'girl':
			girlTexts = TDS.readSetting("girlTexts", type='list', text=True)
			txtindex = random.randint(0, len(girlTexts) - 1)
			instance.say(girlTexts[txtindex], 5000)

	def onKickButtonPress(self):
		self.hide_instancemenu()
		self.hero.kick(self.instancemenu.instance.getLocationRef())
		self.instancemenu.instance.say('Hey!', 1000)

	def onInspectButtonPress(self):
		self.hide_instancemenu()
		inst = self.instancemenu.instance
		saytext = ['Engine told me that this instance has']
		if inst.getId():
			saytext.append(' name %s,' % inst.getId())
		saytext.append(' ID %s and' % inst.getFifeId())
		saytext.append(' object name %s' % inst.getObject().getId())
		self.hero.agent.say('\n'.join(saytext), 3500)

	def pump(self):
		if self.cameras['small'].isEnabled():
			loc = self.cameras['small'].getLocation()
			c = loc.getExactLayerCoordinatesRef()
			if self.cam2_scrolling_right:
				self.cur_cam2_x = c.x = c.x+0.1
				if self.cur_cam2_x > self.initial_cam2_x+10:
					self.cam2_scrolling_right = False
			else:
				self.cur_cam2_x = c.x = c.x-0.1
				if self.cur_cam2_x < self.initial_cam2_x-10:
					self.cam2_scrolling_right = True
			self.cameras['small'].setLocation(loc)
		self.changeRotation()
		self.pump_ctr += 1
