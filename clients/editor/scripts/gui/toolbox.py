from pychan import widgets

class Toolbox(widgets.Window):
	def __init__(self, *args, **kwargs):
		super(Toolbox, self).__init__(*args, **kwargs)
		self.title = ""
		self.addChild(widgets.Label(text=u"Toolbox"))
		