# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

### Widget/Container Base Classes ###

"""
Widget wrappers.

Please look at the documentation of L{Widget} for details.
"""

from widget import Widget

from spacer import Spacer
from adjustingcontainer import AdjustingContainer
from containers import Container, VBox, HBox, CBox, Window
from flowcontainer import FlowContainer
from label import Label
from icon import Icon
from animationicon import AnimationIcon
from buttons import Button, ToggleButton, ImageButton
from checkbox import CheckBox
from radiobutton import RadioButton
from tabbedarea import TabbedArea, Tab
from textfield import TextField
from textbox import TextBox
from listbox import ListBox
from dropdown import DropDown
from scrollarea import ScrollArea
from slider import Slider
from percentagebar import PercentageBar
from passwordfield import PasswordField
from iconprogressbar import IconProgressBar
from imageprogressbar import ImageProgressBar
from resizablewindow import ResizableWindow
from dockarea import DockArea
from panel import Panel
from bargraph import BarGraph
from curvegraph import CurveGraph
from linegraph import LineGraph
from piegraph import PieGraph
from pointgraph import PointGraph


# Global Widget Class registry

WIDGETS = {
	# Containers
	"Container" : Container,
	"Window" : Window,
	"VBox" : VBox,
	"HBox" : HBox,
	"CBox" : CBox,
	"ScrollArea" :ScrollArea,
	"AdjustingContainer" : AdjustingContainer,
	"FlowContainer" : FlowContainer,
	"TabbedArea" : TabbedArea,
	"Tab" : Tab,
	"ResizableWindow" : ResizableWindow,
	"DockArea" : DockArea,
	"Panel" : Panel,

	# Simple Widgets
	"Spacer" : Spacer,
	"Icon" : Icon,
	"AnimationIcon" : AnimationIcon,
	"Label" : Label,
	"PercentageBar" : PercentageBar,
	"IconProgressBar" : IconProgressBar,
	"ImageProgressBar" : ImageProgressBar,
	"BarGraph" : BarGraph,
	"CurveGraph" : CurveGraph,
	"LineGraph" : LineGraph,
	"PieGraph" : PieGraph,
	"PointGraph" : PointGraph,

	# Button Widgets
	"Button" : Button,
	"CheckBox" : CheckBox,
	"RadioButton" : RadioButton,
	"ImageButton" : ImageButton,
	"ToggleButton" : ToggleButton,

	#Complexer Widgets / Text io
	"TextField" : TextField,
	"TextBox" : TextBox,
	"PasswordField" : PasswordField,
	"ListBox" : ListBox,
	"DropDown" : DropDown,
	"Slider" : Slider
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
