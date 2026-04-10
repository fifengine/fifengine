#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Actor action and base actor implementations for the RPG demo."""

from scripts.misc.serializer import Serializer
from scripts.objects.baseobject import (
    BaseGameObject,
    GameObjectTypes,
    ObjectActionListener,
)
from scripts.objects.items import GoldStack

Actions = {"NONE": 0, "PICKUP": 1, "TALK": 2, "ATTACK": 3, "OPEN": 4, "ENTER": 5}


class BaseAction:
    """Abstract base for actions that actors can perform."""

    def __init__(self):
        self._actiontype = Actions["NONE"]

    def execute(self):
        """Execute the action (override in subclasses)."""
        pass


class TalkAction(BaseAction):
    """Action that causes one object to talk to another."""

    def __init__(self, sourceobj, destobj):
        self._actiontype = Actions["TALK"]
        self._source = sourceobj
        self._dest = destobj

    def execute(self):
        """Execute the talk action, delegating to quest logic where appropriate."""
        if self._dest.type == GameObjectTypes["QUESTGIVER"]:
            if self._dest.haveQuest():
                if not self._dest.activequest:
                    self._dest.offerNextQuest()
                else:
                    self._dest.completeQuest()
            else:
                self._dest.showNoQuestDialog()
        else:
            self._dest.say("Hello there!")


class AttackAction(BaseAction):
    """Action representing an attack from one actor to another."""

    def __init__(self, attacker, defender):
        self._actiontype = Actions["ATTACK"]
        self._attacker = attacker
        self._defender = defender

    def execute(self):
        """Execute the attack action (simple flavor text in demo)."""
        if self._defender.type == GameObjectTypes["ENEMY"]:
            self._defender.say("Ouch")


class PickUpItemAction(BaseAction):
    """Action to pick up an item and add it to an actor's inventory."""

    def __init__(self, actor, item):
        self._actiontype = Actions["PICKUP"]
        self._actor = actor
        self._item = item

    def execute(self):
        """Execute the pickup action by delegating to the actor."""
        self._actor.pickUpItem(self._item)


class EnterPortalAction(BaseAction):
    """Action to enter a portal and trigger a map switch."""

    def __init__(self, actor, portal):
        self._actiontype = Actions["ENTER"]
        self._actor = actor
        self._portal = portal

    def execute(self):
        """Execute the enter-portal action by instructing the actor to enter."""
        self._actor.enterPortal(self._portal)


ActorStates = {"STAND": 0, "WALK": 1, "ATTACK": 2}


class ActorActionListener(ObjectActionListener):
    """Adapter that maps FIFE instance actions to actor methods."""

    def __init__(self, gamecontroller, obj):
        super().__init__(gamecontroller, obj)

    def onInstanceActionFinished(self, instance, action):
        """Handle finished instance actions, transition actor state when walking."""
        if action.getId() == "walk":
            self._object.stand()
            self._object.performNextAction()

    def onInstanceActionCancelled(self, instance, action):
        """Handle cancelled instance actions (no-op by default)."""
        pass


class ActorAttributes(Serializer):
    """Container for actor attribute values with serialize/deserialize hooks."""

    def __init__(self, strength=0, dexterity=0, intelligence=0, health=0, walkspeed=0):
        self._str = strength
        self._dex = dexterity
        self._int = intelligence
        self._hp = health
        self._walkspeed = walkspeed

    def serialize(self):
        """Serialize attribute values into a plain dict.

        Returns
        -------
        dict
            A mapping of attribute names to their serialized values.
        """
        lvars = {}

        lvars["str"] = self._str
        lvars["dex"] = self._dex
        lvars["int"] = self._int
        lvars["hp"] = self._hp
        lvars["walk_speed"] = self._walkspeed

        return lvars

    def deserialize(self, valuedict):
        """Deserialize attribute values from a dict, coercing types as needed."""
        if "str" in valuedict:
            self._str = int(valuedict["str"])
        if "dex" in valuedict:
            self._dex = int(valuedict["dex"])
        if "int" in valuedict:
            self._int = int(valuedict["int"])
        if "hp" in valuedict:
            self._hp = int(valuedict["hp"])
        if "walk_speed" in valuedict:
            self._walkspeed = float(valuedict["walk_speed"])

    def _getStrength(self):
        return self._str

    def _setStrength(self, strength):
        self._str = strength

    def _getDexterity(self):
        return self._dexterity

    def _setDexterity(self, dexterity):
        self._dexterity = dexterity

    def _getIntelligence(self):
        return self._int

    def _setIntelligence(self, intelligence):
        self._int = intelligence

    def _getHealth(self):
        return self._hp

    def _setHealth(self, health):
        self._hp = health

    def _getWalkSpeed(self):
        return self._walkspeed

    def _setWalkSpeed(self, walkspeed):
        self._walkspeed = walkspeed

    strength = property(_getStrength, _setStrength)
    dexterity = property(_getDexterity, _setDexterity)
    intelligence = property(_getIntelligence, _setIntelligence)
    health = property(_getHealth, _setHealth)
    walkspeed = property(_getWalkSpeed, _setWalkSpeed)


