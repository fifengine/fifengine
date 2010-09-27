# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2009 by the FIFE team
#  http://www.fifengine.de
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

from layout import Spacer
from containers import Container, VBox, HBox, Window
from label import Label, ClickLabel
from icon import Icon
from buttons import Button, ToggleButton, ImageButton
from checkbox import CheckBox
from radiobutton import RadioButton
from textfield import TextField
from textbox import TextBox
from listbox import ListBox
from dropdown import DropDown
from scrollarea import ScrollArea
from slider import Slider
from percentagebar import PercentageBar

# Global Widget Class registry

WIDGETS = {
	# Containers
	"Container" : Container,
	"Window" : Window,
	"VBox" : VBox,
	"HBox" : HBox,
	"ScrollArea" :ScrollArea,

	# Simple Widgets
	"Icon" : Icon,
	"Label" : Label,
	"ClickLabel" : ClickLabel,
	"PercentageBar" : PercentageBar,

	# Button Widgets
	"Button" : Button,
	"CheckBox" : CheckBox,
	"RadioButton" : RadioButton,
	"ImageButton" : ImageButton,
	"ToggleButton" : ToggleButton,

	#Complexer Widgets / Text io
	"TextField" : TextField,
	"TextBox" : TextBox,
	"ListBox" : ListBox,
	"DropDown" : DropDown,
	"Slider" : Slider
}

def registerWidget(cls):
	"""
	Register a new Widget class for pychan.
	"""
	global WIDGETS
	name = cls.__name__
	if name in WIDGETS:
		raise InitializationError("Widget class name '%s' already registered." % name)
	WIDGETS[name] = cls
