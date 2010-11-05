# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2010 by the FIFE team
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

from fife import fife
import math, random
from fife.extensions import pychan
from fife.extensions.pychan import widgets

from scripts.common.eventlistenerbase import EventListenerBase
from fife.extensions.loaders import loadMapFile
from fife.extensions.savers import saveMapFile
from fife.extensions.soundmanager import SoundManager
from agents.hero import Hero
from agents.girl import Girl
from agents.cloud import Cloud
from agents.beekeeper import Beekeeper
from agents.agent import create_anonymous_agents
from fife.extensions.fife_settings import Setting

TDS = Setting(app_name="rio_de_hola")

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
	"""
	The world!

	This class handles:
	  setup of map view (cameras ...)
	  loading the map
	  GUI for right clicks
	  handles mouse/key events which aren't handled by the GUI.
	   ( by inheriting from EventlistenerBase )

	That's obviously too much, and should get factored out.
	"""
	def __init__(self, engine):
		super(World, self).__init__(engine, regMouse=True, regKeys=True)
		self.engine = engine
		self.eventmanager = engine.getEventManager()
		self.model = engine.getModel()
		self.filename = ''
		self.pump_ctr = 0 # for testing purposis
		self.ctrldown = False
		self.instancemenu = None
		self.instance_to_agent = {}
		self.dynamic_widgets = {}

		self.light_intensity = 1
		self.light_sources = 0
		self.lightmodel = int(TDS.get("FIFE", "Lighting"))

		self.soundmanager = SoundManager(self.engine)
		self.music = None

	def show_instancemenu(self, clickpoint, instance):
		"""
		Build and show a popupmenu for an instance that the player
		clicked on. The available actions are dynamically added to
		the menu (and mapped to the onXYZ functions).
		"""
		if instance.getFifeId() == self.hero.agent.getFifeId():
			return

		# Create the popup.
		self.build_instancemenu()
		self.instancemenu.clickpoint = clickpoint
		self.instancemenu.instance = instance

		# Add the buttons according to circumstances.
		self.instancemenu.addChild(self.dynamic_widgets['inspectButton'])
		target_distance = self.hero.agent.getLocationRef().getLayerDistanceTo(instance.getLocationRef())
		if target_distance > 3.0:
			self.instancemenu.addChild(self.dynamic_widgets['moveButton'])
		else:
			if self.instance_to_agent.has_key(instance.getFifeId()):
				self.instancemenu.addChild(self.dynamic_widgets['talkButton'])
				self.instancemenu.addChild(self.dynamic_widgets['kickButton'])
		# And show it :)
		self.instancemenu.position = (clickpoint.x, clickpoint.y)
		self.instancemenu.show()

	def build_instancemenu(self):
		"""
		Just loads the menu from an XML file
		and hooks the events up.
		The buttons are removed and later re-added if appropiate.
		"""
		self.hide_instancemenu()
		dynamicbuttons = ('moveButton', 'talkButton', 'kickButton', 'inspectButton')
		self.instancemenu = pychan.loadXML('gui/instancemenu.xml')
		self.instancemenu.mapEvents({
			'moveButton' : self.onMoveButtonPress,
			'talkButton' : self.onTalkButtonPress,
			'kickButton' : self.onKickButtonPress,
			'inspectButton' : self.onInspectButtonPress,
		})
		for btn in dynamicbuttons:
			self.dynamic_widgets[btn] = self.instancemenu.findChild(name=btn)
		self.instancemenu.removeAllChildren()

	def hide_instancemenu(self):
		if self.instancemenu:
			self.instancemenu.hide()

	def reset(self):
		"""
		Clear the agent information and reset the moving secondary camera state.
		"""
		if self.music:
			self.music.stop()
			
		self.map, self.agentlayer = None, None
		self.cameras = {}
		self.hero, self.girl, self.clouds, self.beekeepers = None, None, [], []
		self.cur_cam2_x, self.initial_cam2_x, self.cam2_scrolling_right = 0, 0, True
		self.target_rotation = 0
		self.instance_to_agent = {}

	def load(self, filename):
		"""
		Load a xml map and setup agents and cameras.
		"""
		
		self.filename = filename
		self.reset()
		self.map = loadMapFile(filename, self.engine, extensions = {'lights': True})
		self.maplistener = MapListener(self.map)

		self.initAgents()
		self.initCameras()

		if int(TDS.get("FIFE", "PlaySounds")):
			# play track as background music
			self.music = self.soundmanager.createSoundEmitter('music/rio_de_hola.ogg')
			self.music.looping = True
			self.music.gain = 128.0
			self.music.play()
			
			self.waves = self.soundmanager.createSoundEmitter('sounds/waves.ogg')
			self.waves.looping = True
			self.waves.gain = 16.0
			self.waves.play()

	def initAgents(self):
		"""
		Setup agents.

		For this techdemo we have a very simple 'active things on the map' model,
		which is called agents. All rio maps will have a separate layer for them.

		Note that we keep a mapping from map instances (C++ model of stuff on the map)
		to the python agents for later reference.
		"""
		self.agentlayer = self.map.getLayer('TechdemoMapGroundObjectLayer')
		self.hero = Hero(TDS, self.model, 'PC', self.agentlayer)
		self.instance_to_agent[self.hero.agent.getFifeId()] = self.hero
		self.hero.start()

		self.girl = Girl(TDS, self.model, 'NPC:girl', self.agentlayer)
		self.instance_to_agent[self.girl.agent.getFifeId()] = self.girl
		self.girl.start()

		self.beekeepers = create_anonymous_agents(TDS, self.model, 'beekeeper', self.agentlayer, Beekeeper)
		for beekeeper in self.beekeepers:
			self.instance_to_agent[beekeeper.agent.getFifeId()] = beekeeper
			beekeeper.start()

		# Clouds are currently defunct.
		cloudlayer = self.map.getLayer('TechdemoMapTileLayer')
		self.clouds = create_anonymous_agents(TDS, self.model, 'Cloud', cloudlayer, Cloud)
		for cloud in self.clouds:
			cloud.start(0.1, 0.05)


	def initCameras(self):
		"""
		Before we can actually see something on screen we have to specify the render setup.
		This is done through Camera objects which offer a viewport on the map.

		For this techdemo two cameras are used. One follows the hero(!) via 'attach'
		the other one scrolls around a bit (see the pump function).
		"""
		camera_prefix = self.filename.rpartition('.')[0] # Remove file extension
		camera_prefix = camera_prefix.rpartition('/')[2] # Remove path
		camera_prefix += '_'
		
		for cam in self.map.getCameras():
			camera_id = cam.getId().replace(camera_prefix, '')
			self.cameras[camera_id] = cam
			cam.resetRenderers()
			
		self.cameras['main'].attach(self.hero.agent)

		# Floating text renderers currntly only support one font.
		# ... so we set that up.
		# You'll se that for our demo we use a image font, so we have to specify the font glyphs
		# for that one.
		renderer = fife.FloatingTextRenderer.getInstance(self.cameras['main'])
		textfont = self.engine.getGuiManager().createFont('fonts/rpgfont.png', 0, str(TDS.get("FIFE", "FontGlyphs")));
		renderer.changeDefaultFont(textfont)
		renderer.activateAllLayers(self.map)
		renderer.setBackground(100, 255, 100, 165)
		renderer.setBorder(50, 255, 50)
		renderer.setEnabled(True)
		
		# Activate the grid renderer on all layers
		renderer = self.cameras['main'].getRenderer('GridRenderer')
		renderer.activateAllLayers(self.map)
		
		# The small camera shouldn't be cluttered by the 'humm di dums' of our hero.
		# So we disable the renderer simply by setting its font to None.
		renderer = fife.FloatingTextRenderer.getInstance(self.cameras['small'])
		renderer.changeDefaultFont(None)

		# The following renderers are used for debugging.
		# Note that by default ( that is after calling View.resetRenderers or Camera.resetRenderers )
		# renderers will be handed all layers. That's handled here.
		renderer = self.cameras['main'].getRenderer('CoordinateRenderer')
		renderer.clearActiveLayers()
		renderer.addActiveLayer(self.map.getLayer(str(TDS.get("rio", "CoordinateLayerName"))))

		renderer = self.cameras['main'].getRenderer('QuadTreeRenderer')
		renderer.setEnabled(True)
		renderer.clearActiveLayers()
		if str(TDS.get("rio", "QuadTreeLayerName")):
			renderer.addActiveLayer(self.map.getLayer(str(TDS.get("rio", "QuadTreeLayerName"))))

		# If Light is enabled in settings then init the lightrenderer.
		if self.lightmodel != 0:
			renderer = fife.LightRenderer.getInstance(self.cameras['main'])
			renderer.setEnabled(True)
			renderer.clearActiveLayers()
			renderer.addActiveLayer(self.map.getLayer('TechdemoMapGroundObjectLayer'))
			
		# Set up the second camera
		# NOTE: We need to explicitly call setLocation, there's a bit of a messup in the Camera code.
		self.cameras['small'].setLocation(self.hero.agent.getLocation())
		self.cameras['small'].attach(self.girl.agent)
		
		self.target_rotation = self.cameras['main'].getRotation()


	def save(self, filename):
		saveMapFile(filename, self.engine, self.map)

	def getInstancesAt(self, clickpoint):
		"""
		Query the main camera for instances on our active(agent) layer.
		"""
		return self.cameras['main'].getMatchingInstances(clickpoint, self.agentlayer)

	def getLocationAt(self, clickpoint):
		"""
		Query the main camera for the Map location (on the agent layer)
		that a screen point refers to.
		"""
		target_mapcoord = self.cameras['main'].toMapCoordinates(clickpoint, False)
		target_mapcoord.z = 0
		location = fife.Location(self.agentlayer)
		location.setMapCoordinates(target_mapcoord)
		return location

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
			self.model.deleteMaps()
			self.load(self.filename)
		elif keystr == 'o':
			self.target_rotation = (self.target_rotation + 90) % 360
		elif keystr == '2':
			self.lightIntensity(0.1)
		elif keystr == '1':
			self.lightIntensity(-0.1)
		elif keystr == '5':
			self.lightSourceIntensity(25)
		elif keystr == '4':
			self.lightSourceIntensity(-25)
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
		"""
		Smoothly change the main cameras rotation until
		the current target rotation is reached.
		"""
		currot = self.cameras['main'].getRotation()
		if self.target_rotation != currot:
			self.cameras['main'].setRotation((currot + 5) % 360)

	def mousePressed(self, evt):
		if evt.isConsumedByWidgets():
			return

		clickpoint = fife.ScreenPoint(evt.getX(), evt.getY())
		if (evt.getButton() == fife.MouseEvent.LEFT):
			self.hide_instancemenu()
			self.hero.run( self.getLocationAt(clickpoint) )

		if (evt.getButton() == fife.MouseEvent.RIGHT):
			instances = self.getInstancesAt(clickpoint)
			print "selected instances on agent layer: ", [i.getObject().getId() for i in instances]
			if instances:
				self.show_instancemenu(clickpoint, instances[0])

	def mouseMoved(self, evt):
		renderer = fife.InstanceRenderer.getInstance(self.cameras['main'])
		renderer.removeAllOutlines()

		pt = fife.ScreenPoint(evt.getX(), evt.getY())
		instances = self.getInstancesAt(pt);
		for i in instances:
			if i.getObject().getId() in ('girl', 'beekeeper'):
				renderer.addOutlined(i, 173, 255, 47, 2)

	def lightIntensity(self, value):
		if self.light_intensity+value <= 1 and self.light_intensity+value >= 0:
			self.light_intensity = self.light_intensity + value

			if self.lightmodel == 1:
				self.cameras['main'].setLightingColor(self.light_intensity, self.light_intensity, self.light_intensity, 1.0)

			if self.lightmodel == 2:
				self.cameras['main'].setLightingColor(0, 0, 0, 1-self.light_intensity)

	def lightSourceIntensity(self, value):
		if self.light_sources+value <= 255 and self.light_sources+value >= 0:
			self.light_sources = self.light_sources+value
			renderer = fife.LightRenderer.getInstance(self.cameras['main'])

			renderer.removeAll("beekeeper_simple_light")
			renderer.removeAll("hero_simple_light")
			renderer.removeAll("girl_simple_light")

			if self.lightmodel == 1:
				node = fife.LightRendererNode(self.hero.agent)
				renderer.addSimpleLight("hero_simple_light", node, self.light_sources, 64, 32, 1, 1, 255, 255, 255)

				node = fife.LightRendererNode(self.girl.agent)		
				renderer.addSimpleLight("girl_simple_light", node, self.light_sources, 64, 32, 1, 1, 255, 255, 255)

				for beekeeper in self.beekeepers:
					node = fife.LightRendererNode(beekeeper.agent)
					renderer.addSimpleLight("beekeeper_simple_light", node, self.light_sources, 120, 32, 1, 1, 255, 255, 255)

			if self.lightmodel == 2:
				node = fife.LightRendererNode(self.hero.agent)
				renderer.addSimpleLight("hero_simple_light", node, self.light_sources, 64, 32, 1, 1, 0, 0, 0)
				renderer.addStencilTest("hero_simple_light")

				node = fife.LightRendererNode(self.girl.agent)		
				renderer.addSimpleLight("girl_simple_light", node, self.light_sources, 64, 32, 1, 1, 0, 0, 0)
				renderer.addStencilTest("girl_simple_light")

				for beekeeper in self.beekeepers:
					node = fife.LightRendererNode(beekeeper.agent)
					renderer.addSimpleLight("beekeeper_simple_light", node, 255, 120, 32, 1, 1, 0, 0, 0)
					renderer.addStencilTest("beekeeper_simple_light")				

	def onConsoleCommand(self, command):
		result = ''
		try:
			result = str(eval(command))
		except Exception, e:
			result = str(e)
		return result

	# Callbacks from the popupmenu
	def onMoveButtonPress(self):
		self.hide_instancemenu()
		self.hero.run(self.instancemenu.instance.getLocationRef())

	def onTalkButtonPress(self):
		self.hide_instancemenu()
		instance = self.instancemenu.instance
		self.hero.talk(instance.getLocationRef())
		if instance.getObject().getId() == 'beekeeper':
			beekeeperTexts = TDS.get("rio", "beekeeperTexts")
			instance.say(random.choice(beekeeperTexts), 5000)
		if instance.getObject().getId() == 'girl':
			girlTexts = TDS.get("rio", "girlTexts")
			instance.say(random.choice(girlTexts), 5000)

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
		"""
		Called every frame.
		"""

		self.changeRotation()
		self.pump_ctr += 1
