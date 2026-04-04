#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from scripts.objects.baseobject import BaseGameObject, GameObjectTypes


class BaseItem(BaseGameObject):
    def __init__(
        self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname
    ):
        super().__init__(
            gamecontroller, layer, typename, baseobjectname, itemtype, itemname, True
        )

    def _getItemType(self):
        return self._name

    def _getItemName(self):
        return self._id

    itemtype = property(_getItemType)
    itemname = property(_getItemName)


class PickableItem(BaseItem):
    def __init__(
        self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname
    ):
        super().__init__(
            gamecontroller, layer, typename, baseobjectname, itemtype, itemname
        )
        self._type = GameObjectTypes["ITEM"]

    def onPickUp(self):
        # remove item from the scene
        self._gamecontroller.scene.removeObjectFromScene(self)

    def onDrop(self, dropx, dropy):
        # recreate object
        self._createFIFEInstance(self, self._gamecontroller.scene.itemlayer)
        self.setMapPosition(dropx, dropy)

        self._gamecontroller.scene.addObjectToScene(self)


class GoldStack(PickableItem):
    def __init__(
        self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname
    ):
        super().__init__(
            gamecontroller, layer, typename, baseobjectname, itemtype, itemname
        )

        self._value = 0

    def serialize(self):
        lvars = super().serialize()
        lvars["value"] = self._value

        return lvars

    def deserialize(self, valuedict):
        super().deserialize(valuedict)

        if "value" in valuedict:
            self._value = int(valuedict["value"])
        else:
            self._value = 0

    def _getValue(self):
        return self._value

    def _setValue(self, value):
        self._value = int(value)

    value = property(_getValue, _setValue)


class Portal(BaseItem):
    def __init__(
        self, gamecontroller, layer, typename, baseobjectname, itemtype, itemname
    ):
        super().__init__(
            gamecontroller, layer, typename, baseobjectname, itemtype, itemname
        )
        self._type = GameObjectTypes["PORTAL"]

        self._dest = None

    def serialize(self):
        lvars = super().serialize()
        lvars["dest"] = self._dest

        return lvars

    def deserialize(self, valuedict):
        super().deserialize(valuedict)

        if "dest" in valuedict:
            self._dest = valuedict["dest"]
        else:
            self._dest = "town"

    def _getDest(self):
        return self._dest

    def _setDest(self, dest):
        self._dest = dest

    dest = property(_getDest, _setDest)
