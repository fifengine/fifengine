# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Demo for modal focus handling in widgets."""

from fife.extensions import pychan
from run import PyChanExample


class ModalFocusExample(PyChanExample):
    """Demo for modal focus handling in widgets."""

    def __init__(self):
        super().__init__("gui/modalfocus.xml")

    def start(self):
        """Start the example by loading XML and setting up modal focus."""
        self.widget = pychan.loadXML(self.xmlFile)

        self.window = self.widget
        self.hbox1 = self.widget.findChild(name="hbox1")
        self.hbox2 = self.widget.findChild(name="hbox2")
        self.widgets = [self.widget, self.hbox1, self.hbox2]

        eventMap = {
            "closeButton": self.stop,
            "text1": self.requestModalWindow,
            "text2": self.releaseModalWindow,
            "text3": self.requestMouseModalWindow,
            "text4": self.releaseMouseModalWindow,
            "text5": self.requestModalHBox1,
            "text6": self.releaseModalHBox1,
            "text7": self.requestMouseModalHBox1,
            "text8": self.releaseMouseModalHBox1,
            "text9": self.requestModalHBox2,
            "text10": self.releaseModalHBox2,
            "text11": self.requestMouseModalHBox2,
            "text12": self.releaseMouseModalHBox2,
        }
        self.widget.mapEvents(eventMap)

        self.widget.show()
        self.updateColor()

    def updateColor(self):
        """Update widget colors based on their modal focus state."""
        for widget in self.widgets:
            if widget.isModalFocused():
                if widget.isUnderMouseModal():
                    widget.base_color = (0, 128, 128)
                else:
                    widget.base_color = (0, 128, 0)
            else:
                if widget.isUnderMouseModal():
                    widget.base_color = (128, 0, 128)
                else:
                    widget.base_color = (128, 0, 0)

    def _pushModal(self, widget):
        """Push modal focus for the given widget."""
        widget._focusHandler().pushModal(widget.real_widget)
        self.updateColor()

    def _popModal(self, widget):
        """Pop modal focus for the given widget."""
        widget._focusHandler().popModal()
        self.updateColor()

    def _pushMouseModal(self, widget):
        """Push modal mouse input focus for the given widget."""
        widget._focusHandler().pushModal(None, widget.real_widget)
        self.updateColor()

    def _popMouseModal(self, widget):
        """Pop modal mouse input focus for the given widget."""
        widget._focusHandler().popModal()
        self.updateColor()

    def requestModalWindow(self):
        """Request modal focus for the window."""
        self._pushModal(self.window)

    def releaseModalWindow(self):
        """Release modal focus for the window."""
        self._popModal(self.window)

    def requestMouseModalWindow(self):
        """Request modal mouse input focus for the window."""
        self._pushMouseModal(self.window)

    def releaseMouseModalWindow(self):
        """Release modal mouse input focus for the window."""
        self._popMouseModal(self.window)

    def requestModalHBox1(self):
        """Request modal focus for hbox1."""
        self._pushModal(self.hbox1)

    def releaseModalHBox1(self):
        """Release modal focus for hbox1."""
        self._popModal(self.hbox1)

    def requestMouseModalHBox1(self):
        """Request modal mouse input focus for hbox1."""
        self._pushMouseModal(self.hbox1)

    def releaseMouseModalHBox1(self):
        """Release modal mouse input focus for hbox1."""
        self._popMouseModal(self.hbox1)

    def requestModalHBox2(self):
        """Request modal focus for hbox2."""
        self._pushModal(self.hbox2)

    def releaseModalHBox2(self):
        """Release modal focus for hbox2."""
        self._popModal(self.hbox2)

    def requestMouseModalHBox2(self):
        """Request modal mouse input focus for hbox2."""
        self._pushMouseModal(self.hbox2)

    def releaseMouseModalHBox2(self):
        """Release modal mouse input focus for hbox2."""
        self._popMouseModal(self.hbox2)
