# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


def test_instances(engine_minimized):
    assert engine_minimized.getSoundManager()
    assert engine_minimized.getEventManager()
    assert engine_minimized.getTimeManager()
    assert engine_minimized.getImageManager()
    assert engine_minimized.getAnimationManager()
    assert engine_minimized.getSoundClipManager()
    assert engine_minimized.getLogManager()


def test_pumping(engine_minimized):
    engine_minimized.initializePumping()
    for _ in range(10):
        engine_minimized.pump()
    engine_minimized.finalizePumping()
