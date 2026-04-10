# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Demo for show/hide functionality on widgets."""

from fife.extensions import pychan
from pychan_demo import PyChanExample


class ShowHideExample(PyChanExample):
    """Demo for show/hide functionality on widgets."""

    def __init__(self):
        super().__init__("gui/showhide.xml")

    def start(self):
        """Start the example by loading XML and setting up show/hide callbacks."""
        self.widget = pychan.loadXML(self.xmlFile)

        self.text1widget = self.widget.findChild(name="text1")
        self.text2widget = self.widget.findChild(name="text2")
        self.text3widget = self.widget.findChild(name="text3")
        self.vbox1widget = self.widget.findChild(name="vbox1")

        eventMap = {
            "closeButton": self.stop,
            "hideText1": self.text1widget.hide,
            "showText1": self.text1widget.show,
            "hideText2": self.text2widget.hide,
            "showText2": self.text2widget.show,
            "hideText3": self.text3widget.hide,
            "showText3": self.text3widget.show,
            "hideVBox1": self.vbox1widget.hide,
            "showVBox1": self.vbox1widget.show,
        }

        self.widget.mapEvents(eventMap)

        self.widget.show()
