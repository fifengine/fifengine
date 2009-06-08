import pychan
import pychan.widgets as widgets

class ErrorDialog(object):
	"""
	Shows a dialog with an error message.
	"""
	def __init__(self, message):
		self._widget = pychan.loadXML('gui/error.xml')

		self._widget.mapEvents({
			'okButton'     : self._widget.hide
		})

		self._widget.distributeInitialData({
			'message' : message
		})
		self._widget.show()
		self._widget.adaptLayout() # Necessary to make scrollarea work properly

