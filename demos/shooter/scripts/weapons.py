#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Projectile and weapon implementations for the shooter demo."""

from fife import fife

from scripts.common.baseobject import SHTR_PROJECTILE, SpaceObject


class Projectile(SpaceObject):
    """
    Represents a projectile (or bullet or fireball) in the scene.

    This is the entity that weapons fire.  Projectiles have an owner
    and a time to live.  They are also what cause damage to ships
    and other entities.
    """

    def __init__(self, scene, owner, projectileName, timeToLive):
        """Create a projectile instance.

        Parameters
        ----------
        scene : Scene
            Scene the projectile belongs to.
        owner : Ship
            The ship that fired this projectile.
        projectileName : str
            Name of the FIFE projectile object.
        timeToLive : int
            Time to live in milliseconds.
        """
        super().__init__(scene, projectileName, False)

        self._obj = self._model.getObject(
            self._name, "http://www.fifengine.net/xml/tutorial"
        )

        self._type = SHTR_PROJECTILE

        self._ttl = timeToLive
        self._starttime = 0
        self._totaltime = 0

        self._owner = owner

        self.width = 0.025
        self.height = 0.025

        self._damage = 1

    def create(self, location):
        """Spawn the projectile at `location`.

        Parameters
        ----------
        location : fife.Location
            Location at which to spawn the projectile.
        """
        self._instance = self._layer.createInstance(
            self._obj, location.getExactLayerCoordinates(), "bullet"
        )
        fife.InstanceVisual.create(self._instance)
        self._instance.thisown = 0

    def run(self, velocity, location):
        """Start the projectile moving from `location` with `velocity`.

        Parameters
        ----------
        velocity : fife.DoublePoint
            Starting velocity vector.
        location : fife.Location
            Starting location for the projectile.
        """
        if not self._running:
            self._velocity = velocity
            self._velocity.x /= self._xscale
            self._velocity.y /= self._yscale

            self.create(location)
            self._running = True

            self._starttime = self._scene.time

    def _getTTL(self):
        return self._ttl

    def _getOwner(self):
        return self._owner

    def update(self):
        """Advance projectile lifetime and destroy when expired."""
        self._totaltime += self._scene.timedelta
        if self._running and self._totaltime < self._ttl:
            super().update()
        else:
            self.destroy()

    def _getDamage(self):
        return self._damage

    def _setDamage(self, dam):
        self._damage = dam

    ttl = property(_getTTL)
    owner = property(_getOwner)
    damage = property(_getDamage, _setDamage)


class Weapon:
    """Base weapon class; subclass and implement `fire`.

    Subclasses should implement `fire(direction)` to spawn and launch
    `Projectile` instances.
    """

    def __init__(self, scene, ship, firerate):
        self._scene = scene
        self._model = self._scene.model
        self._layer = self._scene.objectlayer
        self._ship = ship
        self._firerate = firerate
        self._lastfired = 0
        self._projectileVelocity = 0.75
        self._soundclip = None

    def fire(self, direction):
        """Fire the weapon in `direction`.

        Parameters
        ----------
        direction : fife.DoublePoint
            Normalized direction vector to fire towards.
        """
        pass

    def _getProjectileVelocity(self):
        return self._projectileVelocity

    def _setProjectileVelocity(self, vel):
        self._projectileVelocity = vel

    def _getLastFired(self):
        return self._lastfired

    def _setLastFired(self, time):
        self._lastfired = time

    projectilevelocity = property(_getProjectileVelocity, _setProjectileVelocity)
    lastfired = property(_getLastFired, _setLastFired)


class Cannon(Weapon):
    """Cannon weapon that fires fast projectiles."""

    def __init__(self, scene, ship, firerate):
        super().__init__(scene, ship, firerate)

        self._projectileVelocity = 0.75
        self._soundclip = scene.soundmanager.createEmitter("sounds/cannon.ogg")
        scene.soundmanager.addEmitterToSoundEffect(scene._soundeffect, self._soundclip)

    def fire(self, direction):
        """Fire a cannon projectile in `direction`."""
        velocity = fife.DoublePoint(direction)
        velocity.normalize()
        velocity.x = velocity.x * self._projectileVelocity
        velocity.y = velocity.y * self._projectileVelocity

        if (self._scene.time - self._lastfired) > self._firerate:
            pjctl = Projectile(self._scene, self._ship, "bullet1", 3000)
            pjctl.run(velocity, self._ship.location)
            self._lastfired = self._scene.time
            self._scene.addObjectToScene(pjctl)
            if self._soundclip:
                location = self._ship.location.getExactLayerCoordinates()
                self._soundclip.setPosition(location)
                self._soundclip.play()


