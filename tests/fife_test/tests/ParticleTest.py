#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Interactive particle demo.

Run with: cd tests/fife_test && python run.py

The engine ticks the emitters once per frame from its TimeManager; this test
never advances them from Python.
"""

import scripts.test as test
from fife import fife


class ParticleTest(test.Test):
    def create(self, engine, application):
        self._engine = engine
        self._application = application
        self._running = False
        self._map = None
        self._manager = None
        self._emitters = []

    def _make_frames(self, emitter, atlas):
        image_manager = self._engine.getImageManager()
        for i in range(2):
            frame = image_manager.create("particle_frame_%d" % i)
            frame.useSharedImage(atlas, fife.Rect(i * 8, 0, 8, 8))
            emitter.addFrame(frame)

    def _make_emitter(self, manager, atlas, position, **values):
        cfg = fife.ParticleConfig()
        cfg.maxParticles = 1024
        cfg.localSpace = False
        for key, value in values.items():
            setattr(cfg, key, value)
        emitter = manager.createEmitter(cfg)
        emitter.setSeed(1234)
        emitter.attachToPoint(fife.Point(*position))
        self._make_frames(emitter, atlas)
        self._emitters.append(emitter)
        return emitter

    def run(self):
        self._running = True
        engine = self._engine
        model = engine.getModel()

        self._map = model.createMap("particle_demo_map")
        layer = self._map.createLayer("particle_demo_layer", model.getCellGrid("square"))
        backend = engine.getRenderBackend()
        camera = self._map.addCamera(
            "particle_demo_cam", fife.Rect(0, 0, backend.getWidth(), backend.getHeight())
        )
        camera.setLocation(fife.Location(layer))

        renderer = fife.ParticleRenderer.getInstance(camera)
        renderer.setEnabled(True)
        renderer.activateAllLayers(self._map)

        atlas = engine.getImageManager().load("objects/actors/player/skel/stand.png")
        manager = fife.ParticleManager.getInstance()
        self._manager = manager

        # Rain: fast, downward, short-lived.
        self._make_emitter(
            manager,
            atlas,
            (200, 40),
            emissionRate=400.0,
            lifetimeMin=0.8,
            lifetimeMax=1.2,
            speedMin=5.0,
            speedMax=10.0,
            gravity=400.0,
            startSize=2.0,
            endSize=2.0,
            shape=fife.ParticleShape_Box,
            shapeWidth=300.0,
            shapeHeight=10.0,
        )

        # Smoke: slow, rising, growing and fading.
        self._make_emitter(
            manager,
            atlas,
            (400, 400),
            emissionRate=40.0,
            lifetimeMin=2.0,
            lifetimeMax=3.0,
            speedMin=2.0,
            speedMax=5.0,
            gravity=-30.0,
            drag=0.5,
            startSize=4.0,
            endSize=20.0,
        )

        # Sparks: bursts that arc and fade quickly.
        self._make_emitter(
            manager,
            atlas,
            (600, 400),
            emissionRate=0.0,
            burstCount=60,
            burstInterval=0.5,
            lifetimeMin=0.3,
            lifetimeMax=0.8,
            speedMin=20.0,
            speedMax=50.0,
            gravity=200.0,
            startSize=3.0,
            endSize=1.0,
            frameCount=2,
            frameDuration=0.2,
        )

    def stop(self):
        self._running = False

    def destroy(self):
        if self._manager is not None:
            for emitter in self._emitters:
                self._manager.removeEmitter(emitter)
        self._emitters.clear()
        self._manager = None
        self._map = None

    def isRunning(self):
        return self._running

    def getName(self):
        return "ParticleTest"

    def getAuthor(self):
        return "fifengine contributors"

    def getDescription(self):
        return "CPU-simulated rain, smoke and sparks drawn through Image::render()."

    def getHelp(self):
        return "run ParticleTest - shows three emitters; engine.pump() advances them."

    def pump(self):
        # Intentionally empty: the application pumps the engine, which ticks
        # the ParticleManager's TimeEvent exactly once per frame.
        pass
