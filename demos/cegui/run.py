#!/usr/bin/env python

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

# ------------------------------------------------------------------------------
# This is the CEGUI demo of Fifengine.
# ------------------------------------------------------------------------------

from __future__ import print_function

import os
import sys
from builtins import str

fife_path = os.path.join("..", "..", "engine", "python")
if os.path.isdir(fife_path) and fife_path not in sys.path:
    sys.path.insert(0, fife_path)

import PyCEGUI
from fife import fife
from fife.extensions.cegui.ceguibasicapplication import CEGUIApplicationBase
from fife.extensions.fife_settings import Setting

print("Using the FIFE python module found here: ", os.path.dirname(fife.__file__))

DemoSettings = Setting(app_name="CEGUI Demo")


class CEGUIDemo(CEGUIApplicationBase):
    def __init__(self, setting=None):
        super(CEGUIDemo, self).__init__(setting)

        self._loadSchemes()

        self.chat_messages = []

        root = PyCEGUI.WindowManager.getSingleton().createWindow(
            "DefaultWindow", "_MasterRoot"
        )
        PyCEGUI.System.getSingleton().getDefaultGUIContext().setRootWindow(root)

        chatWindow = PyCEGUI.WindowManager.getSingleton().loadLayoutFromFile(
            "MyConsole.layout"
        )
        root.addChild(chatWindow)
        self.chatEdit = chatWindow.getChild("EditBox")
        self.chatEdit.subscribeEvent(PyCEGUI.Editbox.EventKeyDown, self.chatKeyPressed)
        self.chatBox = chatWindow.getChild("ChatBox")
        chatSendButton = chatWindow.getChild("SendButton")
        chatSendButton.subscribeEvent(PyCEGUI.PushButton.EventClicked, self.chatSend)

        keyTestWindow = PyCEGUI.WindowManager.getSingleton().loadLayoutFromFile(
            "KeyTest.layout"
        )
        root.addChild(keyTestWindow)
        keyTestEdit = keyTestWindow.getChild("EditBox")
        keyTestEdit.subscribeEvent(PyCEGUI.Editbox.EventKeyDown, self.testKeyPressed)

    def _loadSchemes(self):
        PyCEGUI.SchemeManager.getSingleton().createFromFile("TaharezLook.scheme")

    def _pump(self):
        if self._listener.quitrequested:
            self.quit()

    def chatSend(self, args=None):
        self.chat_messages.append(PyCEGUI.ListboxTextItem(self.chatEdit.getText()))
        self.chat_messages[-1].setAutoDeleted(False)
        self.chatBox.addItem(self.chat_messages[-1])
        self.chatBox.ensureItemIsVisible(self.chat_messages[-1])
        self.chatEdit.setText("")

    def chatKeyPressed(self, args):
        if args.scancode in [args.scancode.Return, args.scancode.NumpadEnter]:
            self.chatSend()

    def testKeyPressed(self, args):
        args.window.setText(str(args.scancode))


if __name__ == "__main__":
    app = CEGUIDemo(DemoSettings)
    app.run()
