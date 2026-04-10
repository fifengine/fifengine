# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Rio de Hola demo world: map loading, agents, cameras and input handling."""

import random

from fife import fife
from fife.extensions import pychan
from fife.extensions.fife_settings import Setting
from fife.extensions.pychan.internal import get_manager
from fife.extensions.savers import saveMapFile

from scripts.common.eventlistenerbase import EventListenerBase

from .agents.agent import create_anonymous_agents
from .agents.beekeeper import Beekeeper
from .agents.cloud import Cloud
from .agents.girl import Girl
from .agents.hero import Hero

TDS = Setting(app_name="rio_de_hola")


class MapListener(fife.MapChangeListener):
    """Listener for map change events used by the demo."""

    def __init__(self, map):
        fife.MapChangeListener.__init__(self)
        map.addChangeListener(self)

    def onMapChanged(self, map, changedLayers):
        """Handle map-changed events (debug; no-op in normal demo)."""
        return
        print("Changes on map ", map.getId())
        for layer in map.getLayers():
            print(layer.getId())
            print(
                "    ",
                [
                    f"{i.getObject().getId()}, {i.getChangeInfo():x}"
                    for i in layer.getChangedInstances()
                ],
            )

    def onLayerCreate(self, map, layer):
        """Handle layer creation events (no-op)."""
        pass

    def onLayerDelete(self, map, layer):
        """Handle layer deletion events (no-op)."""
        pass


