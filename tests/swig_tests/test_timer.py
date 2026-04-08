# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import time

import pytest
from fife import fife


def test_events(engine_minimized):
    class MyTimeEvent(fife.TimeEvent):
        def __init__(self, period):
            fife.TimeEvent.__init__(self, period)
            self.counter = 0

        def updateEvent(self, curtime):
            self.counter += 1

    timemanager = engine_minimized.getTimeManager()
    e = MyTimeEvent(100)
    timemanager.registerEvent(e)

    for _ in range(10):
        time.sleep(0.1)
        timemanager.update()

    timemanager.unregisterEvent(e)
