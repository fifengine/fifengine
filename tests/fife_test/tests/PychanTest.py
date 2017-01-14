#!/usr/bin/env python

# -*- coding: utf-8 -*-

# ####################################################################
#  Copyright (C) 2005-2017 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from fife import fife, fifechan
from fife.extensions import pychan
from fife.extensions.pychan.tools import callbackWithArguments as cbwa
from fife.extensions.fife_timer import Timer

import scripts.test as test


""" 
For use with the styling example 
"""
STYLES= {
'new default': {
	'default' : {
		'border_size': 2,
		'margins': (0,0),
		'base_color' : fifechan.Color(128,128,128),
		'foreground_color' : fifechan.Color(255,255,255),
		'background_color' : fifechan.Color(55,55,55),
		'font' : 'samanata_small'
	},
	'Button' : {
		'border_size': 2,
		'margins' : (20,5),
		'min_size' : (100,20),
		'font' : 'samanata_small'
	},
	'CheckBox' : {
		'border_size': 0,
		'background_color' : fifechan.Color(0,0,0,0),
	},
	'RadioButton' : {
		'border_size': 0,
		'background_color' : fifechan.Color(0,0,0,0),
	},
	'Label' : {
		'border_size': 0,
		'font' : 'samanata_small'
	},
	'ListBox' : {
		'border_size': 0,
		'font' : 'samanata_small'
	},
	'Window' : {
		'border_size': 1,
		'margins': (10,10),
		'opaque' : 0,
		'titlebar_height' : 30,
		'background_image' : 'data/backgrounds/background.png',
		'font' : 'samanata_large'
	},
	'TextBox' : {
		'font' : 'samanata_small'
	},
	('Container','HBox','VBox') : {
		'border_size': 0,
		'background_image' : 'data/backgrounds/background.png',
		'opaque' : False
	}
	},
	'greenzone' : { 
		'default' : {
			'base_color': fifechan.Color(80,200,80) ,
			'background_color': fifechan.Color(200,250,200),
			},
		'Window' : {
			'titlebar_height' : 30,
		},
		'ListBox' : {
			'font' : 'samanata_large'
			}
		}
}


""" 
Used with the GUI animation test
"""
import time
import random
ACTION_MOVE = 1
ACTION_RESIZE = 2
ACTION_COLOR = 3
ACTIONS = [ACTION_MOVE, ACTION_RESIZE, ACTION_COLOR]
DEFAULT_DELAY = 10


class PyChanExample(object):
	"""
	Example class.
	"""
	def __init__(self,xmlFile):
		self.xmlFile = xmlFile
		self.widget = None

	def start(self):
		"""
		The Example Protocoll: start
		"""
		# For simplicity the most basic examples should define
		# a okButton and/or a closeButton. Those are mapped
		# to the stop handler.
		self.widget = pychan.loadXML(self.xmlFile)
		eventMap = {
			'closeButton':self.stop,
			'okButton'   :self.stop
		}
		# Since the basic example are not required to
		# supply close and ok button, we 'ignoreMissing'
		self.widget.mapEvents(eventMap, ignoreMissing = True)
		self.widget.show()

		#from pprint import pprint
		#pprint(self.widget.getNamedChildren())

	def stop(self):
		"""
		The Example Protocoll: stop
		"""
		if self.widget:
			self.widget.hide()
		self.widget = None
		
class StylingExample(PyChanExample):
	def __init__(self):
		super(StylingExample,self).__init__('data/gui/styling.xml')

		self.styles = ['default'] + STYLES.keys()
		for name,style in STYLES.items():
			pychan.manager.addStyle(name,style)

	def start(self):
		self.styledCredits = pychan.loadXML('data/gui/all_widgets.xml')
		self.styledCredits.distributeInitialData({
			'demoList' : map(lambda x:unicode(x,'utf8'),dir(pychan)),
			'demoText' : unicode(pychan.__doc__,'utf8')
		})

		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'testStyle' : self.testStyle,
			'closeButton':self.stop,
		})
		self.widget.distributeInitialData({
			'styleList' : self.styles
		})
		self.widget.position_technique = 'right-20:center'
		self.styledCredits.position_technique = 'left+20:center'
		self.widget.show()
		self.styledCredits.show()

	def stop(self):
		super(StylingExample,self).stop()
		if self.styledCredits:
			self.styledCredits.hide()

	def testStyle(self):
		style = self.styles[self.widget.collectData('styleList')]
		if self.styledCredits:
			self.styledCredits.hide()
		self.styledCredits.stylize(style)
		self.styledCredits.show()

