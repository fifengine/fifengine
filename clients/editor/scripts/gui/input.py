import pychan
import pychan.widgets as widgets

class InputDialog(object):
	"""
	Input supplies a text box for entering data. The result is passed to onEntry.
	onEntry - the function to call when a input is complete. Accepts one argument: a string of text.
	"""
	def __init__(self, prompt, onEntry):
		self._callback = onEntry

		self._widget = pychan.loadXML('gui/input.xml')

		self._widget.mapEvents({
			'okButton'     : self._complete,
			'cancelButton' : self._widget.hide
		})

		self._widget.distributeInitialData({
			'prompt' : prompt
		})
		self._widget.show()

	def _complete(self):
		self._callback(self._widget.collectData('inputBox'))
		self._widget.hide()

