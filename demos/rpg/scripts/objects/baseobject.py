#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Base game object utilities for the RPG demo."""

from fife import fife

from scripts.misc.exceptions import InstanceNotFoundError
from scripts.misc.serializer import Serializer

GameObjectTypes = {
    "DEFAULT": 0,
    "ITEM": 1,
    "QUESTGIVER": 2,
    "PLAYER": 3,
    "NPC": 4,
    "ENEMY": 5,
    "GOLD": 6,
    "PORTAL": 7,
}


class ObjectActionListener(fife.InstanceActionListener):
    """Adapter to attach an action listener to a game object instance."""

    def __init__(self, gamecontroller, obj):
        """Initialize the listener with gamecontroller and target object."""
        fife.InstanceActionListener.__init__(self)
        self._gamecontroller = gamecontroller
        self._object = obj

        self._attached = False

    def detachActionListener(self):
        """Detach the action listener from the object's instance."""
        if self._attached:
            self._object.instance.removeActionListener(self)
            self._attached = False

    def attachActionListener(self):
        """Attach the action listener to the object's instance."""
        if not self._attached:
            self._object.instance.addActionListener(self)
            self._attached = True

    def onInstanceActionFinished(self, instance, action):
        """Handle instance action finished events (no-op by default)."""
        pass

    def onInstanceActionCancelled(self, instance, action):
        """Handle instance action cancelled events (no-op by default)."""
        pass

    def onInstanceActionFrame(self, instance, action, frame):
        """Handle per-frame instance action events (no-op by default)."""
        pass


class BaseGameObject(Serializer):
    """Serializable game object base class for RPG demo objects."""

    def __init__(
        self,
        gamecontroller,
        layer,
        typename,
        baseobjectname,
        instancename,
        instanceid=None,
        createInstance=False,
    ):
        """Initialize the BaseGameObject with controller, layer, and instance info."""
        self._gamecontroller = gamecontroller
        self._fifeobject = None

        self._typename = typename
        self._type = GameObjectTypes[typename]
        self._baseobjectname = baseobjectname

        self._name = instancename
        if instanceid:
            self._id = instanceid
        else:
            self._id = self._name

        self._instance = None
        self._position = fife.DoublePoint(0.0, 0.0)

        self._actionlistener = None

        self._layer = layer

        if createInstance:
            self._createFIFEInstance(self._layer)
        else:
            self._findFIFEInstance(self._layer)

        self._activated = True

    def hide(self):
        """Mark the FIFE instance as not visible."""
        if self._instance:
            self._instance.get2dGfxVisual().setVisible(False)

    def show(self):
        """Mark the FIFE instance as visible."""
        if self._instance:
            self._instance.get2dGfxVisual().setVisible(True)

    def destroy(self):
        """Delete the FIFE instance from the actor layer and cleanup."""
        if self._actionlistener:
            self._actionlistener.detachActionListener()

        if self._instance:
            self._layer.deleteInstance(self._instance)
            self._instance = None

        self._activated = False

    def spawn(self, x, y):
        """Spawn or move the object to the specified map coordinates."""
        if self._instance:
            self._setMapPostion(x, y)
            self.show()
        else:
            self._position.x = x
            self._position.y = y
            self._createFIFEInstance(self, self._layer)

        if self._actionlistener and self._instance:
            self._actionlistener.attachActionListener()

        self._activated = True

    def setMapPosition(self, x, y):
        """Set the object's position on the map to the given coordinates."""
        curloc = self.location

        self._position = self.location.getExactLayerCoordinates()
        self._position.x = x
        self._position.y = y

        curloc.setExactLayerCoordinates(self._position)
        self.location = curloc

    def serialize(self):
        """Serialize the object's position and type to a dict.

        Returns
        -------
        dict
            Mapping of serialized object fields (posx, posy, type, objectname).
        """
        lvars = {}
        x, y = self.position
        lvars["posx"] = x
        lvars["posy"] = y
        lvars["type"] = self._typename
        lvars["objectname"] = self._baseobjectname

        return lvars

    def deserialize(self, valuedict=None):
        """Deserialize position and type information from a dict."""
        if not valuedict:
            return

        if "posx" in valuedict:
            x = float(valuedict["posx"])
        else:
            x = 0

        if "posy" in valuedict:
            y = float(valuedict["posy"])
        else:
            y = 0

        self.setMapPosition(x, y)

    def _createFIFEInstance(self, layer):
        """Create the underlying FIFE object and instance on the given layer."""
        mapmodel = self._gamecontroller.engine.getModel()
        self._fifeobject = mapmodel.getObject(
            self._name,
            self._gamecontroller.settings.get(
                "RPG", "ObjectNamespace", "http://www.fifengine.net/xml/rpg"
            ),
        )

        self._instance = layer.createInstance(
            self._fifeobject,
            fife.Point3D(int(round(self._position.x)), int(round(self._position.y)), 0),
            self._id,
        )
        fife.InstanceVisual.create(self._instance)

        self._instance.thisown = 0

    def _findFIFEInstance(self, layer):
        """Locate the FIFE instance on the given layer, raising if not found.

        Raises
        ------
        InstanceNotFoundError
            If the underlying FIFE instance cannot be found on `layer`.
        """
        # Raises InstanceNotFoundError if the instance was not found on the layer.
        self._instance = self._layer.getInstance(self._id)
        if self._instance:
            self._instance.thisown = 0
        else:
            raise InstanceNotFoundError(self._id + " was not found on the layer!")

    def _getLocation(self):
        return self._instance.getLocation()

    def _setLocation(self, loc):
        self._instance.setLocation(loc)

    def _getInstance(self):
        return self._instance

    def _getType(self):
        return self._type

    def _getId(self):
        return self._id

    def _getModelName(self):
        return self._name

    def _getPosition(self):
        # if there isn't a FIFE instance just return last known coordinates
        if self._instance:
            self._position = self.location.getExactLayerCoordinates()

        return (self._position.x, self._position.y)

    def _setPosition(self, tuplexy):
        self.setMapPosition(tuplexy[0], tuplexy[1])

    def _getActivated(self):
        return self._activated

    def _setActivated(self, activate):
        self._activated = activate

    location = property(_getLocation, _setLocation)
    instance = property(_getInstance)
    type = property(_getType)
    id = property(_getId)
    modelname = property(_getModelName)
    position = property(_getPosition, _setPosition)
    active = property(_getActivated, _setActivated)