class ShowHideTest(PyChanExample):
	def __init__(self):
		super(ShowHideTest,self).__init__('data/gui/showhide.xml')

	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)

		self.text1widget = self.widget.findChild(name="text1")
		self.text2widget = self.widget.findChild(name="text2")
		self.text3widget = self.widget.findChild(name="text3")
		self.vbox1widget = self.widget.findChild(name="vbox1")


		eventMap = {
			'closeButton':self.stop,
			'hideText1': self.text1widget.hide,
			'showText1': self.text1widget.show,
			'hideText2': self.text2widget.hide,
			'showText2': self.text2widget.show,
			'hideText3': self.text3widget.hide,
			'showText3': self.text3widget.show,
			'hideVBox1': self.vbox1widget.hide,
			'showVBox1': self.vbox1widget.show,
		}

		self.widget.mapEvents(eventMap)
		
		self.widget.show()
		
class ModalTest(PyChanExample):
		def __init__(self):
			super(ModalTest,self).__init__('data/gui/showhide.xml')
		
		def start(self):
			self.widget = pychan.loadXML(self.xmlFile)
			
			self.text1widget = self.widget.findChild(name="text1")
			self.text2widget = self.widget.findChild(name="text2")
			self.text3widget = self.widget.findChild(name="text3")
			self.vbox1widget = self.widget.findChild(name="vbox1")
			
			
			eventMap = {
				'closeButton':self.stop,
				'hideText1': self.requestModal,
				'showText1': self.releaseModal,
				'hideText2': self.requestMouseModal,
				'showText2': self.releaseMouseModal,
			}
			
			self.widget.mapEvents(eventMap)
			
			self.widget.show()
			
		def requestModal(self):
			self.widget.real_widget.requestModalFocus()
		
		def releaseModal(self):
			self.widget.real_widget.releaseModalFocus()
			
		def requestMouseModal(self):
			self.widget.real_widget.requestModalMouseInputFocus()
			
		def releaseMouseModal(self):
			self.widget.real_widget.releaseModalMouseInputFocus()

class DynamicExample(PyChanExample):
	def __init__(self):
		super(DynamicExample,self).__init__('data/gui/dynamic.xml')
		
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'okButton'   :self.stop,
			'addButton'  :self.addLabel,
		})
		self.labelBox = self.widget.findChild(name="labelBox")
		self.widget.show()

	def addLabel(self):
		# Code to check text wrapping code in FIFE
		label = pychan.widgets.Label(max_size=(200,1000),wrap_text=True)
		label.text = self.widget.collectData('labelText')
		label.capture(self.removeLabel)
		self.labelBox.addChild( label )
		self.widget.adaptLayout()
		
	def removeLabel(self,widget=None):
		widget.parent.removeChild(widget)
		self.widget.adaptLayout()


class SliderExample(PyChanExample):
	def __init__(self):
		super(SliderExample,self).__init__('data/gui/slider.xml')
	def start(self):
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'xslider': self.update,
			'yslider': self.update,
			'pbarslider' : self.update,
			'closeButton':self.stop,
		})
		self.update()
		self.widget.show()
	def update(self):
		"""
		Update Icon position from the sliders.
		"""
		icon = self.widget.findChild(name="icon")
		# sliders have floats, fifechan is picky and wants ints
		# so we convert here.
		icon.position = map(int, self.widget.collectData('xslider','yslider'))
		# we distribute to the labels with the x,y value.
		# That's user visible 'text' - so pychan wants unicode.
		self.widget.distributeInitialData({
			'xvalue' : unicode(icon.x),
			'yvalue' : unicode(icon.y),
		})
		
		#quick demo to show the percentage bar in action
		pbarslider = self.widget.findChild(name="pbarslider")
		pbar = self.widget.findChild(name="pbar")
		
		pbar.value = int(pbarslider.value)

