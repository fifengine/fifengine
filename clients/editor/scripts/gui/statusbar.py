from pychan import widgets

class StatusBar(widgets.HBox):
	"""
	A basic widget which displays information of the current status of the program.
	
	Use the text property to set the text to be displayed. Use showTooltip() to display
	a temporary message.
	"""
	def __init__(self, text=u"", panel_size=25, *args, **kwargs):
		super(StatusBar, self).__init__(*args, **kwargs)
		self.min_size = (panel_size, panel_size)
		
		self._tooltip = None
		self._label = widgets.Label(text=text)
		self.addChild(self._label)
	   
	def setText(self, text):
		""" Sets the text to be displayed whenever a tooltip isn't displayed """
		self._label.text = text
		
	def getText(self):
		return self._label.text
	text = property(getText, setText)

	def showTooltip(self, text):
		""" Shows a text which is visible until hideTooltip is called """
		if text == u"":
			self.hideTooltip()
		if self._tooltip is not None:
			self._tooltip.text = text
		else:
			self.removeChild(self._label)
			self._tooltip = widgets.Label(text=text)
			self.addChild(self._tooltip)
		self.adaptLayout()
		
	def hideTooltip(self):
		""" Removes the text set by showTooltip. Whatever text previously set by setText is then displayed. """
		if self._tooltip is not None:
			self.removeChild(self._tooltip)
			self.addChild(self._label)
			self._tooltip = None
			self.adaptLayout()
