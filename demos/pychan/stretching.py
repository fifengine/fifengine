# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife.extensions import pychan
from pychan_demo import PyChanExample


class StretchingExample(PyChanExample):
    def __init__(self):
        super().__init__("gui/stretching.xml")

    def start(self):
        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.mapEvents(
            {
                "wslider": self.update,
                "hslider": self.update,
                "closeButton": self.stop,
            }
        )
        self.update()
        self.widget.show()

    def update(self):
        """
        Update Icon position from the sliders.
        """
        icon = self.widget.findChild(name="icon")
        # sliders have floats, fifechan is picky and wants ints
        # so we convert here.
        icon.size = list(map(int, self.widget.collectData("wslider", "hslider")))
        # we distribute to the labels with the x,y value.
        # That's user visible 'text' - so pychan wants unicode.
        self.widget.distributeInitialData(
            {
                "wvalue": str(icon.width),
                "hvalue": str(icon.height),
            }
        )