class ColorExample(PyChanExample):
	""" a small app (^^) to show how fifechan uses colors on various widgets 
	
	"""
	def __init__(self):
		super(ColorExample,self).__init__('data/gui/colortester.xml')
		
	def start(self):
		""" 
		load XML file and setup callbacks
		"""
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'base_rslider': self.update_basecolor,
			'base_gslider': self.update_basecolor,
			'base_bslider': self.update_basecolor,
			'base_aslider': self.update_basecolor,

			'background_rslider': self.update_background_color,
			'background_gslider': self.update_background_color,
			'background_bslider': self.update_background_color,
			'background_aslider': self.update_background_color,
			
			'foreground_rslider': self.update_foreground_color,
			'foreground_gslider': self.update_foreground_color,
			'foreground_bslider': self.update_foreground_color,
			'foreground_aslider': self.update_foreground_color,
			
			'selection_rslider': self.update_selection_color,
			'selection_gslider': self.update_selection_color,
			'selection_bslider': self.update_selection_color,
			'selection_aslider': self.update_selection_color,						

			'closeButton':self.stop,
		})
		# alpha value needs to be set, otherwise you don't see colors ;-)
		self.widget.findChild(name="base_aslider").value = float(255)
		self.widget.findChild(name="background_aslider").value = float(255)
		self.widget.findChild(name="foreground_aslider").value = float(255)
		self.widget.findChild(name="selection_aslider").value = float(255)
		
		# init stuff
		self.update_basecolor()
		self.update_background_color()
		self.update_foreground_color()
		self.update_selection_color()
		self.widget.show()

	def update_basecolor(self):
		"""
		Update rgba base colors of all examples and show the values
		"""
		r = int(self.widget.findChild(name="base_rslider").value)
		g = int(self.widget.findChild(name="base_gslider").value)
		b = int(self.widget.findChild(name="base_bslider").value)
		a = int(self.widget.findChild(name="base_aslider").value)

		# update slider labels
		self.widget.findChild(name="base_rvalue").text = unicode(str(r), "utf-8")
		self.widget.findChild(name="base_gvalue").text = unicode(str(g), "utf-8")
		self.widget.findChild(name="base_bvalue").text = unicode(str(b), "utf-8")
		self.widget.findChild(name="base_avalue").text = unicode(str(a), "utf-8")
		
		rgba = (r, g, b, a)
		
		self.widget.findChild(name="example1").base_color = rgba
		self.widget.findChild(name="example2").base_color = rgba
		self.widget.findChild(name="example3").base_color = rgba
		self.widget.findChild(name="example4").base_color = rgba
		self.widget.findChild(name="example5").base_color = rgba
		self.widget.findChild(name="example6").base_color = rgba
		self.widget.findChild(name="example7").base_color = rgba
		self.widget.findChild(name="example8").base_color = rgba
		self.widget.findChild(name="example9").base_color = rgba
		
	def update_background_color(self):
		"""
		Update rgba background colors of all examples and show the values
		"""
		r = int(self.widget.findChild(name="background_rslider").value)
		g = int(self.widget.findChild(name="background_gslider").value)
		b = int(self.widget.findChild(name="background_bslider").value)
		a = int(self.widget.findChild(name="background_aslider").value)

		# update slider labels
		self.widget.findChild(name="background_rvalue").text = unicode(str(r), "utf-8")
		self.widget.findChild(name="background_gvalue").text = unicode(str(g), "utf-8")
		self.widget.findChild(name="background_bvalue").text = unicode(str(b), "utf-8")
		self.widget.findChild(name="background_avalue").text = unicode(str(a), "utf-8")
		
		rgba = (r, g, b, a)
		
		self.widget.findChild(name="example1").background_color = rgba
		self.widget.findChild(name="example2").background_color = rgba
		self.widget.findChild(name="example3").background_color = rgba
		self.widget.findChild(name="example4").background_color = rgba
		self.widget.findChild(name="example5").background_color = rgba
		self.widget.findChild(name="example6").background_color = rgba
		self.widget.findChild(name="example7").background_color = rgba
		self.widget.findChild(name="example8").background_color = rgba
		self.widget.findChild(name="example9").background_color = rgba
		
	def update_selection_color(self):
		"""
		Update rgba selection colors of all examples and show the values
		"""
		r = int(self.widget.findChild(name="selection_rslider").value)
		g = int(self.widget.findChild(name="selection_gslider").value)
		b = int(self.widget.findChild(name="selection_bslider").value)
		a = int(self.widget.findChild(name="selection_aslider").value)

		# update slider labels
		self.widget.findChild(name="selection_rvalue").text = unicode(str(r), "utf-8")
		self.widget.findChild(name="selection_gvalue").text = unicode(str(g), "utf-8")
		self.widget.findChild(name="selection_bvalue").text = unicode(str(b), "utf-8")
		self.widget.findChild(name="selection_avalue").text = unicode(str(a), "utf-8")
		
		rgba = (r, g, b, a)
		
		self.widget.findChild(name="example1").selection_color = rgba
		self.widget.findChild(name="example2").selection_color = rgba
		self.widget.findChild(name="example3").selection_color = rgba
		self.widget.findChild(name="example4").selection_color = rgba
		self.widget.findChild(name="example5").selection_color = rgba
		self.widget.findChild(name="example6").selection_color = rgba
		self.widget.findChild(name="example7").selection_color = rgba
		self.widget.findChild(name="example8").selection_color = rgba
		self.widget.findChild(name="example9").selection_color = rgba
		
	def update_foreground_color(self):
		"""
		Update rgba foreground colors of all examples and show the values
		"""
		r = int(self.widget.findChild(name="foreground_rslider").value)
		g = int(self.widget.findChild(name="foreground_gslider").value)
		b = int(self.widget.findChild(name="foreground_bslider").value)
		a = int(self.widget.findChild(name="foreground_aslider").value)

		# update slider labels
		self.widget.findChild(name="foreground_rvalue").text = unicode(str(r), "utf-8")
		self.widget.findChild(name="foreground_gvalue").text = unicode(str(g), "utf-8")
		self.widget.findChild(name="foreground_bvalue").text = unicode(str(b), "utf-8")
		self.widget.findChild(name="foreground_avalue").text = unicode(str(a), "utf-8")
		
		rgba = (r, g, b, a)
		
		self.widget.findChild(name="example1").foreground_color = rgba
		self.widget.findChild(name="example2").foreground_color = rgba
		self.widget.findChild(name="example3").foreground_color = rgba
		self.widget.findChild(name="example4").foreground_color = rgba
		self.widget.findChild(name="example5").foreground_color = rgba
		self.widget.findChild(name="example6").foreground_color = rgba
		self.widget.findChild(name="example7").foreground_color = rgba
		self.widget.findChild(name="example8").foreground_color = rgba
		self.widget.findChild(name="example9").foreground_color = rgba