class FireBall(Weapon):
    """Fireball weapon that launches slower, damaging projectiles."""

    def __init__(self, scene, ship, firerate):
        super().__init__(scene, ship, firerate)

        self._projectileVelocity = 0.50
        self._soundclip = scene.soundmanager.createEmitter("sounds/fireball.ogg")
        scene.soundmanager.addEmitterToSoundEffect(scene._soundeffect, self._soundclip)

    def fire(self, direction):
        """Fire a fireball projectile in `direction`."""
        velocity = fife.DoublePoint(direction)
        velocity.normalize()
        velocity.x = velocity.x * self._projectileVelocity
        velocity.y = velocity.y * self._projectileVelocity

        if (self._scene.time - self._lastfired) > self._firerate:
            pjctl = Projectile(self._scene, self._ship, "fireball", 6000)
            pjctl.run(velocity, self._ship.location)
            self._lastfired = self._scene.time
            self._scene.addObjectToScene(pjctl)
            if self._soundclip:
                location = self._ship.location.getExactLayerCoordinates()
                self._soundclip.setPosition(location)
                self._soundclip.play()


class FireBallBurst(Weapon):
    """Burst fire weapon that fires multiple fireballs in quick succession."""

    def __init__(self, scene, ship, firerate, burstrate, burstnumber):
        super().__init__(scene, ship, firerate)

        self._projectileVelocity = 0.50
        self._soundclip = scene.soundmanager.createEmitter("sounds/fireball.ogg")
        scene.soundmanager.addEmitterToSoundEffect(scene._soundeffect, self._soundclip)

        self._burstrate = burstrate
        self._burstnumber = int(burstnumber)
        self._burstcount = int(burstnumber)

        self._lastburstfired = 0

    def fire(self, direction):
        """Fire a burst of fireballs in `direction`."""
        velocity = fife.DoublePoint(direction)
        velocity.normalize()
        velocity.x = velocity.x * self._projectileVelocity
        velocity.y = velocity.y * self._projectileVelocity

        if (self._scene.time - self._lastfired) > self._firerate:
            if (
                self._scene.time - self._lastburstfired
            ) > self._burstrate and self._burstcount > 0:
                pjctl = Projectile(self._scene, self._ship, "fireball", 6000)
                pjctl.run(velocity, self._ship.location)
                self._scene.addObjectToScene(pjctl)

                if self._soundclip:
                    location = self._ship.location.getExactLayerCoordinates()
                    self._soundclip.setPosition(location)
                    self._soundclip.play()

                self._lastburstfired = self._scene.time
                self._burstcount -= 1

            if self._burstcount <= 0:
                self._lastfired = self._scene.time
                self._burstcount = int(self._burstnumber)
                self._lastburstfired = 0


