#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import scripts.test as test
from fife import fife
from fife.extensions import pychan
from fife.extensions.pychan.exceptions import InitializationError


class KeyListener(fife.IKeyListener):
    def __init__(self, test):
        self._engine = test._engine
        self._test = test
        self._eventmanager = self._engine.getEventManager()

        fife.IKeyListener.__init__(self)

    def keyPressed(self, evt):
        keystr = evt.getKey().getAsString().lower()
        if keystr == "t":
            r = self._test._camera.getRenderer("GridRenderer")
            r.setEnabled(not r.isEnabled())
        elif keystr == "c":
            r = self._test._camera.getRenderer("CoordinateRenderer")
            r.setEnabled(not r.isEnabled())

    def keyReleased(self, evt):
        pass


class MouseListener(fife.IMouseListener):
    def __init__(self, test):
        self._engine = test._engine
        self._test = test
        self._eventmanager = self._engine.getEventManager()

        fife.IMouseListener.__init__(self)

    def mousePressed(self, event):
        if event.isConsumedByWidgets():
            return

        clickpoint = fife.ScreenPoint(event.getX(), event.getY())

        self._test.movePlayer(clickpoint)

    def mouseReleased(self, event):
        pass

    def mouseMoved(self, event):
        pass

    def mouseEntered(self, event):
        pass

    def mouseExited(self, event):
        pass

    def mouseClicked(self, event):
        pass

    def mouseWheelMovedUp(self, event):
        pass

    def mouseWheelMovedDown(self, event):
        pass

    def mouseDragged(self, event):
        pass


class PathfinderTest(test.Test):
    def create(self, engine, application):
        self._application = application
        self._engine = engine
        self._running = False

        self._loader = fife.MapLoader(
            self._engine.getModel(),
            self._engine.getVFS(),
            self._engine.getImageManager(),
            self._engine.getRenderBackend(),
        )

        self._eventmanager = self._engine.getEventManager()

    def destroy(self):
        # any left over cleanup here
        pass

    def run(self):
        self._running = True

        self._mouselistener = MouseListener(self)
        self._eventmanager.addMouseListener(self._mouselistener)

        self._keylistener = KeyListener(self)
        self._eventmanager.addKeyListener(self._keylistener)

        self._font = pychan.internal.get_manager().createFont("data/fonts/rpgfont.png")
        if self._font is None:
            raise InitializationError("Could not load font rpgfont.png")

        self.loadMap("data/maps/pathfinder_grassland.xml")

    def stop(self):
        self._running = False

        self._engine.getModel().deleteMap(self._map)
        self._engine.getModel().deleteObjects()

        self._eventmanager.removeMouseListener(self._mouselistener)
        self._eventmanager.removeKeyListener(self._keylistener)

        del self._mouselistener
        del self._keylistener

    def isRunning(self):
        return self._running

    def getName(self):
        return "PathfinderTest"

    def getAuthor(self):
        return "helios"

    def getDescription(self):
        return "Use this as a template for more complicated tests."

    def getHelp(self):
        return open("data/help/PathfinderTest.txt").read()

    def pump(self):
        """
        This gets called every frame that the test is running.  We have nothing
        to do here for this test.
        """
        pass

    def loadMap(self, filename):
        """
        Simple function to load and display a map file. We could of course
        have passed in the map filename but I'll leave that up to you.

        @param filename The filename.
        """

        self._mapfilename = filename

        if self._loader.isLoadable(self._mapfilename):
            self._map = self._loader.load(self._mapfilename)
            self._mapLoaded = True

        self._camera = self._map.getCamera("camera1")
        self._actorlayer = self._map.getLayer("item_layer")
        self._topactorlayer = self._map.getLayer("top_item_layer")
        self._peakactorlayer = self._map.getLayer("peak_item_layer")
        self._groundlayer = self._map.getLayer("ground_layer")
        self._player = self._actorlayer.getInstance("player")
        self._camera.setLocation(self._player.getLocation())
        self._camera.attach(self._player)
        gridrenderer = self._camera.getRenderer("GridRenderer")
        gridrenderer.activateAllLayers(self._map)

        coordrenderer = fife.CoordinateRenderer.getInstance(self._camera)
        coordrenderer.setFont(self._font)
        coordrenderer.clearActiveLayers()
        coordrenderer.addActiveLayer(self._peakactorlayer)

        cellrenderer = fife.CellRenderer.getInstance(self._camera)
        cellrenderer.setEnabled(True)
        cellrenderer.setFont(self._font)
        cellrenderer.clearActiveLayers()
        cellrenderer.addActiveLayer(self._topactorlayer)
        # cellrenderer.setEnabledBlocking(True)
        # cellrenderer.setEnabledPathVisual(True)
        # cellrenderer.addPathVisual(self._player)

    def getLocationAt(self, screenpoint):
        """
        Query the main camera for the Map location (on the actor layer)
        that a screen point refers to.

        @param screenpoint A fife.ScreenPoint

        Returns
        -------
        fife.Location
            Location object representing the map coordinates corresponding to the screen point.
        """

        target_mapcoord = self._camera.toMapCoordinates(screenpoint, False)
        target_mapcoord.z = 0
        location = fife.Location(self._actorlayer)

        location.setMapCoordinates(target_mapcoord)
        # use top instance
        layers = self._map.getLayers()
        for layer in reversed(layers):
            loc = fife.Location(layer)
            loc.setMapCoordinates(target_mapcoord)
            instances = layer.getInstancesAt(loc)
            if len(instances) > 0:
                if (
                    layer.getId() == "peak_item_layer"
                    or layer.getId() == "peak_ground_layer"
                ):
                    loc.setLayer(self._peakactorlayer)
                    location = loc
                    break
                if (
                    layer.getId() == "top_item_layer"
                    or layer.getId() == "top_ground_layer"
                ):
                    loc.setLayer(self._topactorlayer)
                    location = loc
                    break
                if (
                    layer.getId() == "item_layer"
                    or layer.getId() == "ground_layer"
                    or layer.getId() == "water_layer"
                ):
                    loc.setLayer(self._actorlayer)
                    location = loc
                    break
        return location

    def movePlayer(self, screenpoint):
        """
        Simple function that moves the player instance to the given screenpoint.

        @param screenpoint A fife.ScreenPoint
        """

        self._player.move("walk", self.getLocationAt(screenpoint), 4.0)
