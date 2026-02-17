#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2019 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from __future__ import absolute_import, print_function

from builtins import range

from .swig_test_utils import fife, getEngine, unittest


class MyEventListener(fife.ICommandListener):
    def __init__(self):
        fife.ICommandListener.__init__(self)

    def onCommand(self, command):
        print("received command with code %d" % command.getCode())


class TestEventChannel(unittest.TestCase):

    def setUp(self):
        self.engine = getEngine(True)
        self.eventmanager = self.engine.getEventManager()

    def tearDown(self):
        self.engine.destroy()

    def testEvents(self):
        listener = MyEventListener()
        self.eventmanager.addCommandListener(listener)
        cmd = fife.Command()
        cmd.setCode(0)
        print("Sending commands...")
        for i in range(20):
            self.eventmanager.dispatchCommand(cmd)
            cmd.setCode(i)
        self.eventmanager.removeCommandListener(listener)


TEST_CLASSES = [TestEventChannel]

if __name__ == "__main__":
    unittest.main()