class PocAnimations(PyChanExample):
	""" a small app (^^) to show how gui animations ~could~ be
		made by using B{fife.TimeEvent}s	
	"""
	def __init__(self):
		super(PocAnimations,self).__init__('data/gui/poc_guianimation.xml')
		
		self._move_timer = None
		self._resize_timer = None
		self._color_timer = None
		self._progress_timer = None
		
	def start(self):
		""" 
		load XML file and setup callbacks
		"""
		
		self.widget = pychan.loadXML(self.xmlFile)
		
		self.widget.mapEvents({
			'closeButton'	:	self.stop,
			
			'example_move'	:	cbwa(self._start_anim, type=ACTION_MOVE),
			'example_color'	:	cbwa(self._start_anim, type=ACTION_COLOR),
			'example_resize':	cbwa(self._start_anim, type=ACTION_RESIZE),
			'example_all'	: 	self._anim_all,			
			'delay'			:	self._set_delay_display,
		})
		
		self.move_example_widget = self.widget.findChild(name="move")
		self.mew = self.move_example_widget

		self.resize_example_widget = self.widget.findChild(name="resize")
		self.rew = self.resize_example_widget		

		self.color_example_widget = self.widget.findChild(name="color")
		self.cew = self.color_example_widget
		
		self.delay_slider = self.widget.findChild(name="delay")
		self.delay_slider.value = float(DEFAULT_DELAY)
		
		self.delay_display = self.widget.findChild(name="delay_label")
		self.delay_display.text = unicode(str(DEFAULT_DELAY))
		
		self.progressbar = self.widget.findChild(name="progressbar")
