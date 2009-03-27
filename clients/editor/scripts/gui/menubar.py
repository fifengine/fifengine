from pychan import widgets

class MenuBar(widgets.HBox):
	def __init__(self, *args, **kwargs):
		super(MenuBar, self).__init__(*args, **kwargs)
		self.addChild(widgets.Label(text=u"MenuBar"))
	