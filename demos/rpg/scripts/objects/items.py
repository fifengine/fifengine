#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Item types used by the RPG demo (pickups, gold, portals)."""

from scripts.objects.baseobject import BaseGameObject, GameObjectTypes


class BaseItem(BaseGameObject):
    """Base class for in-game items."""

    def __init__(
        self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname
    ):
        super().__init__(
            gamecontroller, layer, typename, baseobjectname, itemtype, itemname, True
        )

    def _getItemType(self):
        """Return the item's type identifier.

        Returns
        -------
        str
            The type identifier for this item.
        """
        return self._name

    def _getItemName(self):
        """Return the item's instance name/ID.

        Returns
        -------
        str
            The instance id/name of the item.
        """
        return self._id

    itemtype = property(_getItemType)
    itemname = property(_getItemName)


class PickableItem(BaseItem):
    """An item that can be picked up by players."""

    def __init__(
        self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname
    ):
        super().__init__(
            gamecontroller, layer, typename, baseobjectname, itemtype, itemname
        )
        self._type = GameObjectTypes["ITEM"]

    def onPickUp(self):
        """Remove the item from the scene when picked up."""
        self._gamecontroller.scene.removeObjectFromScene(self)

    def onDrop(self, dropx, dropy):
        """Recreate and place the item back into the scene at given coords."""
        self._createFIFEInstance(self, self._gamecontroller.scene.itemlayer)
        self.setMapPosition(dropx, dropy)

        self._gamecontroller.scene.addObjectToScene(self)


class GoldStack(PickableItem):
    """A stack of gold that can be picked up by players."""

    def __init__(
        self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname
    ):
        super().__init__(
            gamecontroller, layer, typename, baseobjectname, itemtype, itemname
        )

        self._value = 0

    def serialize(self):
        """Serialize the gold stack value along with base object data.

        Returns
        -------
        dict
            The serialized representation of the gold stack.
        """
        lvars = super().serialize()
        lvars["value"] = self._value

        return lvars

    def deserialize(self, valuedict):
        """Deserialize the stored gold value if present."""
        super().deserialize(valuedict)

        if "value" in valuedict:
            self._value = int(valuedict["value"])
        else:
            self._value = 0

    def _getValue(self):
        """Return the stored gold value.

        Returns
        -------
        int
            The integer gold value stored in this stack.
        """
        return self._value

    def _setValue(self, value):
        """Set the stored gold value, coercing to int."""
        self._value = int(value)

    value = property(_getValue, _setValue)


class Portal(BaseItem):
    """A portal item that transports players to another map destination."""

    def __init__(
        self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname
    ):
        super().__init__(
            gamecontroller, layer, typename, baseobjectname, itemtype, itemname
        )
        self._type = GameObjectTypes["PORTAL"]

        self._dest = None

    def serialize(self):
        """Serialize the portal destination along with base data.

        Returns
        -------
        dict
            The serialized representation of the portal.
        """
        lvars = super().serialize()
        lvars["dest"] = self._dest

        return lvars

    def deserialize(self, valuedict):
        """Deserialize the portal's destination, defaulting to 'town'."""
        super().deserialize(valuedict)

        if "dest" in valuedict:
            self._dest = valuedict["dest"]
        else:
            self._dest = "town"

    def _getDest(self):
        """Return the portal destination identifier.

        Returns
        -------
        str
            The destination identifier for this portal.
        """
        return self._dest

    def _setDest(self, dest):
        """Set the portal destination identifier."""
        self._dest = dest

    dest = property(_getDest, _setDest)