class World(EventListenerBase):
    """The world.

    This class handles:
        setup of map view (cameras ...)
        loading the map
        GUI for right clicks
        handles mouse/key events which aren't handled by the GUI.
         ( by inheriting from EventlistenerBase )

    That's obviously too much, and should get factored out.
    """

    def __init__(self, engine):
        super().__init__(engine, regMouse=True, regKeys=True)
        self.engine = engine
        self.eventmanager = engine.getEventManager()
        self.model = engine.getModel()
        self.filename = ""
        self.pump_ctr = 0  # for testing purposis
        self.ctrldown = False
        self.instancemenu = None
        self.instance_to_agent = {}
        self.dynamic_widgets = {}

        self.light_intensity = 1
        self.light_sources = 0
        self.lightmodel = int(TDS.get("FIFE", "Lighting"))

        self.soundmanager = engine.getSoundManager()
        self.soundmanager.init()
        self.music = None

    def show_instancemenu(self, clickpoint, instance):
        """Build and show a popup menu for the clicked instance."""
        if instance.getFifeId() == self.hero.agent.getFifeId():
            return

        # Create the popup.
        self.build_instancemenu()
        self.instancemenu.clickpoint = clickpoint
        self.instancemenu.instance = instance

        # Add the buttons according to circumstances.
        self.instancemenu.addChild(self.dynamic_widgets["inspectButton"])
        target_distance = self.hero.agent.getLocationRef().getLayerDistanceTo(
            instance.getLocationRef()
        )
        if target_distance > 3.0:
            self.instancemenu.addChild(self.dynamic_widgets["moveButton"])
        else:
            if instance.getFifeId() in self.instance_to_agent:
                self.instancemenu.addChild(self.dynamic_widgets["talkButton"])
                self.instancemenu.addChild(self.dynamic_widgets["kickButton"])
        # And show it :)
        self.instancemenu.position = (clickpoint.x, clickpoint.y)
        self.instancemenu.show()

    def build_instancemenu(self):
        """Load the instance popup menu from XML and hook up events."""
        self.hide_instancemenu()
        dynamicbuttons = ("moveButton", "talkButton", "kickButton", "inspectButton")
        self.instancemenu = pychan.loadXML("gui/instancemenu.xml")
        self.instancemenu.mapEvents(
            {
                "moveButton": self.onMoveButtonPress,
                "talkButton": self.onTalkButtonPress,
                "kickButton": self.onKickButtonPress,
                "inspectButton": self.onInspectButtonPress,
            }
        )
        for btn in dynamicbuttons:
            self.dynamic_widgets[btn] = self.instancemenu.findChild(name=btn)
        self.instancemenu.removeAllChildren()

    def hide_instancemenu(self):
        """Hide the instance popup menu if it is visible."""
        if self.instancemenu:
            self.instancemenu.hide()

    def reset(self):
        """Clear agent information and reset secondary camera state."""
        if self.music:
            self.music.stop()

        self.map, self.agentlayer = None, None
        self.cameras = {}
        self.hero, self.girl, self.clouds, self.beekeepers = None, None, [], []
        self.cur_cam2_x, self.initial_cam2_x, self.cam2_scrolling_right = 0, 0, True
        self.target_rotation = 0
        self.instance_to_agent = {}

    def load(self, filename):
        """Load an XML map and set up agents and cameras."""
        self.filename = filename
        self.reset()
        loader = fife.MapLoader(
            self.engine.getModel(),
            self.engine.getVFS(),
            self.engine.getImageManager(),
            self.engine.getRenderBackend(),
        )

        if loader.isLoadable(filename):
            self.map = loader.load(filename)

        self.initAgents()
        self.initCameras()

        # Set background color
        self.engine.getRenderBackend().setBackgroundColor(80, 80, 255)

        if int(TDS.get("FIFE", "PlaySounds")):
            # play track as background music
            self.music = self.soundmanager.createEmitter("music/rio_de_hola.ogg")
            self.music.setLooping(True)
            self.music.setGain(0.5)
            self.music.play()

            self.waves = self.soundmanager.createEmitter("sounds/waves.ogg")
            self.waves.setLooping(True)
            self.waves.setGain(0.1)
            self.waves.play()

    def initAgents(self):
        """Create and start agents for the map and populate the instance mapping."""
        self.agentlayer = self.map.getLayer("TechdemoMapGroundObjectLayer")
        self.hero = Hero(TDS, self.model, "PC", self.agentlayer)
        self.instance_to_agent[self.hero.agent.getFifeId()] = self.hero
        self.hero.start()

        self.girl = Girl(TDS, self.model, "NPC:girl", self.agentlayer)
        self.instance_to_agent[self.girl.agent.getFifeId()] = self.girl
        self.girl.start()

        self.beekeepers = create_anonymous_agents(
            TDS, self.model, "beekeeper", self.agentlayer, Beekeeper
        )
        for beekeeper in self.beekeepers:
            self.instance_to_agent[beekeeper.agent.getFifeId()] = beekeeper
            beekeeper.start()

        # Clouds are currently defunct.
        cloudlayer = self.map.getLayer("TechdemoMapTileLayer")
        self.clouds = create_anonymous_agents(TDS, self.model, "Cloud", cloudlayer, Cloud)
        for cloud in self.clouds:
            cloud.start(0.1, 0.05)

    def initCameras(self):
        """Initialize cameras, renderers, and floating text for the map view."""
        camera_prefix = self.filename.rpartition(".")[0]  # Remove file extension
        camera_prefix = camera_prefix.rpartition("/")[2]  # Remove path
        camera_prefix += "_"

        for cam in self.map.getCameras():
            camera_id = cam.getId().replace(camera_prefix, "")
            self.cameras[camera_id] = cam
            cam.resetRenderers()

        self.cameras["main"].attach(self.hero.agent)

        # Floating text renderers currntly only support one font.
        # ... so we set that up.
        # You'll se that for our demo we use a image font, so we have to specify the font glyphs
        # for that one.
        renderer = fife.FloatingTextRenderer.getInstance(self.cameras["main"])
        textfont = get_manager().createFont(
            "fonts/rpgfont.png", 0, str(TDS.get("FIFE", "FontGlyphs"))
        )
        renderer.setFont(textfont)
        renderer.activateAllLayers(self.map)
        renderer.setBackground(100, 255, 100, 165)
        renderer.setBorder(50, 255, 50)
        renderer.setEnabled(True)

        # Activate the grid renderer on all layers
        renderer = self.cameras["main"].getRenderer("GridRenderer")
        renderer.activateAllLayers(self.map)

        # The small camera shouldn't be cluttered by the 'humm di dums' of our hero.
        # So we disable the renderer simply by setting its font to None.
        renderer = fife.FloatingTextRenderer.getInstance(self.cameras["small"])
        renderer.setFont(None)

        # The following renderers are used for debugging.
        # Note that by default ( that is after calling View.resetRenderers or Camera.resetRenderers )
        # renderers will be handed all layers. That's handled here.
        renderer = fife.CoordinateRenderer.getInstance(self.cameras["main"])
        renderer.setFont(textfont)
        renderer.clearActiveLayers()
        renderer.addActiveLayer(
            self.map.getLayer(str(TDS.get("rio", "CoordinateLayerName")))
        )

        renderer = self.cameras["main"].getRenderer("QuadTreeRenderer")
        renderer.setEnabled(True)
        renderer.clearActiveLayers()
        if str(TDS.get("rio", "QuadTreeLayerName")):
            renderer.addActiveLayer(
                self.map.getLayer(str(TDS.get("rio", "QuadTreeLayerName")))
            )

        # If Light is enabled in settings then init the lightrenderer.
        if self.lightmodel != 0:
            renderer = fife.LightRenderer.getInstance(self.cameras["main"])
            renderer.setEnabled(True)
            renderer.clearActiveLayers()
            renderer.addActiveLayer(self.map.getLayer("TechdemoMapGroundObjectLayer"))

        # Set up the second camera
        # NOTE: We need to explicitly call setLocation, there's a bit of a messup in the Camera code.
        self.cameras["small"].setLocation(self.hero.agent.getLocation())
        self.cameras["small"].attach(self.girl.agent)

        self.target_rotation = self.cameras["main"].getRotation()

    def save(self, filename):
        """Save the current map to the given filename."""
        saveMapFile(filename, self.engine, self.map)

    def getInstancesAt(self, clickpoint):
        """
        Query the main camera for instances on our active(agent) layer.

        Returns
        -------
        list
            A list of matching instances.
        """
        screen_point = fife.Point3D(int(clickpoint.x), int(clickpoint.y), 0)
        return self.cameras["main"].getMatchingInstances(screen_point, self.agentlayer)

    def getLocationAt(self, clickpoint):
        """Return the map Location corresponding to the given screen point.

        Returns
        -------
        fife.Location
            A Location object corresponding to the map coordinates.
        """
        screen_point = fife.Point3D(int(clickpoint.x), int(clickpoint.y), 0)
        target_mapcoord = self.cameras["main"].toMapCoordinates(screen_point, False)
        target_mapcoord.z = 0
        location = fife.Location(self.agentlayer)
        location.setMapCoordinates(target_mapcoord)
        return location

    def keyPressed(self, evt):
        """Handle key pressed events for toggles and shortcuts."""
        keyval = evt.getKey().getValue()
        keystr = evt.getKey().getAsString().lower()
        if keystr == "t":
            r = self.cameras["main"].getRenderer("GridRenderer")
            r.setEnabled(not r.isEnabled())
        elif keystr == "c":
            r = self.cameras["main"].getRenderer("CoordinateRenderer")
            r.setEnabled(not r.isEnabled())
        elif keystr == "s":
            c = self.cameras["small"]
            c.setEnabled(not c.isEnabled())
        elif keystr == "r":
            self.model.deleteMaps()
            self.load(self.filename)
        elif keystr == "o":
            self.target_rotation = (self.target_rotation + 90) % 360
        elif keystr == "2":
            self.lightIntensity(0.1)
        elif keystr == "1":
            self.lightIntensity(-0.1)
        elif keystr == "4":
            self.lightSourceIntensity(25)
        elif keystr == "3":
            self.lightSourceIntensity(-25)
        elif keystr == "0" or keystr == fife.Key.NUM_0:
            if self.ctrldown:
                self.cameras["main"].setZoom(1.0)
        elif keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
            self.ctrldown = True

    def keyReleased(self, evt):
        """Handle key released events and update modifier state."""
        keyval = evt.getKey().getValue()
        if keyval in (fife.Key.LEFT_CONTROL, fife.Key.RIGHT_CONTROL):
            self.ctrldown = False

    def mouseWheelMovedUp(self, evt):
        """Zoom in when control is held and the wheel is moved up."""
        if self.ctrldown:
            self.cameras["main"].setZoom(self.cameras["main"].getZoom() * 1.05)

    def mouseWheelMovedDown(self, evt):
        """Zoom out when control is held and the wheel is moved down."""
        if self.ctrldown:
            self.cameras["main"].setZoom(self.cameras["main"].getZoom() / 1.05)

    def changeRotation(self):
        """Smoothly adjust main camera rotation towards the target rotation."""
        currot = self.cameras["main"].getRotation()
        if self.target_rotation != currot:
            self.cameras["main"].setRotation((currot + 5) % 360)

    def mousePressed(self, evt):
        """Handle mouse press: left-click moves, right-click opens instance menu."""
        if evt.isConsumedByWidgets():
            return

        clickpoint = fife.Point3D(evt.getX(), evt.getY())
        if evt.getButton() == fife.MouseEvent.LEFT:
            self.hide_instancemenu()
            self.hero.run(self.getLocationAt(clickpoint))

        if evt.getButton() == fife.MouseEvent.RIGHT:
            instances = self.getInstancesAt(clickpoint)
            print(
                "selected instances on agent layer: ",
                [i.getObject().getId() for i in instances],
            )
            if instances:
                self.show_instancemenu(clickpoint, instances[0])

    def mouseMoved(self, evt):
        """Handle mouse moved events and update outlines for nearby agents."""
        renderer = fife.InstanceRenderer.getInstance(self.cameras["main"])
        renderer.removeAllOutlines()

        pt = fife.Point3D(evt.getX(), evt.getY())
        instances = self.getInstancesAt(pt)
        for i in instances:
            if i.getObject().getId() in ("girl", "beekeeper"):
                renderer.addOutlined(i, 173, 255, 47, 2)

    def lightIntensity(self, value):
        """Adjust global light intensity within valid bounds and update renderer."""
        if self.light_intensity + value <= 1 and self.light_intensity + value >= 0:
            self.light_intensity = self.light_intensity + value

            if self.lightmodel == 1:
                self.cameras["main"].setLightingColor(
                    self.light_intensity, self.light_intensity, self.light_intensity
                )

    def lightSourceIntensity(self, value):
        """Adjust per-source light intensity and update simple lights."""
        if self.light_sources + value <= 255 and self.light_sources + value >= 0:
            self.light_sources = self.light_sources + value
            renderer = fife.LightRenderer.getInstance(self.cameras["main"])

            renderer.removeAll("beekeeper_simple_light")
            renderer.removeAll("hero_simple_light")
            renderer.removeAll("girl_simple_light")

            if self.lightmodel == 1:
                node = fife.RendererNode(self.hero.agent)
                renderer.addSimpleLight(
                    "hero_simple_light",
                    node,
                    self.light_sources,
                    64,
                    32,
                    1,
                    1,
                    255,
                    255,
                    255,
                )

                node = fife.RendererNode(self.girl.agent)
                renderer.addSimpleLight(
                    "girl_simple_light",
                    node,
                    self.light_sources,
                    64,
                    32,
                    1,
                    1,
                    255,
                    255,
                    255,
                )

                for beekeeper in self.beekeepers:
                    node = fife.RendererNode(beekeeper.agent)
                    renderer.addSimpleLight(
                        "beekeeper_simple_light",
                        node,
                        self.light_sources,
                        120,
                        32,
                        1,
                        1,
                        255,
                        255,
                        255,
                    )

    def onConsoleCommand(self, command):
        """Execute a console command string and return the result or error.

        Returns
        -------
        str
            Result string or error message produced by executing `command`.
        """
        result = ""
        try:
            result = str(eval(command))
        except Exception as e:
            result = str(e)
        return result

    # Callbacks from the popupmenu
    def onMoveButtonPress(self):
        """Handle the Move button press from the instance popup menu."""
        self.hide_instancemenu()
        self.hero.run(self.instancemenu.instance.getLocationRef())

    def onTalkButtonPress(self):
        """Handle the Talk button press and play NPC dialog if applicable."""
        self.hide_instancemenu()
        instance = self.instancemenu.instance
        self.hero.talk(instance.getLocationRef())
        if instance.getObject().getId() == "beekeeper":
            beekeeperTexts = TDS.get("rio", "beekeeperTexts")
            instance.say(random.choice(beekeeperTexts), 5000)
        if instance.getObject().getId() == "girl":
            girlTexts = TDS.get("rio", "girlTexts")
            instance.say(random.choice(girlTexts), 5000)

    def onKickButtonPress(self):
        """Handle the Kick button press from the instance popup menu."""
        self.hide_instancemenu()
        self.hero.kick(self.instancemenu.instance.getLocationRef())
        self.instancemenu.instance.say("Hey!", 1000)

    def onInspectButtonPress(self):
        """Handle the Inspect button press and show instance metadata."""
        self.hide_instancemenu()
        inst = self.instancemenu.instance
        saytext = ["Engine told me that this instance has"]
        if inst.getId():
            saytext.append(f" name {inst.getId()},")
        saytext.append(f" ID {inst.getFifeId()} and")
        saytext.append(f" object name {inst.getObject().getId()}")
        self.hero.agent.say("\n".join(saytext), 3500)

    def pump(self):
        """Run per-frame updates (rotation and pump counter)."""
        self.changeRotation()
        self.pump_ctr += 1