class Actor(BaseGameObject):
    """Base actor class providing movement, inventory and simple actions."""

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
        super().__init__(
            gamecontroller,
            layer,
            typename,
            baseobjectname,
            instancename,
            instanceid,
            createInstance,
        )

        self._nextaction = None
        self._inventory = []
        self._maxinventoryitems = 20

        self._gold = 0
        self._attributes = ActorAttributes()

        # set the default walkspeed
        self._attributes.walkspeed = self._gamecontroller.settings.get(
            "RPG", "DefaultActorWalkSpeed", 4.0
        )

        self.stand()

    def stand(self):
        """Set actor to standing state and play the stand animation."""
        self._state = ActorStates["STAND"]
        self._instance.actOnce("stand", self._instance.getFacingLocation())

    def walk(self, location):
        """Set actor to walking state and move towards the location."""
        self._state = ActorStates["WALK"]
        self._instance.move("walk", location, self._attributes.walkspeed)

    def say(self, text):
        """Make the actor say `text` using the FIFE instance text display."""
        self._instance.say(text, 2500)

    def performNextAction(self):
        """Execute and clear the queued next action if one is present."""
        if self._nextaction:
            self._nextaction.execute()
            self._nextaction = None

    def pickUpItem(self, item):
        """Attempt to pick up `item` and add it to the inventory."""
        if self.addItemToInventory(item):
            item.onPickUp()
        else:
            # could do something cool like throw the item back on the ground
            pass

    def enterPortal(self, portal):
        """Enter the portal, triggering a map switch for players or removal for others."""
        if self._id == "player":
            self._gamecontroller.switchMap(portal.dest)
        else:
            self._gamecontroller.scene.removeObjectFromScene(self._id)

    def addItemToInventory(self, item):
        """Try to add `item` to inventory; return True on success, False if full.

        Returns
        -------
        bool
            True if the item was added, False if the inventory was full.
        """
        if len(self._inventory) >= self._maxinventoryitems:
            return False
        else:
            if isinstance(item, GoldStack):
                self._gold += item.value
            else:
                self._inventory.append(item)

        return True

    def removeItemFromInventory(self, itemid):
        """Remove the item with `itemid` from the inventory if present."""
        itemtoremove = None
        for item in self._inventory:
            if item.id == itemid:
                itemtoremove = item

        if itemtoremove:
            self._inventory.remove(itemtoremove)

    def serialize(self):
        """Serialize actor state (gold and attributes) into a dict.

        Returns
        -------
        dict
            Serialized actor state including gold and attribute values.
        """
        lvars = super().serialize()

        lvars["gold"] = self._gold

        att_vars = self._attributes.serialize()
        for key, value in list(att_vars.items()):
            lvars[key] = value

        return lvars

    def deserialize(self, valuedict):
        """Deserialize actor state from a dict, restoring gold and attributes."""
        super().deserialize(valuedict)

        if "gold" in valuedict:
            self._gold = int(valuedict["gold"])
        else:
            self._gold = 0

        self._attributes.deserialize(valuedict)

    def _getState(self):
        return self._state

    def _setState(self, state):
        self._state = state

    def _getNextAction(self):
        return self._nextaction

    def _setNextAction(self, action):
        self._nextaction = action

    def _getGold(self):
        return self._gold

    def _setGold(self, gold):
        self._gold = gold

    def _getInventory(self):
        return self._inventory

    def _getAttributes(self):
        return self._attributes

    state = property(_getState, _setState)
    nextaction = property(_getNextAction, _setNextAction)
    gold = property(_getGold, _setGold)
    inventory = property(_getInventory)
    attributes = property(_getAttributes)
