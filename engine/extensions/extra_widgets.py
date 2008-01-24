# Guichan extensions. These utilities abstract away some of the bookeeping needed
# for raw Guichan for a more 'pythonic' interface. They also collect some common
# standard gui functionality that Guichan doesn't provide.

import os
import fife

class GenericListmodel(fife.ListModel,list):
	def __init__(self,*args):
		super(GenericListmodel,self).__init__()
		map(self.append,args)
		
	def getNumberOfElements(self):
		return len(self)
		
	def getElementAt(self, i):
		i = max(0,min(i,len(self) - 1))
		return str(self[i])

# Basic container class for widgets. Most widgets inherit from this.
class Container:
	def __init__(self, guimanager):
		self.guimanager = guimanager
		self.widgets = []

	def togglehide(self):
		for child in self.widgets:
			child.setVisible(not child.isVisible())

	def add_widget(self, w):
		self.widgets.append(w)
		self.guimanager.add(w)

	def remove_widget(self, w):
		self.guimanager.remove(w)
#TODO: removing final reference (destroying) the widget results in segfault. So for now, old gui
# elements just hang around in the sytem.
#		self.widgets.remove(w)


# A window that manages children (Container) and receives events (fife.IWidgetListener).
class Dialog(fife.IWidgetListener, fife.Window, Container):
	def __init__(self, event_manager, guimanager, parent, caption, position, size):
		self.eventmanager = event_manager
		fife.IWidgetListener.__init__(self)
		self.eventmanager.addWidgetListener(self)

		fife.Window.__init__(self, caption)
		self.setTitleBarHeight(20)
		self.setPosition(position[0], position[1])
		self.setSize(size[0], size[1])
		self.setVisible(True)

		Container.__init__(self, guimanager)
		self.parent = parent
		self.parent.add_widget(self)

	def __del__(self):
		for child in self.widgets:
			self.remove_widget(child)
		self.eventmanager.removeWidgetListener(self)
		print 'deleting'

	def add_widget(self, w):
		self.widgets.append(w)
		self.add(w)

	def remove_widget(self, w):
		self.remove(w)
		self.widgets.remove(w)

	def onWidgetAction(self, evt):
		pass


# Small extension to Dialog, intended for "window-like" widgets.
class Form(Dialog):
	def __init__(self, event_manager, gui_manager, parent, caption, position, size):
		Dialog.__init__(self, event_manager, gui_manager, parent, caption, position, size)

		self.clbutton = fife.Button('Close')
		self.clbutton.setActionEventId('CloseEvt')
		self.clbutton.addActionListener(self.guimanager)
		self.clbutton.adjustSize()
		self.clbutton.setPosition(self.getWidth() - self.clbutton.getWidth() - 7, self.getHeight() - self.clbutton.getHeight() - 25)
		self.add_widget(self.clbutton)

	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'CloseEvt':
			if(evt.getSourceWidget().this == self.clbutton.this):
				self.parent.remove_widget(self)


# Presents a selection to the user (e.g. ok - cancel).
# INCOMPLETE: currently just prints selection to stdout. Needs to return a value somehow.
class ButtonBox(Dialog): 

	# prompt - prompt for button selection
	# optionA/B - labels for the two selection buttons.
	def __init__(self, event_manager, gui_manager, parent, caption, prompt, optionA, optionB):
		self.size = (200, 100)
		self.position = (300,300)

		Dialog.__init__(self, event_manager, gui_manager, parent, caption, self.position, self.size)

		label = fife.Label(prompt + ':')
		label.setPosition(5, 5)
		label.adjustSize()
		self.add_widget(label)

		self.buttonA = fife.Button(optionA)
		self.buttonA.setActionEventId('optionA')
		self.buttonA.addActionListener(self.guimanager)
		self.buttonA.setSize(50,25)
		self.buttonA.setPosition(45, 5 + label.getHeight() + 5)
		self.add_widget(self.buttonA)

		self.buttonB = fife.Button(optionB)
		self.buttonB.setActionEventId('optionB')
		self.buttonB.addActionListener(self.guimanager)
		self.buttonB.setSize(50, 25)
		self.buttonB.setPosition(100, self.buttonA.getY())
		self.add_widget(self.buttonB)

	def onWidgetAction(self, evt):
		evtid = evt.getId()
		src = evt.getSourceWidget()
		if src.this == self.buttonA.this or src.this == self.buttonB.this:
			if evtid == 'optionA':
				print 'First option selected'
			elif evtid == 'optionB':
				print 'Second option selected'
			self.parent.remove_widget(self)


