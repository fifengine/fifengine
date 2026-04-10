# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Demo for dynamic widget creation and management."""

from fife.extensions import pychan
from pychan_demo import PyChanExample


class DynamicExample(PyChanExample):
    """Demo for dynamic widget creation and management."""

    def __init__(self):
        super().__init__("gui/dynamic.xml")

    def start(self):
        """Start the example by loading XML and setting up callbacks."""
        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.mapEvents(
            {
                "okButton": self.stop,
                "addButton": self.addLabel,
            }
        )
        self.labelBox = self.widget.findChild(name="labelBox")
        self.widget.show()

    def addLabel(self):
        """Add a new label widget to the label box."""
        # Code to check text wrapping code in FIFE
        label = pychan.widgets.Label(max_size=(200, 1000), wrap_text=True)
        label.text = self.widget.collectData("labelText")
        label.capture(self.removeLabel)
        self.labelBox.addChild(label)
        self.widget.adaptLayout()

    def removeLabel(self, widget=None):
        """Remove a label widget from the label box."""
        widget.parent.removeChild(widget)
        self.widget.adaptLayout()
