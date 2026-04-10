# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Demo for widget styling and theming."""

from fife import fifechan
from fife.extensions import pychan
from pychan_demo import PyChanExample

STYLES = {
    "new default": {
        "default": {
            "border_size": 2,
            "margins": (0, 0),
            "base_color": fifechan.Color(128, 128, 128),
            "foreground_color": fifechan.Color(255, 255, 255),
            "background_color": fifechan.Color(55, 55, 55),
            "font": "samanata_small",
        },
        "Button": {
            "border_size": 2,
            "margins": (20, 5),
            "min_size": (100, 20),
            "font": "samanata_small",
        },
        "CheckBox": {
            "border_size": 0,
            "background_color": fifechan.Color(0, 0, 0, 0),
        },
        "RadioButton": {
            "border_size": 0,
            "background_color": fifechan.Color(0, 0, 0, 0),
        },
        "Label": {"border_size": 0, "font": "samanata_small"},
        "ListBox": {"border_size": 0, "font": "samanata_small"},
        "Window": {
            "border_size": 1,
            "margins": (10, 10),
            "opaque": False,
            "titlebar_height": 30,
            "background_image": "gui/backgrounds/background.png",
            "font": "samanata_large",
        },
        "TextBox": {"font": "samanata_small"},
        ("Container", "HBox", "VBox"): {
            "border_size": 0,
            "background_image": "gui/backgrounds/background.png",
            "opaque": False,
        },
    },
    "greenzone": {
        "default": {
            "base_color": fifechan.Color(80, 200, 80),
            "background_color": fifechan.Color(200, 250, 200),
        },
        "Window": {
            "titlebar_height": 30,
        },
        "ListBox": {"font": "samanata_large"},
    },
}


class StylingExample(PyChanExample):
    """Demo for widget styling and theming."""

    def __init__(self):
        super().__init__("gui/styling.xml")

        self.styles = ["default"] + list(STYLES.keys())
        for name, style in list(STYLES.items()):
            pychan.manager.addStyle(name, style)

        pychan.loadFonts("fonts/samanata.xml")

    def start(self):
        """Start the example by loading XML and applying styles."""
        self.styledCredits = pychan.loadXML("gui/all_widgets.xml")
        self.styledCredits.distributeInitialData(
            {"demoList": [x for x in dir(pychan)], "demoText": pychan.__doc__}
        )

        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.mapEvents(
            {
                "testStyle": self.testStyle,
                "closeButton": self.stop,
            }
        )
        self.widget.distributeInitialData({"styleList": self.styles})
        self.widget.position_technique = "right-20:center"
        self.styledCredits.position_technique = "left+20:center"
        self.widget.show()
        self.styledCredits.show()

    def stop(self):
        """Stop the example and clean up styled credits."""
        super().stop()
        if self.styledCredits:
            self.styledCredits.hide()
            self.styledCredits = None

    def testStyle(self):
        """Apply the selected style to the styled credits widget."""
        style = self.styles[self.widget.collectData("styleList")]
        self.styledCredits.stylize(style)
        self.styledCredits.show()
