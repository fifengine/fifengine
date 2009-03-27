from pychan import widgets

class ToolBar(widgets.HBox):
	def __init__(self, *args, **kwargs):
		super(ToolBar, self).__init__(*args, **kwargs)
		self.addChild(widgets.Label(text=u"Toolbar"))
	