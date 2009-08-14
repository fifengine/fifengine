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
		
		self._text = u""
		self._tooltipDisplayed = False
	   
	def setText(self, text):
		""" Sets the text to be displayed whenever a tooltip isn't displayed """
		self._text = text
		if not self.isTooltipDisplayed():
			self._label.text = text
		
	def getText(self):
		return self._text
	text = property(getText, setText)

	def showTooltip(self, text):
		""" Shows a text which is visible until hideTooltip is called """
		self._label.text = text
		self._tooltipDisplayed = True
		
	def hideTooltip(self):
		""" Removes the text set by showTooltip. Whatever text previously set by setText is then displayed. """
		self._label.text = self._text
		self._tooltipDisplayed = False
		
	def isTooltipDisplayed(self):
		""" Returns true if tool tip is displayed """
		return self._tooltipDisplayed
