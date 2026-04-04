#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife
from fife.fife import FloatRect as Rect  # noqa: F401

from scripts.common.baseobject import (
    SHTR_ENEMYSHIP,
    SHTR_LASTBOSS,
    SHTR_PLAYER,
    SHTR_POWERUP,
    SHTR_PROJECTILE,
)
from scripts.powerups import (
    CannonSpread5PU,
    ExtraLifePU,
    PowerUp,
)
from scripts.ships.enemies import (
    Boss,
    DiagSaucer,
    Saucer1,
    Saucer2,
    Streaker,
)
from scripts.ships.player import Player
from scripts.ships.shipbase import Ship


class SoundEmitterListener(fife.SoundEmitterListener):
    def __init__(self, emitter, scene):
        fife.SoundEmitterListener.__init__(self)
        emitter.addListener(self)
        self._scene = scene

    def onSoundFinished(self, emitterId, soundClipId):
        self._scene.musicHasFinished()


class SceneNode:
    """
    A node in the scene graph.

    This represents a node in the scene.  The node stores a list
    of objects that exist in the node.  This is used by the Scene
    to reduce the number of collision checks.
    """

    def __init__(self, spaceobjects=None):
        """
        @param spaceobjects A list of spaceobjects that will exist in this node
        """
        if not spaceobjects:
            self._spaceobjects = list()
        else:
            self._spaceobjects = spaceobjects

    def _getObjects(self):
        return self._spaceobjects

    def _setObjects(self, spaceobjects):
        self._spaceobjects = spaceobjects

    spaceobjects = property(_getObjects, _setObjects)


