# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Demo for image-based progress bar widgets."""

from fife.extensions import pychan
from pychan_demo import PyChanExample


class ImageProgressBarExample(PyChanExample):
    """Demo for image-based progress bar widgets."""

    def __init__(self):
        super().__init__("gui/imageprogressbar.xml")

    def start(self):
        """Start the example by loading XML and setting up callbacks."""
        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.mapEvents(
            {
                "increaseButton": self.increase,
                "decreaseButton": self.decrease,
                "closeButton": self.stop,
            }
        )

        self.widget.show()

    def increase(self):
        """Increase the progress bar value by 5."""
        print("increase")
        bar = self.widget.findChild(name="bar")
        bar.value = bar.value + 5
        print("value ", bar.value)

    def decrease(self):
        """Decrease the progress bar value by 5."""
        print("decrease")
        bar = self.widget.findChild(name="bar")
        bar.value = bar.value - 5
        print("value ", bar.value)
