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

from fife.extensions import pychan
from fife.extensions.pychan import loadXML
import fife.extensions.pychan.tools as tools
import widgets
from internal import get_manager, screen_width, screen_height
from StringIO import StringIO

OK,YES,NO,CANCEL = True,True,False,None

def print_event(**kwargs):
	print kwargs

class XMLDialog(object):
	def __init__(self, xml, ok_field = None, cancel_field = None,initial_data={},data={}):
		self.gui = loadXML(xml)
		self.ok_field = ok_field
		self.cancel_field = cancel_field
		self.initial_data= initial_data
		self.data= data
		self.max_size=None
		self.min_size=None
#		self.gui.capture(print_event,"mouseEntered")

	def execute(self):
		self.gui.distributeInitialData(self.initial_data)
		self.gui.distributeData(self.data)

		screen_w, screen_h = screen_width(), screen_height()
		if self.max_size is None:
			self.max_size = screen_w // 2, screen_h // 3
		if self.min_size is None:
			self.min_size = screen_w // 2, screen_h // 4
		self.gui.max_size = self.max_size
		self.gui.min_size = self.min_size

		resultMap = {}
		if self.gui.findChild(name="okButton"):
			resultMap["okButton"] = OK

		if self.gui.findChild(name="cancelButton"):
			resultMap["cancelButton"] = CANCEL

		if self.gui.findChild(name="yesButton"):
			resultMap["noButton"] = NO

		if self.gui.findChild(name="yesButton"):
			resultMap["yesButton"] = YES

		ok = self.gui.execute(resultMap)
		if ok:
			return self.getOkResult()
		return self.getCancelResult()

	def getOkResult(self):
		if self.ok_field:
			return self.gui.collectData(self.ok_field)
		return True

	def getCancelResult(self):
		if self.cancel_field:
			return self.gui.collectData(self.cancel_field)
		return False

MESSAGE_BOX_XML = """\
<Window name="window" title="Message">
<ScrollArea>
<Label wrap_text="1" text="$MESSAGE" name="message" vexpand="True"/>
</ScrollArea>
<HBox>
<Spacer/><Button min_size="50,0" name="okButton" text="OK"/>
</HBox>
</Window>
"""

YESNO_BOX_XML = """\
<Window name="window" title="Question">
<ScrollArea>
<Label wrap_text="1" text="$MESSAGE" name="message" vexpand="True"/>
</ScrollArea>
<HBox>
<Spacer/>
<Button min_size="50,0" name="yesButton" text="Yes"/>
<Button min_size="50,0" name="noButton" text="No"/>
</HBox>
</Window>
"""

YESNOCANCEL_BOX_XML = """\
<Window name="window" title="Question">
<ScrollArea>
<Label wrap_text="1" text="$MESSAGE" name="message" vexpand="True"/>
</ScrollArea>
<HBox>
<Spacer/>
<Button min_width="50" name="yesButton" text="Yes"/>
<Button min_width="50" name="noButton" text="No"/>
<Button min_width="50" name="cancelButton" text="Cancel"/>
</HBox>
</Window>
"""

SELECT_BOX_XML = """\
<Window name="window" title="Select">
<Label wrap_text="1" text="$MESSAGE" name="message"/>
<ScrollArea>
<ListBox name="selection">
</ListBox>
</ScrollArea>
<HBox>
<Spacer/>
<Button min_width="50" name="okButton" text="Select"/>
<Button min_width="50" name="cancelButton" text="Cancel"/>
</HBox>
</Window>
"""

EXCEPTION_CATCHER_XML="""\
<Window name="window" title="An exception occurred - what now?">
  <VBox hexpand="True">
    <Label wrap_text="1" max_size="400,90000" text="$MESSAGE" name="message"/>
    <ScrollArea>
    <Label text="$MESSAGE" name="traceback"/>
    </ScrollArea>
  </VBox>
  <HBox>
    <Spacer/>
    <Button name="yesButton" text="Retry"/>
    <Button name="noButton" text="Ignore"/>
    <Button name="cancelButton" text="Reraise"/>
  </HBox>
</Window>
"""

def _make_text(message):
	if callable(message):
		message = message()
	if hasattr(message,"read"):
		message = message.read()
	return message

def message(message="",caption="Message"):
	text = _make_text(message)
	dialog = XMLDialog(StringIO(MESSAGE_BOX_XML))
	dialog.gui.findChild(name="message").max_width = screen_width() // 2 - 50
	dialog.gui.findChild(name="message").text = text
	dialog.gui.findChild(name="window").title = caption

	dialog.execute()

def yesNo(message="",caption="Message"):
	text = _make_text(message)
	dialog = XMLDialog(StringIO(YESNO_BOX_XML))
	dialog.gui.findChild(name="message").max_width = screen_width() // 2 - 50
	dialog.gui.findChild(name="message").text = text
	dialog.gui.findChild(name="window").title = caption

	return dialog.execute()

def yesNoCancel(message="",caption="Message"):
	text = _make_text(message)
	dialog = XMLDialog(StringIO(YESNOCANCEL_BOX_XML))
	dialog.gui.findChild(name="message").max_width = screen_width() // 2 - 50
	dialog.gui.findChild(name="message").text = text
	dialog.gui.findChild(name="window").title = caption

	return dialog.execute()

def select(message="",options=[],caption="Message"):
	text = _make_text(message)
	dialog = XMLDialog(StringIO(SELECT_BOX_XML))
	dialog.size = screen_width() // 3, (2 * screen_height()) // 3
	dialog.gui.findChild(name="message").max_width = screen_width() // 2 - 50
	dialog.gui.findChild(name="message").text = text
	dialog.gui.findChild(name="window").title = caption
	
	listbox = dialog.gui.findChild(name="selection")
	listbox.items = options
	if dialog.execute():
		return listbox.selected_item
	return None

def trace(f):
	import sys, traceback
	def new_f(*args,**kwargs):
		try:
			return pychan.tools.applyOnlySuitable(f,*args,**kwargs)

		except Exception, e:
			dialog = XMLDialog(StringIO(EXCEPTION_CATCHER_XML))
			
			dialog.gui.findChild(name="message").text = str(e)
			
			tb = traceback.format_exception(sys.exc_type, sys.exc_value, sys.exc_traceback)
			dialog.gui.findChild(name="traceback").text = "".join(tb)
			dialog.min_size = screen_width() // 2, (3 * screen_height()) // 4
			dialog.max_size = screen_width() // 2, (3 * screen_height()) // 4
			result = dialog.execute()
			if result == YES:
				return new_f(*args,**kwargs)
			elif result == NO:
				return
			raise
	return new_f