class Scene:
    """
    Master game scene.  Keeps track of all game objects.

    This is the meat and potatoes of the game.  This class takes care of the scene graph,
    updating objects, destroying objects, collision detection, etc etc.
    """

    def __init__(self, world, engine, objectLayer, soundmanager):
        """
        @param world: A reference to the master instance of the World class
        @type world: L{World}
        @param engine: A reference to the FIFE engine
        @type engine: L{fife.Engine}
        @param objectLayer: The layer that all objects exist on
        @type objectLayer: L{fife.Layer}
        @param soundmanager: A reference to the SoundManager
        @type soundmanager: L{fife.SoundManager}
        """
        self._engine = engine
        self._world = world
        self._model = engine.getModel()
        self._layer = objectLayer
        self._soundmanager = soundmanager
        # set the listener orientation
        # self._soundmanager.setListenerOrientation(AudioSpaceCoordinate(0, 1, 0))
        # deactivate distance model
        self._soundmanager.setDistanceModel(fife.SD_DISTANCE_NONE)
        # create and enable soundeffect
        self._soundeffect = self._soundmanager.createSoundEffectPreset(
            fife.SE_PRESET_SPACESTATION_LONGPASSAGE
        )
        self._soundmanager.enableSoundEffect(self._soundeffect)

        self._nodes = list()

        self._player = None
        self._objectstodelete = list()

        self._maxnodes = 128
        self._xscale = 0

        self._time = 0
        self._timedelta = 0
        self._lasttime = 0

        self._xscale = self._layer.getCellGrid().getXScale()
        self._yscale = self._layer.getCellGrid().getYScale()

        self._paused = False
        self._timemod = 0

        self._gameover = False

        self._music = None
        self._musiclistener = None

    def destroyScene(self):
        """
        Removes all objects from the scene and deletes them from the layer.
        """
        nodestodelete = list()
        objtodelete = list()

        for node in self._nodes:
            nodestodelete.append(node)
            for obj in node.spaceobjects:
                objtodelete.append(node)

            for obj in objtodelete:
                if obj in node.spaceobjects:
                    if obj.instance:
                        self._layer.deleteInstance(obj.instance)
                        obj.instance = None
                    node.spaceobjects.remove(obj)

            objtodelete = list()

        for node in nodestodelete:
            if node in self._nodes:
                self._nodes.remove(node)

    def initScene(self, mapobj):
        """
        Initializess the scene and scene graph.  This creates game objects for
        FIFE instances that exist in the map.

        The scene graph is a one dimensional graph.  Each node represents one layer
        unit.  Objects are added to nodes in the scene graph based on their exact
        layer coordinates.
        """

        # initialize our scene array to some arbitrary size
        for i in range(0, self._maxnodes):
            self._nodes.append(SceneNode())

        self._player = Player(self, "player")
        self._player.init()
        self._player.start()

        enemies = list()
        powerups = list()

        temp = self._layer.getInstances("dodge1")
        enemies.extend(temp)

        temp = self._layer.getInstances("dodge2")
        enemies.extend(temp)

        temp = self._layer.getInstances("diag_top_right")
        enemies.extend(temp)

        temp = self._layer.getInstances("diag_bottom_right")
        enemies.extend(temp)

        temp = self._layer.getInstances("streaker")
        enemies.extend(temp)

        temp = self._layer.getInstances("boss")
        enemies.extend(temp)

        temp = self._layer.getInstances("cannonspread5")
        powerups.extend(temp)

        temp = self._layer.getInstances("extralife")
        powerups.extend(temp)

        for instance in enemies:
            objectName = instance.getId()

            if objectName == "dodge1":
                enemy = Saucer1(self, "enemy", instance, False)
            elif objectName == "dodge2":
                enemy = Saucer2(self, "enemy", instance, False)
            elif objectName == "diag_top_right":
                enemy = DiagSaucer(self, "enemy", 0, instance, False)
            elif objectName == "diag_bottom_right":
                enemy = DiagSaucer(self, "enemy", 1, instance, False)
            elif objectName == "streaker":
                enemy = Streaker(self, "enemy", instance, False)
            elif objectName == "boss":
                enemy = Boss(self, "enemy", instance, False)
            else:
                enemy = Ship(self, "enemy", instance, False)

            enemy.start()

            loc = instance.getLocation().getExactLayerCoordinates()
            nodeindex = int(loc.x * self._xscale)
            enemy.scenenodeid = nodeindex
            self._nodes[nodeindex].spaceobjects.append(enemy)

        for instance in powerups:
            objectName = instance.getId()

            if objectName == "cannonspread5":
                powerup = CannonSpread5PU(self, "cannonspread5", instance, False)
            elif objectName == "extralife":
                powerup = ExtraLifePU(self, "extralife", instance, False)
            else:
                powerup = PowerUp(self, "powerup", instance, False)

            powerup.start()

            loc = instance.getLocation().getExactLayerCoordinates()
            nodeindex = int(loc.x * self._xscale)
            powerup.scenenodeid = nodeindex
            self._nodes[nodeindex].spaceobjects.append(powerup)

        # and finally add the player to the scene
        self.addObjectToScene(self._player)

        self._music = self._soundmanager.createEmitter("music/waynesmind2.ogg")
        self._musiclistener = SoundEmitterListener(self._music, self)
        self._music.play()

        self.startCamera()

    def musicHasFinished(self):
        """
        Sound callback example that gets fired after the music has finished playing.
        """
        if self._gameover:
            return
        print(
            self._music.getSoundClip().getName()
            + " has finished playing.  Starting it again...\n"
        )
        self._music.play()

    def pause(self, time):
        self._pausedtime = time
        self._paused = True

    def unpause(self, time):
        self._timemod += time - self._pausedtime
        self._paused = False

    def playerHit(self, damage):
        self._player.applyHit(damage)

    def gameOver(self):
        self._gameover = True
        self._music.stop()
        self._world.gameOver()

    def endLevel(self):
        self._gameover = True
        self._music.stop()
        self._world.endLevel()

    def queueObjectForRemoval(self, obj):
        """
        You should use this function to remove an object from the scene.
        The garbage collector is what ultimately deletes the object from memory.
        DO NOT do this yourself as you may cause issues with segfaults.
        """
        self._objectstodelete.append(obj)

    def getObjectsInNode(self, nodeindex):
        """
        @param nodeindex: the index of the node you which to retrieve objects from.
        @return: The list of objects in the specified node index
        """
        return self._nodes[nodeindex].instances

    def getObjectsInRange(self, rangeL, rangeR):
        """
        @param rangeL: the left most node index
        @param rangeR: the right most node index
        @return: A combined list of objects in the specified node index range (inclusive)
        """
        objects = list()

        for i in range(rangeL, rangeR):
            objects.extend(self._nodes[i].spaceobjects)

        return objects

    def addObjectToScene(self, obj):
        """
        Adds an object to the scene in the correct scene node

        @param obj: The object to add to the scene
        """

        # @todo: search to ensure the object isn't already part of the scene
        loc = obj.instance.getLocation().getExactLayerCoordinates()
        nodeindex = int(loc.x * self._xscale)

        if nodeindex >= 0:
            obj.scenenodeid = nodeindex
            if obj not in self._nodes[nodeindex].spaceobjects:
                self._nodes[nodeindex].spaceobjects.append(obj)
        else:
            self.queueObjectForRemoval(obj)

    def moveObjectInScene(self, obj):
        """
        When an object moves in the scene you should call this function to update
        scene graph.  You MUST do this or the graph will be incorrect.

        @param obj: The object to move in the scene
        """

        loc = obj.instance.getLocation().getExactLayerCoordinates()
        nodeindex = int(loc.x * self._xscale)

        if nodeindex >= 0:
            if nodeindex != obj.scenenodeid:
                if obj in self._nodes[obj.scenenodeid].spaceobjects:
                    self._nodes[obj.scenenodeid].spaceobjects.remove(obj)

                self._nodes[nodeindex].spaceobjects.append(obj)

                obj.scenenodeid = nodeindex
        else:
            self.queueObjectForRemoval(obj)

    def removeObjectFromScene(self, obj):
        """
        You would not normally call this function directly.  You should probably
        call queueObjectForRemoval().

        This function releases any memory allocated for the object by deleting
        the FIFE instance.

        @param obj: The object to delete
        """
        for node in self._nodes:
            if obj in node.spaceobjects:
                if obj.instance:
                    self._layer.deleteInstance(obj.instance)
                    obj.instance = None
                node.spaceobjects.remove(obj)
                break

    def attachCamera(self, cam):
        self._camera = cam
        self._camera.setLocation(self._player.location)

    def stopCamera(self):
        self._cameraspeed = 0

    def startCamera(self):
        """
        Starts the camera moving slowly to the right.
        """
        self._cameraspeed = 0.001

    def collectGarbage(self):
        """
        This should be called once a frame.  It removes the object from the scene.
        """
        for obj in self._objectstodelete:
            self.removeObjectFromScene(obj)

        self._objectstodelete = list()

    def update(self, time, keystate):
        """
        This function should be called once a frame.  This is where all the game logic
        happens.
        """
        timedelta = (time - self._timemod) - self._time
        self._timedelta = timedelta
        self._time = time - self._timemod

        self._keystate = keystate

        # some garbage cleanup
        self.collectGarbage()

        # update camera location
        loc = self._camera.getLocation()
        exactloc = self._camera.getLocation().getExactLayerCoordinates()
        # slowly move to the right
        exactloc.x += timedelta * self._cameraspeed
        loc.setExactLayerCoordinates(exactloc)
        self._camera.setLocation(loc)

        # update the listener position
        self._soundmanager.setListenerPosition(exactloc)

        topleft = self._camera.toMapCoordinates(fife.Point3D(0, 0, 0))
        bottomright = self._camera.toMapCoordinates(fife.Point3D(1024, 768, 0))

        # which scene nodes to use to update objects
        leftnode = int(topleft.x)
        rightnode = int(bottomright.x) + 1

        # get a list of objects on the screen
        if leftnode < 0:
            leftnode = 0
        if rightnode > self._maxnodes:
            rightnode = self._maxnodes
        screenlist = self.getObjectsInRange(leftnode, rightnode)

        # update objects on the screen
        for obj in screenlist:
            if obj.type == SHTR_LASTBOSS:
                if bottomright.x > (
                    (obj.location.getExactLayerCoordinates().x * self._xscale) + 0.5
                ):
                    self.stopCamera()

            if not (obj.type == SHTR_PLAYER and self._gameover):
                obj.update()

            if obj.changedposition:
                self.moveObjectInScene(obj)

            if obj.type != SHTR_PLAYER and obj.type != SHTR_PROJECTILE:
                if obj.running and obj.boundingbox.intersects(self._player.boundingbox):
                    if obj.type == SHTR_ENEMYSHIP:
                        # player touched an enemy.  Destroy player and
                        # re-initialize scene
                        if not self._player.invulnerable:
                            # collision damage of 1
                            self.playerHit(1)
                            obj.applyHit(1)
                    elif obj.type == SHTR_POWERUP:
                        obj.applyPowerUp(self._player)

            elif obj.type == SHTR_PROJECTILE:
                # projectile went off the right edge of the screen.  Remove it from the scene.
                if obj.scenenodeid >= rightnode:
                    self.queueObjectForRemoval(obj)
                    continue

                # could probably just get the nodes in the projectiles scenenode.
                # use a range to be sure.
                pcollide = self.getObjectsInRange(
                    obj.scenenodeid - 1, obj.scenenodeid + 1
                )

                for o in pcollide:
                    # cant get hit by your own bullet
                    if (
                        obj.owner != o
                        and o.type != SHTR_PROJECTILE
                        and o.type != SHTR_POWERUP
                    ):
                        if o.running and obj.boundingbox.intersects(o.boundingbox):
                            if o != self._player and obj.owner.type == SHTR_PLAYER:
                                o.applyHit(obj.damage)
                                # check if enemy ship was destroyed
                                if not o.running:
                                    self._player.applyScore(o.scorevalue)
                                obj.destroy()
                            elif o == self._player:
                                # player got hit by a projectile
                                if not self._player.invulnerable:
                                    self.playerHit(obj.damage)
                                    obj.destroy()

                # queue list of projectiles to remove (ttl expired or has been destroyed)
                if not obj.running:
                    self.queueObjectForRemoval(obj)

            # self._world.renderBoundingBox(obj)

        """
		Do some more cleanup.  Objects that leave the scene should be removed.

		@todo: Objects that leave the scene should be added to the scenes object removal
		in the objects update function.  The current implementation is much slower.
		"""

        worldlist = self.getObjectsInRange(0, self._maxnodes)

        for obj in worldlist:
            if obj.scenenodeid < leftnode and obj.type != SHTR_PLAYER:
                self.queueObjectForRemoval(obj)
            if obj.type == SHTR_PROJECTILE and obj.scenenodeid > rightnode:
                self.queueObjectForRemoval(obj)

    def _getPlayer(self):
        return self._player

    def _getKeyState(self):
        return self._keystate

    def _getCamera(self):
        return self._camera

    def _getObjectLayer(self):
        return self._layer

    def _getModel(self):
        return self._model

    def _getTime(self):
        return self._time

    def _getTimeDelta(self):
        return self._timedelta

    def _getPaused(self):
        return self._paused

    def _getSoundManager(self):
        return self._soundmanager

    player = property(_getPlayer)
    keystate = property(_getKeyState)
    camera = property(_getCamera)
    objectlayer = property(_getObjectLayer)
    model = property(_getModel)
    time = property(_getTime)
    timedelta = property(_getTimeDelta)
    paused = property(_getPaused)
    soundmanager = property(_getSoundManager)
