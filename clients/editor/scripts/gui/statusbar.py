from pychan import widgets

class StatusBar(widgets.HBox):
	def __init__(self, text=u"", panel_size=25, *args, **kwargs):
		super(StatusBar, self).__init__(*args, **kwargs)
		self.min_size = (panel_size, panel_size)
		
		self._tooltip = None
		self._label = widgets.Label(text=text)
		self.addChild(self._label)
	   
	def setText(self, text):
		self._label.text = text
		
	def getText(self):
		return self._label.text
	text = property(getText, setText)

	# Shows a text which is visible until hideTooltip is called
	def showTooltip(self, text):
		if text == u"":
			self.hideTooltip()
		if self._tooltip is not None:
			self._tooltip.text = text
		else:
			self.removeChild(self._label)
			self._tooltip = widgets.Label(text=text)
			self.addChild(self._tooltip)
		self.adaptLayout()

	# Removes the text set by showTooltip. Whatever text previously set by setText is then displayed.
	def hideTooltip(self):
		if self._tooltip is not None:
			self.removeChild(self._tooltip)
			self.addChild(self._label)
			self._tooltip = None
			self.adaptLayout()
