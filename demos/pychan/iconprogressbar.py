# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Demo for icon-based progress bar widgets."""

from fife.extensions import pychan
from pychan_demo import PyChanExample


class IconProgressBarExample(PyChanExample):
    """Demo for icon-based progress bar widgets."""

    def __init__(self):
        super().__init__("gui/iconprogressbar.xml")

    def start(self):
        """Start the example by loading XML and setting up callbacks."""
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
        """Advance the progress bar."""
        bar = self.widget.findChild(name="bar")
        bar.advance()

    def resetBar(self):
        """Reset the progress bar to the beginning."""
        bar = self.widget.findChild(name="bar")
        bar.reset()
