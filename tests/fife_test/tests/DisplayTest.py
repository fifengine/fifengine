#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import scripts.test as test
from fife.extensions.pychan.fife_pychansettings import FifePychanSettings


class DisplayTest(test.Test):
    def create(self, engine, application):
        self._engine = engine
        self._application = application
        self._running = False

        self._settings_dialog = None
        self._display_widget = None

    def destroy(self):
        self._settings_dialog = None
        self._display_widget = None

    def run(self):
        self._running = True

        self._settings_dialog = FifePychanSettings(
            app_name="fife_test",
            settings_file="./settings.xml",
            default_settings_file="settings-dist.xml",
        )

        display_count = self._engine.getDeviceCaps().getDisplayCount()
        screen_modes = self._engine.getDeviceCaps().getSupportedScreenModes()
        displays = sorted({mode.getDisplay() for mode in screen_modes})
        if not displays:
            displays = list(range(max(1, display_count)))

        self._settings_dialog.setValidDisplays(displays)
        self._settings_dialog.showSettingsDialog()

        self._display_widget = self._settings_dialog._optionsDialog.findChildByName(
            "display"
        )
        model = self._display_widget.items
        entries = [model.getElementAt(i) for i in range(model.getNumberOfElements())]

        print("DisplayTest: display_count=", display_count)
        print("DisplayTest: detected displays=", displays)
        print("DisplayTest: dropdown entries=", entries)

    def stop(self):
        self._running = False

        if self._settings_dialog and self._settings_dialog._optionsDialog:
            self._settings_dialog._optionsDialog.hide()

    def isRunning(self):
        return self._running

    def getName(self):
        return "DisplayTest"

    def getAuthor(self):
        return "github copilot"

    def getDescription(self):
        return "Opens the settings dialog and reports display dropdown entries."

    def getHelp(self):
        return "run DisplayTest - Opens settings and prints display dropdown options."

    def onConsoleCommand(self, cmd):
        if not self._display_widget:
            return "DisplayTest: no display widget available yet."

        if cmd[0].lower() in ("entries", "displayentries"):
            model = self._display_widget.items
            entries = [model.getElementAt(i) for i in range(model.getNumberOfElements())]
            return "DisplayTest entries: " + str(entries)

        return cmd[0] + ": not found."

    def pump(self):
        pass
