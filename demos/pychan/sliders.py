# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Demo for slider widgets."""

from fife.extensions import pychan
from pychan_demo import PyChanExample


class SliderExample(PyChanExample):
    """Demo for slider widgets."""

    def __init__(self):
        super().__init__("gui/slider.xml")

    def start(self):
        """Start the example by loading XML and setting up slider callbacks."""
        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.mapEvents(
            {
                "xslider": self.update,
                "yslider": self.update,
                "pbarslider": self.update,
                "closeButton": self.stop,
            }
        )
        self.update()
        self.widget.show()

    def update(self):
        """Update icon position from the sliders."""
        icon = self.widget.findChild(name="icon")
        # sliders have floats, fifechan is picky and wants ints
        # so we convert here.
        icon.position = list(map(int, self.widget.collectData("xslider", "yslider")))
        # we distribute to the labels with the x,y value.
        # That's user visible 'text' - so pychan wants unicode.
        self.widget.distributeInitialData(
            {
                "xvalue": str(icon.x),
                "yvalue": str(icon.y),
            }
        )

        # quick demo to show the percentage bar in action
        pbarslider = self.widget.findChild(name="pbarslider")
        pbar = self.widget.findChild(name="pbar")

        pbar.value = int(pbarslider.value)
