# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife import fife


def test_events(engine_minimized):
    class MyEventListener(fife.ICommandListener):
        def __init__(self):
            fife.ICommandListener.__init__(self)

        def onCommand(self, command):
            pass

    eventmanager = engine_minimized.getEventManager()
    listener = MyEventListener()
    eventmanager.addCommandListener(listener)
    cmd = fife.Command()
    cmd.setCode(0)
    for i in range(20):
        eventmanager.dispatchCommand(cmd)
        cmd.setCode(i)
    eventmanager.removeCommandListener(listener)