class FireBallSpread(Weapon):
    """Spread weapon that fires multiple fireballs in a spread pattern."""

    def __init__(self, scene, ship, firerate):
        super().__init__(scene, ship, firerate)

        self._projectileVelocity = 0.50
        self._soundclip = scene.soundmanager.createEmitter("sounds/fireball.ogg")
        scene.soundmanager.addEmitterToSoundEffect(scene._soundeffect, self._soundclip)

    def fire(self, direction):
        """Fire multiple projectiles in a spread around `direction`."""
        if (self._scene.time - self._lastfired) > self._firerate:
            velocity = fife.DoublePoint(direction)
            velocity.normalize()
            velocity.x = velocity.x * self._projectileVelocity
            velocity.y = velocity.y * self._projectileVelocity

            origin = fife.DoublePoint(0, 0)

            p1 = fife.DoublePoint(velocity)
            p2 = fife.DoublePoint(velocity)
            p3 = fife.DoublePoint(velocity)
            p4 = fife.DoublePoint(velocity)
            p5 = fife.DoublePoint(velocity)
            p6 = fife.DoublePoint(velocity)
            p7 = fife.DoublePoint(velocity)

            p1.rotate(origin, -30)
            p2.rotate(origin, -20)
            p3.rotate(origin, -10)
            p4.rotate(origin, 0)
            p5.rotate(origin, 10)
            p6.rotate(origin, 20)
            p7.rotate(origin, 30)

            pjctl1 = Projectile(self._scene, self._ship, "fireball", 6000)
            pjctl1.run(p1, self._ship.location)
            self._scene.addObjectToScene(pjctl1)

            pjctl2 = Projectile(self._scene, self._ship, "fireball", 6000)
            pjctl2.run(p2, self._ship.location)
            self._scene.addObjectToScene(pjctl2)

            pjctl3 = Projectile(self._scene, self._ship, "fireball", 6000)
            pjctl3.run(p3, self._ship.location)
            self._scene.addObjectToScene(pjctl3)

            pjctl4 = Projectile(self._scene, self._ship, "fireball", 6000)
            pjctl4.run(p4, self._ship.location)
            self._scene.addObjectToScene(pjctl4)

            pjctl5 = Projectile(self._scene, self._ship, "fireball", 6000)
            pjctl5.run(p5, self._ship.location)
            self._scene.addObjectToScene(pjctl5)

            pjctl6 = Projectile(self._scene, self._ship, "fireball", 6000)
            pjctl6.run(p6, self._ship.location)
            self._scene.addObjectToScene(pjctl6)

            pjctl7 = Projectile(self._scene, self._ship, "fireball", 6000)
            pjctl7.run(p7, self._ship.location)
            self._scene.addObjectToScene(pjctl7)

            if self._soundclip:
                location = self._ship.location.getExactLayerCoordinates()
                self._soundclip.setPosition(location)
                self._soundclip.play()

            self._lastfired = self._scene.time


class CannonSpread5(Weapon):
    """Cannon variant that fires a five-projectile spread."""

    def __init__(self, scene, ship, firerate):
        super().__init__(scene, ship, firerate)

        self._projectileVelocity = 1
        self._soundclip = scene.soundmanager.createEmitter("sounds/cannon.ogg")
        scene.soundmanager.addEmitterToSoundEffect(scene._soundeffect, self._soundclip)

    def fire(self, direction):
        """Fire a 5-projectile cannon spread in `direction`."""
        if (self._scene.time - self._lastfired) > self._firerate:
            velocity = fife.DoublePoint(direction)
            velocity.normalize()
            velocity.x = velocity.x * self._projectileVelocity
            velocity.y = velocity.y * self._projectileVelocity

            origin = fife.DoublePoint(0, 0)

            p2 = fife.DoublePoint(velocity)
            p3 = fife.DoublePoint(velocity)
            p4 = fife.DoublePoint(velocity)
            p5 = fife.DoublePoint(velocity)
            p6 = fife.DoublePoint(velocity)

            p2.rotate(origin, -10)
            p3.rotate(origin, -5)
            p4.rotate(origin, 0)
            p5.rotate(origin, 5)
            p6.rotate(origin, 10)

            pjctl2 = Projectile(self._scene, self._ship, "bullet1", 3000)
            pjctl2.run(p2, self._ship.location)
            self._scene.addObjectToScene(pjctl2)

            pjctl3 = Projectile(self._scene, self._ship, "bullet1", 3000)
            pjctl3.run(p3, self._ship.location)
            self._scene.addObjectToScene(pjctl3)

            pjctl4 = Projectile(self._scene, self._ship, "bullet1", 3000)
            pjctl4.run(p4, self._ship.location)
            self._scene.addObjectToScene(pjctl4)

            pjctl5 = Projectile(self._scene, self._ship, "bullet1", 3000)
            pjctl5.run(p5, self._ship.location)
            self._scene.addObjectToScene(pjctl5)

            pjctl6 = Projectile(self._scene, self._ship, "bullet1", 3000)
            pjctl6.run(p6, self._ship.location)
            self._scene.addObjectToScene(pjctl6)

            if self._soundclip:
                location = self._ship.location.getExactLayerCoordinates()
                self._soundclip.setPosition(location)
                self._soundclip.play()

            self._lastfired = self._scene.time
