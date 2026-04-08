# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import time

from fife import fife
from fife.extensions import fifelog


def test_left_right(engine_minimized):
    soundmanager = engine_minimized.getSoundManager()
    soundmanager.init()
    sound = soundmanager.createEmitter("tests/data/left_right_test.ogg")
    sound.setLooping(True)
    sound.play()
    time.sleep(3)


def test_2_streams(engine_minimized):
    soundmanager = engine_minimized.getSoundManager()
    soundclipmanager = engine_minimized.getSoundClipManager()
    soundmanager.init()
    log = fifelog.LogManager(engine_minimized, promptlog=True, filelog=False)

    em = soundmanager.createEmitter()
    sound = soundmanager.createEmitter()
    clip = soundclipmanager.load("tests/data/left_right_test.ogg")
    sound.setSoundClip(clip)
    sound.setLooping(True)
    sound.setCursor(fife.SD_TIME_POS, 5)
    em.setSoundClip(clip)
    em.setGain(0.7)
    em.play()
    sound.play()
    time.sleep(3)
