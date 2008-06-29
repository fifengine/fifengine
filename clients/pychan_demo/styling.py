#!/usr/bin/env python
# coding: utf-8

import sys, os, re

def _jp(path):
	return os.path.sep.join(path.split('/'))

_paths = ('../../engine/swigwrappers/python', '../../engine/extensions')
for p in _paths:
	if p not in sys.path:
		sys.path.append(_jp(p))

import fife
import fifelog
import settings

import pychan

from pychan_test import PyChanExample

STYLES= {
'new default': {
	'default' : {
		'border_size': 2,
		'margins': (0,0),
		'base_color' : fife.Color(128,128,128),
		'foreground_color' : fife.Color(255,255,255),
		'background_color' : fife.Color(55,55,55),
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
		'background_color' : fife.Color(0,0,0,0),
	},
	'RadioButton' : {
		'border_size': 0,
		'background_color' : fife.Color(0,0,0,0),
	},
	'Label' : {
		'border_size': 0,
		'font' : 'samanata_small'
	},
	'ClickLabel' : {
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
		'background_image' : 'content/gui/background.png',
		'font' : 'samanata_large'
	},
	'TextBox' : {
		'font' : 'samanata_small'
	},
	('Container','HBox','VBox') : {
		'border_size': 0,
		'background_image' : 'content/gui/background.png',
		'opaque' : False
	}
	},
	'greenzone' : { 
		'default' : {
			'base_color': fife.Color(80,200,80) ,
			'background_color': fife.Color(200,250,200),
			},
		'ListBox' : {
			'font' : 'samanata_large'
			}
		}
}

class StylingExample(PyChanExample):
	def __init__(self):
		super(StylingExample,self).__init__('content/gui/styling.xml')

		self.styles = ['default'] + STYLES.keys()
		for name,style in STYLES.items():
			pychan.manager.addStyle(name,style)

		pychan.loadFonts("content/fonts/samanata.fontdef")

	def start(self):
		self.styledCredits = None
		self.widget = pychan.loadXML(self.xmlFile)
		self.widget.mapEvents({
			'testStyle' : self.testStyle,
			'closeButton':self.stop,
		})
		self.widget.distributeInitialData({
			'styleList' : self.styles
		})
		self.widget.show()

	def stop(self):
		super(StylingExample,self).stop()
		if self.styledCredits:
			self.styledCredits.hide()

	def testStyle(self):
		style = self.styles[self.widget.collectData('styleList')]
		if self.styledCredits:
			self.styledCredits.hide()
		self.styledCredits = pychan.loadXML('content/gui/all_widgets.xml')
		self.styledCredits.distributeInitialData({
			'demoList' : dir(pychan),
			'demoText' : pychan.__doc__
		})
		self.styledCredits.stylize(style)
		self.styledCredits.mapEvents({'okButton':self.styledCredits.hide})
		self.styledCredits.show()
