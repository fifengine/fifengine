#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Enemy ship implementations for the shooter demo."""

from fife import fife
from fife.extensions import fife_timer

from scripts.common.baseobject import SHTR_ENEMYSHIP, SHTR_LASTBOSS
from scripts.ships.shipbase import Ship, ShipActionListener
from scripts.weapons import Cannon, FireBall, FireBallBurst, FireBallSpread


class EnemyActionListener(ShipActionListener):
    """Action listener for generic enemy ships."""

    def __init__(self, ship):
        super().__init__(ship)

    def onInstanceActionFinished(self, instance, action):
        """Handle completion of a ship instance action."""
        super().onInstanceActionFinished(instance, action)

    def onInstanceActionCancelled(self, instance, action):
        """Handle cancelled ship instance action (no-op)."""
        pass


class BossActionListener(ShipActionListener):
    """Action listener with boss-specific behavior."""

    def __init__(self, ship):
        super().__init__(ship)

    def onInstanceActionFinished(self, instance, action):
        """Handle completion of boss-specific instance actions."""
        super().onInstanceActionFinished(instance, action)

        if action.getId() == "explode":
            self.delayed = fife_timer.delayCall(5000, self._ship.endLevel())

    def onInstanceActionCancelled(self, instance, action):
        """Handle cancelled boss instance actions (no-op)."""
        pass


class Saucer1(Ship):
    """Simple oscillating saucer enemy."""

    def __init__(self, scene, name, instance, findInstance=True):
        super().__init__(scene, name, findInstance)
        self.instance = instance
        self._type = SHTR_ENEMYSHIP
        self._dir = 0
        self._time = 500
        self.width = 0.2
        self.height = 0.075
        self.velocity.x = -0.5

        self.weapon = Cannon(self._scene, self, 1000)
        self.weapon.projectilevelocity = 0.4

        self._actionlistener = EnemyActionListener(self)

        self.hitpoints = 1
        self.scorevalue = 50

    def update(self):
        """Update movement and firing for the saucer."""
        if self._dir == 1:
            self.applyThrust(fife.DoublePoint(0, -0.5))
        elif self._dir == 0:
            self.applyThrust(fife.DoublePoint(0, 0.5))

        if self._time >= 1000:
            if self._dir == 1:
                self._dir = 0
            elif self._dir == 0:
                self._dir = 1

            self._time = 0

        self._time += self._scene.timedelta

        super().update()

        self.fire(fife.DoublePoint(-1, 0))


class Saucer2(Ship):
    """Larger saucer with more hitpoints and slower movement."""

    def __init__(self, scene, name, instance, findInstance=True):
        super().__init__(scene, name, findInstance)
        self.instance = instance
        self._type = SHTR_ENEMYSHIP
        self._dir = 0
        self._time = 1000
        self.width = 0.2
        self.height = 0.2
        self.velocity.x = -0.1

        self.weapon = Cannon(self._scene, self, 2000)
        self.weapon.projectilevelocity = 0.4

        self._actionlistener = EnemyActionListener(self)

        self.hitpoints = 2
        self.scorevalue = 100

    def applyHit(self, hp):
        """Apply hit to the saucer and flash effect."""
        self.flash(1)
        super().applyHit(hp)

    def update(self):
        """Update movement and firing for the saucer."""
        if self._dir == 1:
            self.applyThrust(fife.DoublePoint(0, -0.25))
        elif self._dir == 0:
            self.applyThrust(fife.DoublePoint(0, 0.25))

        if self._time >= 2000:
            if self._dir == 1:
                self._dir = 0
            elif self._dir == 0:
                self._dir = 1

            self._time = 0

        self._time += self._scene.timedelta

        super().update()

        self.fire(fife.DoublePoint(-1, 0))


class DiagSaucer(Ship):
    """Diagonal-moving saucer that thrusts diagonally based on direction."""

    def __init__(self, scene, name, direction, instance, findInstance=True):
        super().__init__(scene, name, findInstance)
        self.instance = instance
        self._type = SHTR_ENEMYSHIP
        self.width = 0.2
        self.height = 0.075

        if direction == 0:
            self._ythrust = 0.25
        else:
            self._ythrust = -0.25

        self.weapon = Cannon(self._scene, self, 2000)
        self.weapon.projectilevelocity = 0.4

        self._actionlistener = EnemyActionListener(self)

        self.hitpoints = 1
        self.scorevalue = 50

    def update(self):
        """Update diagonal thrust and firing."""
        self.applyThrust(fife.DoublePoint(-0.25, self._ythrust))
        super().update()

        self.fire(fife.DoublePoint(-1, 0))


class Streaker(Ship):
    """Fast enemy that chases the player and fires accordingly."""

    def __init__(self, scene, name, instance, findInstance=True):
        super().__init__(scene, name, findInstance)
        self.instance = instance
        self._type = SHTR_ENEMYSHIP
        self.width = 0.2
        self.height = 0.2

        self._maxvelocity = 2.0

        self.weapon = FireBall(self._scene, self, 2000)
        self.weapon.projectilevelocity = 0.25

        self._actionlistener = EnemyActionListener(self)

        self.hitpoints = 2
        self.scorevalue = 150

    def applyHit(self, hp):
        """Apply hit to the streaker and flash effect."""
        self.flash(1)
        super().applyHit(hp)

    def update(self):
        """Update movement to chase the player and fire."""
        self.applyThrust(fife.DoublePoint(-0.40, 0))
        super().update()

        playerloc = self._scene.player.location.getExactLayerCoordinates()
        enemyloc = self.location.getExactLayerCoordinates()

        playerloc.x -= enemyloc.x
        playerloc.y -= enemyloc.y

        self.fire(fife.DoublePoint(playerloc.x, playerloc.y))


class Boss(Ship):
    """Boss ship with multiple attack phases and behaviors."""

    def __init__(self, scene, name, instance, findInstance=True):
        super().__init__(scene, name, findInstance)
        self.instance = instance
        self._type = SHTR_LASTBOSS
        self.width = 0.85
        self.height = 0.25

        self._maxvelocity = 2.0

        self.weapon = FireBall(self._scene, self, 1000)
        self.weapon.projectilevelocity = 0.5

        self._actionlistener = BossActionListener(self)

        self.hitpoints = 30
        self.scorevalue = 1000

        self._explodclip = self._scene.soundmanager.createEmitter(
            "sounds/bossexplode.ogg"
        )

    def endLevel(self):
        """Signal the scene that the boss ended the level."""
        self._scene.endLevel()

    def update(self):
        """Update boss behavior and fire towards the player."""
        super().update()

        playerloc = self._scene.player.location.getExactLayerCoordinates()
        bossloc = self.location.getExactLayerCoordinates()

        playerloc.x -= bossloc.x
        playerloc.y -= bossloc.y

        self.fire(fife.DoublePoint(playerloc.x, playerloc.y))

    def applyHit(self, hp):
        """Apply damage to the boss and change behavior at thresholds."""
        self.flash(2)
        super().applyHit(hp)

        if self.hitpoints == 20:
            self.weapon = FireBallBurst(self._scene, self, 2000, 100, 10)
            self.weapon.lastfired = self._scene.time

        elif self.hitpoints == 10:
            self.weapon = FireBallSpread(self._scene, self, 2000)
            self.weapon.lastfired = self._scene.time
