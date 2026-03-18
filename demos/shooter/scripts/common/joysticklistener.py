#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife


class JoystickListener(fife.IJoystickListener):
    def __init__(self, world):
        self.world = world
        self.eventmanager = world._engine.getEventManager()
        fife.IJoystickListener.__init__(self)
        self.eventmanager.addJoystickListener(self)

    def axisMotion(self, evt):
        axisX = 0
        axisY = 1
        if evt.isController:
            axisX = fife.Joystick.CONTOLLER_AXIS_LEFTX
            axisY = fife.Joystick.CONTOLLER_AXIS_LEFTY
        if evt.getAxis() == axisX:
            if evt.getAxisValue() < 0:
                self.world._keystate["LEFT"] = True
            elif evt.getAxisValue() > 0:
                self.world._keystate["RIGHT"] = True
            else:
                self.world._keystate["LEFT"] = False
                self.world._keystate["RIGHT"] = False
        elif evt.getAxis() == axisY:
            if evt.getAxisValue() < 0:
                self.world._keystate["UP"] = True
            elif evt.getAxisValue() > 0:
                self.world._keystate["DOWN"] = True
            else:
                self.world._keystate["UP"] = False
                self.world._keystate["DOWN"] = False

    def hatMotion(self, evt):
        pass

    def buttonPressed(self, evt):
        button = 0
        if evt.isController():
            button = fife.Joystick.CONTOLLER_BUTTON_A
        if evt.getButton() == button:
            self.world._keystate["SPACE"] = True

    def buttonReleased(self, evt):
        button = 0
        if evt.isController():
            button = fife.Joystick.CONTOLLER_BUTTON_A
        if evt.getButton() == button:
            self.world._keystate["SPACE"] = False

    def deviceAdded(self, evt):
        pass

    def deviceRemoved(self, evt):
        pass
