#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Scene handling utilities for the RPG demo."""

import os
import uuid

from fife import fife
from fife.extensions.serializers.simplexml import SimpleXMLSerializer

from scripts.actors.baseactor import Actor
from scripts.actors.player import Player
from scripts.actors.questgiver import QuestGiver
from scripts.misc.exceptions import ObjectAlreadyInSceneError, ObjectNotFoundError
from scripts.misc.serializer import Serializer
from scripts.objects.items import GoldStack, Portal


class Scene(Serializer):
    """Manage map loading, objects, and scene lifecycle."""

    def __init__(self, gamecontroller):
        """Initialize a Scene with a reference to the GameController."""
        self._gamecontroller = gamecontroller

        self._map = None
        self._mapname = None
        self._maincameraname = self._gamecontroller.settings.get(
            "RPG", "DefaultCameraName", "camera1"
        )
        self._cameras = {}

        self._actorlayer = None

        self._player = None
        self._objectlist = {}

        self._objectsettings = None
        self._modelsettings = None

        self._loader = fife.MapLoader(
            self._gamecontroller.engine.getModel(),
            self._gamecontroller.engine.getVFS(),
            self._gamecontroller.engine.getImageManager(),
            self._gamecontroller.engine.getRenderBackend(),
        )

    def loadObject(self, objectname, objectid=None, valuedict=None):
        """Load an object by template name and return a new instance.

        If `objectid` is provided it will be used; otherwise a new id is generated.
        If `valuedict` is supplied it overrides the loaded default values.

        Returns
        -------
        object or None
            The created object instance, or ``None`` if the object's type is
            not recognised.

        Raises
        ------
        ObjectNotFoundError
            If the object template or its model cannot be found in the model
            settings.
        """
        if objectid:
            identifier = objectid
        else:
            identifier = str(uuid.uuid1())

        try:
            objdict = self._modelsettings.get("objects", objectname, {})
            modeldict = self._modelsettings.get("models", objdict["modelname"], {})

            self._loader.loadImportFile(modeldict["file"])

            if objdict["type"] == "GOLD":
                newobject = GoldStack(
                    self._gamecontroller,
                    self.itemlayer,
                    objdict["type"],
                    objectname,
                    modeldict["model"],
                    identifier,
                )
            elif objdict["type"] == "PORTAL":
                newobject = Portal(
                    self._gamecontroller,
                    self.itemlayer,
                    objdict["type"],
                    objectname,
                    modeldict["model"],
                    identifier,
                )
            elif objdict["type"] == "QUESTGIVER":
                newobject = QuestGiver(
                    self._gamecontroller,
                    self.actorlayer,
                    objdict["type"],
                    objectname,
                    modeldict["model"],
                    identifier,
                    True,
                )
            elif objdict["type"] == "ENEMY":
                newobject = Actor(
                    self._gamecontroller,
                    self.actorlayer,
                    objdict["type"],
                    objectname,
                    modeldict["model"],
                    identifier,
                    True,
                )
            else:
                return None

            # if the valuedict is supplied it overrides the loaded default values
            if valuedict:
                newobject.deserialize(valuedict)
            else:
                newobject.deserialize(objdict)

        except KeyError:
            raise ObjectNotFoundError

        return newobject

    def loadObjects(self, mapfilename):
        """Load all objects defined in the object settings for the map."""
        for obj in self._objectsettings.get("objects", "objectlist", []):
            try:
                objdict = self._objectsettings.get("objects", obj, {})
                newobj = self.loadObject(objdict["objectname"], obj, objdict)
                self._gamecontroller.logger.log_debug("Loaded object: " + obj)
            except ObjectNotFoundError:
                self._gamecontroller.logger.log_error(
                    "Error while loading object: " + obj
                )
                continue

            try:
                if newobj:
                    self.addObjectToScene(newobj)
            except ObjectAlreadyInSceneError:
                self._gamecontroller.logger.log_error(
                    "Object already part of scene:" + obj
                )
                continue

    def createPlayerObject(self):
        """Create the player object and load saved player state if available."""
        modeldict = self._modelsettings.get("models", "Player", {})

        print(f"loading: {modeldict['file']}")
        self._loader.loadImportFile(modeldict["file"])
        self._player = Player(self._gamecontroller, self.actorlayer, "warrior")

        playerfilename = os.path.join("saves", "player_save.xml")

        if os.path.isfile(playerfilename):
            player_settings = SimpleXMLSerializer(playerfilename)
            pvals = player_settings.get("player", "player", {})
            self._player.deserialize(pvals)

    def createScene(self, mapname):
        """Create and initialize the scene for the given map name."""
        mapfilename = os.path.join("maps", mapname + ".xml")

        if self._map:
            self.destroyScene()

        if self._loader.isLoadable(mapfilename):
            self._map = self._loader.load(mapfilename)

        self._mapname = mapname

        if os.path.isfile(os.path.join("saves", mapname + "_save.xml")):
            objectfile = os.path.join("saves", mapname + "_save.xml")
        else:
            objectfile = os.path.join("maps", mapname + "_objects.xml")

        modelfile = self._gamecontroller.settings.get(
            "RPG", "AllObjectFile", "maps/allobjects.xml"
        )

        self._objectsettings = SimpleXMLSerializer(objectfile)
        self._modelsettings = SimpleXMLSerializer(modelfile)

        for cam in self._map.getCameras():
            self._cameras[cam.getId()] = cam

        self._cameras[self._maincameraname].setZoom(
            self._gamecontroller.settings.get("RPG", "DefaultZoom", 1.0)
        )

        self._actorlayer = self._map.getLayer(
            self._gamecontroller.settings.get("RPG", "ActorLayer", "actor_layer")
        )
        self._itemlayer = self._map.getLayer(
            self._gamecontroller.settings.get("RPG", "ItemLayer", "item_layer")
        )

        self.loadObjects(mapfilename)

        # finally load the player
        self.createPlayerObject()

    def destroyScene(self):
        """Destroy the current scene and clean up loaded resources."""
        for obj in list(self._objectlist.values()):
            obj.destroy()

        self._objectlist.clear()
        self._objectlist = {}
        self._cameras.clear()
        self._cameras = {}

        self._player.destroy()
        self._player = None

        for cam in self._map.getCameras():
            cam.resetRenderers()

        if self._map:
            self._gamecontroller.engine.getModel().deleteMap(self._map)

        self._gamecontroller.engine.getModel().deleteObjects()

        self._map = None
        self._mapname = None

        self._actorlayer = None
        self._itemlayer = None

        self._objectsettings = None
        self._modelsettings = None

    def getInstancesAt(self, clickpoint, layer):
        """Return a list of instances on the specified layer at the screen point.

        Returns
        -------
        list
            A list of matching instances at the given screen point and layer.
        """
        screen_point = fife.Point3D(int(clickpoint.x), int(clickpoint.y), 0)
        return self.cameras[self._maincameraname].getMatchingInstances(
            screen_point, layer
        )

    def getLocationAt(self, clickpoint):
        """Return the map Location corresponding to a screen point.

        Returns
        -------
        fife.Location
            A `fife.Location` instance corresponding to the given screen
            coordinates.
        """
        screen_point = fife.Point3D(int(clickpoint.x), int(clickpoint.y), 0)
        target_mapcoord = self._cameras[self._maincameraname].toMapCoordinates(
            screen_point, False
        )
        target_mapcoord.z = 0
        location = fife.Location(self._actorlayer)
        location.setMapCoordinates(target_mapcoord)
        return location

    def getObject(self, objid):
        """Get an object by its id.

        Returns
        -------
        object
            The object with the given id.

        Raises
        ------
        ObjectNotFoundError
            If the object with `objid` cannot be found on the scene.
        """
        try:
            return self._objectlist[objid]
        except KeyError:
            raise ObjectNotFoundError(objid + " was not found on the scene.")

    def addObjectToScene(self, obj):
        """Add `obj` to the scene, raising if it is already present.

        Raises
        ------
        ObjectAlreadyInSceneError
            If an object with the same id already exists in the scene.
        """
        if obj.id not in self._objectlist:
            self._objectlist[obj.id] = obj
        else:
            obj.destroy()
            raise ObjectAlreadyInSceneError

    def removeObjectFromScene(self, obj):
        """Remove `obj` from the scene.

        Raises
        ------
        ObjectNotFoundError
            If the object to remove is not present in the scene.
        """
        obj.destroy()

        try:
            del self._objectlist[obj.id]
        except KeyError:
            raise ObjectNotFoundError(
                obj.id
                + " could not be removed from the scene as it was not found in the scene."
            )

    def serialize(self):
        """Serialize the current scene and player state to save files."""
        filename = os.path.join("saves", self._mapname + "_save.xml")
        playerfilename = os.path.join("saves", "player_save.xml")
        map_settings = SimpleXMLSerializer(filename)
        player_settings = SimpleXMLSerializer(playerfilename)

        objectlist = []

        for obj in list(self._objectlist.values()):
            ovals = obj.serialize()
            map_settings.set("objects", obj.id, ovals)
            objectlist.append(obj.id)

        map_settings.set("objects", "objectlist", objectlist)

        pvals = self._player.serialize()
        player_settings.set("player", "player", pvals)

        map_settings.save()
        player_settings.save()

    def deserialize(self):
        """Deserialize scene data; recreate the scene if a map name is set."""
        if self._mapname:
            self.createScene(self._mapname)

    def updateScene(self):
        """Update the scene each frame (placeholder)."""
        pass

    def _getActorLayer(self):
        return self._actorlayer

    def _getItemLayer(self):
        return self._itemlayer

    def _getCameras(self):
        return self._cameras

    def _getPlayer(self):
        return self._player

    def _getObjectList(self):
        return self._objectlist

    def _getMap(self):
        return self._map

    def _getMapName(self):
        return self._mapname

    actorlayer = property(_getActorLayer)
    itemlayer = property(_getItemLayer)
    cameras = property(_getCameras)
    player = property(_getPlayer)
    objectlist = property(_getObjectList)
    map = property(_getMap)
    mapname = property(_getMapName)
