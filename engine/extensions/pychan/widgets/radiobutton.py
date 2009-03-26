# -*- coding: utf-8 -*-

from common import *
from basictextwidget import BasicTextWidget

class RadioButton(BasicTextWidget):
	"""
	A basic radiobutton (an exclusive checkbox).

	New Attributes
	==============

	  - marked: Boolean: Whether the checkbox is checked or not.
	  - group: String: All RadioButtons with the same group name
	  can only be checked exclusively.

	Data
	====
	The marked status can be read and set via L{distributeData} and L{collectData}
	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [BoolAttr('marked'),Attr('group')]

	def __init__(self,group="_no_group_",**kwargs):
		self.real_widget = fife.RadioButton()
		super(RadioButton,self).__init__(**kwargs)

		self.group = group

		# Prepare Data collection framework
		self.accepts_data = True
		self._realGetData = self._isMarked
		self._realSetData = self._setMarked

		# Initial data stuff inherited.

	def _isMarked(self): return self.real_widget.isSelected()
	def _setMarked(self,mark): self.real_widget.setSelected(mark)
	marked = property(_isMarked,_setMarked)

	def _setGroup(self,group): self.real_widget.setGroup(group)
	def _getGroup(self): return self.real_widget.getGroup()
	group = property(_getGroup,_setGroup)

	def resizeToContent(self,recurse=True):
		self.width = self.real_font.getWidth(text2gui(self.text)) + 35# Size of the Checked box?
		self.height = self.real_font.getHeight()