#		self.progressbar2 = self.widget.findChild(name="progressbar2")
		
		self.little_matrix = []
		for x in range(1,4):
			for y in range(1,4):
				name = "color_%s_%s" % (x, y)
				widget = self.widget.findChild(name=name)
				self.little_matrix.append(widget)
		
		self.widget.adaptLayout(True)
		self.widget.show()
		
	def _set_delay_display(self):
		""" set delay display according to slider value """
		value = self.delay_slider.value
		self.delay_display.text = unicode(str(int(value)))		
		
	def _anim_all(self):
		""" fire all animations """
		for action in ACTIONS:
			self._start_anim(type=action)
		
	def _start_anim(self, type=None):
		""" start the animation of the given type """
		self._reset_anim(type)
		kwargs = {
			'delay' : int(self.delay_slider.value),
			'callback' : None,
			'repeat' : 0,
		}
		if type == ACTION_MOVE:
			kwargs['callback'] = self._move
			self._move_timer = Timer(**kwargs)
			self._move_timer.start()
		elif type == ACTION_RESIZE:
			kwargs['callback'] = self._resize
			self._resize_timer = Timer(**kwargs)
			self._resize_timer.start()
		elif type == ACTION_COLOR:
			kwargs['callback'] = self._color
			self._color_timer = Timer(**kwargs)
			self._color_timer.start()
		
		# progressbar
		kwargs['callback'] = self._update_progress
		self.progressbar.value = 0
#		self.progressbar2.value = 0
		self._progress_timer = Timer(**kwargs)
		self._progress_timer.start()
			
	def _reset_anim(self, type=None):
		""" undo changes made by the animation (but leave alone disco matrix ^^) """
		if type == ACTION_MOVE:
			if self._move_timer:
				self._move_timer.stop()
			self.mew.position = 0, 0
		elif type == ACTION_RESIZE:
			if self._resize_timer:
				self._resize_timer.stop()
			SIZE = 100, 100
			self.rew.size = SIZE
			self.rew.min_size = SIZE
			self.rew.max_size = SIZE
			self.widget.findChild(name="resize_wrapper").adaptLayout()
		elif type == ACTION_COLOR:
			if self._color_timer:
				self._color_timer.stop()
			COLOR = 255, 255, 255, 100
			self.cew.base_color = COLOR
		
		if self._progress_timer:
			self._progress_timer.stop()
			self.progressbar.value = 0
