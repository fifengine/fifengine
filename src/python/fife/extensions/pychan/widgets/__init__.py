# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
Widget wrappers.

Please look at the documentation of L{Widget} for details.
"""

from .adjustingcontainer import AdjustingContainer
from .animationicon import AnimationIcon
from .bargraph import BarGraph
from .buttons import Button, ImageButton, ToggleButton
from .checkbox import CheckBox
from .containers import ABox, CBox, Container, HBox, VBox, Window
from .curvegraph import CurveGraph
from .dockarea import DockArea
from .dropdown import DropDown
from .flowcontainer import FlowContainer
from .icon import Icon
from .iconprogressbar import IconProgressBar
from .imageprogressbar import ImageProgressBar
from .label import Label
from .linegraph import LineGraph
from .listbox import ListBox
from .panel import Panel
from .passwordfield import PasswordField
from .percentagebar import PercentageBar
from .piegraph import PieGraph
from .pointgraph import PointGraph
from .radiobutton import RadioButton
from .resizablewindow import ResizableWindow
from .scrollarea import ScrollArea
from .slider import Slider
from .spacer import Spacer
from .tabbedarea import Tab, TabbedArea
from .textbox import TextBox
from .textfield import TextField
from .widget import Widget as Widget

# Global Widget Class registry

WIDGETS = {
    # Containers
    "Container": Container,
    "Window": Window,
    "ABox": ABox,
    "VBox": VBox,
    "HBox": HBox,
    "CBox": CBox,
    "ScrollArea": ScrollArea,
    "AdjustingContainer": AdjustingContainer,
    "FlowContainer": FlowContainer,
    "TabbedArea": TabbedArea,
    "Tab": Tab,
    "ResizableWindow": ResizableWindow,
    "DockArea": DockArea,
    "Panel": Panel,
    # Simple Widgets
    "Spacer": Spacer,
    "Icon": Icon,
    "AnimationIcon": AnimationIcon,
    "Label": Label,
    "PercentageBar": PercentageBar,
    "IconProgressBar": IconProgressBar,
    "ImageProgressBar": ImageProgressBar,
    "BarGraph": BarGraph,
    "CurveGraph": CurveGraph,
    "LineGraph": LineGraph,
    "PieGraph": PieGraph,
    "PointGraph": PointGraph,
    # Button Widgets
    "Button": Button,
    "CheckBox": CheckBox,
    "RadioButton": RadioButton,
    "ImageButton": ImageButton,
    "ToggleButton": ToggleButton,
    # Complexer Widgets / Text io
    "TextField": TextField,
    "TextBox": TextBox,
    "PasswordField": PasswordField,
    "ListBox": ListBox,
    "DropDown": DropDown,
    "Slider": Slider,
}


def registerWidget(cls):
    """
    Register a new Widget class for pychan.

    If the widget class name is already used you will overwrite the existing
    definition.  There is no warning to tell you if this has happened.  Be sure
    to understand what you are doing here!
    """
    global WIDGETS
    name = cls.__name__

    WIDGETS[name] = cls
