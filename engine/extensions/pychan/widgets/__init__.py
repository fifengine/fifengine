# -*- coding: utf-8 -*-
### Widget/Container Base Classes ###

"""
Widget wrappers.

Please look at the documentation of L{Widget} for details.
"""

from widget import Widget

from containers import Container, VBox, HBox, Window, Spacer
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