#			self.progressbar2.value = 0
							
	def _move(self):
		""" move the mew widget """
		position = list(self.mew.position)
		if position[0] < 100:
			position[0] += 1
			self.mew.position = position
		else:
			self._reset_anim(ACTION_MOVE)	
		
	def _resize(self):
		""" resize the rew widget """
		size = list(self.rew.size)
		if size[0] > 0:
			size[0] -= 1
			size[1] -= 1
			self.rew.size = size
			self.rew.min_size = size
			self.rew.max_size = size
		else:
			self._reset_anim(ACTION_RESIZE)
			
	def _color(self):
		""" tint the cew widgets """
		color = self.cew.base_color
		red = color.r
		if red > 1:
			red -= 1
			self.cew.base_color = (red, 255, 255, 100)

			# disco!
			for widget in self.little_matrix:
				color = tuple(random.randint(1,255) for i in range(1,5))
				widget.background_color = color
		else:
			self._reset_anim(ACTION_COLOR)
			
	def _update_progress(self):
		""" """
		if self.progressbar.value < self.progressbar.size[0]:
			size = self.progressbar.size
			value = int(self.progressbar.value) + random.randint(0, DEFAULT_DELAY)
			self.progressbar.value = value
#			self.progressbar2.value = value
		else:
			self._reset_anim()

class PychanTest(test.Test):
	""" The base calss for all tests.  All tests must override these functions! """

	def create(self, engine, application):
		self._application = application
		self._engine = engine
		self._running = False

		# Build the main GUI
		self.gui = pychan.loadXML('data/gui/demoapp.xml')
		self.gui.min_size = self._engine.getRenderBackend().getScreenWidth(),self._engine.getRenderBackend().getScreenHeight()

		eventMap = {
			'creditsLink'  : self.showCredits,
			'closeButton'  : self._application.testmanager.stopTest,
			'demoList'     : self.selectExample,
		}
		self.gui.mapEvents(eventMap)
		
		# A simple hover-effect for the credits label
		credits = self.gui.findChild(name="creditsLink")
		# Note that we can't simply write:
		# credits.capture(credits._setText(u"CREDITS"), event_name="mouseEntered")
		# that's because that would call credits._setText NOW and we want to call
		# it later.
		credits.capture(lambda : credits._setText(u"CREDITS"), event_name="mouseEntered")
		# Note that we can't simply write:
		# credits.capture(credits._setText(u"Credits"), event_name="mouseExited")
		# that's because that would call credits._setText NOW and we want to call
		# it later.
		credits.capture(lambda : credits._setText(u"Credits"), event_name="mouseExited")
		
		self._examples = {
			'Absolute Positioning' : PyChanExample('data/gui/absolute.xml'),
			'All Widgets' : PyChanExample('data/gui/all_widgets.xml'),
			'Basic Styling' : StylingExample(),
			'Dynamic Widgets' : DynamicExample(),
			'Sliders' : SliderExample(),
			'ScrollArea' : PyChanExample('data/gui/scrollarea.xml'),
			'Colortester': ColorExample(),
			'GuiAnimations' : PocAnimations(),
			'Show Hide Test' : ShowHideTest(),
			'Modal Test' : ModalTest(),
		}
		self.demoList = self.gui.findChild(name='demoList')
		self.demoList.items = sorted(self._examples.keys())
		
		self.currentExample = None

	def selectExample(self):
		"""
		Callback handler for clicking on the example list.
		"""
		if self.demoList.selected_item is None:
			return
		#print "selected",self.demoList.selected_item
		if self.currentExample:
			self.currentExample.stop()
		self.currentExample = self._examples[self.demoList.selected_item]
		self.gui.findChild(name="xmlSource").text = unicode(open(self.currentExample.xmlFile).read(), 'utf8')
		self.currentExample.start()

	def showCredits(self):
		"""
		Callback handler from the credits link/label.
		"""
		# We use PyChan's synchronous execution feature here.
		pychan.loadXML('data/gui/credits.xml').execute({ 'okButton' : "Yay!" })
		
	def destroy(self):
		if self.currentExample:
			self.currentExample.stop()
			
		del self.gui
		
	def run(self):
		self._running = True
		
		self.gui.show()

	def stop(self):
		self._running = False
		
		self.gui.hide()
		
	def isRunning(self):
		return self._running

	def getName(self):
		return "PychanTest"
		
	def getAuthor(self):
		return "prock"

	def getDescription(self):
		return "Use this to test all pychan widgets."

	def getHelp(self):
		return open( 'data/help/PychanTest.txt', 'r' ).read()