# A form for browsing the OS directory structure.
class FileBrowser(Form):

	# path - default path to open the browser with
	def __init__(self, event_manager, gui_manager, parent, path, filefilter = lambda f: True):
		self.size = (400,400)
		self.position = (300,100)

		self.filefilter = filefilter

		Form.__init__(self, event_manager, gui_manager, parent, 'File Browser', self.position, self.size)

		self.path = path

		self.dir_list = GenericListmodel('..')
		self.dir_list.extend([dir for dir in os.listdir(self.path) if not os.path.isfile(os.path.join(self.path,dir))])

		dir_label = fife.Label('Directories: ')
		dir_label.setPosition(5, 5)
		dir_label.adjustSize()
		self.add_widget(dir_label)

		self.dir_box = fife.ListBox(self.dir_list)
		self.dir_box.setSelected(0)
		self.dir_box.setPosition(5,5 + dir_label.getHeight())
		self.dir_box.setSize(150,400)
		self.dir_box.setActionEventId('ChangeDirectory')
		self.dir_box.addActionListener(self.guimanager)
		self.add_widget(self.dir_box)

		self.file_list = GenericListmodel()
		self.file_list.extend([file for file in os.listdir(self.path) if os.path.isfile(os.path.join(self.path,file))])

		file_label = fife.Label('Files: ')
		file_label.setPosition(5 + self.dir_box.getWidth() + 5, 5)
		file_label.adjustSize()
		self.add_widget(file_label)

		self.file_box = fife.ListBox(self.file_list)
		self.file_box.setSelected(0)
		self.file_box.setPosition(5 + self.dir_box.getWidth() + 5,5 + file_label.getHeight())
		self.file_box.setSize(150,400)
		self.file_box.setActionEventId('SelectFile')
		self.file_box.addActionListener(self.guimanager)
		self.add_widget(self.file_box)

		self.button = fife.Button('Load')
		self.button.setActionEventId('LoadSelectedMap')
		self.button.addActionListener(self.guimanager)
		self.button.adjustSize()
		self.button.setPosition(5 + self.dir_box.getWidth() + 5 + self.file_box.getWidth() + 5, self.file_box.getY())
		self.add_widget(self.button)

	def onWidgetAction(self, evt):
		evtid = evt.getId()
		if evtid == 'ChangeDirectory' and (evt.getSourceWidget().this == self.dir_box.this):
			if(self.dir_list[self.dir_box.getSelected()] == '..'):
				lst = self.path.split('/')
				if(len(lst) > 2):
					lst.pop()
					self.path = '/'.join(lst)	
				else:
					self.path = '/'
			else:
				lst = self.path.split('/')
				lst.append(self.dir_list[self.dir_box.getSelected()])
				self.path = '/'.join(lst)
				
			self.dir_list = GenericListmodel('..')
			self.dir_list.extend([dir for dir in os.listdir(self.path) if not os.path.isfile(os.path.join(self.path,dir))])
			self.dir_box.setListModel(self.dir_list)
			self.dir_box.setSelected(0)

			self.file_list = GenericListmodel()
			self.file_list.extend([file for file in os.listdir(self.path) if os.path.isfile(os.path.join(self.path,file)) and self.filefilter(file)])

			self.file_box.setListModel(self.file_list)
			self.file_box.setSelected(0)
		elif evtid == 'SelectFile' and (evt.getSourceWidget().this == self.file_box.this):
			print 'Selected file: ' + self.file_list[self.file_box.getSelected()]
		elif evtid == 'LoadSelectedMap' and (evt.getSourceWidget().this == self.button.this):
			print 'Loading file ' + self.file_list[self.file_box.getSelected()]
		else:
			Form.onWidgetAction(self, evt)

