# -*- coding: utf-8 -*-

from common import *
from widget import Widget

class Icon(Widget):
	"""
	An image icon.

	New Attributes
	==============

	  - image: String or GuiImage: The source location of the Image or a direct GuiImage
	"""
	ATTRIBUTES = Widget.ATTRIBUTES + [Attr('image')]

	def __init__(self,image="",**kwargs):
		self.real_widget = fife.Icon(None)
		super(Icon,self).__init__(**kwargs)
		self._source = self._image = None
		if image:
			self.image = image

	def _setImage(self,source):
		if isinstance(source,str):
			self._source = source
			self._image = get_manager().loadImage(source)
		elif isinstance(source,fife.GuiImage):
			self._source = None
			self._image = source
		else:
			raise RuntimeError("Icon.image only accepts GuiImage and python strings, not '%s'" % repr(source))
		self.real_widget.setImage( self._image )

		# Set minimum size accoriding to image
		self.min_size = self.real_widget.getWidth(),self.real_widget.getHeight()
		self.size  = self.max_size = self.min_size

	def _getImage(self):
		if self._source is not None:
			return self._source
		return self._image
	image = property(_getImage,_setImage)
