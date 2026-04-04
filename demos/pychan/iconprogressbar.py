# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife.extensions import pychan
from pychan_demo import PyChanExample


class IconProgressBarExample(PyChanExample):
    def __init__(self):
        super().__init__("gui/iconprogressbar.xml")

    def start(self):
        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.mapEvents(
            {
                "advanceButton": self.advanceBar,
                "resetButton": self.resetBar,
                "closeButton": self.stop,
            }
        )

        self.widget.show()

    def advanceBar(self):
        bar = self.widget.findChild(name="bar")
        bar.advance()

    def resetBar(self):
        bar = self.widget.findChild(name="bar")
        bar.reset()
