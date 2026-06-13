# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Visual regression/inspection test for widget border rendering.

Displays bordered boxes side by side with different border sizes,
colors, and widget types so you can visually verify correctness.
"""

from fife.extensions import pychan
from run import PyChanExample

from fife import fifechan

STYLES = {
    "border_0": {
        "default": {
            "border_size": 0,
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_1": {
        "default": {
            "border_size": 1,
            "border_color": fifechan.Color(80, 80, 100),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_2": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(80, 80, 100),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_3": {
        "default": {
            "border_size": 3,
            "border_color": fifechan.Color(80, 80, 100),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_4": {
        "default": {
            "border_size": 4,
            "border_color": fifechan.Color(80, 80, 100),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_white": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(255, 255, 255),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(180, 180, 200),
        },
    },
    "border_dark": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(15, 15, 25),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_red": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(220, 60, 60),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_green": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(60, 200, 60),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_blue": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(60, 100, 230),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "border_yellow": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(220, 200, 40),
            "base_color": fifechan.Color(60, 60, 80),
            "background_color": fifechan.Color(45, 45, 58),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
    },
    "button_test": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(100, 140, 200),
            "base_color": fifechan.Color(50, 55, 72),
            "foreground_color": fifechan.Color(210, 210, 220),
            "background_color": fifechan.Color(45, 45, 58),
        },
    },
    "textbox_test": {
        "default": {
            "border_size": 2,
            "border_color": fifechan.Color(100, 140, 200),
            "base_color": fifechan.Color(50, 55, 72),
            "foreground_color": fifechan.Color(210, 210, 220),
            "background_color": fifechan.Color(45, 45, 58),
        },
    },
    "tabbed_test": {
        "default": {
            "border_size": 1,
            "border_color": fifechan.Color(80, 100, 140),
            "base_color": fifechan.Color(50, 60, 80),
            "background_color": fifechan.Color(40, 48, 65),
            "foreground_color": fifechan.Color(210, 210, 220),
        },
        "Label": {
            "border_size": 0,
        },
    },
}


class BorderTypesExample(PyChanExample):
    """Visual regression test for widget border rendering."""

    def __init__(self):
        super().__init__("gui/bordertypes.xml")

        for name, style in list(STYLES.items()):
            pychan.manager.addStyle(name, style)

    def start(self):
        """Start the border types demo."""
        self.widget = pychan.loadXML(self.xmlFile)
        self.widget.mapEvents(
            {
                "closeButton": self.stop,
            }
        )
        self.widget.show()
