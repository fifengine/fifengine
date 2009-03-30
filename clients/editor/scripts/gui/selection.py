import pychan
import pychan.widgets as widgets

class SelectionDialog(object):
	"""
	Selection displays a list of options for the user to select from. The result is passed to onSelection.
	list - the list to select from
	onSelection - the function to call when a selection is made. Accepts one argument: an element of the list.
	"""
	def __init__(self, list, onSelection):
		self.list = list
		self._callback = onSelection

		self._widget = pychan.loadXML('gui/selection.xml')

		self._widget.mapEvents({
			'okButton'     : self._selected,
			'cancelButton' : self._widget.hide
		})

		self._widget.distributeInitialData({
			'optionDrop' : list
		})
		self._widget.show()

	def _selected(self):
		selection = self._widget.collectData('optionDrop')
		if selection < 0: return
		self._callback(self.list[selection])
		self._widget.hide()

class ClickSelection():
	"""
	ClickSelection displays a list of options for the user to select from. The result is passed to onSelection.
	Differs from Selection: the selection is made when a list element is clicked, rather than when the box is closed.	
	list - the list to select from
	onSelection - the function to call when a selection is made. Accepts one argument: an element of the list.
	"""
	def __init__(self, list, onSelection):
		self.list = list
		self._callback = onSelection

		self._widget = pychan.loadXML('gui/selection.xml')

		self._widget.mapEvents({
			'okButton'     : self._widget.hide,
			'cancelButton' : self._widget.hide,
			'optionDrop'   : self._selected
		})

		self._widget.distributeInitialData({
			'optionDrop' : list
		})
		self._widget.show()

	def _selected(self):
		selection = self._widget.collectData('optionDrop')
		if selection < 0: return
		self._callback(self.list[selection])
