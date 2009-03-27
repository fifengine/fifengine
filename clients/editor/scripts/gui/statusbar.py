from pychan import widgets

class StatusBar(widgets.HBox):
	def __init__(self, *args, **kwargs):
		super(StatusBar, self).__init__(*args, **kwargs)
		self.addChild(widgets.Label(text=u"StatusBar"))
	