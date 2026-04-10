# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Pychan demo app for testing RGBA colors on widgets."""

from fife.extensions import pychan
from pychan_demo import PyChanExample


class ColorExample(PyChanExample):
    """Small app to demonstrate how FIFE uses colors on various widgets."""

    def __init__(self):
        super().__init__("gui/colortester.xml")

    def start(self):
        """Load the XML file and set up callbacks."""
        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.mapEvents(
            {
                "base_rslider": self.update_basecolor,
                "base_gslider": self.update_basecolor,
                "base_bslider": self.update_basecolor,
                "base_aslider": self.update_basecolor,
                "background_rslider": self.update_background_color,
                "background_gslider": self.update_background_color,
                "background_bslider": self.update_background_color,
                "background_aslider": self.update_background_color,
                "foreground_rslider": self.update_foreground_color,
                "foreground_gslider": self.update_foreground_color,
                "foreground_bslider": self.update_foreground_color,
                "foreground_aslider": self.update_foreground_color,
                "selection_rslider": self.update_selection_color,
                "selection_gslider": self.update_selection_color,
                "selection_bslider": self.update_selection_color,
                "selection_aslider": self.update_selection_color,
                "closeButton": self.stop,
            }
        )
        # alpha value needs to be set, otherwise you don't see colors ;-)
        self.widget.findChild(name="base_aslider").value = float(255)
        self.widget.findChild(name="background_aslider").value = float(255)
        self.widget.findChild(name="foreground_aslider").value = float(255)
        self.widget.findChild(name="selection_aslider").value = float(255)

        # init stuff
        self.update_basecolor()
        self.update_background_color()
        self.update_foreground_color()
        self.update_selection_color()
        self.widget.show()

    def update_basecolor(self):
        """Update RGBA base colors of all examples and show the values."""
        r = int(self.widget.findChild(name="base_rslider").value)
        g = int(self.widget.findChild(name="base_gslider").value)
        b = int(self.widget.findChild(name="base_bslider").value)
        a = int(self.widget.findChild(name="base_aslider").value)

        # update slider labels
        self.widget.findChild(name="base_rvalue").text = str(r)
        self.widget.findChild(name="base_gvalue").text = str(g)
        self.widget.findChild(name="base_bvalue").text = str(b)
        self.widget.findChild(name="base_avalue").text = str(a)

        rgba = (r, g, b, a)

        self.widget.findChild(name="example1").base_color = rgba
        self.widget.findChild(name="example2").base_color = rgba
        self.widget.findChild(name="example3").base_color = rgba
        self.widget.findChild(name="example4").base_color = rgba
        self.widget.findChild(name="example5").base_color = rgba
        self.widget.findChild(name="example6").base_color = rgba
        self.widget.findChild(name="example7").base_color = rgba
        self.widget.findChild(name="example8").base_color = rgba
        self.widget.findChild(name="example9").base_color = rgba

    def update_background_color(self):
        """Update RGBA background colors of all examples and show the values."""
        r = int(self.widget.findChild(name="background_rslider").value)
        g = int(self.widget.findChild(name="background_gslider").value)
        b = int(self.widget.findChild(name="background_bslider").value)
        a = int(self.widget.findChild(name="background_aslider").value)

        # update slider labels
        self.widget.findChild(name="background_rvalue").text = str(r)
        self.widget.findChild(name="background_gvalue").text = str(g)
        self.widget.findChild(name="background_bvalue").text = str(b)
        self.widget.findChild(name="background_avalue").text = str(a)

        rgba = (r, g, b, a)

        self.widget.findChild(name="example1").background_color = rgba
        self.widget.findChild(name="example2").background_color = rgba
        self.widget.findChild(name="example3").background_color = rgba
        self.widget.findChild(name="example4").background_color = rgba
        self.widget.findChild(name="example5").background_color = rgba
        self.widget.findChild(name="example6").background_color = rgba
        self.widget.findChild(name="example7").background_color = rgba
        self.widget.findChild(name="example8").background_color = rgba
        self.widget.findChild(name="example9").background_color = rgba

    def update_selection_color(self):
        """Update RGBA selection colors of all examples and show the values."""
        r = int(self.widget.findChild(name="selection_rslider").value)
        g = int(self.widget.findChild(name="selection_gslider").value)
        b = int(self.widget.findChild(name="selection_bslider").value)
        a = int(self.widget.findChild(name="selection_aslider").value)

        # update slider labels
        self.widget.findChild(name="selection_rvalue").text = str(r)
        self.widget.findChild(name="selection_gvalue").text = str(g)
        self.widget.findChild(name="selection_bvalue").text = str(b)
        self.widget.findChild(name="selection_avalue").text = str(a)

        rgba = (r, g, b, a)

        self.widget.findChild(name="example1").selection_color = rgba
        self.widget.findChild(name="example2").selection_color = rgba
        self.widget.findChild(name="example3").selection_color = rgba
        self.widget.findChild(name="example4").selection_color = rgba
        self.widget.findChild(name="example5").selection_color = rgba
        self.widget.findChild(name="example6").selection_color = rgba
        self.widget.findChild(name="example7").selection_color = rgba
        self.widget.findChild(name="example8").selection_color = rgba
        self.widget.findChild(name="example9").selection_color = rgba

    def update_foreground_color(self):
        """Update RGBA foreground colors of all examples and show the values."""
        r = int(self.widget.findChild(name="foreground_rslider").value)
        g = int(self.widget.findChild(name="foreground_gslider").value)
        b = int(self.widget.findChild(name="foreground_bslider").value)
        a = int(self.widget.findChild(name="foreground_aslider").value)

        # update slider labels
        self.widget.findChild(name="foreground_rvalue").text = str(r)
        self.widget.findChild(name="foreground_gvalue").text = str(g)
        self.widget.findChild(name="foreground_bvalue").text = str(b)
        self.widget.findChild(name="foreground_avalue").text = str(a)

        rgba = (r, g, b, a)

        self.widget.findChild(name="example1").foreground_color = rgba
        self.widget.findChild(name="example2").foreground_color = rgba
        self.widget.findChild(name="example3").foreground_color = rgba
        self.widget.findChild(name="example4").foreground_color = rgba
        self.widget.findChild(name="example5").foreground_color = rgba
        self.widget.findChild(name="example6").foreground_color = rgba
        self.widget.findChild(name="example7").foreground_color = rgba
        self.widget.findChild(name="example8").foreground_color = rgba
        self.widget.findChild(name="example9").foreground_color = rgba
