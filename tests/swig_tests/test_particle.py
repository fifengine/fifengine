# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
import time

from fife import fife


def test_particle_api_and_renderer_lookup(shared_engine):
    engine = shared_engine
    model = engine.getModel()
    map_obj = model.createMap("particle_py_map")
    layer = map_obj.createLayer("particle_py_layer", model.getCellGrid("square"))
    rb = engine.getRenderBackend()
    cam = map_obj.addCamera(
        "particle_py_cam", fife.Rect(0, 0, rb.getWidth(), rb.getHeight())
    )
    cam.setLocation(fife.Location(layer))

    assert fife.ParticleShape_Point != fife.ParticleShape_Box
    assert fife.ParticleShape_Circle != fife.ParticleShape_Box

    renderer = fife.ParticleRenderer.getInstance(cam)
    assert renderer is not None
    renderer.setEnabled(True)
    renderer.activateAllLayers(map_obj)

    manager = engine.getParticleManager()
    assert manager is not None
    assert fife.ParticleManager.getInstance().this == manager.this

    cfg = fife.ParticleConfig()
    cfg.emissionRate = 120.0
    cfg.lifetimeMin = 10.0
    cfg.lifetimeMax = 10.0
    cfg.maxParticles = 1024
    cfg.localSpace = False
    emitter = manager.createEmitter(cfg)
    emitter.setSeed(1234)
    emitter.attachToPoint(fife.Point(10, 10))

    engine.initializePumping()
    engine.pump()
    time.sleep(0.05)
    engine.pump()
    assert emitter.getEmittedCount() >= 1
    assert emitter.getAliveCount() >= 1
    engine.finalizePumping()
