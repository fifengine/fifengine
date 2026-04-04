#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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
    def __init__(self, gamecontroller):
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
        """
        @todo: once we have all art assets this should be able to load one of 3 player models
        """
        modeldict = self._modelsettings.get("models", "Player", {})

        print("loading: %s" % modeldict["file"])
        self._loader.loadImportFile(modeldict["file"])
        self._player = Player(self._gamecontroller, self.actorlayer, "warrior")

        playerfilename = os.path.join("saves", "player_save.xml")

        if os.path.isfile(playerfilename):
            player_settings = SimpleXMLSerializer(playerfilename)
            pvals = player_settings.get("player", "player", {})
            self._player.deserialize(pvals)

    def createScene(self, mapname):
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
        """
        Query the main camera for instances on the specified layer.
        """
        screen_point = fife.Point3D(int(clickpoint.x), int(clickpoint.y), 0)
        return self.cameras[self._maincameraname].getMatchingInstances(
            screen_point, layer
        )

    def getLocationAt(self, clickpoint):
        """
        Query the main camera for the Map location (on the actor layer)
        that a screen point refers to.
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
        """
        Throws ObjectNotFoundError when an object cannot be found on the scene
        """

        try:
            return self._objectlist[objid]
        except KeyError:
            raise ObjectNotFoundError(objid + " was not found on the scene.")

    def addObjectToScene(self, obj):
        if obj.id not in self._objectlist:
            self._objectlist[obj.id] = obj
        else:
            obj.destroy()
            raise ObjectAlreadyInSceneError

    def removeObjectFromScene(self, obj):
        """
        Throws ObjectNotFoundError when an object cannot be found on the scene
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
        if self._mapname:
            self.createScene(self._mapname)

    def updateScene(self):
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
