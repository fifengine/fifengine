# -*- coding: utf-8 -*-

from common import *
from basictextwidget import BasicTextWidget

class Label(BasicTextWidget):
	"""
	A basic label - displaying a string.

	Also allows text wrapping and onMouse hover callbacks.

	New Attributes
	==============

	 - wrap_text: Boolean: Enable/Disable automatic text wrapping. Disabled by default.
	 Currently to actually see text wrapping you have to explicitly set a max_size with
	 the desired width of the text, as the layout engine is not capable of deriving
	 the maximum width from a parent container.

	"""

	ATTRIBUTES = BasicTextWidget.ATTRIBUTES + [BoolAttr('wrap_text')]

	def __init__(self,wrap_text=False,hexpand=1,**kwargs):
		self.real_widget = fife.Label("")
		self.wrap_text = wrap_text
		super(Label,self).__init__(hexpand=hexpand,**kwargs)

	def resizeToContent(self):
		self.real_widget.setWidth( self.max_size[0] )
		self.real_widget.adjustSize()
		self.height = self.real_widget.getHeight() + self.margins[1]*2
		self.width  = self.real_widget.getWidth()  + self.margins[0]*2
		#print self.width,self.max_size[0]

	def _setTextWrapping(self,wrapping): self.real_widget.setTextWrapping(wrapping)
	def _getTextWrapping(self): self.real_widget.isTextWrapping()
	wrap_text = property(_getTextWrapping,_setTextWrapping)

class ClickLabel(Label):
	"""
	Deprecated - use L{Label} instead.
	"""
	__init__ = tools.this_is_deprecated(Label.__init__,message = "ClickLabel - Use Label instead")
