# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife.extensions import pychan
from pychan_demo import PyChanExample


class ImageProgressBarExample(PyChanExample):
    def __init__(self):
        super().__init__("gui/imageprogressbar.xml")

    def start(self):
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
        print("increase")
        bar = self.widget.findChild(name="bar")
        bar.value = bar.value + 5
        print("value ", bar.value)

    def decrease(self):
        print("decrease")
        bar = self.widget.findChild(name="bar")
        bar.value = bar.value - 5
        print("value ", bar.value)
